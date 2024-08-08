#ifndef __SCOP_SCENES_RENDERER__
#define __SCOP_SCENES_RENDERER__

#include <Graphics/Scene.h>

namespace Scop
{
	class SceneRenderer
	{
		public:
			SceneRenderer();

			void Render(Scene& scene); // TODO : add RTT support

			~SceneRenderer();

		private:
	};
}

#endif
