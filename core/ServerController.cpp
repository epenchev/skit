//
// ServerController.cpp
// Copyright (C) 2014  Emil Penchev, Bulgaria

#include "Logger.h"
#include "ServerController.h"

void ServerController::OnAccept(TcpSocket* socket, ErrorCode& err)
{
    if (!err && socket)
    {
    	ServerHandler* handler = m_servers[socket->GetThreadID()][socket->GetLocalPort()];
    	handler->AcceptConnection(socket);
    }
    else
    {
    	if (err)
    	{
    	    LOG(logWARNING) << err.Message();
    	}
    }
}

void ServerController::Load(const string& filename)
{
	try
	{
		bool httpActive = false;
		m_globalConfig.Init(filename);

		// create socket acceptors
		for ( Skit::PropertyTree::Iterator it = m_globalConfig.Begin("servers");
		      it != m_globalConfig.End("servers"); ++it )
		{
			if (it.GetData("name") == "http")
			{
				httpActive = true;
			}

			unsigned port = it.GetData<unsigned>("listen");
			SocketAcceptor* acceptor = new SocketAcceptor(port, this, m_io_service);
			m_acceptors.push_back(acceptor);
		}
		if (!httpActive)
		{
			LOG(logERROR) << "No HTTP server handler configuration";
		}
	}
	catch ( exception& ex )
	{
		LOG(logERROR) << "Exception from PropertyTree " << ex.what();
		return;
	}

	try
	{
		int threadCount = m_globalConfig.GetData<int>("threads");
		TaskScheduler::Instance().Run(threadCount);

		for ( list<SocketAcceptor*>::iterator it = m_acceptors.begin();
		      it != m_acceptors.end(); it++ )
		{
			(*it)->Listen();
		}

		for (int i = 0; i < threadCount; i++)
		{
			ThreadID id = TaskScheduler::Instance().GetNextThread();

			for ( Skit::PropertyTree::Iterator it = m_globalConfig.Begin("servers");
			      it != m_globalConfig.End("servers"); ++it )
			{
				string handlerName = it.GetData("name");
				ServerHandler* handler = HandlerFactory::CreateInstance(handlerName);
				if (!handler)
				{
					LOG(logWARNING) << "No registered server handler with this name : " << handlerName;
					continue;
				}

				handler->SetThread(id);
				m_servers[id][it.GetData<unsigned>("listen")] = handler;
			}
		}
	}
	catch ( TaskSchedulerException& ex )
	{
		LOG(logERROR) << "Exception caught from scheduler->Run() :" << ex.what();
		return;
	}
	catch (SystemException& ex)
	{
		LOG(logERROR) << ex.what();
		return;
	}

	// run the machine
	m_io_service.run();
}



