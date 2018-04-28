#include "TimerQueue.h"

#include <iterator>
#include <sys/timerfd.h>
#include <string.h>

#include "Reactor.h"
#include "GlogWrapper.h"

using namespace ws::base;

/**
* @brief    ����timerfd
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
* @brief    ʱ�����˿̵�ʱ���
* @param[in] when���Ƚ�ʱ���
* @return timespec
* @retval  ʱ���
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
* @brief    ����timerfd��ʱ
* @param[in] tm��ʱ���
* @param[in] timerfd��
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

  //add MAX timer�����ڴ���ֻ��һ����ʱ�����������
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
* @brief    �ⲿֱ�ӵ��ò��붨ʱ��
* @param[in] timerCallback����ʱ���ص�����
* @param[in] time:ʱ���
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
* @brief    �ⲿֱ�ӵ��ã����ز��붨ʱ��
* @param[in] tm:��ʱ��
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
* @brief    ��ʱ�����ڣ�ִ�лص�
* @param[in] evtime:����ʱ���
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

	///ִ�е��ڵ�timer�ص�����������Դ�ִ��
	for (std::vector<TimerEntity>::iterator it = vec.begin();
		it != vec.end(); it++)
	{
		if (it->second != NULL)
		{
			it->second->run();
		}
	}

	//���ö�ʱ��
	resetExpired(vec, evtime);

}

/**
* @brief    �ڲ����ã����붨ʱ
* @param[in] tmer:��ʱ��
* @return bool
* @retval ��ʱ����С�ĵ���ʱ��㣬�Ƿ�ı䣬�������Ƿ�����timerfd�ȴ�ʱ��
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
* @brief    ��ȡ���ڵĶ�ʱ������
* @param[in] tm������ʱ���
* @param[out] expire:��ʱ������
*/
void TimerQueue::getExpireTimer(Timestamp& tm, std::vector<TimerEntity>& expire)
{
	TimerEntity curTimer(tm, reinterpret_cast<Timer*>(UINTPTR_MAX));
	//��tmʱ���С�����ж�ʱ������Ϊ���ڶ�ʱ��
	TimerList::iterator expireUper = timerLists_.lower_bound(curTimer);
	if (expireUper == timerLists_.end())
	{
		return;
	}

	//ɾ�����ڶ�ʱ�������brepeat�����������ʱ��������ٴμ���
	std::copy(timerLists_.begin(), expireUper, std::back_inserter(expire));
	timerLists_.erase(timerLists_.begin(), expireUper);
}

/**
* @brief    ���ù��ڶ�ʱ��
* @param[in] tm������ʱ���
* @param[out] expire:��ʱ������
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
* @brief    ɾ����ʱ��
* @param[in] id����ʱ��ID
*/
void TimerQueue::delTimer(TimerId& id)
{
	ownerLoop_->runInLoop(std::bind(&TimerQueue::delTimerInLoop, this, id));
}


/**
* @brief    ��֤ͬһ�̲߳���timerlist
* @param[in] id����ʱ��ID
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
		//���ö�ʱ��
		TimerList::iterator it = timerLists_.begin();
		Timestamp tim = it->first;
		resetTimerFd(timerfd_, tim);
	}

}