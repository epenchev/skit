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
#include <iostream>
#include <syslog.h>
#include <unistd.h>
#include "DataPacket.h"
#include "HttpSource.h"
#include "HttpSink.h"

#include "logging/logging.h"
using namespace ::logging;


int main(int argc, char* argv[])
{
    log::emit< Info>() << "Starting blitz daemon ..."   << log::endl;

    try
    {
        const char* media_url = "http://37.157.179.35:8086/bysid/700";
        boost::asio::io_service io_service;

        boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);
        signals.async_wait(boost::bind(&boost::asio::io_service::stop, &io_service));

        io_service.notify_fork(boost::asio::io_service::fork_prepare);

        if (pid_t pid = fork())
        {
            if (pid > 0)
                exit(0);
            else
            {
                std::cout << "[ERROR] first fork failed \n";
                return 1;
            }

        }
        setsid();
        umask(0);

        // A second fork ensures the process cannot acquire a controlling terminal.
        if (pid_t pid = fork())
        {
            if (pid > 0)
                exit(0);
            else
            {
                std::cout << "[ERROR] Second fork failed \n";
                return 1;
            }
        }
        close(0);
        close(1);
        close(2);

        // We don't want the daemon to have any standard input.
        if (open("/dev/null", O_RDONLY) < 0)
        {
            syslog(LOG_ERR | LOG_USER, "Unable to open /dev/null: %m");
            return 1;
        }

        // Send standard output to a log file.
        const char* output = "blitz.log";
        const int flags = O_WRONLY | O_CREAT | O_APPEND;
        const mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

        if (open(output, flags, mode) < 0)
        {
            syslog(LOG_ERR | LOG_USER, "Unable to open output file %s: %m", output);
            return 1;
        }

        // Also send standard error to the same log file.
        if (dup(1) < 0)
        {
            syslog(LOG_ERR | LOG_USER, "Unable to dup output descriptor: %m");
            return 1;
        }

        io_service.notify_fork(boost::asio::io_service::fork_child);

        blitz::HttpSource source(io_service, media_url);
        blitz::HttpSink sink(io_service, 9999);
        source.addSink(&sink);
        source.start();

        io_service.run();
    }
    catch (std::exception& e)
    {
        // this is in log file
        std::cout << "Exception: " << e.what() << "\n";
    }

    return 0;
}


