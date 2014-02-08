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
#include "TCPConnection.h"
#include "HTTP/HTTPRequest.h"
#include "HTTP/HTTPResponse.h"
#include <set>
#include <boost/enable_shared_from_this.hpp>

class HTTPSessionListener;
class HTTPServerListener;

/**
* HTTP network session.
*/
class HTTPSession : public boost::enable_shared_from_this<HTTPSession>,
                    public TCPConnection, public IOChannelListener
{
public:
    HTTPSession(unsigned id, TCPSocket* inSocket);
    virtual ~HTTPSession();

    /**
    * Add a listener/observer object to be notified for incoming HTTP requests.
    * @param listener - listener/observer object
    */
    void AddHTTPSessionListener(HTTPSessionListener* listener);

    /**
    * Remove a listener/observer object.
    * @param listener - listener/observer object
    */
    void RemoveHTTPSessionListener(HTTPSessionListener* listener);

    /**
    * Start accepting HTTP requests.
    */
    void AcceptRequest();

    /**
    * Get the request object representing the current accepted HTTP request.
    * @return const HTTPRequest&  - constant reference to HTTPRequest object.
    */
    const HTTPRequest& GetRequest() const { return m_request; }

    /* From IOChannelListener */
    void OnRead(IOChannel* chan, std::size_t bytesRead, ErrorCode& err);

protected:

    /**
    * Notify listeners for incoming HTTP request.
    */
    void NotifyOnHTTPRequest();

    IOChannel* m_channel;                        /**< Connection channel for I/O events */
    Buffer* m_buffer;                            /**< buffer for I/O operations */
    std::string m_reqheaders;                    /**< string with full or part HTTP request headers */
    HTTPRequest m_request;                       /**< HTTP request object */
    SystemMutex m_lockListeners;                 /**< listeners lock */
    std::set<HTTPSessionListener*> m_listeners;  /**< listeners/observers for session events */
    const static std::size_t m_recvsize = 100;   /**< receive buffer size bytes */
};

typedef boost::shared_ptr<HTTPSession> HTTPSessionPtr;

/**
* HTTP server.
* Accept HTTP connections on a given port.
* plug-in modules can be attached to the server via the HTTPServerObserver interface.
*/
class HTTPServer : public TCPAcceptorHandler,
                   public NetConnectionListener
{
public:
    HTTPServer(unsigned short port);
    HTTPServer(std::string localAdress, unsigned short port);
    virtual ~HTTPServer();

    /**
    * Start HTTPServer
    */
    void Start();

    /**
    * Stop HTTPServer
    */
    void Stop();

    /**
    * Get accepted sessions count.
    * @return unsigned - session count.
    */
    unsigned GetConnectionCount();

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
    * @param session - instance to HTTPSession
    */
    void NotifyOnSessionCreate(HTTPSessionPtr session);

    /**
    * Notify listeners for server start event.
    */
    void NotifyOnServerStart();

    /**
    * Notify listeners for server stop event.
    */
    void NotifyOnServerStop();

    /* From NetConnectionListener */
    void OnConnectionClose(NetConnection& conn);

    bool m_started;                                 /**< server started flag  */
    TCPAcceptor m_acceptor;                         /**< TCP acceptor object  */
    SystemMutex m_lockListeners;                    /**< listeners lock */
    SystemMutex m_lockSessions;                     /**< sessions lock */
    std::set<HTTPSessionPtr> m_sessions;            /**< HTTP sessions/connections */
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
    * @param session - object instance of accepted HTTPSession
    */
    virtual void OnHTTPSessionAccept(HTTPSessionPtr session) {}

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
* HTTPSession listener/observer to be notified on events from session.
*/
class HTTPSessionListener
{
public:

	/**
	* Triggered when HTTP request is received.
	* @param session - reference to HTTP session object.
	* @param inRequest - HTTP request received.
	*/
    virtual void OnHTTPrequest(HTTPSessionPtr session, const HTTPRequest& inRequest) {}
};


#endif /* HTTPSERVER_H_ */
