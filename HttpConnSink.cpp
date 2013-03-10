/*
 * HTTPConnSink.cpp
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
 *  Created on: Feb 25, 2013
 *      Author: emo
 */

#include "HttpConnSink.h"
#include "Log.h"

namespace blitz {

HTTPConnSink::HTTPConnSink(boost::asio::io_service& io_service) : MediaHTTPConnection(io_service), m_ready(true)
{}

void HTTPConnSink::write(DataPacket *p)
{
    m_ready = false;
    DataSink::write(p);

    if (m_packet)
    {
        boost::asio::async_write(socket(), boost::asio::buffer(m_packet->data(), m_packet->size()),
                                 boost::bind(&HTTPConnSink::handleWriteContent, this,
                                   boost::asio::placeholders::error));
    }
}

void HTTPConnSink::handleWriteContent(const boost::system::error_code& error)
{
    if (!error)
    {
        m_ready = true;
        BLITZ_LOG_INFO("Data send");
        delete m_packet;
        usleep(500);
        notify();
    }
    else
    {
        BLITZ_LOG_ERROR("Terminating connection with error: %s", error.message().c_str());
        close();
    }
}

} // blitz
