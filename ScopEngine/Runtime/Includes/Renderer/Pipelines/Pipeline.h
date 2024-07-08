#ifndef __SCOP_PIPELINE__
#define __SCOP_PIPELINE__

#include <kvf.h>

namespace Scop
{
	class Pipeline
	{
		public:
			Pipeline() = default;

			inline virtual bool BindPipeline(VkCommandBuffer command_buffer) noexcept { vkCmdBindPipeline(command_buffer, GetPipelineBindPoint(), GetPipeline()); return true; }
			inline virtual void EndPipeline([[maybe_unused]] VkCommandBuffer command_buffer) noexcept {}

			virtual const VkPipeline& GetPipeline() const = 0;
			virtual const VkPipelineLayout& GetPipelineLayout() const = 0;
			virtual const VkPipelineBindPoint& GetPipelineBindPoint() const = 0;

			virtual ~Pipeline() = default;
	};
}

#endif
