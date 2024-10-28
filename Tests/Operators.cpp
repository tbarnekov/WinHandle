#include "pch.h"
#include "CppUnitTest.h"
#include <WinHandle.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Operators
{
	TEST_CLASS(Operators)
	{
	private:
		inline static const HANDLE Handle1 = reinterpret_cast<HANDLE>(1234);
		inline static const HANDLE Handle2 = reinterpret_cast<HANDLE>(1234);
		inline static const HANDLE NullHandle = reinterpret_cast<HANDLE>(-1);

		using handle_type = std::remove_cv_t<decltype(Handle1)>;

		TEST_METHOD(OperatorBool)
		{
			WinHandle<handle_type, NullHandle> h1{ NullHandle, nullptr };
			if (h1)
				Assert::Fail();

			if (!!h1)
				Assert::Fail();
		}

		TEST_METHOD(OperatorNot)
		{
			WinHandle<handle_type, NullHandle> h1{ NullHandle, nullptr };

			// Check if null
			Assert::IsTrue(!h1);
		}

		TEST_METHOD(BoolAssignment)
		{
			WinHandle<handle_type, NullHandle> h1{ NullHandle, nullptr };
			bool valid = h1;
			Assert::IsFalse(valid);
		}

		bool IsValid(const WinHandle<handle_type, NullHandle>& h)
		{
			return h;
		}

		TEST_METHOD(ReturnBool)
		{
			WinHandle<handle_type, NullHandle> h1{ NullHandle, nullptr };
			bool valid = IsValid(h1);
			Assert::IsFalse(valid);
		}

		WinHandle<handle_type, NullHandle> getWinHandle(const WinHandle<handle_type, NullHandle>& h)
		{
			return h;
		}

		TEST_METHOD(ReturnWinHandle)
		{
			WinHandle<handle_type, NullHandle> h1{ Handle1, nullptr };
			WinHandle<handle_type, NullHandle> h2 = getWinHandle(h1);

			Assert::AreEqual(2l, h1.use_count());
		}

		TEST_METHOD(OperatorEqual)
		{
			WinHandle<handle_type, NullHandle> h1{ NullHandle, nullptr };
			WinHandle< handle_type, NullHandle> h2 = h1;
			WinHandle<handle_type, NullHandle> h3{ NullHandle, nullptr };

			// Check if equal
			Assert::IsTrue(h2 == h1);
			Assert::IsTrue(h1 == h2);

			Assert::IsFalse(h3 == h1);
			Assert::IsFalse(h1 == h3);
		}

		TEST_METHOD(OperatorNotEqual)
		{
			WinHandle<handle_type, NullHandle> h1{ NullHandle, nullptr };
			WinHandle<handle_type, NullHandle> h2{ NullHandle, nullptr };

			// Check if equal
			Assert::IsTrue(h2 != h1);
			Assert::IsTrue(h1 != h2);
		}

		TEST_METHOD(OperatorHandleEqual)
		{
			WinHandle<handle_type, NullHandle> h1{ NullHandle, nullptr };
			// Check if equal
			Assert::IsTrue(NullHandle == h1);
			Assert::IsTrue(h1 == NullHandle);
		}

		TEST_METHOD(OperatorHandleNotEqual)
		{
			WinHandle<handle_type, NullHandle> h1{ NullHandle, nullptr };
		
			// Check if not equal
			Assert::IsFalse(NullHandle != h1);
			Assert::IsFalse(h1 != NullHandle);
		}

		TEST_METHOD(OperatorLess)
		{
			WinHandle<handle_type, NullHandle> h1{ NullHandle, nullptr };

			// Check if less
			Assert::IsFalse(NullHandle < h1);
			Assert::IsFalse(h1 < NullHandle);
		}

		TEST_METHOD(OperatorGreater)
		{
			WinHandle<handle_type, NullHandle> h1{ NullHandle, nullptr };

			// Check if less
			Assert::IsFalse(NullHandle > h1);
			Assert::IsFalse(h1 > NullHandle);
		}

		// TODO - See if we can prevent user from assigning output of non-const ptr() to a T*

	};
}
