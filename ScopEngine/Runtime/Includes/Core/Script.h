#ifndef __SCOP_SCRIPT__
#define __SCOP_SCRIPT__

#include <Utils/NonOwningPtr.h>

namespace Scop
{
	class Script
	{
		public:
			Script() = default;

			virtual void OnInit(NonOwningPtr<class Actor> actor) = 0;
			virtual void OnUpdate(NonOwningPtr<class Actor> actor, float delta) = 0;
			virtual void OnQuit(NonOwningPtr<class Actor> actor) = 0;

			virtual ~Script() = default;
	};
}

#endif
