#ifndef __SCOP_SCENE__
#define __SCOP_SCENE__

#include <memory>
#include <string>
#include <string_view>

#include <Renderer/Actor.h>

namespace Scop
{
	struct SceneDescriptor
	{
		std::shared_ptr<Shader> fragment_shader;
	};

	class Scene
	{
		public:
			Scene(std::string_view name, SceneDescriptor desc);

			Actor CreateActor(Model model) noexcept;
			Actor CreateActor(std::string_view name, Model model);

			inline const std::string& GetName() const noexcept { return m_name; }

			~Scene() = default;

		private:
			std::string m_name;
			std::shared_ptr<Shader> m_fragment_shader;
	};
}

#endif
