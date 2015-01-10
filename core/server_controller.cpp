//
// server_controller.cpp
// Copyright (C) 2014  Emil Penchev, Bulgaria

#include "Logger.h"

#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <string>

#include "server_controller.h"
#include "socket_utils.h"

namespace smkit
{

server_controller::server_controller()
{}

void server_controller::on_accept(tcp::socket* socket, task_scheduler::thread_id_t service_id, boost::system::error_code& err)
{
    if (err)
    {
        LOG(logWARNING) << err.message();
        return;
    }
    server_handler* handler = m_server_handlers[service_id][socket_get_local_port(*socket)];
    if (handler)
    {
        task_scheduler* scheduler = &task_scheduler::instance();
    	scheduler->queue_task(task::connect(&server_handler::accept_connection, handler, socket), service_id);
    }
    else
    {
        LOG(logERROR) << "Missing server handler for thread: " << service_id << " and port: " << socket_get_local_port(*socket);
    	delete socket;
    }
}

void server_controller::load(const std::string& filename)
{
    int thread_count;
    boost::property_tree::ptree::iterator it;
    boost::property_tree::ptree::iterator itend;
    std::map<std::string, unsigned> handlers_config;

    // assign global log level
    // Log::ReportingLevel() = logERROR; */

    try
	{
    	read_xml(filename, m_config_tree);
    	thread_count = m_config_tree.get<int>("threads");
    	bool run_daemon = m_config_tree.get<bool>("daemon");
    	std::string log_file = m_config_tree.get<std::string>("logfile");
    	std::string pid_file = m_config_tree.get<std::string>("pidfile");
	}
    catch(std::exception& ex)
    {
        LOG(logERROR) << filename << " : " << ex.what();
        return;
    }

    // server configuration starts here
    try
    {
    	it = m_config_tree.get_child("servers").begin();
    	itend = m_config_tree.get_child("servers").end();
    	while (it != itend)
    	{
    		std::string server_name = it->second.get<std::string>("name");
    		unsigned server_port = it->second.get<unsigned>("listen");
    		it++;

    		// Server name and server port are mandatory
    		// for a complete server handler configuration.
    		if (server_name.empty() || !server_port)
    		{
    			LOG(logWARNING) << "Incomplete server configuration, empty 'name' or 'listen' section";
    			continue;
    		}
    		handlers_config[server_name] = server_port;
		}
    }
	catch(std::exception& ex)
	{
		LOG(logERROR) << "Error in section 'servers' " << " : " << ex.what();
		return;
	}

	// Check for HTTP server configuration
	if (!handlers_config.count("http"))
	{
	    LOG(logERROR) << "No server HTTP configuration present in " << filename;
	    return;
	}

	if (handlers_config.empty())
	{
	    LOG(logERROR) << "No server configuration present in " << filename;
	    return;
	}

	// So far so good basic configuration is complete,
	// continue loading the server.
	try
	{
	    task_scheduler::instance().run(thread_count);

	    std::map<std::string, unsigned>::iterator it;
	    for (it = handlers_config.begin(); it != handlers_config.end(); it++ )
	    {
	        inet::socket_acceptor* acceptor = new inet::socket_acceptor(it->second, this, m_io_service);
	        m_socket_acceptors.push_back(acceptor);
	        acceptor->listen();
	    }
	}
	catch(std::bad_alloc& ex)
	{
	    LOG(logERROR) << "Error allocating memory :" << ex.what();
	}
	catch(task_scheduler_error& err)
	{
	    LOG(logERROR) << "Exception caught from scheduler->run() :" << err.what();
	    return; // must be replaced with exit and a cleanup routine.
	}

	/* Associate each server handler to a thread */
	for (int i = 0; i < thread_count; i++)
	{
	    std::map<std::string, unsigned>::iterator it;
	    for (it = handlers_config.begin(); it != handlers_config.end(); it++ )
	    {
	        server_handler* handler = server_handler_factory_t::create_instance(it->first);  //HandlerFactory::CreateInstance(it->first);
	        if (!handler)
	        {
	            LOG(logWARNING) << "No registered server handler with this name : " << it->first;
	            continue;
	        }
	        m_server_handlers[task_scheduler::instance().next_thread()][it->second] = handler;
	    }
	}
	load_streams();
	// run boost loop
	m_io_service.run();
}

void server_controller::load_streams()
{
	boost::property_tree::ptree::iterator it;
	boost::property_tree::ptree::iterator itend;
    try
    {
    	it = m_config_tree.get_child("streams").begin();
        itend = m_config_tree.get_child("streams").end();
    }
    catch (std::exception& ex)
    {
        LOG(logERROR) << ex.what();
        return;
    }

    while (it != itend)
    {
        try
        {
            std::string name = it->second.get<std::string>("name");
            std::string location = it->second.get<std::string>("location");
            if (!name.empty() && !location.empty())
            {
                Stream* the_stream = new Stream(it);
                m_streams[name] = the_stream;
                /* the_stream->Start(); */
            }
            else
            {
                LOG(logWARNING) << "Incomplete stream configuration, empty 'name' or 'location' section";
            }
            it++;
        }
        catch( exception& ex )
        {
            LOG(logERROR) << "Missing 'name' or 'location' section in stream object : " << ex.what();
            return;
        }
    }
}

Stream* server_controller::get_stream_config(const std::string& name)
{
    LOG(logDEBUG) << "stream name to search is :" << name;
    if (m_streams.count(name) > 0)
    {
        LOG(logDEBUG) << "Got stream with :" << name;
        return m_streams[name];
    }
    else
    {
        LOG(logERROR) << "No stream with this name: " << name;
    }
    return NULL;
}

} // end of smkit

