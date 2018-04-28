#include "Connector.h"

#include <algorithm>
#include "SocketOpts.h"
#include "GlogWrapper.h"

using namespace ws::net;

const double Connector::csInitRetryDelay = 5000;
const double Connector::csMaxRetryDelay = 30*1000;


Connector::Connector(Reactor* loop, const sockaddr_in& addr)
{
	loop_ = loop;
	memcpy(&servAddr_, &addr, sizeof(sockaddr_in));
	state_ = eDisconnected;
	event_ = NULL;
	retryDelayMs_ = csInitRetryDelay;
}


Connector::~Connector()
{
}

/**
* @brief    链接建立回调函数
*/
void Connector::handleWrite()
{
	int sockfd = resetEvent();

	int err = sockets::getsockError(sockfd);
	if (err != 0)
	{
		//
		DLOG(INFO) << "##connector::handlewrite,retry,port=" << sockets::network2Host16(servAddr_.sin_port);
		retry(sockfd);
	}
	else
	{
		DLOG(INFO) << "connect successfully,port=" << sockets::network2Host16(servAddr_.sin_port);
		state_ = eConnected;
		newConnCreateCallback_(sockfd);

		if (retryTimerId_.tm_ != NULL)
		{
			loop_->cancelTimer(retryTimerId_);
		}
	}

	DLOG(INFO) << "Connector::handleWrite end";

}

/**
* @brief    处理错误
*/
void Connector::handleError()
{
  RAW_LOG(WARNING, "Connector::handleError");
}


/**
* @brief    链接重置
*/
int Connector::resetEvent()
{
	if (event_ == NULL)
	{
		return -1;
	}

	int sockfd = event_->fd();
	//in the EventHandler::handEvent
	loop_->runInLoop(std::bind(&Connector::resetInLoop, this));

	DLOG(INFO) << "##Connector::resetEvent fd=" << sockfd;

	return sockfd;
}


void Connector::resetInLoop()
{
	delete event_;
	event_ = NULL;
}


void Connector::start()
{
	assert(loop_ != NULL);
	loop_->runInLoop(std::bind(&Connector::startInLoop, this));
}

/**
* @brief   线程内部发起远程连接
*/
void Connector::startInLoop()
{
	assert(state_ == eDisconnected);
	connect();
}

/**
* @brief    建立链接
*/
void Connector::connect()
{
	int sockfd = sockets::createNonblockTcpSock();
	int ret = sockets::connect(sockfd, servAddr_);
	int err = (ret == 0) ? 0 : errno;
	switch (err)
	{
	case 0:
	case EINPROGRESS:
	case EISCONN:
		LOG(WARNING) << "Connector::connectImpl:err=" << err << " port=" << sockets::network2Host16(servAddr_.sin_port);
		connectImpl(sockfd);
		break;
	case EAGAIN:
	case EADDRINUSE:
	case EADDRNOTAVAIL:
	case ECONNREFUSED:
	  {
		  LOG(WARNING) << "Connector::retry:err=" << err << "port=" << sockets::network2Host16(servAddr_.sin_port);
		retry(sockfd);
		break;
	  }
	case EACCES:
	case EPERM:
	case EAFNOSUPPORT:
	case EALREADY:
	case EBADF:
	case EFAULT:
	case ENOTSOCK:

		break;
	default:
		break;
	}

}

/**
* @brief    重连，每次间隔时差倍增
*/
void Connector::retry(int sockfd)
{
	::close(sockfd);
	state_ = eDisconnected;

	if (retryTimerId_.tm_ != NULL)
	{
		loop_->cancelTimer(retryTimerId_);
	}


	return;
	
	retryTimerId_ = loop_->runDelay(retryDelayMs_ / 1000.0, std::bind(&Connector::startInLoop, this));
	retryDelayMs_ = std::min(retryDelayMs_ * 2, csMaxRetryDelay);

}

void Connector::connectImpl(int sockfd)
{
	if (event_ == NULL)
	{
		event_ = new EventHandler(loop_, sockfd);
		event_->setWriteCallback(std::bind(&Connector::handleWrite, this));
		//event_->setErrorCallback()
		event_->enableWrite();
	}
}

void Connector::stop()
{
	resetEvent();
}
