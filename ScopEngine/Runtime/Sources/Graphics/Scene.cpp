#include <Maths/Mat4.h>
#include <Graphics/Scene.h>
#include <Renderer/Renderer.h>
#include <Renderer/RenderCore.h>
#include <Platform/Inputs.h>
#include <Core/Logs.h>
#include <cstring>

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
		m_forward.matrices_buffer = std::make_shared<UniformBuffer>();
		m_forward.matrices_buffer->Init(sizeof(VertexMatricesData));

		m_forward.matrices_set = std::make_shared<DescriptorSet>(vertex_shader->GetShaderLayout().set_layouts[0].second, vertex_shader->GetPipelineLayout().set_layouts[0], ShaderType::Vertex);
		for(std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_forward.matrices_set->SetUniformBuffer(i, 0, m_forward.matrices_buffer->Get(i));
			m_forward.matrices_set->Update(i);
		}
		m_forward.albedo_set = std::make_shared<DescriptorSet>(m_fragment_shader->GetShaderLayout().set_layouts[0].second, m_fragment_shader->GetPipelineLayout().set_layouts[0], ShaderType::Fragment);
	}

	void Scene::Update(Inputs& input, float timestep, float aspect)
	{
		for(auto& actor : m_actors)
			actor.Update(input, timestep);
		p_camera->Update(input, aspect, timestep);
	}

	void Scene::Destroy()
	{
		m_actors.clear();
		m_pipeline.Destroy();
		m_fragment_shader.reset();
		m_forward.matrices_buffer->Destroy();
		for(auto& child : m_scene_children)
			child.Destroy();
	}
}
