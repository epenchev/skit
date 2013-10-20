/*
 * PluginModule.h
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

#ifndef PLUGINMODULE_H_
#define PLUGINMODULE_H_

/**
* Interface for creating plug-in modules extending server logic.
* A custom module must implement this interface in order to comply with server plug-in system.
*/
class PluginModule
{
public:

    /**
    * Method invoked when module is loaded into the server core system.
    * Some initialization of module can be done here.
    * The server core object is passed here so access to server subsystems is given to the module.
    * @param ServerInstance - Server core object.
    */
    virtual void OnModuleLoad() = 0;

    /**
    * Method invoked when module is to be unloaded from the server core.
    * Module can free resources here and detach itself from server subsystems.
    * After this call returns PluginModule object will be destroyed and module will be unloaded from server memory.
    */
    virtual void OnModuleUnLoad() = 0;

    /**
    * Module should have name so users can identify them easily as they appear in the server.
    * @return string - module name.
    */
    virtual const char* GetModuleName() = 0;

    /**
    * Module should have some short description so users can better utilize modules as they appear in the server.
    * @return string - module short description.
    */
    virtual const char* GetModuleDescription() = 0;

    /**
    * Module should have author name.
    * @return string - module author.
    */
    virtual const char* GetModuleAuthor() = 0;
};

#endif /* PLUGINMODULE_H_ */
