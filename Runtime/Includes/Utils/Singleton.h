#ifndef __SCOPE_SINGLETON__
#define __SCOPE_SINGLETON__

#include <Utils/NonCopyable.h>

template <typename T>
class Singleton : public NonCopyable
{
	public:
		inline static T& Get()
		{
			static T instance;
			return instance;
		}
};

#endif
