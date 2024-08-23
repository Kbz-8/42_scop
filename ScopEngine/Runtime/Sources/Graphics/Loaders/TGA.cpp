#include <Graphics/Loaders/TGA.h>

#include <fstream>

namespace Scop
{
	CPUBuffer LoadTGAFile(const std::filesystem::path& path, Vec2ui32& dimensions)
	{
		if(path.extension() != ".tga")
		{
			Error("TGA loader : not a TGA file, %", path);
			return {};
		}
		std::ifstream file(path, std::ios::binary);
		if(!file.is_open())
		{
			Error("TGA loader : could not open %", path);
			return {};
		}
	}
}
