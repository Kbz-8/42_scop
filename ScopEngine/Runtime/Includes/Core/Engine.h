#ifndef __SCOP_CORE_ENGINE__
#define __SCOP_CORE_ENGINE__

#include <cstdint>
#include <filesystem>

#include <Platform/Window.h>
#include <Platform/Inputs.h>
#include <Renderer/Renderer.h>
#include <Core/Logs.h>
#include <Renderer/Scene.h>

namespace Scop
{
	class ScopEngine
	{
		public:
			ScopEngine(int ac, char** av, const std::string& title, std::uint32_t width, std::uint32_t height, std::filesystem::path assets_path);

			void Run();

			inline const Window& GetWindow() const noexcept { return m_window; }
			inline std::filesystem::path GetAssetsPath() const { return m_assets_path; }

			inline void RegisterMainScene(const Scene& scene) noexcept { (void)scene; }

			constexpr void Quit() noexcept { m_running = false; }

			static inline bool IsInit() noexcept { return s_instance != nullptr; }
			static ScopEngine& Get() noexcept;

			~ScopEngine();

		private:
			static ScopEngine* s_instance;
			Inputs m_inputs;
			Renderer m_renderer;
			Window m_window;
			std::filesystem::path m_assets_path;
			bool m_running = true;
	};
}

#endif
