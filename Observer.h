/*
 * Subject.h
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
 *  Created on: Jan 25, 2013
 *      Author: emo
 */

#ifndef SUBJECT_H_
#define SUBJECT_H_

#include <list>

// Part of the Observer - Subject pattern
namespace blitz {

class Subject;

class Observer
{
public:
    virtual ~Observer() {}
    virtual void update(Subject* changed_subject) = 0;
protected:
    Observer() {}
};

class Subject
{
public:
    Subject() {}
    virtual ~Subject() {}

    virtual void attach(Observer* ob);
    virtual void detach(Observer* ob);
    virtual void notify();
private:
    std::list<Observer*> m_observers;
};

} // blitz

#endif /* SUBJECT_H_ */
