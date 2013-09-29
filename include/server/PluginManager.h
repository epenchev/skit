/*
 * PluginManager.h
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

#ifndef PLUGIN_MANAGER_H_
#define PLUGIN_MANAGER_H_

#include "ErrorCode.h"
#include "system/SystemLib.h"
#include <vector>

class PluginModule;

/**
* PluginManager entity, stores internal object to manage a single plug-in module.
*/
class PluginManagerItem
{
public:
    /**
    * Constructor for the PluginManagerItem class.
    * @param inLib - pointer SystemLib object from which the plug-in is loaded.
    * @param inPluginID - ID of the plug-in generated from PluginManager.
    */
    PluginManagerItem(SystemLib* inLib, int inPluginID);

    ~PluginManagerItem();

    /**
    * Get the id of the plug-in object.
    * @return int - plug-in id.
    */
    int ID() const { return mPluginID; }

    /**
    * Get the SystemLib which this plug-in is loaded from.
    * @return SystemLib* - pointer to object SystemLib.
    */
    SystemLib* Lib() const { return mSharedLib; }

    /**
    * Get the actual plug-in object.
    * @return PluginModule* - pointer to object PluginModule.
    */
    PluginModule* Module() const { return mPlugin; }

private:
    int mPluginID;          /**< plug-in ID */
    SystemLib* mSharedLib;  /**< object pointer to shared library file */
    PluginModule* mPlugin;  /**< object pointer to plug-in module */
};

/**
* Create module (plug-in) objects extending the server logic to some user needs.
* Manage plug-in modules in the server core.
*/
class PluginManager
{
public:
    PluginManager();
    ~PluginManager();

    /**
    * Get the id of the fist loaded plug-in.
    * Note that id's are sequential.
    * @return int - plug-in id or -1 if non loaded.
    */
    int GetFirst();

    /**
    * Get the id of the last loaded plug-in.
    * Note that id's are sequential.
    * @return int - plug-in id or -1 if non loaded.
    */
    int GetLast();

    /**
    * Load library file containing plug-in module code.
    * @param inPath - path to the file.
    * @return int - plug-in id or -1 if loading failed.
    * Check GetLastError() for error description.
    */
    int LoadModule(const char* inPath);

    /**
    * Unload plug-in module from server memory.
    * @param pluginID - id of the plug-in to unload.
    * Check GetLastError() for error description.
    */
    void UnloadModule(int pluginID);

    /**
    * Get the plug-in object associated with this id.
    * @param pluginID - id of the plug-in.
    * @return PluginModule* - valid pointer to the object or NULL in case of error.
    * Check GetLastError() for error description.
    */
    PluginModule* GetPluginModule(int pluginID);

    ErrorCode& GetLastError() { return mErrCode; }
private:
    ErrorCode  mErrCode;                            /**< error code of last operation */
    std::vector<PluginManagerItem*> mPluginItems;   /**< container for the plug-in objects */
};

#endif /* PLUGIN_MANAGER_H_ */
