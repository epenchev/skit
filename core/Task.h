//
// Task.h
// Copyright (C) 2014  Emil Penchev, Bulgaria

#ifndef TASK_H_
#define TASK_H_

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <queue>
#include <map>
#include <vector>

#define REF(obj) boost::ref(obj)    // reference
#define CONST_REF(obj) boost::cref(obj)  // const reference

typedef boost::function<void()> Runnable;

enum TaskPriority { PriorityNormal = 0, PriorityHigh };

class Task
{
public:
	Task(const Task& task);

	template <class Func>
	static inline Task Connect(Func f)
	{
		return Task( boost::bind(f), PriorityNormal );
	}

    template <class Func, class T1>
    static inline Task Connect(Func f, T1 p1)
    {
        return Task( boost::bind(f, p1), PriorityNormal );
    }

    template <class Func, class T1, class T2>
    static inline Task Connect(Func f, T1 p1, T2 p2)
    {
        return Task( boost::bind(f, p1, p2, PriorityNormal) );
    }

    template <class Func, class T1, class T2, class T3>
    static inline Task Connect(Func f, T1 p1, T2 p2, T3 p3)
    {
        return Task( boost::bind(f, p1, p2, p3, PriorityNormal) );
    }

    template <class Func, class T1, class T2, class T3, class T4>
    static inline Task Connect(Func f, T1 p1, T2 p2, T3 p3, T4 p4)
    {
        return Task( boost::bind(f, p1, p2, p3, p4, PriorityNormal) );
    }
    
    template <class Func, class T1, class T2, class T3, class T4, class T5>
    static inline Task Connect(Func f, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5)
    {
        return Task( boost::bind(f, p1, p2, p3, p4, p5) );
    }

    // Run the task
    void Execute();

    // std::priority_queue compare
    friend bool operator < (const Task& a, const Task& b) { return a._priority < b._priority; }
    
protected:
    Runnable     _runObject;     /**< boost functor object */
    TaskPriority _priority;  /**< task priority */

    Task(Runnable func, TaskPriority prio);
};

class TimerTask : public Task
{
public:
	TimerTask(const Task& task, int id);
	void operator()( const boost::system::error_code& error ); // timer handler
private:
	int m_timerID;
};

typedef boost::thread::id ThreadID;
typedef boost::shared_ptr<boost::thread> ThreadPtr;

class TaskScheduler : boost::noncopyable
{
    TaskScheduler();

public:
    ~TaskScheduler();

    // public API
    void Run(unsigned threads);
    void QueueTask(Task task, ThreadID id = boost::this_thread::get_id());
    int  QueueTimer(Task task, int milisec, ThreadID tid = boost::this_thread::get_id());
    // cancel/stop the timer
    void ClearTimer(int id);
    static TaskScheduler& Instance();
    // Get next available thread (round robin scheduling)
    ThreadID GetNextThread();
    // Get io_service object associated with ThreadID
    boost::asio::io_service& GetThreadIOService(ThreadID id);

private:
    
    class TaskQueue
    {
    public:
        
        struct QueueWrapper
        {
            std::priority_queue<Task> priorityQueue;
            boost::mutex lock;
        }; 

        // io_service post handler
        void RunTasks();
        void Push(Task task);
        void Load();
        void RunQueue(QueueWrapper& q);
        boost::asio::io_service& GetIOService() { return m_io_service; }

    private:
        QueueWrapper m_primaryQueue;
        QueueWrapper m_secondaryQueue;
        boost::asio::io_service m_io_service;
    };
    
    typedef boost::shared_ptr<TaskQueue> TaskQueuePtr;
    typedef boost::shared_ptr<boost::asio::deadline_timer> TimerPtr;

    std::vector<ThreadPtr> m_threads;
    std::map<ThreadID, TaskQueuePtr> m_taskQueues;
    boost::mutex m_timersLock;
    std::map<int, TimerPtr> m_timers;

    TaskQueuePtr LookupQueue(ThreadID id);
};

class TaskSchedulerException : public std::exception
{
public:
	TaskSchedulerException(const std::string& message) { m_msg = message; }
    ~TaskSchedulerException() throw() {}
    virtual const char* what() throw() { return m_msg.c_str(); }
protected:
    std::string m_msg;
};

#endif // TASK_H_


