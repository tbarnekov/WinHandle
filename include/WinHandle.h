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


template<typename Type, Type NullValue = static_cast<Type>(0), typename ReturnType = int>
class WinHandle
{
public:
	class MutableHandle;

	// Constructors
	WinHandle();
	explicit WinHandle(Type handle, nullptr_t);

	// std::function
	explicit WinHandle(std::function<ReturnType(Type)> deleter);
	explicit WinHandle(Type handle, std::function<ReturnType(Type)> deleter);

	// Function pointer
	template<typename DType>
	explicit WinHandle(ReturnType(__stdcall* deleter)(DType));

	template<typename DType, typename... Args>
	explicit WinHandle(ReturnType(__stdcall* deleter)(DType, Args...), Args&&... args);

	template<typename DType>
	explicit WinHandle(Type handle, ReturnType(__stdcall* deleter)(DType));

	template<typename DType, typename... Args>
	explicit WinHandle(Type handle, ReturnType(__stdcall* deleter)(DType, Args...), Args&&... args);

	// Member function pointer
	template<typename Class, typename DType>
	explicit WinHandle(ReturnType(__stdcall Class::* deleter)(DType), Class* instance);

	template<typename Class, typename DType, typename... Args>
	explicit WinHandle(ReturnType(__stdcall Class::* deleter)(DType, Args...), Class* instance, Args&&... args);

	template<typename Class, typename DType>
	explicit WinHandle(Type handle, ReturnType(__stdcall Class::* deleter)(DType), Class* instance);

	template<typename Class, typename DType, typename... Args>
	explicit WinHandle(Type handle, ReturnType(__stdcall Class::* deleter)(DType, Args...), Class* instance, Args&&... args);

	// Copy and move constructors
	WinHandle(const WinHandle&) noexcept = default;
	WinHandle(WinHandle&& move) noexcept;

	// Copy and move operators
	WinHandle& operator =(const WinHandle&) noexcept = default;
	WinHandle& operator =(WinHandle&& move) noexcept;

	// Destructor
	~WinHandle() noexcept = default;

	// Comparison operators
	bool operator==(const WinHandle& other) const noexcept;
#if __cpp_impl_three_way_comparison
	std::strong_ordering operator<=>(const WinHandle& other) noexcept;
#else
	bool operator!() const noexcept;
	bool operator <(const WinHandle& other) const noexcept;
#endif __cpp_impl_three_way_comparison

	bool operator==(const Type& handle) const noexcept;
	bool operator!=(const Type& handle) const noexcept;
	bool operator <(const Type& handle) const noexcept;
	bool operator >(const Type& handle) const noexcept;

	// Assignment operators
	WinHandle& operator=(Type handle) noexcept;

	// Conversion operators

	// Safe Bool Idiom
	using bool_type = void (WinHandle::*)() const noexcept;
	operator bool_type() const noexcept;

	// Smart pointer operations
	void swap(WinHandle& other) noexcept;
	void reset();
	void reset(Type handle);
	long use_count() const noexcept;

	// Handle operations
	bool valid() const noexcept;
	Type get() const noexcept;
	const Type* ptr() const noexcept; // Returns a non-mutable pointer to the handle
	[[nodiscard]] MutableHandle ptr() noexcept; // Returns a mutable pointer to the handle
	ReturnType close() noexcept; // Close the handle using the assigned deleter

private:
	// Safe Bool Idiom
	void this_type_does_not_support_comparisons() const noexcept {}

	class impl
	{
	public:
		// Constructors
		explicit impl(Type handle, nullptr_t) noexcept;

		// std::function
		explicit impl(Type handle, std::function<ReturnType(Type)> deleter);

		// Copy and move
		impl(const impl&) = delete;
		impl(impl&&) = default;
		impl& operator=(const impl&) = delete;
		impl& operator=(impl&&) = default;

		// Destructor
		~impl() noexcept;

		// Assignment
		ReturnType assign(Type v) noexcept;

		// Member access
		Type get() const noexcept;
		const Type* ptr() const noexcept;
		std::function<ReturnType(Type)> deleter() const noexcept;

	private:
		ReturnType destroy() noexcept;

		Type m_handle{ NullValue };
		std::function<ReturnType(Type)> m_deleter{ nullptr };
	};

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
		operator Type* () noexcept;

