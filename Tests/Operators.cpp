#include "pch.h"
#include "CppUnitTest.h"
#include <WinHandle.h>
#include <map>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Operators
{
	TEST_CLASS(Operators)
	{
	private:
		inline static const HANDLE Handle1 = reinterpret_cast<HANDLE>(1234);
		inline static const HANDLE Handle2 = reinterpret_cast<HANDLE>(1234);
		inline static const HANDLE NullHandle = INVALID_HANDLE_VALUE;

		using handle_type = std::remove_cv_t<decltype(Handle1)>;
		using winhandle_type = WinHandle<handle_type, INVALID_HANDLE_VALUE>;

		TEST_METHOD(OperatorBool)
		{
			winhandle_type h1{ NullHandle, nullptr };
			if (h1)
				Assert::Fail();

			if (!!h1)
				Assert::Fail();
		}

		TEST_METHOD(OperatorNot)
		{
			winhandle_type h1{ NullHandle, nullptr };

			// Check if null
			Assert::IsTrue(!h1);
		}

		TEST_METHOD(BoolAssignment)
		{
			winhandle_type h1{ NullHandle, nullptr };
			bool valid = h1;
			Assert::IsFalse(valid);
		}

		bool IsValid(const winhandle_type& h)
		{
			return h;
		}

		TEST_METHOD(ReturnBool)
		{
			winhandle_type h1{ NullHandle, nullptr };
			bool valid = IsValid(h1);
			Assert::IsFalse(valid);
		}

		winhandle_type getWinHandle(const winhandle_type& h)
		{
			return h;
		}

		TEST_METHOD(ReturnWinHandle)
		{
			winhandle_type h1{ Handle1, nullptr };
			winhandle_type h2 = getWinHandle(h1);

			Assert::AreEqual(2l, h1.use_count());
		}

		TEST_METHOD(OperatorEqual)
		{
			winhandle_type h1{ NullHandle, nullptr };
			winhandle_type h2 = h1;
			winhandle_type h3{ Handle1, nullptr };

			// Check if equal
			Assert::IsTrue(h2 == h1);
			Assert::IsTrue(h1 == h2);

			Assert::IsFalse(h3 == h1);
			Assert::IsFalse(h1 == h3);
		}

		TEST_METHOD(OperatorNotEqual)
		{
			winhandle_type h1{ NullHandle, nullptr };
			winhandle_type h2{ Handle1, nullptr };

			// Check if not equal
			Assert::IsTrue(h2 != h1);
			Assert::IsTrue(h1 != h2);
		}

		TEST_METHOD(OperatorLess)
		{
			winhandle_type h1{ NullHandle, nullptr };
			winhandle_type h2{ Handle1, nullptr };

			// Check if less
			Assert::IsFalse(h1 < h2);
			Assert::IsTrue(h2 < h1);
		}

		TEST_METHOD(OperatorLess2)
		{
			std::map<winhandle_type, bool> test;
			winhandle_type h1{ Handle1, nullptr };
			winhandle_type h2{ Handle2, nullptr };

			test.insert(std::make_pair(h1, true));
			test.insert(std::make_pair(h2, false));
		}

		TEST_METHOD(OperatorGreater)
		{
			winhandle_type h1{ NullHandle, nullptr };
			winhandle_type h2{ Handle1, nullptr };

			// Check if greater
			Assert::IsTrue(h1 > h2);
			Assert::IsFalse(h2 > h1);
		}

		TEST_METHOD(OperatorHandleEqual)
		{
			winhandle_type h1{ NullHandle, nullptr };

			// Check if equal
			Assert::IsTrue(NullHandle == h1);
			Assert::IsTrue(h1 == NullHandle);
		}

		TEST_METHOD(OperatorHandleNotEqual)
		{
			winhandle_type h1{ NullHandle, nullptr };
			winhandle_type h2{ Handle1, nullptr };
		
			// Check if not equal
			Assert::IsFalse(NullHandle != h1);
			Assert::IsFalse(h1 != NullHandle);

			Assert::IsTrue(NullHandle != h2);
			Assert::IsTrue(h2 != NullHandle);
		}

		TEST_METHOD(OperatorHandleLess)
		{
			winhandle_type h1{ NullHandle, nullptr };

			// Check if less
			Assert::IsFalse(NullHandle < h1);
			Assert::IsFalse(h1 < NullHandle);
		}

		TEST_METHOD(OperatorHandleGreater)
		{
			winhandle_type h1{ NullHandle, nullptr };

			// Check if less
			Assert::IsFalse(NullHandle > h1);
			Assert::IsFalse(h1 > NullHandle);
		}

		TEST_METHOD(OperatorNullEqual)
		{
			winhandle_type h1{ NullHandle, nullptr };
			winhandle_type h2{ Handle1, nullptr };
			winhandle_type h3{ static_cast<handle_type>(0), nullptr };

			Assert::IsTrue(h1 == nullptr);
			Assert::IsTrue(nullptr == h1);

			Assert::IsFalse(h2 == nullptr);
			Assert::IsFalse(nullptr == h2);

			Assert::IsFalse(h3 == nullptr);
			Assert::IsFalse(nullptr == h3);
		}

		TEST_METHOD(IfCreate)
		{
			if (winhandle_type h1{ NullHandle, nullptr })
			{
				Assert::Fail();
			}

			if (winhandle_type h2{ Handle1, nullptr })
			{
				Assert::IsTrue(h2.valid());
			}
		}

		TEST_METHOD(IfAssign)
		{
			winhandle_type h1{ NullHandle, nullptr };
			if (winhandle_type h = h1)
			{
				Assert::Fail();
			}


			winhandle_type h2{ Handle1, nullptr };
			if (winhandle_type h = h2)
			{
				Assert::IsTrue(h.valid());
			}
		}
	};
}
