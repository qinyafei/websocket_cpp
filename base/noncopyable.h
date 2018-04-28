#ifndef _NONCOPYABLE_H_
#define _NONCOPYABLE_H_


//  Private copy constructor and copy assignment ensure classes derived from
//  class noncopyable cannot be copied.

//  Contributed by Dave Abrahams

namespace ws
{

namespace base  // protection from unintended ADL
{

  //value class can copyable
class copyable
{
};

	class noncopyable
	{
	protected:
		noncopyable() {}
		~noncopyable() {}
	private:  // emphasize the following members are private
		noncopyable(const noncopyable&);
		const noncopyable& operator=(const noncopyable&);
	};
}

typedef base::noncopyable noncopyable;

}//namespace ws

#endif
