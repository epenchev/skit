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
#include <exception>

using boost::asio::ip::tcp;

TCPServerSocket::TCPServerSocket(unsigned short port)
 : mAcceptor(TaskThreadPool::GetEventThread().mIOServiceLoop),
   mlocalPort(port), mlocalAdress(""), mEventListener(NULL), misListening(false)
{}

TCPServerSocket::TCPServerSocket(std::string localAdress, unsigned short port)
 : mAcceptor(TaskThreadPool::GetEventThread().mIOServiceLoop),
   mlocalPort(port), mlocalAdress(localAdress), mEventListener(NULL), misListening(false)
{}

void TCPServerSocket::Listen()
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
        mErrCode.SetValue(ex.code().value());
        throw SystemException(mErrCode);
    };
    misListening = true;
}


void TCPServerSocket::Accept()
{
    if (!mEventListener)
    {
        mErrCode.SetValue(EFAULT);
        throw SystemException(mErrCode);
    }
    TCPClientSocket* outSocket = new TCPClientSocket();
    mAcceptor.async_accept(outSocket->mIOSock,
                            boost::bind(&TCPServerSocket::HandleAccept, this,
                                  outSocket, boost::asio::placeholders::error));
}

unsigned short TCPServerSocket::GetPort()
{
    return mAcceptor.local_endpoint().port();
}

void TCPServerSocket::AddSocketListener(ServerSocketObserver* inListener)
{
    if (!mEventListener)
    {
        if (inListener)
        {
            mEventListener = inListener;
        }
        else
        {
            mErrCode.SetValue(EINVAL);
            throw SystemException(mErrCode);
        }
    }
}


void TCPServerSocket::RemoveSocketListener()
{
    mEventListener = NULL;
}

void TCPServerSocket::HandleAccept(TCPClientSocket* outSocket,
                                   const boost::system::error_code& error)
{
    Task* acceptTask = new Task();
    if (error)
    {
        // TODO log error
        delete outSocket;
        mErrCode.SetValue(error.value());
    }
    acceptTask->Connect(&ServerSocketObserver::OnAccept, mEventListener, outSocket);
    TaskThreadPool::Signal(acceptTask);

    // continue accepting new connections
    this->Accept();
}


