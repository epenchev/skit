/*
 * TCPServerSocket.h
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

#ifndef TCPSERVERSOCKET_H_
#define TCPSERVERSOCKET_H_

#include <string>
#include <exception>
#include <boost/asio.hpp>
#include "SocketInterface.h"
#include "TCPClientSocket.h"
#include "Buffer.h"
#include "ErrorCode.h"

class TCPServerSocket : public ServerSocket
{
public:
    TCPServerSocket(unsigned short port);
    TCPServerSocket(std::string localAdress, unsigned short port);
    virtual ~TCPServerSocket() {}

    /* From ServerSocket */
    void Accept();

    /* From ServerSocket */
    unsigned short GetPort();

    /* From ServerSocket */
    void Listen();

    /* From ServerSocket */
    void AddSocketListener(ServerSocketObserver* inListener);

    /* From ServerSocket */
    void RemoveSocketListener();

   /**
   * Get the last error from this socket.
   * @return SocketError reference.
   */
   ErrorCode& GetError() { return mErrCode; }

private:

    // temp
    ErrorCode mErrCode;

    boost::asio::ip::tcp::acceptor mAcceptor;
    unsigned short                 mlocalPort;
    std::string                    mlocalAdress;
    ServerSocketObserver*          mEventListener;  /**< socket observer for events */
    bool                           misListening;     /**< is socket in listening state */

    /* boost tcp::acceptor IO handlers */
    void HandleAccept(TCPClientSocket* outSocket, const boost::system::error_code& error);
};

#endif /* TCPSERVERSOCKET_H_ */
