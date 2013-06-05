/*
 * HttpSink.h
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

#ifndef HTTPSINK_H_
#define HTTPSINK_H_

#include <string>
#include <boost/asio.hpp>
#include "DataSink.h"
#include "DataPacket.h"
#include "MediaHttpServer.h"
#include "ControlChannel.h"
#include "MediaSessionDB.h"

using boost::asio::deadline_timer;

namespace blitz {

enum HTTPSinkCommands { CreateSeesionID = 1, GetConnectionCount, GetConnectedPeers };

class HttpSink : public DataSink, public Controler
{
public:
    HttpSink(boost::asio::io_service& io_service, short port, std::string& ip_address, std::string& name, unsigned short conf_id);
    virtual ~HttpSink() {}
    virtual void write(DataPacket *p);

    // from Controler
    virtual bool execCommand(Command& cmd, std::string& response_out);
    virtual void setChannel(ControlChannel* channel) { this->channel = channel; }
    virtual void closeChannel(ControlChannel* channel) { this->channel = (blitz::ControlChannel*) 0;}

private:
    void sessionloop(const boost::system::error_code& error);
    const static unsigned int session_loop_timeout = 1; // seconds

    deadline_timer m_session_timer;
    blitz::MediaSessionDB m_dbase;
    MediaHTTPServer server;
    ControlChannel* channel;
    std::string m_name;
    unsigned short m_id;
    unsigned short m_port;
    std::string m_ip_address;
};

} // blitz

#endif /* HTTPSINK_H_ */
