#ifndef __SCOP_RENDER_CORE__
#define __SCOP_RENDER_CORE__

#include <kvf.h>
#include <Utils/Singleton.h>

namespace Scop
{
	class RenderCore : public Singleton<RenderCore>
	{
		friend class Singleton<RenderCore>;

		public:
			void Init() noexcept;
			void Destroy() noexcept;

			inline VkInstance GetInstance() const noexcept { return m_instance; }
			inline VkDevice GetDevice() const noexcept { return m_device; }

		private:
			RenderCore() = default;
			~RenderCore() = default;

		private:
			VkInstance m_instance = VK_NULL_HANDLE;
			VkDevice m_device = VK_NULL_HANDLE;
	};
}

#endif