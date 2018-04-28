#ifndef _WEBSOCKETSERVER_H_
#define _WEBSOCKETSERVER_H_

#include "net/NetServer.h"
#include "base/Buffer.h"
#include "base/Reactor.h"
#include "GlobalDef.h"
#include "WSProtocol.h"
#include "Heartbeat.h"

using namespace ws::base;
using namespace ws::net;

namespace ws
{

class WebSocketServer
{
public:
	WebSocketServer(Reactor* loop, std::string ip, int port);
	~WebSocketServer();

public:
	void setOnConnMsgFunc(OnConnMsgFunc func)
	{
		onConnMsgFunc_ = func;
	}

	static void send(std::string connid, const char* data, int len);

public:
	static NetServer* server_;
private:
	void onConnNew(NetConnection* conn);
	void onConnMessage(const NetConnection* conn, Buffer* buf);

	static void encFrameCallback(unsigned char* data, int len, void* context);
	void decFrameCallback(unsigned char* data, int len, void* context);

private:
	Reactor* loop_;
	OnConnMsgFunc onConnMsgFunc_;
	HeartbeatRecv* heartbeat_;

};


}


#endif