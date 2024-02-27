#include <Core/Engine.h>
#include <Renderer/RenderCore.h>

ScopEngine::ScopEngine(int ac, char** av)
{
	RenderCore::Get().Init();
}

void ScopEngine::Run()
{

}

ScopEngine::~ScopEngine()
{
	RenderCore::Get().Destroy();
}
