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

namespace blitz {

/**
* Class for opening and read from media files such as .avi .mov ..
*/
class FileReader
{
public:
    FileReader();
    virtual ~FileReader() {}

    /**
    * Open file for reading
    * @param filename.
    * @return bool: true on success, false otherwise.
    */
    bool open(const char* filename);

    /**
    * Close the associated file resource
    */
    void close();

    /*
     * Read from file size bytes.
     * @param buf: allocated buffer to store data.
     * @param bytes: size bytes to read from file.
     * @return size bytes read from file or -1 on error. If bytes read < length test for eof flag with is_eof()
     */
    int read(void* buf, int length);

    void seekInFile(int bytes);

    inline bool is_eof() { return  m_ifstream.eof(); }
    inline bool is_open() { return m_ifstream.is_open(); }
    inline long long getSize(void) { return static_cast<long long>(m_fsize); }

private:
    std::ifstream m_ifstream;
    std::ifstream::pos_type m_fsize;

};

} // blitz

#endif /* FILEREADER_H_ */
