#ifndef __SCOP_CORE_ENGINE__
#define __SCOP_CORE_ENGINE__

namespace Scop
{
	class ScopEngine
	{
		public:
			ScopEngine(int ac, char** av);
			void Run();
			~ScopEngine();
	};
}

#endif