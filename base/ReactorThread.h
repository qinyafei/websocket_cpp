/**
*  @file  ReactorThread.h
*  @brief 事件监听处理线程
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
	*  @brief 事件监听线程
	*   包含监听线程，外部调用可实现非阻塞
	*  非线程安全
	*/
class ReactorThread : public noncopyable
{
public:
	ReactorThread();
	~ReactorThread();

private:
	MutexLock mutex_;///用于通知
	Condition wait_;
	Thread thread_;///<监听线程
	Reactor* loop_;///<事件监听轮询


public:
	///开启事件监听轮询
	Reactor* startLoop();
	///监听线程函数
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
