#include <Renderer/RenderPasses/FinalPass.h>
#include <Renderer/Pipelines/Graphics.h>
#include <Renderer/Renderer.h>
#include <Graphics/Scene.h>
#include <Core/EventBus.h>
#include <Core/Engine.h>

namespace Scop
{
	void FinalPass::Init()
	{
		ShaderLayout vertex_shader_layout(
			{}, {}
		);
		p_vertex_shader = LoadShaderFromFile(ScopEngine::Get().GetAssetsPath() / "Shaders/Build/ScreenVertex.spv", ShaderType::Vertex, std::move(vertex_shader_layout));
		ShaderLayout fragment_shader_layout(
			{
				{ 0,
					ShaderSetLayout({ 
						{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER }
					})
				}
			}, {}
		);
		p_fragment_shader = LoadShaderFromFile(ScopEngine::Get().GetAssetsPath() / "Shaders/Build/ScreenFragment.spv", ShaderType::Fragment, std::move(fragment_shader_layout));

		std::function<void(const EventBase&)> functor = [this](const EventBase& event)
		{
			if(event.What() == 56)
				m_pipeline.Destroy();
		};
		EventBus::RegisterListener({ functor, "__ScopFinalPass" });

		p_set = std::make_shared<DescriptorSet>(p_fragment_shader->GetShaderLayout().set_layouts[0].second, p_fragment_shader->GetPipelineLayout().set_layouts[0], ShaderType::Fragment);
	}

	void FinalPass::Pass(Scene& scene, Renderer& renderer, Texture& render_target)
	{
		if(m_pipeline.GetPipeline() == VK_NULL_HANDLE)
			m_pipeline.Init(p_vertex_shader, p_fragment_shader, &renderer, nullptr, VK_CULL_MODE_NONE, true);

		VkCommandBuffer cmd = renderer.GetActiveCommandBuffer();

		p_set->SetImage(renderer.GetCurrentFrameIndex(), 0, render_target);
		p_set->Update(renderer.GetCurrentFrameIndex(), cmd);

		std::array<float, 4> clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_pipeline.BindPipeline(cmd, renderer.GetSwapchainImageIndex(), clear_color);
			VkDescriptorSet set = p_set->GetSet(renderer.GetCurrentFrameIndex());
			vkCmdBindDescriptorSets(cmd, m_pipeline.GetPipelineBindPoint(), m_pipeline.GetPipelineLayout(), 0, 1, &set, 0, nullptr);
			vkCmdDraw(cmd, 3, 1, 0, 0);
			renderer.GetDrawCallsCounterRef()++;
			renderer.GetPolygonDrawnCounterRef()++;
		m_pipeline.EndPipeline(cmd);
	}

	void FinalPass::Destroy()
	{
		m_pipeline.Destroy();
		p_vertex_shader.reset();
		p_fragment_shader.reset();
		p_set.reset();
	}
}
