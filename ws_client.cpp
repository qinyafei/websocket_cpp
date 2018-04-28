// websocket.cpp : 定义控制台应用程序的入口点。
//

#include "unitest.h"
#include "log/GlogWrapper.h"
#include "websocket/WebSocket.h"
#include "websocket/Utility.h"

#include <signal.h>

using namespace ws;

#define GLOG_FLAG "ws_client.log"


void onclose()
{
	LOG(INFO) << "******client close";
}

void onmsgrecv(unsigned char* data, int len)
{
	LOG(INFO) << "*****onmsgrecv msg:" << data << ",len=" << len;
}

int main()
{
	signal(SIGPIPE, SIG_IGN);

	//log
	GlogWrapper log(GLOG_FLAG);

	const std::string url = "ws://127.0.0.1:1234";
	WebSocket wsk(url);
	wsk.setStateFuncCall(NULL, onclose, NULL, onmsgrecv);

	while (true)
	{
		unsigned char chardata[100];
		memset(chardata, 'a', 100);
		chardata[99] = '\0';
		unsigned char shortdata[460];
		memset(shortdata, 'b', 460);
		shortdata[459] = '\0';
		unsigned char slicedata[2048];
		memset(slicedata, 'c', 2048);

		wsk.sendBlob(chardata, 100);
		LOG(INFO) << "******client send msg:" << (char*)chardata;
		//milliseconds_sleep(5 * 1000);

	    wsk.sendBlob(shortdata, 460);
		LOG(INFO) << "******client send msg:" << shortdata;
		//milliseconds_sleep(10 * 1000);

		wsk.sendBlob(slicedata, 2048);
		LOG(INFO) << "******client send msg:" << slicedata;
		milliseconds_sleep(3600 * 1000);

		RAW_LOG(INFO, "ws_client exit....");
		break;
	}
	//test_protocol();
    return 0;
}

