#ifndef __SCOP_PASSES__
#define __SCOP_PASSES__

#include <Renderer/RenderPasses/ForwardPass.h>

namespace Scop
{
	class RenderPasses
	{
		public:
			RenderPasses() = default;

			void Pass(class Scene& scene, class Renderer& renderer);

			~RenderPasses() = default;

		private:
			ForwardPass m_forward;
	};
}

#endif
