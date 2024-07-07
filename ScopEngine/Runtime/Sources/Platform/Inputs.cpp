#include <Platform/Inputs.h>
#include <cstring>

namespace Scop
{
	bool Inputs::IsKeyPressed(const std::uint32_t button) const noexcept
	{
		return m_keys[button] == 2;
	}

	bool Inputs::IsKeyReleased(const std::uint32_t button) const noexcept
	{
		return m_keys[button] == 1;
	}

	bool Inputs::IsMouseButtonPressed(const std::uint8_t button) const noexcept
	{
		return m_mouse[button] == 2;
	}

	bool Inputs::IsMouseButtonReleased(const std::uint8_t button) const noexcept
	{
		return m_mouse[button] == 1;
	}

	void Inputs::Update()
	{
		m_x_rel = 0;
		m_y_rel = 0;

		std::memset(m_keys.data(), 0, m_keys.size());
		std::memset(m_mouse.data(), 0, m_mouse.size());

		while(SDL_PollEvent(&m_event))
		{
			if(m_event.type == SDL_MOUSEMOTION) 
			{
				m_x = m_event.motion.x;
				m_y = m_event.motion.y;

				m_x_rel = m_event.motion.xrel;
				m_y_rel = m_event.motion.yrel;
			}

			switch(m_event.type) 
			{
				case SDL_KEYDOWN: m_keys[m_event.key.keysym.scancode] = 2; break;
				case SDL_KEYUP: m_keys[m_event.key.keysym.scancode] = 1; break;
				case SDL_MOUSEBUTTONDOWN: m_mouse[m_event.button.button] = 2; break;
				case SDL_MOUSEBUTTONUP: m_mouse[m_event.button.button] = 1; break;

				case SDL_WINDOWEVENT:
				{
					switch(m_event.window.event)
					{
						case SDL_WINDOWEVENT_CLOSE: m_has_recieved_close_event = true; break;

						default : break;
					}
					break;
				}

				default: break;
			}
		}
	}
}
