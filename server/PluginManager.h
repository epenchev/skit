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
    * @return PluginModule* - plug-in module object or NULL on error.
    */
    static PluginModule* LoadModule(const char* inPath);

    /**
    * Unload plug-in library module from server memory.
    * @param module - module to unload.
    */
    static void UnloadModule(PluginModule* module);

    /**
    * Get a module by module identifier
    * @param id - id of the module.
    */
    static PluginModule* GetModule(unsigned id);
};

#endif /* PLUGIN_MANAGER_H_ */
