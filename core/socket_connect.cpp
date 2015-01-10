/*
 * socket_connect.cpp
 * Copyright (C) 2013  Emil Penchev, Bulgaria
 */

#include "socket_connect.h"

void handle_resolve(tcp::socket* sock, socket_connect_listener* listener, boost::system::error_code& err,
                    tcp::resolver::iterator endpoint_iterator)
{
    if (err)
    {
        if (boost::asio::error::operation_aborted == err)
                return; // operation aborted, don't signal

        // signal only in case of error
        if (listener)
            listener->on_connect(*sock, err);
    }
    else
    {
        boost::asio::async_connect(*sock, endpoint_iterator,
            boost::bind(&handle_connect, sock, listener, boost::asio::placeholders::error));
    }
}

void handle_connect(tcp::socket* sock, socket_connect_listener* listener, boost::system::error_code& err)
{
    if (err)
    {
        if (boost::asio::error::operation_aborted == err)
                return; // operation aborted, don't signal
    }

    if (listener)
        listener->on_connect(*sock, err);
}

void socket_async_connect(tcp::socket& sock, std::string host, std::string service,
                          socket_connect_listener& listener)
{
    boost::system::error_code err;
    boost::asio::ip::address address = boost::asio::ip::address::from_string(host, err);
    if (err)
    {
            // host is not IP address, resolve it
            tcp::resolver resolver(sock.get_io_service());
            tcp::resolver::query query(host, service);
            resolver.async_resolve(query,
                boost::bind(handle_resolve, &listener, boost::asio::placeholders::error,
                            boost::asio::placeholders::iterator));
    }
    else
    {
        if (!service.empty())
        {
            boost::asio::ip::tcp::endpoint endpoint(address, atoi(service.c_str()));
            sock.async_connect(endpoint, boost::bind(handle_connect, &listener, boost::asio::placeholders::error));
        }
    }
}


