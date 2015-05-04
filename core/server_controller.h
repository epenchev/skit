//
// ServerController.h
// Copyright (C) 2014  Emil Penchev, Bulgaria

#ifndef SERVER_CONTROLLER_H_
#define SERVER_CONTROLLER_H_

#include "Stream.h"
//#include "PropertyTree.h"

#include "task.h"
#include "socket_acceptor.h"
#include "reg_factory.h"

#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

namespace smkit
{

class server_handler
{
public:
    virtual ~server_handler() {}

    /// Factory creator method, to be overridden by subclasses.
    static server_handler* create_item() { return NULL; }

    /// Custom hook method for accepting incoming TCP connections, implementation is in subclasses.
    /// Custom server_handlers receive a connected TCP socket object ready for I/O.
    virtual void accept_connection(tcp::socket* socket) = 0;
};

/// main server/stream controller
/// Responsible for managing server_handlers and streams.
/// Reads xml configuration file and starts up the whole software machine (socket acceptors, media streams ...)
/// and gives access to modules to the internal parts of the system.
class server_controller : public inet::socket_acceptor::listener
{
public:
	/// Factory for registering all the server handler classes.
	typedef reg_factory<server_handler> server_handler_factory_t;

    /// server_controller is a singleton, can be accessed only with this method.
	static server_controller& instance()
    {
        static server_controller s_controller;
        return s_controller;
    }

    /// read server configuration and start.
    void load(const string& filename);

    /// From socket_acceptor::listener
    void on_accept(tcp::socket* socket, boost::thread::id service_id, boost::system::error_code& err);

    /// Get a stream configuration with this name.
    Stream* get_stream_config(const std::string& name);

private:

    server_controller();
    ~server_controller() {}

    void load_streams();

    boost::property_tree::ptree                        m_config_tree;      // XML global configuration object
    boost::asio::io_service                            m_io_service;       // main process io_service, all socket acceptors are attached on.
    std::list<inet::socket_acceptor*>                  m_socket_acceptors; // socket acceptors listening for incoming connections and notifying the server controller.
    std::map<std::string, Stream*>                     m_streams;          // all media streams associated by names.
    std::map<task_scheduler::thread_id_t, std::map<unsigned int, server_handler*> > m_server_handlers; // server handlers map, thread_id -> map(id, handler)
};
} //end of smkit

#endif // SERVER_CONTROLLER_H_
