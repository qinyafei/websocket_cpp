/**
*  @file  MutexLock.h
*  @brief 互斥锁
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
#ifndef _MUTEXLOCK_H_
#define _MUTEXLOCK_H_

#include "noncopyable.h"
#include <assert.h>

#include <pthread.h>

namespace ws
{

namespace base
{


	/** @class MutexLock
	*  @brief 互斥锁
	*   不可拷贝，赋值
	*  非线程安全
	*/
class MutexLock : public  noncopyable
{
public:
	MutexLock()
		:holder_(0)
	{
		assert(pthread_mutex_init(&mutex_, NULL) == 0);
	}

	~MutexLock()
	{
		assert(holder_ == 0);
		assert(pthread_mutex_destroy(&mutex_) == 0);
	}

	///锁
	void lock()
	{
		assert(pthread_mutex_lock(&mutex_) == 0);
		holder_ = 1; //current thread tid
	}

	///解锁
	void unlock()
	{
		assert(pthread_mutex_unlock(&mutex_) == 0);
		holder_ = 0;
	}

	pthread_mutex_t* getThreadMutex()
	{
		return &mutex_;
	}

private:
	pthread_mutex_t mutex_;  ///<互斥量
	int holder_;
};




/** @class LockGuard
*  @brief 互斥锁 auto模式
*   不可拷贝，赋值
*  非线程安全
*/
class LockGuard : public noncopyable
{
public:
	LockGuard(MutexLock* mutex)
		:mutex_(mutex)
	{
		mutex_->lock();
	}

	~LockGuard()
	{
		mutex_->unlock();
	}

private:
	MutexLock* mutex_;
};


}

}//end of namespace base

#endif
