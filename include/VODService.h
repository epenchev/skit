/*
 * VODService.h
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

#ifndef VODSERVICE_H_
#define VODSERVICE_H_

#include <boost/asio.hpp>
#include <map>
#include <vector>
#include "TcpServer.h"
#include "DataSink.h"
#include "DataPacket.h"
#include "Observer.h"
#include "DataSource.h"
#include "FileReader.h"
#include "MediaSessionDB.h"
#include "ControlChannel.h"

using boost::asio::deadline_timer;

namespace blitz {

enum VODSessionState { VOD_STATE_OPEN, VOD_STATE_CLOSED, VOD_STATE_REQ_RECEIVED };
enum VODResponse { VOD_STATE_OK, VOD_STATEREJECT };
enum VODServiceCommands { VCreateSeesionID = 1, VGetConnectionCount, VGetConnectedPeers };

class VODSession : public TCPConnection, public Subject, public DataSink
{
public:
    VODSession(boost::asio::io_service& io_service);
    virtual ~VODSession() {}

    // from DataSink
    virtual void write(blitz::DataPacket* p);

    // from TCPSession
    virtual void start(void);
    virtual void close(void);

    inline std::string getResouceId() const { return m_resource_id; }
    inline VODSessionState getState() const { return m_state; }

    void openFile(std::string& filename);
    void sendHeaders(VODResponse response_type);

private:
    void handleReadHeader(const boost::system::error_code& error);
    void handleWriteContent(const boost::system::error_code& error);
    void handleWriteHeader(const boost::system::error_code& error);
    void handleTimeoutReceive(const boost::system::error_code& error);
    void handleTimeoutSend(const boost::system::error_code& error);
    void handleWriteReject(const boost::system::error_code& error);

    std::string m_resource_id;
    boost::asio::streambuf m_client_request;
    VODSessionState m_state;
    deadline_timer m_io_control_timer;
    FileReader m_freader;
    DataSource m_source;
    DataPacket m_packet;
    std::string m_filename;

    const static unsigned receive_timeout = 60; /**< max seconds to wait for data */
    const static unsigned send_timeout = 5;     /**< max seconds to wait for any async_send to complete */
};

class VODService : public TCPServer, public Observer, public Controler
{
public:
    VODService(boost::asio::io_service& io_service, const unsigned int tcp_port, std::string ip_address, std::string filepath);
    virtual ~VODService() {}

    // from TcpServer
    virtual TCPConnection* createTCPConnection(boost::asio::io_service& io_service);

    // from Observer
    void update(Subject* changed_subject);

    // from Controler
    virtual bool execCommand(Command& cmd, std::string& response_out);
    virtual void setChannel(ControlChannel* channel) { this->channel = channel; }
    virtual void closeChannel(ControlChannel* channel) { this->channel = (blitz::ControlChannel*) 0;}

private:
    void clearSessions(void);

    std::vector<VODSession*> m_orphane_sessions;
    std::map<std::string, std::string> m_filenames;
    blitz::MediaSessionDB m_dbase;
    ControlChannel* channel;
    std::string m_ip_address;
    std::string m_basepath;
    unsigned short m_port;
};

} // blitz

#endif /* VODSERVICE_H_ */
