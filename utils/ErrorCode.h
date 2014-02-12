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
    ErrorCode();
    ErrorCode(const ErrorCode& err);

    virtual ~ErrorCode();

    /**
    * Get the error message if error is set.
    */
    const char* Message() const;

    /**
    * Set error message, also sets the error if message string is not empty.
    */
    void SetMessage(const std::string& message);

    void operator = (bool iset);

    ErrorCode& operator = (const ErrorCode& err);

    operator bool() const;

    bool operator!() const;

protected:
    bool m_iset;             /**< error set/unset flag */
    std::string m_message;   /**< error message */
};

#include <exception>

class SystemException : public std::exception
{
public:
    SystemException(ErrorCode& err) { m_errcode = err; }
    ~SystemException() throw() {}
    virtual const char* what() throw() { return m_errcode.Message(); }
    ErrorCode& Code() { return m_errcode; }
protected:
    ErrorCode m_errcode;
};


#endif /* ERRORCODE_H_ */
