#ifndef __SCOPE_RENDERER_MODEL__
#define __SCOPE_RENDERER_MODEL__

#include <memory>
#include <vector>
#include <filesystem>

#include <Graphics/Mesh.h>
#include <Graphics/Material.h>

namespace Scop
{
	class Model
	{
		public:
			Model(const Mesh& mesh);
			Model(const std::vector<Mesh>& meshes);

			~Model() = default;

	};

	Model LoadModelFromObjFile(std::filesystem::path path) noexcept;
}

#endif
