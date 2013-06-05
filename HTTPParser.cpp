/*
 * HTTPParser.cpp
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

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <string>
#include <list>
#include "HTTPParser.h"
#include "Log.h"
#include <cstdio>
#include <ctime>

namespace blitz {

const std::string http_version_1_0 = "HTTP/1.0";
const std::string http_version_1_1 = "HTTP/1.1";
const std::string http_crlf = "\r\n";
const std::string http_end_headers = "\r\n\r\n";
const std::string server_name = "blitz-stream";


inline bool is_special(int c)
{
    switch (c)
    {
        case '(': case ')': case '<': case '>': case '@':
        case ',': case ';': case ':': case '\\': case '"':
        case '/': case '[': case ']': case '?': case '=':
        case '{': case '}': case ' ': case '\t':
            return true;
        default:
            return false;
    }
}

const std::string currentDateTime()
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);

    /* Visit http://www.cplusplus.com/reference/clibrary/ctime/strftime/
    * for more information about date/time format
    * strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    */
    strftime(buf, sizeof(buf), "%a, %d %b %g %T GMT", &tstruct);

    return buf;
}

std::string getFileExtension(const std::string& filename)
{
    if (filename.find_last_of(".") != std::string::npos)
        return filename.substr(filename.find_last_of(".")+1);
    return "";
}

HTTPParser::HTTPParser() : m_msg(NULL)
{}

HTTPParser::~HTTPParser()
{ delete m_msg; }


HTTPMessage& HTTPParser::getMessageObj()
{
    if (!m_msg)
    {
        throw std::runtime_error("Undefined HTTP Parser");
    }

    return *m_msg;
}

bool HTTPParser::isValid(boost::asio::streambuf& msg_buf, HTTPMsgType type)
{
    bool is_valid = false;
    m_msg_type = type;

    boost::asio::streambuf::const_buffers_type const_buf = msg_buf.data();
    const char* char_buf = boost::asio::buffer_cast<const char*>(const_buf);
    std::string message(char_buf);

    //std::string str(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + buf_size);

    if (HTTPRes == type)
    {
        m_msg = new blitz::HTTPResponse();
    }
    else if (HTTPReq == type)
    {
        m_msg = new blitz::HTTPRequest();
    }

    if (m_msg)
    {
        is_valid = m_msg->isValid(message);
        if (!is_valid)
        {
            delete m_msg;
            m_msg = NULL;
        }
    }

    return is_valid;
}

HTTPRequest::HTTPRequest() : HTTPMessage()
{}

bool HTTPRequest::isValid(std::string message)
{
    bool is_valid = false;
    std::string msg_line;

    // HTTP Fields
    std::string client_ident;
    std::string host_id;

    if (message.empty())
    {
        return is_valid;
    }

    // copy headers
    m_raw_headers = message;

    std::istringstream iss(message);
    std::getline(iss, msg_line);

    // get HTTP request
    m_http_req = msg_line.substr(0, 3);

    // find first slash after request
    size_t slash_pos = msg_line.find_first_of("/");

    if (slash_pos != std::string::npos)
    {
        size_t version_pos = message.find("HTTP/1.");
        if (version_pos != std::string::npos && version_pos > slash_pos)
        {
            m_req_resource = msg_line.substr(slash_pos + 1, (version_pos-2) - slash_pos);
        }
    }
    else
    {
        BLITZ_LOG_ERROR("Unable to find first slash");
        return is_valid;
    }

    // get next line
    msg_line.erase();
    std::getline(iss, msg_line);

    // search for host in header
    if (!(msg_line.substr(0, 5) == "Host:"))
    {
        BLITZ_LOG_ERROR("No (Host:) in header present: %s", msg_line.c_str());
        return is_valid;
    }
    host_id = msg_line;

    is_valid = true;

    return is_valid;
}

