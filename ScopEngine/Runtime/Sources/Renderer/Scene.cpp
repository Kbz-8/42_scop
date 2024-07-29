#include <Renderer/Scene.h>
#include <Renderer/Renderer.h>
#include <Renderer/RenderCore.h>

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

	void Scene::Init(NonOwningPtr<Renderer> renderer)
	{
		m_pipeline.Init(RenderCore::Get().GetDefaultVertexShader(), m_fragment_shader, renderer);
	}

	void Scene::Destroy()
	{
		m_pipeline.Destroy();
	}
}
