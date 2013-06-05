/*
 * MediaHttpClient.cpp
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
 *  Created on: Jan 27, 2013
 *      Author: emo
 */

#include "MediaHtppClient.h"
#include "DataPacket.h"
#include "Log.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>

namespace blitz {
namespace http {

void MediaHTTPClient::readContent()
{
    if (m_sock.is_open())
    {
        if (m_source)
        {
            m_packet = new DataPacket();

            m_io_control_timer.expires_from_now(boost::posix_time::seconds(MediaHTTPClient::receive_time));

            boost::asio::async_read(m_sock, boost::asio::buffer(m_packet->data(), DataPacket::max_size),
                                    boost::bind(&MediaHTTPClient::handleReadContent, this,
                                      boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));

            m_io_control_timer.async_wait(boost::bind(&MediaHTTPClient::handleDeadline, this,
                                            boost::asio::placeholders::error));
        }
        else
        {
            BLITZ_LOG_WARNING("Data source not attached !!!");
        }
    }
    else
    {
        BLITZ_LOG_WARNING("Socket is not open !!!");
    }
}

void MediaHTTPClient::handleDeadline(const boost::system::error_code& error)
{
    if (!error)
    {
        BLITZ_LOG_WARNING("Timeout receiving data !!!");
        m_io_control_timer.expires_from_now(boost::posix_time::seconds(MediaHTTPClient::receive_time));
        m_io_control_timer.async_wait(boost::bind(&MediaHTTPClient::handleDeadline, this,
                                                    boost::asio::placeholders::error));
    }
}

void MediaHTTPClient::handleReadContent(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    m_io_control_timer.cancel();

    if (!error && bytes_transferred)
    {
        if (bytes_transferred <= (std::size_t)DataPacket::max_size)
        {
            m_packet->size(bytes_transferred);
        }
        else
        {
            m_packet->size(DataPacket::max_size);
            BLITZ_LOG_WARNING("bytes_transferred: %d,  DataPacket::max_size: %d",
                                                       bytes_transferred, (DataPacket::max_size));

        }
        m_source->addData(m_packet);
        readContent();
    }
    else if (error)
    {
        BLITZ_LOG_ERROR("disconnecting...  got error: %s", error.message().c_str());
        delete m_packet;
        disconnect();
    }
}

void MediaHTTPClient::attach(Observer* ob)
{
    Subject::attach(ob);

    // our Observer is actually DataSource
    DataSource* source = NULL;
    try
    {
        source = dynamic_cast<DataSource*>(ob);
        m_source = source;
    }
    catch(std::exception& e)
    {
        BLITZ_LOG_ERROR("bad cast exception: %s", e.what());
        return;
    }
}


} // http
} // blitz
