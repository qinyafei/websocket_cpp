/**
*  @file  ReactorThreadPool.h
*  @brief 事件监听线程池
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
#ifndef _REACTORTHREADPOOL_H_
#define _REACTORTHREADPOOL_H_

#include <vector>
#include "noncopyable.h"

namespace ws
{

namespace base
{

	class Reactor;
	class ReactorThread;

	/** @class ReactorThreadPool
	*  @brief 同时开启多个事件监听线程，满足大量并发的场景
	*   不可拷贝构造，赋值
	*/
class ReactorThreadPool : public noncopyable
{
public:
	ReactorThreadPool(Reactor* base);
	~ReactorThreadPool();

private:
	Reactor* baseLoop_; ///<基础监听句柄
	int numThreads_;///<并发线程数
	int index_;
	std::vector<ReactorThread*> threads_; ///<监听线程队列
	std::vector<Reactor*> loops_;///<监听句柄队列

public:
	void setThreadNum(int numThreads)
	{
		numThreads_ = numThreads;
	}
	///开启监听线程池
	void start();
	///取监听线程池下一个监听句柄
	///此处采用轮询方式获取
	Reactor* nextLoop();

};

}

}//namespace

#endif
