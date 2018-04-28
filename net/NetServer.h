/**
*  @file  NetServer.h
*  @brief ���������
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
	*  @brief ������������
	*/
	class NetServer
	{
	public:
		NetServer(Reactor* loop,
			const char* servIp, short servPort);
		~NetServer();

		typedef std::map<std::string, NetConnection*> ConnectionMap;

	private:
		Reactor* loop_;  ///<�¼�������ѯ
		struct sockaddr_in listenAddr_;///<������������ַ
		Acceptor* acceptor_;  ///<client���ӽ��մ�����

		ConnectionCallback connCallbackFunc_;  ///<�½����ӻص�
		MessageCallback msgCallbackFunc_;  ///<���ݽ��ջص�
		WriteCompleteCallback writeComplFunc_;///<���ݷ�����ɻص�
		ReactorThreadPool* threadPool_; ///<�¼������̳߳أ�Ӧ���ڴ�����������

		int connIndex_;
		ConnectionMap conns_; ///<�������ӹ������

	private:
		///�½�client���ӻص�
		void newConnAccept(int sockfd, const struct sockaddr_in& clientAddr);
		///ɾ�����ӻص�
		void removeConn(const NetConnection* conn);
		void removeConnInLoop(const NetConnection* conn);

	public:
		///�����������ӿ�
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
		///���ü����̳߳��̸߳���
		void setThreadNum(int numThreads);
	};

}

}//namespace

#endif
