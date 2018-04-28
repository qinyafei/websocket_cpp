/**
*  @file  SelectPoller.h
*  @brief poll��ѯ
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
#ifndef _SELECTPOLLER_H_
#define _SELECTPOLLER_H_

#include "Poller.h"
#include "MutexLock.h"

namespace ws
{

namespace base
{ 


	/** @class SelectPoller
	*  @brief poll��ѯ�࣬�̳���Poller
	*/
class SelectPoller : public Poller
{
public:
	SelectPoller();
	~SelectPoller();

	typedef std::vector<struct pollfd> PollFdList;
    
	///poll��ѯ
	virtual void poll(int timeout, EventList& activeEvents);
	///ע������¼�
	virtual void registerEvHandler(EventHandler* event);
	///ɾ�������¼�
	virtual void removeEvHandler(EventHandler* event);

private:
	///������д������¼�
	void findActiveEvHandlers(int num, EventList& activeEvents);

private:
	PollFdList pollFds_; ///<pollfd ��������
	MutexLock mutex_;
};

}

}//namespace

#endif
