/*
 * Url.cpp
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

#include "Url.h"
#include <cstring>
#include <string>
#include <iostream>
#include <stdexcept>

namespace blitz {

Url::Url(const std::string& murl)
{
    if (murl.empty())
    {
        throw std::invalid_argument("empty string Url::Url()");
    }

    readUrl(murl);

    if ( m_server.empty() || m_service.empty() || m_resource.empty() )
    {
        throw std::invalid_argument("Failed parsing url Url::Url()");
    }
}

void Url::readUrl(const std::string& url)
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
            m_server = url.substr( (doubleSlashPos + doubleSlashSize), n );

            size_t slashPosition = url.find_first_of("/", colonPosition + 1);
            m_resource = url.substr(slashPosition);

            if ( slashPosition != std::string::npos && slashPosition > colonPosition )
            {
                size_t n = slashPosition - (colonPosition + 1);
                m_service = url.substr(colonPosition + 1, n);
            }
        }
        else // standart web port 80
        {
            size_t slashPosition = url.find_first_of("/", doubleSlashPos + doubleSlashSize);

            if ( slashPosition != std::string::npos )
            {
                m_resource  = url.substr(slashPosition);

                size_t n = slashPosition - (doubleSlashPos + doubleSlashSize);

                m_server = url.substr(doubleSlashPos + doubleSlashSize, n);
                m_service = "http";
            }
        }
    }
}

std::string Url::serverName()
{
    return m_server;
}

std::string Url::service()
{
    return m_service;
}

std::string Url::resource()
{
    return m_resource;
}





} // blitz
