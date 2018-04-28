#ifndef GLOBALDEF_H_
#define GLOBALDEF_H_

#include <functional>

namespace ws
{

typedef std::function<void(int)> OnOpenFunc;
typedef std::function<void()> OnCloseFunc;
typedef std::function<void(int, std::string)> OnErrorFunc;
typedef std::function<void(unsigned char*, int)> OnMessageFunc;

//connection id 
typedef std::function<void(std::string, const char* msg, int len)> OnConnMsgFunc;
}

#endif

