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
#include "Logger.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <cstring>
#include <iostream>

FileReader::FileReader()
{}

bool FileReader::Open(const std::string& filename)
{
    bool result = false;
    if (!filename.empty())
    {
        m_filename = filename;
        LOG(logDEBUG) << "Opening file for reading "<< m_filename;
        m_ifstream.open(filename.c_str() , std::ifstream::binary | std::ios_base::in);

        if (!m_ifstream.good() || m_ifstream.eof() || !m_ifstream.is_open())
        {
            LOG(logERROR) << "Unable to open file for reading " << filename;
            return result;
        }
        m_ifstream.seekg(0, std::ios_base::end);
        m_fsize = m_ifstream.tellg();
        m_ifstream.seekg(0, std::ios_base::beg);

        result = true;
    }
    else
    {
        LOG(logERROR) << "Error file name not set";
    }

    return result;
}

void FileReader::Close()
{
    LOG(logDEBUG) << "Closing file "<< m_filename;
    if (m_ifstream.is_open())
    {
        m_ifstream.close();
    }
}

bool FileReader::Seek(unsigned position)
{
    bool result = false;
    if (position)
    {
        m_ifstream.seekg(position, std::ios::beg);
        if (m_ifstream.eof() || m_ifstream.bad())
        {
            LOG(logERROR) << "Error seeking to position:"<< position;
        }
    }

    return result;
}

bool FileReader::IsEof()
{
    return  m_ifstream.eof();
}

bool FileReader::IsOpen()
{
    return m_ifstream.is_open();
}

long long FileReader::GetSize()
{
    return static_cast<long long>(m_fsize);
}

int FileReader::Read(void* buf, int length)
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
            LOG(logERROR) << "Error reading from file " << m_filename;
            return bytes_read;
        }
        bytes_read = m_ifstream.gcount();
    }
    else
    {
        LOG(logERROR) << "Error reading from file " << m_filename;
    }

    return bytes_read;
}

time_t FileReader::LastWriteTime()
{
    struct stat statBuf;
    memset(&statBuf, 0, sizeof(statBuf));
    if (IsOpen())
    {
        int err = stat(m_filename.c_str(), &statBuf);
        if (-1 == err)
        {
            LOG(logERROR) << "Unable to get file status " << m_filename
                          << " " << strerror(errno);
        }
    }

    return statBuf.st_mtime;
}

