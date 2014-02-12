/*
 * TCPSocket.h
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

#ifndef TCPSOCKET_H_
#define TCPSOCKET_H_

#include <boost/asio.hpp>
#include "utils/Buffer.h"
#include "utils/ErrorCode.h"

class TCPSocket;

/**
* Class interface for TCPSocket handler operations. All socket I/O operations are asynchronous,
* in order to check when a socket I/O operation is complete this interface class must be used to implement such handlers.
*/
class TCPSocketHandler
{
public:
    /**
    * Triggered when a Send() operation is complete on a Socket object.
    * @param inSocket - pointer to socket object.
    * @param sendBytes - count bytes send.
    * @param inErr - error code of the operation.
    */
    virtual void OnSend(TCPSocket* inSocket, std::size_t sendBytes, ErrorCode& inError) {}

    /**
    * Triggered when a Receive() or ReceiveSome() operation/s is complete on a Socket object.
    * @param inSocket - pointer to socket object.
    * @param receivedBytes - count bytes received.
    * @param inErr - error code of the operation.
    */
    virtual void OnReceive(TCPSocket* inSocket, std::size_t receivedBytes, ErrorCode& inError) {}

    /**
    * Triggered when a Connect() operation is complete on a Socket object.
    * @param inSocket - pointer to socket object.
    * @param inErr - error code of the operation.
    */
    virtual void OnConnect(TCPSocket* inSocket, ErrorCode& inError) {}
};

/**
* Class for TCP socket based on boost boost::asio::ip::tcp::socket.
*/
class TCPSocket
{
public:
    TCPSocket();
    virtual ~TCPSocket();

    /**
    * Send data from Buffer asynchronously.
    * When operation is complete TCPSocketHandler::OnSend() will be called.
    * @param inData - reference to data to be sent.
    * @param handler - The handler object to be signaled when the I/O operation completes.
    */
    void Send(Buffer& inData, TCPSocketHandler* handler);

    /**
    * Receive data in to Buffer asynchronously. Operation will complete when buffer is full.
    * When operation is complete TCPSocketHandler::OnReceive() will be called.
    * @param outData - reference to buffer for storing data.
    * @param handler - The handler object to be signaled when the I/O operation completes.
    */
    void Receive(Buffer& outData, TCPSocketHandler* handler);

    /**
    * Receive data in to Buffer asynchronously. May not read all requested bytes.
    * Will read only available data in the moment.
    * When operation is complete TCPSocketHandler::OnReceive() will be called.
    * @param outData - reference to buffer for storing data.
    * @param handler - The handler object to be signaled when the I/O operation completes.
    */
    void ReceiveSome(Buffer& outData, TCPSocketHandler* handler);

    /**
    * Get the IP address to the connected endpoint.
    * @param outError - error from operation.
    * @return string - reference representing IP address.
    */
    std::string GetRemoteIP(ErrorCode& outError) const;

    /**
    * Get the port to the connected endpoint.
    * @param outError - error from operation.
    * @return port value.
    */
    unsigned short GetRemotePort(ErrorCode& outError) const;

    /**
    * Connect to remote host.
    * @param host - remote host name or IP address.
    * @param netService - remote TCP/UDP port or service name.
    */
    void Connect(std::string host, std::string netService, TCPSocketHandler* handler);

    /**
    * Return true if socket is open false otherwise.
    */
    bool IsOpen() const;

    /**
    * Disconnect from remote host.
    */
    void Close();

    /* Total number of bytes send */
    unsigned long GetSendBytes();

    /* Total number of bytes received. */
    unsigned long GetRecvBytes();

private:
    boost::asio::ip::tcp::socket m_socket;     /**< boost socket object */
    ErrorCode                    m_error;      /**< error code of last socket operation */
    unsigned long                m_recvbytes;  /**< total receive bytes counter */
    unsigned long                m_sendbytes;  /**< total send bytes counter */

    /* boost socket IO handlers */
    void HandleWrite(const boost::system::error_code& error, std::size_t bytes_transferred, TCPSocketHandler* handler);
    void HandleRead(const boost::system::error_code& error, std::size_t bytes_transferred, TCPSocketHandler* handler);
    void HandleConnect(const boost::system::error_code& err, TCPSocketHandler* handler);
    void HandleResolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator, TCPSocketHandler* handler);

    friend class TCPAcceptor;
};

#endif /* TCPSOCKET_H_ */
