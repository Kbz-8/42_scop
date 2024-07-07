#ifndef __SCOP_SHADERS_LIBRARY__
#define __SCOP_SHADERS_LIBRARY__

#include <cstdint>
#include <memory>
#include <unordered_map>

#include <Utils/Singleton.h>

namespace Scop
{
	using ShaderID = std::uint32_t;
	constexpr ShaderID nullshader = 0;

	class ShadersLibrary : public Singleton<ShadersLibrary>
	{
		friend class Singleton<ShadersLibrary>;

		public:
			std::shared_ptr<class Shader> GetShader(ShaderID id);
			ShaderID AddShaderToLibrary(std::shared_ptr<class Shader> shader);
			void RemoveShaderFromLibrary(ShaderID id);

			void ClearLibrary();

		private:
			ShadersLibrary() = default;
			~ShadersLibrary() = default;

		private:
			std::unordered_map<ShaderID, std::shared_ptr<class Shader>> m_cache;
			ShaderID m_current_id = 1;
	};
}

#endif
