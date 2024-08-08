#ifndef __SCOP_PLATFORM_WINDOW__
#define __SCOP_PLATFORM_WINDOW__

#include <SDL2/SDL.h>

#include <cstdint>
#include <string>

namespace Scop
{
	class Window
	{
		friend class ScopEngine;
		public:
			Window(const std::string& title, std::uint32_t width, std::uint32_t height);

			[[nodiscard]] inline const std::string& GetTitle() const noexcept { return m_title; }
			[[nodiscard]] inline std::uint32_t GetWidth() const noexcept { return m_width; } 
			[[nodiscard]] inline std::uint32_t GetHeight() const noexcept { return m_height; } 
			[[nodiscard]] inline SDL_Window* GetSDLWindow() const noexcept { return m_window; }

			~Window();

		private:
			// Can only be called by the engine
			void Destroy() noexcept;

		private:
			SDL_Window* m_window = nullptr;
			std::string m_title;
			std::uint32_t m_height = 0;
			std::uint32_t m_width = 0;
	};
}

#endif
