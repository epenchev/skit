/*
 * TcpServer.h
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
 *  Created on: Jan 30, 2013
 *      Author: emo
 */

#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#include <set>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>


using boost::asio::ip::tcp;

namespace blitz {

class TCPConnection
  : private boost::noncopyable
{
public:
    TCPConnection(boost::asio::io_service& io_service)
          : m_sock(io_service), m_connected(false) {}

    virtual ~TCPConnection() {}

    virtual void start(void);
    virtual void close(void);

    inline bool isConnected(void) const { return m_connected; }
    tcp::socket& socket() { return m_sock; }

    /**
     * returns the client's IP address
     */
    inline boost::asio::ip::address getRemoteIP(void) const
    { return m_sock.remote_endpoint().address(); }

    /**
     * returns the client's port number
     */
    inline unsigned short getRemotePort(void)
    { return m_sock.remote_endpoint().port(); }

private:
    tcp::socket m_sock;
    bool m_connected;
};

class TCPServer
    : private boost::noncopyable
{
public:
   void start(void);
   void stop(void);

   /**
    * returns tcp port number that the server listens for connections on
    */
   inline unsigned int getPort(void) const { return m_tcp_acceptor.local_endpoint().port(); }

   /**
    * returns IP address that the server listens for connections
    */
   inline boost::asio::ip::address getAddress(void) const { return m_tcp_acceptor.local_endpoint().address(); }

   inline unsigned getConnectionCount(void) { return m_conn_pool.size(); }

   virtual ~TCPServer() { if (m_is_listening) stop(); }

   inline boost::asio::ip::address getServerIP(void) const
   { return m_tcp_acceptor.local_endpoint().address(); }
protected:
   TCPServer(boost::asio::io_service& io_service, const boost::asio::ip::tcp::endpoint& endpoint);

   TCPServer(boost::asio::io_service& io_service, const unsigned int tcp_port);

    virtual void handleStartServer(void) {}
    virtual void handleStopServer(void) {}
    virtual TCPConnection* createTCPConnection(boost::asio::io_service& io_service) { return NULL; }

    std::set<TCPConnection*> m_conn_pool;
private:
    void accept(void);
    void handleAccept(TCPConnection* new_connection, const boost::system::error_code& error);

    tcp::acceptor m_tcp_acceptor;
    bool m_is_listening;      /**< set to true when the server is listening for new connections */
};

} // blitz

#endif /* TCPSERVER_H_ */
