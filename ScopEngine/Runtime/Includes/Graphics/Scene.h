#ifndef __SCOP_SCENE__
#define __SCOP_SCENE__

#include <memory>
#include <string>
#include <string_view>

#include <Utils/NonOwningPtr.h>

#include <Graphics/Actor.h>
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
			Scene(std::string_view name, SceneDescriptor desc);

			[[nodiscard]] Actor& CreateActor(Model model) noexcept;
			[[nodiscard]] Actor& CreateActor(std::string_view name, Model model);

			[[nodiscard]] inline Scene& AddChildScene(std::string_view name, SceneDescriptor desc) { return m_scene_children.emplace_back(name, std::move(desc)); }
			void SwitchToChild(std::string_view name);
			void SwitchToParent();

			[[nodiscard]] inline const std::string& GetName() const noexcept { return m_name; }

			~Scene() = default;

		private:
			Scene() = default;
			Scene(std::string_view name, SceneDescriptor desc, NonOwningPtr<Scene> parent);
			void Init(NonOwningPtr<class Renderer> renderer);
			void Update(class Inputs& input, float delta, float aspect);
			void Destroy();

		private:
			GraphicPipeline m_pipeline;
			std::vector<Actor> m_actors;
			std::vector<Scene> m_scene_children;
			NonOwningPtr<Scene> p_parent;
			std::string m_name;
			std::shared_ptr<Shader> m_fragment_shader;
			std::shared_ptr<BaseCamera> p_camera;
	};
}

#endif
