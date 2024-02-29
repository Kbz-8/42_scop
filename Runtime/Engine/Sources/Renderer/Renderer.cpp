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

		int width, height;
		SDL_Vulkan_GetDrawableSize(m_window_ptr->GetSDLWindow(), &width, &height);
		VkExtent2D extent = { static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height) };
		m_swapchain = kvfCreateSwapchainKHR(render_core.GetDevice(), render_core.GetPhysicalDevice(), m_surface, extent, true);

		for(std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
			m_semaphores[i] = kvfCreateSemaphore(render_core.GetDevice());
	}

	void Renderer::Destroy() noexcept
	{
		auto& render_core = RenderCore::Get();

		for(std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
			kvfDestroySemaphore(render_core.GetDevice(), m_semaphores[i]);

		kvfDestroySwapchainKHR(render_core.GetDevice(), m_swapchain);
		vkDestroySurfaceKHR(render_core.GetInstance(), m_surface, nullptr);
		m_surface = VK_NULL_HANDLE;
	}
}
