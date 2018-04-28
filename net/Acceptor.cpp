#include "Acceptor.h"
#include "GlogWrapper.h"


using namespace ws::net;

Acceptor::Acceptor(Reactor* loop, const struct sockaddr_in& addr)
{
	loop_ = loop;
	listenfd_ = sockets::createNonblockTcpSock();
	event_ = new EventHandler(loop, listenfd_);
	event_->setReadCallback(std::bind(&Acceptor::handleRead, this));

	sockets::setReuseAddr(listenfd_, 1);
	sockets::setReusePort(listenfd_, 1);
	int ret = sockets::bind(listenfd_, &addr);

	DLOG(ERROR) << "Acceptor::Acceptor,bind err=" << ret << " listenfd=" << listenfd_;

}


Acceptor::~Acceptor()
{
	DLOG(INFO) << "Acceptor::~Acceptor";

	delete event_;
	event_ = NULL;
}

/**
* @brief    接收client链接请求，调用新进链接回调
*/
void Acceptor::handleRead()
{
	struct sockaddr_in peerAddr;
	int connfd = sockets::accept(listenfd_, &peerAddr);
	if (connfd > 0)
	{
		if (acceptConnFunc_ != NULL)
		{
			acceptConnFunc_(connfd, peerAddr);
		}
		else
		{
			::close(connfd);
		}
	}
	else
	{
		if (errno == EMFILE)
		{

		}
	}
}

/**
* @brief    监听远程连接
*/
void Acceptor::listen()
{ 
	int ret = sockets::listen(listenfd_);
	DLOG(INFO) << "Acceptor::listen ret=" << ret <<" listenfd=" << listenfd_;
	event_->enableRead();
}
