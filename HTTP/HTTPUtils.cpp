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

typedef std::size_t offset;

static const char* endHeaders = "\r\n\r\n";
static const char* endHeaders1 = "\n\n";


ErrorCode HTTPUtils::ReadHeader( const std::string& inHeader, HTTPHeadersMap& outMapHeaders )
{
    HTTPParam param;
    ErrorCode errCode;

    if (inHeader.empty())
    {
        errCode.SetMessage("Empty header");
        return errCode;
    }

    if (std::string::npos == inHeader.find(endHeaders) &&
        std::string::npos == inHeader.find(endHeaders1))
    {
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
                    break;
            }
            catch(std::exception& ex)
            {
                errCode.SetMessage(ex.what());
                return errCode;
            }
            ErrorCode retCode = HTTPUtils::SplitHeaderLine(line, param);
            if (!retCode) // no need to check result of the insert operation
                outMapHeaders.insert(param);
        }
    }
    else
        errCode.SetMessage("Error parsing header, no proper formating found");

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
                errCode.SetMessage(ex.what());
                return errCode;
            }
        }
        else {
            errCode.SetMessage("Error splitting, control char ':' missing");
        }
    }
    else {
        errCode.SetMessage("Line is empty");
    }

    return errCode;
}
