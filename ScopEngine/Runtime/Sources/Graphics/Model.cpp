#include <Graphics/Model.h>
#include <Graphics/Loaders/OBJ.h>

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
		auto obj_data = LoadOBJFromFile(path);
		if(!obj_data)
			return { nullptr };
		TesselateOBJData(*obj_data);
		ObjModel obj_model = ConvertObjDataToObjModel(*obj_data);
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		for(auto& [group, faces] : obj_model.faces)
		{
			std::vector<Vertex> vertices;
			std::vector<std::uint32_t> indices;
			for(std::size_t i = 0; i < faces.size(); i++)
			{
				Vertex v(
					Vec4f{
						obj_model.vertex[faces[i]],
						1.0f
					},
					Vec4f{ 1.0f, 1.0f, 1.0f, 1.0f },
					Vec4f{
						(obj_model.normal.empty() ? Vec3f{ 1.0f } : obj_model.normal[faces[i]]),
						1.0f
					},
					(obj_model.tex_coord.empty() ? Vec2f{ 1.0f } : obj_model.tex_coord[faces[i]])
				);
				indices.push_back(vertices.size());
				vertices.push_back(std::move(v));
			}
			mesh->AddSubMesh({ vertices, indices });
		}
		Model model(mesh);
		return model;
	}
}
