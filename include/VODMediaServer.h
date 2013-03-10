/*
 * VODMediaServer.h
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
 *  Created on: Feb 28, 2013
 *      Author: emo
 */

#ifndef VODMEDIASERVER_H_
#define VODMEDIASERVER_H_

#include <boost/asio.hpp>
#include "MediaHttpServer.h"
#include "DataSource.h"
#include "FsSource.h"
#include <map>

namespace blitz {

class VODMediaServer : public MediaHTTPServer
{
public:
    VODMediaServer(boost::asio::io_service& io_service, const unsigned int tcp_port);
    virtual ~VODMediaServer() {}

    // from MediaHTTPServer
    virtual TCPConnection* createTCPConnection(boost::asio::io_service& io_service);

    // from MediaHTTPServer
    virtual void update(Subject* changed_subject);
private:
    std::map<std::string, blitz::DataSource*> m_sources;
    std::map<std::string, std::string> m_filenames;
    FsSource* m_source;
};

} // blitz

#endif /* VODMEDIASERVER_H_ */
