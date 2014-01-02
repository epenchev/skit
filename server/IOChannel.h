/*
 * IOChannel.h
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
 *  Created on: Nov 22, 2013
 *      Author: emo
 */

#ifndef IOCHANNEL_H_
#define IOCHANNEL_H_

#include "system/Buffer.h"
#include "utils/ErrorCode.h"
#include "server/NetConnection.h"
#include "system/SystemThread.h"

class IOChannelListener;

/** Objects of this class are used to perform I/O operations on NetConnection. */
class IOChannel
{
public:
    IOChannel(unsigned id, IOChannelListener* listener, NetConnection* conn);
    virtual ~IOChannel();

    /**
    * Write data from Buffer into NetConnection thru channel.
    * @param data - reference to Buffer to write into.
    * Channel listener will be notified when write completes.
    */
    void Write(Buffer& data);

    /**
    * Read data into Buffer from NetConnection.
    * @param data - reference to Buffer where data will be stored.
    * @param read_some - if true will read only what is available in connection buffer,
    * otherwise will wait until data is full. Channel listener will be notified when read completes.
    */
    void Read(Buffer& data, bool read_some = false);

    /**
    * Get the connection this channel is open from.
    * @return NetConnection* - pointer to NetConnection
    */
    NetConnection* GetConnection() { return m_connection; }

    /**
    * Return the ID of this channel.
    * @return unsigned - channel ID.
    */
    unsigned GetID() { return m_channelId; }

    /**
    * Channel is notified from connection for event complete.
    * @param event - IOEvent operation.
    * @param err - error code of the IO operation.
    * @param iobytes - bytes transfered.
    */
    void Notify(IOEvent event, ErrorCode& err, std::size_t iobytes);

private:
    unsigned m_channelId;          /**< Channel ID */
    SystemMutex m_writelock;        /**< write buffer lock */
    SystemMutex m_readlock;         /**< read buffer lock */
    Buffer* m_writebuff;            /**< buffer for write operations  */
    Buffer* m_readbuff;             /**< buffer for read operations  */
    NetConnection* m_connection;    /**< NetConnection object this channel is associated with  */
    IOChannelListener* m_listener;  /**< Channel listener/observer to be called on operation complete  */
};

/**
* Abstract base class (Interface) for IOChannel listener/observer.
*/
class IOChannelListener
{
public:
    /**
    * Triggered when read operation on connection from this channel is complete.
    * @param chan - pointer to IOChannel object.
    * @param  bytesRead - bytes read.
    * @param err - error code of the operation.
    */
    virtual void OnRead(IOChannel* chan, std::size_t bytesRead, ErrorCode& err) {}

    /**
    * Triggered when write operation on connection from this channel is complete.
    * @param chan - pointer to IOChannel object.
    * @param  bytesWritten - bytes written.
    * @param err - error code of the operation.
    */
    virtual void OnWrite(IOChannel* chan, std::size_t bytesWritten, ErrorCode& err) {}
};


#endif /* IOCHANNEL_H_ */
