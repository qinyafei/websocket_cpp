#ifndef _UNITEST_H_
#define _UNITEST_H_

#include "WSProtocol.h"

using namespace ws;

//void __stdcall decFrameCallback(unsigned char* data, int len);
//void __stdcall encFrameCallback(unsigned char* data, int len);

void decFrameCallback(unsigned char* data, int len);
void encFrameCallback(unsigned char* data, int len);
void test_protocol();




#endif

