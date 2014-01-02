/*
 * StreamFactory.cpp
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
 *  Created on: Dec 18, 2013
 *      Author: emo
 */

#include <map>
#include <list>
#include <string>
#include <exception>
#include "utils/Logger.h"
#include "stream/StreamSource.h"
#include "stream/StreamFilter.h"
#include "stream/StreamSink.h"
#include "utils/IDGenerator.h"
#include "stream/StreamFactory.h"
#include "system/Task.h"
#include "system/TaskThread.h"
#include "system/SystemThread.h"

static SystemMutex m_lockStreams;                       /**< streams lock */
static SystemMutex m_lockItems;                         /**< streams items lock (m_sources, m_filters, m_sinks) */
static SystemMutex m_lockListeners;                     /**< listeners lock */

static std::map<unsigned, Stream*> m_streams;          /**< map created streams stream ID -> Stream */
static std::map<unsigned, std::string> m_pubStreams;   /**< map published streams      ID -> stream name */

static std::map<StreamItem, std::map<std::string, PluginModule*> > m_streamItems;
                                                         /**< map stream Items, Item -> map(classID, module) */

static std::list<StreamFactoryListener*> m_listeners;   /**< listeners/observers for events from factory  */

static const char* itemNames[] = { "StreamSource", "StreamFilter", "StreamSink" };

