#ifndef __SCOP_GPU_BUFFER__
#define __SCOP_GPU_BUFFER__

#include <kvf.h>
#include <Renderer/Vulkan/VulkanPrototypes.h>
#include <Renderer/Enums.h>
#include <Core/Logs.h>
#include <Renderer/RenderCore.h>
#include <Utils/Buffer.h>

namespace Scop
{
	class GPUBuffer
	{
		public:
			GPUBuffer() = default;

			void Init(BufferType type, VkDeviceSize size, VkBufferUsageFlags usage, CPUBuffer data);
			void Destroy() noexcept;

			inline void MapMem(void** data = nullptr, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) noexcept
			{
				if(vkMapMemory(RenderCore::Get().GetDevice(), m_memory, m_offset + offset, size, 0, data) != VK_SUCCESS)
					FatalError("Vulkan : failed to map a buffer");
				m_is_mapped = true;
			}
			[[nodiscard]] inline bool IsMapped() const noexcept { return m_is_mapped; }
			inline void UnMapMem() noexcept { vkUnmapMemory(RenderCore::Get().GetDevice(), m_memory); m_is_mapped = false; }

			bool CopyFrom(const GPUBuffer& buffer) noexcept;

			void Swap(GPUBuffer& buffer) noexcept;

			[[nodiscard]] inline VkBuffer operator()() const noexcept { return m_buffer; }
			[[nodiscard]] inline VkBuffer Get() const noexcept { return m_buffer; }
			[[nodiscard]] inline VkDeviceMemory GetMemory() const noexcept { return m_memory; }
			[[nodiscard]] inline VkDeviceSize GetSize() const noexcept { return m_size; }
			[[nodiscard]] inline VkDeviceSize GetOffset() const noexcept { return m_offset; }

			[[nodiscard]] inline bool IsInit() const noexcept { return m_buffer != VK_NULL_HANDLE; }

			~GPUBuffer() = default;

		protected:
			void PushToGPU() noexcept;

		protected:
			VkBuffer m_buffer = VK_NULL_HANDLE;
			VkDeviceMemory m_memory = VK_NULL_HANDLE;
			VkDeviceSize m_size = 0;
			VkDeviceSize m_offset = 0;

		private:
			void CreateBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

		private:
			VkBufferUsageFlags m_usage = 0;
			VkMemoryPropertyFlags m_flags = 0;
			bool m_is_mapped = false;
	};

	class VertexBuffer : public GPUBuffer
	{
		public:
			inline void Init(std::uint32_t size, VkBufferUsageFlags additional_flags = 0) { GPUBuffer::Init(BufferType::LowDynamic, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | additional_flags, {}); }
			void SetData(CPUBuffer data);
			inline void Bind(VkCommandBuffer cmd) const noexcept { vkCmdBindVertexBuffers(cmd, 0, 1, &m_buffer, &m_offset); }
	};

	class IndexBuffer : public GPUBuffer
	{
		public:
			inline void Init(std::uint32_t size, VkBufferUsageFlags additional_flags = 0) { GPUBuffer::Init(BufferType::LowDynamic, size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | additional_flags, {}); }
			void SetData(CPUBuffer data);
			inline void Bind(VkCommandBuffer cmd) const noexcept { vkCmdBindIndexBuffer(cmd, m_buffer, m_offset, VK_INDEX_TYPE_UINT32); }
	};

	class UniformBuffer
	{
		public:
			void Init(std::uint32_t size);
			void SetData(CPUBuffer data, std::size_t frame_index);
			void Destroy() noexcept;

			inline VkDeviceSize GetSize(int i) const noexcept { return m_buffers[i].GetSize(); }
			inline VkDeviceSize GetOffset(int i) const noexcept { return m_buffers[i].GetOffset(); }
			inline VkBuffer GetVk(int i) const noexcept { return m_buffers[i].Get(); }
			inline GPUBuffer& Get(int i) noexcept { return m_buffers[i]; }

		private:
			std::array<GPUBuffer, MAX_FRAMES_IN_FLIGHT> m_buffers;
			std::array<void*, MAX_FRAMES_IN_FLIGHT> m_maps;
	};
}

#endif
