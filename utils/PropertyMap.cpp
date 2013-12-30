/*
 * PropertyMap.cpp
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
 *  Created on: Dec 19, 2013
 *      Author: emo
 */

#include "utils/PropertyMap.h"
#include <sstream>
#include <iostream>

template <class T>
void PropertyMap::SetProperty(const std::string& name, T value)
{
    if (!name.empty())
    {
        std::stringstream ss;
        ss << value;
        if (m_propertymap.count(name) > 0)
        {
            // change value of property
            std::map<std::string, std::string>::iterator it = m_propertymap.find(name);
            it->second = ss.str();
        }
        else
        {
            // insert new property
            m_propertymap.insert( std::pair<std::string, std::string>(name, ss.str()) );
        }
    }
}

template <class T>
T PropertyMap::GetProperty(const std::string& name, T defaultVal) const
{
    T propertyVal = defaultVal;
    if (!name.empty())
    {
        if (m_propertymap.count(name) > 0)
        {
            std::map<std::string, std::string>::const_iterator it = m_propertymap.find(name);
            std::stringstream ss(it->second);
            ss >> propertyVal;
        }
    }
    return propertyVal;
}

std::string PropertyMap::GetProperty(const std::string& name) const
{
    std::string propertyVal;
    if (!name.empty())
    {
        if (m_propertymap.count(name) > 0)
        {
            std::map<std::string, std::string>::const_iterator it = m_propertymap.find(name);
            propertyVal = it->second;
        }
    }
    return propertyVal;
}
