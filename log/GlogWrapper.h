/**
*  @file  GlogWrapper.h
*  @brief glog封装
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
#ifndef _GLOGWRAPPER_H_
#define _GLOGWRAPPER_H_


#include "glog/logging.h"
#include "glog/raw_logging.h"


///捕捉 SIGSEGV 信号信息输出
void SigSEGVHandle(const char* err, int size);



/** @class GlogWrapper
*  @brief glog封装类
*  非线程安全
*/
class GlogWrapper
{
public:
	GlogWrapper(const char* program);
	~GlogWrapper();
};


#endif
