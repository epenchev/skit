/*
 * socket_acceptor.cpp
 * Copyright (C) 2013  Emil Penchev, Bulgaria
 */

#include "socket_acceptor.h"
#include "Task.h"

namespace smkit
{
namespace inet
{

socket_acceptor::socket_acceptor(unsigned short port, listener& listener, boost::asio::io_service& io_service)
 : m_port(port), m_listener(&listener), m_impl(io_service)
{
    m_address.clear();
}

socket_acceptor::socket_acceptor(std::string address, unsigned short port,
                                 listener& listener, boost::asio::io_service& io_service)
 : m_port(port), m_address(address), m_listener(listener), m_impl(io_service)
{}

socket_acceptor::~socket_acceptor()
{
    stop();
}

void socket_acceptor::listen()
{
    try
    {
        if (m_impl.is_open())
                return; // already listening

        m_impl.open(boost::asio::ip::tcp::v4());
        if (!m_address.empty())
        {
            //boost::asio::ip::address addr;
            //addr.from_string(m_address);
            //m_impl.bind(tcp::endpoint(addr, m_port));
        	boost::asio::ip::address addr;
        }
        else
            m_impl.bind(tcp::endpoint(tcp::v4(), m_port));

        m_impl.set_option(tcp::acceptor::reuse_address(true));
        m_impl.listen();
    }
    catch(boost::system::system_error& ex)
    {
        ErrorCode outError;
        //outError.SetMessage(ex.code().message());
        //throw SystemException(outError);
    };
    this->accept();
}

void socket_acceptor::stop()
{
    if (m_impl.is_open())
    {
        boost::system::error_code error; // no error handling here just disable exceptions
        m_impl.close(error);
    }
}

void socket_acceptor::accept()
{
    try
    {
        socket_thread_pair pair;
        task_scheduler& scheduler = task_scheduler::instance();
        pair.service_id = scheduler.next_thread();
        pair.sock = new tcp::socket(scheduler.get_thread_io_service(pair.service_id));
        m_impl.async_accept(*pair.sock, boost::bind(&socket_acceptor::handle_accept,
                                               this, pair, boost::asio::placeholders::error));
    }
    catch (std::exception& ex)
    {
        // perhaps log it
        ex.what();
    }
}

unsigned short socket_acceptor::port() const
{
    unsigned short inetport = 0;
    if (m_impl.is_open())
    {
    	boost::system::error_code err;
        tcp::endpoint endpoint = m_impl.local_endpoint(err);
        if (!err)
            port = endpoint.port();
    }
    return inetport;
}

void socket_acceptor::handle_accept(socket_thread_pair pair, const boost::system::system_error& err)
{
    if (err)
    {
        delete pair.sock;
        if (boost::asio::error::operation_aborted == err)
            return; // operation aborted, don't signal
    }
    // warning, if error is set socket is invalid object
    m_listener->on_accept(pair.sock, pair.service_id, err);
    // Continue accepting new connections automatically.
    this->accept();
}

}} // end of namespaces


