#include <kvf.h>
#include <algorithm>

#include <Core/Logs.h>
#include <Renderer/Image.h>
#include <Renderer/Buffer.h>
#include <Renderer/Descriptor.h>
#include <Renderer/RenderCore.h>

namespace Scop
{
	DescriptorSet::DescriptorSet(const ShaderSetLayout& layout, VkDescriptorSetLayout vklayout, ShaderType shader_type)
	: m_set_layout(vklayout)
	{
		for(auto& [binding, type] : layout.binds)
		{
			m_descriptors.emplace_back();
			m_descriptors.back().type = type;
			m_descriptors.back().shader_type = shader_type;
			m_descriptors.back().binding = binding;
		}
		m_set = kvfAllocateDescriptorSet(RenderCore::Get().GetDevice(), vklayout);
	}

	DescriptorSet::DescriptorSet(VkDescriptorSetLayout layout, const std::vector<Descriptor>& descriptors)
	: m_set_layout(layout), m_descriptors(descriptors)
	{
		m_set = kvfAllocateDescriptorSet(RenderCore::Get().GetDevice(), layout);
	}

	void DescriptorSet::SetImage(std::uint32_t binding, class Image& image)
	{
		auto it = std::find_if(m_descriptors.begin(), m_descriptors.end(), [=](Descriptor descriptor)
		{
			return binding == descriptor.binding;
		});
		if(it == m_descriptors.end())
		{
			Warning("Vulkan : cannot update descriptor set image; invalid binding");
			return;
		}
		if(it->type != VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
		{
			Error("Vulkan : trying to bind an image to the wrong descriptor");
			return;
		}
		it->image_ptr = &image;
	}

	void DescriptorSet::SetBuffer(std::uint32_t binding, class GPUBuffer& buffer)
	{
		auto it = std::find_if(m_descriptors.begin(), m_descriptors.end(), [=](Descriptor descriptor)
		{
			return binding == descriptor.binding;
		});
		if(it == m_descriptors.end())
		{
			Warning("Vulkan : cannot update descriptor set buffer; invalid binding");
			return;
		}
		if(it->type != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER || it->type != VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
		{
			Error("Vulkan : trying to bind a buffer to the wrong descriptor");
			return;
		}
		it->buffer_ptr = &buffer;
	}

	void DescriptorSet::Update(VkCommandBuffer cmd) noexcept
	{
		std::vector<VkWriteDescriptorSet> writes;
		for(auto& descriptor : m_descriptors)
		{
			if(descriptor.image_ptr)
				writes.push_back(kvfWriteImageToDescriptorSet(RenderCore::Get().GetDevice(), m_set, descriptor.image_ptr->GetImageView(), descriptor.image_ptr->GetSampler(), descriptor.binding));
			else if(descriptor.buffer_ptr)
				writes.push_back(kvfWriteBufferToDescriptorSet(RenderCore::Get().GetDevice(), m_set, descriptor.buffer_ptr->Get(), descriptor.binding));
		}
		vkUpdateDescriptorSets(RenderCore::Get().GetDevice(), writes.size(), writes.data(), 0, nullptr);
	}
}
