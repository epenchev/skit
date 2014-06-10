//
// Buffer.h
// Copyright (C) 2014  Emil Penchev, Bulgaria

#ifndef BUFFER_H_
#define BUFFER_H_

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <vector>

class Buffer : public boost::asio::mutable_buffer
{
public:
    Buffer()
     : boost::asio::mutable_buffer(), m_allocated(false) {}
    
    Buffer(void* data, std::size_t size) 
     : boost::asio::mutable_buffer(data, size), m_allocated(false) {}
    
    Buffer(std::size_t size)
     : boost::asio::mutable_buffer( new unsigned char[size], size ), m_allocated(true) {}
    
    virtual ~Buffer()
    {
        if (m_allocated)
        {
            delete boost::asio::buffer_cast<unsigned char*>(*this);
        }
    }
    
    std::size_t Size() const
    { 
        return boost::asio::buffer_size( boost::asio::const_buffer(*this) );
    }
    
    std::size_t Copy(const Buffer& bufsrc)
    {
        boost::asio::const_buffer src(bufsrc);
        return boost::asio::buffer_copy( static_cast<boost::asio::mutable_buffer>(*this), src );
    }
    
    template <typename T>
    T Get() const
    { 
        return boost::asio::buffer_cast<T>( boost::asio::const_buffer(*this) ); 
    }
    
    template <typename T>
    T Get()
    { 
        return boost::asio::buffer_cast<T>(*this);
    }
    
    // Create a new modifiable buffer that is offset from the start of another.
    friend Buffer operator + (Buffer& b, std::size_t offset)
    {
        if ( offset < b.Size() )
        {
            Buffer buf( b.Get<unsigned char*>() + offset, b.Size() - offset ); 
            return buf;
        }
        else 
        { 
            Buffer buf; 
            return buf;
        }
    }
    
     // Create a new modifiable buffer that is offset from the start of another.
    friend Buffer operator + (std::size_t offset, Buffer& b)
    {
        return (b + offset);
    }
    
protected:
    bool m_allocated;
};

typedef std::vector<Buffer> BufferSequence;

#define CreateBufferSequence(buf) boost::asio::buffer(buf)


       

#endif /* BUFFER_H_ */


