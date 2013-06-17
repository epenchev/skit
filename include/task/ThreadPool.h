/*
 * ThreadPool.h
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
 *   Created on: May 31, 2013
 *      Author: emo
 */

#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <boost/noncopyable.hpp>
#include <vector>
#include "TaskThread.h"

/**
* Singleton class, one global thread pool for managing all the threads in the server.
*/

class ThreadPool : private boost::noncopyable
{
    ThreadPool() {}
    virtual ~ThreadPool() {}

    static void AddThreads(unsigned numToAdd);
    static void  RemoveThreads();
    static TaskThread* GetThread(unsigned index);
    static unsigned  GetNumThreads();
private:
    static std::vector<TaskThread*> mThreadsArray;
};

#endif /* THREADPOOL_H_ */
