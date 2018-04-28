/**
*  @file  TimerAlgorithm.h
*  @brief 定时器时间戳算法基类
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
#ifndef _TIMERALGORITHM_H_
#define _TIMERALGORITHM_H_

#include "Timestamp.h"

using namespace ws::base;

namespace ws
{

	/** @class TimerAlgorithm
	*  @brief 定时器时间戳算法基类，纯虚类
	*/
class TimerAlgorithm
{
public:
	TimerAlgorithm(void){}
	virtual ~TimerAlgorithm(void){}

public:
	///下一个时间戳虚函数接口
	virtual Timestamp nextTimestamp() = 0;
};


}

#endif
