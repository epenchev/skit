/*
 * HTTPListener.cpp
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

#include "server/HTTPServer.h"
#include "server/TCPConnection.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "HTTP/HTTPRequest.h"
#include "HTTP/HTTPUtils.h"
#include "ErrorCode.h"
#include "system/Task.h"
#include "system/TaskThread.h"

HTTPSession::HTTPSession(unsigned sessionId, TCPClientSocket* inSocket)
 : TCPConnection(sessionId, inSocket), mioChannel(NULL), msocketBuffer(NULL)
{}

HTTPSession::~HTTPSession()
{
    delete msocketBuffer;
    this->Close();
}

void HTTPSession::AddHTTPSessionListener(HTTPSessionObserver* listener)
{
    if (listener)
    {
        msessionListeners.insert(listener);
    }
    else
    {
        // TODO Log
        std::cout << "invalid HTTP session listener \n";
    }
}

void HTTPSession::RemoveHTTPSessionListener(HTTPSessionObserver* listener)
{
    if (listener)
    {
        std::set<HTTPSessionObserver*>::iterator it = msessionListeners.find(listener);
        if (it != msessionListeners.end())
        {
            msessionListeners.erase(it);
        }
        else
        {
            // TODO Log
            std::cout << "Invalid HTTP session listener in Remove() call \n";
        }
    }
}

void HTTPSession::SendHTTPResponse()
{
    // TODO
}

void HTTPSession::WaitForRequest()
{
    ErrorCode err;

    std::cout << "WaitForRequest() in  !! \n";
    if (!msocketBuffer)
    {
        std::size_t bufferSize = 100;
        msocketBuffer = new Buffer(new char[bufferSize], bufferSize);
        msocketBuffer->Clear();
    }
    else
    {
        msocketBuffer->Clear();
    }

    mioChannel = this->OpenChannel(this);
    mioChannel->Emit(msocketBuffer, IOReadSome, err);
    if (err)
    {
        // TODO log
        std::cout << err.GetErrorMessage() << std::endl;
    }
    std::cout << "WaitForRequest() out of here !! \n";
}

void HTTPSession::OnRead(IOChannel* chan, std::size_t bytesRead, ErrorCode* inErr)
{
    ErrorCode err = *inErr;
    std::cout << "bytes read " << bytesRead << std::endl;

    if (!err)
    {
        if (bytesRead)
        {
            mrequestStr.append(msocketBuffer->BufferCast<char*>());
            size_t pos = mrequestStr.find("\r\n\r\n");
            if (std::string::npos == pos)
            {
                if (bytesRead < msocketBuffer->Size())
                {
                    // TODO Log
                    std::cout << "Must disconnect !!! \n";
                }
                else
                {
                    mioChannel->Emit(msocketBuffer, IOReadSome, err);
                    if (err)
                    {
                        // TODO log
                        std::cout << err.GetErrorMessage() << std::endl;
                    }
                }
            }
            else // found end of request assemble HTTPRequest object
            {
                HTTPRequest request;
                std::cout << "found end of request \n";
                request.Init(mrequestStr);
                //if (request.GetLastError()) TODO Error checking

                std::cout << request.GetPath() << std::endl;
                std::cout << request.GetRequestURI() << std::endl;
                std::cout << request.GetProtocol() << std::endl;

                if (!msessionListeners.empty())
                {
                	// notify listeners for HTTP request
                	for (std::set<HTTPSessionObserver*>::iterator it = msessionListeners.begin(); it != msessionListeners.end(); ++it)
                    {
                		HTTPSessionObserver* listener = *it;
                        Task* eventTask = new Task();
                        eventTask->Connect(&HTTPSessionObserver::OnHTTPrequest, listener, &request);
                        TaskThreadPool::Signal(eventTask);
                    }
                }
                else
                {
                	// TODO log
                    std::cout << "No server listeners HTTP::OnAccept \n";
                }

#if 0
                std::string responseData = "<html><body><h2>Hi from Emo</h2></body></html>";

                HTTPHeadersMap mapheaders;
                mapheaders.insert(HTTPParam("Content-Type", "text/html"));

                std::string httpResponse = HTTPUtils::HTTPResponseToString(200, &responseData, &mapheaders);
                std::cout << httpResponse << std::endl;
                Buffer outbuf((char*)httpResponse.c_str(), httpResponse.size());
                mioChannel->Emit(&outbuf, IOWrite, err);
#endif
            }
        }
        else
        {
            std::cout << "nothing to bytesRead == 0 read must disconnect !!! \n";
            //chan->
        }
    }
    else
    {
        // TODO log
        std::cout << err.GetErrorMessage() << std::endl;
    }

#if 0
    //if (data)
    {
        std::cout << data << std::endl;
        std::string str(data);
        request.Init(str);
        std::cout << request.GetPath() << std::endl;
        std::cout << request.GetRequestURI() << std::endl;
        std::cout << request.GetProtocol() << std::endl;


        std::string httpResponse = HTTPUtils::HTTPResponseToString(200);
        Buffer outbuf((char*)httpResponse.c_str(), httpResponse.size());
        mioChannel->Emit(&outbuf, IOWrite);
    }
#endif
}

void HTTPSession::OnWrite(IOChannel* chan, std::size_t bytesWriten, ErrorCode* inErr)
{
    std::cout << "bytes written " << bytesWriten << std::endl;

    //std::string temp(BufferCast<char*>(*msocketBuffer));
    //temp.clear();
    //mioChannel->Emit(msocketBuffer, IOReadSome);
    this->Close();
}

void HTTPSession::OnConnectionClose(IOChannel* chan)
{
    std::cout << "Connection is closed \n";
}



HTTPServer::HTTPServer(unsigned short port)
 : mIsStarted(false), mServerSock(port)
{}


HTTPServer::HTTPServer(std::string localAdress, unsigned short port)
 :  mIsStarted(false), mServerSock(localAdress, port)
{}

HTTPServer::~HTTPServer()
{
    if (!mIsStarted)
    {
        for (std::map<unsigned, HTTPSession*>::iterator it = msessionsMap.begin(); it != msessionsMap.end(); ++it)
        {
            HTTPSession* session = msessionsMap.end()->second;
            delete session;
        }
    }
    else
    {
        // TODO Log
        std::cout << "Server is not stopped connections are not destroyed leaking memory " << std::endl;
    }
}


void HTTPServer::OnAccept(TCPClientSocket* inNewSocket, ErrorCode* inError)
{
    HTTPSession* session = NULL;
    ErrorCode err = *inError;
    if (!err)
    {
        if (inNewSocket)
        {
            // TODO log
            std::cout << "connected from " << inNewSocket->GetRemotePeerIP(err) << ":"
                                  << inNewSocket->GetRemotePeerPort(err) << std::endl;

            if (!msessionsMap.empty())
            {
                HTTPSession* lastSession = msessionsMap.rbegin()->second;
                session = new HTTPSession((lastSession->GetConnId() + 1), inNewSocket);
            }
            else
            {
                session = new HTTPSession(1, inNewSocket);
            }
            // Attach to session as listener (catch disconnect events)
            IOChannel* channel = session->OpenChannel(this);
            msessionsMap.insert(std::pair<unsigned,HTTPSession*>(channel->GetChannelId(), session));

            // Start listening for incoming HTTP requests
            session->WaitForRequest();

            if (!mlisteners.empty())
            {
            	// notify listeners for incoming connection
            	for (std::set<HTTPServerObserver*>::iterator it = mlisteners.begin(); it != mlisteners.end(); ++it)
                {
            		HTTPServerObserver* listener = *it;
                    Task* eventTask = new Task();
                    eventTask->Connect(&HTTPServerObserver::OnHTTPSessionCreate, listener, session);
                    TaskThreadPool::Signal(eventTask);
                }
            }
            else
            {
            	// TODO log
            	std::cout << "No server listeners HTTP::OnAccept \n";
            }

        }
        else
        {
            // TODO log
            std::cout << "Invalid TCPClientSocket object" << std::endl;
        }
    }
    else
    {
        // TODO log
        std::cout << err.GetErrorMessage() << std::endl;
    }
}

void HTTPServer::Start()
{
    ErrorCode err;

    if (!mIsStarted)
    {
        mServerSock.SetListener(this, err);
        mServerSock.Listen(err);
        mServerSock.Accept(err);
        mIsStarted = true;

        if (!mlisteners.empty())
        {
        	for (std::set<HTTPServerObserver*>::iterator it = mlisteners.begin(); it != mlisteners.end(); ++it)
        	{
        		HTTPServerObserver* listener = *it;
        		Task* eventTask = new Task();
        		eventTask->Connect(&HTTPServerObserver::OnServerStart, listener, this);
        		TaskThreadPool::Signal(eventTask);
        	}
        }
        else
        {
        	// TODO log
        	std::cout << " No HTTP server listeners \n";
        }
    }
}

void HTTPServer::Stop()
{
    if (mIsStarted)
    {
        mServerSock.Stop();
        for (std::set<HTTPServerObserver*>::iterator it = mlisteners.begin(); it != mlisteners.end(); ++it)
        {
            HTTPServerObserver* listener = *it;
            // notify every listener that we are stopping so detaching from connection can be done
            listener->OnServerStop(this);
        }

        // close all connections and detach server from listening for events on connection channels
        for (std::map<unsigned, HTTPSession*>::iterator it = msessionsMap.begin(); it != msessionsMap.end(); ++it)
        {
            HTTPSession* session = msessionsMap.end()->second;
            // close channel so we don't get nofied for every connection close
            session->CloseChannel(it->first);
            session->Close();
        }
    }
    else
    {
        // TODO log
        std::cout << "Server already stopped" << std::endl;
    }
}

unsigned int HTTPServer::GetConnectionCount()
{
    return msessionsMap.size();
}

void HTTPServer::GetServerProperties()
{
    // TODO to be implemented
}

void HTTPServer::AddServerListener(HTTPServerObserver* serverListener)
{
    if (serverListener)
    {
        mlisteners.insert(serverListener);
    }
    else
    {
        // TODO Log
        std::cout << "Invalid HTTP server listener \n";
    }
}

void HTTPServer::RemoveServerListener(HTTPServerObserver* serverListener)
{
    if (serverListener)
    {
        std::set<HTTPServerObserver*>::iterator it = mlisteners.find(serverListener);
        if (it != mlisteners.end())
        {
            mlisteners.erase(it);
        }
    }
    else
    {
        // TODO Log
        std::cout << "Invalid HTTP server listener in Remove() call \n";
    }
}

void HTTPServer::OnRead(IOChannel* chan, std::size_t bytesRead, ErrorCode* inErr)
{ /* empty, no need to implement this */ }

void HTTPServer::OnWrite(IOChannel* chan, std::size_t bytesWriten, ErrorCode* inErr)
{ /* empty, no need to implement this */ }


void HTTPServer::OnConnectionClose(IOChannel* chan)
{
    // TODO Log
    std::cout << "Connection is closed \n";
}



