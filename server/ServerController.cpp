/*
 * ServerController.cpp
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

#include <map>
#include "server/ServerController.h"
#include "server/PluginManager.h"
#include "Logger.h"

static IHTTPServer*                        m_httpServer = NULL;             /**< HTTP server instance */
static std::map<unsigned, PluginModule*>   m_plugins;                       /**< plug-ins register */



void ServerController::LoadPlugin(const char* filePath)
{
    LOG(logDEBUG) << "Loading " << filePath;
    if (filePath)
    {
    	PluginModule* module = PluginManager::LoadModule(filePath);

        /*
    	else
        {
            LOG(logERROR) << "error loading " << filePath << std::endl;
        }*/
    }
}

void ServerController::SetHTTPServer(IHTTPServer* server)
{
    if (server)
    {
    	m_httpServer = server;
    }
}

IHTTPServer* ServerController::GetHTTPServer()
{
    return m_httpServer;
}

