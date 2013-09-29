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
#include <cerrno>
#include <cstring>
#include <iostream>
#include <exception>

class ErrorCode
{
public:
    ErrorCode() : mErrCodeValue(0) { errMessage.clear(); }
    ErrorCode(int errv) : mErrCodeValue(errv) { errMessage.clear(); }
    ErrorCode(const char* message) : mErrCodeValue(0), errMessage(message) {}
    ErrorCode(std::string& message) : mErrCodeValue(0), errMessage(message) {}

    ErrorCode(const ErrorCode& err)
    {
        this->errMessage = err.errMessage;
        this->mErrCodeValue = err.mErrCodeValue;
    }

    virtual ~ErrorCode() {}

    std::string& GetMessage() { return errMessage; }
    void SetValue(int errv)
    {
        this->mErrCodeValue = errv;
    }

    const char* GetErrorMessage()
    {
        /* custom error */
        if ((-1 == mErrCodeValue) && (NULL != errMessage.c_str()))
        {
            return errMessage.c_str();
        }

        const char* errorMsg = std::strerror(mErrCodeValue);
        if (!errorMsg)
        {
            return "Unknown error";
        }
        return errorMsg;
    }

    void SetMessage(std::string& errMsg) { errMessage = errMsg; }
    void SetMessage(const char* errMsg) { errMessage = errMsg; }

    //inline operator bool() const { return !errMessage.empty(); }
    inline operator bool() const { return (mErrCodeValue != 0); }

    //inline bool operator!() const { return errMessage.empty(); }
    inline bool operator!() const { return !(mErrCodeValue != 0); }

    void Clear()
    {
        errMessage.clear();
        mErrCodeValue = 0;
    }

    inline ErrorCode& operator = (ErrorCode& err)
    {
        this->errMessage = err.errMessage;
        this->mErrCodeValue = err.mErrCodeValue;

        return *this;
    }

    inline ErrorCode& operator = (int errCodeValue)
    {
        this->mErrCodeValue = errCodeValue;
        return *this;
    }

    inline bool operator == (ErrorCode& err) const
    {
        return (this->mErrCodeValue == err.mErrCodeValue ? true : false);
    }


/*
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
*/

private:
    int mErrCodeValue;
    std::string errMessage;
};


class SystemException : public std::exception
{
public:
    SystemException(ErrorCode& err) { this->mErrCode = err; }
    ~SystemException() throw() {}
    virtual const char* what() throw() { return mErrCode.GetErrorMessage(); }
    ErrorCode& Code() { return mErrCode; }
private:
    ErrorCode mErrCode;
};


#endif /* ERRORCODE_H_ */
