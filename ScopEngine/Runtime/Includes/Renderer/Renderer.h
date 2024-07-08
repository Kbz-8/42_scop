#ifndef __SCOP_RENDERER__
#define __SCOP_RENDERER__

#include <Platform/Window.h>
#include <Utils/NonOwningPtr.h>
#include <Renderer/RenderCore.h>

#include <kvf.h>

#include <array>
#include <vector>
#include <memory>

namespace Scop
{
	// Ugly fat god class but I don't care
	class Renderer
	{
		public:
			Renderer() = default;

			void Init(Window* window);

			bool BeginFrame();
			void EndFrame();

			inline VkSwapchainKHR GetSwapchain() const noexcept { return m_swapchain; }
			inline VkSurfaceKHR GetSurface() const noexcept { return m_surface; }
			inline VkRenderPass GetRenderPass() const noexcept { return m_render_pass; }
			inline VkSemaphore GetSemaphore(int index) const noexcept { return m_semaphores[index]; }
			inline VkCommandBuffer GetCommandBuffer(int index) const noexcept { return m_cmd_buffers[index]; }
			inline VkCommandBuffer GetActiveCommandBuffer() const noexcept { return m_cmd_buffers[m_current_frame_index]; }

			void Destroy() noexcept;

			~Renderer() = default;

		private:
			std::vector<std::shared_ptr<class Shader> > m_internal_shaders;
			std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> m_semaphores;
			std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> m_cmd_buffers;
			std::array<VkFence, MAX_FRAMES_IN_FLIGHT> m_cmd_fences;
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
