#include <Graphics/Model.h>
#include <Graphics/Loaders/OBJ.h>
#include <Renderer/Pipelines/Graphics.h>

namespace Scop
{
	Model::Model(std::shared_ptr<Mesh> mesh) : p_mesh(mesh)
	{
		if(p_mesh)
			m_materials.resize(p_mesh->GetSubMeshCount() + 1);

		CPUBuffer default_albedo_pixels{ kvfFormatSize(VK_FORMAT_R8G8B8A8_UNORM) };
		default_albedo_pixels.GetDataAs<std::uint32_t>()[0] = 0xFFFFFFFF;
		MaterialTextures textures;
		textures.albedo = std::make_shared<Texture>(std::move(default_albedo_pixels), 1, 1, VK_FORMAT_R8G8B8A8_UNORM);
		m_materials.back() = std::make_shared<Material>(textures);
	}

	void Model::Draw(VkCommandBuffer cmd, const DescriptorSet& matrices_set, const GraphicPipeline& pipeline, DescriptorSet& set, std::size_t& drawcalls, std::size_t& polygondrawn, std::size_t frame_index) const
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
			material->Bind(frame_index);
			std::array<VkDescriptorSet, 2> sets = { matrices_set.GetSet(frame_index), material->GetSet(frame_index) };
			vkCmdBindDescriptorSets(cmd, pipeline.GetPipelineBindPoint(), pipeline.GetPipelineLayout(), 0, sets.size(), sets.data(), 0, nullptr);
			p_mesh->Draw(cmd, drawcalls, polygondrawn, i);
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
				min_x = std::min(obj_model.vertex[faces[i]].x, min_x);
				min_y = std::min(obj_model.vertex[faces[i]].y, min_y);
				min_z = std::min(obj_model.vertex[faces[i]].z, min_z);
				max_x = std::max(obj_model.vertex[faces[i]].x, max_x);
				max_y = std::max(obj_model.vertex[faces[i]].y, max_y);
				max_z = std::max(obj_model.vertex[faces[i]].z, max_z);

				float color_base;
				std::size_t triangle_index = i / 3;

				if(triangle_index % 6 < 2)
					color_base = 0.2f;
				else if(triangle_index % 6 == 2)
					color_base = 0.4f;
				else if(triangle_index % 6 == 3)
					color_base = 0.6f;
				else if(triangle_index % 6 == 4)
					color_base = 0.8f;
				else
					color_base = 1.0f;

				Vertex v(
					Vec4f{
						obj_model.vertex[faces[i]],
						1.0f
					},
					(obj_model.color.empty() ? Vec4f{ color_base, color_base, color_base, 1.0f } : obj_model.color[faces[i]]),
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
		model.m_center = Vec3f{ (min_x + max_x) / 2.0f, (min_y + max_y) / 2.0f, (min_z + max_z) / 2.0f };
		return model;
	}
}
