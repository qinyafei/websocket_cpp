/**
*  @file  EventHandler.h
*  @brief 监听异步事件，处理
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

#ifndef _EVENTHANDLER_H_
#define _EVENTHANDLER_H_

#include "Timestamp.h"
#include <functional>
#include "noncopyable.h"


namespace ws
{

namespace base
{

class Reactor;


/** @class EventHandler
*  @brief 监听异步事件，调用相应回掉处理
*   不可拷贝，赋值
*  非线程安全
*/
 class EventHandler : public noncopyable
{
public:
	EventHandler(Reactor* reactor, int fd);
	~EventHandler();

	typedef std::function<void()> EventCallback;  ///<事件回调函数
	typedef std::function<void(Timestamp)> ReadEventCallback; ///<读事件回调函数
	//
	static const int csEventNone;///<无事件
	static const int csEventRead;///<读事件
	static const int csEventWrite;///<写事件

private:
	int fd_; 
	Reactor* preactor_; ///<事件监听轮询
	int events_;  ///<待监听事件
	int revents_;  ///<触发事件

	///event callback function
	ReadEventCallback readCallback_;
	EventCallback writeCallback_;
	EventCallback closeCallback_;
	EventCallback errorCallback_;
	


public:
	int fd()
	{
		return fd_; 
	}

	int events()
	{
		return events_;
	}

	void setRevents(int revent)
	{
		revents_ = revent;
	}
        ///无事监听
        bool isNoneEvent()
        {
	  return (events_ == csEventNone);
	}

	void setReadCallback(const ReadEventCallback& rcb )
	{
		readCallback_ = rcb; 
	}

	void setWriteCallback(const EventCallback& wcb)
	{
		writeCallback_ = wcb;
	}

	void setCloseCallback(const EventCallback& ccb)
	{
		closeCallback_ = ccb;
	}

	///更新注册监听事件到poll
	void update();
    ///删除监听handler
	void remove();
	///处理监听事件
	void handEvent();
	///监听可读
	void enableRead() 
	{ 
		events_ |= csEventRead;
		update(); 
	}
	///取消可读
	void disableRead() 
	{
		events_ &= ~csEventRead; 
		update(); 
	}
	///监听可写
	void enableWrite() 
	{ 
		events_ |= csEventWrite; 
		update(); 
	}
	void disableWrite()
	{
		events_ &= ~csEventWrite; 
		update(); 
	}
	///停止监听所有事件
	void disableAll()
	{ 
		events_ = csEventNone; 
		update(); 
	}
	bool isWriteable() const
	{
		return events_ & csEventWrite; 
	}


};

}

}//namespace

#endif
