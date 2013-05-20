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

#include "MediaHttpServer.h"
#include "DataPacket.h"
#include "Log.h"
#include "HTTPParser.h"
#include <cstring>
#include <boost/lexical_cast.hpp>

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
}

void MediaHTTPConnection::start(void)
{
    TCPConnection::start();

    // start HTTP connection
    if (isConnected() && socket().is_open())
    {
        state = STATE_OPEN;
        /*
        notify our observers about new connection;
        notify();
        */

        BLITZ_LOG_INFO("got connection from: %s, from port:%d", getRemoteIP().c_str(), getRemotePort());

        // Set a deadline for receiving HTTP headers.
        m_io_control_timer.expires_from_now(boost::posix_time::seconds(MediaHTTPConnection::receive_timeout));

        boost::asio::async_read_until(socket(), m_response, "\r\n\r\n",
                                      boost::bind(&MediaHTTPConnection::handleReadHeader, this,
                                         boost::asio::placeholders::error));

        m_io_control_timer.async_wait(boost::bind(&MediaHTTPConnection::handleTimeoutOnSocket, this,
                                        boost::asio::placeholders::error));
    }
}

void MediaHTTPConnection::setApproved(bool status)
{
    bool send_headers = false;
    HTTPResponse http_response;

    // connection is approved send HTTP headers
    if (true == status)
    {
        if (!m_connection_approved)
        {
            // 200 OK
            send_headers = true;
            m_connection_approved = status;
            http_response.stream();
        }
    }
    else
    {
        if (!m_connection_approved && isConnected())
        {
            // Access denied
            send_headers = true;
            http_response.data(404);
            m_resource_id.clear(); // clear resource_id so no DB lookup is performed
        }
        m_connection_approved = status;
    }

    if (send_headers && isConnected())
    {
        boost::asio::async_write(socket(),boost::asio::buffer(http_response.getHeaders().c_str(), http_response.getHeaders().size()),
                                    boost::bind(&MediaHTTPConnection::handleWriteHeader, this,
                                   boost::asio::placeholders::error));
        // Set a deadline for sending HTTP headers.
        m_io_control_timer.expires_from_now(boost::posix_time::seconds(MediaHTTPConnection::send_timeout));

        m_io_control_timer.async_wait(boost::bind(&MediaHTTPConnection::handleTimeoutOnSocket, this,
                                        boost::asio::placeholders::error));
    }
}

void MediaHTTPConnection::handleTimeoutOnSocket(const boost::system::error_code& error)
{
    if (!error)
    {
        BLITZ_LOG_WARNING("Timeout on socket I/O closing ..");
        close();
    }
}

void MediaHTTPConnection::close(void)
{
    TCPConnection::close();

    state = STATE_CLOSED;

    // clear packet queue
    m_packets.clear();

    // notify our observers about closing;
    notify();
}

void MediaHTTPConnection::handleReadHeader(const boost::system::error_code& error)
{
    m_io_control_timer.cancel();

    if (!error)
    {
        HTTPParser parser;
        std::string response; // HTTP response headers

        if (parser.isValid(m_response, HTTPReq))
        {
            HTTPRequest& req = parser.getRequest();
            std::string req_type = req.getRequestType();
            if ("GET" == req_type)
            {
                m_resource_id = req.getRequestedRes();

                state = STATE_REQ_RECEIVED;

               // notify our observers about new request
               notify();
            }
            else
            {
                BLITZ_LOG_WARNING("not a GET request: %s", req_type.c_str());
                close();
                return;
            }
        }
        else
        {
            BLITZ_LOG_WARNING("Not a valid HTTP request");
            close();
            return;
        }
    }
    else if (boost::asio::error::operation_aborted != error) // error on socket, skip abort from timeout
    {
        BLITZ_LOG_ERROR("MediaHTTPConnection::handleReadHeader %s", error.message().c_str());
        close();
    }
}

void MediaHTTPConnection::handleWriteHeader(const boost::system::error_code& error)
{
    m_io_control_timer.cancel();

    if (error && boost::asio::error::operation_aborted != error)
    {
        BLITZ_LOG_ERROR("error: %s", error.message().c_str());
        close();
    }
    else if (!m_connection_approved)
    {
        close();
    }
}

