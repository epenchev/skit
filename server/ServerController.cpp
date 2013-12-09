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

#include "utils/IDGenerator.h"
#include <vector>
#include <map>
#include <set>
//#include "server/NetConnection.h"
#include "server/ServerController.h"
#include "server/PluginManager.h"
#include "stream/Stream.h"
#include "stream/StreamSource.h"
#include "system/SystemThread.h"
#include "Logger.h"

// std::map<unsigned, NetConnection*>  m_clientToConnection; /**< client id to NetConnection */

// static SystemMutex m_connectionMutex;                     /**< Lock connections set */
// static std::set<NetConnection*> m_connections;            /**< Global set with all connections */

static SystemMutex m_streamsMutex;                                /**<  Lock to streams map for thread safety */
static std::map<std::string, unsigned> m_streamNamesToIdMap;      /**<  Map stream name to id */
static std::map<unsigned, Stream*> m_streamsMap;                  /**<  Streams container */

static std::map<unsigned, unsigned> mMapModuleSource;   /**<  ID of module, ID of StreamSource */
static std::map<unsigned, unsigned> mMapModuleFilter;   // ID of module, ID of StreamFilter
static std::map<unsigned, unsigned> mMapModuleSink;     // ID of module, ID of StreamSink
static std::vector<unsigned> mvModules;                 // ID of all modules
static IHTTPServer* mhttpServer = NULL;                  // HTTP server instance

static StreamSource* fsource = NULL; // TODO for test only

unsigned ServerController::CreateStream(const std::string name)
{
    unsigned id = 0;
    if (!name.empty())
    {
        m_streamsMutex.Lock();
        id = IDGenerator::Instance().Next();
        Stream* s = new Stream(id);
        s->SetName(name.c_str());
        m_streamsMap.insert(std::pair<unsigned,Stream*>(id, s));
        m_streamsMutex.Unlock();
    }
    return id;
}

void ServerController::DeteleStream(unsigned id)
{
     m_streamsMutex.Lock();
     if (m_streamsMap.count(id) > 0)
     {
         std::map<unsigned, Stream*>::iterator it = m_streamsMap.find(id);
         Stream* s = it->second;
         m_streamsMap.erase(it);
         m_streamNamesToIdMap.erase(s->GetName());
         s->Stop();

         // TODO notify listeners.
         delete s;
     }
     m_streamsMutex.Unlock();
}

Stream* ServerController::GetStream(const char* name)
{
    Stream* outSteam = NULL;
    if (name)
    {
        std::string streamName(name);
        m_streamsMutex.Lock();
        if (m_streamNamesToIdMap.count(streamName) > 0)
        {
            std::map<std::string, unsigned>::iterator it = m_streamNamesToIdMap.find(streamName);
            outSteam = ServerController::GetStream(it->second);
        }
        m_streamsMutex.Unlock();
    }
    return outSteam;
}

Stream* ServerController::GetStream(unsigned id)
{
    Stream* outSteam = NULL;
    m_streamsMutex.Lock();
    if (m_streamsMap.count(id) > 0)
    {
        std::map<unsigned, Stream*>::iterator it = m_streamsMap.find(id);
        outSteam = it->second;
    }
    m_streamsMutex.Unlock();
    return outSteam;
}

unsigned ServerController::GetStreamCount()
{
    return m_streamsMap.size();
}

void ServerController::LoadPlugin(const std::string& filePath)
{
    StreamSource* sourcePlugin = NULL;
    StreamFilter* filterPlugin = NULL;
    StreamSink* sinkPlugin = NULL;
    if (!filePath.empty())
    {
        int moduleId = PluginManager::LoadModule(filePath.c_str());
        if (moduleId != -1) {
            // collect all plug-in ID's in one place
            mvModules.push_back(moduleId);
            PluginModule* module = PluginManager::CreateInstance(moduleId);
            if (module)
            {
                module->OnModuleLoad();
                if ((sourcePlugin = dynamic_cast<StreamSource*>(module)) != NULL)
                {
                    std::cout << "Found a source \n";
                    //sourcePlugin->Start();

                    fsource = sourcePlugin; // TODO for test

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

void ServerController::SetHTTPServer(IHTTPServer* server)
{
    if (server)
    {
        mhttpServer = server;
    }
}

IHTTPServer* ServerController::GetHTTPServer()
{
    return mhttpServer;
}

/*
void ServerController::OnHTTPSessionCreate(IHTTPSession* session)
{
    if (session)
    {
        NetConnection* conn = dynamic_cast<NetConnection*>(session);
        if (conn)
        {
            m_connectionMutex.Lock();
            m_connections.insert(conn);
            m_connectionMutex.Unlock();
        }
    }
}

void ServerController::OnHTTPSessionDestroy(IHTTPSession* session)
{
    if (session)
    {
        m_connectionMutex.Lock();
        NetConnection* conn = dynamic_cast<NetConnection*>(session);
        if (conn)
        {
            m_connectionMutex.Lock();

            // remove mapping clientId to connection
            for (std::map<unsigned, NetConnection*>::iterator it = m_clientToConnection.begin();
                                                                 it != m_clientToConnection.end(); ++it)
            {
                 if (it->second == conn)
                 {
                     unsigned clientId = it->first;
                     m_clientToConnection.erase(clientId); // TODO value_comp()
                 }
            }
            m_connections.erase(conn);
            m_connectionMutex.Unlock();
        }
    }
}

void ServerController::RegisterClient(unsigned clientId, NetConnection* conn)
{
    if (clientId)
    {
        if (conn)
        {
            m_connectionMutex.Lock();
            if (m_connections.count(conn) > 0)
            {
                LOG(logDEBUG) << "Register client " << clientId << " to connection" << conn->GetConnId();
                m_clientToConnection.insert(std::pair<unsigned,NetConnection*>(clientId, conn));
            }
            m_connectionMutex.Unlock();
        }
        else
        {
            LOG(logERROR) << "Invalid connection";
        }
    }
    else
    {
        LOG(logERROR) << "Invalid client Id";
    }
}

NetConnection* ServerController::GetClientConnection(unsigned clientId)
{
    NetConnection* outConn = NULL;
    if (clientId)
    {
        m_connectionMutex.Lock();
        if (m_clientToConnection.count(clientId) > 0)
        {
            outConn = m_clientToConnection.at(clientId);
        }
        else
        {
            LOG(logERROR) << "No connection for this client:" << clientId;
        }
        m_connectionMutex.Unlock();
    }
    else
    {
        LOG(logERROR) << "Invalid client Id";
    }

    return outConn;
}
*/

StreamSource* ServerController::GetFileSource()
{
    return fsource;
}

