#include <Renderer/Memory/DeviceAllocator.h>
#include <Core/Logs.h>

#include <optional>

namespace Scop
{
	constexpr VkDeviceSize CHUNK_SIZE = 4096;

	[[nodiscard]] MemoryBlock DeviceAllocator::Allocate(VkDeviceSize size, VkDeviceSize alignment, std::int32_t memory_type_index)
	{
		Verify(m_device != VK_NULL_HANDLE, "invalid device");
		Verify(m_physical != VK_NULL_HANDLE, "invalid physical device");
		for(auto& chunk : m_chunks)
		{
			if(chunk.GetMemoryTypeIndex() == memory_type_index)
			{
				std::optional<MemoryBlock> block = chunk.Allocate(size, alignment);
				if(block.has_value())
					return *block;
			}
		}
		MemoryChunk& chunk = m_chunks.emplace_back(m_device, m_physical, (CHUNK_SIZE < size + alignment ? size + alignment : CHUNK_SIZE), memory_type_index);
		std::optional<MemoryBlock> block = chunk.Allocate(size, alignment);
		if(block.has_value())
			return *block;
		FatalError("Device Allocator : could not allocate a memory block");
		return {}; // to avoid warnings
	}

	void DeviceAllocator::Deallocate(const MemoryBlock& block)
	{
		Verify(m_device != VK_NULL_HANDLE, "invalid device");
		Verify(m_physical != VK_NULL_HANDLE, "invalid physical device");
		for(auto& chunk : m_chunks)
		{
			if(chunk.Has(block))
			{
				chunk.Deallocate(block);
				return;
			}
		}
		Error("Device Allocator : unable to free a block; could not find it's chunk");
	}
}
