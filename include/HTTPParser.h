/*
 * HTTPParser.h
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
 *  Created on: Mar 12, 2013
 *      Author: emo
 */

#ifndef HTTPPARSER_H_
#define HTTPPARSER_H_

#include <string>
#include <boost/noncopyable.hpp>
#include <vector>
#include <utility>
#include <map>

namespace blitz {

typedef std::pair<std::string, std::string> http_param;

class HttpCodeMap : public std::map<unsigned, std::string>
{
public:
    HttpCodeMap()
    {
        insert(std::make_pair(200, "OK"));
        insert(std::make_pair(206, "Partial Content"));
        insert(std::make_pair(401, "Unauthorized"));
        insert(std::make_pair(403, "Forbidden"));
        insert(std::make_pair(404, "Not Found"));
        insert(std::make_pair(405, "Method Not Allowed"));
        insert(std::make_pair(411, "Length Required"));
        insert(std::make_pair(505, "HTTP Version Not Supported"));
    }
} const static http_codes;

class MimeTypesMap : public std::map<std::string, std::string>
{
public:
    MimeTypesMap()
    {
        insert(std::make_pair("divx", "Content-Type: video/divx"));
        insert(std::make_pair("mp4", "Content-Type: video/mp4"));
        insert(std::make_pair("avi", "Content-Type: video/x-msvideo"));
        insert(std::make_pair("mpeg", "Content-Type: video/mpeg"));
        insert(std::make_pair("mkv", "Content-Type: video/x-matroska"));
        insert(std::make_pair("txt", "Content-Type: text/html"));
        insert(std::make_pair("flv", "Content-Type: video/x-flv"));
        insert(std::make_pair("mov", "Content-Type: video/quicktime"));
        insert(std::make_pair("wmv", "Content-Type: video/x-ms-wmv"));
    }
} const static mime_types;


/**
* Class representing generic HTTP message (Request/Response).
*/
class HTTPMessage
{
public:
     virtual ~HTTPMessage() {}

    /**
    * Return a reference to string with raw HTTP headers without parsing.
    * @return reference to std::string.
    */
    inline const std::string& getHeaders(void) const { return m_raw_headers; }

    /**
    * Read the HTTP message and returns true if message is valid, subclasses must implement this.
    */
    virtual bool isValid(std::string message) { return true; }

protected:
    HTTPMessage() {}
    std::string m_raw_headers; /**< HTTP raw headers. */
};

/**
* Class representing HTTP request message.
*/
class HTTPRequest : public HTTPMessage,
   private boost::noncopyable
{
public:
    HTTPRequest();
    virtual ~HTTPRequest() {}

    // from HTTPMessage
    virtual bool isValid(std::string message);

    /**
    * Return the type of the HTTP request (GET, POST, HEAD ...) as a string.
    */
    inline const std::string& getRequestType() const { return m_http_req; }

    /**
    * Return the requested resource as a string.
    */
    inline const std::string& getRequestedRes() const { return m_req_resource; }

    /**
    * Return the client identification string.
    */
    std::string getClientIdent();

    /**
    * Return parameters from request as pair(name, value) in vparam vector.
    */
    std::string getParams(std::vector<http_param>& vparam);

private :
    std::string m_http_req;
    std::string m_req_resource;
};

/**
* Class representing HTTP response message.
*/
class HTTPResponse : public HTTPMessage,
   private boost::noncopyable
{
public:
    HTTPResponse();
    virtual ~HTTPResponse() {}

    // from HTTPMessage
    virtual bool isValid(std::string message);

    /**
    * Return the HTTP response status code.
    */
    inline unsigned short getStatusCode(void) const { return m_status_code; }

    /**
    * Return the HTTP response status message.
    */
    inline const std::string& getStatusMessage(void) const { return m_status_message; }

    /**
    * Set the HTTP response status code.
    */
    void setStatusCode(unsigned int code);

    /**
    * Add self generated header line.
    */
    void header(const std::string& line);

    /**
    * Generate headers for streaming a file.
    */
    void stream(std::string filename = "", std::size_t size = 0);

    /**
    * Generate simple data headers with content type text/html
    */
    void data(unsigned int code, std::size_t size = 0);

private:
    // The HTTP response status code
    unsigned int m_status_code;

    // The HTTP response status message
    std::string m_status_message;
};

enum HTTPMsgType { HTTPRes, HTTPReq };

/**
* Class for parsing HTTP protocol messages.
*/
class HTTPParser :
     private boost::noncopyable
{
public:
    HTTPParser();
    virtual ~HTTPParser();

    /**
    * Read HTTP message of type T (HTTPRequest or HTTPResponse ) and returns true if is valid.
    * @param msg_buf: HTTP message headers.
    */
    bool isValid(boost::asio::streambuf& msg_buf, HTTPMsgType type);

    inline HTTPResponse& getResponse() { return (HTTPResponse&) getMessageObj(); }
    inline HTTPRequest& getRequest() { return (HTTPRequest&) getMessageObj(); }

private:
    /**
    * Return a reference to HTTPMessage object (Response/Request type) if HTTP message is parsed ok.
    * @return reference to HTTPMessage object.
    * @throws exception in case of invalid HTTP message or bad cast.
    */
    HTTPMessage& getMessageObj(void);

    HTTPMessage* m_msg;      /**< HTTP message object (Request or Response). */
    HTTPMsgType m_msg_type;
};

} // blitz

#endif /* HTTPPARSER_H_ */
