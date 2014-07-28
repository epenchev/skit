//
// HttpServer.h
// Copyright (C) 2014  Emil Penchev, Bulgaria

#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include "HTTP.h"
#include "Buffer.h"
#include "Socket.h"
#include "ServerController.h"

#include <list>
#include <set>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

class HttpServer; // forward
class HttpSession : public boost::enable_shared_from_this<HttpSession>
{
public:
	HttpSession(HttpServer& server, TcpSocketPtr socket);
    virtual ~HttpSession() {}

    // start reading HTTP request
    void AcceptRequest();

    // terminate HTTP session, will close socket and destroy object
    void Disconnect();

    TcpSocketPtr GetSocket() { return m_socket; }

private:
    void OnReceive(const SysError& error, std::size_t bytes_transferred);
    HttpServer&         m_server;
    TcpSocketPtr        m_socket;
    Buffer              m_buffer;
    std::string         m_reqdata;
    Skit::HTTP::Request m_request;
};

typedef boost::shared_ptr<HttpSession> HttpSessionPtr;

// default HTTP server
class HttpServer : public Skit::ServerHandler
{
public:
	static ServerHandler* CreateItem() { return new HttpServer; }

	HttpServer();
    virtual ~HttpServer() {}

    // listener/observer to be notified for every HTTP request
    class ReqListener
    {
    public:
    	static ReqListener* CreateListener() { return NULL; }
        virtual void OnHttpRequest(HttpSessionPtr session, Skit::HTTP::Request& request) = 0;
    };

    typedef ReqListener* (*CreateListenerFunc)();

    // register listeners
    static void Register(CreateListenerFunc func) { GetRegistry().insert(func); }

    // from ServerHandler
    void AcceptConnection(TcpSocketPtr socket);

    // Notify HttpServer listeners for incoming HTTP request
    void NotifyOnHttpRequest(HttpSessionPtr session, Skit::HTTP::Request& request);

    // Notify HttpServer for session close, will destroy the session
    void NotifyOnHttpSessionClose(HttpSessionPtr session);

private:
    std::set<HttpSessionPtr> m_sessions;
    std::set<ReqListener*>   m_listeners;

    static std::set<CreateListenerFunc>& GetRegistry()
    {
    	static std::set<CreateListenerFunc> s_registryListeners;
        return s_registryListeners;
    }
};

#endif // HTTP_SERVER_H_
