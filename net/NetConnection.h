/**
*  @file  NetConnection.h
*  @brief 网络连接类
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
#ifndef _NET_CONNECTION_H_
#define _NET_CONNECTION_H_


#include <netinet/in.h>
#include "noncopyable.h"
#include "Buffer.h"
#include <functional>
#include <string>
#include "Reactor.h"
#include "EventHandler.h"

#include "NetCallbacks.h"


using namespace ws::base;

namespace ws
{

	namespace net
	{



		/** @class NetConnection
		*  @brief 网络连接处理类，server和client都会用到此类
		*   不可拷贝构造和赋值
		*/
		class NetConnection : public noncopyable
		{
		public:
			NetConnection(Reactor* loop, std::string connid, int sockfd, const sockaddr_in& localAddr,
				const sockaddr_in& peerAddr);
			~NetConnection();

			enum NetState
			{
				eDisconnected = 0,
				eConnecting,
				eConnected
			};

		private:
			Reactor* loop_; ///<事件监听轮询
			EventHandler* event_;///<事件监听句柄

			struct sockaddr_in localAddr_; ///<本地地址
			struct sockaddr_in remoteAddr_;///<远程地址

			Buffer readBuffer_;  ///<读数据缓存
			Buffer writeBuffer_;///<写数据缓存

			ConnectionCallback connCallbackFunc_; ///<建立连接回调
			MessageCallback msgRecvFunc_;///<数据接收回调
			WriteCompleteCallback writeCompleteFunc_;///<数据发送完成回调
			CloseConnCallback closeFunc_; ///<链接关闭回调

			NetState state_;  ///<链接状态
			std::string connId_;  ///<链接标识符ID
			void* context_; ///<
		private:
			///socket 读、写、关闭回函数
			void handleRead(Timestamp tm);
			void handleWrite();
			void handleClose();
			void handleError();

			///数据发送接口
			void sendInReactor(const std::string& data);
			///链接关闭接口
			void shutdownInReactor();

		public:
			void setConnCallback(const ConnectionCallback& cb)
			{
				connCallbackFunc_ = cb;
			}
			void setMsgCallback(const MessageCallback& cb)
			{
				msgRecvFunc_ = cb;
			}
			void setWriteCompleteCallback(const WriteCompleteCallback& cb)
			{
				writeCompleteFunc_ = cb;
			}

			void setCloseCallback(const CloseConnCallback& cb)
			{
				closeFunc_ = cb;
			}

			bool isConnected() const
			{
				return (state_ == eConnected);
			}

			const std::string& connId() const
			{
				return connId_;
			}
			Reactor* getLoop() const
			{
				return loop_;
			}

			void setContext(void* context)
			{
				context_ = context;
			}
			void* getContext()
			{
				return context_;
			}

			///发送数据接口
			void send(const char* msg, int len);
			///外部强制断开连接
			void forceClose();

			///链接建立回调
			void connEstablished();
			///链接关闭
			void shutdown();


		};

	}

}//namespace


#endif
