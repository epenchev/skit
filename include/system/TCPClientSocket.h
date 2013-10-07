/*
 * TCPClientSocket.h
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
 *  Created on: Jun 17, 2013
 *      Author: emo
 */

#ifndef TCPCLIENTSOCKET_H_
#define TCPCLIENTSOCKET_H_

#include <boost/asio.hpp>
#include "SocketObserver.h"
#include "Buffer.h"
#include "ErrorCode.h"

class TCPClientSocket
{
public:
    TCPClientSocket();
    virtual ~TCPClientSocket();

    /**
    * Send data from Buffer asynchronously.
    * When operation is complete ClientSocketObserver::OnSend() will be called.
    * @param inData - reference to data to be sent.
    * @throws SystemException in case of error
    */
    void Send(Buffer& inData);

    /**
    * Receive data in to Buffer asynchronously. Operation will complete when buffer is full.
    * When operation is complete ClientSocketObserver::OnReceive() will be called.
    * @param outData - reference to buffer for storing data.
    * @throws SystemException in case of error.
    */
    void Receive(Buffer& outData);

    /**
    * Receive data in to Buffer asynchronously. May not read all requested bytes.
    * Will read only available data in the moment.
    * When operation is complete ClientSocketObserver::OnReceive() will be called.
    * @param outData - reference to buffer for storing data.
    * @throws SystemException in case of error.
    */
    void ReceiveSome(Buffer& outData);

    /**
    * Get the IP address to the connected endpoint.
    * @param outError - error from operation.
    * @return string - reference representing IP address.
    */
    std::string GetRemotePeerIP(ErrorCode& outError);

    /**
    * Get the port to the connected endpoint.
    * @param outError - error from operation.
    * @return port value.
    */
    unsigned short GetRemotePeerPort(ErrorCode& outError);

    /**
    * Connect to remote host.
    * @param host - remote host name or IP address.
    * @param netService - remote TCP/UDP port or service name.
    */
    void Connect(std::string host, std::string netService);

    /**
    * Return true if socket is open false otherwise.
    */
    bool IsOpen();

    /**
    * Disconnect from remote host.
    */
    void Disconnect();

    /**
    * Add observer object who will be notified about every socket event.
    * A socket object can have only one observer listening for events.
    * @param inListener - pointer to observer object.
    * @param outError - error from operation.
    */
    void SetListener(TCPClientSocketObserver* inListener, ErrorCode& outError);

private:
    boost::asio::ip::tcp::socket mIOSock;            /**< socket object */
    TCPClientSocketObserver*     mEventListener;     /**< socket observer for events */
    ErrorCode*                   mOutError;          /**< error code of last socket operation */

    /* boost socket IO handlers */
    void HandleWrite(const boost::system::error_code& error, std::size_t bytes_transferred);
    void HandleRead(const boost::system::error_code& error, std::size_t bytes_transferred);
    void HandleResolve(const boost::system::error_code& err,
                         boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
    void HandleConnect(const boost::system::error_code& err);

    friend class TCPServerSocket;
};

#endif /* TCPCLIENTSOCKET_H_ */
