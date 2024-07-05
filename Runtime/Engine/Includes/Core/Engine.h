#ifndef __SCOP_CORE_ENGINE__
#define __SCOP_CORE_ENGINE__

#include <cstdint>

#include <Platform/Window.h>
#include <Platform/Inputs.h>
#include <Renderer/Renderer.h>
#include <Renderer/Actor.h>

namespace Scop
{
	class ScopEngine
	{
		public:
			ScopEngine(int ac, char** av, const std::string& title, std::uint32_t width, std::uint32_t height);

			void Run();

			inline const Window& GetWindow() const noexcept { return m_window; }

			inline void RegisterActor(const Actor& actor) noexcept { (void)actor; }

			constexpr void Quit() noexcept { m_running = false; }

			~ScopEngine();

		private:
			Inputs m_inputs;
			Renderer m_renderer;
			Window m_window;
			bool m_running = true;
	};
}

#endif
