#ifndef __SCOPE_SINGLETON__
#define __SCOPE_SINGLETON__

#include <Utils/NonCopyable.h>

namespace Scop
{
	template <typename T>
	class Singleton : public NonCopyable
	{
		public:
			[[nodiscard]] inline static T& Get()
			{
				static T instance;
				return instance;
			}
	};
}

#endif
