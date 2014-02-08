/*
 * HTTPListener.cpp
 *
 * Copyright (C) 2013  Emil Penchev, Bulgaria
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 *  Created on: Aug 21, 2013
 *      Author: emo
 */

#include "server/HTTPServer.h"
#include "server/TCPConnection.h"
#include "server/IOChannel.h"
#include "HTTP/HTTPRequest.h"
#include "HTTP/HTTPUtils.h"
#include "utils/ErrorCode.h"
#include "system/Task.h"
#include "system/TaskThread.h"
#include "utils/Logger.h"
#include "utils/IDGenerator.h"
#include <cstdlib>
#include <cstring>

HTTPSession::HTTPSession(unsigned id, TCPSocket* inSocket)
 : TCPConnection(id, inSocket), m_channel(NULL), m_buffer(NULL)
{}

HTTPSession::~HTTPSession()
{
    // m_channel is not destroyed TCPConnection will do it for us
	LOG(logDEBUG) << "{}";
    delete m_buffer;
    Disconnect();
}

void HTTPSession::AddHTTPSessionListener(HTTPSessionListener* listener)
{
	if (listener)
	{
		LOG(logDEBUG) << "Add session listener";
		SystemMutexLocker lock(m_lockListeners);
		m_listeners.insert(listener);
	}
	else
	{
		LOG(logERROR) << "Invalid listener";
	}
}

void HTTPSession::RemoveHTTPSessionListener(HTTPSessionListener* listener)
{
	if (listener)
	{
		SystemMutexLocker lock(m_lockListeners);
		LOG(logDEBUG) << "Remove session listener";
		std::set<HTTPSessionListener*>::iterator it = m_listeners.find(listener);
		if (it != m_listeners.end())
		{
			m_listeners.erase(it);
		}
		else
		{
			LOG(logWARNING) << "Listener not from this HTTP session";
		}
    }
	else
	{
		LOG(logERROR) << "Invalid listener";
	}
}

void HTTPSession::AcceptRequest()
{
	if (!m_buffer)
    {
		m_buffer = new Buffer(new char[m_recvsize], m_recvsize);
        m_buffer->Clear();
    }
    else
    {
    	m_buffer->Clear();
    }
    if (!m_channel)
    {
    	m_channel = OpenChannel(this);
    }
    m_channel->Read(*m_buffer, true);
}

void HTTPSession::NotifyOnHTTPRequest()
{
	m_lockListeners.Lock();
	std::set<HTTPSessionListener*> listeners(m_listeners);
	m_lockListeners.Unlock();

    LOG(logDEBUG) << "Notify listeners for OnHTTPrequest() event";
    std::set<HTTPSessionListener*>::iterator it = listeners.begin();
    for (; it != listeners.end(); ++it)
    {
    	(*it)->OnHTTPrequest(shared_from_this(), m_request);
    }
}

void HTTPSession::OnRead(IOChannel* chan, std::size_t bytesRead, ErrorCode& err)
{
    if (!err)
    {
        if (bytesRead && bytesRead <= m_buffer->Size())
        {
            char partHeader[m_recvsize + 1] = { 0 };
            memcpy(partHeader, m_buffer->BufferCast<char*>(), bytesRead);
            if (m_reqheaders.empty())
            {
                m_reqheaders = partHeader;
            }
            else
            {
                m_reqheaders += partHeader;
            }

            // search for end of request
            if (std::string::npos ==  m_reqheaders.find("\r\n\r\n"))
            {
                if (bytesRead < m_buffer->Size())
                {
                    // TODO implement timeout protection in TCPConnection and the remove this
                    LOG(logWARNING) << "Buffer not full, end of request not found, headers may be invalid, disconnecting";
                    Disconnect();
                }
                else
                {
                    // collect the rest of the headers
                    m_channel->Read(*m_buffer, true);
                }
            }
            else // found end of request assemble HTTPRequest object
            {
                // LOG(logINFO) << "HTTP Request received";

                ErrorCode ercode;
                m_request.Init(m_reqheaders, ercode);
                m_reqheaders.clear();
                delete m_buffer;
                m_buffer = NULL;

                if (err)
                {
                    LOG(logERROR) << ercode.GetErrorMessage();
                    this->Disconnect();
                    return;
                }
                NotifyOnHTTPRequest();
                AcceptRequest();
            }
        }
        else
        {
            LOG(logINFO) << "nothing to read bytesRead == 0 read must disconnect !!! \n";
        }
    }
    else
    {
        LOG(logERROR) << err.GetErrorMessage();
        Disconnect();
    }
}

HTTPServer::HTTPServer(unsigned short port)
 : m_started(false), m_acceptor(port)
{}


HTTPServer::HTTPServer(std::string localAdress, unsigned short port)
 :  m_started(false), m_acceptor(localAdress, port)
{}

