/**
*  @file  Poller.h
*  @brief 事件轮询基类文件
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
#ifndef _POLLER_H_
#define _POLLER_H_

#include <vector>
#include "EventHandler.h"
#include <map>
#include <poll.h>
#include <arpa/inet.h>

namespace ws
{

namespace base
{



	/** @class Poller
	*  @brief 事件轮询基类，可继承poll和epoll模型
	*   不可拷贝，赋值
	*  非线程安全
	*/
class Poller
{
public:
  Poller(){}
  virtual ~Poller(){}

	typedef std::vector<EventHandler* > EventList;

	virtual void poll(int timeout, EventList& activeEvents) = 0;
	virtual void registerEvHandler(EventHandler* event) = 0;
	virtual void removeEvHandler(EventHandler* event) = 0;
        static Poller* createPoller();

protected:
	typedef std::map<int, EventHandler*> EvHandlerMap; 
	EvHandlerMap evHandlers_;///<监听队列

};

}

}//namespace

#endif
