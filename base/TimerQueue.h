/**
*  @file  TimerQueue.h
*  @brief 定时器队列
*  @version 0.0.1
*  @since 0.0.1
*  @author
*  @date 2016-7-25  Created it
*/
/******************************************************************************
*@note
Copyright 2007, BeiJing Bluestar Corporation, Limited
ALL RIGHTS RESERVED
Permission is hereby granted to licensees of BeiJing Bluestar, Inc. products
to use or abstract this computer program for the sole purpose of implementing
a product based on BeiJing Bluestar, Inc. products. No other rights to
reproduce, use, or disseminate this computer program,whether in part or  in
whole, are granted. BeiJing Bluestar, Inc. makes no representation or
warranties with respect to the performance of this computer program, and
specifically disclaims any responsibility for any damages, special or
consequential, connected with the use of this program.
For details, see http://www.bstar.com.cn/
******************************************************************************/
#ifndef _TIMERQUEUE_H_
#define _TIMERQUEUE_H_

#include "EventHandler.h"
#include <set>
#include <utility>
#include <vector>

#include "Timer.h"
#include "Timestamp.h"

#include <functional>

namespace ws
{

namespace base
{

  class Reactor;


  /** @class TimerId
  *  @brief 定时器标识timerid
  *  可拷贝构造，赋值
  */
class TimerId
{
public:
	TimerId()
		:index_(0), tm_(NULL)
	{}

	TimerId(int index, Timer* tmer)
	{
		index_ = index;
		tm_ = tmer;
	}

	int64_t index_; ///<序号标识
	Timer* tm_;///<定时器指针

};



/** @class TimerQueue
*  @brief 定时器队列类
*   管理多个定时器，定时唤醒
*  不可拷贝构造，赋值
*/
 class TimerQueue : public noncopyable
{
public:
	TimerQueue(Reactor* loop);
	~TimerQueue(void);

	///定时器实体定义
	typedef std::pair<Timestamp, Timer*> TimerEntity;
	///定时器列表定义
	typedef std::set<TimerEntity> TimerList;
	///timer队列定义
	typedef std::pair<int64_t, Timer*> TimerIdentity;
	typedef std::set<TimerIdentity> TimerIdList;

private:

	Reactor* ownerLoop_;///<定时器事件轮询
	const int timerfd_; ///<timerfd
        EventHandler timerfdEv_;///<事件调度器 

	TimerList timerLists_;  ///<定时器列表
	TimerIdList freeTimerIdLists_;///定时器ID管理列表
public:
        ///usually only execute once time
	TimerId addTimer(Timestamp& time, TimerCallback timerCallback, const double interval = 0.0, bool brepeat = false);
        ///usually can execute repeat
        TimerId addTimer(Timer* tm);	
	///删除定时器
void delTimer(TimerId& id);
void delTimerInLoop(TimerId& id);
        

private:
	///timerfd读函数
	void eventRead(Timestamp evtime);
	///插入定时器
	bool insert(Timer* tmer);
	///获取到期的定时器
	void getExpireTimer(Timestamp& tm, std::vector<TimerEntity>& expire);
	///重置到期定时器
	void resetExpired(const std::vector<TimerEntity>& expire, Timestamp& tm);
};

}

}//namespace

#endif
