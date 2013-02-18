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
#include <boost/bind.hpp>
#include "DataPacket.h"
#include "HttpSource.h"
#include "HttpSink.h"
#include "Log.h"
#include "Daemon.h"
#include "Config.h"

int main(int argc, char* argv[])
{
    BLITZ_LOG_INFO("Starting blitz daemon");

    try
    {
    	const char* media_url = "http://127.0.0.1:8080/";
    	boost::asio::io_service io_service;
    	blitz::Config c;

    	c.readConfig("../conf.xml");
    	c.printConfig();


    	/*
    	blitz::Daemon::daemonize("blitz.lock", "blitz.log");

        blitz::HttpSource source(io_service, media_url);
        blitz::HttpSink sink(io_service, 9999);
        source.addSink(&sink);
        source.start();

        io_service.run();
        */
    }
    catch (std::exception& e)
    {
        // this is in log file
        BLITZ_LOG_ERROR("Exception: %s", e.what());
        exit(1);
    }

    return 0;
}


