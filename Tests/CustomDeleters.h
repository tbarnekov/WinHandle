#pragma once
#include "pch.h"
#include "CppUnitTest.h"
#include <functional>
#include <stack>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


template<typename T, typename RT = BOOL>
class CustomDeleter
{
public:
	CustomDeleter() = default;
	explicit CustomDeleter(std::function<RT(T)> deleter) : m_deleter(deleter) {}
	~CustomDeleter() = default;

	template<RT RV = static_cast<RT>(1)>
	RT __stdcall Delete(T h)
	{
		Assert::IsFalse(m_called, TEXT("Deleter has already been called"));

		m_called = true;

		if (m_deleter)
			return m_deleter(h);

		return RV;
	}


	bool called() const
	{
		return m_called;
	}

private:
	bool m_called = false;
	std::function<RT(T)> m_deleter;
};
