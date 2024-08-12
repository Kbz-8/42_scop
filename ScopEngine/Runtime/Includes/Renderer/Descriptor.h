#ifndef __SCOP_DESCRIPTOR_SET__
#define __SCOP_DESCRIPTOR_SET__

#include <vector>
#include <cstdint>

#include <kvf.h>
#include <Utils/NonOwningPtr.h>
#include <Renderer/Pipelines/Shader.h>
#include <Renderer/Vulkan/VulkanPrototypes.h>

namespace Scop
{
	struct Descriptor
	{
		NonOwningPtr<class GPUBuffer> storage_buffer_ptr;
		NonOwningPtr<class GPUBuffer> uniform_buffer_ptr;
		NonOwningPtr<class Image> image_ptr;
		VkDescriptorType type;
		ShaderType shader_type;
		std::uint32_t binding;
	};

	class DescriptorSet
	{
		public:
			DescriptorSet(const ShaderSetLayout& layout, VkDescriptorSetLayout vklayout, ShaderType shader_type);

			void SetImage(std::uint32_t binding, class Image& image);
			void SetStorageBuffer(std::uint32_t binding, class GPUBuffer& buffer);
			void SetUniformBuffer(std::uint32_t binding, class GPUBuffer& buffer);
			void Update() noexcept;

			inline void Bind(VkCommandBuffer cmd, VkPipelineBindPoint bindpoint, VkPipelineLayout layout) const noexcept
			{
				vkCmdBindDescriptorSets(cmd, bindpoint, layout, 0, 1, &m_set, 0, nullptr);
			}

			[[nodiscard]] inline DescriptorSet Duplicate() const { return DescriptorSet{ m_set_layout, m_descriptors }; }

			~DescriptorSet() = default;

		private:
			DescriptorSet(VkDescriptorSetLayout layout, const std::vector<Descriptor>& descriptors);

		private:
			std::vector<Descriptor> m_descriptors;
			VkDescriptorSet m_set;
			VkDescriptorSetLayout m_set_layout;
	};
}

#endif
