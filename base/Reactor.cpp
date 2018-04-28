#include "Reactor.h"

#include <assert.h>
#include <sys/eventfd.h>
#include <signal.h>
#include "SocketOpts.h"
#include "common.h"
#include "GlogWrapper.h"

using namespace ws::base;

//haomiao
const int csPollTimeoutMs = 1000;

namespace
{



/**
*  忽略读写错误信号
*/
class IgnoreSigPipe
{
public:
	IgnoreSigPipe()
	{
		::signal(SIGPIPE, SIG_IGN);
	}
};


IgnoreSigPipe ginitApp;


}//namespace


Reactor::Reactor()
	: bquit_(false),
	  threadId_(CurrentThread::tid()),
	  btaskRunning_(false)
{
  poller_ = Poller::createPoller();
 
    taskWakeupFd_ = createEventfd();
	taskHandler_ = new EventHandler(this, taskWakeupFd_);
	taskHandler_->enableRead();
	taskHandler_->setReadCallback(std::bind(&Reactor::taskWakeupRead, this, std::placeholders::_1));

	timersList_.push_back(new TimerQueue(this));
}


Reactor::~Reactor()
{
	DLOG(INFO) << "Reactor::~Reactor...";
	//1.
	std::vector<TimerQueue*>::iterator it;
	for (it = timersList_.begin(); it != timersList_.end();)
	{
		delete (*it);
		(*it) = NULL;
		it = timersList_.erase(it);
	}
	//2.
	functorList_.clear();
	//3.
	close(taskWakeupFd_);
	delete taskHandler_;
	taskHandler_ = NULL;
	//4.
	delete poller_;
	poller_ = NULL;
}


/**
* @brief    事件监听线程
*  阻塞调用线程 
*/
void Reactor::loop()
{
	while (!bquit_)
	{
		activeList_.clear();
		if (poller_ != NULL)
		{
			poller_->poll(csPollTimeoutMs, activeList_);
		}

		//fortest
		//DLOG(INFO) << "Reactor|loop activelist size=" << activeList_.size() << " Reactor=" << this;
		//sleep(3);

		//触发事件处理
		for (EventList::iterator it = activeList_.begin();
		     it != activeList_.end(); it++)
		{
			(*it)->handEvent();
		}

		//DLOG(INFO) << "Reactor|loop end handEvent Reactor=" << this;

		//执行触发任务
		runningTask();

		//DLOG(INFO) << "Reactor|loop loop=0x" << this << "eventlist=" << activeList_.size();
	}

	LOG(WARNING) << "Reactor|loop exit...";
	
}


/**
* @brief    退出监听线程
*/
void Reactor::quit()
{
	DLOG(WARNING) << "Reactor|quit";

	bquit_ = true;
	eventfdWrite(taskWakeupFd_);
}


/**
* @brief    执行线程任务
* @param[in] TaskFunctor：任务函数
*/
void Reactor::runInLoop(const TaskFunctor& func)
{
	//如果是在同一线程内，则直接执行
	if (isInLoopThread())
	{
		DLOG(INFO) << "in same reactor loop" << this;
		func();
	}
	else
	{
		queueInLoop(func);
	}
}


/**
* @brief    添加触发任务到线程队列
* @param[in] TaskFunctor：任务执行函数
*/
void Reactor::queueInLoop(const TaskFunctor& func)
{
	{
		LockGuard lock(&mutex_);
		functorList_.push_back(func);
	}

	if (!isInLoopThread())
	{
		DLOG(INFO) << "Reactor::queueInLoop:not same loop thread,task size=" << functorList_.size() << " Reactor=" << this;
		eventfdWrite(taskWakeupFd_);
	}

	if (btaskRunning_)
	{
		DLOG(INFO) << "btaskRunning_=true";
		eventfdWrite(taskWakeupFd_);
	}

}


/**
* @brief    task触发读回调函数
* @param[in] Timestamp：taskfd触发读操作
*/
void Reactor::taskWakeupRead(Timestamp time)
{
  eventfdRead(taskWakeupFd_);
}


/**
* @brief    添加触发任务到线程队列
* @param[in] TaskFunctor：任务执行函数
*/
void Reactor::runningTask()
{
	//fortest
	//DLOG(INFO) << "Reactor|runningTask size=" << functorList_.size() << " Reactor=" << this;
	//sleep(1);

	if (functorList_.empty())
	{
		return;
	}

	btaskRunning_ = true;
	std::vector<TaskFunctor> functor;

	{
		LockGuard lock(&mutex_);
		functor.swap(functorList_);
	}

	for (int i = 0; i < functor.size(); i++)
	{
		functor[i]();
	}

	btaskRunning_ = false;
	DLOG(INFO) << "***Reactor::runningTask end " << " Reactor=" << this;
}


void Reactor::registerEvHandler(EventHandler* handler)
{
	poller_->registerEvHandler(handler);
}

void Reactor::removeEvHandler(EventHandler* handler)
{
  poller_->removeEvHandler(handler);
}

TimerId Reactor::runAt(Timestamp& time, const TimerCallback& cb)
{
  return timersList_[0]->addTimer(time, cb);
}

TimerId Reactor::runAt(Timer* tm)
{
	DLOG(INFO) << "##Reactor::runAt" << tm->expiration().formatString();
  return timersList_[0]->addTimer(tm);
}


/**
* @brief    延迟delay秒执行函数
* @param[in] delay：延迟秒数
* @param[in] cb：定时执行回调函数
* @return TimerId
* @retval  
*/
TimerId Reactor::runDelay(double delay, const TimerCallback& cb)
{
  DLOG(INFO) <<"Reactor::runDelay:" <<delay;

  Timestamp now = Timestamp::now();
  Timestamp tm(addSeconds(now, delay));
  return runAt(tm, cb);
}


///间隔一段时间循环执行
TimerId Reactor::runEvery(double interval, const TimerCallback& cb)
{
	Timestamp now = Timestamp::now();
	Timestamp tm(addSeconds(now, interval));
	return timersList_[0]->addTimer(tm, cb, interval, true);
}


void Reactor::cancelTimer(TimerId& id)
{
	//TODO:此处默认只有一个定时器队列
	timersList_[0]->delTimer(id);
}