#ifndef _WSPROTOCOL_H_
#define _WSPROTOCOL_H_

#include <functional>

struct WSHead
{
	unsigned char FIN : 1;
	unsigned char RSV : 3;
	unsigned char Opcode : 4;
	unsigned char mask : 1;
	unsigned char PayloadLen : 7;
};

enum OpCode 
{
	eContinueFrame = 0x0,
	eTextFrame = 0x1,
	eBlobFrame = 0x2,
	eCloseFrame = 0x8,
	ePingFrame = 0x9,
	ePongFrame = 0xA,
};

#define MASK_CODE 0x80200802


namespace ws
{

	/** @class WSProtocol
	*  @brief websocket 数据帧格式类
	*  数据帧的封包，分片，组包操作
	*/
	class WSProtocol
	{
	public:
		WSProtocol();
		~WSProtocol();

		typedef std::function<void(unsigned char*, int, void*)> WSFrameFunc;

	public:
		///计算掩码
		void computeMask(unsigned char* data, int len);
		void computeUnMask(unsigned char* data, int len);
		///
		int decodeFrame(unsigned char* data, int len, void* context, WSFrameFunc func);
		///
		int encodeFrame(unsigned char* payload, int len, OpCode type, void* context, WSFrameFunc func);

	private:
		unsigned char* frameBuff_;
		int framePos_;

	};


}


#endif
