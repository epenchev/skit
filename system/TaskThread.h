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

#include "SystemThread.h"
#include "SynchronisedQueue.h"
#include "Task.h"

#include <vector>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#define EventTaskThreadIndex 0
#define MainTaskThreadIndex 1

/**
* Specific SystemThread class to run Tasks. Every Task must be inserted into TaskThread for later execution.
*/
class TaskThread : public SystemThread
{
public:
    TaskThread() : SystemThread() {}
    virtual ~TaskThread() {}

    /**
    * Inherit from SystemThread, thread entry point.
    */
    void Entry();

    /**
    * Insert Task in thread's queue for later execution.
    * @param inTask - pointer to Task to be inserted in queue.
    */
    void InsertTask(Task* inTask);

    /**
    * @return - tasks in the queue.
    */
    unsigned GetTaskCount() { return mQueueTasks.GetLength(); }

private:
    // TODO low priority task queue and high priority task queue
    SynchronisedQueue<Task*> mQueueTasks; /**< container for the tasks */
};

/**
* Specific EventThread class for socket events and timer events.
*/
class EventThread : public SystemThread
{
public:
    EventThread() : SystemThread() {}
    virtual ~EventThread() {}

    /**
    * Inherit from SystemThread, thread entry point.
    */
    void Entry() { mIOServiceLoop.run(); }

private:
    boost::asio::io_service mIOServiceLoop;

    friend class TCPSocket;
    friend class TCPAcceptor;
    friend class SystemTimer;
};

/**
* Singleton class, one global thread pool for managing all the threads in the server.
*/
class TaskThreadPool : private boost::noncopyable
{
public:
    static void StartThreads();

    static EventThread& GetEventThread() { return mIOThread; }
    /**
    * Add threads (TaskThread) to the pool.
    * @param numToAdd - count threads (TaskThread) to be added to the pool.
    */
    static void AddThread();

    /**
    * Remove all the threads in the pool.
    */
    static void RemoveThreads();

    /**
    * Return TaskThread pointer by index.
    * @param index - index to the thread which will be returned.
    * @return TaskThread* - pointer to thread.
    */
    static TaskThread* GetThread(unsigned index);

    /**
    * Get thread count in the pool.
    * @return unsigned - thread count.
    */
    static unsigned  GetNumThreads(); /*{ return mThreadsArray.size(); }*/

    /**
    * Signal a Task to be executed.
    * The Task is added to thread in the pool for execution.
    */
    static void Signal(Task* inTask);

protected:
    TaskThreadPool();
    virtual ~TaskThreadPool() {}

private:
    static EventThread mIOThread;                         /**< Thread only for I/O operations and timers  */
    static SystemMutex mLock;                             /**< Thread safety access to mThreadsArray */
    static std::vector<TaskThread*> mThreadsArray;        /**< Dynamic thread storage */
    static unsigned lastUsedThread;
};



#endif /* TASKTHREAD_H_ */
