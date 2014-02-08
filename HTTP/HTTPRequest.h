/*
 * HTTPRequest.h
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
 *  Created on: Jul 18, 2013
 *      Author: emo
 */

#ifndef HTTPREQUEST_H_
#define HTTPREQUEST_H_

#include <map>
#include <set>
#include <string>
#include "utils/ErrorCode.h"
#include "HTTP/HTTPUtils.h"

typedef std::set<std::string> HTTPHeaderNamesSet;
typedef std::set<std::string> HTTPParamNamesSet;

//typedef std::map<std::string, std::string> HTTPHeadersMap;
typedef std::map<std::string, std::string> HTTPReqParamsMap;

/**
* Abstraction of HTTP request (GET, POST ..) with additional headers.
*/
class HTTPRequest
{
public:
    HTTPRequest();
    virtual ~HTTPRequest() {}

    /**
    * Create request from HTTP text headers.
    * @param inHeader - string with header.
    * @param outError - error code with message.
    */
    void Init(const std::string& inHeader, ErrorCode& outError);

    /**
    * Read HTTP header input from file and process it.
    * @param fileName - string with file name.
    * @param outError - error code with message.
    */
    void ReadHeaderFromFile(const char* fileName, ErrorCode& outError);

    /**
    * Returns the HTTP path element of the request no query params .
    * @return string - string with path or empty string on error.
    */
    std::string GetPath() const;

    /**
    * Get the request content type.
    * @return string - content type or empty string on error.
    */
    std::string GetContentType();

    /**
    * Get the content length of the body of the message .
    * @return int - content length or 0 on error.
    */
    int GetContentLength();

    /**
    * Get the header as text.
    * @return string - header text or empty string on error.
    */
    std::string GetRawHeader() const;

    /**
    * Get the HTTP method : GET, POST, HEAD
    * @return string - method name or empty string on error.
    */
    std::string GetMethod() const;

    /**
    * Get the request protocol (example: HTTP/1.1)
    * @return string - protocol version or empty string on error.
    */
    std::string GetProtocol();

    /**
    * Get the query string part of the url (everything after the ?)
    * @return string - query or empty string on error.
    */
    std::string GetQueryString();

    /**
    * Get the request url (same as URI minus the query string)
    * @return string - URL or empty string on error.
    */
    std::string GetRequestURL();

    /**
    * Get the full request URI
    * @return string - URI or empty string on error.
    */
    std::string GetRequestURI();

    /**
    * Get the remote host name (if known) if not return ip address.
    * @return string - Host name or empty string on error.
    */
    std::string GetRemoteHost();

    /**
    * Get the remote host name (if known) if not return ip address.
    * @return string - Host name or empty string on error.
    */
    std::string GetRemoteAddr();

    /**
    * Get the remote service port.
    * @return unsigned short - port number or 0 on error.
    */
    unsigned short GetRemoteServicePort();

    /**
    * Get a HTTP header value such as 'Content-Length'
    * @param name - header name.
    * @return string - header value or empty string on error.
    */
    std::string GetHeader(const char* name) const;

    /**
    * Get the HTTP request header map.
    * @return std::map& - header as map or empty map on error.
    */
    HTTPHeadersMap& GetHeaderMap();

    /**
    * Get a Set of the header names.
    * @return std::set - header names or empty set on error.
    */
    HTTPHeaderNamesSet& GetHeaderNames();

    /**
    * Get a parameter value
    * @param name - name of the parameter.
    * @return string - parameter value or empty string on error.
    */
    std::string GetParameter(const char* name);

    /**
    * Get the entire parameter Map.
    * @return std::map& - parameters or empty map on error.
    */
    HTTPReqParamsMap& GetParameterMap();

    /**
    * Get a Set of parameter names.
    * @return std::set& - parameter names or empty set on error.
    */
    HTTPParamNamesSet& GetParameterNames();

private:
    void ReadQueryString();
    void ReadQueryParams();

    std::string mReqLine;                /**< Request line including method, path, query string and protocol version. */
    std::string mQueryString;           /**< Query string part from the request line after the ? symbol. */
    std::string mHeaderText;             /**< Full header text. */
    HTTPHeadersMap mMapHeaders;         /**< Header fields name->value map. */
    HTTPHeaderNamesSet mHeaderNames;    /**< All the header names in a set. */
    HTTPParamNamesSet mParamsNames;     /**< All the parameters names from the query string. */
    HTTPReqParamsMap  mMapParams;       /**< Parameters name->value map. */
};

#endif /* HTTPREQUEST_H_ */
