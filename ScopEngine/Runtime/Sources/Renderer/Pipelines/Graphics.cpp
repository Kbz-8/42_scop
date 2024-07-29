#include <Renderer/Pipelines/Graphics.h>
#include <Renderer/RenderCore.h>
#include <Renderer/Renderer.h>
#include <Core/EventBus.h>
#include <Core/Logs.h>
#include <Renderer/Vertex.h>

namespace Scop
{
	GraphicPipeline::GraphicPipeline() : p_builder(kvfCreateGPipelineBuilder(), GraphicPipeline::KvfGraphicsPipelineBuilderDestructor{}) {}

	void GraphicPipeline::Init(std::shared_ptr<Shader> vertex_shader, std::shared_ptr<Shader> fragment_shader, NonOwningPtr<Renderer> renderer)
	{
		std::function<void(const EventBase&)> functor = [this, renderer](const EventBase& event)
		{
			if(event.What() == 56)
			{
				for(auto& fb : m_framebuffers)
					kvfDestroyFramebuffer(RenderCore::Get().GetDevice(), fb);
				m_framebuffers.clear();
				kvfDestroyRenderPass(RenderCore::Get().GetDevice(), m_renderpass);
				m_depth.Destroy();
				auto extent = kvfGetSwapchainImagesSize(renderer->GetSwapchain());
				m_depth.Init(extent.width, extent.height);
				TransitionAttachments();
				CreateFramebuffers(renderer->GetSwapchainImages());
			}
		};
		EventBus::RegisterListener({ functor, std::to_string((std::uintptr_t)(void**)this) });

		m_is_swapchain = true;
		Init(std::move(vertex_shader), std::move(fragment_shader), renderer->GetSwapchainImages());
	}

	void GraphicPipeline::Init(std::shared_ptr<Shader> vertex_shader, std::shared_ptr<Shader> fragment_shader, std::vector<Image> attachments)
	{
		if(!vertex_shader || !fragment_shader)
			FatalError("Vulkan : invalid shaders");

		p_vertex_shader = vertex_shader;
		p_fragment_shader = fragment_shader;

		std::vector<VkPushConstantRange> push_constants;
		std::vector<VkDescriptorSetLayout> set_layouts;
		push_constants.insert(push_constants.end(), vertex_shader->GetPipelineLayout().push_constants.begin(), vertex_shader->GetPipelineLayout().push_constants.end());
		push_constants.insert(push_constants.end(), fragment_shader->GetPipelineLayout().push_constants.begin(), fragment_shader->GetPipelineLayout().push_constants.end());
		set_layouts.insert(set_layouts.end(), vertex_shader->GetPipelineLayout().set_layouts.begin(), vertex_shader->GetPipelineLayout().set_layouts.end());
		set_layouts.insert(set_layouts.end(), fragment_shader->GetPipelineLayout().set_layouts.begin(), fragment_shader->GetPipelineLayout().set_layouts.end());
		m_pipeline_layout = kvfCreatePipelineLayout(RenderCore::Get().GetDevice(), set_layouts.data(), set_layouts.size(), push_constants.data(), push_constants.size());

		m_depth.Init(attachments.back().GetWidth(), attachments.back().GetHeight());

		TransitionAttachments();
		CreateFramebuffers(attachments);

		kvfGPipelineBuilderAddShaderStage(p_builder.get(), vertex_shader->GetShaderStage(), vertex_shader->GetShaderModule(), "main");
		kvfGPipelineBuilderAddShaderStage(p_builder.get(), fragment_shader->GetShaderStage(), fragment_shader->GetShaderModule(), "main");
		kvfGPipelineBuilderSetInputTopology(p_builder.get(), VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		kvfGPipelineBuilderSetPolygonMode(p_builder.get(), VK_POLYGON_MODE_FILL, 1.0f);
		kvfGPipelineBuilderSetCullMode(p_builder.get(), VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
		kvfGPipelineBuilderDisableBlending(p_builder.get());
		kvfGPipelineBuilderEnableDepthTest(p_builder.get(), VK_COMPARE_OP_LESS, true);

		VkVertexInputBindingDescription binding_description = Vertex::GetBindingDescription();
		auto attributes_description = Vertex::GetAttributeDescriptions();
		kvfGPipelineBuilderSetVertexInputs(p_builder.get(), binding_description, attributes_description.data(), attributes_description.size());

		m_pipeline = kvfCreateGraphicsPipeline(RenderCore::Get().GetDevice(), m_pipeline_layout, p_builder.get(), m_renderpass);
	}

	bool GraphicPipeline::BindPipeline(VkCommandBuffer command_buffer, std::size_t framebuffer_index) noexcept
	{
		VkFramebuffer fb = m_framebuffers[framebuffer_index];
		VkExtent2D fb_extent = kvfGetFramebufferSize(fb);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = fb_extent.width;
		viewport.height = fb_extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(command_buffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = fb_extent;
		vkCmdSetScissor(command_buffer, 0, 1, &scissor);

		vkCmdBindPipeline(command_buffer, GetPipelineBindPoint(), GetPipeline());
		return true;
	}

	void GraphicPipeline::EndPipeline(VkCommandBuffer command_buffer) noexcept
	{
		vkCmdEndRenderPass(command_buffer);
	}

	void GraphicPipeline::Destroy() noexcept
	{
		p_vertex_shader.reset();
		p_fragment_shader.reset();
		p_builder.reset();
		for(auto& fb : m_framebuffers)
			kvfDestroyFramebuffer(RenderCore::Get().GetDevice(), fb);
		m_framebuffers.clear();
		kvfDestroyPipelineLayout(RenderCore::Get().GetDevice(), m_pipeline_layout);
		kvfDestroyRenderPass(RenderCore::Get().GetDevice(), m_renderpass);
		kvfDestroyPipeline(RenderCore::Get().GetDevice(), m_pipeline);
	}

	void GraphicPipeline::CreateFramebuffers(const std::vector<Image>& render_targets)
	{
		std::vector<VkAttachmentDescription> attachments;
		for(const auto& image : render_targets)
			attachments.push_back(kvfBuildAttachmentDescription((kvfIsDepthFormat(image.GetFormat()) ? KVF_IMAGE_DEPTH : KVF_IMAGE_COLOR), image.GetFormat(), image.GetLayout(), image.GetLayout(), true));
		m_renderpass = kvfCreateRenderPass(RenderCore::Get().GetDevice(), attachments.data(), attachments.size(), GetPipelineBindPoint());

		for(const auto& image : render_targets)
		{
			if(kvfIsDepthFormat(image.GetFormat()))
				continue;
			m_framebuffers.push_back(kvfCreateFramebuffer(RenderCore::Get().GetDevice(), m_renderpass, image.GetImageView(), { .width = image.GetWidth(), .height = image.GetHeight() }));
		}
	}

	void GraphicPipeline::TransitionAttachments()
	{
		m_depth.TransitionLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}
}
