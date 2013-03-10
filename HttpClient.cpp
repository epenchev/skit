/*
 * HTTPClient.cpp
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
 *  Created on: Jan 17, 2013
 *      Author: emo
 */

#include <ios>
#include <iostream>
#include <exception>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "HttpClient.h"
#include "Log.h"

using boost::asio::ip::tcp;

namespace blitz {
namespace http {

HTTPClient::HTTPClient(boost::asio::io_service& io_service)
: m_sock(io_service), state(STATE_DISCONNECT), m_io_control_timer(io_service)
{}

void HTTPClient::connect(const std::string& server, const std::string& service)
{
    if ( server.empty() || service.empty() )
    {
        throw std::invalid_argument("HTTPClient::connect() invalid parameters");
    }

    m_server_name = server;
    BLITZ_LOG_INFO("Connecting to server: %s", server.c_str());

    boost::system::error_code err;
    boost::asio::ip::address ip_address = boost::asio::ip::address::from_string(server, err);

    if (err)
    {
        BLITZ_LOG_INFO("Server name is not IP address");

        tcp::resolver resolver(m_sock.get_io_service());
        tcp::resolver::query query(server, service);

        resolver.async_resolve(query, boost::bind(&HTTPClient::handleResolve, this,
                                  boost::asio::placeholders::error,
                                         boost::asio::placeholders::iterator));
    }
    else
    {
        int service_port = 0;

        if (!service.compare("http"))
        {
            service_port = 80;
        }
        else
        {
            service_port = atoi(service.c_str());
        }

        boost::asio::ip::tcp::endpoint endpoint(ip_address, (unsigned short)service_port);
        m_sock.async_connect(endpoint, boost::bind(&HTTPClient::handleConnect, this,
                                boost::asio::placeholders::error));
    }
}

void HTTPClient::handleResolve(const boost::system::error_code& err, tcp::resolver::iterator endpoint_iterator)
{
    if (!err)
    {
        boost::asio::async_connect(m_sock, endpoint_iterator,
                                   boost::bind(&HTTPClient::handleConnect, this,
                                       boost::asio::placeholders::error));
    }
    else
    {
        BLITZ_LOG_ERROR("Error resolving: %s", err.message().c_str());
        // notify observer for this error
        notify();
    }
}

void HTTPClient::handleConnect(const boost::system::error_code& err)
{
    if (!err)
    {
        state = STATE_CONNECT;
        BLITZ_LOG_INFO("Connection established");
        notify();
    }
    else
    {
        BLITZ_LOG_INFO("Error connecting: %s", err.message().c_str());
        // notify observer for this error
        notify();
    }
}

void HTTPClient::disconnect(void)
{
    if (m_sock.is_open())
    {
        boost::system::error_code err;
        m_sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both, err);
        m_sock.close(err);
        state = STATE_DISCONNECT;
        BLITZ_LOG_INFO("Connection terminated");
        notify();
    }
}

void HTTPClient::sendReq(std::string req, const std::string& resource)
{
    boost::asio::streambuf request;
    std::ostream request_stream(&request);

    request_stream << req << " " << resource << " HTTP/1.1\r\n";
    request_stream << "Host: " << m_server_name << "\r\n";
    request_stream << "Accept: */*\r\n\r\n";

    if (m_sock.is_open())
    {
        boost::asio::async_write(m_sock, request,
                                 boost::bind(&HTTPClient::handleWriteHeader, this,
                                   boost::asio::placeholders::error,
                                     boost::asio::placeholders::bytes_transferred));
        BLITZ_LOG_INFO("HTTP request send");
    }
    else
    {
        throw std::logic_error("HTTPClient::sendReq() socket not connected");
    }
}

// override this member to add own behavior
void HTTPClient::readContent(void)
{
    BLITZ_LOG_INFO("executed");

    boost::asio::async_read(m_sock, m_response,
                             boost::asio::transfer_at_least(1),
                                  boost::bind(&HTTPClient::handleReadContent, this,
                                      boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
}

void HTTPClient::handleWriteHeader(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if (!error)
    {
        BLITZ_LOG_INFO("HTTPClient::handleWriteHeader()");

        boost::asio::async_read_until(m_sock, m_response, "\r\n\r\n",
                                      boost::bind(&HTTPClient::handleReadHeader, this,
                                        boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        BLITZ_LOG_ERROR("Error writing HTTP request: %s", error.message().c_str());
        disconnect();
    }
}

boost::asio::streambuf& HTTPClient::getContent()
{
    return m_response;
}

void HTTPClient::handleReadHeader(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    std::string status_message;
    std::string http_version;
    unsigned http_return_code;

    if (!error)
    {
        try
        {
            if (!bytes_transferred && !(boost::asio::buffer_size(m_response.data())))
            {
                BLITZ_LOG_ERROR("Invalid header");
                disconnect();
            }

            // Copy headers for future use
            m_header = boost::asio::buffer_cast<const char *>(m_response.data());

            BLITZ_LOG_INFO("HTTP Header: \n %s \n", m_header.c_str());

            // Check that response is OK.
            std::istream response_stream(&m_response);

            response_stream.exceptions( std::ios_base::eofbit | std::ios_base::failbit | std::ios_base::badbit);
            response_stream >> http_version;
            response_stream >> http_return_code;
            std::getline(response_stream, status_message);
        }
        catch(std::exception& e)
        {
            BLITZ_LOG_ERROR("HTTP Client invalid response from server exception is %s", e.what());
            disconnect();
        }

        if (http_version.substr(0, 5) != "HTTP/")
        {
            BLITZ_LOG_ERROR("No HTTP field in header");
            disconnect();
        }

        if (http_return_code != 200)
        {
            BLITZ_LOG_ERROR("Error from server: %s, HTTP error code:%d ", status_message.c_str(), http_return_code);
            disconnect();
        }

        // release/clear the response buffer for future use
        m_response.consume(bytes_transferred);

        //start reading the data, virtual function
        readContent();
    }
    else
    {
        BLITZ_LOG_ERROR("Error reading HTTP response, %s", error.message().c_str());
        disconnect();
    }
}

void HTTPClient::handleReadContent(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if (!error)
    {
        if (bytes_transferred)
        {
            notify();
            readContent();
        }
    }
    else if (error != boost::asio::error::eof)
    {
        throw std::runtime_error(error.message());
    }
}

} // http
} // blitz
