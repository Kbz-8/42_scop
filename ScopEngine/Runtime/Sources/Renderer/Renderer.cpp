#include <kvf.h>
#include <Renderer/Renderer.h>
#include <Core/Logs.h>

#include <SDL2/SDL_vulkan.h>

namespace Scop
{
	void Renderer::Init(Window* window)
	{
		m_window_ptr = window;
		auto& render_core = RenderCore::Get();
		if(SDL_Vulkan_CreateSurface(m_window_ptr->GetSDLWindow(), render_core.GetInstance(), &m_surface) != SDL_TRUE)
			FatalError("Vulkan : failed to create a surface : %", SDL_GetError());
		Message("Vulkan : surface created");

		int width, height;
		SDL_Vulkan_GetDrawableSize(m_window_ptr->GetSDLWindow(), &width, &height);
		VkExtent2D extent = { static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height) };
		m_swapchain = kvfCreateSwapchainKHR(render_core.GetDevice(), render_core.GetPhysicalDevice(), m_surface, extent, true);
		Message("Vulkan : swapchain created");

		std::vector<VkAttachmentDescription> attachements;

		for(std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_semaphores[i] = kvfCreateSemaphore(render_core.GetDevice());
			Message("Vulkan : semaphore created");
			m_cmd_buffers[i] = kvfCreateCommandBuffer(render_core.GetDevice());
			Message("Vulkan : command buffer created");
			m_cmd_fences[i] = kvfCreateFence(render_core.GetDevice());
			Message("Vulkan : fence created");
		}
	}

	bool Renderer::BeginFrame()
	{
		return true;
	}

	void Renderer::EndFrame()
	{
	}

	void Renderer::Destroy() noexcept
	{
		auto& render_core = RenderCore::Get();

		for(std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			kvfDestroySemaphore(render_core.GetDevice(), m_semaphores[i]);
			Message("Vulkan : semaphore destroyed");
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
