/**
*  @file  TimerQueue.h
*  @brief ��ʱ������
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
  *  @brief ��ʱ����ʶtimerid
  *  �ɿ������죬��ֵ
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

	int64_t index_; ///<��ű�ʶ
	Timer* tm_;///<��ʱ��ָ��

};



/** @class TimerQueue
*  @brief ��ʱ��������
*   ��������ʱ������ʱ����
*  ���ɿ������죬��ֵ
*/
 class TimerQueue : public noncopyable
{
public:
	TimerQueue(Reactor* loop);
	~TimerQueue(void);

	///��ʱ��ʵ�嶨��
	typedef std::pair<Timestamp, Timer*> TimerEntity;
	///��ʱ���б���
	typedef std::set<TimerEntity> TimerList;
	///timer���ж���
	typedef std::pair<int64_t, Timer*> TimerIdentity;
	typedef std::set<TimerIdentity> TimerIdList;

private:

	Reactor* ownerLoop_;///<��ʱ���¼���ѯ
	const int timerfd_; ///<timerfd
        EventHandler timerfdEv_;///<�¼������� 

	TimerList timerLists_;  ///<��ʱ���б�
	TimerIdList freeTimerIdLists_;///��ʱ��ID�����б�
public:
        ///usually only execute once time
	TimerId addTimer(Timestamp& time, TimerCallback timerCallback, const double interval = 0.0, bool brepeat = false);
        ///usually can execute repeat
        TimerId addTimer(Timer* tm);	
	///ɾ����ʱ��
void delTimer(TimerId& id);
void delTimerInLoop(TimerId& id);
        

private:
	///timerfd������
	void eventRead(Timestamp evtime);
	///���붨ʱ��
	bool insert(Timer* tmer);
	///��ȡ���ڵĶ�ʱ��
	void getExpireTimer(Timestamp& tm, std::vector<TimerEntity>& expire);
	///���õ��ڶ�ʱ��
	void resetExpired(const std::vector<TimerEntity>& expire, Timestamp& tm);
};

}

}//namespace

#endif
