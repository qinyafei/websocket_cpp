#include "TimerQueue.h"

#include <iterator>
#include <sys/timerfd.h>
#include <string.h>

#include "Reactor.h"
#include "GlogWrapper.h"

using namespace ws::base;

/**
* @brief    创建timerfd
* @return int
* @retval  timerfd
*/
int createTimerfd()
{
	int fd = timerfd_create(CLOCK_REALTIME,
		TFD_NONBLOCK | TFD_CLOEXEC);
	if (fd < 0)
	{
		//handle_error("timerfd_create");
	}

	return fd;
}


/**
* @brief    时间戳距此刻的时间差
* @param[in] when：比较时间戳
* @return timespec
* @retval  时间差
*/
struct timespec howMuchTimeFromNow(Timestamp when)
{
  int64_t nowtm = Timestamp::now().microsecondsSinceEpoch();
	int64_t microseconds = when.microsecondsSinceEpoch() - nowtm;
	if (microseconds < 100)
	{
		microseconds = 100;
	}
	struct timespec ts;
	ts.tv_sec = static_cast<time_t>(
		microseconds / Timestamp::csMicrosecondsPerSecond);
	ts.tv_nsec = static_cast<long>(
		(microseconds % Timestamp::csMicrosecondsPerSecond) * 1000);
	return ts;
}


void readTimerfd(int timerfd)
{
	uint64_t howmany;
	unsigned int nlen = ::read(timerfd, &howmany, sizeof howmany);
	if (nlen != sizeof howmany)
	{
		//error
	}
}

/**
* @brief    重置timerfd定时
* @param[in] tm：时间戳
* @param[in] timerfd：
*/
void resetTimerFd(int timerfd, Timestamp& tm)
{
  static bool bsetup = false;
  if (!bsetup)
    {
      struct itimerspec setup_tm;
      memset(&setup_tm, 0, sizeof(itimerspec));

      setup_tm.it_value.tv_sec = tm.microsecondsSinceEpoch() / Timestamp::csMicrosecondsPerSecond;
      setup_tm.it_value.tv_nsec = 0;

      int ret = ::timerfd_settime(timerfd, TFD_TIMER_ABSTIME, &setup_tm, NULL);
      if (ret != 0)
	{
	  assert(0);
	}

      bsetup = true;
    }
  else
    {
      struct itimerspec newValue;
      struct itimerspec oldValue;
      memset(&newValue, 0, sizeof(itimerspec));
      memset(&oldValue, 0, sizeof(itimerspec));
      newValue.it_value = howMuchTimeFromNow(tm);

      int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
      if (ret != 0)
	{
	  assert(0);
	}
    }
}


void defaultTimerCallback()
{
  assert(0);
}



TimerQueue::TimerQueue(Reactor* loop)
  :ownerLoop_(loop),
   timerfd_(createTimerfd()),
   timerfdEv_(loop, timerfd_)
{
  DLOG(INFO) << "##TimerQueue|timerfd=" << timerfd_;

  timerfdEv_.setReadCallback(std::bind(&TimerQueue::eventRead, this, std::placeholders::_1));
  timerfdEv_.enableRead();

  //add MAX timer，便于处理只有一个定时器的特殊情况
  Timestamp maxTimestamp = Timestamp::now();
  maxTimestamp = addSeconds(maxTimestamp, 24*3600*365);
  std::function<void()> fn = std::bind(defaultTimerCallback);
  addTimer(maxTimestamp, std::bind(defaultTimerCallback));
}

TimerQueue::~TimerQueue(void)
{
	timerfdEv_.disableAll();
	timerfdEv_.remove();
	close(timerfd_);

	for (TimerList::iterator it = timerLists_.begin(); it != timerLists_.end(); it++)
	{
		delete it->second;
	}

	timerLists_.clear();
}


/**
* @brief    外部直接调用插入定时器
* @param[in] timerCallback：定时器回调函数
* @param[in] time:时间戳
* @return TimerId
* @retval  
*/
TimerId  TimerQueue::addTimer(Timestamp& time, TimerCallback timerCallback, const double interval, bool brepeat)
{
	Timer* tmer = new Timer(timerCallback, time, interval, brepeat);
	bool bupdateNextTimestamp = insert(tmer);
	if (bupdateNextTimestamp)
	{
		resetTimerFd(timerfd_, time);
	}

	return TimerId(tmer->index(), tmer);
}

/**
* @brief    外部直接调用，重载插入定时器
* @param[in] tm:定时器
* @return TimerId
* @retval
*/
TimerId TimerQueue::addTimer(Timer* tm)
{
  assert(tm != NULL);

  bool bupdateNextTimestamp = insert(tm);
  if (bupdateNextTimestamp)
    {
      Timestamp tms = tm->expiration();
      resetTimerFd(timerfd_, tms);
    }

  return TimerId(tm->index(), tm);
}


