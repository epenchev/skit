/*
 * PluginManager.cpp
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
 *  Created on: Aug 16, 2013
 *      Author: emo
 */

#include "server/PluginManager.h"
#include "ErrorCode.h"

/**< map association with plug-in id and native library instance */
static std::map<unsigned, SystemLib*> msysLibMap;

/**<  map association with plug-in id objects created from this library module */
static std::map<unsigned, std::list<PluginModule*> > mpluginObjectsMap;

int PluginManager::LoadModule(const char* inPath)
{
    int outId = -1;
    if (!inPath)
    {
        // TODO LOG
        std::cout << "Empty file path \n";
        return outId;
    }

    SystemLib* lib = new SystemLib();
    try
    {
        lib->LoadFile(inPath);
    }
    catch(SystemException& ex)
    {
        // TODO LOG
        std::cout << ex.Code().GetErrorMessage();
        delete lib;
        return outId;
    }

    if (msysLibMap.empty())
    {
        outId = 1;
        msysLibMap.insert(std::make_pair(outId, lib));
    }
    else
    {
        std::map<unsigned, SystemLib*>::iterator it = msysLibMap.end();
        unsigned id = it->first + 1;

        msysLibMap.insert(std::make_pair(id, lib));
        outId = id;
    }

    return outId;
}

void PluginManager::UnloadModule(int pluginID)
{
    SystemLib* lib = PluginManager::GetSystemLib(pluginID);
    if (lib)
    {
        PluginManager::DestroyInstances(pluginID);
        msysLibMap.erase(pluginID);
        delete lib;
    }
    else
    {
        // TODO log
        std::cout << "Error geting lib";
    }
}

PluginModule* PluginManager::CreateInstance(int pluginID)
{
    ErrorCode error;
    PluginModule* outPluginMod = NULL;

    SystemLib* lib = PluginManager::GetSystemLib(pluginID);
    if (lib)
    {
        PluginModule* (*createFunc)();
        createFunc = (PluginModule* (*)()) lib->GetSymbol("CreateModuleObject", error);
        if (createFunc)
        {
            // Just in case don't crash the server
            try
            {
                outPluginMod = (PluginModule*)createFunc();
                if (!outPluginMod)
                {
                    // TODO log
                    // std::cout << "Invalid module object  ";
                    return outPluginMod;
                }

                if (mpluginObjectsMap.count(pluginID) > 0)
                {
                    std::map<unsigned, std::list<PluginModule*> >::iterator it = mpluginObjectsMap.find(pluginID);
                    std::list<PluginModule*> pluginObjects = it->second;

                    // Add new created plug-in object
                    pluginObjects.push_back(outPluginMod);
                }
                else // no objects instances have been created with this module
                {
                    std::list<PluginModule*> pluginObjects;
                    pluginObjects.push_back(outPluginMod);
                    mpluginObjectsMap.insert(std::make_pair(pluginID, pluginObjects));
                }
            }
            catch(std::exception& ex)
            {
                // TODO log
                std::cout << "ex.what() ";
                outPluginMod = NULL;
            }
        }
        else
        {
            // TODO log
            std::cout << "Unable to lookup symbol CreateModuleObject ";
            std::cout << error.GetErrorMessage() ;
        }
    }
    else
    {
        // TODO log
        std::cout << "Error geting lib";
    }

    return outPluginMod;
}

void PluginManager::DestroyInstances(int pluginID)
{
    ErrorCode error;
    void (*destroyFunc)(PluginModule*);

    SystemLib* lib = PluginManager::GetSystemLib(pluginID);
    if (lib) // pluginID is validated here
    {
        // Get Objects loaded from this library
        if (mpluginObjectsMap.count(pluginID) > 0)
        {
            std::map<unsigned, std::list<PluginModule*> >::iterator it = mpluginObjectsMap.find(pluginID);

            std::list<PluginModule*> pluginObjects = it->second;
            destroyFunc = (void (*)(PluginModule*)) lib->GetSymbol("DestroyModuleObject", error);
            if (destroyFunc)
            {
                for ( std::list<PluginModule*>::iterator itObj = pluginObjects.begin();
                                                                   itObj != pluginObjects.end(); it++)
                {
                    PluginModule* pluginObj = *itObj;
                    destroyFunc(pluginObj);
                }
                // Just to be nice
                pluginObjects.clear();
            }
            else
            {
                // TODO log
                std::cout << "Unable to lookup symbol DestroyModuleObject ";
                std::cout << error.GetErrorMessage() ;
            }
        }
    }
    else
    {
        // TODO log
        std::cout << "Error geting lib";
    }
}

SystemLib* PluginManager::GetSystemLib(int pluginID)
{
    SystemLib* lib = NULL;
    if (!pluginID)
    {
        // TODO
        std::cout << "Error invalid ID ";
        return lib;
    }

    if (!msysLibMap.empty())
    {
        unsigned searchId = (unsigned)pluginID;
        if (msysLibMap.count(searchId) > 0)
        {
            std::map<unsigned, SystemLib*>::iterator it = msysLibMap.find(searchId);
            lib = it->second;
        }
        else
        {
            // TODO log
            std::cout << "No such id present in list";
        }
    }
    else
    {
        // TODO log
        std::cout << "No libs loaded";
    }

    return lib;
}



