/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderer.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 20:24:11 by maldavid          #+#    #+#             */
/*   Updated: 2024/07/11 21:43:31 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kvf.h>
#include <Renderer/Renderer.h>
#include <Core/Logs.h>

#include <SDL2/SDL_vulkan.h>

#include <Core/Engine.h>
#include <Renderer/Pipelines/Shader.h>

#include <Core/EventBus.h>

namespace Scop
{
	namespace Internal
	{
		struct ResizeEventBroadcast : public EventBase
		{
			std::uint32_t What() const override { return 56; }
		};
	}

	void Renderer::Init(NonOwningPtr<Window> window)
	{
		Verify(ScopEngine::IsInit(), "ScopEngine is not init (wtf)");

		std::function<void(const EventBase&)> functor = [this](const EventBase& event)
		{
			if(event.What() == 24)
				this->RequireFramebufferResize();
		};
		EventBus::RegisterListener({ functor, "Renderer" });

		m_window_ptr = window;
		auto& render_core = RenderCore::Get();
		if(SDL_Vulkan_CreateSurface(m_window_ptr->GetSDLWindow(), render_core.GetInstance(), &m_surface) != SDL_TRUE)
			FatalError("Vulkan : failed to create a surface : %", SDL_GetError());
		Message("Vulkan : surface created");

		CreateSwapchain();

		std::vector<VkAttachmentDescription> attachements;

		for(std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_image_available_semaphores[i] = kvfCreateSemaphore(render_core.GetDevice());
			Message("Vulkan : image available semaphore created");
			m_render_finished_semaphores[i] = kvfCreateSemaphore(render_core.GetDevice());
			Message("Vulkan : render finished semaphore created");
			m_cmd_buffers[i] = kvfCreateCommandBuffer(render_core.GetDevice());
			Message("Vulkan : command buffer created");
			m_cmd_fences[i] = kvfCreateFence(render_core.GetDevice());
			Message("Vulkan : fence created");
		}

		ShaderLayout vertex_shader_layout(
			{
				{ 0,
					ShaderSetLayout({ 
						{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER }
					})
				}
			}, { ShaderPushConstantLayout({ 0, 16 }) }
		);
		m_internal_shaders.emplace_back(LoadShaderFromFile(ScopEngine::Get().GetAssetsPath() / "Shaders/Build/Vertex.spv", ShaderType::Vertex, std::move(vertex_shader_layout)));

		ShaderLayout default_fragment_shader_layout(
			{
				{ 1,
					ShaderSetLayout({ 
						{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER }
					})
				}
			}, {}
		);
		m_internal_shaders.emplace_back(LoadShaderFromFile(ScopEngine::Get().GetAssetsPath() / "Shaders/Build/DefaultFragment.spv", ShaderType::Fragment, std::move(default_fragment_shader_layout)));
	}

	bool Renderer::BeginFrame()
	{
		vkWaitForFences(RenderCore::Get().GetDevice(), 1, &m_cmd_fences[m_current_frame_index], VK_TRUE, UINT64_MAX);
		VkResult result = vkAcquireNextImageKHR(RenderCore::Get().GetDevice(), m_swapchain, UINT64_MAX, m_image_available_semaphores[m_current_frame_index], VK_NULL_HANDLE, &m_swapchain_image_index);
		if(result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			kvfDestroySwapchainKHR(RenderCore::Get().GetDevice(), m_swapchain);
			CreateSwapchain();
			EventBus::SendBroadcast(Internal::ResizeEventBroadcast{});
			return false;
		}
		else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			FatalError("Vulkan error : failed to acquire swapchain image, %", kvfVerbaliseVkResult(result));

		vkResetCommandBuffer(m_cmd_buffers[m_current_frame_index], 0);
		kvfBeginCommandBuffer(m_cmd_buffers[m_current_frame_index], 0);
		return true;
	}

	void Renderer::EndFrame()
	{
		VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		kvfEndCommandBuffer(m_cmd_buffers[m_current_frame_index]);
		kvfSubmitCommandBuffer(m_cmd_buffers[m_current_frame_index], RenderCore::Get().GetDevice(), KVF_GRAPHICS_QUEUE, m_render_finished_semaphores[m_current_frame_index], m_image_available_semaphores[m_current_frame_index], m_cmd_fences[m_current_frame_index], wait_stages);
		if(!kvfQueuePresentKHR(RenderCore::Get().GetDevice(), m_render_finished_semaphores[m_current_frame_index], m_swapchain, &m_swapchain_image_index) || m_framebuffers_resize)
		{
			m_framebuffers_resize = false;
			kvfDestroySwapchainKHR(RenderCore::Get().GetDevice(), m_swapchain);
			CreateSwapchain();
			EventBus::SendBroadcast(Internal::ResizeEventBroadcast{});
		}
		m_current_frame_index = (m_current_frame_index + 1) % MAX_FRAMES_IN_FLIGHT;
		kvfResetDeviceDescriptorPools(RenderCore::Get().GetDevice());
	}

	void Renderer::CreateSwapchain()
	{
		int width, height;
		SDL_Vulkan_GetDrawableSize(m_window_ptr->GetSDLWindow(), &width, &height);
		VkExtent2D extent = { static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height) };
		m_swapchain = kvfCreateSwapchainKHR(RenderCore::Get().GetDevice(), RenderCore::Get().GetPhysicalDevice(), m_surface, extent, true);
		Message("Vulkan : swapchain created");
	}

	void Renderer::Destroy() noexcept
	{
		auto& render_core = RenderCore::Get();

		m_internal_shaders.clear();

		for(std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			kvfDestroySemaphore(render_core.GetDevice(), m_image_available_semaphores[i]);
			Message("Vulkan : image available semaphore destroyed");
			kvfDestroySemaphore(render_core.GetDevice(), m_render_finished_semaphores[i]);
			Message("Vulkan : render finished semaphore destroyed");
			kvfDestroyFence(render_core.GetDevice(), m_cmd_fences[i]);
			Message("Vulkan : fence destroyed");
		}

		kvfDestroySwapchainKHR(render_core.GetDevice(), m_swapchain);
		Message("Vulkan : swapchain destroyed");
		vkDestroySurfaceKHR(render_core.GetInstance(), m_surface, nullptr);
		Message("Vulkan : surface destroyed");
		m_surface = VK_NULL_HANDLE;
	}
}
