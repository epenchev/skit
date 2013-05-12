/*
 * WebService.h
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
 *  Created on: Mar 27, 2013
 *      Author: emo
 */

#ifndef WEBSERVICE_H_
#define WEBSERVICE_H_

#include <boost/asio.hpp>
#include "ControlChannel.h"
#include "HttpClient.h"
#include "TcpServer.h"

namespace blitz {

enum WebSessionState { SERVICE_REQ_RECEIVED, SERVICE_CLOSED };

/*
 * Receiver session for the WebService class.
 */
class WebSession
 : public TCPConnection, public Subject
{
public:
     WebSession(boost::asio::io_service& io_service);
     virtual ~WebSession() {}

     // from TCPConnection
     virtual void start(void);
     virtual void close(void);

     inline WebSessionState getState() const { return m_state; }
     inline boost::asio::streambuf& getData() { return m_response; }

     void sendData(const void* data, std::size_t size);
private:
    boost::asio::streambuf m_response;
    WebSessionState m_state;

    void handleReadCommand(const boost::system::error_code& error, std::size_t bytes_transferred);
    void handleSendResponse(const boost::system::error_code& error);
    void handleTimeoutReceive(const boost::system::error_code& error);
    void handleTimeoutSend(const boost::system::error_code& error);
};

/**
* Control channel implemented as a web service with HTTP GET request.
* It can receive and send request.
*/
class WebService
 : public ControlChannel, public TCPServer, public Observer
{
public:
    WebService(boost::asio::io_service& io_service, const unsigned int tcp_port);
    virtual ~WebService() {}

    // from TcpServer
    virtual TCPConnection* createTCPConnection(boost::asio::io_service& io_service);

    // from Observer
    virtual void update(Subject* changed_subject);

    // from ControlChannel
    virtual void registerControler(Controler* contrl);
    virtual void unregisterControler(Controler* contrl);
    virtual void triggerEvent(ControlerEvent& ev);

private:
    void clearSessions(void);

    blitz::http::HTTPClient m_client;
    std::list<blitz::Controler*> m_controlers;
    std::vector<WebSession*> m_orphane_sessions;
};

} // blitz

#endif /* WEBSERVICE_H_ */
