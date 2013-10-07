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
#include <map>
#include <utility>
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

HTTPResponse::HTTPResponse() : mResponseCode(0)
{}

void HTTPResponse::Init(const std::string& inHeader)
{
	ErrorCode err = HTTPUtils::ReadHeader(inHeader, mMapHeaders);
	if (!err)
	{
		offset pos = inHeader.find("HTTP/1.");
		if (std::string::npos != pos)
		{
			try
			{
				std::string code = inHeader.substr(pos + 2, 3);
				if (!code.empty())
				{
					mResponseCode = atoi(code.c_str());
					return;
				}
			}
			catch (std::exception& ex)
			{
				mErrCode.SetMessage(ex.what());
				return;
			}
		}
		mErrCode.SetMessage("Not a valid HTTP response");
	}
	else
	{
		mErrCode = err;
		return;
	}

}

bool HTTPResponse::ContainsHeader(const std::string& name)
{
    bool retCode = false;

    if (!mMapHeaders.empty() && !name.empty())
    {
        HTTPHeadersMap::iterator it = mMapHeaders.find(name);
        if (mMapHeaders.end() != it)
        {
        	retCode = true;
        }
    }
    return retCode;
}

std::string HTTPResponse::GetHeader(const std::string& name)
{
    mErrCode.Clear();
    if (!mMapHeaders.empty() && !name.empty())
    {
    	HTTPHeadersMap::iterator it = mMapHeaders.find(name);
        if (mMapHeaders.end() != it)
        {
        	return it->second;
        }
        else
        {
        	std::string errMsg = "No such field in header "; errMsg += name;
            mErrCode.SetMessage(errMsg);
        }
    }
    else
    {
    	mErrCode.SetMessage("no name or map header is empty");
    }
    return "";
}


HTTPHeadersMap& HTTPResponse::GetHeaders()
{
    return mMapHeaders;
}

void HTTPResponse::RemoveHeader(const std::string& name)
{
	mErrCode.Clear();
	if (!name.empty())
	{
		GetHeader(name);
		if (!GetLastError())
		{
			mMapHeaders.erase(name);
		}
		else
		{
			mErrCode.SetMessage("No such header in response");
		}
	}
	mErrCode.SetMessage("Empty name");
}

void HTTPResponse::SetHeader(const std::string& name, std::string value)
{
	mErrCode.Clear();
	if (!name.empty() && !value.empty())
	{
		GetHeader(name);
		if (!GetLastError())
		{
			mMapHeaders.at("name") = value;
		}
		else
		{
			mMapHeaders.insert(HTTPParam(name, value));
		}
	}
	else
	{
		mErrCode.SetMessage("Empty name");
	}
}

void HTTPResponse::SetResponseCode(unsigned responseCode)
{
	mErrCode.Clear();
	if (responseCode > 0)
	{
		if (!(HTTPCodes.find(responseCode)->second.empty()))
		{
			mResponseCode = responseCode;
		}
		else
		{
			mErrCode.SetMessage("No such HTTP response code");
		}
	}
	else
	{
		mErrCode.SetMessage("Empty response code");
	}
}

std::string HTTPResponse::StatusCodeToStr(int statusCode)
{
	mErrCode.Clear();
	std::string statusMessage = HTTPCodes.find(statusCode)->second;
	if (statusMessage.empty())
	{
		mErrCode.SetMessage("No such HTTP status code");
	}

	return statusMessage;
}

