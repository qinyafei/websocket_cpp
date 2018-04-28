/**
*  @file  NetCallbacks.h
*  @brief 网络回调函数定义头文件
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
#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_

#include <functional>
#include "Buffer.h"

using namespace ws::base;


namespace ws
{

namespace net
{

class NetConnection;
///网络回调函数定义
 typedef std::function<void(NetConnection*)> ConnectionCallback;
	typedef std::function<void(const NetConnection*, Buffer*)> MessageCallback;
	typedef std::function<void(const NetConnection*)> WriteCompleteCallback;
	typedef std::function<void(const NetConnection*)>CloseConnCallback;
}

}


#endif