std::string HTTPRequest::getParams(std::vector<http_param>& vparams)
{
    std::string target_resource;

    if (!m_http_req.compare("GET"))
    {
        size_t params_begin = m_req_resource.find_first_of('?');

        if (params_begin != std::string::npos)
        {
            target_resource = m_req_resource.substr(0, params_begin);

            std::string params = m_req_resource.substr(params_begin + 1);

            std::stringstream ss_in(params);
            std::string param_pair;

            while(std::getline(ss_in, param_pair, '&'))
            {
                size_t delim_position =  param_pair.find_first_of('=');

                if (delim_position != std::string::npos)
                {
                    std::string value = param_pair.substr(delim_position + 1);
                    std::string param = param_pair.substr(0, delim_position);

                    vparams.push_back(http_param(param, value));
                }
            }
        }
    }
    return target_resource;
}

HTTPResponse::HTTPResponse() : HTTPMessage(), m_status_code(0)
{}

bool HTTPResponse::isValid(std::string message)
{
    return true;
}

void HTTPResponse::setStatusCode(unsigned int code)
{
    m_status_message = http_codes.find(code)->second;
    if (!m_status_message.empty())
    {
        m_status_code = code;
        /* complete headers */
        this->data(code, 0);
    }
}

void HTTPResponse::header(const std::string& line)
{
    if(!line.empty())
    {
        m_raw_headers += line + http_crlf;
    }
}

void HTTPResponse::stream(std::string filename, std::size_t size)
{
    m_raw_headers.clear();
    m_status_code = 200;
    m_status_message = http_codes.find(m_status_code)->second;

    try
    {
        header(http_version_1_1 + " " + boost::lexical_cast<std::string>(m_status_code) + " " + m_status_message);
        header("Date: " + currentDateTime());
        header("Server: " + server_name);
    }
    catch(boost::bad_lexical_cast &)
    {
        BLITZ_LOG_ERROR("bad lexical cast %d", m_status_code);
    }

    if (!filename.empty())
    {
        std::string extension = getFileExtension(filename);
        header(mime_types.find(extension)->second);
        header("Content-Disposition: inline; filename=" + filename);
        header("Expires: 0");
        header("Pragma: public");
        header("Cache-Control: must-revalidate");
        try
        {
            std::string size_txt = boost::lexical_cast<std::string>(size);
            header("Content-Length: " + size_txt);
        }
        catch(boost::bad_lexical_cast &)
        {
            BLITZ_LOG_ERROR("bad lexical cast %d", size);
        }

        m_raw_headers += "Connection: keep-alive: " + http_end_headers;
    }
    else
    {
        m_raw_headers += mime_types.find("mpeg")->second + http_end_headers;
    }
}

void HTTPResponse::data(unsigned int code, std::size_t size)
{
    m_raw_headers.clear();
    m_status_message = http_codes.find(code)->second;
    m_status_code = code;

    try
    {
        header(http_version_1_1 + " " + boost::lexical_cast<std::string>(m_status_code) + " " + m_status_message);
        header("Date: " + currentDateTime());
        header("Server: " + server_name);
    }
    catch(boost::bad_lexical_cast &)
    {
        BLITZ_LOG_ERROR("bad lexical cast %d", m_status_code);
    }

    if (m_status_code < 300 && size > 0)
    {
        try
        {
            m_raw_headers += mime_types.find("txt")->second + http_crlf;
            //m_raw_headers += "Content-Length: " + boost::lexical_cast<std::string>(size) + http_crlf;
            m_raw_headers += "Content-Length: " + boost::lexical_cast<std::string>(size) + http_end_headers;
        }
        catch(boost::bad_lexical_cast &)
        {
            BLITZ_LOG_ERROR("bad lexical cast %d", size);
        }
    }
    else if (m_status_code >= 400)
    {
        m_raw_headers += "Connection: close: " + http_end_headers;
    }
    //m_raw_headers += "Connection: Keep-Alive" + http_end_headers;
}

} // blitz




