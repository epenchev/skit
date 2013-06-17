/*
 * TaskThread.h
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
 *  Created on: May 31, 2013
 *      Author: emo
 */

#ifndef TASKTHREAD_H_
#define TASKTHREAD_H_

#include <queue>
#include "SystemThread.h"
#include "Task.h"

/**
* Specific SystemThread class to run Tasks. Every Task must be inserted into TaskThread for later execution.
*/
class TaskThread : public SystemThread
{
public:
    TaskThread() {}
    virtual ~TaskThread() {}

    /**
    * Inherit from SystemThread, thread entry point.
    */
    void Entry();

    /**
    * Insert Task in thread's queue for later execution.
    */
    void InsertTask(Task* inTask);

private:
    std::queue<Task*> mQueueTasks;
};

#endif /* TASKTHREAD_H_ */
