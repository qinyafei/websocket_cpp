/**
*  @file  NetClient.h
*  @brief  ����client������
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
	*  @brief ����ͻ��˴�����
	*/
	class NetClient : public noncopyable
	{
	public:
		NetClient(Reactor* loop, const char* dstIp, short dstPort);
		~NetClient();

	private:
		///�������ӻص�����
		void establishConnection(int sockfd);
		///�������ӻص�����
		void destroyConnection(const NetConnection* conn);

	private:
		Reactor* loop_;///<�¼�������ѯ
		Connector* connector_;///<client��������
		NetConnection* conn_;///<���Ӷ���
		int connIndex_;

		ConnectionCallback connCallback_;///<client���ӻص�
		MessageCallback msgCallback_;///<���ݽ��ջص�
		WriteCompleteCallback writeCompleteCallback_;///<���ݷ��ͳɹ��ص�
		struct sockaddr_in servAddr_; ///<server addr
		MutexLock mutex_;

		void* context_;///<����������
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
		///���ӷ�����
		void connect();
		///�Ͽ�����
		void disconnect();

	};


}

} //namespace

#endif
