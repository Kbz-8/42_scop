#ifndef __SCOP_SCENE__
#define __SCOP_SCENE__

#include <memory>
#include <string>
#include <string_view>

#include <Utils/NonOwningPtr.h>

#include <Graphics/Actor.h>
#include <Renderer/Buffer.h>
#include <Renderer/Descriptor.h>
#include <Renderer/RenderCore.h>
#include <Graphics/Cameras/Base.h>
#include <Renderer/Pipelines/Shader.h>
#include <Renderer/Pipelines/Graphics.h>

namespace Scop
{
	struct SceneDescriptor
	{
		std::shared_ptr<Shader> fragment_shader;
		std::shared_ptr<BaseCamera> camera;
	};

	class Scene
	{
		friend class ScopEngine;

		public:
			struct ForwardData
			{
				std::shared_ptr<DescriptorSet> matrices_set;
				std::shared_ptr<DescriptorSet> albedo_set;
				std::shared_ptr<UniformBuffer> matrices_buffer;
				bool wireframe = false;
			};

		public:
			Scene(std::string_view name, SceneDescriptor desc);

			[[nodiscard]] Actor& CreateActor(Model model) noexcept;
			[[nodiscard]] Actor& CreateActor(std::string_view name, Model model);

			[[nodiscard]] inline Scene& AddChildScene(std::string_view name, SceneDescriptor desc) { return m_scene_children.emplace_back(name, std::move(desc)); }
			inline void AddSkybox(std::shared_ptr<CubeTexture> cubemap) { p_skybox = cubemap; }
			void SwitchToChild(std::string_view name);
			void SwitchToParent();

			[[nodiscard]] inline ForwardData& GetForwardData() noexcept { return m_forward; }
			[[nodiscard]] inline const std::vector<std::shared_ptr<Actor>>& GetActors() const noexcept { return m_actors; }
			[[nodiscard]] inline const std::string& GetName() const noexcept { return m_name; }
			[[nodiscard]] inline GraphicPipeline& GetPipeline() noexcept { return m_pipeline; }
			[[nodiscard]] inline std::shared_ptr<BaseCamera> GetCamera() const { return p_camera; }
			[[nodiscard]] inline DepthImage& GetDepth() noexcept { return m_depth; }
			[[nodiscard]] inline std::shared_ptr<Shader> GetFragmentShader() const { return m_fragment_shader; }
			[[nodiscard]] inline std::shared_ptr<CubeTexture> GetSkybox() const { return p_skybox; }

			~Scene() = default;

		private:
			Scene() = default;
			Scene(std::string_view name, SceneDescriptor desc, NonOwningPtr<Scene> parent);
			void Init(NonOwningPtr<class Renderer> renderer);
			void Update(class Inputs& input, float delta, float aspect);
			void Destroy();

		private:
			GraphicPipeline m_pipeline;
			ForwardData m_forward;
			DepthImage m_depth;
			std::shared_ptr<CubeTexture> p_skybox;
			std::vector<std::shared_ptr<Actor>> m_actors;
			std::vector<Scene> m_scene_children;
			std::string m_name;
			std::shared_ptr<Shader> m_fragment_shader;
			std::shared_ptr<BaseCamera> p_camera;
			NonOwningPtr<Scene> p_parent;
	};
}

#endif
