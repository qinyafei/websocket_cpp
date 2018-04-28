#include "NetServer.h"

#include "SocketOpts.h"
#include "Acceptor.h"
#include "NetConnection.h"
#include "GlogWrapper.h"

using namespace ws::net;

NetServer::NetServer(Reactor* loop,
	const char* servIp, short servPort)
{
	loop_ = loop;
	connIndex_ = 0;

	sockets::makeSockAddr(servIp, servPort, &listenAddr_);

	acceptor_ = new Acceptor(loop, listenAddr_);
	threadPool_ = new ReactorThreadPool(loop);
	acceptor_->setAcceptConnCallback(std::bind(&NetServer::newConnAccept,
		this, std::placeholders::_1,
		std::placeholders::_2));

	DLOG(INFO) << "NetServer|NetServer servIp=" << servIp << " servPort=" << servPort;
}


NetServer::~NetServer()
{
	DLOG(INFO) << "NetServer::~NetServer";

	delete acceptor_;
	acceptor_ = NULL;

	DLOG(INFO) << "NetServer::~NetServer 2";

	delete threadPool_;
	threadPool_ = NULL;

	DLOG(INFO) << "NetServer::~NetServer 3";

	ConnectionMap::iterator it;
	for (it = conns_.begin(); it != conns_.end();)
	{
		ConnectionMap::iterator restore = it;
		it++;
		delete restore->second;
	}

	DLOG(INFO) << "NetServer::~NetServer 4";
}


void NetServer::setThreadNum(int numThreads)
{
	assert(numThreads > 0);
	threadPool_->setThreadNum(numThreads);
}

/**
* @brief    acceptor 新接收client链接回调
* @param[in] sockfd：
* @param[in] clientAddr：新建连接客户端地址
*/
void NetServer::newConnAccept(int sockfd, const struct sockaddr_in& clientAddr)
{
    //取一个IO事件监听线程
	Reactor* ioReactor = threadPool_->nextLoop();

	char ipstr[32] = { 0 };
	sockets::toIp(ipstr, 32, clientAddr);
	uint16_t port = sockets::network2Host16(clientAddr.sin_port);

	const int csmax = 64;
	char buf[csmax] = { 0 };
	snprintf(buf, sizeof(buf), "%s#%d#%d", ipstr, port, connIndex_);
	struct sockaddr_in local = sockets::getLocalAddr(sockfd);

	RAW_LOG(INFO, "##NetServer|newConnAccept new connection accept:%s", buf);

	//新建连接
	NetConnection* conn = new NetConnection(ioReactor, buf, sockfd, local, clientAddr);
    conn->setConnCallback(connCallbackFunc_);
	conn->setMsgCallback(msgCallbackFunc_);
	conn->setWriteCompleteCallback(writeComplFunc_);
	conn->setCloseCallback(std::bind(&NetServer::removeConn, this, std::placeholders::_1));

	conns_[buf] = conn;

	//回调给上层新建的链接
	ioReactor->runInLoop(std::bind(&NetConnection::connEstablished, conn));
              
}


/**
* @brief    删除链接
* @param[in] conn：
*/
void NetServer::removeConn(const NetConnection* conn)
{
	loop_->runInLoop(std::bind(&NetServer::removeConnInLoop, this, conn));

}

void NetServer::removeConnInLoop(const NetConnection* conn)
{
	int n = conns_.erase(conn->connId());
	//Reactor* ioLoop = conn->getLoop();
	//ioLoop->runInLoop(std::bind(&NetConnection::connDestroyed, const_cast<NetConnection*>(conn)));
}

/**
* @brief    服务器启动监听
*/
void NetServer::start()
{
	assert(loop_ != NULL);

	threadPool_->start();
	loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_));
}


NetConnection* NetServer::getconn(std::string connid)
{
	if (conns_.size() <= 0)
	{
		return NULL;
	}
	
	return conns_[connid];
}