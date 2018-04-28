#include "Timestamp.h"
#include <sys/time.h>

using namespace ws::base;

/*
struct tm
{
int tm_sec;
int tm_min;
int tm_hour;
int tm_mday;
int tm_mon;
int tm_year;
int tm_wday;
int tm_yday;
int tm_isdst;
};
int tm_sec 代表目前秒数，正常范围为0-59，但允许至61秒
int tm_min 代表目前分数，范围0-59
int tm_hour 从午夜算起的时数，范围为0-23
int tm_mday 目前月份的日数，范围01-31
int tm_mon 代表目前月份，从一月算起，范围从0-11
int tm_year 从1900 年算起至今的年数
int tm_wday 一星期的日数，从星期一算起，范围为0-6
int tm_yday 从今年1月1日算起至今的天数，范围为0-365
int tm_isdst 日光节约时间的旗标
此函数返回的时间日期未经时区转换，而是UTC时间。


time_t cur_time=time(NULL);
if( cur_time < 0 )
{
perror("time");
return -1;
}

struct tm utc_tm;;
if( NULL == gmtime_r( &cur_time, &utc_tm ) )
{
perror("gmtime" );
return -1;
}

struct tm local_tm;
if( NULL == localtime_r( &cur_time, &local_tm ) )
{
perror("localtime" );
return -1;
}
printf("UTC = %s", asctime(&utc_tm) );
printf("LOC = %s", asctime(&local_tm) );
printf("LOC = %s", ctime(&cur_time) );

*/


const int64_t Timestamp::csMicrosecondsPerSecond = 1000 * 1000;


Timestamp::Timestamp(int64_t microsecondsSinceEpoch)
	:microsecondsSinceEpoch_(microsecondsSinceEpoch)
{

}


/**
* @brief    时间戳转换为tm结构体
* @param[out] datetime：tm结构体
*/
void Timestamp::gettm(tm* datetime)
{
	time_t seconds = static_cast<time_t>(microsecondsSinceEpoch_ / csMicrosecondsPerSecond);
	//if (gmtime_r(&seconds, datetime) == NULL)
	//{
		//error
	//}
	if(localtime_r( &seconds, datetime ) == NULL)
	  {
	    //error
	  }

	//UTC+8
        datetime->tm_mon += 1;
	datetime->tm_year += 1900;
	//datetime->tm_hour += 12;
        //datetime->tm_wday += 1;
}


Timestamp Timestamp::now()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	int64_t seconds = tv.tv_sec;

	return Timestamp(seconds*csMicrosecondsPerSecond + tv.tv_usec);
}



/**
* @brief    格式化当前时间，已经加入china时区 +8
* @param[int] bShowMicroseconds：是否显示微妙
* @return string
* @retval  格式化后的字符串
*/
std::string Timestamp::formatString(bool bShowMicroseconds) const
{
	char buf[64] = { 0 };
	struct tm datetime;
	time_t seconds = static_cast<time_t>(microsecondsSinceEpoch_ / csMicrosecondsPerSecond);
	if (localtime_r(&seconds, &datetime) == NULL)
	{
		//error
		return "";
	}

	if (bShowMicroseconds)
	{
		int microsec = static_cast<int>(microsecondsSinceEpoch_ % csMicrosecondsPerSecond);
			snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
			datetime.tm_year + 1900, datetime.tm_mon + 1, datetime.tm_mday,
			datetime.tm_hour, datetime.tm_min, datetime.tm_sec, microsec);
	}
	else
	{
		snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
			datetime.tm_year + 1900, datetime.tm_mon + 1, datetime.tm_mday,
			datetime.tm_hour, datetime.tm_min, datetime.tm_sec);
	}

	return buf;
}
