#include "NetClient.h"
#include "SocketOpts.h"
#include "GlogWrapper.h"

#include "NetConnection.h"
#include "Connector.h"
#include <assert.h>

using namespace ws::net;

NetClient::NetClient(Reactor* loop, const char* dstIp, short dstPort)
  :loop_(loop),
   connCallback_(NULL),
   msgCallback_(NULL),
   writeCompleteCallback_(NULL)
{
	connIndex_ = 1;
	conn_ = NULL;
	sockets::makeSockAddr(dstIp, dstPort, &servAddr_);
	connector_ = new Connector(loop, servAddr_);
	connector_->setNewConnectionCallback(std::bind(&NetClient::establishConnection, this, std::placeholders::_1));

}


NetClient::~NetClient()
{
	delete conn_;
	conn_ = NULL;

	delete connector_;
	connector_ = NULL;
}

/**
* @brief    客户端与服务器连接建立，回调
* @param[in] sockfd
*/
void NetClient::establishConnection(int sockfd)
{
	sockaddr_in peeraddr = sockets::getRemoteAddr(sockfd);
	sockaddr_in localaddr = sockets::getLocalAddr(sockfd);

	char ipstr[32] = { 0 };
	sockets::toIp(ipstr, 32, peeraddr);
	uint16_t port = sockets::network2Host16(peeraddr.sin_port);
	const int csmax = 64;
	char buf[csmax] = { 0 };
	snprintf(buf, sizeof(buf), "NetClient|establishConnection %s#%d#%d", ipstr, port, connIndex_++);
	DLOG(INFO) << buf;

	//新建连接
	NetConnection* conn = new NetConnection(loop_,
		buf,
		sockfd,
		localaddr,
		peeraddr);
	conn->setConnCallback(connCallback_);
	conn->setMsgCallback(msgCallback_);
	conn->setWriteCompleteCallback(writeCompleteCallback_);
	conn->setCloseCallback(std::bind(&NetClient::destroyConnection, this, std::placeholders::_1));

	{
		LockGuard lock(&mutex_);
		if (conn_ != NULL)
		{
			delete conn_;
		}
		conn_ = conn;
	}

	conn->connEstablished();

}


/**
* @brief    销毁连接，回调
* @param[in] conn
*/
void NetClient::destroyConnection(const NetConnection* conn)
{
	//assert(loop_->isInLoopThread());
	//loop_->runInLoop(std::bind(&NetConnection::connDestroyed, const_cast<NetConnection*>(conn)));
	//

}

/**
* @brief    断开连接
*/
void NetClient::disconnect()
{
	{
		LockGuard lock(&mutex_);
		if (conn_ != NULL)
		{
			conn_->shutdown();
		}
	}
}

/**
* @brief    发起服务器连接
*/
void NetClient::connect()
{
	if (connector_ != NULL)
	{
		connector_->start();
	}
}
