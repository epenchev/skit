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

void Config::readConfig(const std::string &filename)
{
    ptree pt;
    try
    {
        read_xml(filename, pt);
        //m_logfile = pt.get<std::string>("system.logfile");
        //m_pidfile = pt.get<std::string>("system.pidfile");

        //ptree t = pt.get_child("modules");
        //m_modules_count = t.size();



        BOOST_FOREACH( ptree::value_type const& v, pt.get_child("modules") )
        {
            std::cout << v.first << std::endl;

            if( v.first == "module" )
            {
                std::cout << "name is " << v.second.get<std::string>("name") << std::endl;
                std::cout << "source is " << v.second.get<std::string>("source") << std::endl;
                std::cout << "sink is " << v.second.get<unsigned>("sink") << std::endl;
                //f.date = v.second.get<Date>("date");
                //f.cancelled = v.second.get("<xmlattr>.cancelled", false);
            }
        }

        for (ptree::iterator iter = pt.get_child("modules").begin(); iter != pt.get_child("modules").end(); iter++)
        {
          //std::cout << iter->first << "," << iter->second.data() << std::endl;
            if (iter->first == "module")
            {
                //ptree::value_type const& v = *iter;
                std::cout << "name is " << (*iter).second.get<std::string>("source") << std::endl;
            }
        }

    }
    catch(std::exception& ex)
    {
        std::cout << "Exception from boost::property_tree " << ex.what() << std::endl;
    }
}

void Config::printConfig(void)
{
    /*
    if (!m_logfile.empty() && !m_pidfile.empty())
    {
        std::cout << "pidfile is " << m_pidfile << std::endl;
        std::cout << "logfile is " << m_logfile << std::endl;
    }

    std::cout << "module count is " << m_modules_count << std::endl;
    for (std::set<std::string>::iterator it = m_module_names.begin(); it != m_module_names.end(); ++it)
    {
        std::cout << "Module name: " << *it << std::endl;
    }
    */
}

} // blitz


