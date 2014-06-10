//
// HTTP.h
// Copyright (C) 2014  Emil Penchev, Bulgaria

#ifndef HTTP_H_
#define HTTP_H_

#include <map>
#include <string>

namespace Skit {
namespace HTTP {

// Basic HTTP header parser 
class Parser
{
public:
    Parser();
    virtual ~Parser() {}

    // Read HTTP header input from 'data' string and process it.
    bool ReadHeader(const std::string& data);

    // Get a HTTP header value by 'name', returns empty string if header is not present
    std::string GetHeader(const std::string& name) const;
    
    // Set header 'value' or insert new header field if 'name' is not present.
    void SetHeader(std::string name, std::string value);
    
    // Set header 'name' to integer value 'value' or insert new header field.
    void SetHeader(std::string name, int value);
     
    // Trims any whitespace at the front or back of the string 'str'.
    void Trim(std::string & str) const;
    
    // Get the body from response/request if present.
    std::string GetBody() const;
    
    // Sets the body of a response or request, along with the correct Content-Length header.
    void SetBody(const std::string& data);
    
    // Get the request/response protocol (example: HTTP/1.1)
    std::string GetProtocol() const;
    
    // erase headers and body and set default protocol
    virtual void Reset();

protected:
    
    // read/parse the body part of request/response
    void ReadBody(const std::string& data);
    
    std::string m_body;
    std::string m_protocol;
    std::map<std::string, std::string> m_headers;
};

// HTTP request parser
class Request : public Parser
{
public:
    Request() : Parser() {}
    
    // Read/parse request from string.
    void Init(const std::string& inData);

    // Get the HTTP method : GET, POST, HEAD
    std::string GetMethod() const;

    // Get the request URL
    std::string GetURL() const;
    
    // Get the request URI (same as URL minus the query string)
    std::string GetURI() const;

    // Get a request parameter variable value
    std::string GetParamVar(const std::string& name) const;
    
    // Set a request parameter variable value
    void SetParamVar(const std::string& name, const std::string& value);
    
    // Returns a string containing a valid HTTP 1.1 request, ready for sending. 
    // Use Init() method first to add request line.
    std::string& BuildRequest(const std::string& method, const std::string url);
    
    // erase and set default values to members
    void Reset();

private:
    std::string m_url;
    std::string m_method;
    std::string m_requestBuilder;
    std::map<std::string, std::string> m_reqvars; // GET or POST variable params data
    
    // Read request variable parameters
    void ReadQueryParams(const std::string& data);
};

class Response : public Parser
{
public:
    Response() : Parser() { Reset(); }
    
    // Read/parse response from string.
    void Init(const std::string& inData);
    
    // Returns a string with a valid HTTP/1.1 response with HTTP response 'code' and 'message' ready for sending.
    std::string& BuildResponse(const std::string& code, const std::string& message);
    
    // Get the HTTP response code
    int GetResponseCode() const;
    
    // Get the HTTP response message
    std::string GetResponseMessage() const;
    
    // erase and set default values to members
    void Reset();
    
private:
    unsigned m_responseCode;        // HTTP response code
    std::string m_responseMessage;  // HTTP response message
    std::string m_responseBuilder;
};

} // HTTP
} // Skit

#endif /* HTTP_H_ */



