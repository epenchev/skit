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
#include "SocketInterface.h"
#include "Buffer.h"
#include "ErrorCode.h"

class TCPClientSocket : public ClientSocket
{
public:
    TCPClientSocket();
    virtual ~TCPClientSocket() {}

    /* From ClientSocket */
    void Send(Buffer& inData);

    /* From ClientSocket */
    void Receive(Buffer& outData);

    /* From ClientSocket */
    void ReceiveSome(Buffer& outData);

    /* From ClientSocket */
    std::string GetRemotePeerIP();

    /* From ClientSocket */
    unsigned short GetRemotePeerPort();

    /* From ClientSocket */
    void Connect(std::string host, std::string netService);

    /* From ClientSocket */
    void Disconnect();

    /* From ClientSocket */
    void AttachSocketListener(ClientSocketObserver* inListener);

    /* From ClientSocket */
    void RemoveSocketListener();

    /* From ClientSocket */
    ErrorCode& GetLastError() { return mErrCode; }

    std::size_t GetBytesTransfered() { return mBytesTransferred; }

private:
    boost::asio::ip::tcp::socket mIOSock;         /**< socket object */
    ClientSocketObserver*        mEventListener;  /**< socket observer for events */
    ErrorCode  mErrCode;                          /**< error code of last socket operation */
    std::size_t mBytesTransferred;                /**< bytes transferred by last socket operation */

    /* boost socket IO handlers */
    void HandleWrite(const boost::system::error_code& error, std::size_t bytes_transferred);
    void HandleRead(const boost::system::error_code& error, std::size_t bytes_transferred);
    void HandleResolve(const boost::system::error_code& err,
                         boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
    void HandleConnect(const boost::system::error_code& err);

    friend class TCPServerSocket;
};

#endif /* TCPCLIENTSOCKET_H_ */
