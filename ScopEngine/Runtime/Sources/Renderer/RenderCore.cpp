#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <vector>
#include <cstdint>

#include <Core/Engine.h>
#include <Renderer/RenderCore.h>
#include <Renderer/Pipelines/Shader.h>
#include <Core/Logs.h>

#define KVF_IMPLEMENTATION
#ifdef DEBUG
	#define KVF_ENABLE_VALIDATION_LAYERS
#endif
#include <kvf.h>

namespace Scop
{
	std::optional<std::uint32_t> FindMemoryType(std::uint32_t type_filter, VkMemoryPropertyFlags properties, bool error)
	{
		VkPhysicalDeviceMemoryProperties mem_properties;
		vkGetPhysicalDeviceMemoryProperties(RenderCore::Get().GetPhysicalDevice(), &mem_properties);

		for(std::uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
		{
			if((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;
		}
		if(error)
			FatalError("Vulkan : failed to find suitable memory type");
		return std::nullopt;
	}

	void ErrorCallback(const char* message) noexcept
	{
		FatalError(message);
	}

	void ValidationErrorCallback(const char* message) noexcept
	{
		Error(message);
	}

	void ValidationWarningCallback(const char* message) noexcept
	{
		Warning(message);
	}

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

		kvfSetErrorCallback(&ErrorCallback);
		kvfSetValidationErrorCallback(&ValidationErrorCallback);
		kvfSetValidationWarningCallback(&ValidationWarningCallback);

		m_instance = kvfCreateInstance(extensions.data(), extensions.size());
		Message("Vulkan : instance created");

		VkSurfaceKHR surface = VK_NULL_HANDLE;
		SDL_Vulkan_CreateSurface(win, m_instance, &surface);

		m_physical_device = kvfPickGoodDefaultPhysicalDevice(m_instance, surface);

		// just for style
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(m_physical_device, &props);
		Message("Vulkan : physical device picked '%'", props.deviceName);

		m_device = kvfCreateDefaultDevice(m_physical_device);
		Message("Vulkan : logical device created");

		vkDestroySurfaceKHR(m_instance, surface, nullptr);
		SDL_DestroyWindow(win);

		ShaderLayout vertex_shader_layout(
			{
				{ 0,
					ShaderSetLayout({ 
						{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER }
					})
				}
			}, { ShaderPushConstantLayout({ 0, 16 }) }
		);
		m_internal_shaders[0] = LoadShaderFromFile(ScopEngine::Get().GetAssetsPath() / "Shaders/Build/Vertex.spv", ShaderType::Vertex, std::move(vertex_shader_layout));

		ShaderLayout default_fragment_shader_layout(
			{
				{ 1,
					ShaderSetLayout({ 
						{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER }
					})
				}
			}, {}
		);
		m_internal_shaders[DEFAULT_FRAGMENT_SHADER_ID] = LoadShaderFromFile(ScopEngine::Get().GetAssetsPath() / "Shaders/Build/DefaultFragment.spv", ShaderType::Fragment, std::move(default_fragment_shader_layout));

		ShaderLayout basic_fragment_shader_layout(
			{
				{ 1,
					ShaderSetLayout({ 
						{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER }
					})
				}
			}, {}
		);
		m_internal_shaders[BASIC_FRAGMENT_SHADER_ID] = LoadShaderFromFile(ScopEngine::Get().GetAssetsPath() / "Shaders/Build/BasicFragment.spv", ShaderType::Fragment, std::move(basic_fragment_shader_layout));
	}

	void RenderCore::Destroy() noexcept
	{
		vkDeviceWaitIdle(m_device);
		for(auto& shader : m_internal_shaders)
			shader.reset();
		kvfDestroyDevice(m_device);
		Message("Vulkan : logical device destroyed");
		kvfDestroyInstance(m_instance);
		Message("Vulkan : instance destroyed");
	}
}
