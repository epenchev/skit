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

enum IOAction { IORead = 0, IOReadSome, IOWrite };
class ChannelObserver;

class IOChannel
{
public:
    /**
    * Read buffer size data from connection .
    * @param Buffer - reference to buffer where data will be stored.
    * All listeners will be notified when read completes.
    */
    virtual void Emit(Buffer* data, IOAction ioOper, ErrorCode& outError) = 0;

    /**
    * Return the unique connection ID. Each ID is allocated only from server.
    * @return unsigned - connection ID.
    */
    virtual unsigned GetConnId() = 0;

    /**
    * Return the ID of this channel.
    * @return unsigned - channel ID.
    */
    virtual unsigned GetChannelId() = 0;
};

/**
* Abstract base class (Interface) for IOChannel observer.
*/
class IOChannelObserver
{
public:
    /**
    * Triggered when read operation on connection from this channel is complete.
    * @param chan - pointer to IOChannel object.
    * @param  bytesRead - bytes read.
    * @param inErr - error code of the operation.
    */
    virtual void OnRead(IOChannel* chan, std::size_t bytesRead, ErrorCode* inErr) = 0;

    /**
    * Triggered when read operation on connection from this channel is complete.
    * @param chan - pointer to IOChannel object.
    * @param  bytesWritten - bytes written.
    * @param inErr - error code of the operation.
    */
    virtual void OnWrite(IOChannel* chan, std::size_t bytesWritten, ErrorCode* inErr) = 0;

    /**
    * Triggered when connection is closed.
    * @param chan - pointer to IOChannel object.
    */
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
    virtual unsigned GetConnId() = 0;

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

    /**
    * Get instance to IO channel associated with channel id.
    * @param channelId - id of the channel.
    * @return IOChannel instance.
    */
    virtual IOChannel* GetChannel(unsigned channelId) = 0;

    /**
    * Open channel to NetConnection.
    * @param inListener - IOChannelObserver listening for channel events.
    * @return IOChannel - newly allocated instance.
    */
    virtual IOChannel* OpenChannel(IOChannelObserver* inListener) = 0;

    /**
    * Close IO channel associated with channel id.
    * @param channelId - id of the channel.
    */
    virtual void CloseChannel(unsigned channelId) = 0;

    /**
    * Notify connection for pending IO operation on IO channel.
    * @param channelId - id of the channel.
    * @param ioOper - IO operation to be performed ex.(read/write).
    * @param outErr - return error code from operation.
    */
    virtual void NotifyChannel(unsigned channelId, IOAction ioOper, ErrorCode& outErr) = 0;
};



#endif /* NETCONNECTION_H_ */
