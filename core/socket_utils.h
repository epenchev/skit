/*
 * socket_utils.h
 * Copyright (C) 2013  Emil Penchev, Bulgaria
 */

#ifndef SOCKET_UTILS_H_
#define SOCKET_UTILS_H_

#include <string>
#include <boost/asio.hpp>

using namespace boost::asio::ip;

/// Get the IP address to the connected end point, returns empty string on error.
std::string socket_get_remote_address(tcp::socket& sock);

/// Get the remote port to the connected end point, returns 0 on error.
unsigned short socket_get_remote_port(tcp::socket& sock);

/// Get the local listening port, returns 0 on error.
unsigned short socket_get_local_port(tcp::socket& sock);

#endif /* SOCKET_UTILS_H_ */
