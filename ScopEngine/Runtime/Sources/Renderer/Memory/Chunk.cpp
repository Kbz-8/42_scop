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
		MemoryBlock& block = m_blocks.emplace_back();
		block.memory = m_memory;
		block.offset = 0;
		block.size = size;
		block.free = true;
	}

	[[nodiscard]] std::optional<MemoryBlock> MemoryChunk::Allocate(VkDeviceSize size, VkDeviceSize alignment)
	{
		for(std::size_t i = 0; i < m_blocks.size(); i++)
		{
			if(!m_blocks[i].free || m_blocks[i].size < size)
				continue;
			VkDeviceSize offset_displacement = (m_blocks[i].offset % alignment > 0) ? alignment - m_blocks[i].offset % alignment : 0;
			VkDeviceSize old_size_available = m_blocks[i].size - offset_displacement;

			if(size + offset_displacement <= m_blocks[i].size)
			{
				m_blocks[i].offset += offset_displacement;
				m_blocks[i].size = size;
				m_blocks[i].free = false;

				if(p_map != nullptr)
					m_blocks[i].map = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(p_map) + m_blocks[i].offset);

				MemoryBlock new_block;
				new_block.memory = m_memory;
				new_block.offset = m_blocks[i].offset + size;
				new_block.size = old_size_available - size;
				new_block.free = true;

				if(new_block.size > 0)
					m_blocks.emplace(m_blocks.begin() + i + 1, new_block);
				return m_blocks[i];
			}
		}
		return std::nullopt;
	}

	void MemoryChunk::Deallocate(const MemoryBlock& block)
	{
		auto it = std::find(m_blocks.begin(), m_blocks.end(), block);
		if(it == m_blocks.end())
			FatalError("Memory Chunk : cannot deallocate a block that is owned by another chunk");
		it->free = true;

		bool end = false;
		while(!end)
		{
			end = true;
			for(auto it = m_blocks.begin(); it != m_blocks.end(); ++it)
			{
				if(it->free && (it + 1)->free)
				{
					it->size = (it + 1)->offset + (it + 1)->size - it->offset;
					m_blocks.erase(it + 1);
					end = false;
					break;
				}
			}
		}
	}

	MemoryChunk::~MemoryChunk()
	{
		vkFreeMemory(m_device, m_memory, nullptr);
	}
}