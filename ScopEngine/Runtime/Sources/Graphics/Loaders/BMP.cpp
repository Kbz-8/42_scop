#include <Graphics/Loaders/BMP.h>
#include <Maths/Vec3.h>

#include <fstream>

namespace Scop
{
	CPUBuffer LoadBMPFile(const std::filesystem::path& path, Vec2ui32& dimensions)
	{
		if(path.extension() != ".bmp")
		{
			Error("BMP loader : not a BMP file, %", path);
			return {};
		}
		std::ifstream file(path, std::ios::binary);
		if(!file.is_open())
		{
			Error("BMP loader : could not open %", path);
			return {};
		}
		std::int16_t bpp;
		file.seekg(18, std::ios_base::beg);
		file.read(reinterpret_cast<char*>(&dimensions.x), sizeof(dimensions.x));
		file.read(reinterpret_cast<char*>(&dimensions.y), sizeof(dimensions.y));
		file.seekg(28, std::ios_base::beg);
		file.read(reinterpret_cast<char*>(&bpp), sizeof(bpp));
		file.seekg(54, std::ios_base::beg);
		if(bpp != 24)
			Warning("BMP loader : warning while loadeing %, cannot handle yet different color palette sizes", path);
		CPUBuffer buffer{ dimensions.x * dimensions.y * 4 };
		for(std::size_t i = 0; i <= buffer.GetSize(); i += 4)
		{
			Vec3b data{ 0, 0, 0 };
			data.r = (file.eof() ? 0xFF : file.get());
			data.g = (file.eof() ? 0xFF : file.get());
			data.b = (file.eof() ? 0xFF : file.get());
			buffer.GetData()[i + 0] = data.b;
			buffer.GetData()[i + 1] = data.g;
			buffer.GetData()[i + 2] = data.r;
			buffer.GetData()[i + 3] = 0xFF;
		}
		return buffer;
	}
}