/*
 * HTTPRequest.cpp
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

#include "HTTP/HTTPRequest.h"
#include "Logger.h"
#include <fstream>
#include <cstdlib>

typedef std::size_t offset;

static const std::string httpVer1_0 = "HTTP/1.0";
static const std::string httpVer1_1 = "HTTP/1.1";

static const char* arrHttpRequestMethod[] =
{ "GET", "HEAD", "POST", "TRACE", "CONNECT",
  "PUT", "OPTIONS", "DELETE", "PATCH", '\0'
};

void HTTPRequest::ReadQueryParams()
{
    offset paramSplitPos = 0;
    offset nameValueSplitPos = 0;

    if (mQueryString.empty())
    {
        LOG(logDEBUG) << "Empty query string";
        return;
    }

    while (nameValueSplitPos != std::string::npos)
    {
        nameValueSplitPos = mQueryString.find_first_of('=', paramSplitPos);
        if ( (nameValueSplitPos != std::string::npos) && (nameValueSplitPos > paramSplitPos) )
        {
            std::string paramName;
            try
            {
                paramName = mQueryString.substr(paramSplitPos, nameValueSplitPos - paramSplitPos);
            }
            catch (std::exception& ex)
            {
                LOG(logDEBUG) << ex.what();
                break;
            }
            paramSplitPos = mQueryString.find_first_of('&', nameValueSplitPos);
            if ( (paramSplitPos != std::string::npos) && (paramSplitPos > nameValueSplitPos) )
            {
                try
                {
                    std::string paramValue = mQueryString.substr(nameValueSplitPos + 1, (paramSplitPos - 1) - nameValueSplitPos);
                    paramSplitPos++;
                    if (!paramName.empty() && !paramValue.empty())
                    {
                        mMapParams[paramName] = paramValue;
                        mParamsNames.insert(paramName);
                    }
                }
                catch (std::exception& ex)
                {
                    LOG(logDEBUG) << ex.what();
                    break;
                }
            }
            else
            {
                offset endQueryString = mQueryString.find_first_of(' ', nameValueSplitPos);
                if ((endQueryString != std::string::npos) && (endQueryString > nameValueSplitPos) )
                {
                    try
                    {
                        std::string paramValue = mQueryString.substr(nameValueSplitPos + 1, endQueryString - nameValueSplitPos);
                        if (!paramName.empty() && !paramValue.empty())
                        {
                            mMapParams[paramName] = paramValue;
                            mParamsNames.insert(paramName);
                        }
                    }
                    catch (std::exception& ex)
                    {
                        LOG(logDEBUG) << ex.what();
                        break;
                    }
                }
            }
        }
    }
}

void HTTPRequest::ReadQueryString()
{
    if (!mReqLine.empty())
    {
        offset queryStringStart = mReqLine.find_first_of('?');
        if (queryStringStart != std::string::npos)
        {
            offset queryStringEnd = mReqLine.find_first_of(' ', queryStringStart + 1);
            if (queryStringEnd != std::string::npos)
            {
                try
                {
                    mQueryString = mReqLine.substr(queryStringStart + 1, queryStringEnd - queryStringStart);
                }
                catch(std::exception& ex)
                {
                    LOG(logDEBUG) << ex.what();
                }
            }

        }
    }
}

HTTPRequest::HTTPRequest()
{}

void HTTPRequest::ReadHeaderFromFile(const char* fileName, ErrorCode& outError)
{
    std::string header;
    std::ifstream fileStream;

    if (!fileName)
    {
        outError.SetValue(EINVAL);
        LOG(logDEBUG) << outError.GetErrorMessage();
        return;
    }

    fileStream.open(fileName);
    if (!fileStream.good() || !fileStream.is_open())
    {
        std::string errMsg = "Error opening file "; errMsg += fileName;
        outError.SetValue(-1);
        outError.SetMessage(errMsg);
        LOG(logDEBUG) << outError.GetErrorMessage();
        return;
    }

    // TODO set a limit for file size here
    while (!fileStream.eof())
    {
        char buf[100] = {0};
        fileStream.read(buf, 100);
        header += buf;
    }

    fileStream.close();
    outError = HTTPUtils::ReadHeader(header, mMapHeaders);
    if (outError) {
        LOG(logDEBUG) << outError.GetErrorMessage();
    }
}

void HTTPRequest::Init(const std::string& inHeader, ErrorCode& outError)
{
    mMapHeaders.clear();
    outError = HTTPUtils::ReadHeader(inHeader, mMapHeaders);
    if (!outError)
    {
        offset endLinePos = inHeader.find_first_of('\n');
        if (endLinePos != std::string::npos || endLinePos < inHeader.length())
        {
            mReqLine = inHeader.substr(0, endLinePos);
        }
        else
        {
            outError.SetValue(-1);
            outError.SetMessage("No valid request line found");
            LOG(logDEBUG) << outError.GetErrorMessage();
        }
    }
    else {
        LOG(logDEBUG) << outError.GetErrorMessage();
    }

}

int HTTPRequest::GetContentLength()
{
    if (mMapHeaders.empty())
    {
        LOG(logDEBUG) << "Header map is empty call ReadHeader()";
        return 0;
    }

    HTTPHeadersMap::iterator it = mMapHeaders.find("Content-Length");
    if (mMapHeaders.end() == it)
    {
        std::string length = it->second;
        return atoi(length.c_str());
    }

    return 0;
}

std::string HTTPRequest::GetContentType()
{
    if (mMapHeaders.empty())
    {
        LOG(logDEBUG) << "Header map is empty call ReadHeader()";
        return "";
    }

    HTTPHeadersMap::iterator it = mMapHeaders.find("Content-Type");
    if (mMapHeaders.end() != it)
    {
        return it->second;
    }

    return "";
}

std::string HTTPRequest::GetRawHeader()
{
    return "";
}

std::string HTTPRequest::GetMethod()
{
    int i = 0;

    if (mReqLine.empty())
    {
        LOG(logDEBUG) << "Request line empty call ReadHeader()";
        return "";
    }

    while(arrHttpRequestMethod[i] != '\0')
    {
        if (mReqLine.find(arrHttpRequestMethod[i]) != std::string::npos)
                    return arrHttpRequestMethod[i];
        i++;
    }

    LOG(logDEBUG) << "No method match found for request";
    return "";
}

std::string HTTPRequest::GetProtocol()
{
    if (mReqLine.empty())
    {
        LOG(logDEBUG) << "Request line empty call ReadHeader()";
        return "";
    }

    if (mReqLine.find(httpVer1_0) != std::string::npos)
    {
                return httpVer1_0;
    }
    if (mReqLine.find(httpVer1_1) != std::string::npos)
    {
                return httpVer1_1;
    }

    LOG(logDEBUG) << "No protocol match found";
    return "";
}

std::string HTTPRequest::GetQueryString()
{
    return mQueryString;
}

std::string HTTPRequest::GetPath()
{
    offset pathStartPos = 0;
    offset pathEndPos = 0;

    if (!mReqLine.empty())
    {
        /* skip trailing slash */
        pathStartPos = mReqLine.find_first_of('/');
        if (pathStartPos != std::string::npos)
        {
            /* check if we have parameters in request */
            std::string method = GetMethod();
            if ((method.compare("GET") == 0)  &&  !mQueryString.empty())
            {
                    pathEndPos = mReqLine.find_first_of('?');
            }
            else {
                pathEndPos = mReqLine.find_first_of(' ', pathStartPos + 1);
            }

            if (pathEndPos != std::string::npos && pathEndPos > pathStartPos)
            {
                try
                {
                     return mReqLine.substr(pathStartPos + 1, (pathEndPos - 1) - pathStartPos);
                }
                catch (std::exception& ex)
                {
                     LOG(logDEBUG) << ex.what();
                }
            }
        }
        else {
             LOG(logDEBUG) << "No proper formating found in request line";
        }
    }
    else {
         LOG(logDEBUG) << "Request line empty call ReadHeader()";
    }
    return "";
}

