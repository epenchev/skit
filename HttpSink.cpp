/*
 * HttpSink.cpp
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

#include "HttpSink.h"
#include "DataPacket.h"

namespace blitz {

HttpSink::HttpSink(boost::asio::io_service& io_service, short port) : server(io_service, port)
{
    server.start();
}

void HttpSink::write(DataPacket* pkt)
{
    server.sendPacket(pkt);
}

} // blitz



