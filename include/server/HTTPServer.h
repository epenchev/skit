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

#include "system/TCPServerSocket.h"
#include "TCPConnection.h"
#include <set>
#include "HTTP/HTTPRequest.h"

class HTTPSessionObserver
{
public:
    virtual void OnHTTPrequest(HTTPRequest* inRequest) = 0;
};

/**
* HTTP network session.
* Interface between media clients and server using HTTP protocol.
*/
class HTTPSession : public TCPConnection, public IOChannelObserver
{
public:
    HTTPSession(unsigned sessionId, TCPClientSocket* inSocket);

    virtual ~HTTPSession();

    void AddHTTPSessionListener(HTTPSessionObserver* listener);

    void RemoveHTTPSessionListener(HTTPSessionObserver* listener);

    void SendHTTPResponse();

    void WaitForRequest();


    void OnRead(IOChannel* chan, std::size_t bytesRead, ErrorCode* inErr);

    void OnWrite(IOChannel* chan, std::size_t bytesWriten, ErrorCode* inErr);

    void OnConnectionClose(IOChannel* chan);

private:
    IOChannel* mIoChann;
    std::set<HTTPSessionObserver*> mSessionListeners;
    Buffer* mRequestBuf;
    Buffer* mMainBuffer;
};

class HTTPServer;
/**
* Abstract base class for implementing event listeners(observers) for HTTPServer object.
*/
class HTTPServerObserver
{
    /**
    * Triggered when new HTTP session is created.
    */
    virtual void OnHTTPSessionCreate(HTTPSession* session) = 0;

    /**
    * Triggered when HTTP session is destroyed.
    */
    virtual void OnHTTPSessionDestroy(HTTPSession* session) = 0;

    /**
    * Triggered when server is starting.
    */
    virtual void OnServerStart(HTTPServer* server) = 0;

    /**
    * Triggered when server is shutting down.
    */
    virtual void OnServerStop(HTTPServer* server) = 0;
};


/**
* HTTP server.
* Accept connections on a given port and accepts HTTP request from clients.
* Different plug-in modules can be attached to the server via the HTTPServerObserver interface.
*/
class HTTPServer : public TCPServerSocketObserver, public IOChannelObserver
{
public:
    HTTPServer(unsigned short port);
    HTTPServer(std::string localAdress, unsigned short port);
    virtual ~HTTPServer();

    void Start();

    void Stop();

    void GetGlobalServerInstance();

    unsigned int GetConnectionCount();

    void GetServerProperties();

    void AddServerListener(HTTPServerObserver* serverListener);

    void RemoveServerListener(HTTPServerObserver* serverListener);


    void OnRead(IOChannel* chan, std::size_t bytesRead, ErrorCode* inErr);

    void OnWrite(IOChannel* chan, std::size_t bytesWriten, ErrorCode* inErr);

    void OnConnectionClose(IOChannel* chan);

private:
    /* From ServerSocketObserver */
    void OnAccept(TCPClientSocket* inNewSocket, ErrorCode* inError);

    TCPServerSocket mServerSock;
    HTTPServerObserver* mEventObserver;
};

#endif /* HTTPSERVER_H_ */
