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
#define NETCONNECTION_H_

#include "system/Buffer.h"
#include "utils/ErrorCode.h"
#include <string>

enum IOEvent { IORead = 0, IOReadSome, IOWrite };

class IOChannel;
class IOChannelListener;
class NetConnectionListener;

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
    * Return the unique connection ID.
    * Each ID is allocated only from server.
    * @return unsigned - connection ID.
    */
    virtual unsigned GetID() = 0;

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
    * Disconnect from remote host.
    */
    virtual void Disconnect() = 0;

    /**
    * Get instance to IO channel associated with this id.
    * @param id - unique id of the channel.
    * @return IOChannel instance.
    */
    virtual IOChannel* GetChannel(unsigned id) = 0;

    /**
    * Open channel to NetConnection.
    * @param inListener - IOChannelListener listening for channel I/O events.
    * @return IOChannel - newly allocated instance.
    */
    virtual IOChannel* OpenChannel(IOChannelListener* listener) = 0;

    /**
    * Close IO channel.
    * @param channel - IOChannel instance to close.
    */
    virtual void CloseChannel(IOChannel* channel) = 0;

    /**
    * Notify connection for pending I/O operation on channel.
    * @param event - I/O event to be performed.
    * @param data - Buffer instance for I/O operation.
    * @param channel - IOChannel instance.
    */
    virtual void Notify(IOEvent event, Buffer* data, IOChannel* channel) = 0;

    /**
    * Adds a listener for connection events to this object.
    * @param listener - listener object.
    */
    virtual void AddListener(NetConnectionListener* listener) = 0;

    /**
    * Removes a listener for connection events to this object.
    * @param listener - listener object.
    */
    virtual void RemoveListener(NetConnectionListener* listener) = 0;
};

/**
* Interface for listeners to connection events.
*/
class NetConnectionListener
{
public:

    /* A new connection was established. */
    virtual void OnConnectionOpen(NetConnection* conn) {}

    /* A connection was disconnected. */
    virtual void OnConnectionClose(NetConnection* conn) {}
};

#endif /* NETCONNECTION_H_ */
