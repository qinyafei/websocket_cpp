/**
*  @file  Thread.h
*  @brief 线程简单封装
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
#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>
#include <unistd.h>
#include "noncopyable.h"
#include <string>
#include <functional>


namespace ws
{

	/** @class CurrentThread
	*  @brief 线程相关函数
	*/
namespace CurrentThread
{
  ///获取当前线程ID
  int tid();
  ///sleep ,微妙
  void sleepUsec(int64_t usec);
}


namespace base
{
	typedef std::function<void()> ThreadFunc;


	/** @class Thread
	*  @brief 线程简单封装
	*   不可拷贝构造，赋值
	*/
class Thread : public noncopyable
{
public:
	explicit Thread(const ThreadFunc& func, const std::string name = std::string());
	~Thread();

private:
	pthread_t pthreadId_;///<线程句柄
	pid_t* tid_; ///<线程ID
	ThreadFunc func_;   ///<线程函数

	bool bstarted_;
	bool bjoined_; ///<joinable
	
	std::string name_; ///<线程名称，调试

private:
	void setDefaultName();

	///thread running function
    static void* ThreadProc(void* context);

public:
	void start();
	///thread wait for end
	int join();

	const std::string& name() const
	{
		return name_; 
	}
};


}

}//namespace

#endif
