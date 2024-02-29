#include <Core/Engine.h>
#include <Renderer/RenderCore.h>
#include <SDL2/SDL.h>
#include <Core/Logs.h>
#include <Core/EventBus.h>
#include <csignal>
#include <iostream>

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

	ScopEngine::ScopEngine(int ac, char** av, const std::string& title, std::uint32_t width, std::uint32_t height) : m_renderer(), m_window(title, width, height)
	{
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

	void ScopEngine::Run()
	{
		while(m_running)
		{
			m_inputs.Update();

			if(m_running)
				m_running = !m_inputs.HasRecievedCloseEvent();
		}
	}

	ScopEngine::~ScopEngine()
	{
		m_window.Destroy();
		m_renderer.Destroy();
		RenderCore::Get().Destroy();
		SDL_Quit();
		Message("Successfully executed !");
	}
}
