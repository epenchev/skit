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
#include <boost/noncopyable.hpp>
#include <boost/system/system_error.hpp>
#include <exception>

typedef boost::thread::id thread_id;

/**
* Exception error for the SystemThread classes.
*/

class ThreadError : public std::exception
{
public:
    ThreadError( const std::string& inMsg ) : mErrMessage(inMsg) {}

    virtual ~ThreadError() throw() {}

    virtual const char* what() const throw() { return mErrMessage.c_str(); }
private:
    std::string mErrMessage;
};

/**
* Class for platform independent thread abstraction based on boost implementation.
*/

class SystemThread : private boost::noncopyable
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
    */
    static void Sleep(unsigned int inMsec);

    /**
    * Pause current thread of execution for milliseconds inMsec.
    */
    static thread_id GetId();

    /**
    * Switch current thread of execution.
    */
    static void Yield();

    /**
    * Thread execution.
    */
    void operator() ();

private:
    bool mJoined;            /**< true if thread is joined */
    boost::thread* mpThread; /**< The thread  */
};

#endif /* SYSTEMTHREAD_H_ */
