#include <Core/Engine.h>
#include <Renderer/RenderCore.h>
#include <SDL2/SDL.h>
#include <Core/Logs.h>

namespace Scop
{
	ScopEngine::ScopEngine(int ac, char** av)
	{
		if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0)
			FatalError("SDL error : unable to init all subsystems : %s", SDL_GetError());
		RenderCore::Get().Init();
	}

	void ScopEngine::Run()
	{

	}

	ScopEngine::~ScopEngine()
	{
		RenderCore::Get().Destroy();
		SDL_Quit();
	}
}
