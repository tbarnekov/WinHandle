#include "pch.h"
#include "CppUnitTest.h"
#include "MockDeleter.h"
#include <WinHandle.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Ownership
{
	TEST_CLASS(Ownership)
	{
	public:
		inline static const HANDLE Handle1 = reinterpret_cast<HANDLE>(1234);
		inline static const HANDLE Handle2 = reinterpret_cast<HANDLE>(4321);

		using handle_type = std::remove_cv_t<decltype(Handle1)>;

		TEST_METHOD(DefaultContructor)
		{
			WinHandle<handle_type> handle;

			Assert::IsFalse(handle.valid());
		}

		TEST_METHOD(ContructorNonOwning1)
		{
			WinHandle<handle_type> handle{ 0, nullptr };

			Assert::IsFalse(handle.valid());
		}

		TEST_METHOD(ContructorNonOwning2)
		{
			WinHandle<handle_type> handle{ Handle1, nullptr };

			Assert::IsTrue(handle.valid());
		}

		TEST_METHOD(ConstructorOwning1)
		{
			MockDeleter<handle_type> deleter{ false };
			WinHandle<handle_type> handle{ &MockDeleter<handle_type>::Delete, &deleter };

			Assert::IsFalse(handle.valid());
		}

		TEST_METHOD(ConstructorOwning2)
		{
			MockDeleter<handle_type> deleter{ false };
			WinHandle<handle_type> handle{ 0, &MockDeleter<handle_type>::Delete, &deleter };

			Assert::IsFalse(handle.valid());
		}

		TEST_METHOD(ConstructorOwning3)
		{
			MockDeleter<handle_type> deleter{ std::vector<handle_type>{ Handle1 } };
			WinHandle<handle_type> handle{ Handle1, &MockDeleter<handle_type>::Delete, &deleter };

			Assert::IsTrue(handle.valid());
		}
	};
}
