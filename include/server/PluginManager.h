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

#include "PluginModule.h"
#include "system/SystemLib.h"
#include <map>
#include <list>

/**
* Create module (plug-in) objects extending the server logic to some user needs.
* Manage plug-in modules in the server core.
*/
class PluginManager
{
public:

    /**
    * Load library file containing plug-in module code.
    * @param inPath - path to the file.
    * @return int - plug-in id of the library or -1 if loading failed.
    */
    static int LoadModule(const char* inPath);

    /**
    * Unload plug-in library module from server memory.
    * @param pluginID - id of the plug-in library to unload.
    */
    static void UnloadModule(int pluginID);

    /**
    * Get plug-in object instance associated with this id.
    * @param pluginID - id of the plug-in.
    * @return PluginModule* - valid pointer to object or NULL in case of error.
    */
    static PluginModule* CreateInstance(int pluginID);

private:

    /**
    * Destroy all created previously objects from this module.
    * @param pluginID - id of the plug-in module.
    */
    static void DestroyInstances(int pluginID);

    /**
    * Helper function for accessing the native library associated with this id.
    * @param pluginID - id of the plug-in module.
    */
    static SystemLib* GetSystemLib(int pluginID);
};

#endif /* PLUGIN_MANAGER_H_ */
