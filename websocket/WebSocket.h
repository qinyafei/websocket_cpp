#ifndef _WEBSOCKET_H_
#define _WEBSOCKET_H_

#include <string>
#include <functional>
#include "WSNet.h"


namespace ws
{

	/** @class WebSocket
	*  @brief websocket client�����ӿ���
	*   ����ws������payload�����ջظ����ر�ws
	*/
class WebSocket
{
public:
	WebSocket(std::string url);
	~WebSocket();

	enum WSState {
		eConnecting = 1,
		eOpen,
		eClosing,
		eClosed
	};

public:
	int close(int code, std::string reason);
	int sendBlob(unsigned char* payload, int len);
	int sendString(std::string cpayload);

	void setStateFuncCall(OnOpenFunc openCall, OnCloseFunc closeCall,
		OnErrorFunc errorCall, OnMessageFunc msgCall)
	{
		onOpen_ = openCall;
		onClose_ = closeCall;
		onError_ = errorCall;
		onMessage_ = msgCall;

		net_->setMsgRecvCallback(onMessage_);
	}

private:
	WSNet* net_;

	OnOpenFunc onOpen_;
	OnCloseFunc onClose_;
	OnErrorFunc onError_;
	OnMessageFunc onMessage_;

};



}

#endif