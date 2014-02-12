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
#include "HTTP/HTTPRequest.h"
#include "HTTP/HTTPUtils.h"
#include "utils/ErrorCode.h"
#include "system/Task.h"
#include "system/TaskThread.h"
#include "utils/Logger.h"
#include "utils/IDGenerator.h"
#include <cstdlib>
#include <cstring>

HTTPConnection::HTTPConnection(unsigned id, TCPSocket* inSocket)
 : m_connID(id), m_socket(inSocket), m_buffer(NULL)
{
	m_buffer.resize(m_recvsize);
}

HTTPConnection::~HTTPConnection()
{
    delete m_socket;
}

void HTTPConnection::AddHTTPConnectionListener(HTTPConnectionListener* listener)
{
	if (listener)
	{
		LOG(logDEBUG) << "Add session listener";

		SystemMutexLocker lock(m_lockListeners);
		m_listeners.insert(listener);
	}
}

void HTTPConnection::RemoveHTTPConnectionListener(HTTPConnectionListener* listener)
{
	if (listener)
	{
		SystemMutexLocker lock(m_lockListeners);

		std::set<HTTPConnectionListener*>::iterator it = m_listeners.find(listener);
		if (it != m_listeners.end())
		{
			LOG(logDEBUG) << "Remove session listener";
			m_listeners.erase(it);
		}
		else
		{
			LOG(logWARNING) << "Listener not from this HTTP session";
		}
    }
}

void HTTPConnection::AcceptRequest()
{
	m_buffer.replace(0, m_buffer.size(), 1, 0); // fill
	m_socket->ReceiveSome(m_buffer, this);
}

void HTTPConnection::NotifyOnHTTPRequest()
{
	m_lockListeners.Lock();
	std::set<HTTPConnectionListener*> listeners(m_listeners);
	m_lockListeners.Unlock();

    LOG(logDEBUG) << "Notify listeners for OnHTTPrequest() event";
    std::set<HTTPConnectionListener*>::iterator it = listeners.begin();
    for (; it != listeners.end(); ++it)
    {
    	(*it)->OnHTTPrequest(shared_from_this(), m_request);
    }
}

void HTTPConnection::OnReceive(TCPSocket* inSocket, std::size_t bytesRead, ErrorCode& err)
{
    if (!err)
    {
        if (bytesRead && bytesRead <= m_buffer.size())
        {
            m_buffer.commit(bytesRead);

        	char partHeader[m_recvsize + 1] = { 0 };
            memcpy(partHeader, m_buffer.data(), bytesRead);

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
                if (bytesRead < m_buffer.size())
                {
                    // TODO implement timeout protection in TCPConnection and the remove this
                    LOG(logWARNING) << "Buffer not full, end of request not found, headers may be invalid, disconnecting";

                    // TODO notify listeners on ERROR HTTPRequest
                    m_socket->Close();
                }
                else
                {
                    // collect the rest of the headers
                	m_buffer.replace(0, m_buffer.size(), 1, 0); // fill
                	m_socket->ReceiveSome(m_buffer, this);
                }
            }
            else // found end of request assemble HTTPRequest object
            {
                // LOG(logINFO) << "HTTP Request received";

                ErrorCode ercode;
                m_request.Init(m_reqheaders, ercode);
                m_reqheaders.clear();
                //delete m_buffer;
                //m_buffer = NULL;

                if (err)
                {
                    LOG(logERROR) << ercode.Message();
                    // TODO notify listeners on ERROR HTTPRequest
                    m_socket->Close();
                    return;
                }
                NotifyOnHTTPRequest();
                AcceptRequest();
            }
        }
        else
        {
            LOG(logINFO) << "nothing to read bytesRead == 0 read must disconnect !!! \n";
            // TODO notify listeners on ERROR HTTPRequest
            m_socket->Close();
        }
    }
    else
    {
        LOG(logERROR) << err.Message();
        // TODO notify listeners on ERROR HTTPRequest
        m_socket->Close();
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
        HTTPConnectionPtr connPtr( new HTTPConnection(IDGenerator::Instance().Next(), inNewSocket) );
        NotifyOnConnectionCreate(connPtr);
    }
    else
    {
        LOG(logERROR) << inError.Message();
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
            LOG(logERROR) << "Error starting web server: " << ex.Code().Message();
            return;
        }
        m_started = true;

        // Server started notify listeners
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

        // Server stopped notify listeners
        Task* taskOnStop = new Task();
        taskOnStop->Connect(&HTTPServer::NotifyOnServerStop, this);
        TaskThreadPool::Signal(taskOnStop);
    }
    else
    {
        LOG(logWARNING) << "Server already stopped";
    }
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
    	LOG(logDEBUG) << "Remove server listener";

    	SystemMutexLocker locker(m_lockListeners);
        std::set<HTTPServerListener*>::iterator it = m_listeners.find(listener);
        if (it != m_listeners.end())
        {
            m_listeners.erase(it);
        }
    }
}

void HTTPServer::NotifyOnConnectionCreate(HTTPConnectionPtr conn)
{
    LOG(logDEBUG) << "Notify listeners for OnHTTPConnectionCreate() event";

    // get a copy of listeners so add/remove not block
    m_lockListeners.Lock();
    std::set<HTTPServerListener*> listeners = m_listeners;
    m_lockListeners.Unlock();

    for (std::set<HTTPServerListener*>::iterator it = listeners.begin(); it != listeners.end(); ++it)
    {
        (*it)->OnHTTPConnectionAccept(conn);
    }

    // Start listen for HTTP request
    conn->AcceptRequest();
}

void HTTPServer::NotifyOnServerStart()
{
    LOG(logDEBUG) << "Notify listeners for OnServerStart() event";

    // get a copy of listeners so add/remove not block
    m_lockListeners.Lock();
    std::set<HTTPServerListener*> listeners = m_listeners;
    m_lockListeners.Unlock();

    for (std::set<HTTPServerListener*>::iterator it = listeners.begin(); it != listeners.end(); ++it)
    {
        (*it)->OnServerStart(*this);
    }
}

void HTTPServer::NotifyOnServerStop()
{
    LOG(logDEBUG) << "Notify listeners for OnServerStop() event";

    // get a copy of listeners so add/remove not block
    m_lockListeners.Lock();
    std::set<HTTPServerListener*> listeners = m_listeners;
    m_lockListeners.Unlock();

    for (std::set<HTTPServerListener*>::iterator it = listeners.begin(); it != listeners.end(); ++it)
    {
        (*it)->OnServerStop(*this);
    }

}

