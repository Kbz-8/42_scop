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
		auto data = LoadOBJFromFile(path);
		if(!data)
			return { nullptr };
		TesselateOBJData(*data);
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		std::vector<Vertex> vertices;
		std::vector<std::uint32_t> indices;
		for(std::size_t i = 0; i < data->vertex.size() / 3; i++)
		{
			indices.push_back(vertices.size());
			Vertex v(
				Vec4f{
					data->vertex[3 * i + 0],
					data->vertex[3 * i + 1],
					data->vertex[3 * i + 2],
					1.0f
				},
				Vec4f{ 1.0f, 1.0f, 1.0f, 1.0f },
				Vec4f{
					data->normal[3 * i + 0],
					data->normal[3 * i + 1],
					data->normal[3 * i + 2],
					1.0f
				},
				Vec2f{
					data->tex_coord[2 * i + 0],
					data->tex_coord[2 * i + 1],
				}
			);
			vertices.push_back(v);
		}
		mesh->AddSubMesh({ vertices, indices });
		Model model(mesh);
		return model;
	}
}
