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

#include "DataPacket.h"
#include "HttpSource.h"
#include "HttpSink.h"
#include "logging/logging.h"
#include <iostream>

using namespace ::logging;

//#define LOGGING_DEFINE_EXTENDED_OUTPUT_TYPE
//LOGGING_DEFINE_OUTPUT( FileLogType )

const char* media_url = "http://37.157.179.35:8086/bysid/700";

int main(int argc, char* argv[])
{
	log::emit< Info>() << "Starting blitz daemon ..."   << log::endl;

	try
	{
		boost::asio::io_service io_service;
		blitz::HttpSource source(io_service, media_url);
		blitz::HttpSink sink(io_service, 9999);
		source.addSink(&sink);
		source.start();
		io_service.run();
	}
    catch (std::exception& e)
    {
    	std::cout << "Exception: " << e.what() << "\n";
    }

    return 0;
}


