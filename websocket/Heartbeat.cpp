#include "Heartbeat.h"

#include "base/Timestamp.h"
#include "net/NetConnection.h"
#include "WSProtocol.h"
#include "log/GlogWrapper.h"

using namespace ws;
using namespace ws::net;


HeartbeatSender::HeartbeatSender(Reactor* loop)
{
	loop_ = loop;
	state_ = eWaitPing;
}


HeartbeatSender::~HeartbeatSender()
{
}

void HeartbeatSender::processPong()
{
	setHeartState(eWaitPing);
}

void HeartbeatSender::start(NetClient* net)
{
	assert(net != NULL && loop_ != NULL);
    
	net_ = net;

	sendHeartbeat();
	setHeartState(eWaitPong);

	HeartTimer* algor = new HeartTimer();
	algor->setHeartState(&state_);
	Timestamp tsp = algor->nextTimestamp();

	Timer* timeout = new Timer(std::bind(&HeartbeatSender::timerHandle, this), tsp, true);
	timeout->setContext(static_cast<void*>(this));
	timeout->setTimerAlgorithm(algor);

	timerId_ = loop_->runAt(timeout);
}

void HeartbeatSender::timerHandle()
{
	RAW_LOG(INFO, "HeartbeatSender::timerHandle:state=%d", state_);

	if (state_ == eWaitPing)
	{
		sendHeartbeat();
		setHeartState(eWaitPong);
	}
	else if (state_ == eWaitPong)
	{
		//WSNet* net = static_cast<WSNet*>(net_);
		net_->connect();
		setHeartState(eReconnect);
	}
	else if (state_ == eReconnect)
	{
		//now = addSeconds(now, ReconnectTimeout);
	}
	else if (state_ == eToBeClosed)
	{

	}
	else
	{

	}
}


void HeartbeatSender::sendHeartbeat()
{
	WSProtocol proto;
	std::string ping = "PING";
	proto.encodeFrame((unsigned char*)ping.c_str(), ping.length(),
		ePingFrame, net_, std::bind(&HeartbeatSender::encFrameCallback,
		this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void HeartbeatSender::encFrameCallback(unsigned char * data, int len, void* context)
{
	RAW_LOG(INFO, "HeartbeatSender::encFrameCallback:data=%s, len=%d", data, len);

	NetClient* conn = (NetClient*)context;
	if (conn == NULL)
	{
		return;
	}

	conn->getConn()->send((const char*)data, len);
}


Timestamp HeartbeatSender::HeartTimer::nextTimestamp()
{
	//ping interval must < waitpong
	int PingTimeInterval = 10;
	int WaitPongTimeout = 5;
	int ReconnectTimeout = 8;

	Timestamp now = Timestamp::now();
	if (*hs_ == eWaitPing)
	{
		//
		now = addSeconds(now, PingTimeInterval);
	}
	else if (*hs_ == eWaitPong)
	{
		now = addSeconds(now, WaitPongTimeout);
	}
	else if (*hs_ == eReconnect)
	{
		now = addSeconds(now, ReconnectTimeout);
	}
	else if (*hs_ == eToBeClosed)
	{

	}
	else
	{

	}

	return now;
}

//-----------------------------------

HeartbeatRecv::HeartbeatRecv(Reactor* loop)
{
	loop_ = loop;
}

HeartbeatRecv::~HeartbeatRecv()
{
	if (loop_ != NULL)
	{
		loop_->cancelTimer(timerId_);
	}

	connList_.clear();
}


void HeartbeatRecv::start()
{
	assert(loop_ != NULL);

	const double interval = 1.0;
	timerId_ = loop_->runEvery(interval, std::bind(&HeartbeatRecv::timerCallback, this));
}

int HeartbeatRecv::updateConn(NetConnection* conn, const char* data, int len)
{
	RAW_LOG(INFO, "HeartbeatRecv::updateConn:data=%s, len=%d", data, len);
	
	std::list<ConnHeartStruct>::iterator it;
	Timestamp now = Timestamp::now();
	for (it = connList_.begin(); it != connList_.end(); it++)
	{
		if (it->first == conn)
		{
			it->second = now.microsecondsSinceEpoch() / Timestamp::csMicrosecondsPerSecond;
			return processPing(conn, data, len);
		}
	}

	ConnHeartStruct info(conn, now.microsecondsSinceEpoch() / Timestamp::csMicrosecondsPerSecond);
	connList_.push_back(info);

	return processPing(conn, data, len);
}

void HeartbeatRecv::timerCallback()
{
	const int timeout = 10 + 5;

	std::list<ConnHeartStruct>::iterator it;
	Timestamp now = Timestamp::now();
	for (it = connList_.begin(); it != connList_.end(); it++)
	{
		if (now.microsecondsSinceEpoch() / Timestamp::csMicrosecondsPerSecond - it->second > timeout)
		{
			RAW_LOG(INFO, "HeartbeatRecv::timerCallback,heartbeat timeout,force close peer");
			//超时强制断开
			it->first->forceClose();
			it = connList_.erase(it);
			it--;
		}
	}
}

int HeartbeatRecv::processPing(NetConnection* conn, const char* data, int len)
{
	int ret = 0;
	const std::string csping = "PING";
	if (len == csping.length())
	{
		std::string ping((char*)data, len);
		if (csping.compare(ping) == 0)
		{
			WSProtocol proto;
			std::string pong = "PONG";
			proto.encodeFrame((unsigned char*)pong.c_str(), pong.length(),
				ePongFrame, conn, std::bind(&HeartbeatRecv::encFrameCallback,
					this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

			ret = 1;
		}
	}

	return ret;
}


void HeartbeatRecv::encFrameCallback(unsigned char * data, int len, void* context)
{
	RAW_LOG(INFO, "HeartbeatRecv::encFrameCallback:data=%s, len=%d", data, len);

	NetConnection* conn = (NetConnection*)context;
	if (conn == NULL)
	{
		return;
	}

	conn->send((const char*)data, len);
}