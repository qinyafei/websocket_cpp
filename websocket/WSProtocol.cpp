#include "WSProtocol.h"
#include <assert.h>
#include <string.h>

using namespace ws;

#define SLICE_SIZE 500
#define SHORT_LENGTH 126
#define LONG_LENGTH 127

#define MAX_FRAMESIZE 1024*1024*4


WSProtocol::WSProtocol()
{
	frameBuff_ = new unsigned char[MAX_FRAMESIZE];
	framePos_ = 0;
}

WSProtocol::~WSProtocol()
{
	if (frameBuff_ != NULL)
	{
		delete []frameBuff_;
		frameBuff_ = NULL;
	}
	framePos_ = 0;
}

void WSProtocol::computeMask(unsigned char * data, int len)
{
	printf("###Premask orign=%s,len=%d\n", data, len);

	assert(data != NULL && len > 0);

	static short mask[4] = { MASK_CODE & 0xff ,
		(MASK_CODE >> 8) & 0xff ,
		(MASK_CODE >> 16) & 0xff ,
		(MASK_CODE >> 24) & 0xff
	};

	
	for (int i = 0; i < len; i++)
	{
		int pos = i % 4;
		data[i] = data[i] ^ mask[pos];
	}

	printf("###mask rsult=%s,len=%d\n", data, len);
}

void WSProtocol::computeUnMask(unsigned char * data, int len)
{
	printf("@@@@@UnPremask orign=%s,len=%d\n", data, len);

	assert(data != NULL && len > 0);

	static short mask[4] = { MASK_CODE & 0xff ,
		(MASK_CODE >> 8) & 0xff ,
		(MASK_CODE >> 16) & 0xff ,
		(MASK_CODE >> 24) & 0xff
	};


	for (int i = 0; i < len; i++)
	{
		int pos = i % 4;
		data[i] = data[i] ^ mask[pos];
	}

	printf("@@@@@Unmask rsult=%s,len=%d\n", data, len);
}

int WSProtocol::encodeFrame(unsigned char * payload, int len, OpCode type, void* context, WSFrameFunc func)
{
	WSHead header;
	unsigned char* buff = NULL;

	switch (type)
	{
	case eTextFrame:
	case eBlobFrame:
	{
		header.FIN = 0;
		header.RSV = 0;
		header.mask = 1;
		if (type == eTextFrame)
		{
			header.Opcode = eTextFrame;
		}
		else if (type == eBlobFrame)
		{
			header.Opcode = eBlobFrame;
		}

		if (len < SHORT_LENGTH)
		{
			header.FIN = 1;
			header.PayloadLen = len;

			computeMask(payload, len);
			int bufflen = len + sizeof(WSHead) + 4;
			buff = new unsigned char[bufflen + 1];
			if (buff == NULL)
			{
				//error
			}
			memcpy(buff, &header, sizeof(WSHead));
			unsigned mask = MASK_CODE;
			memcpy(buff + sizeof(WSHead), &mask, 4);
			memcpy(buff + sizeof(WSHead) + 4, payload, len);
			if (func != NULL)
			{
				func(buff, bufflen, context);
			}
		}
		else if (len < SLICE_SIZE)
		{
			//不需要分片
			header.FIN = 1;
			header.PayloadLen = SHORT_LENGTH;

			unsigned short actuallen = len;
			computeMask(payload, len);
			int bufflen = len + sizeof(WSHead) + 2 + 4;
			buff = new unsigned char[bufflen + 1];
			if (buff == NULL)
			{
				//error
			}
			memcpy(buff, &header, sizeof(WSHead));
			memcpy(buff + sizeof(WSHead), &actuallen, 2);
			unsigned mask = MASK_CODE;
			memcpy(buff + sizeof(WSHead) + 2, &mask, 4);
			memcpy(buff + sizeof(WSHead) + 4 + 2, payload, len);
			if (func != NULL)
			{
				func(buff, bufflen, context);
			}
		}
		else
		{
			int slice = len / SLICE_SIZE;
			unsigned short lastlen = len % SLICE_SIZE;
			if (lastlen > 0)
			{
				slice += 1;
			}

			//计算掩码
			computeMask(payload, len);

			for (int i = 0; i < slice; i++)
			{
				//分片长度小于short存储长度65535
				header.PayloadLen = SHORT_LENGTH;
				//分片包大小
				unsigned short actuallen = SLICE_SIZE + sizeof(WSHead) + 2 + 4;

				if (i == 0)
				{
					header.FIN = 0;
				}
				else if (i + 1 < slice)
				{
					header.FIN = 0;
					header.Opcode = eContinueFrame;
				}
				else
				{
					//最后一个分片
					header.FIN = 1;
					header.Opcode = eContinueFrame;

					if (lastlen < 126)
					{
						header.PayloadLen = lastlen;
						actuallen = lastlen + sizeof(WSHead) + 4;
						buff = new unsigned char[actuallen + 1];
						if (buff == NULL)
						{
							//error
						}
						memcpy(buff, &header, sizeof(WSHead));
						unsigned int mask = MASK_CODE;
						memcpy(buff + sizeof(WSHead), &mask, 4);
						memcpy(buff + sizeof(WSHead) + 4, payload + (i - 1) * SLICE_SIZE, lastlen);
					}
					else
					{
						actuallen = lastlen + sizeof(WSHead) + 2 + 4;
						buff = new unsigned char[actuallen + 1];
						if (buff == NULL)
						{
							//error
						}

						memcpy(buff, &header, sizeof(WSHead));
						memcpy(buff + sizeof(WSHead), &lastlen, 2);
						unsigned int mask = MASK_CODE;
						memcpy(buff + sizeof(WSHead) + 2, &mask, 4);
						memcpy(buff + sizeof(WSHead) + 4 + 2, payload + i * SLICE_SIZE, lastlen);
					}
					if (func != NULL)
					{
						func(buff, actuallen, context);
					}
					return 0;
				}

				buff = new unsigned char[actuallen + 1];
				if (buff == NULL)
				{
					//error
				}
				unsigned short packlen = SLICE_SIZE;
				memcpy(buff, &header, sizeof(WSHead));
				memcpy(buff + sizeof(WSHead), &packlen, 2);
				unsigned int mask = MASK_CODE;
				memcpy(buff + sizeof(WSHead) + 2, &mask, 4);
				memcpy(buff + sizeof(WSHead) + 4 + 2, payload + i * SLICE_SIZE, packlen);
				if (func != NULL)
				{
					func(buff, actuallen, context);
				}

				delete []buff;
				buff = NULL;
			}
		}

	}
		break;
	case ePingFrame:
	case ePongFrame:
	{
		header.FIN = 1;
		header.mask = 1;
		if (type == ePingFrame)
		{
			header.Opcode = ePingFrame;
		}
		else
		{
			header.Opcode = ePongFrame;
		}
		unsigned char pinglen = len;
		header.PayloadLen = pinglen;

		computeMask(payload, len);
		int bufflen = len + sizeof(WSHead) + 4;
		buff = new unsigned char[bufflen];
		if (buff == NULL)
		{
			//error
		}
		memcpy(buff, &header, sizeof(WSHead));
		unsigned int mask = MASK_CODE;
		memcpy(buff + sizeof(WSHead), &mask, 4);
		memcpy(buff + sizeof(WSHead) + 4, payload, len);
		if (func != NULL)
		{
			func(buff, bufflen, context);
		}

		delete[]buff;
		buff = NULL;
	}
	break;
	case eCloseFrame:
	{
		header.FIN = 1;
		header.Opcode = eCloseFrame;
		unsigned char closelen = len;
		header.PayloadLen = closelen;

		computeMask(payload, len);
		int bufflen = len + sizeof(WSHead) + 4;
		buff = new unsigned char[bufflen];
		if (buff == NULL)
		{
			//error
		}
		memcpy(buff, &header, sizeof(WSHead));
		unsigned mask = MASK_CODE;
		memcpy(buff + sizeof(WSHead), &mask, 4);
		memcpy(buff + sizeof(WSHead) + 4, payload, len);
		if (func != NULL)
		{
			func(buff, bufflen, context);
		}
	}
		break;
	}
	
	return 0;
}


