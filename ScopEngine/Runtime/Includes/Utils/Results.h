#ifndef __SCOP_RESULTS__
#define __SCOP_RESULTS__

#include <variant>

namespace Scop
{
	template<typename T, typename E>
	class Result
	{
		public:
			Result() = default;

			[[nodiscard]] inline bool IsOK() const noexcept;
			[[nodiscard]] inline T& GetValue() noexcept;

			~Result() = default;

		private:
			std::variant<T, E> m_value;
	};
}

#endif
