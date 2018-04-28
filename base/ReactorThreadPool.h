/**
*  @file  ReactorThreadPool.h
*  @brief �¼������̳߳�
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
	*  @brief ͬʱ��������¼������̣߳�������������ĳ���
	*   ���ɿ������죬��ֵ
	*/
class ReactorThreadPool : public noncopyable
{
public:
	ReactorThreadPool(Reactor* base);
	~ReactorThreadPool();

private:
	Reactor* baseLoop_; ///<�����������
	int numThreads_;///<�����߳���
	int index_;
	std::vector<ReactorThread*> threads_; ///<�����̶߳���
	std::vector<Reactor*> loops_;///<�����������

public:
	void setThreadNum(int numThreads)
	{
		numThreads_ = numThreads;
	}
	///���������̳߳�
	void start();
	///ȡ�����̳߳���һ���������
	///�˴�������ѯ��ʽ��ȡ
	Reactor* nextLoop();

};

}

}//namespace

#endif