std::string HTTPRequest::GetRequestURL()
{
    std::string headerHost = GetHeader("Host");
    std::string path = GetPath();

    if (!headerHost.empty() && !path.empty())
    {
        headerHost += '/';
        path += '&';
        std::string url = "http://" + path + mQueryString;
        return url;
    }

    LOG(logDEBUG) << "Empty host or path in header";
    return "";
}

std::string HTTPRequest::GetRequestURI()
{
    std::string path = GetPath();
    std::string headerHost = GetHeader("Host");

    if (!headerHost.empty() && !path.empty())
    {
        headerHost += '/';
        std::string uri = "http://" + path;
        return uri;
    }

    LOG(logDEBUG) << "Empty host or path in header";
    return "";
}

std::string HTTPRequest::GetRemoteHost()
{
    return GetHeader("Host");
}

unsigned short HTTPRequest::GetRemoteServicePort()
{
    unsigned short servicePort = 0;

    std::string hostName = GetHeader("Host");
    if (!hostName.empty())
    {
        offset colonPosition = hostName.find_first_of(':');
        if ( colonPosition != std::string::npos )
        {
            try
            {
                servicePort = atoi(hostName.substr(colonPosition + 1).c_str());
            }
            catch (std::exception& ex)
            {
                servicePort = 0;
            }
        }
    }
    return servicePort;
}

std::string HTTPRequest::GetRemoteAddr()
{
    return GetRemoteHost();
}

std::string HTTPRequest::GetHeader(const char* name)
{
    if (!mMapHeaders.empty() && name)
    {
        HTTPHeadersMap::iterator it = mMapHeaders.find(name);
        if (mMapHeaders.end() != it)
                return it->second;
        else
        {
            std::string errMsg = "No such field in header "; errMsg += name;
            LOG(logDEBUG) << errMsg;
        }
    }
    else {
        LOG(logDEBUG) << "no name or map header is empty";
    }

    return "";
}

HTTPHeadersMap& HTTPRequest::GetHeaderMap()
{
    return mMapHeaders;
}

HTTPHeaderNamesSet& HTTPRequest::GetHeaderNames()
{
    return mHeaderNames;
}

std::string HTTPRequest::GetParameter(const char* name)
{
    if (!mMapParams.empty() && name)
    {
        HTTPReqParamsMap::iterator it = mMapParams.find(name);
        if (mMapParams.end() != it)
        {
            return it->second;
        }
        else
        {
            std::string errMsg = "No such parameter"; errMsg += name;
            LOG(logDEBUG) << errMsg;
        }
    }
    else {
        LOG(logDEBUG) << "no name or map parameters is empty";
    }

    return "";
}

HTTPReqParamsMap& HTTPRequest::GetParameterMap()
{
    return mMapParams;
}

HTTPParamNamesSet& HTTPRequest::GetParameterNames()
{
    return mParamsNames;
}
