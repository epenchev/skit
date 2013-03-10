/*
 * MediaHttpServer.cpp
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

#include "MediaHttpServer.h"
#include "DataPacket.h"
#include "Log.h"
#include <cstring>

namespace blitz {

MediaHTTPConnection::MediaHTTPConnection(boost::asio::io_service& io_service)
    : TCPConnection(io_service), m_connection_is_busy(false),
      m_connection_approved(false), state(STATE_CLOSED), m_io_control_timer(io_service)
{
    BLITZ_LOG_INFO("Create HTTP connection");
}


MediaHTTPConnection::~MediaHTTPConnection()
{
    BLITZ_LOG_INFO("Delete MediaHTTPConnection");

    // clear packet queue
    if (!m_packets.empty())
    {
        m_packets.clear();
    }
}

void MediaHTTPConnection::start(void)
{
    TCPConnection::start();

    // start HTTP connection
    if (isConnected() && socket().is_open())
    {
        state = STATE_OPEN;
        // notify our observers about new connection;
        notify();

        BLITZ_LOG_INFO("got connection from: %s, from port:%d", getRemoteIP().to_string().c_str(), getRemotePort());

        // Set a deadline for receiving HTTP headers.
        m_io_control_timer.expires_from_now(boost::posix_time::seconds(MediaHTTPConnection::receive_timeout));

        boost::asio::async_read_until(socket(), m_response, "\r\n\r\n",
                                      boost::bind(&MediaHTTPConnection::handleReadHeader, this,
                                         boost::asio::placeholders::error));

        m_io_control_timer.async_wait(boost::bind(&MediaHTTPConnection::handleTimeoutReceive, this,
                                        boost::asio::placeholders::error));
    }
}

void MediaHTTPConnection::handleTimeoutReceive(const boost::system::error_code& error)
{
    if (!error)
    {
        BLITZ_LOG_WARNING("Timeout receiving data closing HTTP session");
        close();
    }
}

void MediaHTTPConnection::handleTimeoutSend(const boost::system::error_code& error)
{
    if (!error)
    {
        BLITZ_LOG_WARNING("Timeout sending data over connection closing ...");
        close();
    }
}


void MediaHTTPConnection::close(void)
{
    TCPConnection::close();

    state = STATE_CLOSED;

    // notify our observers about closing;
    notify();
}

void MediaHTTPConnection::handleReadHeader(const boost::system::error_code& error)
{
    m_io_control_timer.cancel();

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
                    BLITZ_LOG_INFO("resource requested: %s", m_resource_id.c_str());
                }
            }
        }
        else
        {
            BLITZ_LOG_WARNING("not a GET request: %s", request.c_str());
            return;
        }

        request.erase();
        std::getline(request_stream, request);

        if (!(request.substr(0, 5) == "Host:"))
        {
            BLITZ_LOG_WARNING("No (Host:) in header present: %s",request.c_str());
            return;
        }

        BLITZ_LOG_INFO("Host line: %s", request.c_str());

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
        BLITZ_LOG_ERROR("MediaHTTPConnection::handleReadHeader %s", error.message().c_str());
        close();
    }
}

void MediaHTTPConnection::handleWriteHeader(const boost::system::error_code& error)
{
    m_io_control_timer.cancel();

    if (!error)
    {
        state = STATE_REQ_RECEIVED;

        // notify our observers about new request
        notify();
    }
    else
    {
        BLITZ_LOG_ERROR("error: %s", error.message().c_str());
        close();
    }
}

void MediaHTTPConnection::handleWriteContent(const boost::system::error_code& error)
{
    m_io_control_timer.cancel();

    if (!error)
    {
        m_connection_is_busy = false;
        m_packets.pop_front();
    }
    else if (boost::asio::error::operation_aborted == error)
    {
        BLITZ_LOG_WARNING("Timeout sending data to connection");
    }
    else
    {
        BLITZ_LOG_ERROR("Terminating connection with error: %s", error.message().c_str());
        close();
    }
}

void MediaHTTPConnection::addData(PacketPtr ptr)
{
    // add packet to queue
    m_packets.push_back(ptr);

    if (!m_connection_is_busy)
    {
        m_connection_is_busy = true;
        PacketPtr packet_ptr = m_packets.front();

        m_io_control_timer.expires_from_now(boost::posix_time::seconds(MediaHTTPConnection::send_timeout));

        boost::asio::async_write(socket(), boost::asio::buffer(packet_ptr->data(), packet_ptr->size()),
                                 boost::bind(&MediaHTTPConnection::handleWriteContent, this,
                                    boost::asio::placeholders::error));

        m_io_control_timer.async_wait(boost::bind(&MediaHTTPConnection::handleTimeoutSend, this,
                                        boost::asio::placeholders::error));
    }
    else if (m_packets.size() >= MediaHTTPConnection::max_queue_size)
    {
        BLITZ_LOG_WARNING("packet queue size %d, remove packet", m_packets.size());
        m_packets.pop_back();
    }
}


MediaHTTPServer::MediaHTTPServer(boost::asio::io_service& io_service, const unsigned int tcp_port)
: TCPServer(io_service, tcp_port), m_activity_timer(io_service)
{}

/*
MediaHTTPServer::MediaHTTPServer(boost::asio::io_service& io_service, const unsigned int tcp_port)
: TCPServer(io_service, tcp::endpoint(boost::asio::ip::address_v4::from_string("192.168.1.107"), tcp_port)),
  m_activity_timer(io_service)
{}
*/

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
            BLITZ_LOG_ERROR("bad cast exception: %s", e.what());
            return;
        }

        MediaHTTPConnectionState conn_state = conn->getState();
        if (STATE_OPEN == conn_state)
        {
            // TODO attach ACL module here in the future
            // TODO set timer for connection.
            BLITZ_LOG_INFO("got connection from: %s from port: %d",
                            conn->getRemoteIP().to_string().c_str(), conn->getRemotePort());
        }
        else if (STATE_CLOSED == conn_state)
        {
            // TODO inform DB for connection close.
            conn->setApproved(false);
            BLITZ_LOG_INFO("got closing connection, connection pool count: %d", m_conn_pool.size() - 1);

            // remove connection from pool
            m_conn_pool.erase(conn);

            // set to be destroyed later from sendPacket() call
            m_orphane_connections.push_back(conn);

        }
        else if (STATE_REQ_RECEIVED)
        {
            // TODO check requested resource against a database
            // make connection approved if resource is OK in DB
            // else detach observer and close() connection.
            BLITZ_LOG_INFO("resource is: %s", conn->getResouceId().c_str());
            if (!conn->getApproved())
            {
                BLITZ_LOG_INFO("connection is not approved setting status to approved");
                conn->setApproved();
            }

        }
    }
}

