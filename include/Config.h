/*
 * Config.h
 *
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

#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>
#include <set>

namespace blitz {

class Config
{
public:
	Config() : m_modules_count(0)
	{}
	void readConfig(const std::string &filename);
	void printConfig(void);
private:
	int m_modules_count;
	std::string m_logfile;
	std::string m_pidfile;
	std::set<std::string> m_module_names;
	//void insertName(std::string name) { m_module_names.insert(name); }

};

} // blitz

#endif /* CONFIG_H_ */
