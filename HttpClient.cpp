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

#include "logging/logging.h"
using namespace ::logging;

using boost::asio::ip::tcp;

namespace blitz {
namespace http {

void HTTPClient::connect(const std::string& server, const std::string& service)
{
    if ( !server.empty() && !service.empty() )
    {
        tcp::resolver resolver(m_sock.get_io_service());
        tcp::resolver::query query(server, service);

        try
        {
            tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
            boost::asio::connect(m_sock, endpoint_iterator);
        }
        catch(boost::system::system_error& e)
        {
            std::string errmsg = "HTTPClient::connect() error ";// + e.what();
            errmsg += e.what();
            throw std::runtime_error(errmsg);
        }

        m_server_name = server;
    }
    else
    {
        throw std::invalid_argument("HTTPClient::connect() invalid parameters");
    }
}

void HTTPClient::disconnect(void)
{
    if (m_sock.is_open())
    {
        boost::system::error_code error;
        m_sock.close(error);

        if (error)
        {
            std::string errmsg = "HTTPClient::connect() error " + error.message();
            throw std::runtime_error(errmsg);
        }
    }
    else
        throw std::runtime_error("HTTPClient::close() socket not connected");
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
        log::emit< Trace>() << "HTTPClient::sendReq()" << log::endl;

        boost::asio::async_write(m_sock, request,
                                 boost::bind(&HTTPClient::handleWriteHeader, this,
                                   boost::asio::placeholders::error,
                                     boost::asio::placeholders::bytes_transferred));
    }
    else {
        throw std::logic_error("HTTPClient::sendReq() socket not connected");
    }
}

// override this member to add own behavior
void HTTPClient::readContent(void)
{
    log::emit< Trace>() << "HTTPClient::readContent()" << log::endl;

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
        log::emit< Trace>() << "HTTPClient::handleWriteHeader()" << log::endl;

        boost::asio::async_read_until(m_sock, m_response, "\r\n\r\n",
                                      boost::bind(&HTTPClient::handleReadHeader, this,
                                        boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        std::string errorMessage = "Error writing HTTP request " + error.message();
        throw std::runtime_error(errorMessage);

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

    log::emit< Trace>() << "HTTPClient::handleReadHeader() start !!!" << log::endl;

    if (!error)
    {
        log::emit< Trace>() << "HTTPClient::handleReadHeader()" << log::endl;
        try
        {
            if (!bytes_transferred && !(boost::asio::buffer_size(m_response.data())))
            {
                throw std::runtime_error("HTTP Client Invalid header");
            }

            // Copy headers for future use
            m_header = boost::asio::buffer_cast<const char *>(m_response.data());

            // Check that response is OK.
            std::istream response_stream(&m_response);

            response_stream.exceptions( std::ios_base::eofbit | std::ios_base::failbit | std::ios_base::badbit);
            response_stream >> http_version;
            response_stream >> http_return_code;
            std::getline(response_stream, status_message);
        }
        catch(std::exception& e) {
            throw std::runtime_error("HTTP Client invalid response from server");
        }

        if (http_version.substr(0, 5) != "HTTP/") {
            throw std::runtime_error("HTTP Client Invalid header");
        }

        if (http_return_code != 200) {
            throw HTTPClientException(status_message.c_str(), http_return_code);
        }

        //start reading the data, virtual function
        this->readContent();
    }
    else
    {
        std::string errormsg = "Error reading HTTP response " + error.message();
        throw std::runtime_error(errormsg);
    }
}

void HTTPClient::handleReadContent(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if (!error)
    {
        if (bytes_transferred)
        {
            this->notify();
            this->readContent();
        }
    }
    else if (error != boost::asio::error::eof)
    {
        throw std::runtime_error(error.message());
    }
}

} // http
} // blitz
