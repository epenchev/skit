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

#include <map>
#include <string>
#include "ErrorCode.h"
#include "HTTP/HTTPUtils.h"

//typedef std::map<std::string, std::string> HTTPHeadersMap;

/**
* Abstraction of HTTP response (200 OK, 404 Not found ..) with additional headers.
*/
class HTTPResponse
{
public:
    HTTPResponse();
    virtual ~HTTPResponse() {}

    /**
    * Read HTTP header input from string and process it.
    * @param inHeader - string with header text.
    */
    //void ReadHeader(const std::string& inHeader);

    // Returns true if response header contains parameter name
    bool ContainsHeader(const std::string& name);

    // Get header value
    std::string GetHeader(const std::string& name);

    //Get the current response headers as a map
    HTTPHeadersMap& GetHeaders();

    // Remove header value
    void RemoveHeader(const std::string& name);

    //Set header value
    void SetHeader(const std::string& name, std::string value);

    //Set header value as int
    void SetIntHeader(const std::string& name, int value);

    //Set the HTTP response code
    void SetResponseCode(int responseCode);

    // Convert an HTTP status code to a string
    std::string StatusCodeToStr(int statusCode);

    /**
    * Get the error from the last operation.
    * @return ErrorCode reference with last operation error.
    */
    ErrorCode& GetLastError() { return mErrCode; }

private:
    ErrorCode  mErrCode;         /**< error code of last operation */
    HTTPHeadersMap mMapHeaders;  /**< Map headers name->value */
    std::string mTextHeaders;    /**< HTTP headers as text */
    int mResponseCode;           /**< HTTP response code */
};

#endif /* HTTPRESPONSE_H_ */
