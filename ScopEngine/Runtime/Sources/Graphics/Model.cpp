#include <Graphics/Model.h>

namespace Scop
{
	Model::Model(std::shared_ptr<Mesh> mesh) : p_mesh(mesh)
	{
		if(p_mesh)
			m_materials.resize(p_mesh->GetSubMeshCount());
	}

	void Model::Draw(VkCommandBuffer cmd, std::size_t& drawcalls) const
	{
		if(!p_mesh)
			return;
		for(std::size_t i = 0; i < GetSubMeshCount(); i++)
		{
			// TODO : bind material
			p_mesh->Draw(cmd, drawcalls, i);
		}
	}

	Model LoadModelFromObjFile(std::filesystem::path path) noexcept
	{
		return {nullptr};
	}
}
