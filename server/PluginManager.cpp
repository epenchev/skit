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

PluginManager::PluginManager()
{ mErrCode.Clear(); }

PluginManager::~PluginManager()
{
    int itemId;
    int lastItem = GetLast();

    for (itemId = GetFirst(); itemId <= lastItem; itemId++)
    {
        PluginManagerItem* item = mPluginItems.at(itemId);
        delete item;
    }
}

int PluginManager::GetFirst()
{
    if (mPluginItems.empty()) return -1;
    return 0;
}

int PluginManager::GetLast()
{
    if (mPluginItems.empty()) return -1;
    return mPluginItems.size() - 1;
}

int PluginManager::LoadModule(const char* inPath)
{
    mErrCode.Clear();
    if (!inPath)
    {
        mErrCode.SetMessage("Empty file path");
        return -1;
    }

    SystemLib* lib; /* = new SystemLib(inPath); */
    if (lib)
    {
        try
        {
            lib->LoadFile(inPath);
        }
        catch(SystemException& ex)
        {
            mErrCode = ex.Code();
            delete lib;
            return -1;
        }

        PluginManagerItem* item = new PluginManagerItem(lib, GetLast() + 1);
        if (item)
        {
            mPluginItems.push_back(item);
            return item->ID();
        }
    }
    delete lib;
    return -1;
}

PluginModule* PluginManager::GetPluginModule(int pluginID)
{
    PluginModule* outPluginMod = NULL;

    mErrCode.Clear();
    if (mPluginItems.empty())
    {
        mErrCode.SetMessage("No modules loaded");
        return outPluginMod;
    }

    if ( (pluginID >= 0) && (pluginID < (int)mPluginItems.size()) )
    {
        PluginManagerItem* item = mPluginItems.at(pluginID);
        if (item)
        {
            outPluginMod = item->Module();
            if (!outPluginMod)
            {
                mErrCode.SetMessage("Invalid module format"); // module dosn't have create function.
                outPluginMod = NULL;
            }
        }
        else
        {
            mErrCode.SetMessage("Not a valid id");
        }
    }
    return outPluginMod;
}

void PluginManager::UnloadModule(int pluginID)
{
    if (!mPluginItems.empty())
    {
        PluginManagerItem* item = NULL;
        if ( (pluginID >= 0) && (pluginID < (int)mPluginItems.size()) )
        {
            item = mPluginItems.at(pluginID);
            if (item)
            {
                mPluginItems.erase(mPluginItems.begin() + pluginID);
                delete item;
            }
        }
    }
}

PluginManagerItem::PluginManagerItem(SystemLib* inLib, int inPluginID)
 : mPluginID(inPluginID), mSharedLib(inLib)
{
    mPlugin = NULL;
    ErrorCode error;
    PluginModule* (*createFunc)();

    if (mSharedLib)
    {
        createFunc = (PluginModule* (*)()) mSharedLib->GetSymbol("CreateModuleObject", error);
        if (createFunc)
        {
            mPlugin = (PluginModule*)createFunc();
        }
    }
}

PluginManagerItem::~PluginManagerItem()
{
    ErrorCode error;
    void (*destroyFunc)(PluginModule*);

    if (mSharedLib && mPlugin)
    {
        destroyFunc = (void (*)(PluginModule*)) mSharedLib->GetSymbol("DestroyModuleObject", error);
        if (destroyFunc)
        {
            destroyFunc(mPlugin);
            delete mSharedLib; // unload library file from server memory.
        }
    }
}

