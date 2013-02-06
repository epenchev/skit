/*
 * HTTPServer.cpp
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
 *  Created on: Jan 22, 2013
 *      Author: emo
 */

/*
TODO make timeout for expire async_read_until in case no "\r\n\r\n" is send
TODO make timeout on async_write operations as well
TODO create HTTP header parser class
*/

#include "HttpServer.h"
#include "DataPacket.h"
#include <cstring>

#include "logging/logging.h"
using namespace ::logging;

namespace blitz {

MediaHTTPConnection::MediaHTTPConnection(boost::asio::io_service& io_service)
    : TCPConnection(io_service), m_connection_is_busy(false),
      m_connection_approved(false), state(STATE_CLOSED)
{  std::cout << "Create HTTP connection \n"; }

void MediaHTTPConnection::start(void)
{
    TCPConnection::start();

    // start HTTP connection
    if (isConnected() && socket().is_open())
    {
        state = STATE_OPEN;
        // notify our observers about new connection;
        notify();

        log::emit< Info>() << "MediaHTTPConnection::start() got connection from: " << getRemoteIP().to_string().c_str()
                                                << "from port: " << log::dec << getRemotePort() << log::endl;

        boost::asio::async_read_until(socket(), m_response, "\r\n\r\n",
                                      boost::bind(&MediaHTTPConnection::handleReadHeader, this,
                                         boost::asio::placeholders::error));
    }
}

void MediaHTTPConnection::close(void)
{
    TCPConnection::close();

    // clear packet queue
    m_packets.clear();

    state = STATE_CLOSED;

    // notify our observers about closing;
    notify();
}

void MediaHTTPConnection::handleReadHeader(const boost::system::error_code& error)
{
    if (!error)
    {
        std::istream request_stream(&m_response);

        std::string request;
        std::getline(request_stream, request);

        if (request.substr(0, 3) == "GET")
        {
            size_t slash_pos = request.find_first_of("/");

            if (slash_pos != std::string::npos)
            {
                size_t version_pos = request.find("HTTP/1.");

                if (version_pos != std::string::npos && version_pos > slash_pos)
                {
                    m_resource_id = request.substr(slash_pos + 1, (version_pos-2) - slash_pos);
                    log::emit< Trace>() << "MediaHTTPConnection::handleReadHeader resource requested: "
                                        <<  m_resource_id.c_str() << log::endl;
                }
            }
        }
        else
        {
            log::emit< Warning>() << "MediaHTTPConnection::handleReadHeader not a GET request: "
                                                    <<  request.c_str() << log::endl;
            return;
        }

        request.erase();
        std::getline(request_stream, request);

        if (!(request.substr(0, 5) == "Host:"))
        {
            log::emit< Warning>() << "MediaHTTPConnection::handleReadHeader no Host in header: "
                                                                <<  request.c_str() << log::endl;
            return;
        }

        log::emit< Trace>() << "MediaHTTPConnection::handleReadHeader Host line: "
                                                <<  request.c_str() << log::endl;

        // create HTTP response headers
        std::string response = "HTTP/1.0 200 OK\r\n";
        response = response  + "Server: blitz-stream \r\n";
        response = response +  "Content-type: mpeg/video \r\n\r\n";

        // send the HTTP headers for response
        boost::asio::async_write(socket(),
                                 boost::asio::buffer(response.c_str(), response.size()),
                                    boost::bind(&MediaHTTPConnection::handleWriteHeader, this,
                                       boost::asio::placeholders::error));
    }
    else // error
    {
        log::emit< Error>() << "MediaHTTPConnection::handleReadHeader "
                                                <<  error.message().c_str() << log::endl;
        close();
    }
}

void MediaHTTPConnection::handleWriteHeader(const boost::system::error_code& error)
{
    if (!error)
    {
        state = STATE_REQ_RECEIVED;
        // notify our observers about new request
        notify();
    }
    else if (error != boost::asio::error::operation_aborted)
    {
        log::emit< Error>() << "MediaHTTPConnection::handleWriteHeader "
                                                        <<  error.message().c_str() << log::endl;
        close();
    }
}

void MediaHTTPConnection::handleWriteContent(const boost::system::error_code& error)
{
    if (!error)
    {
        m_connection_is_busy = false;
        //PacketPtr ptr = m_packets.front();
        m_packets.pop_front();
        //ptr.reset();
    }
    else
    {
        log::emit< Error>() << "MediaHTTPConnection::handleWriteContent "
                                                    <<  error.message().c_str() << log::endl;
        close();
    }
}


void MediaHTTPConnection::addData(PacketPtr ptr)
{
    // add packet to queue
    m_packets.push_back(ptr);

    if (!m_connection_is_busy)
    {
        PacketPtr packet_ptr = m_packets.front();

        boost::asio::async_write(socket(), boost::asio::buffer(packet_ptr->data(), packet_ptr->size()),
                                 boost::bind(&MediaHTTPConnection::handleWriteContent, this,
                                    boost::asio::placeholders::error));

        m_connection_is_busy = true;
    }
}

MediaHTTPServer::MediaHTTPServer(boost::asio::io_service& io_service, const unsigned int tcp_port)
: TCPServer(io_service, tcp_port)
{}

TCPConnection* MediaHTTPServer::createTCPConnection(boost::asio::io_service& io_service)
{
    MediaHTTPConnection* connection = new MediaHTTPConnection(io_service);
    connection->attach(this);

    return connection;
}

void MediaHTTPServer::update(Subject* changed_subject)
{
    if (changed_subject)
    {
        MediaHTTPConnection* conn = NULL;
        try
        {
            conn = dynamic_cast<MediaHTTPConnection*>(changed_subject);
        }
        catch(std::exception& e)
        {
            log::emit< Error>() << "bad cast from MediaHTTPServer::update() " << e.what()  << log::endl;
            return;
        }

        MediaHTTPConnectionState conn_state = conn->getState();
        if (STATE_OPEN == conn_state)
        {
            log::emit< Info>() << "MediaHTTPServer::update() got connection from: " << conn->getRemoteIP().to_string().c_str()
                                      << "from port: " << log::dec << conn->getRemotePort() << log::endl;
        }
        else if (STATE_CLOSED == conn_state)
        {
            conn->setApproved(false);
            log::emit< Info>() << "MediaHTTPServer::update() got closing connection connection pool count: "
                                                             << log::dec << m_conn_pool.size() << log::endl;
        }
        else if (STATE_REQ_RECEIVED)
        {
            // check requested resource against a database
            // make connection approved if resource is OK in DB
            // else detach observer and close() connection.
            log::emit< Trace>() << "MediaHTTPServer::update() resource is:  " << conn->getResouceId().c_str() << log::endl;
            if (!conn->getApproved())
            {
                log::emit< Trace>() << "MediaHTTPServer::update() connection is not approved setting status to approved" << log::endl;
                conn->setApproved();
            }

        }
    }
}

void MediaHTTPServer::sendPacket(DataPacket* packet)
{
    if (packet)
    {
        PacketPtr packet_ptr(packet);

        if (m_conn_pool.empty())
        {
            return;
        }

        for (std::set<TCPConnection*>::iterator it = m_conn_pool.begin(); it != m_conn_pool.end(); ++it)
        {
            TCPConnection* connection = *it;

            MediaHTTPConnection* conn = NULL;
            try
            {
                conn = dynamic_cast<MediaHTTPConnection*>(connection);
            }
            catch(std::exception& e)
            {
                log::emit< Error>() << "bad cast from MediaHTTPServer::sendPacket() " << e.what()  << log::endl;
                return;
            }

            if (conn->isConnected())
            {
                if (conn->getApproved())
                    conn->addData(packet_ptr);
                else
                    log::emit< Error>() << "MediaHTTPServer::sendPacket erro connection not approved" << log::endl;
            }
            else if (STATE_CLOSED == conn->getState())
            {
                m_conn_pool.erase(conn);
                log::emit< Trace>() << "MediaHTTPServer::sendPacket() removing closed connection" << log::endl;
                delete conn;
            }
        }
    }
}

} // blitz

