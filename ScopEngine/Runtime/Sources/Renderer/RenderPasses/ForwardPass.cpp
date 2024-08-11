#include <Renderer/RenderPasses/ForwardPass.h>
#include <Renderer/Pipelines/Graphics.h>
#include <Renderer/Renderer.h>
#include <Graphics/Scene.h>

namespace Scop
{
	void ForwardPass::Pass(Scene& scene, Renderer& renderer)
	{
		std::array<float, 4> clear_color = { 0.2f, 0.3f, 0.3f, 1.0f };
		Scene::ForwardData& data = scene.GetForwardData();
		GraphicPipeline& pipeline = scene.GetPipeline();
		VkCommandBuffer cmd = renderer.GetActiveCommandBuffer();
		pipeline.BindPipeline(cmd, renderer.GetSwapchainImageIndex(), clear_color);
		for(auto& actor : scene.GetActors())
		{
			data.matrices_set[renderer.GetCurrentFrameIndex()]->Bind(cmd, pipeline.GetPipelineBindPoint(), pipeline.GetPipelineLayout());
			actor.GetModel().Draw(cmd, renderer.GetDrawCallsCounterRef());
		}
		pipeline.EndPipeline(cmd);
	}
}
