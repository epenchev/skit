/*
 * TCPAcceptor.cpp
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

#include "TCPAcceptor.h"
#include "TaskThread.h"
#include "Task.h"
#include "utils/Logger.h"
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

#define LOG_DISABLE

TCPAcceptor::TCPAcceptor(unsigned short port)
 : m_acceptor(TaskThreadPool::GetEventThread().mIOServiceLoop),
   m_localPort(port), m_localAddress(""), m_isListening(false), m_iohandler(NULL)
{}

TCPAcceptor::TCPAcceptor(std::string localAdress, unsigned short port)
 : m_acceptor(TaskThreadPool::GetEventThread().mIOServiceLoop),
   m_localPort(port), m_localAddress(localAdress), m_isListening(false), m_iohandler(NULL)
{}

TCPAcceptor::~TCPAcceptor()
{
    Stop();
}

void TCPAcceptor::Listen()
{
    ErrorCode outError;
    try
    {
        m_acceptor.open(boost::asio::ip::tcp::v4());
        if (!m_localAddress.empty())
        {
            boost::asio::ip::address bindAdress;
            bindAdress.from_string(m_localAddress);
            m_acceptor.bind(tcp::endpoint(bindAdress, m_localPort));
        }
        else
        {
            m_acceptor.bind(tcp::endpoint(tcp::v4(), m_localPort));
        }
        m_acceptor.set_option(tcp::acceptor::reuse_address(true));
        m_acceptor.listen();
    }
    catch(boost::system::system_error& ex)
    {
        outError.SetMessage(ex.code().message());
        throw SystemException(outError);

    };
    m_isListening = true;
}

void TCPAcceptor::Start()
{
    ErrorCode outError;
    if (!m_iohandler || !m_isListening)
    {
    	outError = true;
        throw SystemException(outError);
    }
    Accept();
}

void TCPAcceptor::Stop()
{
    if (m_isListening)
    {
        // no error handling here, nobody cares
        boost::system::error_code error;
        m_acceptor.close(error);
        m_isListening = false;
    }
}


void TCPAcceptor::Accept()
{
    TCPSocket* clientSocket = new TCPSocket();
    m_acceptor.async_accept(clientSocket->m_socket,
                            boost::bind(&TCPAcceptor::HandleAccept,
                            this,
                            clientSocket,
                            boost::asio::placeholders::error));
}

unsigned short TCPAcceptor::GetListeningPort() const
{
    return m_acceptor.local_endpoint().port();
}

void TCPAcceptor::SetHandler(TCPAcceptorHandler* handler)
{
    m_iohandler = handler;
}

void TCPAcceptor::HandleAccept(TCPSocket* outSocket, const boost::system::error_code& error)
{
    m_error = false;
    LOG(logDEBUG) << "Accepted connection";
    if (error)
    {
    	m_error.SetMessage(error.message());
        delete outSocket;
        outSocket = NULL;
        if (boost::asio::error::operation_aborted == error)
        {
            LOG(logDEBUG) << "Aborting";
            return; // operation aborted, don't signal
        }
    }

    /* warning, if m_error is set socket is invalid object */
    if (m_iohandler)
    {
        Task* acceptTask = new Task();
        acceptTask->Connect(&TCPAcceptorHandler::OnAccept, m_iohandler, outSocket, m_error);
        TaskThreadPool::Signal(acceptTask);
    }

    /* continue accepting new connections. */
    Accept();
}


