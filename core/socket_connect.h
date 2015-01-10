/*
 * socket_connect.h
 *  Copyright (C) 2013  Emil Penchev, Bulgaria
 */

#ifndef SOCKET_CONNECT_H_
#define SOCKET_CONNECT_H_

#include <boost/asio.hpp>
using namespace boost::asio::ip;

/// Listener to be notified when async connect operation is complete.
class socket_connect_listener
{
public:
    virtual void on_connect(tcp::socket& sock, boost::system::error_code& err) = 0;
    virtual ~socket_connect_listener() {}
};

/// Connect to remote host to netService as port number or service name.
void socket_async_connect(tcp::socket& sock, std::string host, std::string service,
                          socket_connect_listener& listener);


#endif /* SOCKET_CONNECT_H_ */
