/**
*  @file  Timestamp.h
*  @brief 时间戳处理
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
#ifndef _TIMESTAMP_H_
#define _TIMESTAMP_H_

#include <time.h>
#include <unistd.h>
#include <string>
#include <stdint.h>
#include <string.h>

namespace ws
{

namespace base
{


	/** @class Timestamp
	*  @brief 时间戳处理类
	*/
class Timestamp
{
public:
	Timestamp()
		:microsecondsSinceEpoch_(0)
	{
	}
	Timestamp(int64_t microsecondsSinceEpoch);
	~Timestamp(){}

	static const int64_t csMicrosecondsPerSecond;///<微妙和秒换算单位
private:
	int64_t microsecondsSinceEpoch_;///<时间戳，微妙

public:
	///当前时间戳
	static Timestamp now(); 
	///格式化当前时间，已经加入china时区 +8
	std::string formatString(bool bShowMicroseconds = true) const;
	///返回微秒数
	int64_t microsecondsSinceEpoch() const
	{
		return microsecondsSinceEpoch_;
	}
	void setMicroSeconds(int64_t microsecs)
	{
		microsecondsSinceEpoch_ = microsecs;
	}

	///时间戳转换为tm结构体
	void gettm(tm* datetime);

	bool valid()
	{
		return (microsecondsSinceEpoch_ > 0);
	}
};


/**
* @brief    Timestamp <,==符号重载
*/
static  bool operator<(Timestamp tmstp, Timestamp tmstp2)                                              
 {                                                                                             
   return (tmstp.microsecondsSinceEpoch() < tmstp2.microsecondsSinceEpoch());        
     }    
                             
static bool operator==(Timestamp tmstp1, Timestamp tmstp2) 
   {   
     return (tmstp1.microsecondsSinceEpoch() == tmstp2.microsecondsSinceEpoch());
   }  


///时间差计算，秒
static double differTimestamp(Timestamp& tmstp1, Timestamp& tmstp2)
{
	int64_t diff = tmstp1.microsecondsSinceEpoch() - tmstp2.microsecondsSinceEpoch();
	return static_cast<double>(diff / Timestamp::csMicrosecondsPerSecond);
}

///加时后的时间戳
static Timestamp addSeconds(Timestamp& tmstp, double seconds)
{
	int64_t microsec = static_cast<int64_t>(seconds * Timestamp::csMicrosecondsPerSecond);
	return Timestamp(tmstp.microsecondsSinceEpoch() + microsec);
}

///tm转换time_t
 static time_t tmToInt(const tm* structtm)
 {
   tm day;
   memset(&day, 0, sizeof(struct tm));
   memcpy(&day, structtm, sizeof(struct tm));

   day.tm_year -= 1900;
   //day.tm_hour -= 12;                                                                                                                                                                           
   day.tm_mon -= 1;
   //day.tm_wday -= 1;                                                                                                                                                                            

   return mktime(&day);
 }

}

}//namespace

#endif
