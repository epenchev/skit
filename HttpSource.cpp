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
#include "HttpSource.h"
#include "DataPacket.h"
#include <boost/asio.hpp>

#include "logging/logging.h"
using namespace ::logging;

using boost::asio::ip::tcp;

namespace blitz {

HttpSource::HttpSource(boost::asio::io_service& io_service,
                          const std::string& url) : m_murl(url), m_client(io_service)
{}

void HttpSource::restart(void)
{
	log::emit< Warning>() << " HttpSource::restart() restarting connection "<< log::endl;
	start();
}

void HttpSource::start(void)
{
    try
    {
        m_client.connect(m_murl.serverName(), m_murl.service());
        m_client.attachDataSource(this);
        m_client.sendReq("GET", m_murl.resource());
    }
    catch(blitz::http::HTTPClientException& ex)
    {
    	log::emit< Warning>() << "HttpSource::start() HTTPClientException: " << ex.what() << " "
    			                            << log::dec << ex.errorCode() << log::endl<< log::endl;
    	throw;
    }
    catch(std::exception& ex)
    {
    	log::emit< Trace>() << "HttpSource() exception in constructor " << ex.what() << log::endl;
        throw;
    }
}

} // blitz
