//
// HTTP.cpp
// Copyright (C) 2014  Emil Penchev, Bulgaria

#include "HTTP.h"
#include "Logger.h"

#include <cstdlib>
#include <sstream>

using namespace std;

static const std::string CurrentDateTime()
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);

    /* Visit http://www.cplusplus.com/reference/clibrary/ctime/strftime/
    * for more information about date/time format
    * strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    */
    strftime(buf, sizeof(buf), "%a, %d %b %g %T GMT", &tstruct);

    return buf;
}

Skit::HTTP::Parser::Parser()
{
    Reset();
}

bool Skit::HTTP::Parser::ReadHeader(const std::string& data)
{
    size_t endheader = data.find("\r\n\r\n");
    if (string::npos == endheader)
    {
        LOG(logDEBUG) << "End of header not found";
        return false;
    }

    size_t endln = 0;
    size_t currentPos = 0;

    while (endln < endheader)
    {
        endln = data.find_first_of('\n', currentPos);
        if (endln != string::npos)
        {
            string line = data.substr(currentPos, (endln - currentPos));
            currentPos = endln + 1;
            if (line.empty())
            {
                break;
            }
            else
            {
                size_t splitPos = line.find_first_of(':');
                if (splitPos != string::npos)
                {
                    string hname = line.substr(0, splitPos);
                    string hvalue = line.substr(splitPos + 2, line.length()); // skip ':' and space
                    m_headers[hname] = hvalue;  // insert pair name->value into header map

                    LOG(logDEBUG) << hname << " : " << hvalue;
                }
                else
                {
                    LOG(logDEBUG) << "Error no split : character in header line";
                    return false;
                }
            }   
        }
        else break;
    }
    return true;
}

void Skit::HTTP::Parser::Trim(std::string & str) const
{
    size_t startpos = str.find_first_not_of(" \t");
    size_t endpos = str.find_last_not_of(" \t");
    
    if ( (string::npos == startpos) || (std::string::npos == endpos) )
    {
        str = "";
    }
    else
    {
        str = str.substr(startpos, endpos - startpos + 1);
    }
}

void Skit::HTTP::Parser::SetHeader(std::string name, std::string value)
{
    Trim(name);
    Trim(value);
    m_headers[name] = value;
}

void Skit::HTTP::Parser::SetHeader(std::string name, int value)
{
    ostringstream ss;   

    Trim(name);
    ss << value;
    m_headers[name] = ss.str();
}

std::string Skit::HTTP::Parser::GetHeader(const std::string& name) const
{
    map<string, string>::const_iterator it = m_headers.find(name);
    if (m_headers.end() != it)
    {
        return it->second; 
    }

    LOG(logDEBUG) << "No header field with this name : " << name;
    return "";
}

std::string Skit::HTTP::Parser::GetBody() const
{
    return m_body;
}

void Skit::HTTP::Parser::ReadBody(const std::string& data)
{
    if (GetHeader("Content-Length") != "")
    {
        unsigned length = atoi(GetHeader("Content-Length").c_str());
        if (m_body.capacity() < length)
        {
            m_body.reserve(length);
        }
            
        size_t endheader = data.find("\r\n\r\n");
        if (string::npos != endheader)
        {
            size_t endheaderOffset = (endheader + 4);
            if ((endheaderOffset + length) <= data.length()) // do not go beyond the string end
            {
                m_body = data.substr(endheader + 4, length);
            }
        }
    }
}

std::string Skit::HTTP::Parser::GetProtocol() const
{
    return m_protocol;
}

void Skit::HTTP::Parser::SetBody(const std::string& data)
{
    if (!data.empty())
    {
        m_body = data;
        SetHeader("Content-Length", data.length());
    }
}

void Skit::HTTP::Parser::Reset()
{
    m_body.clear();
    m_headers.clear();
    m_protocol = "HTTP/1.1";
}

void Skit::HTTP::Request::Init(const std::string& inData)
{
    string buffer = inData;

    size_t endln = buffer.find_first_of('\n');
    if (endln != string::npos)
    {
        // get request line
        string reqline = buffer.substr(0, endln);
        LOG(logDEBUG) << "request line : " << reqline;
        buffer.erase(0, endln + 1); // remove also \n
        
        size_t linepos = reqline.find(' ');
        if (linepos != string::npos)
        {
            m_method = reqline.substr(0, linepos);
            reqline.erase(0, linepos + 1); // remove method
            linepos = reqline.find(' ');
            LOG(logDEBUG) << reqline;
            if (linepos != string::npos)
            {
                m_url = reqline.substr(1, linepos); // skip leading '/'
                reqline.erase(0, linepos + 1); // remove url
                m_protocol = reqline;
            }
        }
        
        if ( m_url.find('?') != string::npos )
        {
        	ReadQueryParams( m_url.substr( m_url.find('?') + 1) ); //read GET variables
        }

        // parse header fields
        if (!ReadHeader(buffer))
        {
            LOG(logDEBUG) << "Error reading headers";
        }
        
        ReadBody(buffer);
        if (m_body.length())
        {
            ReadQueryParams(m_body); //read POST variables
        }
    }
}

