/*
 * FileReaderTest.cpp
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
 *  Created on: Jan 24, 2014
 *      Author: emo
 */

#include <utils/FileReader.h>
#include <system/SystemThread.h>
#include <utils/Logger.h>

#define GLOBAL_READER_TEST
#define MAX_RD_SIZE 16000
#define FILEPATH "/var/www/htdocs/alrmas.mp4"

#ifdef GLOBAL_READER_TEST
FileReader globalFileReader;
SystemMutex globaFilelock;
#endif

class FileReaderThread : public SystemThread
{
public:

	FileReaderThread(unsigned id) : SystemThread(), m_id(id)
	{
#ifdef GLOBAL_READER_TEST
		m_reader = &globalFileReader;
#else
		m_reader = new FileReader();
#endif
	}
	virtual ~FileReaderThread() {}

	void Entry();
private:
	unsigned m_id;
	FileReader* m_reader;
};

void FileReaderThread::Entry()
{
	unsigned offset;
	long unsigned fsize;
	LOG(logINFO) << "Starting thread N:" << m_id;
#ifdef GLOBAL_READER_TEST
	globaFilelock.Lock();
	if (!m_reader->IsOpen())
	{
		LOG(logINFO) << "Opening file from thread N:" << m_id;
#endif
		if (!m_reader->Open(FILEPATH))
		{
			LOG(logERROR) << "Error opening file thread N:" << m_id;
#ifdef GLOBAL_READER_TEST
			globaFilelock.Unlock();
#endif
			return;
		}
#ifdef GLOBAL_READER_TEST
	}
	globaFilelock.Unlock();
#endif
	fsize = m_reader->GetSize();
	offset = (unsigned)(fsize - (fsize * 0.1));
	LOG(logDEBUG) << "offset:" << offset;
	while (offset < fsize)
	{
		char buf[MAX_RD_SIZE] = {0};
		unsigned readlen;
		if ((offset + MAX_RD_SIZE) <=  fsize)
		{
			readlen = MAX_RD_SIZE;
		}
		else
		{
			readlen = fsize - offset;
		}
		// Critical section
#ifdef GLOBAL_READER_TEST
		globaFilelock.Lock();
#endif
		if (m_reader->Seek(offset))
		{
			int rc = m_reader->Read(buf, readlen);
			if (rc != -1)
			{
#ifdef GLOBAL_READER_TEST
				globaFilelock.Unlock();
#endif
				offset += (readlen + 1);
			}
			else
			{
				LOG(logERROR) << "Error reading from file at offset:" << offset
						<< " bytes:" << readlen << " thread N:" << m_id;
#ifdef GLOBAL_READER_TEST
				globaFilelock.Unlock();
#endif
				return;
			}
		}
		else
		{
			LOG(logERROR) << "Error seeking to offset:" << offset << " thread N:" << m_id;
#ifdef GLOBAL_READER_TEST
			globaFilelock.Unlock();
#endif
			return;
		}
	}
	// Second test
	offset = 1;
	while (offset < fsize)
	{
		char buf[MAX_RD_SIZE] = {0};
		// Critical section
#ifdef GLOBAL_READER_TEST
		globaFilelock.Lock();
#endif
		if (m_reader->Seek(offset))
		{
			int rc;
			if ((offset + MAX_RD_SIZE) <= fsize)
			{
				rc = m_reader->Read(buf, MAX_RD_SIZE);
			}
			else
			{
				rc = m_reader->Read(buf, fsize - offset);
			}
			if (rc != -1)
			{
#ifdef GLOBAL_READER_TEST
				globaFilelock.Unlock();
#endif
				offset += (MAX_RD_SIZE + 1);
			}
			else
			{
				LOG(logERROR) << "Error reading from file at offset:" << offset
						<< " bytes:" << MAX_RD_SIZE << " thread N:" << m_id;
#ifdef GLOBAL_READER_TEST
				globaFilelock.Unlock();
#endif
				return;
			}
		}
		else
		{
			LOG(logERROR) << "Error seeking to offset:" << offset << " thread N:" << m_id;
#ifdef GLOBAL_READER_TEST
			globaFilelock.Unlock();
#endif
			return;
		}
	}
	LOG(logINFO) << "Thread finished N:" << m_id;
#ifndef GLOBAL_READER_TEST
	m_reader->Close();
#endif
}

int main(int argc, char* argv[])
{
	int maxThreads = 100;
	FileReaderThread* threads[maxThreads];
	LOG(logINFO) << "Test started";
	std::cout << "\n\n";
	for (int i = 0; i < maxThreads; i++)
	{
		threads[i] = new FileReaderThread(i+1);
		threads[i]->Start();
	}
	for (int i = 0; i < maxThreads; i++)
	{
		threads[i]->Join();
	}

	std::cout << "\n\n";
	LOG(logINFO) << "Test finished";

#ifdef GLOBAL_READER_TEST
	globalFileReader.Close();
#endif
	return 0;
}

