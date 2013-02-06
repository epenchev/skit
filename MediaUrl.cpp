/*
 * MediaUrl.cpp
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
 *  Created on: Jan 13, 2013
 *      Author: emo
 */

#include "MediaUrl.h"
#include <cstring>
#include <string>
#include <iostream>
#include <stdexcept>

namespace blitz {

MediaUrl::MediaUrl(const std::string& url)
{
    if (url.empty())
    {
        // TODO log this error !!!
        std::cout << "empty string MediaUrl::MediaUrl() \n";
        throw std::invalid_argument("empty string MediaUrl::MediaUrl()");
    }

    readUrl(url);

    if ( server_.empty() || service_.empty() || resource_.empty() )
    {
        // TODO log this error !!!
        std::cout << "Failed parsing " << url << " MediaUrl::MediaUrl() \n";
        throw std::invalid_argument("Failed parsing url MediaUrl::MediaUrl()");
    }
}

void MediaUrl::readUrl(const std::string& url)
{
    if (url.substr(0, 7) != "http://")
        return;

    size_t doubleSlashSize = strlen("//");
    size_t doubleSlashPos = url.find_first_of("//");

    if (doubleSlashPos != std::string::npos)
    {
        size_t colonPosition = url.find_first_of(":", doubleSlashPos + doubleSlashSize);

        // we have port != 80
        if ( colonPosition != std::string::npos && colonPosition > (doubleSlashPos + doubleSlashSize) )
        {
            size_t n = colonPosition - (doubleSlashPos + doubleSlashSize);
            server_ = url.substr( (doubleSlashPos + doubleSlashSize), n );

            size_t slashPosition = url.find_first_of("/", colonPosition + 1);
            resource_ = url.substr(slashPosition);

            if ( slashPosition != std::string::npos && slashPosition > colonPosition )
            {
                size_t n = slashPosition - (colonPosition + 1);
                service_ = url.substr(colonPosition + 1, n);
            }
        }
        else // standart web port 80
        {
            size_t slashPosition = url.find_first_of("/", doubleSlashPos + doubleSlashSize);

            if ( slashPosition != std::string::npos )
            {
                resource_  = url.substr(slashPosition);

                size_t n = slashPosition - (doubleSlashPos + doubleSlashSize);

                server_ = url.substr(doubleSlashPos + doubleSlashSize, n);
                service_ = "http";
            }
        }
    }
}

std::string MediaUrl::serverName()
{
    return server_;
}

std::string MediaUrl::service()
{
    return service_;
}

std::string MediaUrl::resource()
{
    return resource_;
}





} // blitz
