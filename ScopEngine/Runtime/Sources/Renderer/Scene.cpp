#include <Renderer/Scene.h>

namespace Scop
{
	Scene::Scene(std::string_view name, SceneDescriptor desc)
	: m_name(name), m_fragment_shader(desc.fragment_shader)
	{
	}

	Actor Scene::CreateActor(Model model) noexcept
	{
		Actor actor(model);
		return actor;
	}

	Actor Scene::CreateActor(std::string_view name, Model model)
	{
		Actor actor(model);
		return actor;
	}
}
