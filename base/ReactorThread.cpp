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
* @brief    �����¼�������ѯ
* @return Reactor*
* @retval  ���ظ��ⲿ�����߼������
*/
Reactor* ReactorThread::startLoop()
{
	thread_.start();

	{
		///�ȴ������߳�ִ�У�������Ч�ļ������
		LockGuard lock(&mutex_);
		while (loop_ == NULL)
		{
			wait_.wait();
		}
	}

	return loop_;
}


/**
* @brief    �¼������̣߳��������˳�
*/
void ReactorThread::threadProc()
{
	Reactor loop;
	{
		LockGuard lock(&mutex_);
		loop_ = &loop;
		wait_.notify();
	}

	//����
	loop.loop();
	//exit
	printf("***thread loop exit \n");
}
