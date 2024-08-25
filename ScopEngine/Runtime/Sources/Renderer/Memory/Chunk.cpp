#include "Renderer/Memory/Block.h"
#include <Renderer/Memory/Chunk.h>
#include <Renderer/Vulkan/VulkanPrototypes.h>
#include <Core/Logs.h>

#include <algorithm>

namespace Scop
{
	MemoryChunk::MemoryChunk(VkDevice device, VkPhysicalDevice physical, VkDeviceSize size, std::int32_t memory_type_index)
		: m_device(device), m_physical(physical), m_size(size), m_memory_type_index(memory_type_index)
	{
		Verify(device != VK_NULL_HANDLE, "Memory Chunk : invalid device");
		VkMemoryAllocateInfo alloc_info{};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = size;
		alloc_info.memoryTypeIndex = m_memory_type_index;
		if(vkAllocateMemory(m_device, &alloc_info, nullptr, &m_memory) != VK_SUCCESS)
			FatalError("Vulkan : failed to allocate memory for a chunk");

		VkPhysicalDeviceMemoryProperties properties;
		vkGetPhysicalDeviceMemoryProperties(m_physical, &properties);
		if((properties.memoryTypes[m_memory_type_index].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
		{
			if(vkMapMemory(m_device, m_memory, 0, VK_WHOLE_SIZE, 0, &p_map) != VK_SUCCESS)
				FatalError("Vulkan : failed to map a host visible chunk");
		}
	}

	[[nodiscard]] std::optional<MemoryBlock> MemoryChunk::Allocate(VkDeviceSize size, VkDeviceSize alignment)
	{
		for(auto& block : m_blocks)
		{
			if(!block.free || size > block.size)
				continue;
			VkDeviceSize offset = (block.offset % alignment > 0 ? alignment - block.offset % alignment : 0);
			if(offset + size <= block.size)
			{
				block.free = false;
				return block;
			}
		}
		VkDeviceSize offset = (m_used_size % alignment > 0 ? alignment - m_used_size % alignment : 0);
		if(size > m_size - (m_used_size + offset))
			return std::nullopt;
		MemoryBlock& block = m_blocks.emplace_back();
		block.memory = m_memory;
		block.offset = m_used_size + offset;
		block.size = size;
		if(p_map != nullptr)
			block.map = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(p_map) + block.offset);
		block.free = false;
		return block;
	}

	void MemoryChunk::Deallocate(const MemoryBlock& block)
	{
		auto it = std::find(m_blocks.begin(), m_blocks.end(), block);
		if(it == m_blocks.end())
			FatalError("Memory Chunk : cannot deallocate a block that is owned by another chunk");
		it->free = true;
	}

	MemoryChunk::~MemoryChunk()
	{
		vkFreeMemory(m_device, m_memory, nullptr);
	}
}
