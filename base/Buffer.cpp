#include "Buffer.h"
#include <errno.h>
#include <sys/uio.h>
#include "SocketOpts.h"
#include <string.h>
using namespace ws::base;


/**
* @brief    socket直接读入数据到buffer
* @param[out] error：错误号
* @param[in] sockfd 
* @return int
* @retval  读取长度
*/
int Buffer::readfd(int sockfd, int* error)
{
	static const int csMaxBufSize = 65536;
	char* morebuf = new char[csMaxBufSize];
	struct iovec vec[2];
	const int writelen = writeableBytes();
	vec[0].iov_base = begin() + writePos_;
	vec[0].iov_len = writelen;
	vec[1].iov_base = morebuf;
	vec[1].iov_len = csMaxBufSize;
	const int iovcount = (writelen < csMaxBufSize) ? 2 : 1;
	const int readlen = ::readv(sockfd, vec, iovcount);
	if (readlen < 0)
	{
		*error = errno;
	}
	else if (readlen <= writelen)
	{
		writePos_ += readlen;
	}
	else
	{
		char debug[64] = { 0, };
		sprintf(debug, "###readfd:readlen=%d,writelen=%d,buffer_size=%d \n", readlen, writelen, buffer_.size());
		printf(debug);

		//writePos_ = buffer_.size();
		writePos_ += writelen;
		Buffer::write(morebuf, readlen - writelen);
	}

	//释放内存
	delete[]morebuf;
	morebuf = NULL;
	return readlen;

}
