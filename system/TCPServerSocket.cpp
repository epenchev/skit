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
 : mAcceptor(TaskThreadPool::GetEventThread().mIOServiceLoop, tcp::endpoint(tcp::v4(), port)),
   isListening(false)
{}

TCPServerSocket::TCPServerSocket(std::string localAdress, unsigned short port)
 : mAcceptor(TaskThreadPool::GetEventThread().mIOServiceLoop), isListening(false)
{
    boost::asio::ip::address bindAdress;
    bindAdress.from_string(localAdress);
    mAcceptor.bind( tcp::endpoint(bindAdress, port));
}

void TCPServerSocket::Accept()
{
    TCPClientSocket* outSocket = new TCPClientSocket();
    mAcceptor.async_accept(outSocket->mIOSock,
                            boost::bind(&TCPServerSocket::HandleAccept, this,
                                         outSocket, boost::asio::placeholders::error));
}

unsigned short TCPServerSocket::GetPort()
{
    return mAcceptor.local_endpoint().port();
}

void TCPServerSocket::Listen()
{
    mErrCode.Clear();
    if (!isListening)
    {
        try
        {
            // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
            mAcceptor.set_option(tcp::acceptor::reuse_address(true));
            mAcceptor.listen();
            /* BLITZ_LOG_INFO("Listening on port: %d", getPort());; */
        }
        catch (std::exception& e)
        {
            //BLITZ_LOG_ERROR("Unable to bind to port: %d exception: %s", getPort(), e.what());
            mErrCode.SetMessage("Unable to bind to port ");
            return;
        }
        isListening = true;
    }
}

void TCPServerSocket::AddSocketListener(ServerSocketObserver* inListener)
{
    mErrCode.Clear();
    if (!mEventListener)
    {
        if (inListener)
            mEventListener = inListener;
        else
        {
            // Error print goes here
            mErrCode.SetMessage("Empty event listener parameter !!!");
        }
    }
    else
    {
        // Error print goes here
        mErrCode.SetMessage("Already have event listener !!!");
    }
}


void TCPServerSocket::RemoveSocketListener()
{
    mEventListener = NULL;
}

void TCPServerSocket::HandleAccept(TCPClientSocket* outSocket,
                                   const boost::system::error_code& error)
{
    mErrCode.Clear();
    if (error)
    {
        // Error print goes here
        mErrCode.SetMessage(error.message().c_str());
    }

    Task* acceptTask = new Task();
    if (acceptTask && mEventListener)
    {
        acceptTask->Connect(&ServerSocketObserver::OnAccept, mEventListener, outSocket, mErrCode);
        TaskThreadPool::Signal(acceptTask);
    }
    else
    {
        // Error print goes here
    }

    // continue accepting new connections
    this->Accept();
}


