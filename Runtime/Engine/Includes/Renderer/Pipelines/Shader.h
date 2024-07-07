#ifndef __SCOP_SHADER__
#define __SCOP_SHADER__

#include <vector>
#include <cstdint>

namespace Scop
{
	class Shader
	{
		public:
			Shader(const std::vector<std::uint32_t>& bytecode);

			inline const std::vector<std::uint32_t>& GetByteCode() const noexcept { return m_bytecode; }

			~Shader();

		private:
			std::vector<std::uint32_t> m_bytecode;
	};
}

#endif
