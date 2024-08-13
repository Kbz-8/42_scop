#include <Renderer/Image.h>
#include <Renderer/RenderCore.h>
#include <Core/Logs.h>

namespace Scop
{
	void Image::Init(std::uint32_t width, std::uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, std::vector<VkMemoryPropertyFlags> properties)
	{
		m_width = width;
		m_height = height;
		m_format = format;
		m_tiling = tiling;

		m_image = kvfCreateImage(RenderCore::Get().GetDevice(), width, height, format, tiling, usage);

		VkMemoryRequirements mem_requirements;
		vkGetImageMemoryRequirements(RenderCore::Get().GetDevice(), m_image, &mem_requirements);

		std::optional<std::uint32_t> mem_type_index;
		for(auto prop : properties)
		{
			mem_type_index = FindMemoryType(mem_requirements.memoryTypeBits, prop, false);
			if(mem_type_index.has_value())
				break;
		}
		if(!mem_type_index.has_value())
			FatalError("Vulkan : failed to find suitable memory type for an image");

		VkMemoryAllocateInfo alloc_info{};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = mem_requirements.size;
		alloc_info.memoryTypeIndex = *mem_type_index;

		if(vkAllocateMemory(RenderCore::Get().GetDevice(), &alloc_info, nullptr, &m_memory) != VK_SUCCESS)
			FatalError("Vulkan : failed to allocate memory for an image");
		vkBindImageMemory(RenderCore::Get().GetDevice(), m_image, m_memory, 0);
		Message("Vulkan : image created");
		s_image_count++;
	}

	void Image::CreateImageView(VkImageViewType type, VkImageAspectFlags aspect_flags) noexcept
	{
		m_image_view = kvfCreateImageView(RenderCore::Get().GetDevice(), m_image, m_format, type, aspect_flags);
	}

	void Image::CreateSampler() noexcept
	{
		// TODO
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
		// TODO
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
			vkFreeMemory(RenderCore::Get().GetDevice(), m_memory, nullptr);
			kvfDestroyImage(RenderCore::Get().GetDevice(), m_image);
		}
		Message("Vulkan : image destroyed");
		m_image = VK_NULL_HANDLE;
		s_image_count--;
	}
}
