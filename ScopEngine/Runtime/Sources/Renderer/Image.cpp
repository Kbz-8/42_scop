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

		// TODO
		FatalError("not implemented yet");
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
			kvfDestroyImage(RenderCore::Get().GetDevice(), m_image);
		}
		m_image = VK_NULL_HANDLE;
	}
}
