#ifndef _HEARTBEAT_H_
#define _HEARTBEAT_H_

#include "net/NetClient.h"
#include "base/TimerAlgorithm.h"
#include "base/TimerQueue.h"
#include "base/MutexLock.h"
#include <list>

using namespace ws;
using namespace ws::base;
using namespace ws::net;

class HeartbeatSender
{
public:
	HeartbeatSender(Reactor* loop);
	~HeartbeatSender();

	enum HeartState
	{
		eWaitPing,
		eWaitPong,
		eReconnect,
		eToBeClosed
	};

	class HeartTimer : public TimerAlgorithm
	{
	public:
		Timestamp nextTimestamp();

		void setHeartState(HeartState* hs)
		{
			hs_ = hs;
		}
		
	public:
		HeartState* hs_;
	};


public:
	void processPong();
	void start(NetClient* net);
	void timerHandle();

	void setHeartState(HeartState state)
	{
		LockGuard lock(&mutex_);
		state_ = state;
	}

	HeartState getHeartState()
	{
		return state_;
	}

	void sendHeartbeat();
	void encFrameCallback(unsigned char * data, int len, void* context);
private:
	HeartState state_;
	Reactor* loop_;
	NetClient* net_;
	TimerId timerId_;

	MutexLock mutex_;
};



class HeartbeatRecv
{
public:
	HeartbeatRecv(Reactor* loop);
	~HeartbeatRecv();

public:
	void start();
	int updateConn(NetConnection* conn, const char* data = NULL, int len = 0);
	void timerCallback();

	void encFrameCallback(unsigned char * data, int len, void* context);
private:
	int processPing(NetConnection* conn, const char* data, int len);
private:
	Reactor* loop_;
	TimerId timerId_;

	typedef std::pair<NetConnection*, unsigned int> ConnHeartStruct;
	std::list<ConnHeartStruct> connList_;
};


#endif
