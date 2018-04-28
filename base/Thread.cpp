#include "Thread.h"

#include <stdlib.h>
#include <stdio.h>
#include "Timestamp.h"
#include <sys/syscall.h>
#include <sys/types.h>
#include <assert.h>
#include <sys/prctl.h>

using namespace ws;
using namespace ws::base;

///
namespace ws
{
namespace CurrentThread
{
	__thread int t_cachedTid = 0;
	__thread char t_tidString[32];
	__thread int t_tidStringLength = 6;
  __thread const char* t_threadName = "unknown";



	pid_t gettid()
	{
	  //return static_cast<pid_t>(::syscall(SYS_gettid));
          return static_cast<pid_t>(::syscall(__NR_gettid));	
        }

	void cacheTid()
	{
		if (t_cachedTid == 0)
		{
			t_cachedTid = gettid();
			t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
		}
	}

	/**
	* @brief    获取当前线程ID
	*/
	int tid()
	{
		if (__builtin_expect(t_cachedTid == 0, 0))
		{
			cacheTid();
		}
		return t_cachedTid;
	}


	/**
	* @brief    精度较高sleep
	* @param[in] usec：微妙
	*/
	void sleepUsec(int64_t usec)
	{
		struct timespec ts = { 0, 0 };
		ts.tv_sec = static_cast<time_t>(usec / Timestamp::csMicrosecondsPerSecond);
		ts.tv_nsec = static_cast<long>(usec % Timestamp::csMicrosecondsPerSecond * 1000);
		::nanosleep(&ts, NULL);
	}
}

}//namespace ws


struct ThreadData
{
	ThreadFunc func_;
	std::string name_;
	pid_t* threadTid_;

	ThreadData(const ThreadFunc& func,
		const std::string& name,
		pid_t* tid)
		:func_(func),
		name_(name),
		threadTid_(tid)
	{}


	/**
	* @brief    线程执行函数
	* @param[in] context：线程执行上下文
	* @return void*
	* @retval
	*/
	void runInThread()
	{
		if (threadTid_ != nullptr)
		{
			*threadTid_ = CurrentThread::gettid();
		}

		CurrentThread::t_threadName = name_.c_str();

		::prctl(PR_SET_NAME, CurrentThread::t_threadName);

		try
		{
			func_();
			CurrentThread::t_threadName = "finished";
		}
		catch (const std::exception& ex)
		{
			CurrentThread::t_threadName = "crashed";
			fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
			fprintf(stderr, "reason: %s\n", ex.what());
			abort();
		}
		catch (...)
		{
			CurrentThread::t_threadName = "crashed";
			fprintf(stderr, "unknown exception caught in Thread %s\n", name_.c_str());
			throw; // rethrow
		}
	}

};

Thread::Thread(const ThreadFunc& func, const std::string name)
	:bstarted_(false),
	bjoined_(false),
	pthreadId_(0),
	tid_(new pid_t(0)),
	func_(func),
	name_(name)
{
	setDefaultName();
}


Thread::~Thread()
{
	if (bstarted_ && !bjoined_)
	{
		pthread_detach(pthreadId_);
	}
}


void Thread::setDefaultName()
{
	int num = 0;
	if (name_.empty())
	{
		char buf[32];
		snprintf(buf, sizeof buf, "Thread%d", num);
		name_ = buf;
	}

}


/**
* @brief    启动线程
*/
void Thread::start()
{
	assert(!bstarted_);
	bstarted_ = true;

	ThreadData* data = new ThreadData(func_, name_, tid_);
	if (pthread_create(&pthreadId_, NULL, &Thread::ThreadProc, data) != 0)
	{
	    printf("***error");
		delete data;
		data = nullptr;
		bstarted_ = false;
	}
}


/**
* @brief    阻塞等待线程结束
*/
int Thread::join()
{
	assert(bstarted_);
	assert(!bjoined_);

	bjoined_ = true;
	return pthread_join(pthreadId_, NULL);
}


/**
* @brief    线程执行函数
*/
void* Thread::ThreadProc(void* context)
{
	ThreadData* data = static_cast<ThreadData*>(context);
	data->runInThread();

	delete data;
	return NULL;
}
