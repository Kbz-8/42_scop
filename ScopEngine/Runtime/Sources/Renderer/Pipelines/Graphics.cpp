#include <Renderer/Pipelines/Graphics.h>
#include <Renderer/RenderCore.h>
#include <Core/EventBus.h>
#include <Core/Logs.h>

namespace Scop
{
	void GraphicPipeline::Init(std::shared_ptr<Shader> vertex_shader, std::shared_ptr<Shader> fragment_shader)
	{
		if(!vertex_shader || !fragment_shader)
			FatalError("Vulkan : invalid shaders");

		std::function<void(const EventBase&)> functor = [this](const EventBase& event)
		{
			if(event.What() == 56)
			{
				for(auto& fb : m_framebuffers)
					kvfDestroyFrameBuffer(RenderCore::Get().GetDevice(), fb);
				m_framebuffers.clear();
				m_renderpass.Destroy();
				m_depth.Destroy();
				m_depth.Init(m_renderer->GetSwapchain().GetExtent().width, m_renderer->GetSwapchain().GetExtent().height);
				TransitionAttachments();
				CreateFramebuffers();
			}
		};
		EventBus::RegisterListener({ functor, std::to_string((std::uintptr_t)(void**)this) });

		std::vector<VkPushConstantRange> push_constants;
		std::vector<VkDescriptorSetLayout> set_layouts;
		push_constants.insert(push_constants.end(), vertex_shader->GetPipelineLayout().push_constants.begin(), vertex_shader->GetPipelineLayout().push_constants.end());
		push_constants.insert(push_constants.end(), fragment_shader->GetPipelineLayout().push_constants.begin(), fragment_shader->GetPipelineLayout().push_constants.end());
		set_layouts.insert(set_layouts.end(), vertex_shader->GetPipelineLayout().set_layouts.begin(), vertex_shader->GetPipelineLayout().set_layouts.end());
		set_layouts.insert(set_layouts.end(), fragment_shader->GetPipelineLayout().set_layouts.begin(), fragment_shader->GetPipelineLayout().set_layouts.end());
		m_pipeline_layout = kvfCreatePipelineLayout(RenderCore::Get().GetDevice(), set_layouts.data(), set_layouts.size(), push_constants.data(), push_constants.size());
	}

	bool GraphicPipeline::BindPipeline(VkCommandBuffer command_buffer) noexcept
	{
	}

	void GraphicPipeline::EndPipeline(VkCommandBuffer command_buffer) noexcept
	{
	}

	void GraphicPipeline::Destroy() noexcept
	{
		m_vertex_shader.reset();
		m_fragment_shader.reset();
	}

	const VkPipelineLayout& GraphicPipeline::GetPipelineLayout() const
	{
	}

	void GraphicPipeline::CreateFramebuffers()
	{
	}

	void GraphicPipeline::TransitionAttachments()
	{
	}
}
