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
#include <boost/asio.hpp>
#include "TCPClientSocket.h"
#include "ErrorCode.h"
#include "SocketObserver.h"

class TCPServerSocket
{
public:
    TCPServerSocket(unsigned short port);
    TCPServerSocket(std::string localAdress, unsigned short port);
    virtual ~TCPServerSocket();

    /**
    * Start accepting incoming connections.
    * @param outError - error from operation.
    */
    void Accept(ErrorCode& outError);

    /**
    * Return the port socket is listening on.
    * @return port value.
    */
    unsigned short GetListeningPort();

    /**
    * Listen for incoming connections.
    * @param outError - error from operation.
    */
    void Listen(ErrorCode& outError);

    /**
    * Set observer object who will be notified about every socket event.
    * @param inListener - pointer to observer object.
    * @param outError - error from operation.
    */
    void SetListener(TCPServerSocketObserver* inListener, ErrorCode& outError);

    /**
    * Close listening socket and stop accepting.
    */
    void Stop();

private:

    boost::asio::ip::tcp::acceptor mAcceptor;       /**< boost listener object */
    unsigned short                 mlocalPort;      /**< listening port number */
    std::string                    mlocalAdress;    /**< listening IP address as string */
    TCPServerSocketObserver*       mEventListener;  /**< socket observer for events */
    bool                           misListening;    /**< is socket in listening state */
    ErrorCode*                     mOutError;          /**< error code of accept operation */

    /* boost tcp::acceptor IO handlers */
    void HandleAccept(TCPClientSocket* outSocket, const boost::system::error_code& error);
};

#endif /* TCPSERVERSOCKET_H_ */
