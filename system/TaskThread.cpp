/*
 * TaskThread.cpp
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
 *  Created on: Jun 18, 2013
 *      Author: emo
 */

#include "system/TaskThread.h"

void TaskThread::Entry()
{
    while (true)
    {
        Task* theTask = mQueueTasks.DeQueue();

        if (theTask)
        {
            theTask->Run();
            delete theTask;
        }
    }
}

void TaskThread::InsertTask(Task* inTask)
{
    this->mQueueTasks.EnQueue(inTask);
}

EventThread TaskThreadPool::mIOThread;
unsigned TaskThreadPool::lastUsedThread = 0;
SystemMutex TaskThreadPool::mLock;
std::vector<TaskThread*> TaskThreadPool::mThreadsArray;

void TaskThreadPool::StartThreads()
{
    mIOThread.Start();

    unsigned threadCount = mThreadsArray.size();
    for (unsigned i = 0; i < threadCount; i++)
    {
        TaskThread* thread = mThreadsArray.back();
        if (thread)
        {
            thread->Start();
        }
    }
}

void TaskThreadPool::AddThread()
{
    SystemMutexLocker locker(TaskThreadPool::mLock);

    TaskThread* taskThread = new TaskThread();
    if (taskThread)
    {
        mThreadsArray.push_back(taskThread);
        taskThread->Start();
    }
}

void TaskThreadPool::RemoveThreads()
{
    SystemMutexLocker locker(TaskThreadPool::mLock);

    unsigned threadCount = mThreadsArray.size();
    for (unsigned i = 0; i < threadCount; i++)
    {
        TaskThread* thread = mThreadsArray.back();
        if (thread)
        {
            delete thread;
        }

        mThreadsArray.pop_back();
    }
}

TaskThread* TaskThreadPool::GetThread(unsigned index)
{
    if (index)
    {
        if (mThreadsArray.size() > index)
        {
            return mThreadsArray[index];
        }
    }
    return NULL;
}

void TaskThreadPool::Signal(Task* inTask)
{
    if (!inTask)
    {
        return;
    }

    unsigned numThreads = mThreadsArray.size();

    if (numThreads)
    {
        if (TaskThreadPool::lastUsedThread >= (numThreads - 1))
        {
            lastUsedThread = 0;
        }
        else
        {
           lastUsedThread++;
        }

        TaskThread* taskThread = mThreadsArray[lastUsedThread];
        taskThread->InsertTask(inTask);
    }
}


