/*
 * HttpConnSink.h
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

#ifndef HTTPCONNSINK_H_
#define HTTPCONNSINK_H_

#include <boost/asio.hpp>
#include "DataSink.h"
#include "DataPacket.h"
#include "MediaHttpServer.h"

namespace blitz {

class HTTPConnSink : public DataSink, public MediaHTTPConnection
{
public:
    HTTPConnSink(boost::asio::io_service& io_service);
    virtual ~HTTPConnSink() {}

    // from DataSink
    virtual void write(DataPacket *p);
    inline bool isReady() { return m_ready; }
private:
    void handleWriteContent(const boost::system::error_code& error);
    bool m_ready;
};

} // blitz

#endif /* HTTPCONNSINK_H_ */
