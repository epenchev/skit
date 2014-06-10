#include <Task.h>
#include <Logger.h>

// forward
static void handle_timer();

static void sample_handler()
{
	static int timerid = 0;
	Skit::TaskScheduler* sheduler = &Skit::TaskScheduler::Instance();

	bool cancelTimer = ((timerid % 2) == 0);
	if (cancelTimer)
	{
	    LOG(logINFO) << "Will cancel timer with id: " << timerid;
		sheduler->ClearTimer(timerid);
	}

	LOG(logINFO) <<  "sample_handler";
	timerid = sheduler->QueueTimer( Skit::Task::Connect(handle_timer), 5000 );
	if (!timerid)
	{
		LOG(logWARNING) << "Invalid timer ID";
	}
}

static void handle_timer()
{
	LOG(logINFO) << "handle_timer";
	Skit::TaskScheduler* sheduler = &Skit::TaskScheduler::Instance();
	sheduler->QueueTask( Skit::Task::Connect(sample_handler) );
}

class Sample
{
public:
	void Func() { LOG(logINFO) << "Sample::Func() executed"; }
};

int main()
{
    Sample sampleObj;
	Skit::TaskScheduler* sheduler = &Skit::TaskScheduler::Instance();

	try
	{
		sheduler->Run(10);
	}
	catch(Skit::TaskSchedulerException& ex)
	{
		LOG(logERROR) << "Exception caught in sheduler->Run() :" << ex.what();
		return 0;
	}

	try
	{
	    Skit::ThreadID id = sheduler->GetNextThread();
	    LOG(logINFO) << "QueueTask on thread: " << id;
		//sheduler->QueueTask( Skit::Task::Connect(&Sample::Func, Ref(sampleObj)), id );
		
		Skit::Runnable r = BindRunnable(&Sample::Func, Ref(sampleObj));
		sheduler->QueueTask( Skit::Task::ConnectNew(BindRunnable(&Sample::Func, Ref(sampleObj))), id );
		
		int timerid = sheduler->QueueTimer( Skit::Task::Connect(handle_timer), 5000, sheduler->GetNextThread() );
		if (!timerid)
		{
			LOG(logERROR) << "Invalid timer ID from sheduler->QueueTimer() ";
		}
		id = sheduler->GetNextThread();
		LOG(logINFO) << "QueueTask on thread: " << id;
		sheduler->QueueTask( Skit::Task::Connect(sample_handler), id );
	}
	catch(Skit::TaskSchedulerException& ex)
	{
		LOG(logERROR) << "Exception caught from sheduler->GetNextThread() :" << ex.what();
	}

	while (1)
    {
		int timerid = 0;
    	boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
        try
        {
        	LOG(logINFO) << "QueueTimer";
        	timerid = sheduler->QueueTimer( Skit::Task::Connect(handle_timer), 5000, sheduler->GetNextThread() );
        	if (!timerid)
        	{
        		LOG(logERROR) << "Invalid timer ID from sheduler->QueueTimer() ";
        	}
        }
        catch(Skit::TaskSchedulerException& ex)
        {
        	LOG(logERROR) << "Exception caught from sheduler->GetNextThread() :" << ex.what();
        }
        boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));

        bool cancelTimer = ((timerid % 2) == 0);
        if (cancelTimer)
        {
            LOG(logINFO) << "Will cancel timer with id: " << timerid;
        	sheduler->ClearTimer(timerid);
        }

        try
        {
        	Skit::ThreadID id = sheduler->GetNextThread();
 	        LOG(logINFO) << "QueueTask on thread: " << id;
        	sheduler->QueueTask( Skit::Task::Connect(sample_handler), id );
        	        	
        	id = sheduler->GetNextThread();        	
        	LOG(logINFO) << "QueueTask on thread: " << id;
        	sheduler->QueueTask( Skit::Task::Connect(&Sample::Func, Ref(sampleObj)), id );
        	
        }
        catch(Skit::TaskSchedulerException& ex)
        {
        	LOG(logERROR) << "Exception caught from sheduler->GetNextThread() :" << ex.what();
        }
    }

    return 0;
}

