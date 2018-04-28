
#include "WebSocketServer.h"
#include <functional>
#include "net/NetServer.h"
#include "log/GlogWrapper.h"
#include "net/NetConnection.h"

#include "base/Timestamp.h"

using namespace ws;
using namespace ws::net;


NetServer* WebSocketServer::server_ = NULL;

WebSocketServer::WebSocketServer(Reactor* loop, std::string ip, int port)
{
	loop_ = loop;

	server_ = new NetServer(loop, ip.c_str(), port);
	server_->setThreadNum(1);
	server_->setConnCallback(std::bind(&WebSocketServer::onConnNew, this, std::placeholders::_1));
	server_->setMsgCallback(std::bind(&WebSocketServer::onConnMessage, this, 
		std::placeholders::_1, std::placeholders::_2));
	server_->start();

	heartbeat_ = new HeartbeatRecv(loop);
	heartbeat_->start();
}


WebSocketServer::~WebSocketServer()
{
	if (server_ != NULL)
	{
		delete server_;
		server_ = NULL;
	}

	if (heartbeat_ != NULL)
	{
		delete heartbeat_;
		heartbeat_ = NULL;
	}
}


void WebSocketServer::onConnNew(NetConnection * conn)
{
	LOG(INFO) << "onConnNew:id=" << conn->connId() << ",isConnected=" << conn->isConnected();

	if (heartbeat_ != NULL)
	{
		heartbeat_->updateConn(conn);
	}

}


void WebSocketServer::onConnMessage(const NetConnection * conn, Buffer * buf)
{
	int len = buf->readableBytes();
	std::string data = buf->readAll();
	RAW_LOG(INFO, "onConnMessage:%s,len=%d", data.c_str(), len);

	WSProtocol proto;
	proto.decodeFrame((unsigned char*)data.c_str(), len, (void*)(conn), std::bind(&WebSocketServer::decFrameCallback, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}


void WebSocketServer::send(std::string connid, const char* data, int len)
{
	NetConnection* conn = server_->getconn(connid);
	if (conn == NULL)
	{
		return;
	}

	WSProtocol proto;
	proto.encodeFrame((unsigned char*)data, len, eTextFrame, conn, std::bind(&WebSocketServer::encFrameCallback,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}



void WebSocketServer::decFrameCallback(unsigned char* data, int len, void* context)
{
	NetConnection* conn = (NetConnection*)context;
	if (conn == NULL)
	{
		return;
	}

	if (heartbeat_ != NULL)
	{
		int ret = heartbeat_->updateConn(conn, (char*)data, len);
		if (ret != 0)
		{
			return;
		}
	}

	
	if (onConnMsgFunc_ != NULL)
	{
		onConnMsgFunc_(conn->connId(), (const char*)data, len);
	}
}

void WebSocketServer::encFrameCallback(unsigned char * data, int len, void* context)
{
	RAW_LOG(INFO, "WebSocketServer::encFrameCallback:data=%s, len=%d", data, len);

	NetConnection* conn = (NetConnection*)context;;
	if (conn == NULL)
	{
		return;
	}

	conn->send((const char*)data, len);
}