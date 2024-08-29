#ifndef __SCOP_GRAPHICS_PIPELINE__
#define __SCOP_GRAPHICS_PIPELINE__

#include <memory>
#include <vector>

#include <kvf.h>

#include <Renderer/Image.h>
#include <Utils/NonOwningPtr.h>
#include <Renderer/Pipelines/Shader.h>
#include <Renderer/Pipelines/Pipeline.h>

namespace Scop
{
	class GraphicPipeline : public Pipeline
	{
		public:
			GraphicPipeline() = default;

			void Init(std::shared_ptr<Shader> vertex_shader, std::shared_ptr<Shader> fragment_shader, NonOwningPtr<class Renderer> renderer, NonOwningPtr<DepthImage> depth, VkCullModeFlagBits culling = VK_CULL_MODE_FRONT_BIT, bool disable_vertex_inputs = false);
			void Init(std::shared_ptr<Shader> vertex_shader, std::shared_ptr<Shader> fragment_shader, std::vector<NonOwningPtr<Image>> attachments, VkCullModeFlagBits culling = VK_CULL_MODE_FRONT_BIT, bool disable_vertex_inputs = false);
			bool BindPipeline(VkCommandBuffer command_buffer, std::size_t framebuffer_index, std::array<float, 4> clear) noexcept;
			void EndPipeline(VkCommandBuffer command_buffer) noexcept override;
			void Destroy() noexcept;

			[[nodiscard]] inline VkPipeline GetPipeline() const override { return m_pipeline; }
			[[nodiscard]] inline VkPipelineLayout GetPipelineLayout() const override { return m_pipeline_layout; }
			[[nodiscard]] inline VkPipelineBindPoint GetPipelineBindPoint() const override { return VK_PIPELINE_BIND_POINT_GRAPHICS; }

			~GraphicPipeline() = default;

		private:
			void CreateFramebuffers(const std::vector<NonOwningPtr<Image>>& render_targets);
			void TransitionAttachments(VkCommandBuffer cmd = VK_NULL_HANDLE);

			// Private override to remove access
			bool BindPipeline(VkCommandBuffer) noexcept override { return false; };

		private:
			std::vector<NonOwningPtr<Image>> m_attachments;
			std::vector<VkFramebuffer> m_framebuffers;
			std::vector<VkClearValue> m_clears;
			std::shared_ptr<Shader> p_vertex_shader;
			std::shared_ptr<Shader> p_fragment_shader;
			VkRenderPass m_renderpass = VK_NULL_HANDLE;
			VkPipeline m_pipeline = VK_NULL_HANDLE;
			VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
			NonOwningPtr<class Renderer> p_renderer;
			NonOwningPtr<DepthImage> p_depth;
	};
}

#endif
