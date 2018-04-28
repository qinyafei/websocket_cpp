/**
*  @file  NetServer.h
*  @brief 网络服务处理
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
#ifndef _NETSERVER_H_
#define _NETSERVER_H_


#include "NetCallbacks.h"
#include <map>
#include "Reactor.h"
#include "ReactorThreadPool.h"

using namespace ws::base;

namespace ws
{

namespace net
{

	class Acceptor;
	class NetConnection;
	

	/** @class NetServer
	*  @brief 服务器处理类
	*/
	class NetServer
	{
	public:
		NetServer(Reactor* loop,
			const char* servIp, short servPort);
		~NetServer();

		typedef std::map<std::string, NetConnection*> ConnectionMap;

	private:
		Reactor* loop_;  ///<事件监听轮询
		struct sockaddr_in listenAddr_;///<服务器监听地址
		Acceptor* acceptor_;  ///<client链接接收处理器

		ConnectionCallback connCallbackFunc_;  ///<新建链接回调
		MessageCallback msgCallbackFunc_;  ///<数据接收回调
		WriteCompleteCallback writeComplFunc_;///<数据发送完成回调
		ReactorThreadPool* threadPool_; ///<事件监听线程池，应用于大量并发连接

		int connIndex_;
		ConnectionMap conns_; ///<网络连接管理队列

	private:
		///新建client链接回调
		void newConnAccept(int sockfd, const struct sockaddr_in& clientAddr);
		///删除链接回调
		void removeConn(const NetConnection* conn);
		void removeConnInLoop(const NetConnection* conn);

	public:
		///服务器启动接口
		void start();
		NetConnection* getconn(std::string connid);

		void setConnCallback(const ConnectionCallback& cb)
		{
			connCallbackFunc_ = cb;
		}

		void setMsgCallback(const MessageCallback& cb)
		{
			msgCallbackFunc_ = cb;
		}

		void setWriteComplCallback(const WriteCompleteCallback& cb)
		{
			writeComplFunc_ = cb;
		}
		///设置监听线程池线程个数
		void setThreadNum(int numThreads);
	};

}

}//namespace

#endif
