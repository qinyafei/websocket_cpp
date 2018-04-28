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
int tm_sec ����Ŀǰ������������ΧΪ0-59����������61��
int tm_min ����Ŀǰ��������Χ0-59
int tm_hour ����ҹ�����ʱ������ΧΪ0-23
int tm_mday Ŀǰ�·ݵ���������Χ01-31
int tm_mon ����Ŀǰ�·ݣ���һ�����𣬷�Χ��0-11
int tm_year ��1900 ���������������
int tm_wday һ���ڵ�������������һ���𣬷�ΧΪ0-6
int tm_yday �ӽ���1��1�������������������ΧΪ0-365
int tm_isdst �չ��Լʱ������
�˺������ص�ʱ������δ��ʱ��ת��������UTCʱ�䡣


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
* @brief    ʱ���ת��Ϊtm�ṹ��
* @param[out] datetime��tm�ṹ��
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
* @brief    ��ʽ����ǰʱ�䣬�Ѿ�����chinaʱ�� +8
* @param[int] bShowMicroseconds���Ƿ���ʾ΢��
* @return string
* @retval  ��ʽ������ַ���
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
