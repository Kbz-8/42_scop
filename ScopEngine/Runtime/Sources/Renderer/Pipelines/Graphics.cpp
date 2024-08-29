#include <Renderer/Pipelines/Graphics.h>
#include <Renderer/RenderCore.h>
#include <Renderer/Renderer.h>
#include <Renderer/Vertex.h>
#include <Core/EventBus.h>
#include <Core/Logs.h>

namespace Scop
{
	void GraphicPipeline::Init(std::shared_ptr<Shader> vertex_shader, std::shared_ptr<Shader> fragment_shader, NonOwningPtr<Renderer> renderer, NonOwningPtr<DepthImage> depth, VkCullModeFlagBits culling, bool disable_vertex_inputs)
	{
		p_renderer = renderer;
		p_depth = depth;
		std::function<void(const EventBase&)> functor = [this, renderer](const EventBase& event)
		{
			if(event.What() == 56)
			{
				for(auto& fb : m_framebuffers)
					kvfDestroyFramebuffer(RenderCore::Get().GetDevice(), fb);
				m_framebuffers.clear();
				kvfDestroyRenderPass(RenderCore::Get().GetDevice(), m_renderpass);
				p_depth->Destroy();
				auto extent = kvfGetSwapchainImagesSize(renderer->GetSwapchain());
				p_depth->Init(extent.width, extent.height);
				TransitionAttachments();
				CreateFramebuffers({});
			}
		};
		EventBus::RegisterListener({ functor, std::to_string((std::uintptr_t)(void**)this) });

		Init(std::move(vertex_shader), std::move(fragment_shader), std::vector<Image>{}, culling, disable_vertex_inputs);
	}

