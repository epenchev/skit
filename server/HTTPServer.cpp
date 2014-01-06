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
 : TCPConnection(id, inSocket), m_acceptRequest(true), m_channel(NULL), m_buffer(NULL)
{}

HTTPSession::~HTTPSession()
{
    // m_channel is not destroyed TCPConnection will do it for us
    delete m_buffer;
    Disconnect();
}

void HTTPSession::AddHTTPSessionListener(HTTPSessionListener* listener)
{
    if (listener)
    {
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
    if (m_acceptRequest)
    {
        m_acceptRequest = false; // disable accepting requests until this one is replied
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
}

void HTTPSession::NotifyOnHTTPrequest()
{
    SystemMutexLocker lock(m_lockListeners);
    for (std::set<HTTPSessionListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
    {
        (*it)->OnHTTPrequest(*this, &m_request);
    }
}

void HTTPSession::NotifyOnHTTPRequestReply(bool& forceReply)
{
    forceReply = false;
    SystemMutexLocker lock(m_lockListeners);
    for (std::set<HTTPSessionListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
    {
        (*it)->OnHTTPRequestReply(*this, &m_response, forceReply);
         if (forceReply)
         {
             // send reply ignore other listeners
             SendResponse();
             break;
         }
    }
}

void HTTPSession::NotifyOnHTTPResponseSend()
{
    SystemMutexLocker lock(m_lockListeners);
    LOG(logDEBUG) << "Response is send, notify listeners";
    for (std::set<HTTPSessionListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
    {
        (*it)->OnReplySend(this);
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
            	LOG(logINFO) << "HTTP Request received";

                ErrorCode ercode;
                m_request.Init(m_reqheaders, ercode);
                m_reqheaders.clear();
                delete m_buffer;
                m_buffer = NULL;

                if (err)
                {
                    LOG(logERROR) << ercode.GetErrorMessage();
                    return;
                }

                // notify listeners for HTTP request
                NotifyOnHTTPrequest();
                bool replySend = false;
                NotifyOnHTTPRequestReply(replySend);
                if (!replySend)
                {
                	SendResponse();
                }
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
    }
}

void HTTPSession::OnWrite(IOChannel* chan, std::size_t bytesWritten, ErrorCode& err)
{
	// clean up after sending response
	m_acceptRequest = true;
	delete m_buffer;
	m_buffer = NULL;

	if (!err)
    {
        LOG(logDEBUG) << "bytes written " << bytesWritten;
        NotifyOnHTTPResponseSend();
    }
    else
    {
        LOG(logERROR) << err.GetErrorMessage();
        if (m_listeners.empty())
        {
            Disconnect();
        }
        else
        {
            NotifyOnHTTPResponseSend(); // with error
        }
    }

}

void HTTPSession::SendResponse()
{
    std::string response = m_response.Str();
    if (response.empty())
    {
    	LOG(logDEBUG) << "No response headers disconnect";
    	Disconnect();
    }
    else
    {
    	std::size_t bufferSize = response.size() + 1;
    	m_buffer = new Buffer(new char[bufferSize], bufferSize);
    	m_buffer->Clear();
    	memcpy(m_buffer->BufferCast<char*>(), response.c_str(), bufferSize);
    	m_channel->Write(*m_buffer);
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

        HTTPSession* session = new HTTPSession(IDGenerator::Instance().Next(), inNewSocket);
        m_lockSessions.Lock();
        m_sessions.insert(session);
        m_lockSessions.Unlock();

        /* testing only */
        NetConnection* conn = dynamic_cast<NetConnection*>(session);
        if (conn)
        {
        	LOG(logDEBUG) << "Add connection listener";
        	conn->AddListener(this);
        }
        else
        {
        	LOG(logERROR) << "cast failed";
        }
        /* testing only*/

        // don't modify the listeners list while in use
        SystemMutexLocker lock(m_lockListeners);
        for (std::set<HTTPServerListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
        {
            // TODO async here don't block accept, low priority task queue and high priority task queue
            (*it)->OnHTTPSessionCreate(session);
        }
        // Listen for HTTP headers
        session->AcceptRequest();
    }
    else
    {
        LOG(logERROR) << inError.GetErrorMessage();
    }
}

void HTTPServer::Start()
{
    LOG(logINFO) << "Starting web server ...";
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

        // don't modify the listeners list while in use
        SystemMutexLocker lock(m_lockListeners);
        for (std::set<HTTPServerListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
        {
            (*it)->OnServerStart(this);
        }
    }
}

void HTTPServer::Stop()
{
    if (m_started)
    {
        LOG(logINFO) << "Stopping web server ...";
        m_acceptor.Stop();
        m_started = false;

        // don't modify the listeners list while in use
        m_lockListeners.Lock();
        for (std::set<HTTPServerListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
        {
            (*it)->OnServerStop(this);
        }
        m_lockListeners.Unlock();

        // close all connections
        m_lockSessions.Lock();
        for (std::set<HTTPSession*>::iterator it = m_sessions.begin(); it != m_sessions.end(); ++it)
        {
            // skip disconnect event
            (*it)->RemoveListener(this);
            (*it)->Disconnect();
        }
        m_lockSessions.Unlock();
    }
    else
    {
        LOG(logINFO) << "Server already stopped";
    }
}

unsigned int HTTPServer::GetConnectionCount()
{
    SystemMutexLocker lock(m_lockSessions);
    unsigned int sessionCount = m_sessions.size();

    return sessionCount;
}

void HTTPServer::AddServerListener(HTTPServerListener* listener)
{
    if (listener)
    {
        SystemMutexLocker lock(m_lockListeners);
        m_listeners.insert(listener);
    }
}

void HTTPServer::RemoveServerListener(HTTPServerListener* listener)
{
    if (listener)
    {
        SystemMutexLocker lock(m_lockListeners);
        std::set<HTTPServerListener*>::iterator it = m_listeners.find(listener);
        if (it != m_listeners.end())
        {
            m_listeners.erase(it);
        }
    }
}

void HTTPServer::OnConnectionClose(NetConnection* conn)
{
	LOG(logDEBUG) << "Destroying connection";
    if (conn)
    {
        LOG(logINFO) << "Connection is closed";
        HTTPSession* session = dynamic_cast<HTTPSession*>(conn);
        if (session)
        {
            SystemMutexLocker lockerSessions(m_lockSessions);
            std::set<HTTPSession*>::iterator it = m_sessions.find(session);
            if (it != m_sessions.end())
            {
                SystemMutexLocker lock(m_lockListeners);
                for (std::set<HTTPServerListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
                {
                    (*it)->OnHTTPSessionDestroy(session);
                }
            }
            m_sessions.erase(it);
            // TODO
            delete session;
        }
    }
}

