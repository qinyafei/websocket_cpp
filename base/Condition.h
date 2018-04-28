/**
*  @file  Conndition.h
*  @brief 线程同步
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
#ifndef _CONDITION_H_
#define _CONDITION_H_

#include "MutexLock.h"
#include "noncopyable.h"
#include <assert.h>
#include <pthread.h>
#include <errno.h>

namespace ws
{

namespace base
{


	/** @class Condition
	*  @brief 线程同步条件变量
	*  非线程安全，不可拷贝，赋值
	*  必须在MutexLock 作用域内使用
	*/
class Condition : public noncopyable
{
public:
	Condition(MutexLock& mutex)
		:mutex_(mutex)
	{
		assert(pthread_cond_init(&pcond_, NULL) == 0);
	}
	~Condition()
	{
		assert(pthread_cond_destroy(&pcond_) == 0);
	}

	///阻塞
	void wait()
	{
		assert(pthread_cond_wait(&pcond_, mutex_.getThreadMutex()) == 0);
	}

	///指定阻塞n秒
	bool waitForSeconds(int seconds)
	{
		struct timespec abstime;
		//TODO:
		clock_gettime(CLOCK_REALTIME, &abstime);
		abstime.tv_sec += seconds;
		return (ETIMEDOUT == pthread_cond_timedwait(&pcond_, mutex_.getThreadMutex(), &abstime));
	}

	///唤醒
	void notify()
	{
		assert(pthread_cond_signal(&pcond_) == 0);
	}
	///唤醒所有阻塞
	void notifyAll()
	{
		assert(pthread_cond_broadcast(&pcond_) == 0);
	}


private:
	MutexLock& mutex_; ///<互斥量
	pthread_cond_t pcond_;  ///<条件变量

public:

};

}

}//namespace

#endif
