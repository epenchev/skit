#include <task.h>
#include <Logger.h>

// forward
static void handle_timer();

static void sample_handler()
{
	static int timer_id = 0;
	smkit::task_scheduler* scheduler = &smkit::task_scheduler::instance();

	bool cancelTimer = ((timer_id % 2) == 0);
	if (cancelTimer)
	{
	    LOG(logINFO) << "Will cancel timer with id: " << timer_id;
		scheduler->clear_timer(timer_id);
	}

	LOG(logINFO) <<  "sample_handler";
	timer_id = scheduler->queue_timer(smkit::task::connect(handle_timer), 5000);
	if (!timer_id)
	{
		LOG(logWARNING) << "Invalid timer ID";
	}
}

static void handle_timer()
{
	LOG(logINFO) << "handle_timer";
	smkit::task_scheduler* scheduler = &smkit::task_scheduler::instance();
	scheduler->queue_task(smkit::task::connect(sample_handler));
}

class sample
{
public:
	void func() { LOG(logINFO) << "sample::func() executed"; }
};

int main()
{
    sample sample_obj;
    smkit::task_scheduler* scheduler = &smkit::task_scheduler::instance();
	try
	{
		scheduler->run(10);
	}
	catch (smkit::task_scheduler_error& err)
	{
		LOG(logERROR) << "Exception caught in scheduler->run() :" << err.what();
		return 0;
	}

	try
	{
	    smkit::task_scheduler::thread_id_t id = scheduler->next_thread();
	    LOG(logINFO) << "queue_task() on thread: " << id;

		scheduler->queue_task(smkit::task::connect(&sample::func, boost::ref(sample_obj)), id);
		scheduler->queue_task(smkit::task::connect(&sample::func, boost::ref(sample_obj)), id);
		int timer_id = scheduler->queue_timer(smkit::task::connect(handle_timer), 5000, scheduler->next_thread() );
		if (!timer_id)
		{
			LOG(logERROR) << "Invalid timer ID from scheduler->queue_timer ";
		}

		id = scheduler->next_thread();
		LOG(logINFO) << "QueueTask on thread: " << id;
		scheduler->queue_task(smkit::task::connect(sample_handler), id);
	}
	catch (smkit::task_scheduler_error& ex)
	{
		LOG(logERROR) << "Exception caught from scheduler->next_thread() :" << ex.what();
	}

	while (1)
    {
		int timer_id = 0;
    	boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
        try
        {
        	LOG(logINFO) << "QueueTimer";
        	timer_id = scheduler->queue_timer(smkit::task::connect(handle_timer), 5000, scheduler->next_thread() );
        	if (!timer_id)
        	{
        		LOG(logERROR) << "Invalid timer ID from scheduler->queue_timer() ";
        	}
        }
        catch(smkit::task_scheduler_error& ex)
        {
        	LOG(logERROR) << "Exception caught from scheduler->next_thread() :" << ex.what();
        }
        boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));

        bool cancelTimer = ((timer_id % 2) == 0);
        if (cancelTimer)
        {
            LOG(logINFO) << "Will cancel timer with id: " << timer_id;
        	scheduler->clear_timer(timer_id);
        }

        try
        {
            smkit::task_scheduler::thread_id_t tid = scheduler->next_thread();
 	        LOG(logINFO) << "QueueTask on thread: " << tid;
        	scheduler->queue_task(smkit::task::connect(sample_handler), tid);
        	        	
        	tid = scheduler->next_thread();
        	LOG(logINFO) << "QueueTask on thread: " << tid;
        	scheduler->queue_task(smkit::task::connect(&sample::func, boost::ref(sample_obj)), tid);
        	
        }
        catch(smkit::task_scheduler_error& ex)
        {
        	LOG(logERROR) << "Exception caught from scheduler->GetNextThread() :" << ex.what();
        }
    }

    return 0;
}

