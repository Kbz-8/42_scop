#include <Renderer/Actor.h>

namespace Scop
{
	Actor::Actor()
	{
		if(p_script)
			p_script->OnInit(this);
	}

	Actor::Actor(Model model)
	{
		if(p_script)
			p_script->OnInit(this);
	}

	void Actor::Update(float delta)
	{
		if(p_script)
			p_script->OnUpdate(this, delta);
	}

	Actor::~Actor()
	{
		if(p_script)
			p_script->OnQuit(this);
	}
}
