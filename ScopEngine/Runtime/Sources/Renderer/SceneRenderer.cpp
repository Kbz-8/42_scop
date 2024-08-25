#include <Renderer/ScenesRenderer.h>
#include <Renderer/Renderer.h>
#include <Graphics/Scene.h>

#include <cstring>

namespace Scop
{
	struct VertexMatricesData
	{
		Mat4f view;
		Mat4f projection;
	};

	void SceneRenderer::Render(Scene& scene, Renderer& renderer)
	{
		VertexMatricesData mat;
		mat.view = scene.GetCamera()->GetView();
		mat.projection = scene.GetCamera()->GetProj();

		static CPUBuffer buffer(sizeof(VertexMatricesData));
		std::memcpy(buffer.GetData(), &mat, buffer.GetSize());
		scene.GetForwardData().matrices_buffer->SetData(buffer, renderer.GetCurrentFrameIndex());

		m_passes.Pass(scene, renderer);
	}
}
