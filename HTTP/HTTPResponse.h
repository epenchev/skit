/*
 * HTTPResponse.h
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

#ifndef HTTPRESPONSE_H_
#define HTTPRESPONSE_H_

#include <string>
#include "HTTP/HTTPUtils.h"

/**
* Abstraction of HTTP response (200 OK, 404 Not found ..) with additional headers.
*/
class HTTPResponse
{
public:
    HTTPResponse();
    virtual ~HTTPResponse() {}

    /**
    * Create reponse from HTTP text headers.
    * @param inHeader - string with header.
    */
    void Init(const std::string& inHeader, ErrorCode& outError);

    /**
    * Returns true if response header contains parameter name.
    * @param name - header field name.
    * @return bool - true if contains false otherwise.
    */
    bool ContainsHeader(const std::string& name);

    /**
    * Get header value
    * @param name - header name .
    * @return string - header value, empty string on error.
    */
    std::string GetHeader(const std::string& name);

    /**
    * Get the current response headers as a map
    * @return std::map& - headers as map, empty map if no headers present.
    */
    HTTPHeadersMap& GetHeaders();

    /**
    * Remove header value.
    * @param string - name of header.
    */
    void RemoveHeader(const std::string& name);

    /**
    * Set header value or insert new header field if name is not present.
    * @param string - name of header field.
    * @param string - value.
    */
    void SetHeader(const std::string& name, std::string value);

    /**
    * Set the HTTP response code.
    * @param int - code as number.
    */
    void SetResponseCode(unsigned responseCode);

    /**
    * Get the HTTP response code.
    * @return unsigned - HTTP response code.
    */
    unsigned GetResponseCode();

    /**
    * Convert an HTTP status code to a string.
    * @param unsigned - status code number.
    * @return string - status message.
    */
    std::string StatusCodeToStr(unsigned statusCode);

    /**
    * Get the string representation of the response.
    * @return string - response object as string, empty string on failure.
    */
    std::string Str();

    // TODO
    void SetData(const std::string& data) { m_data = data; }

private:
    HTTPHeadersMap m_mapheaders;  /**< Map headers name->value */
    std::string m_textheaders;   /**< HTTP headers as text */
    std::string m_data;          /**< additional data */
    unsigned m_responseCode;    /**< HTTP response code */
};

#endif /* HTTPRESPONSE_H_ */
