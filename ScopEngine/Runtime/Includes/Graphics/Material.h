#ifndef __SCOPE_RENDERER_MATERIAL__
#define __SCOPE_RENDERER_MATERIAL__

#include <memory>

#include <Renderer/Image.h>
#include <Renderer/Buffer.h>
#include <Renderer/Descriptor.h>

namespace Scop
{
	struct MaterialTextures
	{
		std::shared_ptr<Texture> albedo;
		// TODO : add other PBR textures
	};

	struct MaterialData
	{
		float dissolve_texture_factor = 1.0f;
		float dissolve_black_white_colors_factor = 1.0f;
		float dissolve_normals_colors_factor = 0.0f;
	};

	class Material
	{
		friend class Model;

		public:
			Material() { m_data_buffer.Init(sizeof(m_data)); }
			Material(const MaterialTextures& textures) : m_textures(textures) { m_data_buffer.Init(sizeof(m_data)); }

			inline void SetMaterialData(const MaterialData& data) noexcept { m_data = data; }

			~Material() { m_data_buffer.Destroy(); }

		private:
			[[nodiscard]] inline bool IsSetInit() const noexcept { return m_set.IsInit(); }
			[[nodiscard]] inline VkDescriptorSet GetSet(std::size_t frame_index) const noexcept { return m_set.GetSet(frame_index); }

			inline void UpdateDescriptorSet(const DescriptorSet& set)
			{
				m_set = set.Duplicate();
			}

			inline void Bind(std::size_t frame_index)
			{
				m_set.SetImage(frame_index, 0, *m_textures.albedo);
				m_set.SetUniformBuffer(frame_index, 1, m_data_buffer.Get(frame_index));
				m_set.Update(frame_index);

				static CPUBuffer buffer(sizeof(MaterialData));
				std::memcpy(buffer.GetData(), &m_data, buffer.GetSize());
				m_data_buffer.SetData(buffer, frame_index);
			}

		private:
			UniformBuffer m_data_buffer;
			MaterialTextures m_textures;
			MaterialData m_data;
			DescriptorSet m_set;
	};
}

#endif

