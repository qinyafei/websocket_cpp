// websocket.cpp : 定义控制台应用程序的入口点。
//

#include "unitest.h"
#include "log/GlogWrapper.h"
#include "websocket/WebSocketServer.h"
#include "base/Reactor.h"
#include "websocket/Utility.h"
#include <string>

#include <iostream>
#include <signal.h>

using namespace ws;

#define GLOG_FLAG "ws_server.log"

void servMsgRecv(std::string connId, const char* msg, int len)
{
	RAW_LOG(INFO, "#########servMsgRecv:connid=%s,msg=%s,len=%d", connId.c_str(), msg,len);

	unsigned char slicedata[2048];
	memset(slicedata, 'd', 2048);
	WebSocketServer::send(connId, (char*)slicedata, 2048);
}

int main()
{
	signal(SIGPIPE, SIG_IGN);
	//log
	GlogWrapper log(GLOG_FLAG);

	Reactor loop;
	WebSocketServer server(&loop, "127.0.0.1", 1234);
	server.setOnConnMsgFunc(servMsgRecv);
	
	loop.loop();
	//test_protocol();
    return 0;
}

