/*
MIT License

Copyright (c) 2024 Thomas Gottschalk Barnekov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include <functional>
#include <memory>


template<typename T, T NullValue = static_cast<T>(0), typename RT = int>
class WinHandle
{
public:
	using element_type = T;

	class MutableHandle;

#pragma region Constructors
	// Constructors
	WinHandle();
	explicit WinHandle(T handle, nullptr_t);

	// std::function
	explicit WinHandle(std::function<RT(T)> deleter);
	explicit WinHandle(T handle, std::function<RT(T)> deleter);

	// Function pointer
	template<typename DType>
	explicit WinHandle(RT(__stdcall* deleter)(DType));

	template<typename DType, typename... Args>
	explicit WinHandle(RT(__stdcall* deleter)(DType, Args...), Args&&... args);

	template<typename DType>
	explicit WinHandle(T handle, RT(__stdcall* deleter)(DType));

	template<typename DType, typename... Args>
	explicit WinHandle(T handle, RT(__stdcall* deleter)(DType, Args...), Args&&... args);

	// Member function pointer
	template<typename Class, typename DType>
	explicit WinHandle(RT(__stdcall Class::* deleter)(DType), Class* instance);

	template<typename Class, typename DType, typename... Args>
	explicit WinHandle(RT(__stdcall Class::* deleter)(DType, Args...), Class* instance, Args&&... args);

	template<typename Class, typename DType>
	explicit WinHandle(T handle, RT(__stdcall Class::* deleter)(DType), Class* instance);

	template<typename Class, typename DType, typename... Args>
	explicit WinHandle(T handle, RT(__stdcall Class::* deleter)(DType, Args...), Class* instance, Args&&... args);
#pragma endregion

#pragma region Copy and move constructors
	// Copy and move constructors
	WinHandle(const WinHandle&) noexcept = default;
	WinHandle(WinHandle&& move) noexcept;
#pragma endregion

#pragma region Copy and move operators
	// Copy and move operators
	WinHandle& operator =(const WinHandle&) noexcept = default;
	WinHandle& operator =(WinHandle&& move) noexcept;
#pragma endregion

#pragma region Destructor
	// Destructor
	~WinHandle() noexcept = default;
#pragma endregion

#pragma region Assignment operators
	// Assignment operators
	WinHandle& operator=(const T& handle) noexcept;
#pragma endregion

#pragma region Conversion operators
	// Conversion operators
	// Safe Bool Idiom
	using bool_type = void (WinHandle::*)() const noexcept;
	operator bool_type() const noexcept;
#pragma endregion

#pragma region Smart pointer operations
	// Smart pointer operations
	void swap(WinHandle& other) noexcept;
	void reset();
	void reset(T handle);
	long use_count() const noexcept;
#pragma endregion

#pragma region Handle operations
	// Handle operations
	bool valid() const noexcept;
	T get() const noexcept;
	const T* ptr() const noexcept; // Returns a non-mutable pointer to the handle
	[[nodiscard]] MutableHandle ptr() noexcept; // Returns a mutable pointer to the handle
	RT close() noexcept; // Close the handle using the assigned deleter
#pragma endregion

private:
	// Safe Bool Idiom
	void this_type_does_not_support_comparisons() const noexcept {}

#pragma region impl
	class impl
	{
	public:
		// Constructors
		explicit impl(T handle, nullptr_t) noexcept;

		// std::function
		explicit impl(T handle, std::function<RT(T)> deleter);

		// Copy and move
		impl(const impl&) = delete;
		impl(impl&&) = default;
		impl& operator=(const impl&) = delete;
		impl& operator=(impl&&) = default;

		// Destructor
		~impl() noexcept;

		// Assignment
		RT assign(T v) noexcept;

		// Member access
		T get() const noexcept;
		const T* ptr() const noexcept;
		std::function<RT(T)> deleter() const noexcept;

	private:
		RT destroy() noexcept;

		T m_handle{ NullValue };
		std::function<RT(T)> m_deleter{ nullptr };
	};
#pragma endregion

#pragma region MutableHandle
	class MutableHandle
	{
	public:
		// Constructors
		MutableHandle() = delete;
		explicit MutableHandle(WinHandle& owner) noexcept;

		// Copy and move
		MutableHandle(const MutableHandle&) = delete;
		MutableHandle(MutableHandle&&) = delete;
		MutableHandle& operator=(const MutableHandle&) = delete;
		MutableHandle& operator=(MutableHandle&&) = delete;

		// Destructor
		~MutableHandle() noexcept;

		// Conversion operator
		operator T* () noexcept;

	private:
		WinHandle& m_owner;
		T m_handle{ NullValue };
	};
#pragma endregion

	std::shared_ptr<impl> m_impl;
};


#pragma region Comparison operators

#pragma region WinHandle comparison
// Comparison operators
template<typename T, T NullValue, typename RT, typename U>
bool operator ==(const WinHandle<T, NullValue, RT>& lhs, const WinHandle<U, NullValue, RT>& rhs) noexcept
{
	return lhs.get() == rhs.get();
}

#if !__cpp_impl_three_way_comparison

template<typename T, T NullValue, typename RT, typename U>
bool operator !=(const WinHandle<T, NullValue, RT>& lhs, const WinHandle<U, NullValue, RT>& rhs) noexcept
{
	return lhs.get() != rhs.get();
}

template<typename T, T NullValue, typename RT, typename U>
bool operator <(const WinHandle<T, NullValue, RT>& lhs, const WinHandle<U, NullValue, RT>& rhs) noexcept
{
	return lhs.get() < rhs.get();
}

template<typename T, T NullValue, typename RT, typename U>
bool operator <=(const WinHandle<T, NullValue, RT>& lhs, const WinHandle<U, NullValue, RT>& rhs) noexcept
{
	return lhs.get() <= rhs.get();
}

template<typename T, T NullValue, typename RT, typename U>
bool operator >(const WinHandle<T, NullValue, RT>& lhs, const WinHandle<U, NullValue, RT>& rhs)
{
	return lhs.get() > rhs.get();
}

template<typename T, T NullValue, typename RT, typename U>
bool operator >=(const WinHandle<T, NullValue, RT>& lhs, const WinHandle<U, NullValue, RT>& rhs) noexcept
{
	return lhs.get() >= rhs.get();
}

#else !__cpp_impl_three_way_comparison

template<typename T, T NullValue, typename RT, typename U>
std::strong_ordering operator <=>(const WinHandle<T, NullValue, RT>& lhs, const WinHandle<U, NullValue, RT>& rhs) noexcept
{
	return std::compare_three_way{}(lhs.get(), rhs.get());
}

#endif !__cpp_impl_three_way_comparison

#pragma endregion

#pragma region Null comparison
// Null comparison

template<typename T, T NullValue, typename RT>
bool operator ==(const WinHandle<T, NullValue, RT>& lhs, std::nullptr_t) noexcept
{
	return !lhs.valid();
}

template<typename T, T NullValue, typename RT>
bool operator ==(std::nullptr_t, const WinHandle<T, NullValue, RT>& rhs) noexcept
{
	return !rhs.valid();
}

#if !__cpp_impl_three_way_comparison

template<typename T, T NullValue, typename RT>
bool operator !=(const WinHandle<T, NullValue, RT>& lhs, std::nullptr_t) noexcept
{
	return lhs.valid();
}

template<typename T, T NullValue, typename RT>
bool operator !=(std::nullptr_t, const WinHandle<T, NullValue, RT>& rhs) noexcept
{
	return rhs.valid();
}

template<typename T, T NullValue, typename RT>
bool operator <(const WinHandle<T, NullValue, RT>& lhs, std::nullptr_t) noexcept
{
	return std::less<T>()(lhs.get(), nullptr);
}

template<typename T, T NullValue, typename RT>
bool operator <(std::nullptr_t, const WinHandle<T, NullValue, RT>& rhs) noexcept
{
	return std::less<T>()(nullptr, rhs.get());
}

template<typename T, T NullValue, typename RT>
bool operator >(const WinHandle<T, NullValue, RT>& lhs, std::nullptr_t) noexcept
{
	return nullptr > lhs.get();
}

template<typename T, T NullValue, typename RT>
bool operator >(std::nullptr_t, const WinHandle<T, NullValue, RT>& rhs) noexcept
{
	return rhs.get() > nullptr;
}

template<typename T, T NullValue, typename RT>
bool operator <=(const WinHandle<T, NullValue, RT>& lhs, std::nullptr_t) noexcept
{
	return !(nullptr < lhs.get());
}

template<typename T, T NullValue, typename RT>
bool operator <=(std::nullptr_t, const WinHandle<T, NullValue, RT>& rhs) noexcept
{
	return !(rhs.get() < nullptr);
}

template<typename T, T NullValue, typename RT>
bool operator >=(const WinHandle<T, NullValue, RT>& lhs, std::nullptr_t) noexcept
{
	return !(lhs.get() < nullptr);
}

template<typename T, T NullValue, typename RT>
bool operator >=(std::nullptr_t, const WinHandle<T, NullValue, RT>& rhs) noexcept
{
	return !(nullptr < rhs.get());
}

#else !__cpp_impl_three_way_comparison

template<typename T, T NullValue, typename RT>
bool operator <=>(const WinHandle<T, NullValue, RT>& lhs, std::nullptr_t) noexcept
{
	return std::compare_three_way{}(lhs.get(), NullValue);
}

#endif !__cpp_impl_three_way_comparison

#pragma endregion

#pragma region Handle comparison operators
// Handle comparison operators

template<typename T, T NullValue, typename RT>
bool operator ==(const T& lhs, const WinHandle<T, NullValue, RT>& rhs) noexcept
{
	return lhs == rhs.get();
}

template<typename T, T NullValue, typename RT>
bool operator ==(const WinHandle<T, NullValue, RT>& lhs, const T& rhs) noexcept
{
	return lhs.get() == rhs;
}

#if !__cpp_impl_three_way_comparison

template<typename T, T NullValue, typename RT>
bool operator !=(const T& lhs, const WinHandle<T, NullValue, RT>& rhs) noexcept
{
	return lhs != rhs.get();
}

template<typename T, T NullValue, typename RT>
bool operator !=(const WinHandle<T, NullValue, RT>& lhs, const T& rhs) noexcept
{
	return lhs.get() != rhs;
}

template<typename T, T NullValue, typename RT>
bool operator <(const T& lhs, const WinHandle<T, NullValue, RT>& rhs) noexcept
{
	return lhs < rhs.get();
}

template<typename T, T NullValue, typename RT>
bool operator <(const WinHandle<T, NullValue, RT>& lhs, const T& rhs) noexcept
{
	return lhs.get() < rhs;
}

template<typename T, T NullValue, typename RT>
bool operator <=(const T& lhs, const WinHandle<T, NullValue, RT>& rhs) noexcept
{
	return lhs <= rhs.get();
}

template<typename T, T NullValue, typename RT>
bool operator <=(const WinHandle<T, NullValue, RT>& lhs, const T& rhs) noexcept
{
	return lhs.get() <= rhs;
}

template<typename T, T NullValue, typename RT>
bool operator >(const T& lhs, const WinHandle<T, NullValue, RT>& rhs) noexcept
{
	return lhs > rhs.get();
}

template<typename T, T NullValue, typename RT>
bool operator >(const WinHandle<T, NullValue, RT>& lhs, const T& rhs) noexcept
{
	return lhs.get() > rhs;
}

template<typename T, T NullValue, typename RT>
bool operator >=(const T& lhs, const WinHandle<T, NullValue, RT>& rhs) noexcept
{
	return lhs >= rhs.get();
}

template<typename T, T NullValue, typename RT>
bool operator >=(const WinHandle<T, NullValue, RT>& lhs, const T& rhs) noexcept
{
	return lhs.get() >= rhs;
}

#else !__cpp_impl_three_way_comparison

template<typename T, T NullValue, typename RT>
std::strong_ordering operator <=>(const T& lhs, const WinHandle<T, NullValue, RT>& rhs) noexcept
{
	return std::compare_three_way{}(lhs, rhs.get());
}

template<typename T, T NullValue, typename RT>
std::strong_ordering operator <=>(const WinHandle<T, NullValue, RT>& lhs, const T& rhs) noexcept
{
	return std::compare_three_way{}(lhs.get(), rhs);
}

#endif !__cpp_impl_three_way_comparison

#pragma endregion

#pragma endregion

#pragma region WinHandle implementation
//////////////////////////////////////////////////////////////////////////
// WinHandle implementation

#pragma region Constructors
// Constructors

template<typename T, T NullValue, typename RT>
WinHandle<T, NullValue, RT>::WinHandle()
	: m_impl{ std::make_shared<impl>(NullValue, nullptr) }
{
}

template<typename T, T NullValue, typename RT>
WinHandle<T, NullValue, RT>::WinHandle(T handle, nullptr_t)
	: m_impl{ std::make_shared<impl>(handle, nullptr) }
{
}

// std::function
template<typename T, T NullValue, typename RT>
WinHandle<T, NullValue, RT>::WinHandle(std::function<RT(T)> deleter)
	: m_impl{ std::make_shared<impl>(NullValue, deleter) }
{
}

template<typename T, T NullValue, typename RT>
WinHandle<T, NullValue, RT>::WinHandle(T handle, std::function<RT(T)> deleter)
	: m_impl{ std::make_shared<impl>(handle, deleter) }
{
}

// Function pointer
template<typename T, T NullValue, typename RT>
template<typename DType>
WinHandle<T, NullValue, RT>::WinHandle(RT(__stdcall* deleter)(DType))
	: m_impl{ std::make_shared<impl>(NullValue, std::bind(deleter, std::placeholders::_1)) }
{
}

template<typename T, T NullValue, typename RT>
template<typename DType, typename... Args>
WinHandle<T, NullValue, RT>::WinHandle(RT(__stdcall* deleter)(DType, Args...), Args&&... args)
	: m_impl{ std::make_shared<impl>(NullValue, std::bind(deleter, std::placeholders::_1, std::forward<Args>(args)...)) }
{
}

template<typename T, T NullValue, typename RT>
template<typename DType>
WinHandle<T, NullValue, RT>::WinHandle(T handle, RT(__stdcall* deleter)(DType))
	: m_impl{ std::make_shared<impl>(handle, std::bind(deleter, std::placeholders::_1)) }
{
}

template<typename T, T NullValue, typename RT>
template<typename DType, typename... Args>
WinHandle<T, NullValue, RT>::WinHandle(T handle, RT(__stdcall* deleter)(DType, Args...), Args&&... args)
	: m_impl{ std::make_shared<impl>(handle, std::bind(deleter, std::placeholders::_1, std::forward<Args>(args)...)) }
{
}

// Member function pointer
template<typename T, T NullValue, typename RT>
template<typename Class, typename DType>
WinHandle<T, NullValue, RT>::WinHandle(RT(__stdcall Class::* deleter)(DType), Class* instance)
	: m_impl{ std::make_shared<impl>(NullValue, std::bind(deleter, instance, std::placeholders::_1)) }
{
}

template<typename T, T NullValue, typename RT>
template<typename Class, typename DType, typename... Args>
WinHandle<T, NullValue, RT>::WinHandle(RT(__stdcall Class::* deleter)(DType, Args...), Class* instance, Args&&... args)
	: m_impl{ std::make_shared<impl>(NullValue, std::bind(deleter, instance, std::placeholders::_1, std::forward<Args>(args)...)) }
{
}

template<typename T, T NullValue, typename RT>
template<typename Class, typename DType>
WinHandle<T, NullValue, RT>::WinHandle(T handle, RT(__stdcall Class::* deleter)(DType), Class* instance)
	: m_impl{ std::make_shared<impl>(handle, std::bind(deleter, instance, std::placeholders::_1)) }
{
}

template<typename T, T NullValue, typename RT>
template<typename Class, typename DType, typename... Args>
WinHandle<T, NullValue, RT>::WinHandle(T handle, RT(__stdcall Class::* deleter)(DType, Args...), Class* instance, Args&&... args)
	: m_impl{ std::make_shared<impl>(handle, std::bind(deleter, instance, std::placeholders::_1, std::forward<Args>(args)...)) }
{
}

#pragma endregion

#pragma region Copy and move constructors
// Copy and move constructors
template<typename T, T NullValue, typename RT>
WinHandle<T, NullValue, RT>::WinHandle(WinHandle&& move) noexcept
	: m_impl(move.m_impl)
{
	std::make_shared<impl>(NullValue, m_impl->deleter()).swap(move.m_impl);
}

#pragma endregion

#pragma region Copy and move assignment operators
// Copy and move assignment operators

template<typename T, T NullValue, typename RT>
WinHandle<T, NullValue, RT>& WinHandle<T, NullValue, RT>::operator =(WinHandle&& move) noexcept
{
	m_impl = move.m_impl;
	std::make_shared<impl>(NullValue, m_impl->deleter()).swap(move.m_impl);
	return *this;
}

#pragma endregion

#pragma region Assignment operators
// Assignment operators

template<typename T, T NullValue, typename RT>
WinHandle<T, NullValue, RT>& WinHandle<T, NullValue, RT>::operator=(const T& handle) noexcept
{
	m_impl->assign(handle);
	return *this;
}

#pragma endregion

#pragma region Conversion operators
// Conversion operators

template<typename T, T NullValue, typename RT>
WinHandle<T, NullValue, RT>::operator bool_type() const noexcept
{
	return valid() ? &WinHandle::this_type_does_not_support_comparisons : nullptr;
}

#pragma endregion

#pragma region Smart pointer operations
// Smart pointer operations

template<typename T, T NullValue, typename RT>
void WinHandle<T, NullValue, RT>::swap(WinHandle& other) noexcept
{
	m_impl.swap(other.m_impl);
}

template<typename T, T NullValue, typename RT>
void WinHandle<T, NullValue, RT>::reset()
{
	std::make_shared<impl>(NullValue, m_impl->deleter()).swap(m_impl);
}

template<typename T, T NullValue, typename RT>
void WinHandle<T, NullValue, RT>::reset(T handle)
{
	if (handle != m_impl->get())
		std::make_shared<impl>(handle, m_impl->deleter()).swap(m_impl);
}

template<typename T, T NullValue, typename RT>
long WinHandle<T, NullValue, RT>::use_count() const noexcept
{
	return m_impl.use_count();
}

#pragma endregion

#pragma region Handle operations
// Handle operations

template<typename T, T NullValue, typename RT>
bool WinHandle<T, NullValue, RT>::valid() const noexcept
{
	return m_impl->get() != NullValue;
}

template<typename T, T NullValue, typename RT>
T WinHandle<T, NullValue, RT>::get() const noexcept
{
	return m_impl->get();
}

template<typename T, T NullValue, typename RT>
const T* WinHandle<T, NullValue, RT>::ptr() const noexcept
{
	return m_impl->ptr();
}

template<typename T, T NullValue, typename RT>
typename WinHandle<T, NullValue, RT>::MutableHandle WinHandle<T, NullValue, RT>::ptr() noexcept
{
	return MutableHandle(*this);
}

template<typename T, T NullValue, typename RT>
RT WinHandle<T, NullValue, RT>::close() noexcept
{
	return m_impl->assign(NullValue);
}

#pragma endregion

#pragma endregion

#pragma region WinHandle::impl implementation
//////////////////////////////////////////////////////////////////////////
// WinHandle::impl implementation

#pragma region Constructors
// Constructors

template<typename T, T NullValue, typename RT>
WinHandle<T, NullValue, RT>::impl::impl(T handle, nullptr_t) noexcept
	: m_handle{ handle }
{
}

// std::function
template<typename T, T NullValue, typename RT>
WinHandle<T, NullValue, RT>::impl::impl(T handle, std::function<RT(T)> deleter)
	: m_handle{ handle }, m_deleter{ deleter }
{
}

#pragma endregion

#pragma region Destructor
// Destructor

template<typename T, T NullValue, typename RT>
WinHandle<T, NullValue, RT>::impl::~impl() noexcept
{
	destroy();
}

#pragma endregion

#pragma region Assignment
// Assignment

template<typename T, T NullValue, typename RT>
RT WinHandle<T, NullValue, RT>::impl::assign(T v) noexcept
{
	RT result = {};
	if (v != m_handle)
	{
		result = destroy();
		m_handle = v;
	}
	return result;
}

#pragma endregion

#pragma region Member access
// Member access

template<typename T, T NullValue, typename RT>
T WinHandle<T, NullValue, RT>::impl::get() const noexcept
{
	return m_handle;
}

template<typename T, T NullValue, typename RT>
const T* WinHandle<T, NullValue, RT>::impl::ptr() const noexcept
{
	return &m_handle;
}

template<typename T, T NullValue, typename RT>
std::function<RT(T)> WinHandle<T, NullValue, RT>::impl::deleter() const noexcept
{
	return m_deleter;
}

#pragma endregion

#pragma region Deleter
// Deleter

template<typename T, T NullValue, typename RT>
RT WinHandle<T, NullValue, RT>::impl::destroy() noexcept
{
	RT result = {};
	if (m_handle != NullValue)
	{
		if (m_deleter)
			result = m_deleter(m_handle);
		m_handle = NullValue;
	}
	return result;
}

#pragma endregion
#pragma endregion

#pragma region MutableHandle implementation

//////////////////////////////////////////////////////////////////////////
// MutableHandle implementation

#pragma region Constructor
// Constructor
template<typename T, T NullValue, typename RT>
WinHandle<T, NullValue, RT>::MutableHandle::MutableHandle(WinHandle& owner) noexcept
	: m_owner(owner), m_handle(owner.get())
{
}

#pragma endregion

#pragma region Destructor
// Destructor
template<typename T, T NullValue, typename RT>
WinHandle<T, NullValue, RT>::MutableHandle::~MutableHandle() noexcept
{
	m_owner.reset(m_handle);
	m_handle = NullValue;
}

#pragma endregion

#pragma region Conversion operator
// Conversion operator
template<typename T, T NullValue, typename RT>
WinHandle<T, NullValue, RT>::MutableHandle::operator T*() noexcept
{
	return &m_handle;
}

#pragma endregion

#pragma endregion
