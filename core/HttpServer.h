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

using namespace std;

class HttpSession;
typedef boost::shared_ptr<HttpSession> HttpSessionPtr;

class HttpSession : public boost::enable_shared_from_this<HttpSession>
{
public:
	HttpSession( TcpSocket* socket );
    virtual ~HttpSession();

    // wait for HTTP request and notify listener
    void AcceptRequest();

    // used for writing or custom handling of the incoming data
    TcpSocket* GetSocket() { return _socketObj; }

    // listener/observer to be notified for a HTTP request event
    class Listener
    {
    public:
        virtual void OnHttpRequest( HttpSessionPtr session,
                                    Skit::HTTP::Request& request,
                                    const SysError& error ) = 0;
    };

    // Attach a listener/observer object to the session
    void SetListener( HttpSession::Listener& listener );

private:
    // socket callback
    void OnReceive( const SysError& error, size_t bytesRead );

    SysError               _error;
    TcpSocket*             _socketObj;
    Buffer                 _bufferObj;
    string                 _reqdata;
    Skit::HTTP::Request    _requestObj;
    HttpSession::Listener* _listener;
};

// default HTTP server
class HttpServer : public ServerHandler,
                   public HttpSession::Listener
{
public:
    // For the ServerController::ServerHandlerFactory
	static ServerHandler* CreateItem() { return new HttpServer; }

	HttpServer();
    virtual ~HttpServer() {}

    // listener/observer to be notified for incoming HTTP session
    class Listener
    {
    public:
    	static HttpServer::Listener* CreateListener() { return NULL; }
    	// true if session is to be handled false otherwise
        virtual bool OnHttpSession( HttpSessionPtr session, Skit::HTTP::Request& request ) = 0;
    };

    typedef HttpServer::Listener* (*CreateListenerFunc)();
    // register listeners to the server
    static void Register( CreateListenerFunc func ) { GetRegistry().insert(func); }

    // from ServerHandler
    void AcceptConnection( TcpSocket* socket );
    // from HttpSession::Listener
    void OnHttpRequest( HttpSessionPtr session,
                        Skit::HTTP::Request& request,
                        const SysError& error );

private:
    // socket callback
    void OnSend( const SysError& error, size_t bytesWriten );

    string _defaultResponse;
    set<HttpServer::Listener*> _listeners;

    static inline set<CreateListenerFunc>& GetRegistry()
    {
    	static set<CreateListenerFunc> s_registry;
        return s_registry;
    }
};

#endif // HTTP_SERVER_H_
