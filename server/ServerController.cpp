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

#include "server/ServerController.h"
#include "server/PluginManager.h"
#include "server/HTTPServer.h"
#include "system/TaskThread.h"
#include "utils/Logger.h"
#include "utils/PropertyMap.h"
#include "stream/StreamFactory.h"
#include <string>

static HTTPServer* m_httpServer = NULL;  /**< HTTP server instance */
static PropertyMap  m_properties;         /**< server properties read from configuration file */

static void LoadPlugin(const char* filePath)
{
    LOG(logDEBUG) << "Loading " << filePath;
    if (filePath)
    {
        PluginModule* module = PluginManager::LoadModule(filePath);
        if (!module)
        {
            LOG(logERROR) << "Error loading module " << filePath;
        }
    }
}

void ServerController::StartServer()
{
	Log::ReportingLevel() = logINFO;
	LOG(logINFO) << "Starting server";
    if (!m_httpServer)
    {
        m_httpServer = new HTTPServer(8080);
        m_httpServer->Start();
        LOG(logINFO) << "Loading plugins";
        LoadPlugin("/home/emo/workspace/blitz/modules/HTTPPseudoStreaming/HTTPPseudoStreamModule.so");
        LoadPlugin("/home/emo/workspace/blitz/modules/FileSource/FileSourceModule.so");

        Stream* s = StreamFactory::CreateStream("FileSource", NULL, "HTTPPseudoStreamSink");
        if (s)
        {
            PropertyMap& pmap = s->GetProperties();
            pmap.SetProperty("filename", "/var/www/oceans-clip.mp4");
            LOG(logDEBUG) << "Stream is created";
            StreamFactory::PublishStream(*s, "ocean");
            s->Play();
        }

        LOG(logDEBUG) << "Starting 1 threads";
        for (int i = 0; i < 1; i++)
        {
            TaskThreadPool::AddThread();
        }
        TaskThreadPool::StartThreads();
    }
    else
    {
        LOG(logWARNING) << "Server already started";
    }
}

void ServerController::StopServer()
{
    LOG(logINFO) << "Stopping server";
    if (m_httpServer)
    {
        m_httpServer->Stop();
    }
    else
    {
        LOG(logWARNING) << "Server is not started";
    }
}

HTTPServer* ServerController::GetHTTPServer()
{
    return m_httpServer;
}

PropertyMap& ServerController::GetServerProperties()
{
    return m_properties;
}
