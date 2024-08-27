#include <Renderer/Image.h>
#include <Renderer/RenderCore.h>
#include <Core/Logs.h>

namespace Scop
{
	void Image::Init(std::uint32_t width, std::uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
	{
		m_width = width;
		m_height = height;
		m_format = format;
		m_tiling = tiling;

		m_image = kvfCreateImage(RenderCore::Get().GetDevice(), width, height, format, tiling, usage);

		VkMemoryRequirements mem_requirements;
		vkGetImageMemoryRequirements(RenderCore::Get().GetDevice(), m_image, &mem_requirements);

		m_memory = RenderCore::Get().GetAllocator().Allocate(mem_requirements.size, mem_requirements.alignment, *FindMemoryType(mem_requirements.memoryTypeBits, properties));
		vkBindImageMemory(RenderCore::Get().GetDevice(), m_image, m_memory.memory, 0);
		Message("Vulkan : image created");
		s_image_count++;
	}

	void Image::CreateImageView(VkImageViewType type, VkImageAspectFlags aspect_flags) noexcept
	{
		m_image_view = kvfCreateImageView(RenderCore::Get().GetDevice(), m_image, m_format, type, aspect_flags);
	}

	void Image::CreateSampler() noexcept
	{
		m_sampler = kvfCreateSampler(RenderCore::Get().GetDevice(), VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_MIPMAP_MODE_NEAREST);
	}

	void Image::TransitionLayout(VkImageLayout new_layout, VkCommandBuffer cmd)
	{
		if(new_layout == m_layout)
			return;
		bool is_single_time_cmd_buffer = cmd == VK_NULL_HANDLE;
		if(is_single_time_cmd_buffer)
			cmd = kvfCreateCommandBuffer(RenderCore::Get().GetDevice());
		kvfTransitionImageLayout(RenderCore::Get().GetDevice(), m_image, cmd, m_format, m_layout, new_layout, is_single_time_cmd_buffer);
		m_layout = new_layout;
	}

	void Image::DestroySampler() noexcept
	{
		if(m_sampler != VK_NULL_HANDLE)
			kvfDestroySampler(RenderCore::Get().GetDevice(), m_sampler);
		m_sampler = VK_NULL_HANDLE;
	}

	void Image::DestroyImageView() noexcept
	{
		if(m_image_view != VK_NULL_HANDLE)
			kvfDestroyImageView(RenderCore::Get().GetDevice(), m_image_view);
		m_image_view = VK_NULL_HANDLE;
	}

	void Image::Destroy() noexcept
	{
		DestroySampler();
		DestroyImageView();

		if(m_image != VK_NULL_HANDLE)
		{
			RenderCore::Get().GetAllocator().Deallocate(m_memory);
			m_memory = NULL_MEMORY_BLOCK;
			kvfDestroyImage(RenderCore::Get().GetDevice(), m_image);
		}
		Message("Vulkan : image destroyed");
		m_image = VK_NULL_HANDLE;
		s_image_count--;
	}
}
