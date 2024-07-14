#ifndef __SCOP_GRAPHICS_PIPELINE__
#define __SCOP_GRAPHICS_PIPELINE__

#include <memory>
#include <vector>

#include <kvf.h>

#include <Renderer/Image.h>
#include <Renderer/Pipelines/Shader.h>
#include <Renderer/ShadersLibrary.h>
#include <Renderer/Pipelines/Pipeline.h>

namespace Scop
{
	class GraphicPipeline : public Pipeline
	{
		public:
			GraphicPipeline() = default;

			void Init(std::shared_ptr<Shader> vertex_shader, std::shared_ptr<Shader> fragment_shader);
			bool BindPipeline(VkCommandBuffer command_buffer) noexcept override;
			void EndPipeline(VkCommandBuffer command_buffer) noexcept override;
			void Destroy() noexcept;

			inline VkPipeline GetPipeline() const override { return m_pipeline; }
			inline VkPipelineLayout GetPipelineLayout() const override { return m_pipeline_layout; }
			inline VkPipelineBindPoint GetPipelineBindPoint() const override { return VK_PIPELINE_BIND_POINT_GRAPHICS; }

			~GraphicPipeline() = default;

		private:
			void CreateFramebuffers();
			void TransitionAttachments();

		private:
			std::vector<VkFramebuffer> m_framebuffers;
			std::shared_ptr<Shader> m_vertex_shader;
			std::shared_ptr<Shader> m_fragment_shader;
			VkRenderPass m_renderpass = VK_NULL_HANDLE;
			VkPipeline m_pipeline = VK_NULL_HANDLE;
			VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
	};
}

#endif
