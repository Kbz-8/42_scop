#ifndef __SCOPE_RENDERER_MODEL__
#define __SCOPE_RENDERER_MODEL__

#include <memory>
#include <filesystem>

#include <kvf.h>

#include <Graphics/Mesh.h>
#include <Graphics/Material.h>

namespace Scop
{
	// Constant meshes for now
	class Model
	{
		public:
			Model() = default;
			Model(std::shared_ptr<Mesh> mesh);

			inline void SetMaterial(std::shared_ptr<Material> material, std::size_t mesh_index) { m_materials[mesh_index] = material; }
			inline std::size_t GetSubMeshCount() const { return p_mesh->GetSubMeshCount(); }

			void Draw(VkCommandBuffer cmd, std::size_t& drawcalls) const;

			~Model() = default;

		private:
			std::vector<std::shared_ptr<Material>> m_materials;
			std::shared_ptr<Mesh> p_mesh;
	};

	Model LoadModelFromObjFile(std::filesystem::path path) noexcept;
}

#endif