/*
 * MediaHttpServer.h
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
 *  Created on: Jan 22, 2013
 *      Author: emo
 */

#ifndef MEDIA_HTTPSERVER_H_
#define MEDIA_HTTPSERVER_H_

#include <set>
#include <vector>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "DataPacket.h"
#include "TcpServer.h"
#include "Observer.h"
#include <boost/shared_ptr.hpp>

using boost::asio::deadline_timer;

namespace blitz {

typedef boost::shared_ptr<DataPacket> PacketPtr;

enum MediaHTTPConnectionState { STATE_OPEN, STATE_CLOSED, STATE_REQ_RECEIVED };

/**
* HTTP connection.
* HTTP client session to server.
* Uses boost asio for socket communication API.
*/
class MediaHTTPConnection
: public TCPConnection, public Subject
{
public:
    MediaHTTPConnection(boost::asio::io_service& io_service);
    virtual ~MediaHTTPConnection();

    inline MediaHTTPConnectionState getState() const { return state; }
    virtual void start(void);
    virtual void close(void);

    void addData(PacketPtr ptr);
    inline std::string getResouceId() const { return m_resource_id; }

    inline bool getApproved() const { return m_connection_approved; }
    inline void setApproved(bool status = true) { m_connection_approved = status; }

private:
    void handleReadHeader(const boost::system::error_code& error);
    void handleWriteContent(const boost::system::error_code& error);
    void handleWriteHeader(const boost::system::error_code& error);
    void handleTimeoutReceive(const boost::system::error_code& error);
    void handleTimeoutSend(const boost::system::error_code& error);

    bool m_connection_is_busy;
    bool m_connection_approved;
    std::string m_resource_id;
    boost::asio::streambuf m_response;
    MediaHTTPConnectionState state;
    std::list<PacketPtr> m_packets;
    deadline_timer m_io_control_timer;

    const static unsigned receive_timeout = 60; /**< max seconds to wait for data */
    const static unsigned send_timeout = 30;  /**< max seconds to wait for any async_send to complete */
    const static unsigned max_queue_size = 1024;
};

/**
* HTTP Server.
* Easy to use server for serving data resources over HTTP protocol.
* Uses boost asio for socket communication API.
* Maintains multiple client connections.
*/
class MediaHTTPServer
    : public TCPServer , public Observer
{
public:
    MediaHTTPServer(boost::asio::io_service& io_service, const unsigned int tcp_port);

    virtual ~MediaHTTPServer() {};
    void sendPacket(DataPacket* packet);

protected:
    // from TCPServer
    virtual void handleStartServer(void) {}
    virtual void handleStopServer(void) {}
    virtual TCPConnection* createTCPConnection(boost::asio::io_service& io_service);
    // from observer
    virtual void update(Subject* changed_subject);

private:
    deadline_timer m_activity_timer;
    void handleDeadline(const boost::system::error_code& error);
    const static unsigned no_data_timeout = 600; /**< max seconds to wait for data,
                                                  if no data is present in given the timeout all connections are closed */
    void clearConnections(void);
    std::vector<MediaHTTPConnection*> m_orphane_connections;
};

} // blitz

#endif /* MEDIA_HTTPSERVER_H_ */
