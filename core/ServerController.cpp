//
// ServerController.cpp
// Copyright (C) 2014  Emil Penchev, Bulgaria

#include "Logger.h"
#include "ServerController.h"

ServerController::ServerController()
{}

void ServerController::OnAccept(TcpSocket* socket, ErrorCode& err)
{
    if (err)
    {
        LOG(logWARNING) << err.Message();
        return;
    }
    ServerHandler* handler = fServerHandlers[socket->GetThreadID()][socket->GetLocalPort()];
    if (handler)
    {
        TaskScheduler* scheduler = &TaskScheduler::Instance();
    	scheduler->QueueTask(Task::Connect(&ServerHandler::AcceptConnection, handler, socket), handler->GetThreadId());
    }
    else
    {
        LOG(logERROR) << "Missing server handler for thread: " << socket->GetThreadID()
    	              << " and port: " << socket->GetLocalPort();
    	delete socket;
    }
}

void ServerController::Load(const string& filename)
{
    int                    threadCount;
    PropertyTree::Iterator it;
    PropertyTree::Iterator itEnd;
    map<string, unsigned>  handlersConfig;

    //Log::ReportingLevel() = logERROR;

    try
	{
        fGlobalConfigTree.Init(filename);
        threadCount = fGlobalConfigTree.GetData<int>("threads");
	}
    catch( exception& ex )
    {
        LOG(logERROR) << "Error reading configuration file " << filename << " : " << ex.what();
        return;
    }

    try
    {
        it = fGlobalConfigTree.Begin("servers");
        itEnd = fGlobalConfigTree.End("servers");
    }
    catch( exception& ex )
    {
        LOG(logERROR) << "Invalid section 'servers' " << " : " << ex.what();
        return;
    }

	while ( it != itEnd )
	{
	    try
		{
		    string serverName = it.GetData("name");
		    unsigned serverPort = it.GetData<unsigned>("listen");
		    it.operator ++();

		    /* Server name and server port are mandatory
		    * for a complete server handler configuration.
		    */

		    if (!serverName.empty() && serverPort)
		        handlersConfig[serverName] = serverPort;
		    else
		        LOG(logWARNING) << "Incomplete server configuration, empty 'name' or 'listen' section";
		}
		catch( exception& ex )
		{
		    LOG(logERROR) << "Missing 'name' or 'listen' section in server object : " << ex.what();
		    return;
		}
	}

	/* Check for HTTP server configuration */
	if (!handlersConfig.count("http"))
	{
	    LOG(logERROR) << "No server HTTP configuration present in " << filename;
	    return;
	}

	if (handlersConfig.empty())
	{
	    LOG(logERROR) << "No server configuration present in " << filename;
	    return;
	}

	/*
	* So far so good basic configuration is complete,
	* continue loading the server.
	*/

	try
	{
	    TaskScheduler::Instance().Run(threadCount);

	    map<string, unsigned>::iterator it = handlersConfig.begin();
	    for ( ; it != handlersConfig.end(); it++ )
	    {
	        SocketAcceptor* acceptor = new SocketAcceptor( it->second, this, fIOService );
	        fAcceptors.push_back(acceptor);
	        acceptor->Listen();
	    }
	}
	catch (std::bad_alloc& ex)
	{
	    LOG(logERROR) << "Error allocating memory :" << ex.what();
	}
	catch ( TaskSchedulerException& ex )
	{
	    LOG(logERROR) << "Exception caught from scheduler->Run() :" << ex.what();
	    return;
	}

	/* Associate each server handler to a thread */
	for (int i = 0; i < threadCount; i++)
	{
	    map<string, unsigned>::iterator it = handlersConfig.begin();
	    for ( ; it != handlersConfig.end(); it++ )
	    {
	        ServerHandler* handler = HandlerFactory::CreateInstance(it->first);
	        if (!handler)
	        {
	            LOG(logWARNING) << "No registered server handler with this name : " << it->first;
	            continue;
	        }

	        handler->SetThread( TaskScheduler::Instance().GetNextThread() );
	        fServerHandlers[handler->GetThreadId()][it->second] = handler;
	    }
	}

	LoadStreams();

	// run boost loop
	fIOService.run();
}

void ServerController::LoadStreams()
{
    PropertyTree::Iterator iter;
    PropertyTree::Iterator iterEnd;

    try
    {
        iter = fGlobalConfigTree.Begin("streams");
        iterEnd = fGlobalConfigTree.End("streams");
    }
    catch( exception& ex )
    {
        LOG(logDEBUG) << "No streams configured " << " : " << ex.what();
        return;
    }

    while ( iter != iterEnd )
    {
        try
        {
            string name = iter.GetData("name");
            string location = iter.GetData("location");

            if (!name.empty() && !location.empty())
            {
                Stream* theStream = new Stream(iter);
                fStreams[name] = theStream;
                //theStream->Start();
            }
            else {
                LOG(logWARNING) << "Incomplete stream configuration, empty 'name' or 'location' section";
            }

            iter.operator ++();
        }
        catch( exception& ex )
        {
            LOG(logERROR) << "Missing 'name' or 'location' section in stream object : " << ex.what();
            return;
        }
    }
}

Stream* ServerController::GetStream(string name)
{
    LOG(logDEBUG) << "stream name to search is :" << name;
    if (fStreams.count(name) > 0)
    {
        LOG(logDEBUG) << "Got stream with :" << name;
        return fStreams[name];
    }
    else {
        LOG(logERROR) << "No stream with this name: " << name;
    }

    return NULL;
}

