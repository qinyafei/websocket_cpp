/**
*  @file  NetClient.h
*  @brief  网络client处理类
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
#ifndef _NETCLIENT_H_
#define _NETCLIENT_H_

#include "MutexLock.h"
#include <functional>
#include "Buffer.h"

#include "noncopyable.h"
#include "NetCallbacks.h"
#include "Reactor.h"

using namespace ws::base;


namespace ws
{

namespace net
{

	class Connector;
	class NetConnection;



	/** @class NetClient
	*  @brief 网络客户端处理类
	*/
	class NetClient : public noncopyable
	{
	public:
		NetClient(Reactor* loop, const char* dstIp, short dstPort);
		~NetClient();

	private:
		///建立连接回调函数
		void establishConnection(int sockfd);
		///销毁链接回调函数
		void destroyConnection(const NetConnection* conn);

	private:
		Reactor* loop_;///<事件监听轮询
		Connector* connector_;///<client端连接器
		NetConnection* conn_;///<连接对象
		int connIndex_;

		ConnectionCallback connCallback_;///<client连接回调
		MessageCallback msgCallback_;///<数据接收回调
		WriteCompleteCallback writeCompleteCallback_;///<数据发送成功回调
		struct sockaddr_in servAddr_; ///<server addr
		MutexLock mutex_;

		void* context_;///<链接上下文
	public:
		void setConnCallback(const ConnectionCallback& cb)
		{
			connCallback_ = cb;
		}
		void setWriteComplCallback(const WriteCompleteCallback& cb)
		{
			writeCompleteCallback_ = cb;
		}

		void setMsgCallback(const MessageCallback& cb)
		{
			msgCallback_ = cb;
		}

		NetConnection* getConn()
		{
			return conn_;
		}
		void setContext(void* context)
		{
			context_ = context;
		}
		void* getContext()
		{
			return context_;
		}
		///连接服务器
		void connect();
		///断开连接
		void disconnect();

	};


}

} //namespace

#endif