	private:
		WinHandle& m_owner;
		Type m_handle{ NullValue };
	};

	std::shared_ptr<impl> m_impl;
};

template<typename Type, Type NullValue, typename ReturnType>
bool operator==(const Type& lhs, const WinHandle<Type, NullValue, ReturnType>& rhs)
{
	return lhs == rhs.get();
}

template<typename Type, Type NullValue, typename ReturnType>
bool operator!=(const Type& lhs, const WinHandle<Type, NullValue, ReturnType>& rhs)
{
	return lhs != rhs.get();
}

template<typename Type, Type NullValue, typename ReturnType>
bool operator <(const Type& lhs, const WinHandle<Type, NullValue, ReturnType>& rhs)
{
	return lhs < rhs.get();
}

template<typename Type, Type NullValue, typename ReturnType>
bool operator >(const Type& lhs, const WinHandle<Type, NullValue, ReturnType>& rhs)
{
	return lhs > rhs.get();
}


//////////////////////////////////////////////////////////////////////////
// WinHandle implementation

// Constructors

template<typename Type, Type NullValue, typename ReturnType>
WinHandle<Type, NullValue, ReturnType>::WinHandle()
	: m_impl{ std::make_shared<impl>(NullValue, nullptr) }
{
}

template<typename Type, Type NullValue, typename ReturnType>
WinHandle<Type, NullValue, ReturnType>::WinHandle(Type handle, nullptr_t)
	: m_impl{ std::make_shared<impl>(handle, nullptr) }
{
}

// std::function
template<typename Type, Type NullValue, typename ReturnType>
WinHandle<Type, NullValue, ReturnType>::WinHandle(std::function<ReturnType(Type)> deleter)
	: m_impl{ std::make_shared<impl>(NullValue, deleter) }
{
}

template<typename Type, Type NullValue, typename ReturnType>
WinHandle<Type, NullValue, ReturnType>::WinHandle(Type handle, std::function<ReturnType(Type)> deleter)
	: m_impl{ std::make_shared<impl>(handle, deleter) }
{
}

// Function pointer
template<typename Type, Type NullValue, typename ReturnType>
template<typename DType>
WinHandle<Type, NullValue, ReturnType>::WinHandle(ReturnType(__stdcall* deleter)(DType))
	: m_impl{ std::make_shared<impl>(NullValue, std::bind(deleter, std::placeholders::_1)) }
{
}

template<typename Type, Type NullValue, typename ReturnType>
template<typename DType, typename... Args>
WinHandle<Type, NullValue, ReturnType>::WinHandle(ReturnType(__stdcall* deleter)(DType, Args...), Args&&... args)
	: m_impl{ std::make_shared<impl>(NullValue, std::bind(deleter, std::placeholders::_1, std::forward<Args>(args)...)) }
{
}

template<typename Type, Type NullValue, typename ReturnType>
template<typename DType>
WinHandle<Type, NullValue, ReturnType>::WinHandle(Type handle, ReturnType(__stdcall* deleter)(DType))
	: m_impl{ std::make_shared<impl>(handle, std::bind(deleter, std::placeholders::_1)) }
{
}

template<typename Type, Type NullValue, typename ReturnType>
template<typename DType, typename... Args>
WinHandle<Type, NullValue, ReturnType>::WinHandle(Type handle, ReturnType(__stdcall* deleter)(DType, Args...), Args&&... args)
	: m_impl{ std::make_shared<impl>(handle, std::bind(deleter, std::placeholders::_1, std::forward<Args>(args)...)) }
{
}

// Member function pointer
template<typename Type, Type NullValue, typename ReturnType>
template<typename Class, typename DType>
WinHandle<Type, NullValue, ReturnType>::WinHandle(ReturnType(__stdcall Class::* deleter)(DType), Class* instance)
	: m_impl{ std::make_shared<impl>(NullValue, std::bind(deleter, instance, std::placeholders::_1)) }
{
}

template<typename Type, Type NullValue, typename ReturnType>
template<typename Class, typename DType, typename... Args>
WinHandle<Type, NullValue, ReturnType>::WinHandle(ReturnType(__stdcall Class::* deleter)(DType, Args...), Class* instance, Args&&... args)
	: m_impl{ std::make_shared<impl>(NullValue, std::bind(deleter, instance, std::placeholders::_1, std::forward<Args>(args)...)) }
{
}

