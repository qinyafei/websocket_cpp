
#include "WebSocket.h"
#include <assert.h>

using namespace ws;

//ws://xxx.xxx.xxx.xxx:8008
WebSocket::WebSocket(std::string url)
{
	net_ = new WSNet(url.c_str());
	net_->connect();
}


WebSocket::~WebSocket()
{
}

int WebSocket::close(int code, std::string reason)
{
	return 0;
}


int WebSocket::sendBlob(unsigned char* payload, int len)
{
	assert(net_ != NULL);

	int ret = net_->send((const char*)payload, len);

	return 0;
}


int WebSocket::sendString(std::string cpayload)
{
	assert(net_ != NULL);

	int ret = net_->send(cpayload.c_str(), cpayload.length());

	return ret;
}
