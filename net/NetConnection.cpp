#include "NetConnection.h"
#include <string>
#include "SocketOpts.h"
#include "GlogWrapper.h"

using namespace ws::net;

NetConnection::NetConnection(Reactor* loop, std::string connid, int sockfd, const sockaddr_in& localAddr,
	const sockaddr_in& peerAddr)
{
	loop_ = loop;
	connId_ = connid;
	event_ = new EventHandler(loop, sockfd);
	memcpy(&localAddr_, &localAddr, sizeof(sockaddr_in));
	memcpy(&remoteAddr_, &peerAddr, sizeof(sockaddr_in));

	state_ = eDisconnected;

	event_->setReadCallback(std::bind(&NetConnection::handleRead, this, std::placeholders::_1));
	event_->setWriteCallback(std::bind(&NetConnection::handleWrite, this));
	event_->setWriteCallback(std::bind(&NetConnection::handleClose, this));
	event_->setWriteCallback(std::bind(&NetConnection::handleError, this));

}


NetConnection::~NetConnection()
{
	//event_
	forceClose();
}


/**
* @brief    socket 读数据回调
*/
void NetConnection::handleRead(Timestamp tm)
{
	int err = 0;
	//TODO:
	readBuffer_.reset();
	int n = readBuffer_.readfd(event_->fd(), &err);
	if (n > 0)
	{
		DLOG(INFO) << "**NetConnection::handleRead,msgRecvfunc(),len=" << readBuffer_.readableBytes() << 
			",msg=" << readBuffer_.peekRead();
		if (msgRecvFunc_ != nullptr)
			msgRecvFunc_(this, &readBuffer_);
	}
	else if (n == 0)
	{
		handleClose();
	}
	else
	{
		LOG(ERROR) << "NetConnection::handleRead: n=" << n << " errno=" << errno;
		errno = err;
		handleError();
	}

}

/**
* @brief    socket 写数据回调，当write繁忙，未及时write时
*         回调会在可写的时候，写数据
*/
void NetConnection::handleWrite()
{
	DLOG(INFO) << "NetConnection::handleWrite";

	int writelen = sockets::write(event_->fd(), static_cast<void*>(const_cast<char*>(writeBuffer_.peekRead())),
		writeBuffer_.readableBytes());
	if (writelen > 0)
	{
		writeBuffer_.retrieve(writelen);
		if (writeBuffer_.readableBytes() == 0)
		{
			event_->disableWrite();
			if (writeCompleteFunc_ != NULL)
			{
				writeCompleteFunc_(this);
			}
		}
	}
	else
	{
		assert(0);
	}

	DLOG(INFO) << "NetConnection::handleWrite:writebuf size=" << writeBuffer_.capacity();
}


/**
* @brief    链接关闭回调
*/
void NetConnection::handleClose()
{
	RAW_LOG(INFO, "**NetConnection::handleClose:fd=%d", event_->fd());

	state_ = eDisconnected;
	if (connCallbackFunc_ != nullptr)
	{
		connCallbackFunc_(this);
	}

	if (closeFunc_ != NULL)
	{
		closeFunc_(this);
	}

	delete event_;
	event_ = NULL;

}

/**
* @brief    链接出错回调
*/
void NetConnection::handleError()
{
  RAW_LOG(ERROR, "**NecConnection|handleError:%d", event_->fd());

  int err = sockets::getsockError(event_->fd());

}

/**
* @brief    外部接口，发送数据
* @param[in] msg：数据指针
* @param[in] len：数据长度
*/
void NetConnection::send(const char* msg, int len)
{
	if (state_ == eConnected)
	{
		///如果是同一线程内部，直接发送
		if (loop_->isInLoopThread())
		{
			return sendInReactor(std::string(msg, len));
		}
		else
		{
			loop_->runInLoop(std::bind(&NetConnection::sendInReactor, this, std::string(msg, len)));
		}
	}
	else
	{
		LOG(WARNING) << "NetConnection|send connection disconnected";
	}
}

/**
* @brief    内部接口，在事件监听线程内部，发送，
*         避免多线程对同一socket写操作冲突，如果write失败，存入写缓存
* @param[in] data：数据指针
* @param[in] len：数据长度
*/
void NetConnection::sendInReactor(const std::string& data)
{
	if (!loop_->isInLoopThread())
	{
		//assert(0);
	}

	bool bret = false;
	if (state_ == eDisconnected)
	{
		LOG(ERROR) << "NetConnection::sendInReactor:state = eDisconnected...";
		return;
	}

	int nowwrite = 0;
	int remain = 0;
	//可读缓冲为0且未有写动作正在执行，直接write
	if (/*!event_->isWriteable() && */writeBuffer_.readableBytes() == 0)
	{
		nowwrite = sockets::write(event_->fd(), const_cast<char*>(data.data()), data.length());
		if (nowwrite >= 0)
		{
			DLOG(INFO) << "NetConnection::sendInReactor 2 wlen=" << nowwrite;

			remain = data.length() - nowwrite;
			if (remain == 0 && writeCompleteFunc_ != NULL)
			{
				writeCompleteFunc_(this);
			}
		}
		else //nowwrite < 0
		{
			nowwrite = 0;
			if (errno != EWOULDBLOCK)
			{
				if (errno == EPIPE || errno == ECONNRESET)
				{
					bret = true;
				}
			}
		}
	}

	//未完全write所有数据，缓存到写buffer里，当可写事件触发，重发
	if (!bret && remain > 0)
	{
		DLOG(INFO) << "NetConnection::sendInReactor 3 remain=" << remain;

		int oldlen = writeBuffer_.readableBytes();
		writeBuffer_.write(static_cast<const char*>(data.data())+nowwrite, remain);
		if (!event_->isWriteable())
		{
			DLOG(WARNING) << "NetConnection|sendInReactor:remain=" << remain << ",nowwrite=" << nowwrite;
			event_->enableWrite();
		}
	}

}

/**
* @brief    外部接口，强制断开连接
*/
void NetConnection::forceClose()
{
	if (state_ == eConnected)
	{
		state_ = eDisconnected;
		loop_->runInLoop(std::bind(&NetConnection::handleClose, this));
	}
}

/**
* @brief    链接建立回调函数
*/
void NetConnection::connEstablished()
{
	state_ = eConnected;
	if (connCallbackFunc_ != NULL)
	{
		connCallbackFunc_(this);
	}
	event_->enableRead();
	//event_->enableWrite();

	DLOG(INFO) << "##NetConnection::connEstablished,port=" << sockets::network2Host16(remoteAddr_.sin_port);
}


/**
* @brief    关闭连接
*/
void NetConnection::shutdown()
{
	if (state_ == eConnected)
	{
		loop_->runInLoop(std::bind(&NetConnection::shutdownInReactor, this));
	}
}

void NetConnection::shutdownInReactor()
{
	if (!event_->isWriteable())
	{
		sockets::shutdownWrite(event_->fd());
	}
}
