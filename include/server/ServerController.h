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

#include "server/IHTTPServer.h"

/**
* Main class for controlling the system and access all server subsystems.
*/
class ServerController
{
public:

    /**
    * Load a plugin module into the server.
    * @param filePath - string with full path to plugin file.
    */
    static void LoadPlugin(const char* filePath);

    // TODO Remove
    static void SetHTTPServer(IHTTPServer* server);
    static IHTTPServer* GetHTTPServer();
};

#endif /* SERVERCONTROLLER_H_ */
