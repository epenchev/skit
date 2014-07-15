//
// ServerController.cpp
// Copyright (C) 2014  Emil Penchev, Bulgaria

#include "Logger.h"
#include "ServerController.h"

using namespace Skit;

void ServerController::OnAccept(TcpSocket* socket, ErrorCode& inError)
{
    if (!inError)
    {
    	TcpSocketPtr sockptr(socket);
    	ServerHandler* handler = m_servers[socket->GetThreadID()][socket->GetLocalPort()];
    	handler->AcceptConnection(sockptr);
    }
    else
    {
    	LOG(logWARNING) << inError.Message();
    }
}

void ServerController::Load(const std::string& filename)
{
	try
	{
		bool httpActive = false;
		m_globalConfig.Init(filename);

		// create socket acceptors
		for ( Skit::PropertyTree::Iterator it = m_globalConfig.Begin("servers"); it != m_globalConfig.End("servers"); ++it )
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
	catch(std::exception& ex)
	{
		LOG(logERROR) << "Exception from PropertyTree " << ex.what();
		return;
	}

	try
	{
		int threadCount = m_globalConfig.GetData<int>("threads");
		Skit::TaskScheduler::Instance().Run(threadCount);

		for (std::list<SocketAcceptor*>::iterator it = m_acceptors.begin(); it != m_acceptors.end(); it++)
		{
			(*it)->Listen();
		}

		for (int i = 0; i < threadCount; i++)
		{
			Skit::ThreadID id = Skit::TaskScheduler::Instance().GetNextThread();
			for ( Skit::PropertyTree::Iterator it = m_globalConfig.Begin("servers"); it != m_globalConfig.End("servers"); ++it )
			{
				std::string handlerName = it.GetData("name");
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
	catch (Skit::TaskSchedulerException& ex)
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



