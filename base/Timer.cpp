#include "Timer.h"
#include "Thread.h"

#include "GlogWrapper.h"

using namespace ws::base;

int64_t Timer::sIndexNum = 0;
MutexLock Timer::mutex_;


#define  _RUN_IN_OTHER_THREAD


/**
* @brief    定时器执行函数体
*/
void Timer::run()
{
#ifdef _RUN_IN_OTHER_THREAD
  Thread th(std::bind(&Timer::asyncRunProc, this));
  th.start();
#else
	if (timerCallback_ != nullptr)
	  {
		timerCallback_();
	}
#endif
	
}


void Timer::asyncRunProc()
{
  //DLOG(INFO)<< "##timerCallback_ running";

	if (timerCallback_ != nullptr)
	{
		timerCallback_();
	}
}
