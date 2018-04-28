/**
*  @file  Timer.h
*  @brief ��ʱ������
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
		*  @brief ��ʱ����
		*   ���ɿ������죬��ֵ
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
			TimerCallback timerCallback_;///<��ʱ���ص�����
			int64_t index_; ///<��ʱ��������ʶ
			Timestamp expiration_;///<��ʱ��ʱ���
			bool brepeat_;///<�Ƿ��ظ���ʱ
			TimerAlgorithm* compute_;///<��ʱ���㷨
			void* context_; ///<������ָ�룬�˴����task

			static int64_t sIndexNum;///<��ʱ������
			static MutexLock mutex_;
			double interval_; ///<���ִ��ʱ���

		public:
			int64_t index()
			{
				return index_;
			}

			///��ʱ������
			void run();
			///�첽�߳�ִ�к���
			void asyncRunProc();

			///���ö�ʱ�㷨
			void setTimerAlgorithm(TimerAlgorithm* compute)
			{
				compute_ = compute;
			}
			///���á���ȡ��ʱ��������
			void setContext(void* context)
			{
				context_ = context;
			}
			void* getContext()
			{
				return context_;
			}

			///������ʱ��
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
			///���õ�ǰʱ���
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
