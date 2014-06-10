//
// HttpServer.cpp
// Copyright (C) 2014  Emil Penchev, Bulgaria

#include "HttpServer.h"
#include "Logger.h"

#include <boost/thread/locks.hpp>

#include <iostream>
#include <cstring>

static boost::mutex s_listenersMutex;
static std::list<HttpServer::ReqListenerWeakPtr> s_listeners;

// register HttpServerHandler to ServerController::ServerHandlerFactory for later use
Skit::ServerController::HandlerFactory::Registrator<HttpServer> reg_http("http");


void HttpServer::AcceptConnection(TcpSocketPtr socket)
{
	HttpSessionPtr session(new HttpSession(*this, socket));
	m_sessions.insert(session);
    session->AcceptRequest();
}

void HttpServer::AddRequestListener(ReqListenerPtr listener)
{
	boost::unique_lock<boost::mutex> lock(s_listenersMutex);
    s_listeners.push_back(listener);
}

void HttpServer::NotifyOnHttpRequest(HttpSessionPtr session, Skit::HTTP::Request& request)
{
    std::list<ReqListenerWeakPtr>::iterator iter = s_listeners.begin();
    while (iter != s_listeners.end())
    {
        if ((*iter).expired())
        {
        	boost::unique_lock<boost::mutex> lock(s_listenersMutex);
        	iter = s_listeners.erase(iter);
        }
        else
        {
            ReqListenerPtr listener = (*iter).lock();
            listener->OnHttpRequest(session, request);
            ++iter;
        }
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

void HttpSession::OnReceive(const BoostErrCode& error, std::size_t bytes_transferred)
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
            HttpServer::NotifyOnHttpRequest(this->shared_from_this(), m_request);
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
	m_socket->ReceiveSome(CreateBufferSequence(m_buffer), SocketHandler(&HttpSession::OnReceive));
}

void HttpSession::Disconnect()
{
	m_socket->Close();
	m_server.NotifyOnHttpSessionClose(shared_from_this());
}

