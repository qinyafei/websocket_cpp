/**	
 *  @file     BCFProcessCreater.hpp
 *  @brief    进程创建器
 *  @version  0.0.1
 *  @author   yulichun<yulc@bstar.com.cn>
 *  @date     2010-10-15    Created it
 *  @note     无
 */

#ifndef BCFPROCESSCREATER_HPP_
#define BCFPROCESSCREATER_HPP_




#include <unistd.h>

namespace BCF
{

  /** @class BCFProcessCreater 
   *  @brief 进程创建器,单件类,创建子进程,并执行指定的函数
   */
  class BCFProcessCreater
  {
  private:
    static BCFProcessCreater *pInstance;

  public:
    static BCFProcessCreater* getInstance();
    static void destroy();

    int spawn(void(*)(void*), void*);

  private:
    BCFProcessCreater() {}
    ~BCFProcessCreater() {}
  };

}




#endif

