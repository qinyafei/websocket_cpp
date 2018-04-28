/**
*  @file  Connector.h
*  @brief client连接服务器处理,与Acceptor相对
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
#ifndef _CONNECTOR_H_
#define _CONNECTOR_H_

#include <netinet/in.h>
#include <functional>
#include "EventHandler.h"
#include "Reactor.h"

#include "noncopyable.h"

using namespace ws::base;

namespace ws
{

namespace net
{

	/** @class Connector
	*  @brief client链接建立处理类
	*/
	class Connector : public noncopyable
	{
	public:
		Connector(Reactor* loop, const sockaddr_in& addr);
		~Connector();

		typedef std::function<void(int sockfd)> NewConnectionCreate;

		///链接状态
		enum NetState
		{
			eDisconnected = 0,
			eConnected
		};

		static const double csInitRetryDelay;
		static const double csMaxRetryDelay;
	private:
		struct sockaddr_in servAddr_;///<server 地址
		Reactor* loop_;///<事件监听轮询
		NetState state_;///<链接状态
		EventHandler* event_;///<监听句柄
		NewConnectionCreate newConnCreateCallback_; ///<链接建立回调
		double retryDelayMs_; ///重连，延迟毫秒
		TimerId retryTimerId_;
	private:
		void handleWrite();
		void handleError();
		///在当前IO线程内连接
		void startInLoop();
		void connectImpl(int sockfd);
		///重置链接
		int resetEvent();
		void resetInLoop();
		///重连
		void retry(int sockfd);

	public:
		void start();
		void stop();
		///connect，非阻塞
		void connect();
		void setNewConnectionCallback(const NewConnectionCreate& cb)
		{
			newConnCreateCallback_ = cb;
		}

	};

}
}//namespace

#endif
