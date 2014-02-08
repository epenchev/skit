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

#include "server/HTTPServer.h"
#include "utils/PropertyMap.h"

/**
* Main class for controlling the system and access all server subsystems.
*/
class ServerController
{
public:

	static void StartServer();

	static void StopServer();

	static HTTPServer* GetHTTPServer();

	static PropertyMap& GetServerProperties();
};

#endif /* SERVERCONTROLLER_H_ */
