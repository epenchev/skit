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

#include <vector>
#include <map>
#include <string>

class Stream;
class HTTPServer;

/**
* Main server class for controlling the system.
*/
class ServerController
{
public:
    static unsigned RegisterStream(const std::string name);

    static void DeregisterStream(unsigned id);

    /* Get a stream by id */
    static Stream* GetStream(unsigned id);

    /* Get count of all registered streams */
    static unsigned GetStreamCount();

    static void LoadPlugin(const std::string& filePath);
/*
    static void SetHTTPServer(HTTPServer* server);

    static HTTPServer* GetHTTPServer();
*/
};

#endif /* SERVERCONTROLLER_H_ */
