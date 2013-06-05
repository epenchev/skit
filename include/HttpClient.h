/*
 * HttpClient.h
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

#ifndef HTTPCLIENT_H_
#define HTTPCLIENT_H_

#include <string>
#include <exception>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include "Observer.h"
#include "DataSource.h"

using boost::asio::ip::tcp;
using boost::asio::deadline_timer;

namespace blitz {
namespace http {

class HTTPClientException : public std::exception
{
public:
    HTTPClientException(const char* error_message, unsigned err_code) : m_status_message(error_message),
                                                                             m_error_code(err_code)
    {}
    virtual const char* what() { return m_status_message; }
    unsigned errorCode() { return m_error_code; }
private:
    const char* m_status_message;
    unsigned m_error_code;
};

enum HTTPClientState { STATE_CONNECT, STATE_DISCONNECT, STATE_DATARECV };

/**
* HTTP Client.
* Easy to use HTTP protocol handler for client connections.
* Uses boost asio for socket communication API.
* Establishes only one active connection.
*/
class HTTPClient : private boost::noncopyable , public Subject
{
public:
    HTTPClient(boost::asio::io_service& io_service);
    virtual ~HTTPClient() {}

    /**
    * Establishes network connection with server.
    * @param server: name of the server to connect.
    * @param service: service name or port to connect.
    * @throws exception in case of error.
    */
    void connect(const std::string& server, const std::string& service);

    /**
    * Closes current connection.
    */
    void disconnect();

    /**
    * Performs GET request for resource.
    * @param req: HTTP request (GET, POST ..)
    * @param resource: name of resource.
    */
    void sendReq(const std::string req, const std::string& resource);

    /**
    * Returns the current state of the HTTPClient concerning observers
    */
    inline HTTPClientState getState(void) const { return state; }

    boost::asio::streambuf& getContent();

    std::string& getHeaders();

protected:
     virtual void readContent();
     tcp::socket m_sock;         /**< socket object */
     std::string m_server_name;   /**< Server name used to form HTTP request as of HTTP 1.1 */
     std::string m_header;       /**< Storage for the HTTP headers returned from server  */
     boost::asio::streambuf m_response;
     HTTPClientState state;
     deadline_timer m_io_control_timer;

private:
     // callbacks for the boost socket operations
     void handleWriteHeader(const boost::system::error_code& error, std::size_t bytes_transferred);
     void handleReadHeader(const boost::system::error_code& error, std::size_t bytes_transferred);
     void handleReadContent(const boost::system::error_code& error, std::size_t bytes_transferred);
     void handleResolve(const boost::system::error_code& err, tcp::resolver::iterator endpoint_iterator);
     void handleConnect(const boost::system::error_code& err);
};

} // http
} // blitz

#endif /* HTTPCLIENT_H_ */
