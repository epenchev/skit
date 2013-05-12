/*
 * HttpSource.h
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

#ifndef HTTPSOURCE_H_
#define HTTPSOURCE_H_

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include "DataSource.h"
#include "DataPacket.h"
#include "Url.h"
#include "MediaHtppClient.h"
#include "Observer.h"
#include "ControlChannel.h"

using boost::asio::ip::tcp;
using boost::asio::deadline_timer;

namespace blitz {

/**
* Derived from DataSource class, reads data from HTTP source (media URL).
*/
class HttpSource : public DataSource, public Observer, public Controler
{
public:
    /**
    * constructor.
    * @param io_service: object reference to boost io_service class for socket operations.
    * @param murl: string media URL.
    */
    HttpSource(boost::asio::io_service& io_service, const std::string& url);
    virtual ~HttpSource() {}

    // from observer
    virtual void update(Subject* changed_subject);

    // from DataSource
    virtual void start();

    // from Controler
    virtual bool execCommand(Command& cmd, std::string& response_out) { return false; }
    virtual void setChannel(ControlChannel* channel) {}
    virtual void closeChannel(ControlChannel* channel) {}

private:
    blitz::Url m_murl;
    deadline_timer m_timer;
    blitz::http::MediaHTTPClient m_client;
    ControlChannel* channel;

    void handleReconnect(const boost::system::error_code& error);
};

} // blitz


#endif /* HTTP_SOURCE_HPP_ */
