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

#include "utils/PropertyMap.h"
#include <boost/shared_ptr.hpp>

/**
* Client is an abstraction representing user connected to Blitz media server.
* Clients are tied to connections and subscribed to Streams.
*/
class StreamClient
{
public:
    virtual ~StreamClient();

    /**
    * Returns the time at which the client was created.
    */
    unsigned long GetCreationTime();

    /**
    *  Get the properties if the client.
    */
    PropertyMap& GetProperties();

    /*  Returns the client id. */
    unsigned GetID();

    /* Total number of bytes read from the client connection. */
    virtual unsigned long GetReadBytes() = 0;

    /**
    * Total number of bytes written to the client connection.
    */
    virtual unsigned long GetWrittenBytes() = 0;

    /**
    * Sets the bandwidth using a mbit/s value.
    * @param mbits - Mega bits per second limit.
    */
    virtual void SetBandwidth(int mbits) = 0;

protected:
    StreamClient();

    unsigned m_clientID;                    /**< Client identifier  */
    unsigned long m_creationTime;           /**< Creation time as time-stamp. */
    PropertyMap  m_propetries;              /**< Client properties. */
};

typedef boost::shared_ptr<StreamClient> StreamClientPtr;

#endif /* STREAMCLIENT_H_ */
