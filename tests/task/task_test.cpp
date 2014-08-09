#include <Task.h>
#include <Logger.h>

// forward
static void handle_timer();

static void sample_handler()
{
	static int timerid = 0;
	TaskScheduler* scheduler = &TaskScheduler::Instance();

	bool cancelTimer = ((timerid % 2) == 0);
	if (cancelTimer)
	{
	    LOG(logINFO) << "Will cancel timer with id: " << timerid;
		scheduler->ClearTimer(timerid);
	}

	LOG(logINFO) <<  "sample_handler";
	timerid = scheduler->QueueTimer( Task::Connect(handle_timer), 5000 );
	if (!timerid)
	{
		LOG(logWARNING) << "Invalid timer ID";
	}
}

static void handle_timer()
{
	LOG(logINFO) << "handle_timer";
	TaskScheduler* scheduler = &TaskScheduler::Instance();
	scheduler->QueueTask( Task::Connect(sample_handler) );
}

class Sample
{
public:
	void Func() { LOG(logINFO) << "Sample::Func() executed"; }
};

int main()
{
    Sample sampleObj;
	TaskScheduler* scheduler = &TaskScheduler::Instance();

	try
	{
		scheduler->Run(10);
	}
	catch ( TaskSchedulerException& ex )
	{
		LOG(logERROR) << "Exception caught in scheduler->Run() :" << ex.what();
		return 0;
	}

	try
	{
	    ThreadID id = scheduler->GetNextThread();
	    LOG(logINFO) << "QueueTask on thread: " << id;

		scheduler->QueueTask( Task::Connect( &Sample::Func, REF(sampleObj) ), id );
		scheduler->QueueTask( Task::Connect( &Sample::Func, REF(sampleObj) ), id );
		
		int timerid = scheduler->QueueTimer( Task::Connect(handle_timer), 5000, scheduler->GetNextThread() );
		if (!timerid)
		{
			LOG(logERROR) << "Invalid timer ID from scheduler->QueueTimer() ";
		}

		id = scheduler->GetNextThread();
		LOG(logINFO) << "QueueTask on thread: " << id;
		scheduler->QueueTask( Task::Connect(sample_handler), id );
	}
	catch ( TaskSchedulerException& ex )
	{
		LOG(logERROR) << "Exception caught from scheduler->GetNextThread() :" << ex.what();
	}

	while (1)
    {
		int timerid = 0;
    	boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
        try
        {
        	LOG(logINFO) << "QueueTimer";
        	timerid = scheduler->QueueTimer( Task::Connect(handle_timer), 5000, scheduler->GetNextThread() );
        	if (!timerid)
        	{
        		LOG(logERROR) << "Invalid timer ID from scheduler->QueueTimer() ";
        	}
        }
        catch( TaskSchedulerException& ex )
        {
        	LOG(logERROR) << "Exception caught from scheduler->GetNextThread() :" << ex.what();
        }
        boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));

        bool cancelTimer = ((timerid % 2) == 0);
        if (cancelTimer)
        {
            LOG(logINFO) << "Will cancel timer with id: " << timerid;
        	scheduler->ClearTimer(timerid);
        }

        try
        {
        	ThreadID id = scheduler->GetNextThread();
 	        LOG(logINFO) << "QueueTask on thread: " << id;
        	scheduler->QueueTask( Task::Connect(sample_handler), id );
        	        	
        	id = scheduler->GetNextThread();
        	LOG(logINFO) << "QueueTask on thread: " << id;
        	scheduler->QueueTask( Task::Connect( &Sample::Func, REF(sampleObj) ), id );
        	
        }
        catch( TaskSchedulerException& ex )
        {
        	LOG(logERROR) << "Exception caught from scheduler->GetNextThread() :" << ex.what();
        }
    }

    return 0;
}

