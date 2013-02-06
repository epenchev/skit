/*
 * TcpServer.cpp
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
 *  Created on: Jan 30, 2013
 *      Author: emo
 */

#include "TcpServer.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "logging/logging.h"

using namespace ::logging;

namespace blitz {

void TCPConnection::start(void)
{
    if (m_sock.is_open())
    {
        m_connected = true;
    }
}

void TCPConnection::close(void)
{
    if (m_connected && m_sock.is_open())
    {
        boost::system::error_code err;
        m_sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both, err);
        if (err)
            log::emit< Error>() << "Error TCPConnection::close() in shutdown() " << err.message().c_str() << log::endl;
        m_sock.close(err);
        if (err)
            log::emit< Error>() << "Error TCPConnection::close() in close() " << err.message().c_str() << log::endl;
        m_connected = false;
    }
}

TCPServer::TCPServer(boost::asio::io_service& io_service, const boost::asio::ip::tcp::endpoint& endpoint)
          : m_tcp_acceptor(io_service, tcp::endpoint(endpoint)),
            m_endpoint(endpoint),
            m_is_listening(false) {}

TCPServer::TCPServer(boost::asio::io_service& io_service, const unsigned int tcp_port)
          : m_tcp_acceptor(io_service, tcp::endpoint(tcp::v4(), tcp_port)),
            //m_endpoint(boost::asio::ip::tcp::v4(), tcp_port),
            m_is_listening(false) {}


void TCPServer::start(void)
{
    if (!m_is_listening)
    {
        log::emit< Trace>() << "TCPServer::start() " << log::endl;

        /*
        try
        {

            m_tcp_acceptor.open(m_endpoint.protocol());

            // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
            //m_tcp_acceptor.set_option(tcp::acceptor::reuse_address(true));

            m_tcp_acceptor.bind(m_endpoint);
            if (0 == m_endpoint.port())
            {
                // update the endpoint to reflect the port chosen by bind
                m_endpoint = m_tcp_acceptor.local_endpoint();
            }
            m_tcp_acceptor.listen();
            log::emit< Info>() << "Listening on port: "
                                  << log::dec << m_endpoint.port() << log::endl;
        }
        catch (std::exception& e)
        {
            log::emit< Error>() << "Unable to bind to port:" << log::dec << m_endpoint.port() << "\n" << e.what() << log::endl;
            throw;
        }
        */
        m_is_listening = true;

        handleStartServer();

        accept();
    }
}

void TCPServer::stop(void)
{
    if (m_is_listening)
    {
        log::emit< Info>() << "Shutting down server on port " << log::dec << m_endpoint.port() << log::endl;

        m_is_listening = false;

        // this terminates any connections waiting to be accepted
        m_tcp_acceptor.close();

        // this terminates open connections and removes them from pool
        std::set<TCPConnection*>::iterator conn_itr = m_conn_pool.begin();
        for ( ; conn_itr != m_conn_pool.end(); ++conn_itr )
        {
            log::emit< Info>() << "TCPServer::stop() closing connection on port " << log::dec << m_endpoint.port() << log::endl;
            (*conn_itr)->close();
            m_conn_pool.erase(conn_itr);
        }

        // call the virtual function of the successor
        handleStopServer();
    }
}

void TCPServer::accept(void)
{
    if (m_is_listening)
    {
        // call the virtual function of the successor so a specific type of TCP connection can be created
        TCPConnection* new_connection = createTCPConnection(m_tcp_acceptor.get_io_service());

        m_tcp_acceptor.async_accept(new_connection->socket(),
            boost::bind(&TCPServer::handleAccept, this, new_connection, boost::asio::placeholders::error)
        );
    }
}

void TCPServer::handleAccept(TCPConnection* new_connection,
                                const boost::system::error_code& error)
{
    if (!error)
    {
        m_conn_pool.insert(new_connection);
        new_connection->start();
    }
    else
    {
        log::emit< Error>() << "Error TCPServer::handleAccept() " << error.message().c_str() << log::endl;
        throw std::runtime_error(error.message());
    }

    // continue accepting new connections
    accept();
}

} // blitz
