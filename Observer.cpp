/*
 * Observer.cpp
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
 *  Created on: Jan 27, 2013
 *      Author: emo
 */

#include "Observer.h"
#include "logging/logging.h"

using namespace ::logging;

namespace blitz {

void Subject::attach(Observer* ob)
{
    try
    {
        m_observers.push_back(ob);
    }
    catch (std::bad_alloc& ex)
    {
        log::emit< Error>() << "Subject::Attach() exception std::bad_alloc from list" << log::endl;
        throw;
    };
}

void Subject::detach(Observer* ob)
{
    log::emit< Trace>() << "Subject::detach() removing observer" << log::endl;
    m_observers.remove(ob);
}

void Subject::notify()
{
    if (!m_observers.empty())
    {
        for (std::list<Observer*>::iterator it = m_observers.begin(); it != m_observers.end(); ++it)
        {
            log::emit< Trace>() << "Subject::notify() notifing observer" << log::endl;
            Observer* ob = *it;
            ob->update(this);
        }
    }
}

} // blitz
