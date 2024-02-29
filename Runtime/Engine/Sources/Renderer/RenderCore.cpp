#define KVF_IMPLEMENTATION
#ifdef DEBUG
	#define KVF_ENABLE_VALIDATION_LAYERS
#endif
#include <kvf.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <vector>
#include <cstdint>

#include <Renderer/RenderCore.h>
#include <Core/Logs.h>

namespace Scop
{
	void RenderCore::Init() noexcept
	{
		SDL_Window* win = SDL_CreateWindow("", 0, 0, 1, 1, SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN);
		if(!win)
			FatalError("Vulkan : cannot get instance extentions from window : %",  SDL_GetError());

		std::uint32_t count;
		if(!SDL_Vulkan_GetInstanceExtensions(win, &count, nullptr))
			FatalError("Vulkan : cannot get instance extentions from window : %",  SDL_GetError());

		std::vector<const char*> extensions = { VK_EXT_DEBUG_REPORT_EXTENSION_NAME };
		size_t additional_extension_count = extensions.size();
		extensions.resize(additional_extension_count + count);

		if(!SDL_Vulkan_GetInstanceExtensions(win, &count, extensions.data() + additional_extension_count))
			FatalError("Vulkan : cannot get instance extentions from window : %", SDL_GetError());

		m_instance = kvfCreateInstance(extensions.data(), extensions.size());

		VkSurfaceKHR surface = VK_NULL_HANDLE;
		SDL_Vulkan_CreateSurface(win, m_instance, &surface);

		m_physical_device = kvfPickGoodDefaultPhysicalDevice(m_instance, surface);
		m_device = kvfCreateDefaultDevice(m_physical_device);

		vkDestroySurfaceKHR(m_instance, surface, nullptr);
		SDL_DestroyWindow(win);
	}

	void RenderCore::Destroy() noexcept
	{
		kvfDestroyDevice(m_device);
		kvfDestroyInstance(m_instance);
	}
}
