#pragma once
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <vector>


namespace ws
{
	unsigned char getbit(unsigned char src, char pos, char count);
	unsigned char setbit(unsigned char src, char pos, char count, unsigned char in);

	void milliseconds_sleep(unsigned long mSec);

	void strsplit(const std::string& s, const std::string seperator, std::vector<std::string>& v);

	void log(int level, const char* fmt, ...);

#if 0

#define DEBUG
#define DMARK  "\033[1;32;40m[%s:%d] \033[1;31;40m[%d]%s(): \033[0m|"
#ifdef DEBUG
#define LOG(level, fmt, args...) \
    log(level, DMARK#fmt,__FILE__,__LINE__,getpid(),__FUNCTION__,##args)
#endif

#define INFO 7
#define WARNING 3
#define ERROR 1

#endif

}

