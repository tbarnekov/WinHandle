#include "pch.h"
#include "CppUnitTest.h"
#include "MockDeleter.h"
#include <WinHandle.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Assignment
{
	TEST_CLASS(Assignment)
	{
	public:
		inline static const HANDLE Handle1 = reinterpret_cast<HANDLE>(1234);
		inline static const HANDLE Handle2 = reinterpret_cast<HANDLE>(4321);

		using handle_type = std::remove_cv_t<decltype(Handle1)>;

		TEST_METHOD(NonOwningAssignment)
		{
			WinHandle<handle_type> handle;
			Assert::IsFalse(handle.valid());

			handle = Handle1;

			Assert::IsTrue(handle.valid());
			Assert::AreEqual(Handle1, handle.get());
		}

		TEST_METHOD(NonReplacingAssignment)
		{
			MockDeleter<handle_type> deleter{ std::vector<handle_type>{ Handle1 } };
			WinHandle<handle_type, INVALID_HANDLE_VALUE> handle{ &MockDeleter<handle_type>::Delete, &deleter };

			Assert::IsFalse(handle.valid());

			handle = Handle1;

			Assert::IsTrue(handle.valid());
			Assert::AreEqual(Handle1, handle.get());
		}

		TEST_METHOD(ReplacingAssignment)
		{
			MockDeleter<handle_type> deleter{ std::vector<handle_type>{ Handle1, Handle2 } };
			WinHandle<handle_type, INVALID_HANDLE_VALUE> handle{ Handle1, &MockDeleter<handle_type>::Delete, &deleter };

			handle = Handle2;
		}

		TEST_METHOD(PointerReplacingAssignment)
		{
			MockDeleter<handle_type> deleter{ std::vector<handle_type>{ Handle1, Handle2 } };
			WinHandle<handle_type, INVALID_HANDLE_VALUE> handle{ Handle1, &MockDeleter<handle_type>::Delete, &deleter };

			TestPointerAssignment(handle.ptr(), Handle2);
		}

		TEST_METHOD(CopyAssignment1)
		{
			MockDeleter<handle_type> deleter1{ std::vector<handle_type>{ Handle1 } };
			MockDeleter<handle_type> deleter2{ std::vector<handle_type>{ Handle2 } };
			{
				WinHandle<handle_type, INVALID_HANDLE_VALUE> handle1{ Handle1, &MockDeleter<handle_type>::Delete, &deleter1 };
				WinHandle<handle_type, INVALID_HANDLE_VALUE> handle2{ Handle2, &MockDeleter<handle_type>::Delete, &deleter2 };
				handle1 = handle2;
			}
		}

		TEST_METHOD(CopyAssignment2)
		{
			MockDeleter<handle_type> deleter1{ std::vector<handle_type>{ Handle1 } };
			MockDeleter<handle_type> deleter2{ std::vector<handle_type>{ Handle2 } };
			{
				WinHandle<handle_type, INVALID_HANDLE_VALUE> handle1{ Handle1, &MockDeleter<handle_type>::Delete, &deleter1 };
				WinHandle<handle_type, INVALID_HANDLE_VALUE> handle2{ Handle2, &MockDeleter<handle_type>::Delete, &deleter2 };
				WinHandle<handle_type, INVALID_HANDLE_VALUE> h3;
				handle1 = handle2;

				Assert::AreEqual(handle2.get(), handle1.get());

				handle1 = h3;

				Assert::AreEqual(h3.get(), handle1.get());
			}
		}

		TEST_METHOD(MoveAssignment1)
		{
			MockDeleter<handle_type> deleter{ std::vector<handle_type>{ Handle1 } };

			WinHandle<handle_type> h1{ Handle1, &MockDeleter<handle_type>::Delete, &deleter };
			WinHandle<handle_type> h2 = h1;

			Assert::IsTrue(h1.valid());
			Assert::IsTrue(h2.valid());
			Assert::AreEqual(Handle1, h1.get());
			Assert::AreEqual(Handle1, h2.get());
		}

	private:
		handle_type TestPointerAssignment(handle_type* h, handle_type new_value)
		{
			*h = new_value;
			return *h;
		}
	};
}
