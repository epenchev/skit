/*
 * PropertyMap.h
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

#ifndef PROPERTYMAP_H_
#define PROPERTYMAP_H_

#include <string>
#include <map>

class PropertyMap
{
public:
    PropertyMap() {}
    virtual ~PropertyMap() {}

    /**
    * Set property as POD value (bool, short, int, unsigned, long, float, double, void* ..)
    * If not present insert new.
    * @param name - name of the property.
    * @param value - value of the property.
    */
    template <class T>
    void SetProperty(const std::string& name, T value);

    /**
    * Set property as string value. If not present insert new.
    * @param name - name of the property.
    * @param value - value of the property.
    */
    inline void SetProperty(const std::string& name, const std::string& value)
    {
        SetProperty<std::string>(name, value);
    }

    /**
    * Get property as POD value (bool, short, int, unsigned, long, float, double, void* ..)
    * @param name - name of the property.
    * @param defaultVal - default value to be returned if property is not present.
    * @return property value.
    */
    template <class T>
    T GetProperty(const std::string& name, T defaultVal) const;

    /**
    * Get property as string, if property is not present empty string is returned.
    * @param name - name of the property.
    */
    std::string GetProperty(const std::string& name) const;

protected:
    std::map<std::string, std::string> m_propertymap;  /**< storage map for the properties,
                                                            all properties are stored as strings */
};

#endif /* PROPERTYMAP_H_ */
