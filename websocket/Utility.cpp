#include "Utility.h"
#include <assert.h>

#include <iostream>
#include <stdarg.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


unsigned char MASK[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

unsigned char ws::getbit(unsigned char src, char pos, char count)
{
	assert(pos + count <= 8);

	unsigned char res = 0;
	int bitpos = 0;
	while (count > 0)
	{
		unsigned char bit = (src & MASK[7 - pos]);
		if (bit > 0)
		{
			res |= 1 << bitpos;
		}
		else
		{
			res |= 0 << bitpos;
		}

		count--;
		pos++;
		bitpos++;
	}

	return res;
}

unsigned char ws::setbit(unsigned char src, char pos, char count, unsigned char in)
{
	assert(pos + count <= 8);

	int bitpos = 0;
	for (int i = 0; i < count; i++)
	{
		if ((in & MASK[7 - i]) > 0)
		{
			src = src | MASK[7 - pos];
		}
		else
		{
			src = src & (~MASK[7 - pos]);
		}
		pos++;
	}

	return src;
}



void ws::log(int level, const char* fmt, ...)
{
	va_list va;
	char buf[1024] = { 0 };
	va_start(va, fmt);
	int n = vsnprintf(buf, sizeof(buf), fmt, va);
	va_end(va);

	char tmstamp[32] = { 0 };
	struct timeval tv;
	gettimeofday(&tv, NULL);
	time_t t = tv.tv_sec;
	int tn = strftime(tmstamp, sizeof(tmstamp), "%T.", localtime(&t));
	sprintf(tmstamp + tn, "%3.3lu ", tv.tv_usec / 1000);

	std::cout << tmstamp << buf << std::endl;
}

void ws::strsplit(const std::string& s, const std::string seperator, std::vector<std::string>& v)
{
	std::string::size_type pos1, pos2;
	pos2 = s.find(seperator);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + seperator.size();
		pos2 = s.find(seperator, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}


void ws::milliseconds_sleep(unsigned long mSec)
{
	struct timeval tv;
	tv.tv_sec = mSec / 1000;
	tv.tv_usec = (mSec % 1000) * 1000;
	int err;
	do {
		err = select(0, NULL, NULL, NULL, &tv);
	} while (err<0 && errno == EINTR);
}

