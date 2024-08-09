#include <Maths/Mat4.h>
#include <Graphics/Scene.h>
#include <Renderer/Renderer.h>
#include <Renderer/RenderCore.h>
#include <Platform/Inputs.h>
#include <Core/Logs.h>

namespace Scop
{
	struct VertexMatricesData
	{
		Mat4f view;
		Mat4f projection;
	};

	Scene::Scene(std::string_view name, SceneDescriptor desc)
	: m_name(name), m_fragment_shader(desc.fragment_shader), p_parent(nullptr), p_camera(desc.camera)
	{
		Verify((bool)p_camera, "No camera given");
	}

	Scene::Scene(std::string_view name, SceneDescriptor desc, NonOwningPtr<Scene> parent)
	: m_name(name), m_fragment_shader(desc.fragment_shader), p_parent(parent), p_camera(desc.camera)
	{
		Verify((bool)p_camera, "No camera given");
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
		auto vertex_shader = RenderCore::Get().GetDefaultVertexShader();
		m_pipeline.Init(vertex_shader, m_fragment_shader, renderer);
		for(std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
			m_forward.matrices_set[i] = std::make_shared<DescriptorSet>(vertex_shader->GetPipelineLayout().set_layouts[0], m_pipeline.GetPipelineLayout(), ShaderType::Vertex);
		m_forward.matrices_buffer = std::make_shared<UniformBuffer>();
		m_forward.matrices_buffer->Init(sizeof(VertexMatricesData));
	}

	void Scene::Update(Inputs& input, float timestep, float aspect)
	{
		for(auto& actor : m_actors)
			actor.Update(input, timestep);
		p_camera->Update(input, aspect);
	}

	void Scene::Destroy()
	{
		m_forward.matrices_buffer->Destroy();
		m_fragment_shader.reset();
		m_pipeline.Destroy();
	}
}
