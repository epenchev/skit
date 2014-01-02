/*
 * HTTPUtils.h
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

#ifndef HTTPUTILS_H_
#define HTTPUTILS_H_

#include <map>
#include <set>
#include <string>
#include <utility>
#include "utils/ErrorCode.h"

typedef std::map<std::string, std::string> HTTPHeadersMap;
typedef std::pair<std::string, std::string> HTTPParam;
typedef std::set<std::string> HTTPHeadersSet;

class HTTPUtils
{
public:

    /**
    * Read HTTP header input from string and process it.
    * @param inHeader - string with header.
    * @param outMapHeaders - result map with header names and values.
    * @return ErrorCode value.
    */
    static ErrorCode ReadHeader( const std::string& inHeader, HTTPHeadersMap& outMapHeaders );

    /**
    * Read line from HTTP header and split name and value.
    * @param line - string containing line.
    * @param outParam - HTTPParam resulting pair name->value.
    * @return ErrorCode value.
    */
    static ErrorCode SplitHeaderLine(const std::string& line, HTTPParam& outParam);

    /**
    * Make HTTP request string.
    * @param inUrl - string containing the URL.
    * @param method - string with HTTP method (GET, POST..).
    * @param data - string additional data after the headers can be empty.
    * @param headers - map with additional headers can be empty.
    * @return string - string with HTTP headers or empty string on error.
    */
    static std::string HTTPRequestToString(const std::string& inUrl, const std::string& method,
                                               const std::string& data, HTTPHeadersMap& headers);

    /** Make a HTTP reponse string.
    * @param code - HTTP response code.
    * @param headers - reference to set with headers can be empty.
    */
    static std::string HTTPResponseToString(unsigned responseCode, HTTPHeadersMap& headers);

protected:
    HTTPUtils() {}
private:
};

#endif /* HTTPUTILS_H_ */
