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
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "logging/logging.h"
using namespace ::logging;

namespace blitz {
namespace http {

void MediaHTTPClient::readContent()
{
    //log::emit< Trace>() << "MediaHTTPClient::readContent()" << log::endl;

    if (m_sock.is_open())
    {
        //log::emit< Trace>() << "MediaHTTPClient::readContent() m_sock.is_open() == true " << log::endl;
        if (m_source)
        {
            m_packet = new DataPacket();
            m_sock.async_read_some(boost::asio::buffer(m_packet->data(), DataPacket::max_size),
                                    boost::bind(&MediaHTTPClient::handleReadContent, this,
                                      boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
        }
    }
}

void MediaHTTPClient::handleReadContent(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if (!error && bytes_transferred)
    {
        //log::emit< Trace>() << "MediaHTTPClient::handleReadContent()" << log::endl;

        if (bytes_transferred <= (std::size_t)DataPacket::max_size) {
            m_packet->size(bytes_transferred);
        }
        else
        {
            // not possible but ...
            m_packet->size(DataPacket::max_size);
            log::emit< Warning>() << "MediaHTTPClient::handleReadContent() bytes_transferred: "
                                    << log::dec << bytes_transferred << log::endl;
        }
        m_source->addData(m_packet);
        readContent();
    }
    else
    {
        log::emit< Error>() << "MediaHTTPClient::handleReadContent()  "
                                            << error.message().c_str() << log::endl;
    }
    /*
    else if (error != boost::asio::error::eof)
    {
        throw std::runtime_error(error.message());
    }*/
}

} // http
} // blitz
