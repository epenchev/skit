/*
 * SocketInterface.h
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

#ifndef SOCKETINTERFACE_H_
#define SOCKETINTERFACE_H_

#include <string>

class ServerSocketObserver;
class ClientSocketObserver;
class Buffer;
class ErrorCode;

/**
* Virtual Class interface for server socket communication.
*/
class ServerSocket
{
public:

    /**
    * Start accepting incoming connections.
    */
    virtual void Accept() = 0;

    /**
    * Return the port socket is listening on.
    * @return port value.
    */
    virtual unsigned short GetPort() = 0;

    /**
    * Listen for incoming connections.
    */
    virtual void Listen() = 0;

    /**
    * Add observer object who will be notified about every socket event.
    * @param inListener - pointer to observer object.
    */
    virtual void AddSocketListener(ServerSocketObserver* inListener) = 0;

    /**
    * Remove observer object from Socket.
    */
    virtual void RemoveSocketListener() = 0;
};

/**
* Virtual Class interface for client socket communication.
*/
class ClientSocket
{
public:
    /**
    * Send data from Buffer object.
    * This operation is not thread safe.
    * When you call Send() you must wait for this operation to complete before sending more data out.
    * @param inData - reference to data to be sent.
    */
    virtual void Send(Buffer& inData) = 0;

    /**
    * Receive data to Buffer object until all buffer size is full.
    * This operation is not thread safe.
    * When you call Receive() you must wait for this operation to complete.
    * @param outData - reference to data.
    */
    virtual void Receive(Buffer& outData) = 0;

    /**
    * Receive available data from socket.
    * @param outData - reference to data.
    */
    virtual void ReceiveSome(Buffer& outData) = 0;

    /**
    * Get the IP address to the connected endpoint.
    * @return string - reference representing IP address.
    */
    virtual std::string GetRemotePeerIP() = 0;

    /**
    * Get the port to the connected endpoint.
    * @return port value.
    */
    virtual unsigned short GetRemotePeerPort() = 0;

    /**
    * Connect to remote host.
    * @param host - remote host name or IP address.
    * @param netService - remote TCP/UDP port or service name.
    */
    virtual void Connect(std::string host, std::string netService) = 0;

    /**
    * Disconnect from remote host.
    */
    virtual void Disconnect() = 0;

    /**
    * Add observer object who will be notified about every socket event.
    * A socket object can have only one observer listening for events.
    * @param inListener - pointer to observer object.
    */
    virtual void AttachSocketListener(ClientSocketObserver* inListener) = 0;

    /**
    * Remove observer object from Socket.
    */
    virtual void RemoveSocketListener() = 0;

    /**
    * Get the last error from this socket.
    * @return SocketError reference.
    */
    virtual ErrorCode& GetLastError() = 0;
};

/**
* Class interface for ServerSocket observers. Observe for events on server sockets.
* Most socket operations are asynchronous so implementing this interface is vital.
*/
class ServerSocketObserver
{
public:
    /**
    * Triggered when a new connection is accepted on a ServerSocket.
    * @param inNewSocket - pointer to newly created socket object.
    * @param inErr - error code of the operation.
    */
    virtual void OnAccept(ClientSocket* inNewSocket) = 0;
};

/**
* Class interface for ClientSocket observers. Observe for events on client sockets.
* Most socket operations are asynchronous so implementing this interface is vital.
*/
class ClientSocketObserver
{
public:
    /**
    * Triggered when a send operation is complete on a ClientSocket object.
    * @param inSocket - pointer to client socket object.
    */
    virtual void OnSend(ClientSocket* inSocket) = 0;

    /**
    * Triggered when a receive operation is complete on a ClientSocket object.
    * @param inSocket - pointer to client socket object.
    */
    virtual void OnReceive(ClientSocket* inSocket) = 0;

    /**
    * Triggered when a connect operation is complete on a ClientSocket object.
    * @param inSocket - pointer to client socket object.
    */
    virtual void OnConnect(ClientSocket* inSocket) = 0;
};

#endif /* SOCKETINTERFACE_H_ */



