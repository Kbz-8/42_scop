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
			[[nodiscard]] inline VkDescriptorSet GetSet() const noexcept { return m_set.GetSet(); }

			inline void UpdateDescriptorSet(const DescriptorSet& set)
			{
				m_set = set.Duplicate();
			}

			inline void Bind()
			{
				m_set.SetImage(0, *m_textures.albedo);
				m_set.Update();
			}

		private:
			MaterialTextures m_textures;
			DescriptorSet m_set;
	};
}

#endif

