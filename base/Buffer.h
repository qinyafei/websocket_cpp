/**
*  @file  Buffer.h
*  @brief 读写缓冲buffer
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

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <vector>
#include <string>
#include <assert.h>


namespace ws
{

namespace base
{


	/** @class Buffer
	*  @brief 读写缓冲队列类，主要应用在网络
	*   发送、接收数据缓冲buffer
	*  非线程安全
	*/

	class Buffer
	{
	public:
		Buffer(int initSize = csInitialSize)
			: readPos_(csPrependSize),
			writePos_(readPos_)
		{
			//buffer_(csPrependSize + csInitialSize),
			buffer_.reserve(csPrependSize + csInitialSize);
		}
		~Buffer(){}

		///初始化大小，前置预留空间，方便前插，不需要memcpy
		static const int csInitialSize = 1024 * 4;
		static const int csPrependSize = 8;
	public:
		///可读字节数
		int readableBytes()
		{
			return writePos_ - readPos_;
		}

		///可写入字节数
		int writeableBytes()
		{
			return capacity() - writePos_;
		}

		///buffer空间不足，重新分配
		void ensureWritableBytes(int len)
		{
			if (writeableBytes() < len)
			{
				newSpace(len);
			}
		}

		///前插数据
		void prepend(const char* data, int len)
		{
			assert(len <= readPos_);
			readPos_ -= len;
			std::copy(data, data + len, begin() + readPos_);
		}
		///移动读指针
		void retrieve(int len)
		{
			assert(len <= readableBytes());

			if (len < readableBytes())
			{
				readPos_ += len;
			}
			else
			{
				readPos_ = writePos_ = csPrependSize;
			}

		}

		void reset()
		{
			readPos_ = writePos_ = csPrependSize;
			buffer_.clear();
		}

		///读取数据，存入string(binary data也可)
		std::string read(int len)
		{
			assert(len <= readableBytes());

			std::string str(peekRead(), len);
			retrieve(len);

			return str;
		}

		///读取all data
		std::string readAll()
		{
			return read(readableBytes());
		}

		///移动写指针
		void hasWritten(int len)
		{
			assert(len <= writeableBytes());

			writePos_ += len;
		}

		///获取buffer头指针
		char* begin()
		{
			return &*buffer_.begin();
		}

		///读指针
		const char* peekRead()
		{
			return begin() + readPos_;
		}

		///写指针
		char* peekWrite()
		{
			return begin() + writePos_;
		}

		////重新分配更大空间
		void newSpace(int len)
		{
			printf("############Buffer::newSpace\n");
			//Èç¹û¿ÉÐ´Èë¿Õ¼äÐ¡ÓÚÊý¾Ý³¤¶È£¬ÔòÖØÐÂ·ÖÅä¿Õ¼ä
			if (writeableBytes() + readPos_ < len + csPrependSize)
			{
				//copy readable data
				int readLen = readableBytes();
				int writepos = writePos_;
				std::string readableData(read(readLen));
				buffer_.resize(writepos + len);
				std::copy(readableData.begin(),
					readableData.end(),
					buffer_.begin() + csPrependSize);

				readPos_ = csPrependSize;
				writePos_ = readPos_ + readLen;
			}
			else
			{
				int readLen = readableBytes();
				std::copy(begin() + readPos_,
					begin() + writePos_,
					begin() + csPrependSize);
				readPos_ = 0;
				writePos_ = readPos_ + readLen;
			}
		}

		///当前容量
		int capacity() const
		{
			return buffer_.capacity();
		}

		///写数据
		void write(const char* data, int len)
		{
			if (writeableBytes() < len)
			{
				newSpace(len);
			}

			assert(len <= writeableBytes());
			std::copy(data, data + len, peekWrite());
			hasWritten(len);
		}

		///直接read socket date到buffer
		int readfd(int sockfd, int* error);

	public:
		int readPos_; ///<读索引
		int writePos_;  ///<写索引
		std::vector<char> buffer_; ///<容器
	};

}

}//namespace

#endif
