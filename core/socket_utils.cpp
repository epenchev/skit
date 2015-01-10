/*
 * socket_utils.cpp
 * Copyright (C) 2013  Emil Penchev, Bulgaria
 */

#include "socket_utils.h"

std::string socket_get_remote_address(tcp::socket& sock)
{
    std::string addr;
    if (sock.is_open())
    {
        boost::system::error_code err;
        boost::asio::ip::tcp::endpoint endpoint = sock.remote_endpoint(err);
        if (!err)
        {
            addr = endpoint.address().to_string(err);
            if (err)
                addr.clear();
        }
    }
    return addr;
}

unsigned short socket_get_remote_port(tcp::socket& sock)
{
    if (sock.is_open())
    {
        boost::system::error_code err;
        boost::asio::ip::tcp::endpoint endpoint = sock.remote_endpoint(err);
        if (!err)
            return endpoint.port();
    }
    return 0;
}

unsigned short socket_get_local_port(tcp::socket& sock)
{
    if (sock.is_open())
    {
        boost::system::error_code err;
        boost::asio::ip::tcp::endpoint endpoint = sock.local_endpoint(err);
        if (!err)
            return endpoint.port();
    }
    return 0;
}

