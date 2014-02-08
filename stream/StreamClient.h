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
 *  Created on: Jan 13, 2014
 *      Author: emo
 */

#ifndef STREAMCLIENT_H_
#define STREAMCLIENT_H_

#include "system/SystemThread.h"
#include "utils/PropertyMap.h"
#include "server/NetConnection.h"
#include <set>
#include <boost/shared_ptr.hpp>

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
    * @param conn - pointer to NetConnection object.
    */
    void Register(NetConnection* conn);

    /**
    * Remove connection from client associated with this client.
    * @param conn - pointer to NetConnection object.
    */
    void UnRegister(NetConnection* conn);

    /**
    * Returns the time at which the client was created.
    */
    unsigned long GetCreationTime();

    /**
    *  Return set of connections for this client.
    */
    std::set<NetConnection*>& GetConnections();

    /**
    *  Get the properties if the client.
    */
    PropertyMap& GetProperties();

    /**
    * Returns the client id.
    */
    unsigned GetID();

private:
    unsigned m_clientID;                    /**< Client identifier  */
    unsigned long m_creationTime;           /**< Creation time as time-stamp. */
    PropertyMap  m_propetries;              /**< Client properties. */
    SystemMutex  m_lockConnections;
    std::set<NetConnection*> m_connections; /**< connections associated with client */
};

typedef boost::shared_ptr<StreamClient> StreamClientPtr;

#endif /* STREAMCLIENT_H_ */
