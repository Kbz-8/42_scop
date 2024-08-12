#ifndef __SCOP_OBJ_LOADER__
#define __SCOP_OBJ_LOADER__

#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include <filesystem>

namespace Scop
{
	struct ObjData
	{
		struct FaceVertex
		{
			FaceVertex() : v(-1), t(-1), n(-1) {}
			std::int32_t v;
			std::int32_t t;
			std::int32_t n;

			inline bool operator<(const FaceVertex& rhs) const
			{
				return (v < rhs.v) || (v == rhs.v && t < rhs.t ) || (v == rhs.v && t == rhs.t && n < rhs.n);
			}
			inline bool operator==(const FaceVertex& rhs) const
			{
				return (v == rhs.v && t == rhs.t && n == rhs.n);
			}
		};

		using FaceList = std::pair<std::vector<FaceVertex>, std::vector<unsigned> >;

		std::vector<float> vertex; //< 3 * N entries
		std::vector<float> tex_coord; //< 2 * N entries
		std::vector<float> normal; //< 3 * N entries

		std::map<std::string, FaceList> faces;
	};

	ObjData LoadOBJFromFile(const std::filesystem::path& path);
	void TesselateOBJData(ObjData& data) noexcept;
}

#endif
