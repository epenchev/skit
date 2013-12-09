/*
 * SystemThread.h
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
 *  Created on: May 29, 2013
 *      Author: emo
 */
#ifndef SYSTEMTHREAD_H_
#define SYSTEMTHREAD_H_

#include <boost/thread/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/locks.hpp>
#include <boost/noncopyable.hpp>
#include <boost/system/system_error.hpp>
#include "ErrorCode.h"

typedef boost::thread::id thread_id;

/**
* Class for thread abstraction based on boost implementation.
*/
class SystemThread : boost::noncopyable
{
public:
    SystemThread();
    virtual ~SystemThread();

    /**
    * Virtual function implemented by subclasses executed on thread Start().
    */
    virtual void Entry() = 0;

    /**
    * Fires up the thread, throws SystemError on error.
    */
    void Start();

    /**
    * Pause current thread of execution for milliseconds inMsec, throws ThreadError on error.
    */
    void Join();

    /**
    * Pause current thread of execution for milliseconds inMsec.
    * @param inMsec - count milliseconds thread will sleep
    */
    static void Sleep(unsigned int inMsec);

    /**
    * Get the ID of the current running thread.
    * @return thread_id type.
    */
    static thread_id GetID();

    /**
    * Switch current thread of execution.
    */
    static void Yield();

    /**
    * Thread execution.
    */
    void operator() ();

    ErrorCode& GetLastError() { return mErrCode; }

private:
    ErrorCode       mErrCode;           /**< Error code of last operation */
    bool            mJoined;            /**< true if thread is joined */
    boost::thread*  mpThread;           /**< The thread  */
};

/**
* Class mutex abstraction based on boost implementation.
*/
class SystemMutex : boost::noncopyable
{
public:
    SystemMutex()  {}
    ~SystemMutex() {}

    /**
    * Lock a mutex object.
    * @note Will block if another thread already has acquired the lock.
    *       Use TryLock() method instead if this is a problem.
    */
    void Lock();

    /**
    * Unlock a mutex object.
    */
    void Unlock();

    /**
    * Try to lock a mutex object.
    * @return true on successful grab of the lock, false on failure.
    */
    bool TryLock();

    ErrorCode& GetLastError() { return mErrCode; }
private:
    ErrorCode               mErrCode;          /**< Error code of last operation */
    boost::mutex            mLockableMutex;   /**< The mutex */
    boost::recursive_mutex  mRecursiveMutex;  /**< The recursive mutex */

    friend class SystemMutexLocker;
};

/**
* Class for mutex locker abstraction based on boost implementation.
*/
class SystemMutexLocker : boost::noncopyable
{
public:
    SystemMutexLocker(SystemMutex& inMutex);
    ~SystemMutexLocker() {}

private:
    boost::unique_lock<boost::mutex> mMutexLock; /**< The actual lock */

    friend class SystemCondVariable;
};


/**
* Class for condition variable abstraction based on boost implementation.
*/
class SystemCondVariable : boost::noncopyable
{

public:
    SystemCondVariable() {}
    ~SystemCondVariable() {}

    /**
    * If any threads are currently blocked waiting in a call to Wait(),
    * NotifyOne() unblocks one of those threads.
    */
    void NotifyOne();

    /**
    * If any threads are currently blocked waiting in a call to Wait(),
    * NotifyAll() unblocks all of those threads.
    */
    void NotifyAll();

    /**
    * Blocks the current thread until NotifyOne() or NotifyAll() is invoked.
    * @param inMutex - mutex variable for a lock.
    */
    void Wait(SystemMutexLocker& inLock);

private:
    boost::condition_variable mCondVariable;
};


#endif /* SYSTEMTHREAD_H_ */





