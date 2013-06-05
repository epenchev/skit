/*
 * ControlChannel.h
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
 *  Created on: Mar 22, 2013
 *      Author: emo
 */

#ifndef CONTROLCHANNEL_H_
#define CONTROLCHANNEL_H_

#include <string>

namespace blitz {

typedef std::pair<std::string, std::string> command_param;
typedef std::pair<std::string, std::string> ControlerEvent;

class Command
{
public:
    Command(std::string& name) : m_module_name(name)
    {}
    virtual ~Command() {}

    inline std::string& module() { return m_module_name; }

    inline void insertParameter(command_param param) { m_command_params.push_back(param); }

    inline std::string getParameterValue(const char* name)
    {
        std::string empty = "";

        for (std::vector<command_param>::iterator it = m_command_params.begin() ; it != m_command_params.end(); ++it)
        {
            command_param param = *it;
            if (param.first.compare(name) == 0) return param.second;
        }

        return empty;
    }

    inline command_param getParameter(const char* name)
    {
        for (std::vector<command_param>::iterator it = m_command_params.begin() ; it != m_command_params.end(); ++it)
        {
            command_param param = *it;
            if (param.first.compare(name) == 0) return param;
        }
    }

protected:
    std::string m_module_name;
    std::vector<command_param> m_command_params;
};

class ControlChannel;

class Controler
{
public:
    virtual bool execCommand(Command& cmd, std::string& response_out) = 0;
    virtual void setChannel(ControlChannel* channel) = 0;
    virtual void closeChannel(ControlChannel* channel) = 0;
};

class ControlChannel
{
public:
    virtual void registerControler(Controler* contrl) = 0;
    virtual void unregisterControler(Controler* contrl) = 0;
    virtual void triggerEvent(ControlerEvent& ev) = 0;
};

} // blitz

#endif /* CONTROLCHANNEL_H_ */
