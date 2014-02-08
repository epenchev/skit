/*
 * HTTPUtils.cpp
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
 *  Created on: Jul 24, 2013
 *      Author: emo
 */

#include "HTTP/HTTPUtils.h"
#include <sstream>
#include <iostream>

typedef std::size_t offset;

static const std::string endHeaders = "\r\n\r\n";
static const std::string endHeaders1 = "\n\n";
static const std::string serverName = "blitz-stream";
static const std::string http_version_1_0 = "HTTP/1.0";
static const std::string http_version_1_1 = "HTTP/1.1";
static const std::string http_crlf = "\r\n";

class HttpResponseCodeMap : public std::map<unsigned, std::string>
{
public:
    HttpResponseCodeMap()
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
} const static responseCodes;


static const std::string currentDateTime()
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

ErrorCode HTTPUtils::ReadHeader( const std::string& inHeader, HTTPHeadersMap& outMapHeaders )
{
    HTTPParam param;
    ErrorCode errCode;

    if (inHeader.empty())
    {
        errCode.SetValue(-1);
        errCode.SetMessage("Empty header");
        return errCode;
    }

    if (std::string::npos == inHeader.find(endHeaders) &&
        std::string::npos == inHeader.find(endHeaders1))
    {
        errCode.SetValue(-1);
        errCode.SetMessage("Error, end of headers not found");
        return errCode;
    }

    offset endLinePos = inHeader.find_first_of('\n');
    if (endLinePos != std::string::npos || endLinePos < inHeader.length())
    {
        while (endLinePos != std::string::npos || endLinePos < inHeader.length())
        {
            std::string line;
            offset currentPos = endLinePos + 1;
            endLinePos = inHeader.find_first_of('\n', currentPos);
            try
            {
                line = inHeader.substr(currentPos, (endLinePos - currentPos));
                if (line.empty())
                {
                    break;
                }
            }
            catch(std::exception& ex)
            {
                errCode.SetValue(-1);
                errCode.SetMessage(ex.what());
                return errCode;
            }
            ErrorCode retCode = HTTPUtils::SplitHeaderLine(line, param);
            if (!retCode) // no need to check result of the insert operation
            {
                outMapHeaders.insert(param);
            }
        }
    }
    else
    {
        errCode.SetValue(-1);
        errCode.SetMessage("Error parsing header, no proper formating found");
    }

    return errCode;
}

ErrorCode HTTPUtils::SplitHeaderLine(const std::string& line, HTTPParam& outParam)
{
    ErrorCode errCode;

    if (!line.empty())
    {
        offset splitPos = line.find_first_of(':');
        if (std::string::npos != splitPos)
        {
            try
            {
                std::string headerName = line.substr(0, splitPos);
                // miss ':' character and white space
                std::string headerValue = line.substr(splitPos + 2, line.length());
                outParam = std::make_pair(headerName, headerValue);
            }
            catch(std::exception& ex)
            {
                errCode.SetValue(-1);
                errCode.SetMessage(ex.what());
                return errCode;
            }
        }
        else
        {
            errCode.SetValue(-1);
            errCode.SetMessage("Error splitting, control char ':' missing");
        }
    }
    else
    {
        errCode.SetValue(-1);
        errCode.SetMessage("Line is empty");
    }

    return errCode;
}

std::string HTTPUtils::HTTPRequestToString(const std::string& inUrl, const std::string& method,
                                               const std::string& data, HTTPHeadersMap& headers)
{
    std::string server;
    std::string resource;
    std::string servicePort;
    std::string resultHTTPReq;
    const int doubleSlashSize = 2;

    if (!inUrl.empty() && !method.empty())
    {
        if (inUrl.substr(0, 7) != "http://")
        {
            return "";
        }

        offset doubleSlashPos = inUrl.find_first_of("//");

        if (doubleSlashPos != std::string::npos)
        {
            offset colonPosition = inUrl.find_first_of(":", doubleSlashPos + doubleSlashSize);
            // we have port != 80
            if ( colonPosition != std::string::npos && colonPosition > (doubleSlashPos + doubleSlashSize) )
            {
                offset n = colonPosition - (doubleSlashPos + doubleSlashSize);
                server = inUrl.substr( (doubleSlashPos + doubleSlashSize), n );

                offset slashPosition = inUrl.find_first_of("/", colonPosition + 1);
                resource = inUrl.substr(slashPosition);

                if ( slashPosition != std::string::npos && slashPosition > colonPosition )
                {
                    size_t n = slashPosition - (colonPosition + 1);
                    servicePort = inUrl.substr(colonPosition + 1, n);
                }
            }
            else // standart web port 80
            {
                offset slashPosition = inUrl.find_first_of("/", doubleSlashPos + doubleSlashSize);
                if ( slashPosition != std::string::npos )
                {
                    resource  = inUrl.substr(slashPosition);
                    size_t n = slashPosition - (doubleSlashPos + doubleSlashSize);
                    server = inUrl.substr(doubleSlashPos + doubleSlashSize, n);
                }
            }
        }
        resultHTTPReq = method + " " + resource + " HTTP/1.1\r\n";
        if (headers.find("Host") == headers.end())
        {
            resultHTTPReq += "Host: " + server;
            if (!servicePort.empty()) {
                resultHTTPReq += ":" + servicePort;
            }
            resultHTTPReq += "\r\n";
        }
        for (HTTPHeadersMap::iterator it=headers.begin(); it!=headers.end(); ++it)
        {
            resultHTTPReq += it->first + ": " + it->second + "\r\n";
        }

        resultHTTPReq += endHeaders;
        if (!data.empty()) {
            resultHTTPReq += data;
        }
    }

    return resultHTTPReq;
}

std::string HTTPUtils::HTTPResponseToString(unsigned code, HTTPHeadersMap& headers)
{
    std::string returnHeaders = "";
    std::string reponseData = "";

    if (responseCodes.count(code) > 0)
    {
        std::stringstream stringCode;
        stringCode << code;
        std::string statusMessage = responseCodes.at(code);
        returnHeaders = http_version_1_1 + " " + stringCode.str() + " " + statusMessage + http_crlf;
    }
    else
    {
        return returnHeaders;
    }

    returnHeaders += "Server: " + serverName + http_crlf;
    returnHeaders += "Date: " + currentDateTime() + http_crlf;


    for (HTTPHeadersMap::iterator it = headers.begin(); it != headers.end(); ++it)
    {
    	// Server and Date headers can't be overridden
    	std::string headerName = it->first;
        if ( headerName.compare("Server") != 0 &&
             headerName.compare("Date") != 0 )
        {
        	returnHeaders += it->first + ": " + it->second + http_crlf;
        }
    }
    if (headers.end() == headers.find("Connection"))
    {
    	//returnHeaders += "Connection: close" + http_crlf;
    }
    returnHeaders += http_crlf; //endHeaders;

    return returnHeaders;
}


