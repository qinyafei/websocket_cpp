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
* @brief    开启监听线程池
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
* @brief    取监听线程池下一个监听句柄
* @return Reactor*
* @retval  监听句柄
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
