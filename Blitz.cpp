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
#include "VODService.h"
#include "ControlChannel.h"
#include "WebService.h"

#include "HTTP/HTTPUtils.h"

#include "server/HTTPServer.h"
#include "system/TaskThread.h"
#include "server/ServerController.h"

int main(int argc, char* argv[])
{
#if 0
    std::string config_filename;
    std::vector<std::string> args(argv, argv+argc);

    /*
    HTTPHeadersMap outMap;
    outMap.insert(std::make_pair("User-Agent:", "Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.5) Gecko/20091102 Firefox)"));
    outMap.insert(std::make_pair("Accept-Language:" , "en-us,en;q=0.5"));

    outMap.insert(std::make_pair("Keep-Alive:", "300"));
    outMap.insert(std::make_pair("Connection:", "keep-alive"));

    HTTPUtils::HTTPRequestToString("http://abv.bg/stream.flv", "GET", "", outMap);
    */

    if (args.size() == 3)
    {
        if (args[1] == "-c")
        {
            config_filename = args[2];
        }
        else
        {
            printf("Usage: %s -c conf.xml \n", argv[0]);
            return 1;
        }
    }
    else
    {
        printf("Usage: %s -c conf.xml \n", argv[0]);
        return 1;
    }

    BLITZ_LOG_INFO("Starting blitz daemon");

    try
    {
        blitz::Config conf;
        conf.readConfig(config_filename);

        blitz::IOServicePool thread_pool(conf.getNumThreads());

        blitz::Daemon::daemonize(conf.getPidfile().c_str(), conf.getLogfile().c_str());

        blitz::WebService web_service(thread_pool.getIOService(), conf.getWebServicePort());

        for (unsigned i = 0; i < conf.getNumPipeline(); i++)
        {
            boost::asio::io_service& io_service = thread_pool.getIOService();
            blitz::DataSource* source = new blitz::HttpSource(io_service, conf.getPipelineSourceURL(i));
            blitz::DataSink* sink = new blitz::HttpSink(io_service, conf.getPipelineSinkPort(i), conf.getPipelineSinkIP(i),
                                                        conf.getPipelineName(i), conf.getPipelineID(i));
            source->addSink(sink);
            source->start();
            blitz::Controler* controler = dynamic_cast<blitz::Controler*>(sink);
            web_service.registerControler(controler);
        }

        if (conf.isVodServiceEnable())
        {
            boost::asio::io_service& io_service = thread_pool.getIOService();
            blitz::VODService* vservice = new blitz::VODService(io_service, conf.getVodServicePort(), conf.getVodServiceIP(), conf.getVodServiceFilePath());
            blitz::Controler* vservice_controler = dynamic_cast<blitz::Controler*>(vservice);
            web_service.registerControler(vservice_controler);
            vservice->start();
        }

        web_service.start();

        thread_pool.run();
    }
    catch (std::exception& e)
    {
        // this is in log file
        BLITZ_LOG_ERROR("Exception: %s", e.what());
        exit(1);
    }
#endif

    std::cout << " Server starting ... \n";

    HTTPServer server("192.168.97.72", 8000);
    server.Start();

    ServerController::LoadPlugin("/home/emo/workspace/blitz/modules/libmod_source_example.so");

    TaskThreadPool::AddThread();
    TaskThreadPool::AddThread();
    TaskThreadPool::AddThread();
    TaskThreadPool::AddThread();
    TaskThreadPool::StartThreads();

    while(1) {}


    return 0;
}


