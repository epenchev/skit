//
// ErrorCode.h
// Copyright (C) 2013  Emil Penchev, Bulgaria

#ifndef ERRORCODE_H_
#define ERRORCODE_H_

#include <string>

class ErrorCode
{
public:
    ErrorCode() : m_iset(false) {}
    ErrorCode(const ErrorCode& err) : m_iset(false)
    {
        SetMessage(err.Message());
	}

    // Get the error message if error is set.
    const char* Message() const { return m_message.c_str(); }

    // Set error message, sets the error if message string is not empty.
    void SetMessage(const std::string& message)
    {
        if (!message.empty())
	    {
		    m_iset = true;
		    m_message = message;
	    }
    }

    void operator = (bool iset)
    {
        m_iset = iset;
	    if (!m_iset)
	    {
		    m_message.clear();
	    }
	}

    ErrorCode& operator = (const ErrorCode& err)
    {
        m_iset = false;
	    SetMessage(err.Message());
	    return *this;
	}

    operator bool() const { return m_iset; }

    bool operator!() const { return !(m_iset); }

protected:
    bool m_iset;             // error set/unset flag
    std::string m_message;   // error message
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
