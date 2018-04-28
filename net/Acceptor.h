/**
*  @file  Acceptor.h
*  @brief ��������acceptor������client����
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
#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#include <functional>
#include "SocketOpts.h"
#include "Reactor.h"
#include "EventHandler.h"

using namespace ws::base;


namespace ws
{

namespace net
{

	/** @class Acceptor
	*  @brief ������acceptor������client����
	*/
class Acceptor
{
public:
	Acceptor(Reactor* loop, const struct sockaddr_in& addr);
	~Acceptor();

	typedef std::function<void(int sockfd, const sockaddr_in&)> AcceptConnCallback;

private:
	AcceptConnCallback acceptConnFunc_;///<�½����ӻص�����
	Reactor* loop_;///<�¼�������ѯ
	EventHandler* event_;///<�¼�������
	int listenfd_;///<server socketfd

private:
	///����client��������
	void handleRead();

public:
	void setAcceptConnCallback(const AcceptConnCallback& cb)
	{
		acceptConnFunc_ = cb;
	}

	///����
	void listen();

};


}
}//namespace ws 


#endif
