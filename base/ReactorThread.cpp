#include "ReactorThread.h"


using namespace ws::base;

ReactorThread::ReactorThread()
	:loop_(NULL),
	mutex_(),
	wait_(mutex_),
	 thread_(std::bind(&ReactorThread::threadProc, this))
{
}


ReactorThread::~ReactorThread()
{
	if (loop_ != NULL)
	{
		loop_->quit();
		thread_.join();
	}

	loop_ = NULL;
}


/**
* @brief    开启事件监听轮询
* @return Reactor*
* @retval  返回给外部调用者监听句柄
*/
Reactor* ReactorThread::startLoop()
{
	thread_.start();

	{
		///等待监听线程执行，返回有效的监听句柄
		LockGuard lock(&mutex_);
		while (loop_ == NULL)
		{
			wait_.wait();
		}
	}

	return loop_;
}


/**
* @brief    事件监听线程，阻塞不退出
*/
void ReactorThread::threadProc()
{
	Reactor loop;
	{
		LockGuard lock(&mutex_);
		loop_ = &loop;
		wait_.notify();
	}

	//阻塞
	loop.loop();
	//exit
	printf("***thread loop exit \n");
}
