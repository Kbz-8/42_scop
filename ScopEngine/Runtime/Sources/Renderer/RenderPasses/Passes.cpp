#include <Renderer/RenderPasses/Passes.h>
#include <Renderer/Renderer.h>
#include <Graphics/Scene.h>

namespace Scop
{
	void RenderPasses::Pass(Scene& scene, Renderer& renderer)
	{
		m_forward.Pass(scene, renderer);
	}
}
