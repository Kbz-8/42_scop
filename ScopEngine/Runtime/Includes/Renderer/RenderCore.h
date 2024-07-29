#ifndef __SCOP_RENDER_CORE__
#define __SCOP_RENDER_CORE__

#include <kvf.h>
#include <memory>
#include <array>
#include <Utils/Singleton.h>

namespace Scop
{
	constexpr const int MAX_FRAMES_IN_FLIGHT = 3;

	constexpr const int DEFAULT_VERTEX_SHADER_ID = 0;
	constexpr const int DEFAULT_FRAGMENT_SHADER_ID = 1;
	constexpr const int BASIC_FRAGMENT_SHADER_ID = 2;

	class RenderCore : public Singleton<RenderCore>
	{
		friend class Singleton<RenderCore>;

		public:
			void Init() noexcept;
			void Destroy() noexcept;

			inline VkInstance GetInstance() const noexcept { return m_instance; }
			inline VkDevice GetDevice() const noexcept { return m_device; }
			inline VkPhysicalDevice GetPhysicalDevice() const noexcept { return m_physical_device; }

			inline std::shared_ptr<class Shader> GetDefaultVertexShader() const { return m_internal_shaders[DEFAULT_VERTEX_SHADER_ID]; }
			inline std::shared_ptr<class Shader> GetBasicFragmentShader() const { return m_internal_shaders[BASIC_FRAGMENT_SHADER_ID]; }
			inline std::shared_ptr<class Shader> GetDefaultFragmentShader() const { return m_internal_shaders[DEFAULT_FRAGMENT_SHADER_ID]; }

		private:
			RenderCore() = default;
			~RenderCore() = default;

		private:
			std::array<std::shared_ptr<class Shader>, 3> m_internal_shaders;
			VkInstance m_instance = VK_NULL_HANDLE;
			VkDevice m_device = VK_NULL_HANDLE;
			VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
	};
}

#endif
