
#include "WSNet.h"
#include <string>
#include <vector>
#include "base/Reactor.h"
#include "base/Buffer.h"
#include <functional>
#include "Utility.h"
#include <assert.h>
#include "NetConnection.h"
#include "log/GlogWrapper.h"

using namespace ws::base;
using namespace ws::net;
using namespace ws;

WSNet::WSNet(std::string url)
{
	bconnected_ = false;
	client_ = NULL;
	loop_ = new ReactorThread();
	Reactor* loop = loop_->startLoop();
	
	std::vector<std::string> vec;
	strsplit(url, "/", vec);
	
	if (vec.size() >= 2)
	{
		std::string addr = vec[2];
		vec.clear();
		strsplit(addr, ":", vec);
		client_ = new NetClient(loop, vec[0].c_str(), atoi(vec[1].c_str()));
	}

	heartbeat_ = new HeartbeatSender(loop);
}


WSNet::~WSNet()
{
	close();
}

void WSNet::connect()
{
	assert(client_ != NULL && loop_ != NULL);
	client_->setConnCallback(std::bind(&WSNet::onConnected, this, std::placeholders::_1));
	client_->setMsgCallback(std::bind(&WSNet::onConnMessage, this, std::placeholders::_1, std::placeholders::_2));
	client_->connect();

}

void WSNet::close()
{
	client_->disconnect();
}

int WSNet::send(const char * data, int len, OpCode payloadType)
{
	while (!bconnected_)
	{
		milliseconds_sleep(100);
	}

	if (bconnected_)
	{
		proto_.encodeFrame((unsigned char*)data, len, payloadType, this, std::bind(&WSNet::encFrameCallback,
			this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}
	else
	{
		LOG(WARNING) << "connection is not connected";
	}

	return 0;
}

void WSNet::onConnected(NetConnection* conn)
{
	LOG(INFO) << "onConnected:id=" << conn->connId() << ",isConnected=" << conn->isConnected();
	bconnected_ = conn->isConnected();

	if (heartbeat_->getHeartState() == HeartbeatSender::eReconnect)
	{
		heartbeat_->setHeartState(HeartbeatSender::eWaitPing);
		heartbeat_->sendHeartbeat();
	}

	if (heartbeat_->getHeartState() == HeartbeatSender::eWaitPing)
	{
		heartbeat_->start(client_);
	}
}


void WSNet::onConnMessage(const NetConnection* conn, Buffer* buf)
{
	int len = buf->readableBytes();
	std::string data = buf->readAll();
	RAW_LOG(INFO, "WSNet::onConnMessage:%s,len=%d", data.c_str(), len);

	proto_.decodeFrame((unsigned char*)data.c_str(), len, (void*)(const_cast<NetConnection*>(conn)), std::bind(&WSNet::decFrameCallback, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

}


void WSNet::encFrameCallback(unsigned char * data, int len, void* context)
{
	RAW_LOG(INFO, "WSNet::encFrameCallback:data=%s, len=%d", data, len);
	assert(client_ != NULL);

	NetConnection* conn = client_->getConn();
	if (conn == NULL)
	{
		return ;
	}

	conn->send((const char*)data, len);
}


void WSNet::decFrameCallback(unsigned char* data, int len, void* context)
{
	RAW_LOG(INFO, "WSNet::decFrameCallback:data=%s, len=%d", data, len);

	NetConnection* conn = (NetConnection*)context;
	if (conn == NULL)
	{
		return;
	}

	const std::string cspong = "PONG";
	if (len == cspong.length())
	{
		std::string pong((char*)data, len);
		if (cspong.compare(pong) == 0)
		{
			heartbeat_->processPong();
			return;
		}
	}

	if (msgRecvCallback_ != NULL)
	{
		msgRecvCallback_(data, len);
	}

}
