/**
*  @file  common.h
*  @brief 通用函数调用文件
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
#ifndef _COMMON_H_
#define _COMMON_H_


#include <sys/eventfd.h>
#include <signal.h>
#include <assert.h>
#include <unistd.h>
#include <arpa/inet.h>



/**
* @brief    创建事件fd
* @return int
* @retval  返回eventfd
*/
static int createEventfd()
{
	int evfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	assert(evfd >= 0);

	return evfd;
}


/**
* @brief    事件读
* @param[in] evfd 
*/
static void eventfdRead(int evfd)
{
	uint64_t cache = 1;
	int readLen = ::read(evfd, static_cast<void*>(&cache), sizeof cache);
	if (readLen != sizeof cache)
	{
		assert(0);
	}
}

/**
* @brief    事件写
* @param[in] evfd
*/
static void eventfdWrite(int evfd)
{
	uint64_t cache = 1;
	int writeLen = ::write(evfd, static_cast<void*>(&cache), sizeof cache);
	if (writeLen != sizeof cache)
	{
		//
		assert(0);
	}
}




#endif