/*
 * SystemLib.h
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
 *  Created on: Aug 15, 2013
 *      Author: emo
 */

#ifndef SYSTEMLIB_H_
#define SYSTEMLIB_H_

/**
* Class for loading OS dependent shared library files,
* .dll files in Windows and .so files in Unix/Linux .
*/

#include "ErrorCode.h"

class SystemLib
{
public:

    /**
    * Constructor for the SystemLib class.
    * @param filePath - file to load given as full path.
    * Note: call GetLastError() method after creating object for confirming loading is successfull.
    */
    SystemLib();

    /**
    * Destructor of the SystemLib class. Unloads the shared library from memory.
    */
    ~SystemLib();

    void LoadFile(const char* filePath);

    /**
    * Get symbol from library, library first must be loaded .
    * @param inSymbolName - name of the symbol lookup from library.
    * @return void* - valid pointer to code or NULL otherwise.
    */
    void* GetSymbol(const char* inSymbolName, ErrorCode& outErr);

    //ErrorCode& GetLastError() { return mErrCode; }
private:
    void* mLibHandler;    /**< library handler */
    ErrorCode  mErrCode;  /**< error code of last operation */
};

#endif /* SYSTEMLIB_H_ */
