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
#include "HttpClient.h"
#include "DataPacket.h"
#include "Log.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

namespace blitz {

HttpSource::HttpSource(boost::asio::io_service& io_service, const std::string& url)
: m_murl(url), m_timer(io_service), m_client(io_service)
{}

void HttpSource::start(void)
{
    try
    {
        m_client.attach(this);
        m_client.connect(m_murl.serverName(), m_murl.service());
    }
    catch(std::exception& ex)
    {
        BLITZ_LOG_WARNING("exception received %s", ex.what());
    }
}

void HttpSource::handleReconnect(const boost::system::error_code& error)
{
	if (!error)
	{
		try
		{
			m_client.connect(m_murl.serverName(), m_murl.service());
		}
		catch(std::exception& ex)
		{
			BLITZ_LOG_WARNING("exception received %s", ex.what());
		}
	}
}

void HttpSource::update(Subject* changed_subject)
{
    BLITZ_LOG_WARNING("We are notified from subject");

    blitz::http::HTTPClientState client_state = m_client.getState();

    if (blitz::http::STATE_CONNECT == client_state)
    {
        BLITZ_LOG_INFO("Client connected sending HTTP request");
        try
        {
            m_client.sendReq("GET", m_murl.resource());
        }
        catch(std::exception& ex)
        {
            BLITZ_LOG_WARNING("exception from HTTPClient::sendReq() %s", ex.what());
        }
    }
    else if (blitz::http::STATE_DISCONNECT == client_state)
    {
        BLITZ_LOG_ERROR("Client disconnected, trying to reconnect");

        m_timer.expires_from_now(boost::posix_time::seconds(60));
        m_timer.async_wait(boost::bind(&HttpSource::handleReconnect, this,
                                         boost::asio::placeholders::error));
    }
}

} // blitz
