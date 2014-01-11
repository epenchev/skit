/*
 * FileReader.h
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

#ifndef FILEREADER_H_
#define FILEREADER_H_

#include <fstream>
#include <string>

/**
* Class for opening and read files.
*/
class FileReader
{
public:
    FileReader();
    virtual ~FileReader() {}

    /**
    * Open file for reading
    * @param filename - full path to the file.
    * @return bool - true on success, false otherwise.
    */
    bool Open(const std::string& filename);

    /**
    * Close the associated file resource
    */
    void Close();

    /*
    * Read from file size bytes.
    * @param buf -  allocated buffer to store data.
    * @param bytes - size bytes to read from file.
    * @return size bytes read from file or -1 on error. If bytes read < length test for eof flag with IsEof()
    */
    int Read(void* buf, int length);

    /*
    * Seek in file to given position.
    * @param position - position to seek in file.
    * @return bool - true if seek is success or false on error.
    */
    bool Seek(unsigned position);

    /**
    * Return true if end of file is reached.
    */
    bool IsEof();

    /**
    * Return true if file is open.
    */
    bool IsOpen();

    /**
    * Return true if file is open.
    */
    long long GetSize();

    /**
    * Get the name of the file reader is associated with.
    */
    const std::string& GetFileName();

    time_t LastWriteTime();

private:
    std::string m_filename;
    std::ifstream m_ifstream;
    std::ifstream::pos_type m_fsize;

};

#endif /* FILEREADER_H_ */
