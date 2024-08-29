#include <Renderer/RenderPasses/SkyboxPass.h>
#include <Renderer/Pipelines/Graphics.h>
#include <Graphics/MeshFactory.h>
#include <Renderer/ViewerData.h>
#include <Renderer/Renderer.h>
#include <Graphics/Scene.h>
#include <Core/EventBus.h>
#include <Core/Engine.h>

namespace Scop
{
	void SkyboxPass::Init()
	{
		ShaderLayout vertex_shader_layout(
			{
				{ 0,
					ShaderSetLayout({ 
						{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER }
					})
				}
			}, {}
		);
		p_vertex_shader = LoadShaderFromFile(ScopEngine::Get().GetAssetsPath() / "Shaders/Build/SkyboxVertex.spv", ShaderType::Vertex, std::move(vertex_shader_layout));
		ShaderLayout fragment_shader_layout(
			{
				{ 1,
					ShaderSetLayout({ 
						{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER }
					})
				}
			}, {}
		);
		p_fragment_shader = LoadShaderFromFile(ScopEngine::Get().GetAssetsPath() / "Shaders/Build/SkyboxFragment.spv", ShaderType::Fragment, std::move(fragment_shader_layout));

		std::function<void(const EventBase&)> functor = [this](const EventBase& event)
		{
			if(event.What() == 56)
				m_pipeline.Destroy(); // Ugly but f*ck off
		};
		EventBus::RegisterListener({ functor, "__ScopSkyboxPass" });

		m_screen_quad = CreateQuad();
		p_set = std::make_shared<DescriptorSet>(p_fragment_shader->GetShaderLayout().set_layouts[0].second, p_fragment_shader->GetPipelineLayout().set_layouts[0], ShaderType::Fragment);
	}

	void SkyboxPass::Pass(Scene& scene, Renderer& renderer, class Texture& render_target)
	{
		if(m_pipeline.GetPipeline() == VK_NULL_HANDLE)
			m_pipeline.Init(p_vertex_shader, p_fragment_shader, { &render_target, &scene.GetDepth() }, VK_CULL_MODE_NONE);

		std::array<float, 4> clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
		VkCommandBuffer cmd = renderer.GetActiveCommandBuffer();
		m_pipeline.BindPipeline(cmd, 0, clear_color);
			std::array<VkDescriptorSet, 2> sets = { scene.GetForwardData().matrices_set->GetSet(renderer.GetCurrentFrameIndex()), p_set->GetSet(renderer.GetCurrentFrameIndex()) };
			vkCmdBindDescriptorSets(cmd, m_pipeline.GetPipelineBindPoint(), m_pipeline.GetPipelineLayout(), 0, sets.size(), sets.data(), 0, nullptr);
			m_screen_quad->Draw(cmd, renderer.GetDrawCallsCounterRef(), renderer.GetPolygonDrawnCounterRef());
		m_pipeline.EndPipeline(cmd);
	}

	void SkyboxPass::Destroy()
	{
		m_pipeline.Destroy();
		p_vertex_shader.reset();
		p_fragment_shader.reset();
		m_screen_quad.reset();
		p_set.reset();
	}
}
