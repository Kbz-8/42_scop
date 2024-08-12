#include <Renderer/RenderPasses/ForwardPass.h>
#include <Renderer/Pipelines/Graphics.h>
#include <Renderer/Renderer.h>
#include <Graphics/Scene.h>
#include <Maths/Mat4.h>

namespace Scop
{
	struct ModelData
	{
		Mat4f model_mat;
		Mat4f normal_mat;
	};

	void ForwardPass::Pass(Scene& scene, Renderer& renderer)
	{
		std::array<float, 4> clear_color = { 0.2f, 0.3f, 0.3f, 1.0f };
		Scene::ForwardData& data = scene.GetForwardData();
		GraphicPipeline& pipeline = scene.GetPipeline();
		VkCommandBuffer cmd = renderer.GetActiveCommandBuffer();
		pipeline.BindPipeline(cmd, renderer.GetSwapchainImageIndex(), clear_color);
		for(auto& actor : scene.GetActors())
		{
			ModelData model_data;
			model_data.model_mat = Mat4f::Translate(actor.GetPosition());
			model_data.normal_mat = Mat4f::Identity();
			vkCmdPushConstants(cmd, pipeline.GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ModelData), &model_data);
			data.matrices_set[renderer.GetCurrentFrameIndex()]->Bind(cmd, pipeline.GetPipelineBindPoint(), pipeline.GetPipelineLayout());
			actor.GetModel().Draw(cmd, renderer.GetDrawCallsCounterRef());
		}
		pipeline.EndPipeline(cmd);
	}
}
