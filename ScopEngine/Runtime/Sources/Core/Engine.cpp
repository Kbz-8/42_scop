#include <Core/Engine.h>
#include <Renderer/RenderCore.h>
#include <SDL2/SDL.h>
#include <Core/Logs.h>
#include <Core/EventBus.h>
#include <csignal>

namespace Scop
{
	namespace Internal
	{
		struct InterruptEvent : public EventBase
		{
			std::uint32_t What() const override { return 168; }
		};
	}

	void FatalErrorEventHandle()
	{
		std::abort();
	}

	void SignalHandler([[maybe_unused]] int sig)
	{
		EventBus::Send("__ScopEngine", Internal::InterruptEvent{});
	}

	ScopEngine* ScopEngine::s_instance = nullptr;

	ScopEngine::ScopEngine(int ac, char** av, const std::string& title, std::uint32_t width, std::uint32_t height, std::filesystem::path assets_path)
		: m_renderer(), m_window(title, width, height), m_assets_path(std::move(assets_path))
	{
		s_instance = this;
		std::function<void(const EventBase&)> functor = [this](const EventBase& event)
		{
			if(event.What() == 167)
				FatalErrorEventHandle();
			if(event.What() == 168)
				this->Quit();
		};
		EventBus::RegisterListener({ functor, "__ScopEngine" });

		signal(SIGINT, SignalHandler);

		if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0)
			FatalError("SDL error : unable to init all subsystems : %s", SDL_GetError());
		RenderCore::Get().Init();
		m_renderer.Init(&m_window);
	}

	ScopEngine& ScopEngine::Get() noexcept
	{
		Verify(s_instance != nullptr, "ScopEngine has not been instanciated");
		return *s_instance;
	}

	void ScopEngine::Run()
	{
		m_main_scene.Init(&m_renderer);
		while(m_running)
		{
			m_inputs.Update();

			if(m_renderer.BeginFrame())
			{
				/* Do scene rendering */

				m_renderer.EndFrame();
			}

			if(m_running)
				m_running = !m_inputs.HasRecievedCloseEvent();
		}
	}

	ScopEngine::~ScopEngine()
	{
		m_main_scene.Destroy();
		m_window.Destroy();
		m_renderer.Destroy();
		RenderCore::Get().Destroy();
		SDL_Quit();
		Message("Successfully executed !");
		s_instance = nullptr;
	}
}
