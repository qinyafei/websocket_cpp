#include "ReactorThreadPool.h"

#include "Reactor.h"
#include "ReactorThread.h"

using namespace ws::base;

ReactorThreadPool::ReactorThreadPool(Reactor* base)
{
	baseLoop_ = base;
	numThreads_ = 0;
	index_ = 0;
}


ReactorThreadPool::~ReactorThreadPool()
{
}


/**
* @brief    ���������̳߳�
*/
void ReactorThreadPool::start()
{
	for (int i = 0; i < numThreads_; i++)
	{
		ReactorThread* thrd = new ReactorThread();
		threads_.push_back(thrd);
		loops_.push_back(thrd->startLoop());
	}
}


/**
* @brief    ȡ�����̳߳���һ���������
* @return Reactor*
* @retval  �������
*/
Reactor* ReactorThreadPool::nextLoop()
{
	Reactor* loop = baseLoop_;
	if (!loops_.empty())
	{
		//round poll
		loop = loops_[index_];
		index_++;
		if (index_ >= loops_.size())
		{
			index_ = 0;
		}
	}

	return loop;
}
