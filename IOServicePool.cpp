/*
 * IOServicePool.cpp
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

#include "IOServicePool.h"
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

namespace blitz {

IOServicePool::IOServicePool(unsigned pool_size) : m_next_io_service(0)
{
	if (pool_size == 0)
	{
	    throw std::runtime_error("io_service_pool size is 0");
	}

	for (unsigned i = 0; i < pool_size; i++)
    {
		io_service_ptr io_service(new boost::asio::io_service);
		m_io_services.push_back(io_service);
    }
}

boost::asio::io_service& IOServicePool::getIOService()
{
	boost::asio::io_service& io_service = *m_io_services[m_next_io_service];
	++m_next_io_service;

	if (m_next_io_service == m_io_services.size())
	{
		m_next_io_service = 0;
	}

	return io_service;
}


void IOServicePool::run()
{
	// Create a pool of threads to run all of the io_services.
	std::vector<boost::shared_ptr<boost::thread> > threads;

	for (std::size_t i = 0; i < m_io_services.size(); ++i)
	{
	    boost::shared_ptr<boost::thread> thread(new boost::thread(
	          boost::bind(&boost::asio::io_service::run, m_io_services[i])));
	    threads.push_back(thread);
	}

	// Wait for all threads in the pool to exit.
	for (std::size_t i = 0; i < threads.size(); ++i)
	{
		threads[i]->join();
	}
}

void IOServicePool::stop()
{
	// Explicitly stop all io_services.
	for (std::size_t i = 0; i < m_io_services.size(); ++i)
	{
		m_io_services[i]->stop();
	}
}

} // blitz



