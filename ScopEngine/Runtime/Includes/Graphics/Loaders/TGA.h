#ifndef __SCOP_TGA_LOADER__
#define __SCOP_TGA_LOADER__

#include <filesystem>

#include <Maths/Vec2.h>
#include <Utils/Buffer.h>

namespace Scop
{
	CPUBuffer LoadTGAFile(const std::filesystem::path& path, Vec2ui32& dimensions);
}

#endif
