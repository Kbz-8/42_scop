#ifndef __SCOPE_CORE_BASE_EVENT__
#define __SCOPE_CORE_BASE_EVENT__

#include <cstdint>

namespace Scop
{
	struct EventBase
	{
		virtual std::uint32_t What() const = 0;
	};
}

#endif
