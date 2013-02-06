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

// logging levels can be disabled at compile time
//LOGGING_DISABLE_LEVEL(::logging::Error);
//LOGGING_DISABLE_LEVEL(::logging::Trace);
//LOGGING_DISABLE_LEVEL(::logging::Warning);
//LOGGING_DISABLE_LEVEL(::logging::Info);

//LOGGING_DEFINE_OUTPUT( FileLogType );

int main(int argc, char* argv[])
{
/*
  log::emit< Error>()   << "Logging an Error " << log::dec << 15 << log::endl;
  log::emit< Trace>()   << "Logging a Trace"   << log::endl;
  log::emit< Warning>() << "Logging a Warning" << log::endl;
  log::emit< Info>()    << "Logging an Info"   << log::endl;
*/
  try
    {
      if (argc != 2)
      {
        std::cout << "Usage: blitz <url>\n";
        return 1;
      }

      boost::asio::io_service io_service;
      blitz::HttpSource source(io_service, argv[1]);
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


