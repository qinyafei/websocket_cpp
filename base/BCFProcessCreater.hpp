/**	
 *  @file     BCFProcessCreater.hpp
 *  @brief    ���̴�����
 *  @version  0.0.1
 *  @author   yulichun<yulc@bstar.com.cn>
 *  @date     2010-10-15    Created it
 *  @note     ��
 */

#ifndef BCFPROCESSCREATER_HPP_
#define BCFPROCESSCREATER_HPP_




#include <unistd.h>

namespace BCF
{

  /** @class BCFProcessCreater 
   *  @brief ���̴�����,������,�����ӽ���,��ִ��ָ���ĺ���
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