template<typename Type, Type NullValue, typename ReturnType>
template<typename Class, typename DType>
WinHandle<Type, NullValue, ReturnType>::WinHandle(Type handle, ReturnType(__stdcall Class::* deleter)(DType), Class* instance)
	: m_impl{ std::make_shared<impl>(handle, std::bind(deleter, instance, std::placeholders::_1)) }
{
}

template<typename Type, Type NullValue, typename ReturnType>
template<typename Class, typename DType, typename... Args>
WinHandle<Type, NullValue, ReturnType>::WinHandle(Type handle, ReturnType(__stdcall Class::* deleter)(DType, Args...), Class* instance, Args&&... args)
	: m_impl{ std::make_shared<impl>(handle, std::bind(deleter, instance, std::placeholders::_1, std::forward<Args>(args)...)) }
{
}

// Copy and move constructors

template<typename Type, Type NullValue, typename ReturnType>
WinHandle<Type, NullValue, ReturnType>::WinHandle(WinHandle&& move) noexcept
	: m_impl(move.m_impl)
{
	std::make_shared<impl>(NullValue, m_impl->deleter()).swap(move.m_impl);
}

// Copy and move assignment operators

template<typename Type, Type NullValue, typename ReturnType>
WinHandle<Type, NullValue, ReturnType>& WinHandle<Type, NullValue, ReturnType>::operator =(WinHandle&& move) noexcept
{
	m_impl = move.m_impl;
	std::make_shared<impl>(NullValue, m_impl->deleter()).swap(move.m_impl);
	return *this;
}

// Comparison operators

template<typename Type, Type NullValue, typename ReturnType>
bool WinHandle<Type, NullValue, ReturnType>::operator==(const WinHandle& other) const noexcept
{
	return m_impl == other.m_impl;
}

#if __cpp_impl_three_way_comparison

template<typename Type, Type NullValue, typename ReturnType>
std::strong_ordering WinHandle<Type, NullValue, ReturnType>::operator<=>(const WinHandle& other) noexcept
{
	return m_impl <=> other.m_impl;
}

#else

template<typename Type, Type NullValue, typename ReturnType>
bool WinHandle<Type, NullValue, ReturnType>::operator!() const noexcept
{
	return !valid();
}

template<typename Type, Type NullValue, typename ReturnType>
bool WinHandle<Type, NullValue, ReturnType>::operator <(const WinHandle& other) const noexcept
{
	return m_impl < other.m_impl;
}

#endif __cpp_impl_three_way_comparison

template<typename Type, Type NullValue, typename ReturnType>
bool WinHandle<Type, NullValue, ReturnType>::operator==(const Type& handle) const noexcept
{
	return get() == handle;
}

template<typename Type, Type NullValue, typename ReturnType>
bool WinHandle<Type, NullValue, ReturnType>::operator!=(const Type& handle) const noexcept
{
	return get() != handle;
}

template<typename Type, Type NullValue, typename ReturnType>
bool WinHandle<Type, NullValue, ReturnType>::operator<(const Type& handle) const noexcept
{
	return get() < handle;
}

template<typename Type, Type NullValue, typename ReturnType>
bool WinHandle<Type, NullValue, ReturnType>::operator>(const Type& handle) const noexcept
{
	return get() > handle;
}

// Assignment operators

template<typename Type, Type NullValue, typename ReturnType>
WinHandle<Type, NullValue, ReturnType>& WinHandle<Type, NullValue, ReturnType>::operator=(Type handle) noexcept
{
	m_impl->assign(handle);
	return *this;
}

// Conversion operators

template<typename Type, Type NullValue, typename ReturnType>
WinHandle<Type, NullValue, ReturnType>::operator bool_type() const noexcept
{
	return valid() ? &WinHandle::this_type_does_not_support_comparisons : nullptr;
}

// Smart pointer operations

template<typename Type, Type NullValue, typename ReturnType>
void WinHandle<Type, NullValue, ReturnType>::swap(WinHandle& other) noexcept
{
	m_impl.swap(other.m_impl);
}

template<typename Type, Type NullValue, typename ReturnType>
void WinHandle<Type, NullValue, ReturnType>::reset()
{
	std::make_shared<impl>(NullValue, m_impl->deleter()).swap(m_impl);
}

