/*
 * SystemTimer.h
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
 *  Created on: Jul 3, 2013
 *      Author: emo
 */

#ifndef SYSTEMTIMER_H_
#define SYSTEMTIMER_H_

#include <boost/asio.hpp>

class TimerObserver
{
public:
    /**
    * Triggered when timer is expired.
    */
    virtual void OnExpire() = 0;
};

/**
* Class representing an asynchronous timer.
*/
class SystemTimer
{
public:
    SystemTimer();
    virtual ~SystemTimer() {}

    /**
    * Set timer expiration in seconds from now.
    * @param seconds - time in seconds .
    */
    void ExpiresSec(unsigned int seconds);

    /**
    * Set timer expiration in microseconds from now.
    * @param useconds - time in microseconds .
    */
    void ExpiresUsec(unsigned int useconds);

    /**
    * Observer to be notified on timer event.
    * @param inListener - observer object to be notified .
    */
    void AddTimerListener(TimerObserver* inListener);

    /**
    * Start timer asynchronous, function will return immediately.
    */
    void Wait();

    /**
    * Cancel timer, no signal is sent to observer.
    */
    void Cancel();
private:
    boost::asio::deadline_timer mtimerImpl;      /**< Timer implementation  */
    TimerObserver*              mEventListener;  /**< Observer for timer events  */

    // boost IO handler
    void HandleTimer(const boost::system::error_code& error);
};

#endif /* SYSTEMTIMER_H_ */
