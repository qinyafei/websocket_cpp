/**
*  @file  ThreadPool.h
*  @brief 线程池封装
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
#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <deque>
#include "Thread.h"
#include "Condition.h"
#include "MutexLock.h"
#include <vector>

#include "noncopyable.h"

namespace ws
{

namespace base
{
	///线程执行函数
	typedef std::function<void()> TaskFunc;

	/** @class ThreadPool
	*  @brief 线程池封装类
	*   不可拷贝构造，赋值
	*/
class ThreadPool : public noncopyable
{
public:
	ThreadPool();
	~ThreadPool();

private:
	std::vector<Thread*> threadList_;///<线程队列
	Condition notEmpty_;///<线程通知变量
	MutexLock mutex_;///<线程安全锁
	bool brunning_;

	std::deque<TaskFunc> taskList_;///<执行函数队列

public:
	///开启线程池
	void start(int threadNum);
	///停止线程池
	void stop();

	///插入任务到线程池
	void run(TaskFunc fun);
	TaskFunc getTask();
private:
	///线程池执行函数
	void runInThread();
};

}

}//end of namespace

#endif
