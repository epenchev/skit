/*
 * MediaHtppClient.h
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

#ifndef MEDIAHTPPCLIENT_H_
#define MEDIAHTPPCLIENT_H_

#include "HttpClient.h"
#include "DataSource.h"

#define Network_Read_Timeout_Err_Code 598

namespace blitz {
namespace http {

/**
* HTTP Media Client extending HTTPClient reading media streams.
*/
class MediaHTTPClient : public HTTPClient
{
public:
    MediaHTTPClient(boost::asio::io_service& io_service) : HTTPClient(io_service) {}
    virtual ~MediaHTTPClient() {}

    // from Subject
    virtual void attach(Observer* ob);
protected:
    // from HTTPClient
    virtual void readContent();
private:
    DataSource* m_source;
    DataPacket* m_packet;
    void handleReadContent(const boost::system::error_code& error, std::size_t bytes_transferred);
    void handleDeadline(const boost::system::error_code& error);
    const static unsigned receive_time = 300; /**< max seconds to wait for data */
};

} // http
} // blitz

#endif /* MEDIAHTPPCLIENT_H_ */
