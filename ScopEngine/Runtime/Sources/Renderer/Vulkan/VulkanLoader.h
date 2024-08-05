#ifndef __SCOP_VULKAN_LOADER__
#define __SCOP_VULKAN_LOADER__

#include <Renderer/Vulkan/VulkanPrototypes.h>

namespace Scop
{
	class VulkanLoader
	{
		public:
			VulkanLoader();
			void LoadInstance(VkInstance instance);
			~VulkanLoader();

		private:
			void LoadGlobalFunctions(void* context, PFN_vkVoidFunction (*load)(void*, const char*)) noexcept;
			void LoadInstanceFunctions(void* context, PFN_vkVoidFunction (*load)(void*, const char*)) noexcept;
			void LoadDeviceFunctions(void* context, PFN_vkVoidFunction (*load)(void*, const char*)) noexcept;

		private:
			void* p_module = nullptr;
	};
}

#endif
