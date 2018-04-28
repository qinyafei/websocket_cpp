/**
*  @file  Reactor.h
*  @brief 事件监听处理中心
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
#ifndef _REACTOR_H_
#define _REACTOR_H_

#include <sys/types.h>
#include "MutexLock.h"
#include <vector>
#include "EventHandler.h"
#include <functional>
#include "Poller.h"
#include "TimerQueue.h"
#include "Thread.h"
#include "noncopyable.h"

namespace ws
{

namespace base
{



	/** @class Reactor
	*  @brief 事件触发器
	*   不可拷贝构造，赋值
	*  
	*  功能：1 多线程下的异步，安全，顺序执行
	*       2 定时器触发调用
	*       3 网络socket读写触发
	*/
	class Reactor : public noncopyable
	{
	public:
		Reactor();
		~Reactor();

		typedef std::vector<EventHandler* > EventList;
		typedef std::function<void()> TaskFunctor;

	private:
		EventList activeList_; ///<触发事件队列
		EventHandler* taskHandler_; ///<轮询线程处理触发任务
		int taskWakeupFd_;///<事件触发任务fd
		Poller* poller_;///<轮询poll
		bool bquit_;
		const pid_t threadId_;
		MutexLock mutex_;
		std::vector<TaskFunctor> functorList_; ///<触发任务队列
		std::vector<TimerQueue*> timersList_; ///定时器队列

		///如果是正在执行task，则需要重新唤醒一次，否则queueInLoop不会立即执行
		bool btaskRunning_;
	public:
		void loop();///<阻塞监听
		void quit(); ///<退出
		///多线程下的异步，安全，顺序执行
		void runInLoop(const TaskFunctor& func); ///<触发任务
		void queueInLoop(const TaskFunctor& func);///<在触发任务队列，增加新任务
		void removeEvHandler(EventHandler* handler); ///<删除监听事件

		///调用者是否处于同一执行线程
		bool isInLoopThread()
		{
			return (threadId_ == CurrentThread::tid());
		}

		///注册监听事件
		void registerEvHandler(EventHandler* handler);
		///timers
		///定时执行任务
		TimerId runAt(Timestamp& time, const TimerCallback& cb);
		TimerId runAt(Timer* tm);
		///延迟delay秒后执行任务
		TimerId runDelay(double delay, const TimerCallback& cb);
		///间隔一段时间循环执行
		TimerId runEvery(double interval, const TimerCallback& cb);
		///取消定时器
		void cancelTimer(TimerId& id);
	private:
		///唤醒触发任务fd
		void taskWakeupRead(Timestamp waketm);
		//void taskWakeup();
		///执行触发式任务
		void runningTask();

	};

}

}//namespace

#endif
