#include <Renderer/Scene.h>
#include <Renderer/Renderer.h>
#include <Renderer/RenderCore.h>

namespace Scop
{
	Scene::Scene(std::string_view name, SceneDescriptor desc)
	: m_name(name), m_fragment_shader(desc.fragment_shader), p_parent(nullptr)
	{
	}

	Scene::Scene(std::string_view name, SceneDescriptor desc, NonOwningPtr<Scene> parent)
	: m_name(name), m_fragment_shader(desc.fragment_shader), p_parent(parent)
	{
	}

	Actor& Scene::CreateActor(Model model) noexcept
	{
		return m_actors.emplace_back(std::move(model));
	}

	Actor& Scene::CreateActor(std::string_view name, Model model)
	{
		return m_actors.emplace_back(std::move(model));
	}

	void Scene::Init(NonOwningPtr<Renderer> renderer)
	{
		m_pipeline.Init(RenderCore::Get().GetDefaultVertexShader(), m_fragment_shader, renderer);
	}

	void Scene::Update(float timestep)
	{
		for(auto& actor : m_actors)
			actor.Update(timestep);
	}

	void Scene::Destroy()
	{
		m_fragment_shader.reset();
		m_pipeline.Destroy();
	}
}
