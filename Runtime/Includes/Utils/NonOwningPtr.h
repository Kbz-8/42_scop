#ifndef __SCOPE_UTILS_NON_OWNING_PTR__
#define __SCOPE_UTILS_NON_OWNING_PTR__

template<typename T>
class NonOwningPtr
{
	public:
		NonOwningPtr(T* ptr = nullptr);
		NonOwningPtr(const NonOwningPtr&) = default;
		NonOwningPtr(NonOwningPtr&& ptr) noexcept;

		NonOwningPtr& operator=(T* ptr);
		NonOwningPtr& operator=(const NonOwningPtr&) = default;
		NonOwningPtr& operator=(NonOwningPtr&& ptr) noexcept;

		inline operator bool() const noexcept;

		inline T* Get() const noexcept;
		inline T* operator->() const noexcept;
		inline T& operator*() const noexcept;

		~NonOwningPtr() = default;

	private:
		T* m_ptr = nullptr;
};

#include <Utils/NonOwningPtr.inl>

#endif
