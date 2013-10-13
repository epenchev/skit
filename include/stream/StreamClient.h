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

class NetConnection;

class StreamClient
{
public:
    StreamClient() {}
    virtual ~StreamClient() {}

    void Subscribe(unsigned streamId) {}
    void UnSubscribe() {}
    bool IsSubscribed() { return true; }
    NetConnection* GetNetConnection() { return NULL; }
    void GetProperties() {}
    void SetProperties() {}
};

#endif /* STREAMCLIENT_H_ */
