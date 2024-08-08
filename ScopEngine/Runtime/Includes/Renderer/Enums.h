#ifndef __SCOP_RENDERER_ENUMS__
#define __SCOP_RENDERER_ENUMS__

#include <cstddef>

namespace Scop
{
	enum class BufferType
	{
		Constant = 0,
		Staging,
		HighDynamic, // typically stored in RAM
		LowDynamic,  // typically stored in VRAM

		EndEnum
	};
	constexpr std::size_t BufferTypeCount = static_cast<std::size_t>(BufferType::EndEnum) + 1;
}

#endif
