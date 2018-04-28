/**
*  @file  EventHandler.h
*  @brief �����첽�¼�������
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
*  @brief �����첽�¼���������Ӧ�ص�����
*   ���ɿ�������ֵ
*  ���̰߳�ȫ
*/
 class EventHandler : public noncopyable
{
public:
	EventHandler(Reactor* reactor, int fd);
	~EventHandler();

	typedef std::function<void()> EventCallback;  ///<�¼��ص�����
	typedef std::function<void(Timestamp)> ReadEventCallback; ///<���¼��ص�����
	//
	static const int csEventNone;///<���¼�
	static const int csEventRead;///<���¼�
	static const int csEventWrite;///<д�¼�

private:
	int fd_; 
	Reactor* preactor_; ///<�¼�������ѯ
	int events_;  ///<�������¼�
	int revents_;  ///<�����¼�

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
        ///���¼���
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

	///����ע������¼���poll
	void update();
    ///ɾ������handler
	void remove();
	///��������¼�
	void handEvent();
	///�����ɶ�
	void enableRead() 
	{ 
		events_ |= csEventRead;
		update(); 
	}
	///ȡ���ɶ�
	void disableRead() 
	{
		events_ &= ~csEventRead; 
		update(); 
	}
	///������д
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
	///ֹͣ���������¼�
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
