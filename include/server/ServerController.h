/*
 * ServerController.h
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
 *  Created on: Oct 14, 2013
 *      Author: emo
 */

#ifndef SERVERCONTROLLER_H_
#define SERVERCONTROLLER_H_

#include <string>
#include "IHTTPServer.h"
#include "stream/Stream.h"

class StreamSource; // TODO test remove after

/*
class ClientObserver
{
    virtual void OnClientCreate() = 0;
    virtual void OnClientDestroy() = 0;
};
*/

/**
* Main server class for controlling the system.
*/
class ServerController /*: public HTTPServerObserver*/
{
public:
    /* Create a stream and add it to the controller */
    static unsigned CreateStream(const std::string name);

    /* destroy a stream and removed it from a controller */
    static void DeteleStream(unsigned id);

    /* Get a stream by id */
    static Stream* GetStream(unsigned id);

    /* Get a stream by name */
    static Stream* GetStream(const char* name);

    /* Get count of all registered streams */
    static unsigned GetStreamCount();

    static void LoadPlugin(const std::string& filePath);

    static void SetHTTPServer(IHTTPServer* server);

    static IHTTPServer* GetHTTPServer();

    static StreamSource* GetFileSource(); // TODO test remove after

private:

    /* from IHTTPServer */
    /* void OnHTTPSessionCreate(IHTTPSession* session); */

    /* from IHTTPServer */
    /* void OnHTTPSessionDestroy(IHTTPSession* session); */
};

#endif /* SERVERCONTROLLER_H_ */