	void GraphicPipeline::Init(std::shared_ptr<Shader> vertex_shader, std::shared_ptr<Shader> fragment_shader, std::vector<Image> attachments, VkCullModeFlagBits culling, bool disable_vertex_inputs)
	{
		if(!vertex_shader || !fragment_shader)
			FatalError("Vulkan : invalid shaders");

		m_attachments = std::move(attachments);
		p_vertex_shader = vertex_shader;
		p_fragment_shader = fragment_shader;

		std::vector<VkPushConstantRange> push_constants;
		std::vector<VkDescriptorSetLayout> set_layouts;
		push_constants.insert(push_constants.end(), vertex_shader->GetPipelineLayout().push_constants.begin(), vertex_shader->GetPipelineLayout().push_constants.end());
		push_constants.insert(push_constants.end(), fragment_shader->GetPipelineLayout().push_constants.begin(), fragment_shader->GetPipelineLayout().push_constants.end());
		set_layouts.insert(set_layouts.end(), vertex_shader->GetPipelineLayout().set_layouts.begin(), vertex_shader->GetPipelineLayout().set_layouts.end());
		set_layouts.insert(set_layouts.end(), fragment_shader->GetPipelineLayout().set_layouts.begin(), fragment_shader->GetPipelineLayout().set_layouts.end());
		m_pipeline_layout = kvfCreatePipelineLayout(RenderCore::Get().GetDevice(), set_layouts.data(), set_layouts.size(), push_constants.data(), push_constants.size());

		TransitionAttachments();
		CreateFramebuffers(m_attachments);

		KvfGraphicsPipelineBuilder* builder = kvfCreateGPipelineBuilder();
		kvfGPipelineBuilderAddShaderStage(builder, vertex_shader->GetShaderStage(), vertex_shader->GetShaderModule(), "main");
		kvfGPipelineBuilderAddShaderStage(builder, fragment_shader->GetShaderStage(), fragment_shader->GetShaderModule(), "main");
		kvfGPipelineBuilderSetInputTopology(builder, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		kvfGPipelineBuilderSetPolygonMode(builder, VK_POLYGON_MODE_FILL, 1.0f);
		kvfGPipelineBuilderSetCullMode(builder, culling, VK_FRONT_FACE_CLOCKWISE);
		kvfGPipelineBuilderDisableBlending(builder);
		kvfGPipelineBuilderEnableDepthTest(builder, VK_COMPARE_OP_LESS, true);

		if(!disable_vertex_inputs)
		{
			VkVertexInputBindingDescription binding_description = Vertex::GetBindingDescription();
			auto attributes_description = Vertex::GetAttributeDescriptions();
			kvfGPipelineBuilderSetVertexInputs(builder, binding_description, attributes_description.data(), attributes_description.size());
		}

		m_pipeline = kvfCreateGraphicsPipeline(RenderCore::Get().GetDevice(), m_pipeline_layout, builder, m_renderpass);
		Message("Vulkan : graphics pipeline created");
		kvfDestroyGPipelineBuilder(builder);
	}

	bool GraphicPipeline::BindPipeline(VkCommandBuffer command_buffer, std::size_t framebuffer_index, std::array<float, 4> clear) noexcept
	{
		TransitionAttachments(command_buffer);
		VkFramebuffer fb;
		if(p_renderer)
			fb = m_framebuffers[framebuffer_index];
		else
			fb = m_framebuffers[0];
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

		for(int i = 0; i < m_clears.size(); i++)
		{
			m_clears[i].color.float32[0] = clear[0];
			m_clears[i].color.float32[1] = clear[1];
			m_clears[i].color.float32[2] = clear[2];
			m_clears[i].color.float32[3] = clear[3];
		}

		m_clears.back().depthStencil = VkClearDepthStencilValue{ 1.0f, 0 };

		kvfBeginRenderPass(m_renderpass, command_buffer, fb, fb_extent, m_clears.data(), m_clears.size());
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
		for(auto& fb : m_framebuffers)
		{
			kvfDestroyFramebuffer(RenderCore::Get().GetDevice(), fb);
			Message("Vulkan : framebuffer destroyed");
		}
		m_framebuffers.clear();
		kvfDestroyPipelineLayout(RenderCore::Get().GetDevice(), m_pipeline_layout);
		m_pipeline_layout = VK_NULL_HANDLE;
		Message("Vulkan : graphics pipeline layout destroyed");
		kvfDestroyRenderPass(RenderCore::Get().GetDevice(), m_renderpass);
		m_renderpass = VK_NULL_HANDLE;
		Message("Vulkan : renderpass destroyed");
		kvfDestroyPipeline(RenderCore::Get().GetDevice(), m_pipeline);
		m_pipeline = VK_NULL_HANDLE;
		Message("Vulkan : graphics pipeline destroyed");
	}

	void GraphicPipeline::CreateFramebuffers(const std::vector<Image>& render_targets)
	{
		std::vector<VkAttachmentDescription> attachments;
		std::vector<VkImageView> attachment_views;
		if(p_renderer)
		{
			attachments.push_back(kvfBuildSwapchainAttachmentDescription(p_renderer->GetSwapchain(), true));
			attachment_views.push_back(p_renderer->GetSwapchainImages()[0].GetImageView());
		}
		else
		{
			for(const auto& image : render_targets)
			{
				attachments.push_back(kvfBuildAttachmentDescription((kvfIsDepthFormat(image.GetFormat()) ? KVF_IMAGE_DEPTH : KVF_IMAGE_COLOR), image.GetFormat(), image.GetLayout(), image.GetLayout(), true));
				attachment_views.push_back(image.GetImageView());
			}
		}

		if(p_depth)
		{
			attachments.push_back(kvfBuildAttachmentDescription((kvfIsDepthFormat(p_depth->GetFormat()) ? KVF_IMAGE_DEPTH : KVF_IMAGE_COLOR), p_depth->GetFormat(), p_depth->GetLayout(), p_depth->GetLayout(), true));
			attachment_views.push_back(p_depth->GetImageView());
		}

		m_renderpass = kvfCreateRenderPass(RenderCore::Get().GetDevice(), attachments.data(), attachments.size(), GetPipelineBindPoint());
		m_clears.clear();
		m_clears.resize(attachments.size());
		Message("Vulkan : renderpass created");

		if(p_renderer)
		{
			for(const Image& image : p_renderer->GetSwapchainImages())
			{
				attachment_views[0] = image.GetImageView();
				m_framebuffers.push_back(kvfCreateFramebuffer(RenderCore::Get().GetDevice(), m_renderpass, attachment_views.data(), attachment_views.size(), { .width = image.GetWidth(), .height = image.GetHeight() }));
				Message("Vulkan : framebuffer created");
			}
		}
		else
		{
			for(const auto& image : render_targets)
			{
				m_framebuffers.push_back(kvfCreateFramebuffer(RenderCore::Get().GetDevice(), m_renderpass, attachment_views.data(), attachment_views.size(), { .width = image.GetWidth(), .height = image.GetHeight() }));
				Message("Vulkan : framebuffer created");
			}
		}
	}

	void GraphicPipeline::TransitionAttachments(VkCommandBuffer cmd)
	{
		if(p_depth)
			p_depth->TransitionLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, cmd);

		for(auto& image : m_attachments)
		{
			if(!image.IsInit())
				continue;
			if(!kvfIsDepthFormat(image.GetFormat()))
				image.TransitionLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, cmd);
		}
	}
}
