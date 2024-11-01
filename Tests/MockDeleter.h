#pragma once
#include "pch.h"
#include "CppUnitTest.h"
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

template<typename T, typename RT = BOOL>
class MockDeleter
{
public:
	MockDeleter() = default;
	explicit MockDeleter(bool assert)
		: m_assert{ assert }
	{
	}

	explicit MockDeleter(const std::vector<T>& expect)
		: m_expect{ expect }
	{
	}

	MockDeleter(const std::vector<T>& expect, bool assert)
		: m_expect{ expect }, m_assert{ assert }
	{
	}

	~MockDeleter()
	{
		if (m_assert)
		{
			if (!m_expect.empty())
				Assert::IsTrue(m_index == m_expect.size());
			else
				Assert::IsTrue(m_index > 0);
		}
	}

	RT __stdcall Delete(T h)
	{
		// Fail if deleter is called twice
		Assert::IsFalse(m_index >= m_expect.size() );

		// Fail if handle doesn't meet expectation
		if (!m_expect.empty())
			Assert::AreEqual(m_expect.at(m_index), h);

		++m_index;

		return {};
	}

	size_t called() const
	{
		return m_index;
	}

private:
	bool m_assert{ true }; // Assert if not called unless specifically disabled
	size_t m_index{ 0 };
	std::vector<T> m_expect; // Vector of expected values
};
