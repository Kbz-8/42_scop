#include <Renderer/Pipelines/Graphics.h>
#include <Renderer/RenderCore.h>
#include <Renderer/Renderer.h>
#include <Core/EventBus.h>
#include <Core/Logs.h>
#include <Renderer/Vertex.h>

namespace Scop
{
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
		attachments.push_back(m_depth);
		CreateFramebuffers(attachments);

		p_builder = kvfCreateGPipelineBuilder();
		kvfGPipelineBuilderAddShaderStage(p_builder, vertex_shader->GetShaderStage(), vertex_shader->GetShaderModule(), "main");
		kvfGPipelineBuilderAddShaderStage(p_builder, fragment_shader->GetShaderStage(), fragment_shader->GetShaderModule(), "main");
		kvfGPipelineBuilderSetInputTopology(p_builder, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		kvfGPipelineBuilderSetPolygonMode(p_builder, VK_POLYGON_MODE_FILL, 1.0f);
		kvfGPipelineBuilderSetCullMode(p_builder, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
		kvfGPipelineBuilderDisableBlending(p_builder);
		kvfGPipelineBuilderEnableDepthTest(p_builder, VK_COMPARE_OP_LESS, true);

		VkVertexInputBindingDescription binding_description = Vertex::GetBindingDescription();
		auto attributes_description = Vertex::GetAttributeDescriptions();
		kvfGPipelineBuilderSetVertexInputs(p_builder, binding_description, attributes_description.data(), attributes_description.size());

		m_pipeline = kvfCreateGraphicsPipeline(RenderCore::Get().GetDevice(), m_pipeline_layout, p_builder, m_renderpass);
		Message("Vulkan : graphics pipeline created");
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
		m_depth.Destroy();
		p_vertex_shader.reset();
		p_fragment_shader.reset();
		kvfDestroyGPipelineBuilder(p_builder);
		for(auto& fb : m_framebuffers)
		{
			kvfDestroyFramebuffer(RenderCore::Get().GetDevice(), fb);
			Message("Vulkan : framebuffer destroyed");
		}
		m_framebuffers.clear();
		kvfDestroyPipelineLayout(RenderCore::Get().GetDevice(), m_pipeline_layout);
		Message("Vulkan : graphics pipeline layout destroyed");
		kvfDestroyRenderPass(RenderCore::Get().GetDevice(), m_renderpass);
		Message("Vulkan : renderpass destroyed");
		kvfDestroyPipeline(RenderCore::Get().GetDevice(), m_pipeline);
		Message("Vulkan : graphics pipeline destroyed");
	}

	void GraphicPipeline::CreateFramebuffers(const std::vector<Image>& render_targets)
	{
		bool swapchain_image_found = false;
		std::vector<VkAttachmentDescription> attachments;
		std::vector<VkImageView> attachment_views;
		for(const auto& image : render_targets)
		{
			if(image.GetLayout() == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR && swapchain_image_found)
				continue;
			else if(image.GetLayout() == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
				swapchain_image_found = true;
			attachments.push_back(kvfBuildAttachmentDescription((kvfIsDepthFormat(image.GetFormat()) ? KVF_IMAGE_DEPTH : KVF_IMAGE_COLOR), image.GetFormat(), image.GetLayout(), image.GetLayout(), true));
			attachment_views.push_back(image.GetImageView());
		}
		m_renderpass = kvfCreateRenderPass(RenderCore::Get().GetDevice(), attachments.data(), attachments.size(), GetPipelineBindPoint());
		Message("Vulkan : renderpass created");

		for(const auto& image : render_targets)
		{
			m_framebuffers.push_back(kvfCreateFramebuffer(RenderCore::Get().GetDevice(), m_renderpass, attachment_views.data(), attachment_views.size(), { .width = image.GetWidth(), .height = image.GetHeight() }));
			Message("Vulkan : framebuffer created");
		}
	}

	void GraphicPipeline::TransitionAttachments()
	{
		m_depth.TransitionLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}
}
