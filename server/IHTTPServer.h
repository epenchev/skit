/*
 * IHTTPServer.h
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
 *  Created on: Oct 21, 2013
 *      Author: emo
 */

#ifndef IHTTPSERVER_H_
#define IHTTPSERVER_H_

class HTTPRequest;
class HTTPResponse;
class IHTTPSession;
class IHTTPServer;

class HTTPSessionListener
{
public:
    // Get HTTP request
    virtual void OnHTTPrequest(const IHTTPSession& session, HTTPRequest* inRequest) {}

    // force sending response discard all other modules
    virtual void OnHTTPRequestReply(const IHTTPSession& session, HTTPResponse* inResponse, bool& forceReply) {}

    // Get send HTTP reply
    virtual void OnReplySend(IHTTPSession* session) {}
};

class HTTPServerListener
{
public:
    /**
    * Triggered when new HTTP session is created.
    */
    virtual void OnHTTPSessionCreate(IHTTPSession* session) {}

    /**
    * Triggered when HTTP session is destroyed.
    */
    virtual void OnHTTPSessionDestroy(IHTTPSession* session) {}

    /**
    * Triggered when server is starting.
    */
    virtual void OnServerStart(IHTTPServer* server) {}

    /**
    * Triggered when server is shutting down.
    */
    virtual void OnServerStop(IHTTPServer* server) {}
};

class IHTTPSession
{
public:
    virtual void AddHTTPSessionListener(HTTPSessionListener* listener) = 0;

    virtual void RemoveHTTPSessionListener(HTTPSessionListener* listener) = 0;

    virtual void AcceptRequest() = 0;

    virtual const HTTPRequest& GetRequest() const = 0 ;
};

class IHTTPServer
{
public:
    virtual void Start() = 0;

    virtual void Stop() = 0;

    virtual unsigned int GetConnectionCount() = 0;

    virtual void AddServerListener(HTTPServerListener* serverListener) = 0;

    virtual void RemoveServerListener(HTTPServerListener* serverListener) = 0;
};



#endif /* IHTTPSERVER_H_ */
