#ifndef __SCOP_CORE_ENGINE__
#define __SCOP_CORE_ENGINE__

#include <cstdint>
#include <filesystem>

#include <Platform/Window.h>
#include <Platform/Inputs.h>
#include <Renderer/Renderer.h>
#include <Renderer/RenderCore.h>
#include <Core/Logs.h>
#include <Graphics/Scene.h>

namespace Scop
{
	class ScopEngine
	{
		friend class Scene;

		public:
			ScopEngine(int ac, char** av, const std::string& title, std::uint32_t width, std::uint32_t height, std::filesystem::path assets_path);

			void Run();

			inline const Window& GetWindow() const noexcept { return m_window; }
			inline std::filesystem::path GetAssetsPath() const { return m_assets_path; }

			inline void RegisterMainScene(const Scene& scene) noexcept { m_main_scene = scene; p_current_scene = &m_main_scene; }

			constexpr void Quit() noexcept { m_running = false; }

			static inline bool IsInit() noexcept { return s_instance != nullptr; }
			static ScopEngine& Get() noexcept;

			~ScopEngine();

		private:
			inline void SwitchToScene(NonOwningPtr<Scene> current) noexcept { p_current_scene = current; }

		private:
			static ScopEngine* s_instance;
			Inputs m_inputs;
			Renderer m_renderer;
			Window m_window;
			Scene m_main_scene;
			std::filesystem::path m_assets_path;
			NonOwningPtr<Scene> p_current_scene;
			bool m_running = true;
	};
}

#endif
