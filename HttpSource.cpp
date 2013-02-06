/*
 * HttpSource.cpp
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
 *  Created on: Jan 7, 2013
 *      Author: emo
 */

#include <cstring>
#include <string>
#include <iostream>
#include "HttpSource.h"
#include "DataPacket.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace blitz {

HttpSource::HttpSource(boost::asio::io_service& io_service,
                          const std::string& url) : m_murl(url), m_client(io_service)
{
    // TODO something
}

void HttpSource::start(void)
{
    try
    {
        m_client.connect(m_murl.serverName(), m_murl.service());
        m_client.attachDataSource(this);
        m_client.sendReq("GET", m_murl.resource());
    }
    catch(std::exception& ex)
    {
        std::cout << "HttpSource() exception in constructor " << ex.what() << std::endl;
        throw;
    }
}

} // blitz