int WSProtocol::decodeFrame(unsigned char * data, int len, void* context, WSFrameFunc func)
{
	assert(data != NULL && len < MAX_FRAMESIZE);

	do
	{
		WSHead header;
		memcpy(&header, data, sizeof(WSHead));

		if (header.FIN == 1)
		{
			//完整一帧数据
			unsigned char* payl = NULL;
			unsigned short payll = 0;
			if (header.Opcode == eContinueFrame)
			{
				if (header.PayloadLen < SHORT_LENGTH)
				{
					payl = data + sizeof(WSHead) + 4;
					payll = header.PayloadLen;

					data += payll + sizeof(WSHead) + 4;
					len -= payll + sizeof(WSHead) + 4;
				}
				else if (header.PayloadLen == SHORT_LENGTH)
				{
					payl = data + sizeof(WSHead) + 2 + 4;
					payll = *(unsigned short*)(data + sizeof(WSHead));

					data += payll + sizeof(WSHead) + 2 + 4;
					len -= payll + sizeof(WSHead) + 2 + 4;
				}

				if (header.mask == 1)
				{
					computeUnMask(payl, payll);
				}
				memcpy(frameBuff_ + framePos_, payl, payll);
				framePos_ += payll;

				if (func != NULL)
				{
					func(frameBuff_, framePos_, context);
				}

				if (len <= 0)
				{
					break;
				}
			}
			else
			{
				if (header.PayloadLen < SHORT_LENGTH)
				{
					payl = data + sizeof(WSHead) + 4;
					payll = header.PayloadLen;

					data += payll + sizeof(WSHead) + 4;
					len -= payll + sizeof(WSHead) + 4;
				}
				else if (header.PayloadLen == SHORT_LENGTH)
				{
					payl = data + sizeof(WSHead) + 2 + 4;
					payll = *(unsigned short*)(data + sizeof(WSHead));

					data += payll + sizeof(WSHead) + 2 + 4;
					len -= payll + sizeof(WSHead) + 2 + 4;
				}

				if (header.mask == 1)
				{
					computeUnMask(payl, payll);
				}

				if (func != NULL)
				{
					func(payl, payll, context);
				}
				
				if (len <= 0)
				{
					break;
				}
			}
		}
		else
		{
			assert(header.PayloadLen == SHORT_LENGTH);
			if (header.Opcode == eTextFrame || header.Opcode == eBlobFrame)
			{
				//帧的第一个分片，后续还有数据
				memset(frameBuff_, 0, MAX_FRAMESIZE);
				framePos_ = 0;
			}
			else if (header.Opcode == eContinueFrame)
			{
			}

			unsigned short payllen = *(unsigned short*)(data + sizeof(WSHead));
			unsigned int mask = *(unsigned int*)(data + sizeof(WSHead) + 2);
			unsigned char* payl = data + sizeof(WSHead) + 2 + 4;
			if (header.mask == 1)
			{
				computeUnMask(payl, payllen);
			}
			memcpy(frameBuff_ + framePos_, payl, payllen);

			framePos_ += payllen;
			data += payllen + sizeof(WSHead) + 2 + 4;
			len -= payllen + sizeof(WSHead) + 2 + 4;
			if (len <= 0)
			{
				break;
			}
		}
	} while (true);

	return 0;
}

