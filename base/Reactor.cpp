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
*  ���Զ�д�����ź�
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
* @brief    �¼������߳�
*  ���������߳� 
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

		//�����¼�����
		for (EventList::iterator it = activeList_.begin();
		     it != activeList_.end(); it++)
		{
			(*it)->handEvent();
		}

		//DLOG(INFO) << "Reactor|loop end handEvent Reactor=" << this;

		//ִ�д�������
		runningTask();

		//DLOG(INFO) << "Reactor|loop loop=0x" << this << "eventlist=" << activeList_.size();
	}

	LOG(WARNING) << "Reactor|loop exit...";
	
}


/**
* @brief    �˳������߳�
*/
void Reactor::quit()
{
	DLOG(WARNING) << "Reactor|quit";

	bquit_ = true;
	eventfdWrite(taskWakeupFd_);
}


/**
* @brief    ִ���߳�����
* @param[in] TaskFunctor��������
*/
void Reactor::runInLoop(const TaskFunctor& func)
{
	//�������ͬһ�߳��ڣ���ֱ��ִ��
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
* @brief    ��Ӵ��������̶߳���
* @param[in] TaskFunctor������ִ�к���
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
* @brief    task�������ص�����
* @param[in] Timestamp��taskfd����������
*/
void Reactor::taskWakeupRead(Timestamp time)
{
  eventfdRead(taskWakeupFd_);
}


/**
* @brief    ��Ӵ��������̶߳���
* @param[in] TaskFunctor������ִ�к���
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
* @brief    �ӳ�delay��ִ�к���
* @param[in] delay���ӳ�����
* @param[in] cb����ʱִ�лص�����
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


///���һ��ʱ��ѭ��ִ��
TimerId Reactor::runEvery(double interval, const TimerCallback& cb)
{
	Timestamp now = Timestamp::now();
	Timestamp tm(addSeconds(now, interval));
	return timersList_[0]->addTimer(tm, cb, interval, true);
}


void Reactor::cancelTimer(TimerId& id)
{
	//TODO:�˴�Ĭ��ֻ��һ����ʱ������
	timersList_[0]->delTimer(id);
}