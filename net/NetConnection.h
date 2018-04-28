/**
*  @file  NetConnection.h
*  @brief ����������
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
		*  @brief �������Ӵ����࣬server��client�����õ�����
		*   ���ɿ�������͸�ֵ
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
			Reactor* loop_; ///<�¼�������ѯ
			EventHandler* event_;///<�¼��������

			struct sockaddr_in localAddr_; ///<���ص�ַ
			struct sockaddr_in remoteAddr_;///<Զ�̵�ַ

			Buffer readBuffer_;  ///<�����ݻ���
			Buffer writeBuffer_;///<д���ݻ���

			ConnectionCallback connCallbackFunc_; ///<�������ӻص�
			MessageCallback msgRecvFunc_;///<���ݽ��ջص�
			WriteCompleteCallback writeCompleteFunc_;///<���ݷ�����ɻص�
			CloseConnCallback closeFunc_; ///<���ӹرջص�

			NetState state_;  ///<����״̬
			std::string connId_;  ///<���ӱ�ʶ��ID
			void* context_; ///<
		private:
			///socket ����д���رջغ���
			void handleRead(Timestamp tm);
			void handleWrite();
			void handleClose();
			void handleError();

			///���ݷ��ͽӿ�
			void sendInReactor(const std::string& data);
			///���ӹرսӿ�
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

			///�������ݽӿ�
			void send(const char* msg, int len);
			///�ⲿǿ�ƶϿ�����
			void forceClose();

			///���ӽ����ص�
			void connEstablished();
			///���ӹر�
			void shutdown();


		};

	}

}//namespace


#endif
