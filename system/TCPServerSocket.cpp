/*
 * TCPServerSocket.cpp
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
 *  Created on: Jul 2, 2013
 *      Author: emo
 */

#include "system/TCPServerSocket.h"
#include "system/TaskThread.h"
#include "system/Task.h"
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

TCPServerSocket::TCPServerSocket(unsigned short port)
 : mAcceptor(TaskThreadPool::GetEventThread().mIOServiceLoop),
   mlocalPort(port), mlocalAdress(""), mEventListener(NULL), misListening(false)
{
    mOutError = new ErrorCode();
}

TCPServerSocket::TCPServerSocket(std::string localAdress, unsigned short port)
 : mAcceptor(TaskThreadPool::GetEventThread().mIOServiceLoop),
   mlocalPort(port), mlocalAdress(localAdress), mEventListener(NULL), misListening(false)
{
    mOutError = new ErrorCode();
}

TCPServerSocket::~TCPServerSocket()
{
    delete mOutError;
    this->Stop();
}

void TCPServerSocket::Listen(ErrorCode& outError)
{
    try
    {
        mAcceptor.open(boost::asio::ip::tcp::v4());
        if (mlocalAdress != "")
        {
            boost::asio::ip::address bindAdress;
            bindAdress.from_string(mlocalAdress);
            mAcceptor.bind(tcp::endpoint(bindAdress, mlocalPort));
        }
        else
        {
            mAcceptor.bind(tcp::endpoint(tcp::v4(), mlocalPort));
        }
        mAcceptor.set_option(tcp::acceptor::reuse_address(true));
        mAcceptor.listen();
    }
    catch(boost::system::system_error& ex)
    {
        outError.SetValue(ex.code().value());
        return;
    };
    misListening = true;
}


void TCPServerSocket::Accept(ErrorCode& outError)
{
    if (!mEventListener)
    {
        outError.SetValue(EFAULT);
        return;
    }
    TCPClientSocket* clientSocket = new TCPClientSocket();
    mAcceptor.async_accept(clientSocket->mIOSock,
                            boost::bind(&TCPServerSocket::HandleAccept, this,
                                    clientSocket, boost::asio::placeholders::error));
}

unsigned short TCPServerSocket::GetListeningPort()
{
    return mAcceptor.local_endpoint().port();
}

void TCPServerSocket::SetListener(TCPServerSocketObserver* inListener, ErrorCode& outError)
{
    if (inListener)
    {
        mEventListener = inListener;
    }
    else
    {
        outError.SetValue(EINVAL);
        return;
    }
}

void TCPServerSocket::Stop()
{
    if (this->misListening)
    {
        // no error handling here, nobody cares
        boost::system::error_code error;
        mAcceptor.close(error);
        this->misListening = false;
    }
}

void TCPServerSocket::HandleAccept(TCPClientSocket* outSocket,
                                   const boost::system::error_code& error)
{
    mOutError->Clear();
    Task* acceptTask = new Task();

    if (error)
    {
        mOutError->SetValue(error.value());
        delete outSocket;
        if (boost::asio::error::operation_aborted == error)
        {
            return; // operation aborted, don't signal
        }
    }

    // warning, if acceptError is true socket is invalid object
    acceptTask->Connect(&TCPServerSocketObserver::OnAccept, mEventListener, outSocket, this->mOutError);
    TaskThreadPool::Signal(acceptTask);

    // continue accepting new connections
    ErrorCode err;
    this->Accept(err);
}


