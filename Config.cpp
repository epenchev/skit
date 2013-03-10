/*
 * Config.cpp
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
 *  Created on: Feb 17, 2013
 *      Author: emo
 */

#include "Config.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <string>

using boost::property_tree::ptree;
using boost::lexical_cast;

namespace blitz {

template<typename T>
inline T getDataIter(ptree::iterator& iter, const char* xml_node)
{
    /* (*iter).second.get<std::string>("<tag>"); */

    ptree::value_type const& v = *iter;
    return v.second.get<T>(xml_node);
}

template<typename T>
inline T getData(ptree& pt, const char* xml_node)
{
    return pt.get<T>(xml_node);
}

inline static void dumpPair(ptree::iterator& iter)
{
    std::cout << iter->first << "-->" << iter->second.data() << std::endl;
}

template<typename T>
T* createObject()
{
    T* object = new T();
    return object;
}

void Config::readConfig(const std::string &filename)
{
    ptree pt;
    try
    {
        read_xml(filename, pt);

        m_logfile = getData<std::string>(pt, "blitz.logfile");
        m_pidfile = getData<std::string>(pt, "blitz.pidfile");
        m_threads = getData<unsigned int>(pt, "blitz.threads");

        for (ptree::iterator iter = pt.get_child("blitz").begin(); iter != pt.get_child("blitz").end(); iter++)
        {
            if (iter->first == "pipeline")
            {
                std::string type = getDataIter<std::string>(iter, "<xmlattr>.type");

                if (0 == type.compare("http"))
                {
                    HttpPipelineConfig conf;

                    conf.id = getDataIter<unsigned int>(iter, "<xmlattr>.id");
                    conf.id = getDataIter<unsigned int>(iter, "<xmlattr>.id");
                    conf.name = getDataIter<std::string>(iter, "<xmlattr>.name");

                    conf.source_url = getDataIter<std::string>(iter, "source");

                    ptree pt_sink = (*iter).second.get_child("sink");

                    conf.sink_ip = getData<std::string>(pt_sink, "ip");
                    conf.sink_port = getData<unsigned short>(pt_sink, "port");
                    conf.sink_sessions = getData<unsigned short>(pt_sink, "sessions");

                    m_pipeline_configs.push_back(conf);
                }
            }
        }
    }
    catch(std::exception& ex)
    {
        std::cout << "Exception from boost::property_tree " << ex.what() << std::endl;
    }
}

unsigned short Config::getPipelineID(unsigned id)
{
    return m_pipeline_configs.at(id).id;
}

unsigned short Config::getPipelineSinkPort(unsigned id)
{
    return m_pipeline_configs.at(id).sink_port;
}

unsigned short Config::getPipelineSinkMaxSesssions(unsigned id)
{
    return m_pipeline_configs.at(id).sink_sessions;
}

std::string& Config::getPipelineName(unsigned id)
{
    return m_pipeline_configs.at(id).name;
}

std::string& Config::getPipelineSourceURL(unsigned id)
{
    return m_pipeline_configs.at(id).source_url;
}

std::string& Config::getPipelineSinkIP(unsigned id)
{
    return m_pipeline_configs.at(id).sink_ip;
}

} // blitz


