#ifndef __SCOPE_RENDERER_MATERIAL__
#define __SCOPE_RENDERER_MATERIAL__

#include <memory>

#include <Renderer/Image.h>
#include <Renderer/Descriptor.h>

namespace Scop
{
	struct MaterialTextures
	{
		std::shared_ptr<Texture> albedo;
		// TODO : add other PBR textures
	};

	class Material
	{
		friend class Model;

		public:
			Material() = default;
			Material(const MaterialTextures& textures) : m_textures(textures) {}
			~Material() = default;

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
				m_set.Update(frame_index);
			}

		private:
			MaterialTextures m_textures;
			DescriptorSet m_set;
	};
}

#endif