void MediaHTTPServer::clearConnections(void)
{
    if (!m_orphane_connections.empty())
    {
        for (std::vector<MediaHTTPConnection*>::iterator it = m_orphane_connections.begin();
                                                    it != m_orphane_connections.end(); ++it)
        {
            MediaHTTPConnection* conn = *it;
            delete conn;
        }
        m_orphane_connections.clear();
    }
}

void MediaHTTPServer::sendPacket(DataPacket* packet)
{
    m_activity_timer.cancel();

    if (packet)
    {
        PacketPtr packet_ptr(packet);

        /*
        if (m_conn_pool.empty())
        {
            return;
        }
        */

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
                BLITZ_LOG_ERROR("bad cast exception: %s", e.what());
                return;
            }

            if (conn->isConnected())
            {
                if (conn->getApproved())
                {
                    conn->addData(packet_ptr);
                }
                else
                {
                    // will flood our logs
                    //BLITZ_LOG_ERROR("connection not approved");
                }
            }
        }
    }

    // remove orphaned connections
    clearConnections();

    m_activity_timer.expires_from_now(boost::posix_time::seconds(MediaHTTPServer::no_data_timeout));
    m_activity_timer.async_wait(boost::bind(&MediaHTTPServer::handleDeadline, this,
                                    boost::asio::placeholders::error));
}

void MediaHTTPServer::handleDeadline(const boost::system::error_code& error)
{
    if (!error)
    {
        BLITZ_LOG_WARNING("No data from source closing connections");

        if (m_conn_pool.empty())
        {
            BLITZ_LOG_WARNING("No connections in server's pool exit from timeout handler");
            return;
        }

        for (std::set<TCPConnection*>::iterator it = m_conn_pool.begin(); it != m_conn_pool.end(); ++it)
        {
            TCPConnection* conn = *it;
            // will inform observer in this case us.
            conn->close();
            delete conn;
        }
        m_conn_pool.clear();
    }
}

} // blitz