template<typename Type, Type NullValue, typename ReturnType>
void WinHandle<Type, NullValue, ReturnType>::reset(Type handle)
{
	if (handle != m_impl->get())
		std::make_shared<impl>(handle, m_impl->deleter()).swap(m_impl);
}

template<typename Type, Type NullValue, typename ReturnType>
long WinHandle<Type, NullValue, ReturnType>::use_count() const noexcept
{
	return m_impl.use_count();
}

// Handle operations

template<typename Type, Type NullValue, typename ReturnType>
bool WinHandle<Type, NullValue, ReturnType>::valid() const noexcept
{
	return m_impl->get() != NullValue;
}

template<typename Type, Type NullValue, typename ReturnType>
Type WinHandle<Type, NullValue, ReturnType>::get() const noexcept
{
	return m_impl->get();
}

template<typename Type, Type NullValue, typename ReturnType>
const Type* WinHandle<Type, NullValue, ReturnType>::ptr() const noexcept
{
	return m_impl->ptr();
}

template<typename Type, Type NullValue, typename ReturnType>
typename WinHandle<Type, NullValue, ReturnType>::MutableHandle WinHandle<Type, NullValue, ReturnType>::ptr() noexcept
{
	return MutableHandle(*this);
}

template<typename Type, Type NullValue, typename ReturnType>
ReturnType WinHandle<Type, NullValue, ReturnType>::close() noexcept
{
	return m_impl->assign(NullValue);
}


//////////////////////////////////////////////////////////////////////////
// WinHandle::impl implementation

// Constructors

template<typename Type, Type NullValue, typename ReturnType>
WinHandle<Type, NullValue, ReturnType>::impl::impl(Type handle, nullptr_t) noexcept
	: m_handle{ handle }
{
}

// std::function
template<typename Type, Type NullValue, typename ReturnType>
WinHandle<Type, NullValue, ReturnType>::impl::impl(Type handle, std::function<ReturnType(Type)> deleter)
	: m_handle{ handle }, m_deleter{ deleter }
{
}

// Destructor

template<typename Type, Type NullValue, typename ReturnType>
WinHandle<Type, NullValue, ReturnType>::impl::~impl() noexcept
{
	destroy();
}

// Assignment

template<typename Type, Type NullValue, typename ReturnType>
ReturnType WinHandle<Type, NullValue, ReturnType>::impl::assign(Type v) noexcept
{
	ReturnType result = {};
	if (v != m_handle)
	{
		result = destroy();
		m_handle = v;
	}
	return result;
}

// Member access

template<typename Type, Type NullValue, typename ReturnType>
Type WinHandle<Type, NullValue, ReturnType>::impl::get() const noexcept
{
	return m_handle;
}

template<typename Type, Type NullValue, typename ReturnType>
const Type* WinHandle<Type, NullValue, ReturnType>::impl::ptr() const noexcept
{
	return &m_handle;
}

template<typename Type, Type NullValue, typename ReturnType>
std::function<ReturnType(Type)> WinHandle<Type, NullValue, ReturnType>::impl::deleter() const noexcept
{
	return m_deleter;
}

// Deleter

template<typename Type, Type NullValue, typename ReturnType>
ReturnType WinHandle<Type, NullValue, ReturnType>::impl::destroy() noexcept
{
	ReturnType result = {};
	if (m_handle != NullValue)
	{
		if (m_deleter)
			result = m_deleter(m_handle);
		m_handle = NullValue;
	}
	return result;
}


//////////////////////////////////////////////////////////////////////////
// MutableHandle implementation

// Constructor

template<typename Type, Type NullValue, typename ReturnType>
WinHandle<Type, NullValue, ReturnType>::MutableHandle::MutableHandle(WinHandle& owner) noexcept
	: m_owner(owner), m_handle(owner.get())
{
}

// Destructor
template<typename Type, Type NullValue, typename ReturnType>
WinHandle<Type, NullValue, ReturnType>::MutableHandle::~MutableHandle() noexcept
{
	m_owner.reset(m_handle);
	m_handle = NullValue;
}

// Conversion operator
template<typename Type, Type NullValue, typename ReturnType>
WinHandle<Type, NullValue, ReturnType>::MutableHandle::operator Type* () noexcept
{
	return &m_handle;
}
