#ifndef _SINGLETON_H
#define _SINGLETON_H
#include <assert.h>

namespace ws
{

	/** @class CSingleton
	*  @brief 单例模式模板类
	*   
	example:
	struct CFooSingleton : public CSingleton < CFooSingleton >
	{
		void foo() { return; }
	};

	// call function:
	CFooSingleton::getInstance().foo();
	*/
	template<class T>
	class CSingleton
	{
	public:
		virtual ~CSingleton() {}

		/// returns a reference and not a pointer to be sure that the user
		/// doesn't have to test the return value and can directly access the class
		static T &getInstance()
		{
			if (!Instance)
			{
				Instance = new T;
				assert(Instance);
			}
			return *Instance;
		}

		///
		static T &instance() { return getInstance(); }

		static void releaseInstance()
		{
			if (Instance)
			{
				delete Instance;
				Instance = nullptr;
			}
		}
    ///must be protected
	protected:
		CSingleton()
		{
		}

		static T *Instance;
	};

	template <class T>
	T* CSingleton<T>::Instance = 0;

//-------------------------------------------------------------------------------

	/** @class CManualSingleton
	*  @brief 单例模式模手动板类
	*
	example:
	struct CFooSingleton : public CManualSingleton<CFooSingleton>
	{
	    void foo() { return ; }
	};

	// create an instance
	CFooSingleton	mySingleton

	// call thefunction:
	CFooSingleton::getInstance()->foo();

	// create another instance is forbiden
	CFooSingleton	otherInstance; //assert()
	*/
	template <class T>
	class CManualSingleton
	{
		static T *&_instance()
		{
			static T *instance = nullptr;

			return instance;
		}

	protected:


		CManualSingleton()
		{
			assert(_instance() == nullptr);
			_instance() = static_cast<T*>(this);
		}

		~CManualSingleton()
		{
			assert(_instance() == this);
			_instance() = nullptr;
		}

	public:

		static bool isInitialized()
		{
			return _instance() != nullptr;
		}

		static T* getInstance()
		{
			nlassert(_instance() != nullptr);

			return _instance();
		}
	};


} //namespace 

#endif 


