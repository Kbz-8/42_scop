#ifndef __SCOPE_UTILS_NON_MOVABLE__
#define __SCOPE_UTILS_NON_MOVABLE__

class NonMovable
{
	protected:
		NonMovable() = default;
		virtual ~NonMovable() = default;

	public:
		NonMovable(const NonMovable&) = default;
		NonMovable(NonMovable&&) noexcept = delete;
		NonMovable& operator=(const NonMovable&) = default;
		NonMovable& operator=(NonMovable&&) noexcept = delete;
};

#endif
