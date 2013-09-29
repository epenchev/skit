/*
 * TCPClientSocket.cpp
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
 *  Created on: Jul 1, 2013
 *      Author: emo
 */


#include "system/TCPClientSocket.h"
#include "system/Task.h"
#include "system/TaskThread.h"
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

TCPClientSocket::TCPClientSocket()
 : mIOSock(TaskThreadPool::GetEventThread().mIOServiceLoop)
{}

void TCPClientSocket::Send(Buffer& inData)
{
    if (!mEventListener)
    {
        mErrCode.SetValue(EFAULT);
        throw SystemException(mErrCode);
    }

    if (mIOSock.is_open())
    {
        const char* dataBuf = BufferCast<const char*>(inData);
        std::size_t dataSize = inData.GetSize();

        if (dataBuf && dataSize)
        {
            boost::asio::async_write(mIOSock, boost::asio::buffer(dataBuf, dataSize),
                                     boost::bind(&TCPClientSocket::HandleWrite, this,
                                       boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            mErrCode.SetValue(EINVAL);
            throw SystemException(mErrCode);
        }
    }
}

void TCPClientSocket::Receive(Buffer& outData)
{
    if (!mEventListener)
    {
         mErrCode.SetValue(EFAULT);
         throw SystemException(mErrCode);
    }

    if (mIOSock.is_open())
    {
        char* dataBuf = BufferCast<char*>(outData);
        std::size_t dataSize = outData.GetSize();

        if (dataBuf && dataSize)
        {
            boost::asio::async_read(mIOSock, boost::asio::buffer(dataBuf, dataSize),
                                    boost::bind(&TCPClientSocket::HandleRead, this,
                                      boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            mErrCode.SetValue(EINVAL);
            throw SystemException(mErrCode);
        }
    }
}


void TCPClientSocket::ReceiveSome(Buffer& outData)
{
    if (!mEventListener)
    {
        mErrCode.SetValue(EFAULT);
        throw SystemException(mErrCode);
    }

    if (mIOSock.is_open())
    {
        char* dataBuf = BufferCast<char*>(outData);
        std::size_t dataSize = outData.GetSize();

        if (dataBuf && dataSize)
        {
            mIOSock.async_read_some(boost::asio::buffer(dataBuf, dataSize),
                                     boost::bind(&TCPClientSocket::HandleRead, this,
                                       boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            mErrCode.SetValue(EINVAL);
            throw SystemException(mErrCode);
        }
    }
}

std::string TCPClientSocket::GetRemotePeerIP()
{
    std::string peer;
    peer.clear();

    if (mIOSock.is_open())
    {
        boost::system::error_code error;
        boost::asio::ip::tcp::endpoint endpoint = mIOSock.remote_endpoint(error);

        if (error)
        {
            mErrCode.SetValue(error.value());
            throw SystemException(mErrCode);
        }
        else
        {
            peer = endpoint.address().to_string(error);
        }
    }
    else
    {
        mErrCode.SetValue(ENOTCONN);
        throw SystemException(mErrCode);
    }
    return peer;
}

unsigned short TCPClientSocket::GetRemotePeerPort()
{
    unsigned short netPort = 0;

    if (mIOSock.is_open())
    {
        boost::system::error_code error;
        boost::asio::ip::tcp::endpoint endpoint = mIOSock.remote_endpoint(error);

        if (error)
        {
            mErrCode.SetValue(error.value());
            throw SystemException(mErrCode);
        }
        else
        {
            netPort = endpoint.port();
        }
    }
    else
    {
        mErrCode.SetValue(ENOTCONN);
        throw SystemException(mErrCode);
    }
    return netPort;
}

void TCPClientSocket::Connect(std::string host, std::string netService)
{
    boost::system::error_code error;
    boost::asio::ip::address hostIP = boost::asio::ip::address::from_string(host, error);

    if (error)
    {
        // host is not IP address, resolve it.
        tcp::resolver resolver(mIOSock.get_io_service());
        tcp::resolver::query query(host, netService);

        resolver.async_resolve(query, boost::bind(&TCPClientSocket::HandleResolve, this,
                                 boost::asio::placeholders::error,
                                   boost::asio::placeholders::iterator));
    }
    else
    {
        boost::asio::ip::tcp::endpoint endpoint(hostIP, atoi(netService.c_str()));
        mIOSock.async_connect(endpoint, boost::bind(&TCPClientSocket::HandleConnect, this,
                                boost::asio::placeholders::error));
    }
}

void TCPClientSocket::Disconnect()
{
    if (mIOSock.is_open())
    {
        // no error handling here, nobody cares
        boost::system::error_code error;
        mIOSock.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
        mIOSock.close(error);
    }
}

void TCPClientSocket::AttachSocketListener(ClientSocketObserver* inListener)
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

void TCPClientSocket::RemoveSocketListener()
{
    mEventListener = NULL;
}

void TCPClientSocket::HandleWrite(const boost::system::error_code& error,
                                  std::size_t bytes_transferred)
{
    mBytesTransferred = bytes_transferred;

    if (error)
    {
        mErrCode.SetValue(error.value());
        if (boost::asio::error::operation_aborted == error)
        {
            return; // operation aborted
        }
    }

    Task* writeTask = new Task();
    writeTask->Connect(&ClientSocketObserver::OnSend, mEventListener, this);
    TaskThreadPool::Signal(writeTask);
}

void TCPClientSocket::HandleRead(const boost::system::error_code& error,
                                 std::size_t bytes_transferred)
{

    mBytesTransferred = bytes_transferred;

    if (error)
    {
        std::cout << error.message() << std::endl;

    	mErrCode.SetValue(error.value());
        if (boost::asio::error::operation_aborted == error)
        {
            return; // operation aborted, don't signal
        }
    }

   Task* readTask = new Task();
   readTask->Connect(&ClientSocketObserver::OnReceive, mEventListener, this);
   TaskThreadPool::Signal(readTask);

}

void TCPClientSocket::HandleResolve(const boost::system::error_code& error,
                                    tcp::resolver::iterator endpoint_iterator)
{
    mErrCode.Clear();

    if (error)
    {
        mErrCode.SetValue(error.value());
        if (boost::asio::error::operation_aborted == error)
        {
            return; // operation aborted, don't signal
        }

        // signal only in case of error
        Task* connectTask = new Task();
        connectTask->Connect(&ClientSocketObserver::OnConnect, mEventListener, this);
        TaskThreadPool::Signal(connectTask);
    }
    else
    {
        boost::asio::async_connect(mIOSock, endpoint_iterator,
                                     boost::bind(&TCPClientSocket::HandleConnect, this,
                                       boost::asio::placeholders::error));
    }

}

void TCPClientSocket::HandleConnect(const boost::system::error_code& error)
{
    mErrCode.Clear();

    if (error)
    {
        mErrCode.SetValue(error.value());
        if (boost::asio::error::operation_aborted == error)
        {
            return; // operation aborted, don't signal
        }
    }

    Task* connectTask = new Task();
    connectTask->Connect(&ClientSocketObserver::OnReceive, mEventListener, this);
    TaskThreadPool::Signal(connectTask);
}

