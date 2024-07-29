#ifndef __SCOP_SCENE__
#define __SCOP_SCENE__

#include <memory>
#include <string>
#include <string_view>

#include <Utils/NonOwningPtr.h>

#include <Renderer/Actor.h>
#include <Renderer/Pipelines/Shader.h>
#include <Renderer/Pipelines/Graphics.h>

namespace Scop
{
	struct SceneDescriptor
	{
		std::shared_ptr<Shader> fragment_shader;
	};

	class Scene
	{
		friend class ScopEngine;

		public:
			Scene(std::string_view name, SceneDescriptor desc);

			Actor CreateActor(Model model) noexcept;
			Actor CreateActor(std::string_view name, Model model);

			inline const std::string& GetName() const noexcept { return m_name; }

			~Scene() = default;

		private:
			Scene() = default;
			void Init(NonOwningPtr<class Renderer> renderer);
			void Destroy();

		private:
			GraphicPipeline m_pipeline;
			std::vector<Scene> m_scene_children;
			std::string m_name;
			std::shared_ptr<Shader> m_fragment_shader;
	};
}

#endif
