/*
 * StreamClient.h
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
 *  Created on: Oct 10, 2013
 *      Author: emo
 */

#ifndef STREAMCLIENT_H_
#define STREAMCLIENT_H_

#include <cstddef>
#include <set>
#include "system/Buffer.h"

class NetConnection;

/**
* Client is an abstraction representing user connected to Blitz media server.
* Clients are tied to connections and subscribed to Streams.
*/
class StreamClient
{
public:
    StreamClient();
    virtual ~StreamClient();

    /**
    * Associate connection with client.
    * No thread safety provided for this call.
    * @param conn - pointer to NetConnection object.
    */
    void Register(NetConnection* conn);

    /**
    * Removes client-connection association for given connection.
    * No thread safety provided for this call.
    * @param conn - pointer to NetConnection object.
    */
    void Unregister(NetConnection* conn);

    /**
    * Returns the time at which the client was created.
    */
    unsigned long GetCreationTime();

    /**
    * Subscribe client to a Stream.
    * @param streamId - stream unique id.
    */
    void Subscribe(unsigned streamId);

    /**
    * Remove client subscription.
    * @param streamId - stream unique id.
    */
    void UnSubscribe(unsigned streamId);

    /**
    * Is client subscribed to a Stream.
    */
    bool IsSubscribed();

    /**
    *  Return set of connections for this client.
    */
    std::set<NetConnection*>& GetConnections();

    /**
    * Get properties for this client.
    */
    void GetProperties();

    /**
    * Returns the client id.
    */
    unsigned GetId();

    /**
    * Disconnects client connections. Event OnDisconnect() will be emitted.
    * Client will be destroyed after.
    */
    void Disconnect();

    void Write(BufferPtr data);

private:
    bool m_subscribed;                       /**< subscribed flag */
    unsigned m_clientId;                     /**< Clients identifier  */
    unsigned long m_creationTime;            /**< Creation time as timestamp. */
    std::set<NetConnection*> m_connections;  /**< connections associated with client */
};

#endif /* STREAMCLIENT_H_ */
