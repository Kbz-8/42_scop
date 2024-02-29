#ifndef __SCOP_RENDERER__
#define __SCOP_RENDERER__

#include <Platform/Window.h>
#include <Utils/NonOwningPtr.h>
#include <Renderer/RenderCore.h>

#include <kvf.h>

#include <array>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Scop
{
	class Renderer
	{
		public:
			Renderer() = default;

			void Init(Window* window);

			inline VkSwapchainKHR GetSwapchain() const noexcept { return m_swapchain; }
			inline VkSurfaceKHR GetSurface() const noexcept { return m_surface; }
			inline VkRenderPass GetRenderPass() const noexcept { return m_render_pass; }
			inline VkSemaphore GetSemaphore(int index) const noexcept { return m_semaphores[index]; }
			inline VkFramebuffer GetFramebuffer(int index) const noexcept { return m_framebuffers[index]; }

			void Destroy() noexcept;

			~Renderer() = default;

		private:
			std::vector<VkFramebuffer> m_framebuffers;
			std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> m_semaphores;
			NonOwningPtr<Window> m_window_ptr;
			VkSurfaceKHR m_surface = VK_NULL_HANDLE;
			VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
			VkRenderPass m_render_pass = VK_NULL_HANDLE;
			std::uint32_t m_current_frame_index = 0;
			std::uint32_t m_swapchain_image_index = 0;
			bool m_framebuffer_resize_required = false;
	};
}

#endif
