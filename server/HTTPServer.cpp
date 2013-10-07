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

HTTPSession::HTTPSession(unsigned sessionId, TCPClientSocket* inSocket) : TCPConnection(sessionId, inSocket)
{
    mIoChann = NULL;
    mRequestBuf = new Buffer(calloc(1, 100), 100);
    mMainBuffer = new Buffer(calloc(1, 1000), 1000);
}

HTTPSession::~HTTPSession()
{
    delete mRequestBuf;
    this->Close();
}

void HTTPSession::AddHTTPSessionListener(HTTPSessionObserver* listener)
{
    if (listener)
    {
        mSessionListeners.insert(listener);
    }
}

void HTTPSession::RemoveHTTPSessionListener(HTTPSessionObserver* listener)
{
    // TODO
}

void HTTPSession::SendHTTPResponse()
{

}

void HTTPSession::WaitForRequest()
{
    ErrorCode err;
	mIoChann = this->OpenChannel(this);
    mIoChann->Emit(mRequestBuf, IOReadSome, err);
}

void HTTPSession::OnRead(IOChannel* chan, std::size_t bytesRead, ErrorCode* inErr)
{
	ErrorCode err;
	std::cout << "bytes read " << bytesRead << std::endl;
    char* data = mRequestBuf->BufferCast<char*>();

    if (bytesRead && data)
    {
        std::string requestStr(data, bytesRead);

        //std::cout << requestStr << std::endl;
        //std::cout << "-----" << std::endl;

        size_t pos = requestStr.find("\r\n\r\n");
        if (std::string::npos == pos)
        {
            if (bytesRead < 100)
            {
                std::cout << "Must disconnect !!! \n";
            }
            else
            {
                char* mainData =  mMainBuffer->BufferCast<char*>();
                strncat(mainData, data, bytesRead);
                memset(data, 0, 100);
                mIoChann->Emit(mRequestBuf, IOReadSome, err);
            }
        }
        else
        {
            HTTPRequest request;
            std::cout << "found end of request \n";
            char* mainData =  mMainBuffer->BufferCast<char*>();
            strncat(mainData, data, bytesRead);
            std::cout << mainData << std::endl;
            std::string str(mainData);
            request.Init(str);
            //if (request.GetLastError())

            std::cout << request.GetPath() << std::endl;
            std::cout << request.GetRequestURI() << std::endl;
            std::cout << request.GetProtocol() << std::endl;

            std::string responseData = "<html><body><h2>Hi from Emo</h2></body></html>";

            HTTPHeadersMap mapheaders;
            mapheaders.insert(HTTPParam("Content-Type", "text/html"));

            std::string httpResponse = HTTPUtils::HTTPResponseToString(200, &responseData, &mapheaders);
            std::cout << httpResponse << std::endl;
            Buffer outbuf((char*)httpResponse.c_str(), httpResponse.size());
            mIoChann->Emit(&outbuf, IOWrite, err);
        }
    }
    else
    {
        std::cout << "nothing to read must disconnect !!! \n";
        //chan->
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
        mIoChann->Emit(&outbuf, IOWrite);
    }
#endif
}

void HTTPSession::OnWrite(IOChannel* chan, std::size_t bytesWriten, ErrorCode* inErr)
{
    std::cout << "bytes written " << bytesWriten << std::endl;

    //std::string temp(BufferCast<char*>(*mRequestBuf));
    //temp.clear();
    //mIoChann->Emit(mRequestBuf, IOReadSome);
    this->Close();
}

void HTTPSession::OnConnectionClose(IOChannel* chan)
{
    std::cout << "Connection is closed \n";
}



HTTPServer::HTTPServer(unsigned short port)
 : mServerSock(port)
{}


HTTPServer::HTTPServer(std::string localAdress, unsigned short port)
 : mServerSock(localAdress, port)
{}

HTTPServer::~HTTPServer()
{

}


void HTTPServer::OnAccept(TCPClientSocket* inNewSocket, ErrorCode* inError)
{
    ErrorCode err;
    std::cout << "Got new connection \n";
    if (inNewSocket)
    {
        std::cout << "connected from " << inNewSocket->GetRemotePeerIP(err)
                              << " : " << inNewSocket->GetRemotePeerPort(err) << std::endl;

        // TODO for testing only
        HTTPSession* session = new HTTPSession(1, (TCPClientSocket*)inNewSocket);
        // Start listening for incoming HTTP requests
        session->WaitForRequest();
        // Attach to session as listener (catch disconnect events)
        session->OpenChannel(this);
    }
}

void HTTPServer::Start()
{
    ErrorCode err;
    mServerSock.SetListener(this, err);
    mServerSock.Listen(err);
    mServerSock.Accept(err);
}

void HTTPServer::Stop()
{

}

void HTTPServer::GetGlobalServerInstance()
{

}

unsigned int HTTPServer::GetConnectionCount()
{
    return 0;
}

void HTTPServer::GetServerProperties()
{

}

void HTTPServer::AddServerListener(HTTPServerObserver* serverListener)
{

}

void HTTPServer::RemoveServerListener(HTTPServerObserver* serverListener)
{

}

void HTTPServer::OnRead(IOChannel* chan, std::size_t bytesRead, ErrorCode* inErr)
{ /* empty, no need to implement this */ }

void HTTPServer::OnWrite(IOChannel* chan, std::size_t bytesWriten, ErrorCode* inErr)
{ /* empty, no need to implement this */ }


void HTTPServer::OnConnectionClose(IOChannel* chan)
{
    std::cout << "Connection is closed \n";
}



