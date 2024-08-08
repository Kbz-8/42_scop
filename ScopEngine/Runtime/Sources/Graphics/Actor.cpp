#include <Graphics/Actor.h>
#include <Platform/Inputs.h>

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

	void Actor::Update(Inputs& input, float delta)
	{
		if(p_script)
			p_script->OnUpdate(this, input, delta);
	}

	Actor::~Actor()
	{
		if(p_script)
			p_script->OnQuit(this);
	}
}
