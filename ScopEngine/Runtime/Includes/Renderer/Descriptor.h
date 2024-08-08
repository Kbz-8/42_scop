#ifndef __SCOP_DESCRIPTOR_SET__
#define __SCOP_DESCRIPTOR_SET__

#include <vector>
#include <cstdint>

#include <kvf.h>
#include <Renderer/Pipelines/Shader.h>
#include <Renderer/Vulkan/VulkanPrototypes.h>

namespace Scop
{
	struct Descriptor
	{
		VkDescriptorType type;
		ShaderType shader_type;
		std::uint32_t binding;
	};

	class DescriptorSet
	{
		public:
			DescriptorSet(ShaderSetLayout layout, VkDescriptorSetLayout vklayout, ShaderType shader_type);

			void SetImage(std::uint32_t binding, class Image& image);
			void SetBuffer(std::uint32_t binding, class GPUBuffer& buffer);
			void Update(VkCommandBuffer cmd) noexcept;

			~DescriptorSet() = default;

		private:
			std::vector<Descriptor> m_descriptors;
			VkDescriptorSet m_set;
	};
}

#endif
