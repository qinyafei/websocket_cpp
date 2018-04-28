/**	
 *  @file     BCFProcessCreater.cpp
 *  @brief    进程创建器
 *  @version  0.0.1
 *  @author   yulichun<yulc@bstar.com.cn>
 *  @date     2010-10-15    Created it
 *  @note     无
 */

#include <stdio.h>
#include <stdlib.h>
#include "BCFProcessCreater.hpp"
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
//#include "BCFStatusType.hpp"


//for GDB debug
#include <sys/stat.h>
#include <unistd.h>

void debug_wait(const char *tag_file)
{  
  while(true)  
    {      
      struct stat buf;
      int ret = stat(tag_file, &buf);
      if (ret >= 0)
	{
	  sleep(1);
	}
      else    
	break;   
    }

}

using namespace BCF;

BCFProcessCreater* BCFProcessCreater::pInstance = NULL;

/**
 * @fn      BCFProcessCreater::getInstance()
 * @brief   获取进程创建器的对象指针,该对象不能显式的创建对象,只能由该方法创建.
 * @return  该类全局唯一实例的指针
 */
BCFProcessCreater * BCFProcessCreater::getInstance()
{
  if(pInstance == NULL)
    pInstance = new BCFProcessCreater;

  return pInstance;
}

/**
 * @fn BCFProcessCreater::destroy()
 * @brief  销毁创建器
 * @return   无返回值
 */
void BCFProcessCreater::destroy()
{
  delete pInstance;
  pInstance = NULL;
}

/**
 *@fn           BCFProcessCreater::spawn(void(*pRun)(void*), void*pArg)
 *@brief        创建进程,并执行指定的函数
 *@param[in] pRun  子进程入口函数
 *@param[in] pArg  传递给pRun的参数
 *                 
 *@return       
 *@retval    >0: 子进程的pid
 *@retval    <0:  系统调用出错,调用 BCFError::getSysError(std::string&) 获取错误信息
 */
int BCFProcessCreater::spawn(void(*pRun)(void*), void*pArg)
{
  pid_t pid = fork();
  
  switch(pid)
  {
    // fork()调用失败
    case -1:
    {
      return -1;   
    }
    
    // 子进程空间
    case 0:
    {
		if ((pid = fork()) < 0)
		{
			assert(0);
		}
		else if(pid > 0)
		{
			//first child exit
			exit(0);
		}

      //BCFLogPrint::setProcessPid(getpid()); // 设置当前子进程的pid
      //for GDB debug      
      debug_wait("/opt/debug.txt");

      pRun(pArg);
      _exit(0);
    }
    
    // 父进程空间,返回pid
	default:
		(void)0;
      //return pid;
  }

  if (waitpid(pid, NULL, 0) != pid) //wait for first child
	  assert(0);

  return pid;
  
}

