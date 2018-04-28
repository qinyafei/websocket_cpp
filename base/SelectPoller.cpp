#include "SelectPoller.h"
#include <poll.h>
#include <errno.h>

#include <assert.h>
#include "GlogWrapper.h"
#include "Thread.h"

using namespace ws::base;

SelectPoller::SelectPoller()
{
}


SelectPoller::~SelectPoller()
{
}

/**
* @brief    poll���ĺ���
* @param[out] activeEvents�������¼�����
* @param[in] timeout��poll��ʱʱ��
*/
void SelectPoller::poll(int timeout, EventList& activeEvents)
{
	//
	int evNum = ::poll(&(*pollFds_.begin()), pollFds_.size(), timeout);
	if (evNum > 0)
	{
		findActiveEvHandlers(evNum, activeEvents);
	}
	else if (evNum == 0)
	{
		//nothing happened
		CurrentThread::sleepUsec(1000);
	}
	else
	{
		if (errno != EINTR)
		{
			//error
		}
	}
}


/**
* @brief    ������д������¼�
* @param[out] activeEvents�������¼�����
* @param[in] evNum�������¼�����
*/
void SelectPoller::findActiveEvHandlers(int evNum, EventList& activeEvents)
{
	for (PollFdList::iterator it = pollFds_.begin(); (it != pollFds_.end() && evNum > 0);
		it++)
	{
		if (it->revents > 0)
		{
		    //RAW_LOG(INFO, "poll events happen:fd=%d, events=%d", it->fd, it->revents);

			evNum--;
			EvHandlerMap::iterator evIter = evHandlers_.find(it->fd);
			assert(evIter != evHandlers_.end());

			EventHandler* ev = evIter->second;
			assert(ev != NULL && ev->fd() == it->fd);

			ev->setRevents(it->revents);
			activeEvents.push_back(ev);

		}
	}
}


/**
* @brief    ע������¼�
* @param[in] event�������¼�
*/
void SelectPoller::registerEvHandler(EventHandler* event)
{
	assert(event != NULL);
	//        DLOG(INFO) <<"SelectPoller|registerEvHandler:fd=" <<event->fd() <<"handlers size=" <<evHandlers_.size();
	//not exist
	if (evHandlers_.find(event->fd()) == evHandlers_.end())
	{
		struct pollfd pfd;
		pfd.fd = event->fd();
		pfd.events = event->events();
		pfd.revents = 0;
		pollFds_.push_back(pfd);

		evHandlers_[event->fd()] = event;
	}
	else
	{
		//existing one
		for (PollFdList::iterator it = pollFds_.begin(); it != pollFds_.end(); it++)
		{
			if (it->fd == event->fd())
			{
				struct pollfd& pfd = *it;
				pfd.events = event->events();
				pfd.revents = 0;
				if (event->isNoneEvent())
				{
					//pfd.fd = -event->fd() - 1;
				}
				break;
			}
		}
	}

#if 0
	//for debug
	for (PollFdList::iterator it = pollFds_.begin(); it != pollFds_.end(); it++)
	{
		DLOG(INFO) << "pollfd list:fd=" << it->fd;
	}
#endif
}

/**
* @brief    ɾ�������¼�
* @param[in] event�������¼�
*/
void SelectPoller::removeEvHandler(EventHandler* event)
{
	if (evHandlers_.find(event->fd()) == evHandlers_.end())
	{
		return;
	}

	DLOG(INFO) << "SelectPoller::removeEvHandler";

	evHandlers_.erase(event->fd());

	DLOG(INFO) << "SelectPoller::removeEvHandler 2";

	for (PollFdList::iterator it = pollFds_.begin(); it != pollFds_.end(); it++)
	{
		if (it->fd == event->fd())
		{
			pollFds_.erase(it);
			break;
		}
	}

	DLOG(INFO) << "SelectPoller::removeEvHandler 3";

}
