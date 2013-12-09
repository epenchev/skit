/*
 * TCPSocket.cpp
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

#include "system/TCPSocket.h"
#include "system/Task.h"
#include "system/TaskThread.h"
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

TCPSocket::TCPSocket()
 : m_socket(TaskThreadPool::GetEventThread().mIOServiceLoop)
{}

TCPSocket::~TCPSocket()
{
    if (m_socket.is_open())
    {
        Close();
    }
}

void TCPSocket::Send(Buffer& inData, TCPSocketHandler* handler)
{
    if (m_socket.is_open())
    {
        const char* data = inData.BufferCast<const char*>();

        if (data)
        {
            boost::asio::async_write(m_socket, boost::asio::buffer(data, inData.Size()), boost::bind(&TCPSocket::HandleWrite,
                                     this, boost::asio::placeholders::error,  boost::asio::placeholders::bytes_transferred, handler));
        }
    }
}

void TCPSocket::Receive(Buffer& outData, TCPSocketHandler* handler)
{
    if (m_socket.is_open())
    {
        char* data = outData.BufferCast<char*>();
        if (data)
        {
           boost::asio::async_read(m_socket, boost::asio::buffer(data, outData.Size()), boost::bind(&TCPSocket::HandleRead,
                                   this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, handler));
        }
    }
}

void TCPSocket::ReceiveSome(Buffer& outData, TCPSocketHandler* handler)
{
    if (m_socket.is_open())
    {
        char* data = outData.BufferCast<char*>();
        if (data)
        {
            m_socket.async_read_some(boost::asio::buffer(data, outData.Size()), boost::bind(&TCPSocket::HandleRead,
                                     this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, handler));
        }
    }
}

std::string TCPSocket::GetRemoteIP(ErrorCode& outError) const
{
    std::string peer;
    peer.clear();

    if (m_socket.is_open())
    {
        boost::system::error_code error;
        boost::asio::ip::tcp::endpoint endpoint = m_socket.remote_endpoint(error);
        if (error)
        {
            outError.SetValue(error.value());
        }
        else
        {
            peer = endpoint.address().to_string(error);
            if (error)
            {
                outError.SetValue(error.value());
            }
        }
    }
    else
    {
        outError.SetValue(ENOTCONN);
    }
    return peer;
}

unsigned short TCPSocket::GetRemotePort(ErrorCode& outError) const
{
    unsigned short netPort = 0;

    if (m_socket.is_open())
    {
        boost::system::error_code error;
        boost::asio::ip::tcp::endpoint endpoint = m_socket.remote_endpoint(error);
        if (error)
        {
            outError.SetValue(error.value());
        }
        else
        {
            netPort = endpoint.port();
        }
    }
    else
    {
        outError.SetValue(ENOTCONN);
    }
    return netPort;
}

void TCPSocket::Connect(std::string host, std::string netService, TCPSocketHandler* handler)
{
    boost::system::error_code error;
    boost::asio::ip::address hostIP = boost::asio::ip::address::from_string(host, error);

    if (error)
    {
        // host is not IP address, resolve it
        tcp::resolver resolver(m_socket.get_io_service());
        tcp::resolver::query query(host, netService);

        resolver.async_resolve(query, boost::bind(&TCPSocket::HandleResolve,
                               this, boost::asio::placeholders::error, boost::asio::placeholders::iterator, handler));
    }
    else
    {
        if (!netService.empty())
        {
            boost::asio::ip::tcp::endpoint endpoint(hostIP, atoi(netService.c_str()));
            m_socket.async_connect(endpoint, boost::bind(&TCPSocket::HandleConnect, this, boost::asio::placeholders::error, handler));
        }
    }
}

void TCPSocket::Close()
{
    if (m_socket.is_open())
    {
        // no error handling here nobody cares, just skip exceptions
        boost::system::error_code error;
        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
        m_socket.close(error);
    }
}

void TCPSocket::HandleWrite(const boost::system::error_code& error, std::size_t bytes_transferred, TCPSocketHandler* handler)
{
    m_error.Clear();
    if (error)
    {
        m_error.SetValue(error.value());
        if (boost::asio::error::operation_aborted == error)
        {
            return; // operation aborted
        }
    }

    if (handler)
    {
        Task* writeTask = new Task();
        writeTask->Connect(&TCPSocketHandler::OnSend, handler, this, bytes_transferred, m_error);
        TaskThreadPool::Signal(writeTask);
    }
}

void TCPSocket::HandleRead(const boost::system::error_code& error, std::size_t bytes_transferred, TCPSocketHandler* handler)
{
    m_error.Clear();

    if (error)
    {
        m_error.SetValue(error.value());
        if (boost::asio::error::operation_aborted == error)
        {
            return; // operation aborted, don't signal
        }
    }

   if (handler)
   {
       Task* readTask = new Task();
       readTask->Connect(&TCPSocketHandler::OnReceive, handler, this, bytes_transferred, m_error);
       TaskThreadPool::Signal(readTask);
   }
}

void TCPSocket::HandleResolve(const boost::system::error_code& error, tcp::resolver::iterator endpoint_iterator, TCPSocketHandler* handler)
{
    m_error.Clear();
    if (error)
    {
        m_error.SetValue(error.value());
        if (boost::asio::error::operation_aborted == error)
        {
            return; // operation aborted, don't signal
        }

        // signal only in case of error
        if (handler)
        {
            Task* connectTask = new Task();
            connectTask->Connect(&TCPSocketHandler::OnConnect, handler, this, m_error);
            TaskThreadPool::Signal(connectTask);
        }
    }
    else
    {
        boost::asio::async_connect(m_socket, endpoint_iterator,
                                   boost::bind(&TCPSocket::HandleConnect, this, boost::asio::placeholders::error, handler));
    }
}

void TCPSocket::HandleConnect(const boost::system::error_code& error, TCPSocketHandler* handler)
{
    m_error.Clear();
    if (error)
    {
        m_error.SetValue(error.value());
        if (boost::asio::error::operation_aborted == error)
        {
            return; // operation aborted, don't signal
        }
    }

    if (handler)
    {
        Task* connectTask = new Task();
        connectTask->Connect(&TCPSocketHandler::OnConnect, handler, this, m_error);
        TaskThreadPool::Signal(connectTask);
    }
}

bool TCPSocket::IsOpen() const
{
    return m_socket.is_open();
}

