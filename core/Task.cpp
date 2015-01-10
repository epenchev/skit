#include "Task.h"
#include "Logger.h"
#include "IDGenerator.h"

namespace smkit
{
void TaskScheduler::TaskQueue::Load()
{
    boost::asio::io_service::work work(m_io_service);
    m_io_service.run();
}
void TaskScheduler::TaskQueue::RunQueue(TaskQueue::QueueWrapper& queue)
{
    boost::unique_lock<boost::mutex> lock(queue.lock);
    while (!queue.priorityQueue.empty())
    {
        Task task = queue.priorityQueue.top();
        task.Execute();
        queue.priorityQueue.pop();
    }
}

void TaskScheduler::TaskQueue::Push(Task task)
{  
    if ( m_primaryQueue.lock.try_lock() )
    {
        m_primaryQueue.priorityQueue.push(task);
        m_io_service.post(boost::bind(&TaskQueue::RunTasks, this));
        m_primaryQueue.lock.unlock();
    }
    else
    {
        boost::unique_lock<boost::mutex> lock(m_secondaryQueue.lock);
        m_secondaryQueue.priorityQueue.push(task);
    }
}

void TaskScheduler::TaskQueue::RunTasks()
{
    RunQueue(m_primaryQueue);
    RunQueue(m_secondaryQueue);
}

TaskScheduler::TaskScheduler()
{}

TaskScheduler::~TaskScheduler()
{}

void TaskScheduler::Run(unsigned threads)
{
	if (!threads)
    {
		throw TaskSchedulerException("invalid thread count");
	}

	if (m_taskQueues.empty()) // protect against multiple runs
	{
		for (unsigned i = 0; i < threads; i++)
	    {
            TaskQueuePtr queue(new TaskQueue());
            ThreadPtr thread(
                new boost::thread( 
                    boost::bind(&TaskQueue::Load, queue.get())
                )
            );
			m_taskQueues.insert(
                std::pair<ThreadID, TaskQueuePtr>(thread->get_id(), queue) 
            );
			m_threads.push_back(thread);
		}
	}
    else
    {
        LOG(logWARNING) << "TaskScheduler already executed";
    }
}

void TaskScheduler::QueueTask(Task task, ThreadID id)
{
    try
    {
        TaskQueuePtr taskQueue = LookupQueue(id);
        taskQueue->Push(task);
    }
    catch(TaskSchedulerException& ex)
    {
        LOG(logERROR) << "exception caught: " << ex.what();
    }
}

int TaskScheduler::QueueTimer(Task task, int milisec, ThreadID id)
{
    unsigned timerID = 0;
    boost::system::error_code err;

    if (milisec)
    {
        try
        {
            TaskQueuePtr queue = LookupQueue(id);
            TimerPtr timer(new boost::asio::deadline_timer(queue->GetIOService()));
            timer->expires_from_now(boost::posix_time::milliseconds(milisec), err);

             if (!err)
             {
            	 timerID = IDGenerator::Instance().Next();
                 timer->async_wait( TimerTask(task, timerID) );

                 boost::unique_lock<boost::mutex> lock(m_timersLock);
                 m_timers.insert(std::pair<int, TimerPtr>(timerID, timer));
             }
             else
             {
            	 LOG(logERROR) << " boost::deadline_timer : " << err.message();
             }
        }
        catch(TaskSchedulerException& ex)
        {
        	LOG(logERROR) << "exception from LookupQueue() : " << ex.what();
        }
    }
    
    return timerID;
}

void TaskScheduler::ClearTimer(int id)
{
    if (id)
    {
    	boost::unique_lock<boost::mutex> lock(m_timersLock);
    	
    	// for debug remove after finish
    	LOG(logDEBUG) << "Clearing timer with ID: " << id;
    	
    	std::map<int, TimerPtr>::iterator it = m_timers.find(id);
    	if (it != m_timers.end())
    	{
    		boost::system::error_code err;
    		m_timers.at(id)->cancel(err);
    		if (err)
            {
    			LOG(logERROR) << err.message();
            }
    		m_timers.erase(it);
    	}
    	else
        {
            LOG(logWARNING) << "No timer present with this ID: " << id;
        }
    }
    else
    {
        LOG(logWARNING) << "Invalid timer: " << id;
    }
}

TaskScheduler& TaskScheduler::Instance()
{
	static TaskScheduler s_scheduler;
    return s_scheduler;
}

TaskScheduler::TaskQueuePtr TaskScheduler::LookupQueue(ThreadID tid) // throws exception if none present
{
    if (m_taskQueues.empty())
    {
    	throw TaskSchedulerException("m_taskQueues error");
    }
    
    std::map<ThreadID, TaskQueuePtr>::iterator it = m_taskQueues.find(tid);
    if (it == m_taskQueues.end())
    {
    	throw TaskSchedulerException("Error no match for this thread");
    }

    return it->second;
}

ThreadID TaskScheduler::GetNextThread()
{
	ThreadID tid;
	static unsigned lastUsedThread = 0;

	unsigned threadCount = m_threads.size();
	if (!threadCount)
    {
        throw TaskSchedulerException("thread count error");
    }

	if (lastUsedThread >= threadCount)
    {
		lastUsedThread = 0;
    }

	tid = m_threads[lastUsedThread]->get_id();
	lastUsedThread++;

	return tid;
}

boost::asio::io_service& TaskScheduler::GetThreadIOService(ThreadID id)
{
    TaskQueuePtr queue = LookupQueue(id);
    return queue->GetIOService();
}

timer_task::timer_task(const task& t, int id)
 : task(t), m_timerID(id)
{}

void timer_task::operator()(const boost::system::error_code& error)
{
	if (!error)
		m_functor();

    TaskScheduler::Instance().ClearTimer(m_timerID);
}

task::task(boost::function<void()> func, task_priority prio)
 : m_functor(func), m_priority(prio)
{}

task::task(const task& t)
{
	this->m_functor = t.m_functor;
	this->m_priority = t.m_priority;
}

void task::execute()
{
	m_functor();
}
} // end of namespace smkit

