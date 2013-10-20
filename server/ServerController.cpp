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
#include "stream/Stream.h"
#include "server/HTTPServer.h"

static std::vector<Stream*> mstreams;                   /* Streams container */
static std::map<unsigned, unsigned> mMapModuleSource;   /* ID of module, ID of StreamSource */
static std::map<unsigned, unsigned> mMapModuleFilter;   // ID of module, ID of StreamFilter
static std::map<unsigned, unsigned> mMapModuleSink;     // ID of module, ID of StreamSink
static std::vector<unsigned> mvModules;                 // ID of all modules

//static HTTPMservermhttpServer

unsigned ServerController::RegisterStream(const std::string name)
{
    unsigned id = 0;
    if (!name.empty())
    {
        id = (mstreams.size() + 1);
        Stream* s = new Stream(id);
        mstreams.push_back(s);
    }

    return id;
}

void ServerController::DeregisterStream(unsigned id)
{
    std::vector<int>::size_type streamCount = mstreams.size();
    for (unsigned i = 0; i < streamCount; i++)
    {
        Stream* s = mstreams[i];
        if (s->GetStreamId() == id)
        {
            mstreams.erase(mstreams.begin() + i);
            delete s;
        }
    }
}

Stream* ServerController::GetStream(unsigned id)
{
    Stream* outSteam = NULL;
    std::vector<int>::size_type streamCount = mstreams.size();
    for (unsigned i = 0; i < streamCount; i++)
    {
        if (mstreams[i]->GetStreamId() == id)
        {
            outSteam = mstreams[i];
            break;
        }
    }

    return outSteam;
}

unsigned ServerController::GetStreamCount()
{
    return mstreams.size();
}

void ServerController::LoadPlugin(const std::string& filePath)
{
    StreamSource* sourcePlugin = NULL;
    StreamFilter* filterPlugin = NULL;
    StreamSink* sinkPlugin = NULL;

    if (!filePath.empty())
    {
        int moduleId = PluginManager::LoadModule(filePath.c_str());
        if (moduleId != -1)
        {
            mvModules.push_back(moduleId); // collect all plug-in ID's in one place
            PluginModule* module = PluginManager::CreateInstance(moduleId);
            if (module)
            {
                module->OnModuleLoad();

                if ((sourcePlugin = dynamic_cast<StreamSource*>(module)) != NULL)
                {
                    std::cout << "Found a source \n";
                    sourcePlugin->Start();
                    std::cout << module->GetModuleDescription() << std::endl;
                    std::cout << "---------\n";
                }
                else if ((filterPlugin = dynamic_cast<StreamFilter*>(module)) != NULL)
                {
                    std::cout << "Found a filter \n";
                    std::cout << module->GetModuleDescription() << std::endl;
                    filterPlugin->Filter();
                    std::cout << "---------\n";
                }
                else if ((sinkPlugin = dynamic_cast<StreamSink*>(module)) != NULL)
                {
                    std::cout << "Found a sink \n";
                    std::cout << module->GetModuleDescription() << std::endl;
                    std::cout << "---------\n";
                }
            }
            else
            {
                // TODO Log
                std::cout << "error invalid plugin object " << filePath << std::endl;
            }
        }
        else
        {
            // TODO Log
            std::cout << "error loading " << filePath << std::endl;
        }
    }
}

/*
static void ServerController::SetHTTPServer(HTTPServer* server)
{

}

static HTTPServer* ServerController::GetHTTPServer()
{
	return NULL;
}
*/
