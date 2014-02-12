/*
 * HTTPServer.h
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

#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_

#include "system/TCPSocket.h"
#include "system/TCPAcceptor.h"
#include "system/SystemThread.h"
#include "HTTP/HTTPRequest.h"
#include "HTTP/HTTPResponse.h"
#include "utils/ErrorCode.h"
#include "utils/Buffer.h"
#include <set>
#include <boost/enable_shared_from_this.hpp>

class HTTPConnectionListener;
class HTTPServerListener;

/**
* HTTP network session.
*/
class HTTPConnection
 : public boost::enable_shared_from_this<HTTPConnection>,
   public TCPSocketHandler
{
public:
    HTTPConnection(unsigned id, TCPSocket* inSocket);
    virtual ~HTTPConnection();

    /**
    * Add a listener/observer object to be notified for incoming HTTP requests.
    * @param listener - listener/observer object
    */
    void AddHTTPConnectionListener(HTTPConnectionListener* listener);

    /**
    * Remove a listener/observer object.
    * @param listener - listener/observer object
    */
    void RemoveHTTPConnectionListener(HTTPConnectionListener* listener);

    /**
    * Start accepting HTTP requests.
    */
    void AcceptRequest();

    /**
    * Get the request object representing the current accepted HTTP request.
    * @return const HTTPRequest&  - constant reference to HTTPRequest object.
    */
    const HTTPRequest& GetRequest() const { return m_request; }

    /* compare connections if they are identical */
    bool operator == (const HTTPConnection& conn) const
    { return (this->m_connID == conn.m_connID ? true : false); }

protected:

    /* From TCPSocketHandler */
    void OnReceive(TCPSocket* inSocket, std::size_t bytesRead, ErrorCode& err);

    /* Notify listeners for incoming HTTP request. */
    void NotifyOnHTTPRequest();

    unsigned                          m_connID;           /**< unique connection identifier */
    TCPSocket*                        m_socket;           /**< socket object for IO */
    Buffer                            m_buffer;           /**< buffer for I/O operations */
    std::string                       m_reqheaders;       /**< string with full or part HTTP request headers */
    HTTPRequest                       m_request;          /**< HTTP request object */
    SystemMutex                       m_lockListeners;    /**< listeners lock */
    std::set<HTTPConnectionListener*> m_listeners;        /**< listeners/observers for connection events */
    const static std::size_t          m_recvsize = 100;   /**< receive buffer size bytes */
};

typedef boost::shared_ptr<HTTPConnection> HTTPConnectionPtr;

/**
* HTTP server.
* Accept HTTP connections on a given port.
* plug-in modules can be attached to the server via the HTTPServerObserver interface.
*/
class HTTPServer : public TCPAcceptorHandler
{
public:
    HTTPServer(unsigned short port);
    HTTPServer(std::string localAdress, unsigned short port);
    virtual ~HTTPServer();

    /* Start HTTPServer */
    void Start();

    /* Stop HTTPServer */
    void Stop();

    /**
    * Add a listener/observer object to be notified for server events.
    * @param listener - listener/observer object
    */
    void AddServerListener(HTTPServerListener* listener);

    /**
    * Remove a listener/observer object.
    * @param listener - listener/observer object
    */
    void RemoveServerListener(HTTPServerListener* listener);

private:
    /* From TCPAcceptorHandler */
    void OnAccept(TCPSocket* inNewSocket, ErrorCode& inError);

    /**
    * Notify listeners for accepted HTTP session.
    * @param session - instance to HTTPConnection
    */
    void NotifyOnConnectionCreate(HTTPConnectionPtr session);

    /**
    * Notify listeners for server start event.
    */
    void NotifyOnServerStart();

    /**
    * Notify listeners for server stop event.
    */
    void NotifyOnServerStop();

    bool m_started;                                 /**< server started flag  */
    TCPAcceptor m_acceptor;                         /**< TCP acceptor object  */
    SystemMutex m_lockListeners;                    /**< listeners lock */
    std::set<HTTPServerListener*> m_listeners;      /**< listeners/observers for server events */
};

/**
* HTTPServer listener/observer to be notified on events from server.
*/
class HTTPServerListener
{
public:
    /**
    * Triggered when new HTTP session is accepted.
    * @param session - object instance of accepted HTTPConnection
    */
    virtual void OnHTTPConnectionAccept(HTTPConnectionPtr session) {}

    /**
    * Triggered when server is started.
    * @param server - reference to server.
    */
    virtual void OnServerStart(HTTPServer& server) {}

    /**
    * Triggered when server is stopped.
    * @param server - reference to server.
    */
    virtual void OnServerStop(HTTPServer& server) {}
};

/**
* HTTPConnection listener/observer to be notified on events from session.
*/
class HTTPConnectionListener
{
public:

	/**
	* Triggered when HTTP request is received.
	* @param session - reference to HTTP session object.
	* @param inRequest - HTTP request received.
	*/
    virtual void OnHTTPrequest(HTTPConnectionPtr session, const HTTPRequest& inRequest) {}
};


#endif /* HTTPSERVER_H_ */
