/*
 * FsSource.cpp
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
 *  Created on: Feb 25, 2013
 *      Author: emo
 */

#include "FsSource.h"
#include "Log.h"
#include "DataPacket.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


namespace blitz {

FsSource::FsSource(const char* filename)
{
	/*

	if (filename)
    {
        BLITZ_LOG_INFO("Opening file for reading %s", filename);

        bytes = 0;
        input.open(filename , std::ifstream::binary);
        input.seekg (0, std::ios::beg);

        if (input.fail())
        {
            BLITZ_LOG_ERROR("Error Opening file for reading %s", filename);
        }
    }
    else
    {
        BLITZ_LOG_INFO("Error file name not set");
    }
*/


	fd = open (filename, O_LARGEFILE);
	if (fd == -1)
	{
		BLITZ_LOG_ERROR("Error Opening file for reading %s", filename);
	}
}

void FsSource::readPacket(void)
{

/*
  if (input.good())
  {
      DataPacket* packet = new DataPacket();

      if (!input.eof())
      {
          //input.read((char *)packet->data(), DataPacket::max_size);
    	  //input.get((char *)packet->data(), DataPacket::max_size);
    	  input.get((char *)packet->data(),  DataPacket::max_size);

          if (input.fail() || input.bad())
          {
              BLITZ_LOG_ERROR("The end of the source of characters is reached before n characters have been read, bytes are %d", bytes);
              //return;
          }

          if (input.gcount() < DataPacket::max_size)
          {
              packet->size(input.gcount());
          }
          else
          {
        	  packet->size( DataPacket::max_size);
          }
          //usleep(000);
          addData(packet);
          bytes += DataPacket::max_size;

      }
      else
      {
          BLITZ_LOG_WARNING("End read");
          input.close();
      }
  }
*/

	DataPacket* packet = new DataPacket();
	size_t result = read (fd, packet->data(), DataPacket::max_size);
	  if (result != DataPacket::max_size) { BLITZ_LOG_ERROR("Reading error"); }
	  //BLITZ_LOG_INFO("READ bytes %d", result);

	  addData(packet);

}

} // blitz
