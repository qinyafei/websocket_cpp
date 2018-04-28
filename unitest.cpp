
#include "unitest.h"
#include <functional>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "websocket/WSProtocol.h"

using namespace ws;

void  decFrameCallback(unsigned char* data, int len, void* context)
{
	std::cout << "decframe,data=" << data << "len=" << len << std::endl;
}


WSProtocol decProto;
void  encFrameCallback(unsigned char* data, int len, void* context)
{
	decProto.decodeFrame(data, len, context, decFrameCallback);
}


void test_protocol()
{
	//prepare for test data
	unsigned char chardata[100];
	memset(chardata, 2, 100);
	unsigned char shortdata[460];
	memset(shortdata, 3, 460);
	unsigned char slicedata[2048];
	memset(slicedata, 4, 2048);

	WSProtocol proto;
	proto.encodeFrame(chardata, 100, eTextFrame, NULL, encFrameCallback);
	proto.encodeFrame(shortdata, 460, eTextFrame, NULL, encFrameCallback);
	proto.encodeFrame(slicedata, 2048, eTextFrame, NULL, encFrameCallback);


}