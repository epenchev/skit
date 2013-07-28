/*
 * ErrorCode.h
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
 *  Created on: Jul 2, 2013
 *      Author: emo
 */

#ifndef ERRORCODE_H_
#define ERRORCODE_H_

#include <string>

class ErrorCode
{
public:
    ErrorCode() { errMessage.clear(); }
    ErrorCode(const char* message) : errMessage(message) {}
    ErrorCode(std::string& message) : errMessage(message) {}

    virtual ~ErrorCode() {}

    std::string& GetMessage() { return errMessage; }

    void SetMessage(std::string& errMsg) { errMessage = errMsg; }
    void SetMessage(const char* errMsg) { errMessage = errMsg; }

    inline operator bool() const { return !errMessage.empty(); }
    inline bool operator!() const { return errMessage.empty(); }

    void Clear() {  errMessage.clear(); }

    inline ErrorCode& operator = (ErrorCode& err)
    {
        this->errMessage = err.errMessage;
        return *this;
    }

    bool operator == (ErrorCode& err) const
    {
        if (0 == this->errMessage.compare(err.errMessage))
            return true;
        else
            return false;
    }

    template <typename T>
    bool operator == (T errMessage) const
    {
        if (0 == this->errMessage.compare(errMessage))
            return true;
        else
            return false;
    }


private:
    std::string errMessage;
};

#endif /* ERRORCODE_H_ */
