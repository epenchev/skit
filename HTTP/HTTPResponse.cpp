/*
 * HTTPResponse.cpp
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

#include "HTTP/HTTPResponse.h"
#include "utils/Logger.h"
#include <map>
#include <utility>
#include <sstream>
#include <cstdlib>
#include <exception>

typedef std::size_t offset;

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
} const static HTTPCodes;

HTTPResponse::HTTPResponse() : m_responseCode(0)
{}

void HTTPResponse::Init(const std::string& inHeader, ErrorCode& outError)
{
    outError = false;
	ErrorCode err = HTTPUtils::ReadHeader(inHeader, m_mapheaders);
    if (!err)
    {
        offset pos = inHeader.find("HTTP/1.");
        if (std::string::npos != pos)
        {
        	LOG(logDEBUG) << "Valid HTTP response";
            try
            {
                std::string code = inHeader.substr(pos + 9, 3);
                if (!code.empty())
                {
                	m_responseCode = atoi(code.c_str());
                }
            }
            catch (std::exception& ex)
            {
                LOG(logERROR) << ex.what();
                outError.SetMessage(ex.what());
                return;
            }
        }
        else
        {
        	LOG(logERROR) << "Not a valid HTTP response";
        }
    }
    else
    {
    	LOG(logERROR) << err.Message();
    	outError = err;
    }

}

bool HTTPResponse::ContainsHeader(const std::string& name)
{
    bool result = false;

    if (!m_mapheaders.empty() && !name.empty())
    {
        HTTPHeadersMap::iterator it = m_mapheaders.find(name);
        if (m_mapheaders.end() != it)
        {
        	result = true;
        }
    }
    return result;
}

std::string HTTPResponse::GetHeader(const std::string& name)
{
    if (!m_mapheaders.empty() && !name.empty())
    {
        HTTPHeadersMap::iterator it = m_mapheaders.find(name);
        if (m_mapheaders.end() != it)
        {
            return it->second;
        }
        else
        {
            LOG(logERROR) << "No such field in header " + name;
        }
    }
    else
    {
    	LOG(logERROR) << "no name or map header is empty";
    }
    return "";
}


HTTPHeadersMap& HTTPResponse::GetHeaders()
{
    return m_mapheaders;
}

void HTTPResponse::RemoveHeader(const std::string& name)
{
    if (!name.empty())
    {
        std::string field = GetHeader(name);
        if (!field.empty())
        {
            m_mapheaders.erase(name);
        }
        else
        {
            LOG(logERROR) << "No such header in response";
        }
    }
}

void HTTPResponse::SetHeader(const std::string& name, std::string value)
{
    if (!name.empty() && !value.empty())
    {
    	// header is present just change value
    	if (m_mapheaders.find(name) != m_mapheaders.end())
        {
            m_mapheaders.at("name") = value;
        }
        else // new header field
        {
            m_mapheaders.insert(HTTPParam(name, value));
        }
    }
}

void HTTPResponse::SetResponseCode(unsigned responseCode)
{
    if (!(HTTPCodes.find(responseCode)->second.empty()))
    {
    	m_responseCode = responseCode;
    }
    else
    {
    	LOG(logERROR) << "No such HTTP response code";
    }
}

unsigned HTTPResponse::GetResponseCode()
{
    return m_responseCode;
}

std::string HTTPResponse::StatusCodeToStr(unsigned statusCode)
{
    std::string statusMessage = HTTPCodes.find(statusCode)->second;
    if (statusMessage.empty())
    {
        LOG(logERROR) << "No such HTTP status code";
    }

    return statusMessage;
}

std::string HTTPResponse::Str()
{
	std::stringstream ss;
	std::string textheaders = "";

	if (m_responseCode)
	{
		ss << m_responseCode;
		textheaders = "HTTP/1.1 " + ss.str() + " " + StatusCodeToStr(m_responseCode) + "\r\n";
		for (HTTPHeadersMap::reverse_iterator it = m_mapheaders.rbegin(); it != m_mapheaders.rend(); ++it)
		{
			textheaders += it->first + ": " + it->second + "\r\n";
		}
		textheaders += "\r\n";

		if (!m_data.empty())
		{
			textheaders += m_data;
		}
	}

	return textheaders;
}

