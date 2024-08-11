#ifndef __SCOP_FORWARD_PASS__
#define __SCOP_FORWARD_PASS__

#include <Renderer/RenderPasses/BasePass.h>

namespace Scop
{
	class ForwardPass : public BasePass
	{
		public:
			ForwardPass() = default;

			void Pass(class Scene& scene, class Renderer& renderer) override;

			~ForwardPass() = default;
	};
}

#endif
