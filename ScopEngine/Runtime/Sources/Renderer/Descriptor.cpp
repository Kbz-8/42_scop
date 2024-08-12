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

	void DescriptorSet::SetStorageBuffer(std::uint32_t binding, class GPUBuffer& buffer)
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
		if(it->type != VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
		{
			Error("Vulkan : trying to bind a buffer to the wrong descriptor");
			return;
		}
		it->storage_buffer_ptr = &buffer;
	}

	void DescriptorSet::SetUniformBuffer(std::uint32_t binding, class GPUBuffer& buffer)
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
		if(it->type != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
		{
			Error("Vulkan : trying to bind a buffer to the wrong descriptor");
			return;
		}
		it->uniform_buffer_ptr = &buffer;
	}

	void DescriptorSet::Update() noexcept
	{
		std::vector<VkWriteDescriptorSet> writes;
		std::vector<VkDescriptorBufferInfo> buffer_infos;
		std::vector<VkDescriptorImageInfo> image_infos;
		for(auto& descriptor : m_descriptors)
		{
			if(descriptor.image_ptr)
			{
				VkDescriptorImageInfo info{};
				info.sampler = descriptor.image_ptr->GetSampler();
				info.imageLayout = descriptor.image_ptr->GetLayout();
				info.imageView = descriptor.image_ptr->GetImageView();
				image_infos.push_back(info);
				writes.push_back(kvfWriteImageToDescriptorSet(RenderCore::Get().GetDevice(), m_set, &image_infos.back(), descriptor.binding));
			}
			else if(descriptor.uniform_buffer_ptr)
			{
				VkDescriptorBufferInfo info{};
				info.buffer = descriptor.uniform_buffer_ptr->Get();
				info.offset = descriptor.uniform_buffer_ptr->GetOffset();
				info.range = VK_WHOLE_SIZE;
				buffer_infos.push_back(info);
				writes.push_back(kvfWriteUniformBufferToDescriptorSet(RenderCore::Get().GetDevice(), m_set, &buffer_infos.back(), descriptor.binding));
			}
			else if(descriptor.storage_buffer_ptr)
			{
				VkDescriptorBufferInfo info{};
				info.buffer = descriptor.storage_buffer_ptr->Get();
				info.offset = descriptor.storage_buffer_ptr->GetOffset();
				info.range = VK_WHOLE_SIZE;
				buffer_infos.push_back(info);
				writes.push_back(kvfWriteStorageBufferToDescriptorSet(RenderCore::Get().GetDevice(), m_set, &buffer_infos.back(), descriptor.binding));
			}
		}
		vkUpdateDescriptorSets(RenderCore::Get().GetDevice(), writes.size(), writes.data(), 0, nullptr);
	}
}
