/*
 * SocketObserver.h
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
 *  Created on: Oct 1, 2013
 *      Author: emo
 */

#ifndef SOCKETOBSERVER_H_
#define SOCKETOBSERVER_H_

class ErrorCode;
class TCPClientSocket;

/**
* Class interface for ServerSocket observers. Observe for events on server sockets.
* Most socket operations are asynchronous so implementing this interface is vital.
*/
class TCPServerSocketObserver
{
public:
    /**
    * Triggered when a new connection is accepted on a ServerSocket.
    * @param inNewSocket - pointer to newly created socket object.
    * @param inErr - error code of the operation.
    */
    virtual void OnAccept(TCPClientSocket* inNewSocket, ErrorCode* inError) = 0;
};

/**
* Class interface for ClientSocket observers. Observe for events on client sockets.
* Most socket operations are asynchronous so implementing this interface is vital.
*/
class TCPClientSocketObserver
{
public:
    /**
    * Triggered when a send operation is complete on a ClientSocket object.
    * @param inSocket - pointer to client socket object.
    * @param sendBytes - count bytes send.
    * @param inErr - error code of the operation.
    */
    virtual void OnSend(TCPClientSocket* inSocket, unsigned sendBytes, ErrorCode* inError) = 0;

    /**
    * Triggered when a receive operation is complete on a ClientSocket object.
    * @param inSocket - pointer to client socket object.
    * @param receivedBytes - count bytes received.
    * @param inErr - error code of the operation.
    */
    virtual void OnReceive(TCPClientSocket* inSocket, unsigned receivedBytes, ErrorCode* inError) = 0;

    /**
    * Triggered when a connect operation is complete on a ClientSocket object.
    * @param inSocket - pointer to client socket object.
    * @param inErr - error code of the operation.
    */
    virtual void OnConnect(TCPClientSocket* inSocket, ErrorCode* inError) = 0;
};

#endif /* SOCKETOBSERVER_H_ */
