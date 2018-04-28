/**
*  @file  SocketOpts.h
*  @brief sockfd ��غ�����װ
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
#ifndef _SOCKETOPTS_H_
#define _SOCKETOPTS_H_

#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

namespace ws
{

namespace base
{


namespace sockets
{
	/**
	* @brief socket��غ���
	*/
	int read(int sockfd, void* buf, int len);
	int write(int sockfd, void* buf, int len);
	int connect(int sockfd, const struct sockaddr_in& addr);

    int bind(int sockfd, const struct sockaddr_in* addr);
	int listen(int sockfd);
	int accept(int sockfd, struct sockaddr_in* addr);

	///����ip��port����sockaddr_in�ṹ��
	void makeSockAddr(const char* ip, uint16_t port, 
	struct sockaddr_in* addr);

	///����socket��ȡ���أ�Զ��addr
	struct sockaddr_in getRemoteAddr(int sockfd);
	struct sockaddr_in getLocalAddr(int sockfd);

	int getsockError(int sockfd);
	void shutdownWrite(int sockfd);

	///����������tcp socket
	int createNonblockTcpSock();
	///��ַ����
	void setReuseAddr(int sockfd, bool on);
	///�˿ڸ���
	void setReusePort(int sockfd, bool on);
	///socket sockaddr_in��غ���
	struct sockaddr* sockaddr_cast(struct sockaddr_in* addr);
	void toIp(char* buf, int size, const struct sockaddr_in& addr);

	uint16_t network2Host16(uint16_t net16);
	uint16_t host2Network16(uint16_t host16);

	uint32_t hostToNetwork32(uint32_t host32);
	uint32_t networkToHost32(uint32_t net32);

	uint64_t networkToHost64(uint64_t net64);
	uint64_t hostToNetwork64(uint64_t host64);

}


}
}


#endif
