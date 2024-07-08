#ifndef __SCOP_GRAPHICS_PIPELINE__
#define __SCOP_GRAPHICS_PIPELINE__

#include <vector>

#include <Renderer/Pipelines/Pipeline.h>
#include <Utils/NonOwningPtr.h>

namespace Scop
{
	class GraphicPipeline : public Pipeline
	{
		public:
			GraphicPipeline() = default;

			void Init(NonOwningPtr<class Renderer> renderer);
			bool BindPipeline(VkCommandBuffer command_buffer) noexcept override;
			void EndPipeline(VkCommandBuffer command_buffer) noexcept override;
			void Destroy() noexcept;

			const VkPipelineLayout& GetPipelineLayout() const override;
			inline const VkPipeline& GetPipeline() const override { return m_pipeline; }
			inline const VkPipelineBindPoint& GetPipelineBindPoint() const override { return m_pipeline_bind_point; }

			~GraphicPipeline() = default;

		private:
			void CreateFramebuffers();
			void TransitionAttachments();

		private:
			std::vector<VkFramebuffer> m_framebuffers;
			std::vector<VkDescriptorSetLayout> m_descriptor_sets_layout;
			std::vector<VkDescriptorSet> m_descriptor_sets;
			VkRenderPass m_renderpass;
			VkDescriptorPool m_pool;
			VkPipeline m_pipeline = VK_NULL_HANDLE;
			VkPipelineBindPoint m_pipeline_bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
			NonOwningPtr<class Renderer> m_renderer;
	};
}

#endif
