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
#include <string>
#include <utility>
#include "ErrorCode.h"

typedef std::map<std::string, std::string> HTTPHeadersMap;
typedef std::pair<std::string, std::string> HTTPParam;

class HTTPUtils
{
public:
    static ErrorCode ReadHeader( const std::string& inHeader, HTTPHeadersMap& outMapHeaders );

    static ErrorCode SplitHeaderLine(const std::string& line, HTTPParam& outParam);

    // Make a HTTP request and return the result as a byte array
    /* static std::string HTTPRequestToString(std::string& inUrl, std::string& method,
                                           std::string& data, std::map& headers);
    */

    //Split a query string into a map and URL decode the values
    /* static std::map SplitQueryStr(std::string queryStr); */

    //Convert an HTTP status code to a string
    //static std::string StatusCodeToStr(int statusCode);

protected:
    HTTPUtils() {}
private:
};

#endif /* HTTPUTILS_H_ */
