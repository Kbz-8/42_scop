#ifndef __SCOP_IMAGE__
#define __SCOP_IMAGE__

#include <cstdint>
#include <vector>
#include <kvf.h>

#include <Renderer/RenderCore.h>

namespace Scop
{
	class Image
	{
		public:
			Image() = default;

			inline void Init(VkImage image, VkFormat format, std::uint32_t width, std::uint32_t height, VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED) noexcept
			{
				m_image = image;
				m_format = format;
				m_width = width;
				m_height = height;
				m_layout = layout;
			}

			void Init(std::uint32_t width, std::uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, std::vector<VkMemoryPropertyFlags> properties);
			void CreateImageView(VkImageViewType type, VkImageAspectFlags aspectFlags) noexcept;
			void CreateSampler() noexcept;
			void TransitionLayout(VkImageLayout new_layout, VkCommandBuffer cmd = VK_NULL_HANDLE);

			void DestroySampler() noexcept;
			void DestroyImageView() noexcept;
			virtual void Destroy() noexcept;

			[[nodiscard]] inline VkImage Get() const noexcept { return m_image; }
			[[nodiscard]] inline VkImage operator()() const noexcept { return m_image; }
			[[nodiscard]] inline VkDeviceMemory GetDeviceMemory() const noexcept { return m_memory; }
			[[nodiscard]] inline VkImageView GetImageView() const noexcept { return m_image_view; }
			[[nodiscard]] inline VkFormat GetFormat() const noexcept { return m_format; }
			[[nodiscard]] inline VkImageTiling GetTiling() const noexcept { return m_tiling; }
			[[nodiscard]] inline VkImageLayout GetLayout() const noexcept { return m_layout; }
			[[nodiscard]] inline VkSampler GetSampler() const noexcept { return m_sampler; }
			[[nodiscard]] inline std::uint32_t GetWidth() const noexcept { return m_width; }
			[[nodiscard]] inline std::uint32_t GetHeight() const noexcept { return m_height; }
			[[nodiscard]] inline bool IsInit() const noexcept { return m_image != VK_NULL_HANDLE; }

			virtual ~Image() = default;

		private:
			VkDeviceMemory m_memory = VK_NULL_HANDLE;
			VkImage m_image = VK_NULL_HANDLE;
			VkImageView m_image_view = VK_NULL_HANDLE;
			VkSampler m_sampler = VK_NULL_HANDLE;
			VkFormat m_format;
			VkImageTiling m_tiling;
			VkImageLayout m_layout = VK_IMAGE_LAYOUT_UNDEFINED;
			std::uint32_t m_width = 0;
			std::uint32_t m_height = 0;
	};

	class DepthImage : public Image
	{
		public:
			DepthImage() = default;
			inline void Init(std::uint32_t width, std::uint32_t height)
			{
				std::vector<VkFormat> candidates = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
				VkFormat format = kvfFindSupportFormatInCandidates(RenderCore::Get().GetDevice(), candidates.data(), candidates.size(), VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
				Image::Init(width, height, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, { VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT });
				Image::CreateImageView(VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_DEPTH_BIT);
				Image::TransitionLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
			}
			~DepthImage() = default;
	};
}

#endif
