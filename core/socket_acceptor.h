/*
 * socket_acceptor.h
 *  Copyright (C) 2013  Emil Penchev, Bulgaria
 */

#ifndef SOCKET_ACCEPTOR_H_
#define SOCKET_ACCEPTOR_H_

#include <string>
#include <boost/asio.hpp>

using namespace boost::asio::ip;

namespace smkit
{
namespace inet
{
/// Accept incoming TCP connections and notify listeners
class socket_acceptor
{
public:

    /// Listener to be notified for every incoming TCP connection by SocketAcceptor instance. */
    class listener
    {
    public:
        /// Triggered when a new socket connection is accepted. Listener object receives connected TCP socket object and a thread id
        /// used for scheduling async tasks and I/O.
        virtual void on_accept(tcp::socket* socket, boost::thread::id service_id, boost::system::error_code& err) = 0;
        virtual ~listener() {}
    };

    socket_acceptor(unsigned short port, listener& listener, boost::asio::io_service& io_service);
    socket_acceptor(std::string address, unsigned short port, listener& listener, boost::asio::io_service& io_service);

    ~socket_acceptor();

    /// Close SocketAcceptor object and stop accepting incoming connections.
    void stop();

    /// Return the port socket acceptor is listening on.
    unsigned short port() const;

    /// Listen for incoming connections.
    void listen();

private:

    struct socket_thread_pair
    {
        tcp::socket* sock;
        boost::thread::id service_id;
    };

    unsigned short                 m_port;
    std::string                    m_address;
    listener*                      m_listener;
    boost::asio::ip::tcp::acceptor m_impl;

    /// Accepts incoming connections.
    void accept();

    /// boost tcp::acceptor handler callback function
    void handle_accept(socket_thread_pair pair, const boost::system::error_code& err);
};

class acceptor_error : public std::exception
{
public:
	acceptor_error(boost::system::system_error& err) { m_err_code = err; }
    ~acceptor_error() throw() {}
    virtual const char* what() throw() { return m_err_code.message(); }
    boost::system::error_code& error_code() { return m_err_code; }
private:
    boost::system::error_code m_err_code;
};

}}

#endif /* SOCKET_ACCEPTOR_H_ */
