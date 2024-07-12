#include <Platform/Window.h>
#include <Core/Logs.h>

namespace Scop
{
	Window::Window(const std::string& title, std::uint32_t width, std::uint32_t height) : m_title(title), m_height(height), m_width(width)
	{
		m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
		if(!m_window)
			FatalError("Unable to open a new window, %", SDL_GetError());
	}

	void Window::Destroy() noexcept
	{
		if(m_window != nullptr)
		{
			SDL_DestroyWindow(m_window);
			m_window = nullptr;
		}
	}

	Window::~Window()
	{
		Destroy();
	}
}
