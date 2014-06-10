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
class HttpSession : /*public TcpSocketListener,*/
					public boost::enable_shared_from_this<HttpSession>
{
public:
	HttpSession(HttpServer& server, TcpSocketPtr socket);
    virtual ~HttpSession() {}

    // from TcpSocketListener
    //void OnReceive(TcpSocket* socket, const BoostErrCode& error, std::size_t bytes_transferred);

    // start reading HTTP request
    void AcceptRequest();

    // terminate HTTP session, will close socket and destroy object
    void Disconnect();

    TcpSocketPtr GetSocket() { return m_socket; }

private:
    void OnReceive(const BoostErrCode& error, std::size_t bytes_transferred);
    HttpServer&  m_server;
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

	HttpServer() {}
    virtual ~HttpServer() {}

    // listener/observer to be notified for every HTTP request
    class ReqListener
    {
    public:
        virtual void OnHttpRequest(HttpSessionPtr session, Skit::HTTP::Request& request) = 0;
    };

    typedef boost::shared_ptr<HttpServer::ReqListener> ReqListenerPtr;
    typedef boost::weak_ptr<HttpServer::ReqListener> ReqListenerWeakPtr;

    // from ServerHandler
    void AcceptConnection(TcpSocketPtr socket);

    // add listener to be notified on HTTP request
    static void AddRequestListener(ReqListenerPtr listener);

    // Notify HttpServer listeners for incoming HTTP request
    static void NotifyOnHttpRequest(HttpSessionPtr session, Skit::HTTP::Request& request);

    // Notify HttpServer for session close, will destroy the session
    void NotifyOnHttpSessionClose(HttpSessionPtr session);

private:
    std::set<HttpSessionPtr> m_sessions;
};

#endif // HTTP_SERVER_H_
