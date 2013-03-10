/*
 * IOServicePool.h
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
 *  Created on: Feb 21, 2013
 *      Author: emo
 */

#ifndef IOSERVICEPOOL_H_
#define IOSERVICEPOOL_H_

#include <vector>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>


namespace blitz {

/*
class IOServiceThread
{
public:
    IOServiceThread();
    virtual ~IOServiceThread() {}
    void run();
    void stop();
    boost::asio::io_service& getIOService(void);
private:
    boost::asio::io_service m_io_service;
};
*/

class IOServicePool : private boost::noncopyable
{
public:
    IOServicePool(unsigned pool_size);
    virtual ~IOServicePool() {}
    void run();
    void run(unsigned thread_id);
    void stop();
    void stop(unsigned thread_id);
    boost::asio::io_service& getIOService();
private:
    typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;
    std::vector<io_service_ptr> m_io_services;

    // The next io_service to use for a connection.
    std::size_t m_next_io_service;
};

} // blitz

#endif /* IOSERVICEPOOL_H_ */
