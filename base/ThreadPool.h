/**
*  @file  ThreadPool.h
*  @brief �̳߳ط�װ
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
	///�߳�ִ�к���
	typedef std::function<void()> TaskFunc;

	/** @class ThreadPool
	*  @brief �̳߳ط�װ��
	*   ���ɿ������죬��ֵ
	*/
class ThreadPool : public noncopyable
{
public:
	ThreadPool();
	~ThreadPool();

private:
	std::vector<Thread*> threadList_;///<�̶߳���
	Condition notEmpty_;///<�߳�֪ͨ����
	MutexLock mutex_;///<�̰߳�ȫ��
	bool brunning_;

	std::deque<TaskFunc> taskList_;///<ִ�к�������

public:
	///�����̳߳�
	void start(int threadNum);
	///ֹͣ�̳߳�
	void stop();

	///���������̳߳�
	void run(TaskFunc fun);
	TaskFunc getTask();
private:
	///�̳߳�ִ�к���
	void runInThread();
};

}

}//end of namespace

#endif
