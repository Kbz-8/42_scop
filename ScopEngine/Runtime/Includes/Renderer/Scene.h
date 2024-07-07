#ifndef __SCOP_SCENE__
#define __SCOP_SCENE__

#include <string>
#include <string_view>

#include <Renderer/Actor.h>
#include <Renderer/ShadersLibrary.h>

namespace Scop
{
	struct SceneDescriptor
	{
		ShaderID fragment_shader = nullshader;
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
			ShaderID m_fragment_shader = nullshader;
			std::string m_name;
	};
}

#endif
