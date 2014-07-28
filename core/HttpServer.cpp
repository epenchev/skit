//
// HttpServer.cpp
// Copyright (C) 2014  Emil Penchev, Bulgaria

#include "HttpServer.h"
#include "Logger.h"

#include <iostream>
#include <cstring>

// register HttpServerHandler to ServerController::ServerHandlerFactory for later use
Skit::ServerController::HandlerFactory::Registrator<HttpServer> reg_http("http");

HttpServer::HttpServer()
{
	std::set<CreateListenerFunc>::iterator it  = HttpServer::GetRegistry().begin();
	for (; it != HttpServer::GetRegistry().end(); it++)
	{
		ReqListener* listener = (*it)();
		m_listeners.insert(listener);
	}
}

void HttpServer::AcceptConnection(TcpSocketPtr socket)
{
	HttpSessionPtr session(new HttpSession(*this, socket));
	m_sessions.insert(session);
    session->AcceptRequest();
}

void HttpServer::NotifyOnHttpRequest(HttpSessionPtr session, Skit::HTTP::Request& request)
{
	std::set<ReqListener*>::iterator iter = m_listeners.begin();
    for (; iter != m_listeners.end(); iter++)
    {
    	(*iter)->OnHttpRequest(session, request);
    }
}

void HttpServer::NotifyOnHttpSessionClose(HttpSessionPtr session)
{
	m_sessions.erase(session);
}

HttpSession::HttpSession(HttpServer& server, TcpSocketPtr socket)
 : m_server(server), m_socket(socket), m_buffer(1024)
{
    m_reqdata.clear();
}

void HttpSession::OnReceive(const SysError& error, std::size_t bytes_transferred)
{
    if (error)
    {
    	LOG(logERROR) << error.message();
    	Disconnect();
    	return;
    }

    if (bytes_transferred && bytes_transferred <= m_buffer.Size())
    {
    	m_reqdata += m_buffer.Get<const char*>();
        if (std::string::npos ==  m_reqdata.find("\r\n\r\n"))
        {
        	if (bytes_transferred < m_buffer.Size())
            {
        		LOG(logWARNING) << "Buffer not full no end marker, disconnecting";
        		Disconnect();
            }
            else
            {
            	AcceptRequest();  // collect the rest of the headers
            }
        }
        else
        {
            m_request.Init(m_reqdata);
            m_server.NotifyOnHttpRequest(this->shared_from_this(), m_request);
            m_reqdata.clear();
        }
    }
    else
    {
    	LOG(logWARNING) << "nothing to read must disconnect";
    	Disconnect();
    }
}

void HttpSession::AcceptRequest()
{
	memset(m_buffer.Get<char*>(), 0, m_buffer.Size());
	m_socket->ReceiveSome(CreateBufferSequence(m_buffer), BIND_HANDLER(&HttpSession::OnReceive));
}

void HttpSession::Disconnect()
{
	m_socket->Close();
	m_server.NotifyOnHttpSessionClose(shared_from_this());
}