void Skit::HTTP::Request::ReadQueryParams(const std::string& data)
{
    std::string name;
    std::string value;
  
    // position where a part start (e.g. after &)
    size_t pos = 0;
    while (pos < data.length())
    {
        size_t nextpos = data.find('&', pos);
        if (string::npos == nextpos)
        {
            nextpos = data.length();
        }
    
        size_t eq_pos = data.find('=', pos);
        if (eq_pos < nextpos)
        {
            // there is a key and value
            name = data.substr(pos, eq_pos - pos);
            value = data.substr(eq_pos + 1, nextpos - eq_pos - 1); 
        }
        else
        {
            // no value, only a key
            LOG(logDEBUG) << "No value for key " << name;
            name = data.substr(pos, nextpos - pos);
            value.clear();
        }

        LOG(logDEBUG) << name << "=" << value;
        SetParamVar(name, value);
    
        if (nextpos == std::string::npos)  // in case the string is gigantic
        {
            break;
        }
        pos = nextpos + 1;
    }
}

void Skit::HTTP::Request::SetParamVar(const std::string& name, const std::string& value)
{
    string paramName = name;
    string paramValue = value;

    Trim(paramName);
    Trim(paramValue);
    
    //only set if there is actually a key
    if (!paramName.empty())
    {
        m_reqvars[paramName] = paramValue;
    }
}

std::string Skit::HTTP::Request::GetMethod() const
{
    return m_method;
}

std::string Skit::HTTP::Request::GetURL() const
{
    return m_url;
}

std::string Skit::HTTP::Request::GetURI() const
{
    size_t uripos = m_url.find('?');
    if ( uripos != std::string::npos )
    {
        return m_url.substr(0, uripos);
    }

    return m_url;
}

std::string Skit::HTTP::Request::GetParamVar(const std::string& name) const
{
    map<string, string>::const_iterator it = m_reqvars.find(name);
    if (m_reqvars.end() != it)
    {
        return it->second; 
    }

    LOG(logDEBUG) << "No such parameter : " << name;

    return "";
}

void Skit::HTTP::Request::Reset()
{
    Skit::HTTP::Parser::Reset();
    m_method = "GET";
    m_url.clear();
    m_reqvars.clear();
}

std::string& Skit::HTTP::Request::BuildRequest(const std::string& method, const std::string url)
{
    string outUrl = url;
    map<string, string>::iterator it;

    if (!m_reqvars.empty())
    {
        string reqParams;
        for (it = m_reqvars.begin(); it != m_reqvars.end(); it++)
        {
            if (it->first != "" && it->second != "")
            {
                reqParams += '&';
                reqParams += it->first + '=' + it->second;
            }
        }

        if ("GET" == method)
        {
            outUrl += '?';
            outUrl += reqParams;
        }
        else if ("POST" == method)
        {
            SetBody(reqParams);
        }
    }
 
    m_requestBuilder = method + " " + outUrl + " " + m_protocol + "\r\n";
    for (it = m_headers.begin(); it != m_headers.end(); it++)
    {
        if ( it->first != "" && it->second != "")
        {
            m_requestBuilder += it->first + ": " + it->second + "\r\n";
        }
    }
    
    m_requestBuilder += "\r\n";
    
    if (!m_body.empty())
    {
        m_requestBuilder += m_body;
    }
      
    return m_requestBuilder;
}


void Skit::HTTP::Response::Init(const std::string& inData)
{
    string buffer = inData;
    
    size_t endln = buffer.find_first_of('\n');
    if (endln != string::npos)
    {
        // get response line
        string responseline = buffer.substr(0, endln);
        LOG(logDEBUG) << "responseline line : " << responseline;
        buffer.erase(0, endln + 1); // remove also \n
        
        size_t protopos = responseline.find(' ');
        if (protopos != string::npos) // validate protocol
        {
            m_protocol = responseline.substr(0, protopos);
            responseline.erase(0, protopos + 1);
            size_t codepos = responseline.find(' ');
            if (codepos != string::npos)
            {
                m_responseCode = atoi( responseline.substr(0, codepos).c_str() ); // get HTTP response code
                responseline.erase(0, codepos + 1); // remove code
                m_responseMessage = responseline; // get message;
            }
        }
    }

    if (!ReadHeader(buffer))
    {
        LOG(logDEBUG) << "Error reading headers";
    }
    
    ReadBody(inData);
}

void Skit::HTTP::Response::Reset()
{
    Skit::HTTP::Parser::Reset();
    m_responseCode = 200;
    m_responseMessage = "OK";
}

int Skit::HTTP::Response::GetResponseCode() const
{
    return m_responseCode;
}
    
std::string Skit::HTTP::Response::GetResponseMessage() const
{
    return m_responseMessage;
}

std::string& Skit::HTTP::Response::BuildResponse(const std::string& code, const std::string& message)
{   
    map<string, string>::iterator it;

    m_responseBuilder = m_protocol + " " + code + " " + message + "\r\n";
    SetHeader("Server", "Skit media server");
    SetHeader("Date", CurrentDateTime());
    
    for (it = m_headers.begin(); it != m_headers.end(); it++)
    {
        if (it->first != "" && it->second != "")
        {
            m_responseBuilder += it->first + ": " + it->second + "\r\n";
        }
    }
    
    m_responseBuilder += "\r\n";

    if (!m_body.empty())
    {
        m_responseBuilder += m_body;
    }
    
    return m_responseBuilder;
}


