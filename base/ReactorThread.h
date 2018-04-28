/**
*  @file  ReactorThread.h
*  @brief �¼����������߳�
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
#ifndef _REACTORTHREAD_H_
#define _REACTORTHREAD_H_

#include "Reactor.h"
#include "Thread.h"
#include "MutexLock.h"
#include "Condition.h"
#include "noncopyable.h"


namespace ws
{

namespace base
{



	/** @class ReactorThread
	*  @brief �¼������߳�
	*   ���������̣߳��ⲿ���ÿ�ʵ�ַ�����
	*  ���̰߳�ȫ
	*/
class ReactorThread : public noncopyable
{
public:
	ReactorThread();
	~ReactorThread();

private:
	MutexLock mutex_;///����֪ͨ
	Condition wait_;
	Thread thread_;///<�����߳�
	Reactor* loop_;///<�¼�������ѯ


public:
	///�����¼�������ѯ
	Reactor* startLoop();
	///�����̺߳���
	void threadProc();

	///get Reactor*
	Reactor* getLoop()
	{
		return loop_;
	}
};




}//base

}

#endif
