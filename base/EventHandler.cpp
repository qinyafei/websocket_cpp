#include "EventHandler.h"

#include "Reactor.h"
#include <errno.h>
#include <poll.h>
#include "GlogWrapper.h"

using namespace ws::base;

//事件
const int EventHandler::csEventNone = 0;
const int EventHandler::csEventRead = POLLIN | POLLPRI;
const int EventHandler::csEventWrite = POLLOUT;



EventHandler::EventHandler(Reactor* reactor, int fd)
	:preactor_(reactor),
	fd_(fd),
	events_(0),
	revents_(0)
{
  
}


EventHandler::~EventHandler()
{
	DLOG(INFO) << "EventHandler::~EventHandler";

	disableAll();
	remove();
}


void EventHandler::update()
{
	preactor_->registerEvHandler(this);
}

void EventHandler::remove()
{
  preactor_->removeEvHandler(this);
}


/**
* @brief    处理监听事件
*   可读，可写，关闭，错误事件处理
*   不可主调用，由poll调用
*/
void EventHandler::handEvent()
{
	//DLOG(INFO) << "EventHandler|handEvent，fd=" << fd_ << " revents=" << revents_ << " Reactor=" << preactor_;

	if ((revents_ & POLLHUP) && !(revents_ & POLLIN))
	{
		if (closeCallback_ != NULL)
			closeCallback_();
	}

	if (revents_ & POLLNVAL)
	{

	}

	if (revents_ & (POLLERR | POLLNVAL))
	{
		if (errorCallback_)
			errorCallback_();
	}

	if (revents_ & (POLLIN | POLLPRI | POLLRDHUP))
	{
		if (readCallback_)
			readCallback_(Timestamp::now());
	}
	if (revents_ & POLLOUT)
	{
		if (writeCallback_)
			writeCallback_();
		RAW_LOG(INFO, "handEvent|write events=%d", revents_);
	}
}
