/*
 * NetConnection.h
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
 *  Created on: Aug 21, 2013
 *      Author: emo
 */

#ifndef NETCONNECTION_H_
#define NETCONNECTION_H

#include <string>
#include "system/Buffer.h"
#include "ErrorCode.h"

enum IOAction { IORead = 0, IOWrite, IOReadSome, IOWriteSome };
class ChannelObserver;

class IOChannel
{
public:

    /**
    * Read buffer size data from connection .
    * @param Buffer - reference to buffer where data will be stored.
    * All listeners will be notified when read completes.
    */
    virtual void Emit(Buffer* data, IOAction ioOper) = 0;

    /**
    * Return unique connection ID. Each ID is allocated only from server.
    * @return unsigned - connection ID.
    */
    virtual unsigned GetSessionId() = 0;

    virtual unsigned GetChannelId() = 0;
};

/**
* Abstract base class (Interface) for IOChannel observer.
*/
class IOChannelObserver
{
public:
    /**
    * Triggered when read operation on connection is complete.
    * @param conn - pointer to NetConnection object.
    */
    virtual void OnRead(IOChannel* chan, std::size_t bytesRead) = 0;

    virtual void OnWrite(IOChannel* chan, std::size_t bytesWriten) = 0;

    virtual void OnConnectionClose(IOChannel* chan) = 0;
};

/**
* Abstract base class (Interface) for network connections.
*/
class NetConnection
{
public:
    /**
    * Return connection status.
    * @return bool - true if connected false otherwise.
    */
    virtual bool IsConnected() = 0;

    /**
    * Return unique connection ID. Each ID is allocated only from server.
    * @return unsigned - connection ID.
    */
    virtual unsigned GetSessionId() = 0;

    /**
    * Get the IP addresses the client is connected from.
    * @return string - IP address.
    */
    virtual std::string GetRemoteAddress() = 0;

    /**
    * Get the port the client is connected from.
    * @return unsigned - net port.
    */
    virtual unsigned GetRemotePort() = 0;

    /**
    * Close the connection. All listeners will be notified.
    */
    virtual void Close() = 0;

    virtual IOChannel* GetChannel(unsigned channelId) = 0;

    virtual IOChannel* OpenChannel(IOChannelObserver* inListener) = 0;

    virtual void CloseChannel(unsigned channelId) = 0;

    virtual void NotifyChannel(unsigned channelId, IOAction ioOper) = 0;

};



#endif /* NETCONNECTION_H_ */