HTTPServer::~HTTPServer()
{
    Stop();
}

void HTTPServer::OnAccept(TCPSocket* inNewSocket, ErrorCode& inError)
{
    if (!inError && inNewSocket)
    {
        ErrorCode err;
        LOG(logINFO) << "connected from " << inNewSocket->GetRemoteIP(err) << ":"
                     << inNewSocket->GetRemotePort(err);
        HTTPSessionPtr sessionPtr( new HTTPSession(IDGenerator::Instance().Next(), inNewSocket) );
        sessionPtr->AddListener(this);
        NotifyOnSessionCreate(sessionPtr);
    }
    else
    {
        LOG(logERROR) << inError.GetErrorMessage();
    }
}

void HTTPServer::Start()
{
    LOG(logINFO) << "Starting HTTP server ...";
    if (!m_started)
    {
        try
        {
            m_acceptor.SetHandler(this);
            m_acceptor.Listen();
            m_acceptor.Start();
        }
        catch(SystemException& ex)
        {
            LOG(logERROR) << "Error starting web server: " << ex.Code().GetErrorMessage();
            return;
        }
        m_started = true;
        // async notify, don't wait on server start
        Task* taskOnStart = new Task();
        taskOnStart->Connect(&HTTPServer::NotifyOnServerStart, this);
        TaskThreadPool::Signal(taskOnStart);
    }
    else
    {
        LOG(logWARNING) << "Server already started";
    }
}

void HTTPServer::Stop()
{
    LOG(logINFO) << "Stopping HTTP server ...";
    if (m_started)
    {
        m_acceptor.Stop();
        m_started = false;

        // async notify, don't wait on server stop
        Task* taskOnStop = new Task();
        taskOnStop->Connect(&HTTPServer::NotifyOnServerStop, this);
        TaskThreadPool::Signal(taskOnStop);
    }
    else
    {
        LOG(logWARNING) << "Server already stopped";
    }
}

unsigned int HTTPServer::GetConnectionCount()
{
    SystemMutexLocker locker(m_lockSessions);
    unsigned int sessionCount = m_sessions.size();

    return sessionCount;
}

void HTTPServer::AddServerListener(HTTPServerListener* listener)
{
    if (listener)
    {
        SystemMutexLocker locker(m_lockListeners);
        LOG(logDEBUG) << "Add server listener";
        m_listeners.insert(listener);
    }
}

void HTTPServer::RemoveServerListener(HTTPServerListener* listener)
{
    if (listener)
    {
        SystemMutexLocker locker(m_lockListeners);
        LOG(logDEBUG) << "Remove server listener";
        std::set<HTTPServerListener*>::iterator it = m_listeners.find(listener);
        if (it != m_listeners.end())
        {
            m_listeners.erase(it);
        }
    }
}

void HTTPServer::OnConnectionClose(NetConnection& conn)
{
    LOG(logINFO) << "Connection is closed";
    SystemMutexLocker lockerSessions(m_lockSessions);
    std::set<HTTPSessionPtr>::iterator it = m_sessions.begin();
    for (; it != m_sessions.end(); ++it)
    {
    	HTTPSessionPtr sessPtr = *it;
    	if (sessPtr->GetID() == conn.GetID())
    	{
    		m_sessions.erase(it);
    		break;
    	}
    }
}

void HTTPServer::NotifyOnSessionCreate(HTTPSessionPtr session)
{
    LOG(logDEBUG) << "Notify listeners for OnHTTPSessionCreate() event";
    m_lockListeners.Lock();
    for (std::set<HTTPServerListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
    {
        (*it)->OnHTTPSessionAccept(session);
    }
    m_lockListeners.Unlock();

    // prevent OnConnectionClose to delete connection while we are working on it
    SystemMutexLocker locker(m_lockSessions);
    m_sessions.insert(session);

    // Start listen for HTTP request
    session->AcceptRequest();
}

void HTTPServer::NotifyOnServerStart()
{
    LOG(logDEBUG) << "Notify listeners for OnServerStart() event";
    SystemMutexLocker locker(m_lockListeners);
    for (std::set<HTTPServerListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
    {
        (*it)->OnServerStart(*this);
    }
}

void HTTPServer::NotifyOnServerStop()
{
    LOG(logDEBUG) << "Notify listeners for OnServerStop() event";
    SystemMutexLocker lockerListeners(m_lockListeners);
    for (std::set<HTTPServerListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
    {
        (*it)->OnServerStop(*this);
    }

    // close all connections
    SystemMutexLocker lockerSessions(m_lockSessions);
    for (std::set<HTTPSessionPtr>::iterator it = m_sessions.begin(); it != m_sessions.end(); ++it)
    {
    	HTTPSessionPtr sessPtr = *it;
    	sessPtr->RemoveListener(this);
    	sessPtr->Disconnect();
    }
    m_sessions.clear();
}

