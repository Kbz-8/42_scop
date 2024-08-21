#include <Graphics/Model.h>
#include <Graphics/Loaders/OBJ.h>
#include <Renderer/Pipelines/Graphics.h>

namespace Scop
{
	Model::Model(std::shared_ptr<Mesh> mesh) : p_mesh(mesh)
	{
		if(p_mesh)
			m_materials.resize(p_mesh->GetSubMeshCount() + 1);

		CPUBuffer default_albedo_pixels{ kvfFormatSize(VK_FORMAT_R8G8B8A8_UNORM) * 4 };
		default_albedo_pixels.GetDataAs<std::uint32_t>()[0] = 0x000000FF;
		default_albedo_pixels.GetDataAs<std::uint32_t>()[1] = 0xFF00FFFF;
		default_albedo_pixels.GetDataAs<std::uint32_t>()[2] = 0x000000FF;
		default_albedo_pixels.GetDataAs<std::uint32_t>()[3] = 0xFF00FFFF;
		MaterialTextures textures;
		textures.albedo = std::make_shared<Texture>(std::move(default_albedo_pixels), 2, 2, VK_FORMAT_R8G8B8A8_UNORM);
		m_materials.back() = std::make_shared<Material>(textures);
	}

	void Model::Draw(VkCommandBuffer cmd, const DescriptorSet& matrices_set, const GraphicPipeline& pipeline, DescriptorSet& set, std::size_t& drawcalls) const
	{
		if(!p_mesh)
			return;
		for(std::size_t i = 0; i < GetSubMeshCount(); i++)
		{
			std::shared_ptr<Material> material;
			if(!m_materials[i])
				material = m_materials.back();
			else
				material = m_materials[i];
			if(!material->IsSetInit())
				material->UpdateDescriptorSet(set);
			material->Bind();
			std::array<VkDescriptorSet, 2> sets = { matrices_set.GetSet(), material->GetSet() };
			vkCmdBindDescriptorSets(cmd, pipeline.GetPipelineBindPoint(), pipeline.GetPipelineLayout(), 0, sets.size(), sets.data(), 0, nullptr);
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
		float min_x = std::numeric_limits<float>::max(), max_x = std::numeric_limits<float>::lowest();
		float min_y = std::numeric_limits<float>::max(), max_y = std::numeric_limits<float>::lowest();
		float min_z = std::numeric_limits<float>::max(), max_z = std::numeric_limits<float>::lowest();
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
					(obj_model.tex_coord.empty() ?
						Vec2f{ (obj_model.vertex[faces[i]].x - min_x) / (max_x - min_x), 1.0f - ((obj_model.vertex[faces[i]].y - min_y) / (max_y - min_y)) }
						:
						obj_model.tex_coord[faces[i]]
					)
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