void MediaHTTPConnection::handleWriteContent(const boost::system::error_code& error)
{
    m_io_control_timer.cancel();

    if (!error)
    {
        // flush the rest of the queue
        if (!m_packets.empty())
        {
            m_io_control_timer.expires_from_now(boost::posix_time::seconds(MediaHTTPConnection::send_timeout));

            PacketPtr packet_ptr = m_packets.front();
            m_packets.pop_front();

            boost::asio::async_write(socket(), boost::asio::buffer(packet_ptr->data(), packet_ptr->size()),
                                     boost::bind(&MediaHTTPConnection::handleWriteContent, this,
                                        boost::asio::placeholders::error));

            m_io_control_timer.async_wait(boost::bind(&MediaHTTPConnection::handleTimeoutOnSocket, this,
                                                    boost::asio::placeholders::error));
        }
        else
        {
            m_connection_is_busy = false;
        }
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
    // Disable buffer fill
    if (m_packets.size() < 20)
        m_packets.push_back(ptr);

    if (!m_connection_is_busy)
    {
        m_connection_is_busy = true;

        PacketPtr packet_ptr = m_packets.front();
        m_packets.pop_front();

        m_io_control_timer.expires_from_now(boost::posix_time::seconds(MediaHTTPConnection::send_timeout));

        boost::asio::async_write(socket(), boost::asio::buffer(packet_ptr->data(), packet_ptr->size()),
                                 boost::bind(&MediaHTTPConnection::handleWriteContent, this,
                                    boost::asio::placeholders::error));

        m_io_control_timer.async_wait(boost::bind(&MediaHTTPConnection::handleTimeoutOnSocket, this,
                                        boost::asio::placeholders::error));
    }

    if ((DataPacket::max_size * m_packets.size()) >= MediaHTTPConnection::max_queue_size)
    {
        BLITZ_LOG_WARNING("queue has reached max size: %d", DataPacket::max_size * m_packets.size());
    }
}


MediaHTTPServer::MediaHTTPServer(boost::asio::io_service& io_service, const unsigned int tcp_port, MediaSessionDB& db)
: TCPServer(io_service, tcp_port), m_activity_timer(io_service), m_dbase(db)
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

#define SERVICE_ACTIVCE

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

        if (STATE_CLOSED == conn_state)
        {
#if defined SERVICE_ACTIVCE
            // inform DB for connection close.
            if (!conn->getResouceId().empty())
            {
                blitz::record_id rec_id;
                MediaSessionDB::Record rec;
                std::stringstream hex_converter;

                hex_converter << std::hex << conn->getResouceId();
                hex_converter >> rec_id;

                if (m_dbase.SelectRecord(rec_id, rec))
                {
                    rec.ip_address = "";
                    rec.livetime_sec = 0;
                    m_dbase.UpdateRecord(rec);
                }
            }
#endif
            // TODO must not conn->setApproved() this in this state
            conn->setApproved(false);

            BLITZ_LOG_INFO("got closing connection, connection pool count: %d", m_conn_pool.size() - 1);

            // remove connection from pool
            m_conn_pool.erase(conn);

            // set to be destroyed later from sendPacket() call
            m_orphane_connections.push_back(conn);

        }
        else if (STATE_REQ_RECEIVED)
        {
            BLITZ_LOG_INFO("resource is: %s", conn->getResouceId().c_str());
#if defined SERVICE_ACTIVCE
            if (!conn->getResouceId().empty())
            {
                blitz::record_id rec_id;
                MediaSessionDB::Record rec;
                std::stringstream hex_converter;

                hex_converter << std::hex << conn->getResouceId();
                hex_converter >> rec_id;

                if (m_dbase.SelectRecord(rec_id, rec))
                {
                    BLITZ_LOG_INFO("Session id: %s is present in DB", conn->getResouceId().c_str());
                    if (!rec.ip_address.empty())
                    {
                        BLITZ_LOG_WARNING("Session id: %s is already connected", conn->getResouceId().c_str());
                        conn->setApproved(false);
                    }
                    else
                    {
                        rec.ip_address = conn->getRemoteIP().c_str();
                        m_dbase.UpdateRecord(rec);
                        conn->setApproved(true);
                    }
                }
                else
                {
                    BLITZ_LOG_WARNING("No such id: %s is present in DB !!!", conn->getResouceId().c_str());
                    conn->setApproved(false);
                }
            }
            else
            {
                BLITZ_LOG_WARNING("No resource ID present");
                conn->setApproved(false);
            }
#else
            if (!conn->getApproved())
            {
                BLITZ_LOG_INFO("connection is not approved setting status to approved");
                conn->setApproved(true);
            }
#endif

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
    //m_activity_timer.cancel();

    if (packet)
    {
        PacketPtr packet_ptr(packet);

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

    /*
    m_activity_timer.expires_from_now(boost::posix_time::seconds(MediaHTTPServer::no_data_timeout));
    m_activity_timer.async_wait(boost::bind(&MediaHTTPServer::handleDeadline, this,
                                    boost::asio::placeholders::error));
    */
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
            //delete conn; REMOVE
        }
        m_conn_pool.clear();

        // remove orphaned connections
        clearConnections();
    }
}

std::string MediaHTTPServer::getConnectedPeers()
{
    std::string peers;
    peers.clear();

    for (std::set<TCPConnection*>::iterator it = m_conn_pool.begin(); it != m_conn_pool.end(); ++it)
    {
        TCPConnection* connection = *it;
        peers += connection->getRemoteIP().c_str();
        peers += "\n";
    }
    return peers;
}


void MediaHTTPServer::closeSession(blitz::record_id rec_id)
{
    for (std::set<TCPConnection*>::iterator it = m_conn_pool.begin(); it != m_conn_pool.end(); ++it)
    {
        MediaHTTPConnection* http_conn = NULL;
        TCPConnection* connection = *it;

        try
        {
            blitz::record_id session_id;
            std::stringstream hex_converter;

            http_conn = dynamic_cast<MediaHTTPConnection*>(connection);

            hex_converter << std::hex << http_conn->getResouceId();
            hex_converter >> session_id;

            if (session_id == rec_id)
            {
                http_conn->close();
                break;
            }
        }
        catch(std::exception& e)
        {
            BLITZ_LOG_ERROR("bad cast exception: %s", e.what());
            return;
        }
    }
}

} // blitz

