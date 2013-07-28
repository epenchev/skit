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


bool HTTPResponse::ContainsHeader(const std::string& name)
{
    bool retCode = false;

    if (!mMapHeaders.empty() && !name.empty())
    {
        HTTPHeadersMap::iterator it = mMapHeaders.find(name);
        if (mMapHeaders.end() != it)
                retCode = true;
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
                    return it->second;
            else
            {
                std::string errMsg = "No such field in header "; errMsg += name;
                mErrCode.SetMessage(errMsg);
            }
        }
        else {
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

}

void HTTPResponse::SetHeader(const std::string& name, std::string value)
{

}

void HTTPResponse::SetIntHeader(const std::string& name, int value)
{

}

void HTTPResponse::SetResponseCode(int responseCode)
{

}

std::string HTTPResponse::StatusCodeToStr(int statusCode)
{

}

