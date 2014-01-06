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
#include "IHTTPServer.h"
#include <set>
#include <map>

/**
* HTTP network session.
*/
class HTTPSession : public IHTTPSession, public TCPConnection, public IOChannelListener
{
public:
    HTTPSession(unsigned id, TCPSocket* inSocket);
    virtual ~HTTPSession();

    /* From IHTTPSession */
    void AddHTTPSessionListener(HTTPSessionListener* listener);

    /* From IHTTPSession */
    void RemoveHTTPSessionListener(HTTPSessionListener* listener);

    /* From IHTTPSession */
    void AcceptRequest();

    /* From IHTTPSession */
    const HTTPRequest& GetRequest() const { return m_request; }

    /* From IOChannelListener */
    void OnRead(IOChannel* chan, std::size_t bytesRead, ErrorCode& err);

    /* From IOChannelListener */
    void OnWrite(IOChannel* chan, std::size_t bytesWritten, ErrorCode& err);

private:

    /**
    * Send the HTTP response.
    */
    void SendResponse();

    // notify listeners for HTTP request
    void NotifyOnHTTPrequest();

    // process reply from listeners
    void NotifyOnHTTPRequestReply(bool& replyOut);

    void NotifyOnHTTPResponseSend();

    bool m_acceptRequest;                        /**< if can accept requests or still waiting for reply to be send  */
    IOChannel* m_channel;                        /**< Connection channel for I/O events */
    Buffer* m_buffer;                            /**< buffer for I/O operations */
    std::string m_reqheaders;                    /**< string with full or part HTTP request headers */
    HTTPRequest m_request;                       /**< HTTP request object */
    HTTPResponse m_response;                     /**< HTTP response object */
    SystemMutex m_lockchan;                      /**< I/O channel lock */
    SystemMutex m_lockListeners;                 /**< listeners lock */
    std::set<HTTPSessionListener*> m_listeners;  /**< listeners/observers for session events */
    const static std::size_t m_recvsize = 100;  /**< receive buffer size bytes */
};

/**
* HTTP server.
* Accept HTTP connections on a given port.
* Different plug-in modules can be attached to the server via the HTTPServerObserver interface.
*/
class HTTPServer : public IHTTPServer, public TCPAcceptorHandler, public NetConnectionListener
{
public:
    HTTPServer(unsigned short port);
    HTTPServer(std::string localAdress, unsigned short port);
    virtual ~HTTPServer();

    /* From IHTTPServer */
    void Start();

    /* From IHTTPServer */
    void Stop();

    /* From IHTTPServer */
    unsigned GetConnectionCount();

    /* From IHTTPServer */
    void AddServerListener(HTTPServerListener* listener);

    /* From IHTTPServer */
    void RemoveServerListener(HTTPServerListener* listener);

    /* From NetConnectionListener */
    void OnConnectionClose(NetConnection* conn);

private:
    /* From TCPAcceptorHandler */
    void OnAccept(TCPSocket* inNewSocket, ErrorCode& inError);

    bool m_started;                                 /**< server started flag  */
    TCPAcceptor m_acceptor;                         /**< TCP acceptor object  */
    SystemMutex m_lockListeners;                    /**< listeners lock */
    SystemMutex m_lockSessions;                     /**< sessions lock */
    std::set<HTTPSession*> m_sessions;              /**< HTTP sessions/connections */
    std::set<HTTPServerListener*> m_listeners;      /**< listeners/observers for server events */
};

#endif /* HTTPSERVER_H_ */
