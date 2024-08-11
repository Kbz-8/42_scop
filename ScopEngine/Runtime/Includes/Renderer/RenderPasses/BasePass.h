#ifndef __SCOP_PASS__
#define __SCOP_PASS__

namespace Scop
{
	class BasePass
	{
		public:
			BasePass() = default;
			virtual void Pass(class Scene& scene, class Renderer& renderer) = 0;
			~BasePass() = default;
	};
}

#endif
