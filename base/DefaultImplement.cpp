/**
*  @file  DefaultImplement.cpp
*  @brief 所有公共接口函数
*  @version 0.0.1
*  @since 0.0.1
*  @author
*  @date 2016-7-25  Created it
*/
/******************************************************************************
*@note
Copyright 2007, BeiJing Bluestar Corporation, Limited
ALL RIGHTS RESERVED
Permission is hereby granted to licensees of BeiJing Bluestar, Inc. products
to use or abstract this computer program for the sole purpose of implementing
a product based on BeiJing Bluestar, Inc. products. No other rights to
reproduce, use, or disseminate this computer program,whether in part or  in
whole, are granted. BeiJing Bluestar, Inc. makes no representation or
warranties with respect to the performance of this computer program, and
specifically disclaims any responsibility for any damages, special or
consequential, connected with the use of this program.
For details, see http://www.bstar.com.cn/
******************************************************************************/

#include "Poller.h"
#include "SelectPoller.h"

#include <stdlib.h>

using namespace ws::base;


/**
* @brief    创建poll
* @return Poller*
* @retval  
*/
Poller* Poller::createPoller()
{
  if (::getenv("USE_EPOLL"))
    {
      return NULL;
    }
  else
    {
      return new SelectPoller();
    }

  return NULL;
}