static void NotifyOnStreamCreate(Stream& s)
{
	m_lockListeners.Lock();
	LOG(logDEBUG) << "Notify listeners";
	for (std::list<StreamFactoryListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
	{
		Task* eventTask = new Task();
	    eventTask->Connect(&StreamFactoryListener::OnStreamCreate, *it, boost::ref(s));
	    TaskThreadPool::Signal(eventTask);
	}
	m_lockListeners.Unlock();
}

static PluginObject* CreateStreamItem(const char* classID)
{
	PluginObject* item = NULL;
	LOG(logDEBUG) << "Create stream item";
	if (classID)
	{
		LOG(logDEBUG) << "ClassID:" << classID;
		SystemMutexLocker lock(m_lockItems);
		std::map<StreamItem, std::map<std::string, PluginModule*> >::iterator it = m_streamItems.begin();
		for (; it != m_streamItems.end(); ++it)
		{
			std::map<std::string, PluginModule*> items = it->second;
			std::map<std::string, PluginModule*>::iterator itItem = items.find(classID);
			if (itItem != items.end())
			{
				item = (itItem->second)->CreateInstance();
				LOG(logDEBUG) << itemNames[it->first]  << " with ClassID:" << classID << " created !";
			}
		}
		if (!item)
		{
			LOG(logERROR) << "No plug-in registered with ClassID " << classID << " or invalid type";
		}
	}

	return item;
}

static void DestroyStreamItem(PluginObject* plugin)
{
	LOG(logDEBUG) << "Destroy stream item";
	if (plugin)
	{
		LOG(logDEBUG) << "ClassID:" << plugin->GetClassID();
		SystemMutexLocker lock(m_lockItems);
		std::map<StreamItem, std::map<std::string, PluginModule*> >::iterator it = m_streamItems.begin();
		for (; it != m_streamItems.end(); ++it)
		{
			std::map<std::string, PluginModule*> items = it->second;
			std::map<std::string, PluginModule*>::iterator itItem = items.find(plugin->GetClassID());
			if (itItem != items.end())
			{
				LOG(logDEBUG) << "Delete " << itemNames[it->first]  << "with ClassID:" << plugin->GetClassID();
				(itItem->second)->DestroyInstance(plugin);
			}
		}
	}
}

Stream* StreamFactory::CreateStream(const char* sourceClassID, const char* filterClassID, const char* sinkClassID)
{
    Stream* outStream = NULL;
    LOG(logDEBUG) << "Creating stream";
    StreamSource* source = (StreamSource*)CreateStreamItem(sourceClassID);
    StreamSink* sink = (StreamSink*)CreateStreamItem(sinkClassID);

    if (source && sink) // source and sink are mandatory
    {
    	outStream = new Stream(IDGenerator::Instance().Next(), source,
    							(StreamFilter*)CreateStreamItem(filterClassID), sink);
    	m_lockStreams.Lock();
    	m_streams.insert(std::pair<unsigned, Stream*>(outStream->GetStreamID(), outStream));
    	m_lockStreams.Unlock();

    	NotifyOnStreamCreate(*outStream);
    }
    else
    {
    	LOG(logERROR) << "Error creating stream";
    	DestroyStreamItem((PluginObject*)source);
    	DestroyStreamItem((PluginObject*)sink);
    }

    return outStream;
}

Stream* StreamFactory::CreateStream(Stream& s, const char* sourceClassID, const char* filterClassID, const char* sinkClassID)
{
	Stream* outStream = NULL;
	LOG(logDEBUG) << "Creating stream from Stream:" << s.GetStreamID();
	StreamSource* source = (sourceClassID != NULL ? (StreamSource*)CreateStreamItem(sourceClassID) : s.GetSource());
	StreamSink* sink = (StreamSink*)CreateStreamItem(sinkClassID);

	if (source && sink) // source and sink are mandatory
	{
		outStream = new Stream(IDGenerator::Instance().Next(), source,
					           filterClassID != NULL ? (StreamFilter*)CreateStreamItem(filterClassID) : s.GetFilter(),
					           sink);
		m_lockStreams.Lock();
		m_streams.insert(std::pair<unsigned, Stream*>(outStream->GetStreamID(), outStream));
		m_lockStreams.Unlock();

		NotifyOnStreamCreate(*outStream);
	}
	else
	{
		// clean up
		LOG(logERROR) << "Error creating stream";
		if (sourceClassID)
		{
			DestroyStreamItem((PluginObject*)source);
		}
		if (sinkClassID)
		{
			DestroyStreamItem((PluginObject*)sink);
		}
	}

	return outStream;
}

void StreamFactory::DestroyStream(Stream* s)
{
	LOG(logDEBUG) << "Destroy stream";
	if (s)
	{
		m_lockStreams.Lock();
		std::map<unsigned, Stream*>::iterator it = m_streams.find(s->GetStreamID());
		if (it != m_streams.end())
		{
			std::map<unsigned, std::string>::iterator itpub = m_pubStreams.find(it->first);
			if (itpub != m_pubStreams.end())
			{
				LOG(logINFO) << "Delete stream:" << itpub->second;
				m_pubStreams.erase(itpub);
			}
			else
			{
				LOG(logWARNING) << "Found stream with ID:" << it->first << " but no published name";
			}
			m_streams.erase(it);
		}
		else
		{
			LOG(logWARNING) << "No match found for stream:" << s->GetStreamID();
		}
		m_lockStreams.Unlock();

		LOG(logDEBUG) << "Notify listeners OnStreamDestroy()";
		m_lockListeners.Lock();
		for (std::list<StreamFactoryListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
		{
			(*it)->OnStreamDestroy(*s);
		}
		m_lockListeners.Unlock();

		DestroyStreamItem((PluginObject *)s->GetSource());
		DestroyStreamItem((PluginObject *)s->GetFilter());
		DestroyStreamItem((PluginObject *)s->GetSink());
		delete s;
	}
}

void StreamFactory::RegisterStreamItem(PluginModule* module, StreamItem item, const char* classID)
{
	LOG(logDEBUG) << "Add new " << itemNames[item];
	if (classID && module)
	{
		SystemMutexLocker lock(m_lockItems);
		LOG(logINFO) << "Add new " << itemNames[item] << " with ClassID:" << classID;
		std::map<StreamItem, std::map<std::string, PluginModule*> >::iterator it = m_streamItems.find(item);
		if (it != m_streamItems.end())
		{
			LOG(logDEBUG) << "Add " << itemNames[it->first] << " to m_streamItems with ClassID:" << classID;
			(it->second).insert(std::pair<std::string, PluginModule*>(classID, module));
		}
		else
		{
			LOG(logDEBUG) << "Add new type of item:" << itemNames[item] << " to m_streamItems with ClassID:" << classID;
			std::map<std::string, PluginModule*> mapItem;
			mapItem.insert(std::pair<std::string, PluginModule*>(classID, module));
			m_streamItems.insert(std::pair<StreamItem, std::map<std::string, PluginModule*> >(item, mapItem));
		}
	}
}

void StreamFactory::PublishStream(Stream& s, const char* name)
{
	LOG(logDEBUG) << "Publish stream:" << s.GetStreamID();
	SystemMutexLocker lock(m_lockStreams);
	std::map<unsigned, Stream*>::iterator it = m_streams.find(s.GetStreamID());
	if (it != m_streams.end())
	{
		std::map<unsigned, std::string>::iterator itpub = m_pubStreams.find(it->first);
		if (itpub != m_pubStreams.end())
		{
			LOG(logWARNING) << "Stream:" << it->first << " is already published with name:" << itpub->second;
		}
		else
		{
			if (name)
			{
				m_pubStreams.insert(std::pair<unsigned, std::string>(it->first, name));
				LOG(logINFO) << "Stream :" << name << " published";

				LOG(logDEBUG) << "Notify listeners OnStreamPublish()";
				SystemMutexLocker listenerLock(m_lockListeners);
				for (std::list<StreamFactoryListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
				{
					Task* eventTask = new Task();
					eventTask->Connect(&StreamFactoryListener::OnStreamPublish, *it, boost::ref(s));
					TaskThreadPool::Signal(eventTask);
				}
			}
		}
	}
}

void StreamFactory::UnpublishStream(Stream& s)
{
	LOG(logDEBUG) << "UnPublish stream:" << s.GetStreamID();
	SystemMutexLocker lock(m_lockStreams);
	std::map<unsigned, std::string>::iterator it = m_pubStreams.find(s.GetStreamID());
	if (it != m_pubStreams.end())
	{
		LOG(logINFO) << "Un-publish stream:" << it->second;
		m_pubStreams.erase(it);

		LOG(logDEBUG) << "Notify listeners OnStreamUnPublish()";
		SystemMutexLocker listenerLock(m_lockListeners);
		for (std::list<StreamFactoryListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
		{
			(*it)->OnStreamUnPublish(s);
		}
	}
	else
	{
		LOG(logWARNING) << "Stream:" << s.GetStreamID() << " is not publish";
	}
}

Stream* StreamFactory::GetPublishedStream(const char* name)
{
	Stream* outStream = NULL;
	if (name)
	{
		SystemMutexLocker lock(m_lockStreams);
		std::map<unsigned, std::string>::iterator itpub = m_pubStreams.begin();
		for (; itpub != m_pubStreams.end(); ++itpub)
		{
			if (itpub->second.compare(name) == 0)
			{
				std::map<unsigned, Stream*>::iterator it = m_streams.find(itpub->first);
				if (it != m_streams.end())
				{
					outStream = it->second;
					break;
				}
				else
				{
					LOG(logWARNING) << "Orphan stream name published" << name <<  " with no actual stream";
					m_pubStreams.erase(itpub); // remove orphan from published streams
				}
			}
		}
	}

	return outStream;
}

bool StreamFactory::IsPublished(unsigned ID)
{
	SystemMutexLocker lock(m_lockStreams);
	std::map<unsigned, std::string>::iterator it = m_pubStreams.find(ID);
	if (it != m_pubStreams.end())
	{
		LOG(logDEBUG) << "Stream is published:" << ID;
		return true;
	}

	return false;
}

const char* StreamFactory::GetStreamName(unsigned ID)
{
	SystemMutexLocker lock(m_lockStreams);
	std::map<unsigned, std::string>::iterator it = m_pubStreams.find(ID);
	if (it != m_pubStreams.end())
	{
		return it->second.c_str();
	}

	return NULL;
}

void StreamFactory::AddListener(StreamFactoryListener* listener)
{
	if (listener)
	{
		LOG(logDEBUG) << "Add listener to StreamFactory";
		SystemMutexLocker lock(m_lockListeners);
		m_listeners.remove(listener);
	}
}

void StreamFactory::RemoveListener(StreamFactoryListener* listener)
{
	if (listener)
	{
		LOG(logDEBUG) << "Remove listener from StreamFactory";
		SystemMutexLocker lock(m_lockListeners);
		m_listeners.push_back(listener);
	}
}

