#ifndef _WSNET_H_
#define _WSNET_H_

#include "base/ReactorThread.h"
#include "net/NetClient.h"
#include "GlobalDef.h"
#include "WSProtocol.h"

#include "Heartbeat.h"

using namespace ws::net;
using namespace ws::base;

namespace ws
{


class WSNet
{
public:
	WSNet(std::string url);
	~WSNet();

public:
	void connect();
	void close();

	int send(const char* data, int len, OpCode payloadType = eTextFrame);

	void onConnected(NetConnection* conn);
	void onConnMessage(const NetConnection* conn, Buffer* buf);

	void encFrameCallback(unsigned char* data, int len, void* context);
	void decFrameCallback(unsigned char* data, int len, void* context);

	void setMsgRecvCallback(const OnMessageFunc& func)
	{
		msgRecvCallback_ = func;
	}
private:
	WSProtocol proto_;
	ReactorThread* loop_;
	NetClient* client_;
	OnMessageFunc msgRecvCallback_;
	bool bconnected_;

	HeartbeatSender* heartbeat_;
};


}

#endif