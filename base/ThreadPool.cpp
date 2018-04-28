#include "ThreadPool.h"
#include <stdio.h>
#include <string.h>

using namespace ws::base;

ThreadPool::ThreadPool()
	: mutex_(),
	notEmpty_(mutex_),
    brunning_(false)
{
}


ThreadPool::~ThreadPool()
{
	if (brunning_)
	{
		stop();
	}
}

/**
* @brief    �����̳߳�
* @param[in] threadNum���̸߳���
*/
void ThreadPool::start(int threadNum)
{
	assert(threadList_.empty());
	threadList_.reserve(threadNum);
	brunning_ = true;

	//��������߳�
	char tmpbuf[16];
	for (int i = 0; i < threadNum; i++)
	{
		memset(tmpbuf, 0, sizeof(tmpbuf));
		snprintf(tmpbuf, sizeof(tmpbuf), "thread%d", i + 1);
		Thread* pthread = new Thread(std::bind(&ThreadPool::runInThread, this), tmpbuf);
		threadList_.push_back(pthread);

		pthread->start();

	}
}

/**
* @brief   ֪ͨ�����̣߳�����ִ��
*/
void ThreadPool::stop()
{
	{
		LockGuard lock(&mutex_);
		brunning_ = false;
		notEmpty_.notifyAll();
	}

	printf("***waitfor thread join\n");
	for (int i = 0; i < threadList_.size(); i++)
	{
		threadList_[i]->join();
		delete threadList_[i];
	}

	threadList_.clear();
}


/**
* @brief    �����߳�����
* @param[in] fun���߳�ִ�к���
*/
void ThreadPool::run(TaskFunc fun)
{
	assert(!threadList_.empty());

	LockGuard lock(&mutex_);
	taskList_.push_back(fun);
	notEmpty_.notify();
}


/**
* @brief    ȡ�߳���������
* @return TaskFunc
* @retval  �߳�����
*/
TaskFunc ThreadPool::getTask()
{
	LockGuard lock(&mutex_);
	while (taskList_.empty() && brunning_)
	{
		notEmpty_.wait();
	}
        printf("*** get new task!\n");

		TaskFunc t;
	
		if (!taskList_.empty())
		{
			t = taskList_.front();
			taskList_.pop_front();
		}
	return t;
}


/**
* @brief   �̳߳�ִ�к���
*/
void ThreadPool::runInThread()
{
	try
	{
		while (brunning_)
		{
			TaskFunc t(getTask());
			if (t != NULL)
			{
				t();
			}
			else
			{
				CurrentThread::sleepUsec(1000 * 40);
			}
		}
	}
	catch (const std::exception& ex)
	{
		fprintf(stderr, "reason: %s\n", ex.what());
		abort();
	}
	catch (...)
	{
		fprintf(stderr, "unknown exception caught in ThreadPool \n");
		throw; // rethrow
	}

	printf("*** threadpool's thread exit \n");
}
