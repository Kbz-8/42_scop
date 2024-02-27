#ifndef __SCOPE_CORE_ENUMS__
#define __SCOPE_CORE_ENUMS__

#include <cstddef>

enum class LogType
{
	Debug = 0,
	Message,
	Warning,
	Error,
	FatalError,

	EndEnum
};

constexpr std::size_t LogTypeCount = static_cast<std::size_t>(LogType::EndEnum) + 1;

#endif
