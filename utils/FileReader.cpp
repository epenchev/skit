/*
 * FileReader.cpp
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
 *  Created on: Mar 13, 2013
 *      Author: emo
 */

#include "FileReader.h"
#include "Log.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>

namespace blitz {

FileReader::FileReader()
{}

bool FileReader::open(const char* filename)
{
    bool op_result = false;

    if (filename)
    {
        BLITZ_LOG_INFO("Opening file for reading %s", filename);

        m_ifstream.open(filename , std::ifstream::binary | std::ios_base::in);

        if (!m_ifstream.good() || m_ifstream.eof() || !m_ifstream.is_open())
        {
            BLITZ_LOG_ERROR("Error Opening file for reading %s", filename);
            return op_result;
        }

        m_ifstream.seekg(0, std::ios_base::end);
        m_fsize = m_ifstream.tellg();

        m_ifstream.seekg(0, std::ios_base::beg);

        op_result = true;
    }
    else
    {
        BLITZ_LOG_INFO("Error file name not set");
    }

    return op_result;
}

void FileReader::close()
{
    if (m_ifstream.is_open())
    {
        m_ifstream.close();
    }
}

void FileReader::seekInFile(int bytes)
{
    // 247382610
    m_ifstream.seekg(bytes, std::ios::beg);

    if (m_ifstream.eof() || m_ifstream.bad())
    {
        BLITZ_LOG_ERROR("Error seeking to position %d", bytes);
    }

}

int FileReader::read(void* buf, int length)
{
    int bytes_read = -1;
    if (!buf || !length)
    {
        return bytes_read;
    }

    if (m_ifstream.good() || !m_ifstream.eof())
    {
        m_ifstream.read((char* )buf, length);
        if (m_ifstream.fail() || m_ifstream.bad())
        {
            BLITZ_LOG_ERROR("Error reading from file");
            return bytes_read;
        }

        bytes_read = m_ifstream.gcount();
    }
    else
    {
        BLITZ_LOG_ERROR("Error reading from file");
    }
    return bytes_read;
}

} // blitz