/**
* @brief    定时器到期，执行回调
* @param[in] evtime:到期时间戳
*/
void TimerQueue::eventRead(Timestamp evtime)
{
	DLOG(INFO) << "TimerQueue::eventRead happened:time " << timerLists_.begin()->first.formatString();
	DLOG(INFO) << "TimerQueue::eventRead happened:now " << evtime.formatString();

	for (TimerList::iterator it = timerLists_.begin(); it != timerLists_.end(); it++)
	{
		DLOG(INFO) << "###timer(n):" << it->first.formatString();
	}

	readTimerfd(timerfd_);
	std::vector<TimerEntity> vec;
	getExpireTimer(evtime, vec);
	if (vec.empty())
	{
		DLOG(WARNING) << "***no timer is on timing";
		return;
	}

	///执行到期的timer回调函数，多个以此执行
	for (std::vector<TimerEntity>::iterator it = vec.begin();
		it != vec.end(); it++)
	{
		if (it->second != NULL)
		{
			it->second->run();
		}
	}

	//重置定时器
	resetExpired(vec, evtime);

}

/**
* @brief    内部调用，插入定时
* @param[in] tmer:定时器
* @return bool
* @retval 定时器最小的到期时间点，是否改变，调用者是否重置timerfd等待时间
*/
bool TimerQueue::insert(Timer* tmer)
{
	bool bnextExpireChanged = false;
	do
	{
		TimerList::iterator it = timerLists_.begin();
		Timestamp newtm = tmer->expiration();
		if (!newtm.valid())
		{
			delete tmer;
			tmer = NULL;
			break;
		}

		if (it == timerLists_.end() || newtm < it->first)
		{
			bnextExpireChanged = true;
		}

		timerLists_.insert(TimerEntity(newtm, tmer));
	} while (0);

	return bnextExpireChanged;

}

/**
* @brief    获取到期的定时器队列
* @param[in] tm：到期时间戳
* @param[out] expire:定时器队列
*/
void TimerQueue::getExpireTimer(Timestamp& tm, std::vector<TimerEntity>& expire)
{
	TimerEntity curTimer(tm, reinterpret_cast<Timer*>(UINTPTR_MAX));
	//比tm时间戳小的所有定时器都判为过期定时器
	TimerList::iterator expireUper = timerLists_.lower_bound(curTimer);
	if (expireUper == timerLists_.end())
	{
		return;
	}

	//删除过期定时器，如果brepeat，后面会重置时间戳，并再次加入
	std::copy(timerLists_.begin(), expireUper, std::back_inserter(expire));
	timerLists_.erase(timerLists_.begin(), expireUper);
}

/**
* @brief    重置过期定时器
* @param[in] tm：到期时间戳
* @param[out] expire:定时器队列
*/
void TimerQueue::resetExpired(const std::vector<TimerEntity>& expire, Timestamp& tm)
{
  for (std::vector<TimerEntity>::const_iterator it = expire.begin(); it != expire.end(); it++)
	{
		if (it->second->isRepeat())
		{
			it->second->restartTimer();
			insert(it->second);
		}
		else
		{
		  //	delete it->second;
			//it->second = NULL;
			//freeTimerIdLists_.push_back(*it);
		}

	}

	if (timerLists_.empty())
	{
		return;
	}

	Timestamp nextExpire = timerLists_.begin()->second->expiration();
	if (nextExpire.valid())
	{
		resetTimerFd(timerfd_, nextExpire);
	}
}




/**
* @brief    删除定时器
* @param[in] id：定时器ID
*/
void TimerQueue::delTimer(TimerId& id)
{
	ownerLoop_->runInLoop(std::bind(&TimerQueue::delTimerInLoop, this, id));
}


/**
* @brief    保证同一线程操作timerlist
* @param[in] id：定时器ID
*/
void TimerQueue::delTimerInLoop(TimerId& id)
{
	bool bfind = false;

	std::set<TimerEntity>::iterator it;
	for (it = timerLists_.begin(); it != timerLists_.end(); )
	{
		if (it->second == id.tm_)
		{
			delete it->second;
			//it->second = NULL;
			it = timerLists_.erase(it);
			bfind = true;
			break;
		}

		it++;
	}

	if (bfind && timerLists_.size() > 0)
	{
		//重置定时器
		TimerList::iterator it = timerLists_.begin();
		Timestamp tim = it->first;
		resetTimerFd(timerfd_, tim);
	}

}