/**
*  @file  Timer.h
*  @brief 定时器处理
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
#ifndef _TIMER_H_
#define _TIMER_H_

#include "Timestamp.h"
#include "MutexLock.h"
#include <functional>
#include "TimerAlgorithm.h"
#include "noncopyable.h"

namespace ws
{

	namespace base
	{

		typedef std::function<void()> TimerCallback;

		/** @class Timer
		*  @brief 定时器类
		*   不可拷贝构造，赋值
		*/
		class Timer : public noncopyable
		{
		public:
			Timer(TimerCallback timerfunc, Timestamp tm, double interval = 0.0, bool repeat = true)
				:timerCallback_(timerfunc),
				expiration_(tm),
				brepeat_(repeat),
				compute_(NULL),
				interval_(interval)
			{
				index_ = indexCreate();
			}

		private:
			TimerCallback timerCallback_;///<定时器回调函数
			int64_t index_; ///<定时器索引标识
			Timestamp expiration_;///<定时器时间戳
			bool brepeat_;///<是否重复定时
			TimerAlgorithm* compute_;///<定时器算法
			void* context_; ///<上下文指针，此处存放task

			static int64_t sIndexNum;///<定时器计数
			static MutexLock mutex_;
			double interval_; ///<间隔执行时间段

		public:
			int64_t index()
			{
				return index_;
			}

			///定时器启动
			void run();
			///异步线程执行函数
			void asyncRunProc();

			///设置定时算法
			void setTimerAlgorithm(TimerAlgorithm* compute)
			{
				compute_ = compute;
			}
			///设置、获取定时器上下文
			void setContext(void* context)
			{
				context_ = context;
			}
			void* getContext()
			{
				return context_;
			}

			///重启定时器
			void restartTimer()
			{
				if (compute_ != NULL)
				{
					expiration_ = compute_->nextTimestamp();
				}
				else if (interval_ > 0.0)
				{
					Timestamp now = Timestamp::now();
					expiration_ = addSeconds(now, interval_);
				}
			}

			bool isRepeat()
			{
				return brepeat_;
			}

			//void addInterval(const double seconds){}
			Timestamp expiration()
			{
				return expiration_;
			}
			///设置当前时间戳
			void setExpiration(Timestamp& tm)
			{
				expiration_ = tm;
			}

			static int64_t indexCreate()
			{
				LockGuard lock(&mutex_);
				return sIndexNum++;
			}

		};

	}

} //namespace
#endif
