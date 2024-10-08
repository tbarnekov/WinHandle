#include "pch.h"
#include "CppUnitTest.h"
#include "MockDeleter.h"
#include <WinHandle.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace SmartPointer
{
	TEST_CLASS(Operations)
	{
	private:
		HANDLE Handle1 = reinterpret_cast<HANDLE>(1234);
		HANDLE Handle2 = reinterpret_cast<HANDLE>(4321);

		using handle_type = std::remove_cv_t<decltype(Handle1)>;

	public:
		TEST_METHOD(Swap)
		{
			MockDeleter<handle_type> deleter1{ std::vector<handle_type>{ Handle1 } };
			MockDeleter<handle_type> deleter2{ std::vector<handle_type>{ Handle2 } };

			WinHandle<handle_type> h1{ Handle1, &MockDeleter<handle_type>::Delete, &deleter1 };
			WinHandle<handle_type> h2{ Handle2, &MockDeleter<handle_type>::Delete, &deleter2 };

			Assert::IsTrue(h1.valid());
			Assert::IsTrue(h2.valid());

			h1.swap(h2);

			Assert::AreEqual(Handle2, h1.get());
			Assert::AreEqual(Handle1, h2.get());
		}

		TEST_METHOD(Reset1)
		{
			MockDeleter<handle_type> deleter{ std::vector<handle_type>{ Handle1 } };
			WinHandle<handle_type> h1{ Handle1, &MockDeleter<handle_type>::Delete, &deleter };

			Assert::IsTrue(h1.valid());

			h1.reset();

			Assert::IsFalse(h1.valid());
			Assert::AreEqual(static_cast<size_t>(1), deleter.called());
		}

		TEST_METHOD(Reset2)
		{
			MockDeleter<handle_type> deleter{ std::vector<handle_type>{ Handle1, Handle2 } };
			WinHandle<handle_type> h1{ Handle1, &MockDeleter<handle_type>::Delete, &deleter };

			Assert::AreEqual(Handle1, h1.get());

			h1.reset(Handle2);

			Assert::AreEqual(Handle2, h1.get());
			Assert::AreEqual(static_cast<size_t>(1), deleter.called());
		}

		TEST_METHOD(UseCount)
		{
			WinHandle<handle_type> h1{ Handle1, nullptr };

			Assert::AreEqual(static_cast<long>(1), h1.use_count());

			WinHandle<handle_type> h2{ h1 };

			Assert::AreEqual(static_cast<long>(2), h1.use_count());
		}
	};
}
