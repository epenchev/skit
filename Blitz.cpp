/*
 * Blitz.cpp
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
 *  Created on: Jan 6, 2013
 *      Author: emo
 */

#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include "DataPacket.h"
#include "HttpSource.h"
#include "HttpSink.h"
#include "Log.h"
#include "Daemon.h"
#include "Config.h"
#include "IOServicePool.h"
#include "VODMediaServer.h"

int main(int argc, char* argv[])
{
    BLITZ_LOG_INFO("Starting blitz daemon");

    try
    {
        blitz::Config conf;
        conf.readConfig("conf.xml");

        blitz::IOServicePool thread_pool(conf.getNumThreads());
        std::cout << "Threads: " << conf.getNumThreads() << std::endl;
        blitz::Daemon::daemonize(conf.getPidfile().c_str(), conf.getLogfile().c_str());

        //blitz::VODMediaServer vod_server(io_service, 9999);
        //vod_server.start();

        for (unsigned i = 0; i < conf.getNumPipeline(); i++)
        {
            boost::asio::io_service& io_service = thread_pool.getIOService();
            blitz::DataSource* source = new blitz::HttpSource(io_service, conf.getPipelineSourceURL(i));
            blitz::DataSink* sink = new blitz::HttpSink(io_service, conf.getPipelineSinkPort(i));
            std::cout << conf.getPipelineSourceURL(i) << std::endl;
            source->addSink(sink);
            source->start();
        }

        thread_pool.run();
    }
    catch (std::exception& e)
    {
        // this is in log file
        BLITZ_LOG_ERROR("Exception: %s", e.what());
        exit(1);
    }

    return 0;
}


