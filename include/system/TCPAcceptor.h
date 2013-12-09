/*
 * TCPAcceptor.h
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

#ifndef TCPACCEPTOR_H_
#define TCPACCEPTOR_H_

#include <string>
#include <boost/asio.hpp>
#include "TCPSocket.h"
#include "ErrorCode.h"

/**
* Class interface for TCPAcceptor handler operations. All socket I/O operations are asynchronous,
* in order to check when a socket I/O operation is complete this interface class must be used to implement such handlers.
*/
class TCPAcceptorHandler
{
public:
    /**
    * Triggered when a new connection is accepted.
    * @param inNewSocket - pointer to newly created socket object.
    * @param inErr - error code of the operation.
    */
    virtual void OnAccept(TCPSocket* inNewSocket, ErrorCode& inError) = 0;
};

class TCPAcceptor
{
public:
    TCPAcceptor(unsigned short port);
    TCPAcceptor(std::string localAddress, unsigned short port);
    virtual ~TCPAcceptor();

    /**
    * Start accepting incoming connections.
    * SetListener() must be called before Start() otherwise exception will be thrown .
    * @throws SystemException in case of error.
    */
    void Start();

    /**
    * Close Acceptor object and stop accepting.
    */
    void Stop();

    /**
    * Return the port socket is listening on.
    * @return port value.
    */
    unsigned short GetListeningPort() const;

    /**
    * Listen for incoming connections.
    * @throws SystemException in case of error.
    */
    void Listen();

    /**
    * Set I/O handler object to be called on every incoming connection.
    * @param handler - pointer to handler object.
    */
    void SetHandler(TCPAcceptorHandler* handler);

private:

    boost::asio::ip::tcp::acceptor m_acceptor;       /**< boost listener object */
    unsigned short               m_localPort;      /**< listening port number */
    std::string                    m_localAddress;   /**< listening IP address as string */
    bool                          m_isListening;    /**< is socket in listening state */
    TCPAcceptorHandler*            m_iohandler;      /**< I/O handler */
    ErrorCode                      m_error;          /**< error code of accept operation */

    /**
    * Accepts incoming connection.
    */
    void Accept();

    /* boost tcp::acceptor IO handlers */
    void HandleAccept(TCPSocket* outSocket, const boost::system::error_code& error);
};

#endif /* TCPACCEPTOR_H_ */
