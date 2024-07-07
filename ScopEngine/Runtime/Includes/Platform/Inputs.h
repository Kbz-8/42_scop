#ifndef __SCOP_PLATFORM_INPUTS__
#define __SCOP_PLATFORM_INPUTS__

#include <array>
#include <cstdint>

#include <SDL2/SDL.h>

namespace Scop
{
	class Inputs
	{
		friend class ScopEngine;

		public:
			Inputs() = default;

			bool IsKeyPressed(const std::uint32_t button) const noexcept;
			bool IsKeyReleased(const std::uint32_t button) const noexcept;
			bool IsMouseButtonPressed(const std::uint8_t button) const noexcept;
			bool IsMouseButtonReleased(const std::uint8_t button) const noexcept;
			inline std::int32_t GetX() const noexcept { return m_x; }
			inline std::int32_t GetY() const noexcept { return m_y; }
			inline std::int32_t GetXRel() const noexcept { return m_x_rel; }
			inline std::int32_t GetYRel() const noexcept { return m_y_rel; }

			inline bool HasRecievedCloseEvent() const noexcept { return m_has_recieved_close_event; }

			~Inputs() = default;

		private:
			void Update();

		private:
			SDL_Event m_event;
			std::array<std::uint8_t, SDL_NUM_SCANCODES> m_keys;
			std::array<std::uint8_t, 5> m_mouse;
			std::int32_t m_x = 0;
			std::int32_t m_y = 0;
			std::int32_t m_x_rel = 0;
			std::int32_t m_y_rel = 0;
			bool m_has_recieved_close_event = false;
	};
}

#endif
