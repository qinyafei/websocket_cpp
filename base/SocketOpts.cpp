#include "SocketOpts.h"
#include <string.h>
#include <assert.h>

using namespace ws::base;


int sockets::read(int sockfd, void* buf, int len)
{
	return ::read(sockfd, buf, len);
}


int sockets::write(int sockfd, void* buf, int len)
{
	return ::write(sockfd, buf, len);
}


void sockets::setReuseAddr(int sockfd, bool on)
{
	int optval = on ? 1 : 0;
	::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
		&optval, static_cast<socklen_t>(sizeof optval));
	
}

void sockets::setReusePort(int sockfd, bool on)
{

#ifdef SO_REUSEPORT
	int optval = on ? 1 : 0;
	int ret = ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof optval));
	if (ret < 0 && on)
	{
		//DLOG(INFO) << "SO_REUSEPORT failed.";
	}
#else
	if (on)
	{
		DLOG(INFO) << "SO_REUSEPORT is not supported.";
	}
#endif
}


/**
* @brief    根据ip，port生成sockaddr_in结构体
* @param[out] addr：输出参数
* @param[in] ip：
* @param[in] port：
*/
void sockets::makeSockAddr(const char* ip, uint16_t port,
struct sockaddr_in* addr)
{
	memset(addr, 0, sizeof(sockaddr_in));

	addr->sin_family = AF_INET;
	addr->sin_port = htobe16(port);
	if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
	{
		assert(0);
	}
}


/**
* @brief    根据fd获取远程addr
* @param[in] sockfd：fd
* @return sockaddr_in
*/
struct sockaddr_in sockets::getRemoteAddr(int sockfd)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));
	socklen_t addrlen = static_cast<socklen_t>(sizeof(addr));
	if (::getpeername(sockfd, sockaddr_cast(&addr), &addrlen) < 0)
	{
		assert(0);
	}

	return addr;
}


/**
* @brief    根据fd获取本地绑定的addr
* @param[in] sockfd：fd
* @return sockaddr_in
*/
struct sockaddr_in sockets::getLocalAddr(int sockfd)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));
	socklen_t addrlen = static_cast<socklen_t>(sizeof(addr));
	if (::getsockname(sockfd, sockaddr_cast(&addr), &addrlen) < 0)
	{
		assert(0);
	}

	return addr;
}


int sockets::getsockError(int sockfd)
{
	int optval = 0;
	socklen_t optlen = static_cast<socklen_t>(sizeof(optval));
	if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
	{
		return errno;
	}

	return optval;
}


void sockets::shutdownWrite(int sockfd)
{
	if (::shutdown(sockfd, SHUT_WR) < 0)
	{
		assert(0);
	}
}

/**
* @brief    创建非阻塞TCP socketfd
* @return int fd
*/
int  sockets::createNonblockTcpSock()
{
	int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
	if (sockfd < 0)
	{
		assert(0);
	}

	return sockfd;
}


/**
* @brief    connect wapper
* @param[in] sockfd：fd
* @param[in] addr：remote address
* @return int
*/
int sockets::connect(int sockfd, const struct sockaddr_in& addr)
{
  return ::connect(sockfd, sockaddr_cast(const_cast<sockaddr_in*>(&addr)), static_cast<socklen_t>(sizeof(sockaddr_in)));
}

/**
* @brief    bind wapper
* @param[in] sockfd：fd
* @param[in] addr：remote address
* @return int
*/
int sockets::bind(int sockfd, const struct sockaddr_in* addr)
{
	int ret = ::bind(sockfd, sockaddr_cast(const_cast<sockaddr_in*>(addr)), static_cast<socklen_t>(sizeof(sockaddr_in)));
	if (ret < 0)
	{
		//assert(0);
		return errno;
	}

  return ret;
}

/**
* @brief    listen wapper
* @param[in] sockfd：
* @return int
*/
int sockets::listen(int sockfd)
{
	int ret = ::listen(sockfd, SOMAXCONN);
	if (ret < 0)
	{
		//assert(0);
		return ret;
	}

	return 0;
}

/**
* @brief    accept wapper
* @param[in] sockfd：
* @param[out] addr：接收addr
* @return int
*/
int sockets::accept(int sockfd, struct sockaddr_in* addr)
{
	socklen_t addrlen = static_cast<socklen_t>(sizeof(sockaddr_in));
	int connfd = ::accept4(sockfd, sockaddr_cast(addr),
		&addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
	if (connfd < 0)
	{
		int err = errno;
		switch (err)
		{
		case EAGAIN:
		case ECONNABORTED:
		case EINTR:
		case EPROTO:
		case EPERM:
		case EMFILE:
			errno = err;
			break;
		case EBADF:
		case EFAULT:
		case EINVAL:
		case ENFILE:
		case ENOBUFS:
		case ENOMEM:
		case ENOTSOCK:
		case EOPNOTSUPP:
			// unexpected errors
			assert(0);
			break;
		default:
			break;
		}
	}

	return connfd;
}

struct sockaddr* sockets::sockaddr_cast(struct sockaddr_in* addr)
{
  return static_cast<struct sockaddr*>((void*)addr);
}

void sockets::toIp(char* buf, int size, const struct sockaddr_in& addr)
{
	assert(size >= INET_ADDRSTRLEN);
	::inet_ntop(AF_INET, &addr.sin_addr, buf, static_cast<socklen_t>(size));

}

/**
* @brief    网络字节序到主机字节序，16bit
* @param[in] net16：
* @return uint16_t
*/
uint16_t sockets::network2Host16(uint16_t net16)
{
	return be16toh(net16);
}

/**
* @brief    主机字节序到网络字节序
* @param[in] host16：
* @return uint16_t
*/
uint16_t sockets::host2Network16(uint16_t host16)
{
	return htobe16(host16);
}


uint32_t sockets::hostToNetwork32(uint32_t host32)
{
	return htobe32(host32);
}

uint32_t sockets::networkToHost32(uint32_t net32)
{
	return be32toh(net32);
}


uint64_t sockets::hostToNetwork64(uint64_t host64)
{
	return htobe64(host64);
}

uint64_t sockets::networkToHost64(uint64_t net64)
{
	return be64toh(net64);
}

