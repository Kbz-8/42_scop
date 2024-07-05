#ifndef __SCOPE_RENDERER_MODEL__
#define __SCOPE_RENDERER_MODEL__

#include <memory>
#include <vector>
#include <filesystem>

#include <Renderer/Material.h>
#include <Renderer/Mesh.h>

namespace Scop
{
	class Model
	{
		public:
			Model() = default;

			~Model() = default;

		private:
			struct SubMeshData
			{
				std::shared_ptr<Material> p_material;
				Mesh m_mesh;
			};

		private:
			std::vector<SubMeshData> m_sub_meshes;
	};

	Model LoadModelFromObjFile(std::filesystem::path path) noexcept;
}

#endif
