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

#include "system/SystemLib.h"
#include "server/PluginManager.h"
#include "utils/IDGenerator.h"
#include "utils/ErrorCode.h"
#include "utils/Logger.h"
#include <map>

/**< map association with plug-in module object and native library instance */
static std::map<PluginModule*, SystemLib*> m_libs;

PluginModule* PluginManager::LoadModule(const char* inPath)
{
	PluginModule* outmod = NULL;
	if (!inPath)
    {
    	LOG(logERROR) << "Empty file path";
        return outmod;
    }

    SystemLib* lib = new SystemLib();
    try
    {
    	ErrorCode err;
        lib->LoadFile(inPath);
        PluginModule* (*createFunc)(unsigned);
        createFunc = (PluginModule* (*)(unsigned)) lib->GetSymbol("CreateModuleObject", err);
        if (createFunc)
        {
        	outmod = (PluginModule*)createFunc(IDGenerator::Instance().Next());
        	if (outmod)
        	{
        		m_libs.insert(std::pair<PluginModule*, SystemLib*>(outmod, lib));
        		outmod->OnModuleLoad();
        	}
        }
        else if (err)
        {
        	LOG(logERROR) << err.GetErrorMessage();
        	delete lib;
        }
    }
    catch(SystemException& ex)
    {
    	LOG(logERROR) << ex.Code().GetErrorMessage();
        delete lib;
    }
    return outmod;
}

void PluginManager::UnloadModule(PluginModule* module)
{
	ErrorCode err;
	void (*destroyFunc)(PluginModule*);
	if (module)
	{
		std::map<PluginModule*, SystemLib*>::iterator it = m_libs.find(module);
		if (it != m_libs.end())
		{
			SystemLib* lib = it->second;
			PluginModule* module = it->first;
			m_libs.erase(it);
			module->OnModuleUnLoad();
			destroyFunc = (void (*)(PluginModule*)) lib->GetSymbol("DestroyModuleObject", err);
			if (destroyFunc)
			{
				destroyFunc(module);
			}
			else if (err)
			{
				LOG(logERROR) << err.GetErrorMessage();
			}
			delete lib;
			m_libs.erase(it);
		}
		else
		{
			LOG(logWARNING) << " No such module in list" << module->GetModuleName();
		}
	}
}

PluginModule* PluginManager::GetModule(unsigned id)
{
	PluginModule* outMod = NULL;
	if (id)
	{
		std::map<PluginModule*, SystemLib*>::iterator it = m_libs.begin();
		for (;it != m_libs.end(); ++it)
		{
			PluginModule* module = it->first;
			if (module->GetModuleID() == id)
			{
				outMod = module;
				break;
			}
		}
	}
	return outMod;
}
