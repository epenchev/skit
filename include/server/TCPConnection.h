/*
 * TCPConnection.h
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
 *  Created on: Aug 27, 2013
 *      Author: emo
 */

#ifndef TCPCONNECTION_H_
#define TCPCONNECTION_H_

#include "NetConnection.h"
#include "ErrorCode.h"
#include "system/TCPSocket.h"
#include "system/SystemThread.h"
#include "server/IOChannel.h"
#include <list>
#include <map>
#include <set>

struct ChannelEvent
{
    IOEvent ioevent;
    Buffer* data;
    unsigned chanid;
};

/**
* Class for TCP connection objects used in different TCP servers.
*/
class TCPConnection : public NetConnection, public TCPSocketHandler
{
public:
    TCPConnection(unsigned id, TCPSocket* inSocket);
    virtual ~TCPConnection();

    /* From NetConnection */
    bool IsConnected();

    /* From NetConnection */
    unsigned GetID() { return m_id; }

    /* From NetConnection */
    std::string GetRemoteAddress();

    /* From NetConnection */
    unsigned GetRemotePort();

    /* From NetConnection */
    void Disconnect();

    /* From NetConnection */
    IOChannel* OpenChannel(IOChannelListener* inListener);

    /* From NetConnection */
    void CloseChannel(IOChannel* channel);

    /* From NetConnection */
    void Notify(IOEvent event, Buffer* data, IOChannel* channel);

    /* From NetConnection */
    IOChannel* GetChannel(unsigned channelId);

    /* From NetConnection */
    void AddListener(NetConnectionListener* listener);

    /* From NetConnection */
    void RemoveListener(NetConnectionListener* listener);

protected:

    /* From TCPSocketHandler */
    void OnSend(TCPSocket* inSocket, std::size_t sendBytes, ErrorCode& inError);

    /* From TCPSocketHandler */
    void OnReceive(TCPSocket* inSocket, std::size_t receivedBytes, ErrorCode& inError);

    /* From TCPSocketHandler */
    void OnConnect(TCPSocket* inSocket, ErrorCode& inError) {}

    unsigned m_id;                                /**< unique connection ID  */
    TCPSocket* m_socket;                          /**< socket used with this connection */
    SystemMutex m_channelsLock;                   /**< lock when getting/adding channels from/to connection */
    SystemMutex m_writeLock;                      /**< lock from writing to socket */
    SystemMutex m_readLock;                       /**< lock for reading from socket */
    bool m_writeBusy;                             /**< socket busy writing */
    bool m_readBusy;                              /**< socket busy reading */
    std::set<NetConnectionListener*> m_listeners; /**< connection listeners/observers for connect and disconnect events */

    std::map<unsigned, IOChannel*> m_channels;    /**< Channels map id/channel */
    std::list<ChannelEvent> m_readChanEvents;     /**< Pending read channel events */
    std::list<ChannelEvent> m_writeChanEvents;    /**< Pending write channel events */
};


#endif /* TCPCONNECTION_H_ */
