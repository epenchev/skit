//
// http_server.h
// Copyright (C) 2014  Emil Penchev, Bulgaria

#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include "HTTP.h"
#include "Buffer.h"
#include "socket.h"
//#include "ServerController.h"

#include <list>
#include <set>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace std;

class HTTP_Session;
typedef boost::shared_ptr<HTTP_Session> HTTP_SessionPtr;

/**
* A wrapper around a TCP socket object which represents a HTTP session.
* Gives event notification for incoming HTTP request.
*/
class HTTP_Session : public boost::enable_shared_from_this<HTTP_Session>
{
public:
	HTTP_Session(TCP_Socket* socket_);
    virtual ~HTTP_Session();

    /**
     * Wait for incoming HTTP request or subsequent data
     * and notify listener if a start of a request is received
     */
    void acceptRequest();

    TCP_Socket* getSocket() { return socket_; }

    /** listener/observer inner class interface to be notified for a incoming HTTP request. */
    class Listener
    {
    public:
        virtual void onRequest(HTTP_SessionPtr session,
                               Skit::HTTP::Request& request,
                               const TCP_Socket::ErrCode& err) = 0;
    };

    /** Attach a listener/observer object to the session */
    void setListener(HTTP_Session::Listener* listener);

private:
    /** socket callback */
    void onReceive(const TCP_Socket::ErrCode& error, size_t bytesRead);

    //SysError                error_;
    TCP_Socket*              socket_;
    Buffer                  buffer_;
    string                  reqdata_;
    Skit::HTTP::Request     request_;
    HTTP_Session::Listener* listener_;
};

/**
 * HTTP server internal implementation
 */
class HTTP_Server : public ServerHandler,
                    public HTTP_Session::Listener
{
public:
    /** From ServerHandler */
	static ServerHandler* CreateItem() { return new HTTP_Server; }

	HTTP_Server();
    virtual ~HTTP_Server() {}

    /** listener/observer interface, inherit and implement in order to receive events from HTTP server */
    class Listener
    {
    public:
    	static HTTP_Session::Listener* CreateListener() { return NULL; }

    	/** Accept incoming session, true if session is to be handled false otherwise */
        virtual bool onAcceptSession(HTTP_SessionPtr session,
        		                     Skit::HTTP::Request& request) = 0;
    };

    typedef HTTP_Session::Listener* (*CreateListenerFunc)();

    /** register listeners to the server */
    static void registerListener( CreateListenerFunc func ) { GetRegistry().insert(func); }

    /** from ServerHandler */
    void acceptConnection(TCP_Socket* socket);

    /** from HttpSession::Listener */
    void onRequest( HTTP_SessionPtr session,
                    Skit::HTTP::Request& request,
                    const SysError& error );

private:
    /** socket callback */
    void OnSend( const SysError& error, size_t bytesWriten );

    string defaultResponse_;
    std::set<HTTP_Session::Listener*> listeners_;

    static inline set<CreateListenerFunc>& GetRegistry()
    {
    	static set<CreateListenerFunc> s_registry;
        return s_registry;
    }
};

#endif // HTTP_SERVER_H_
