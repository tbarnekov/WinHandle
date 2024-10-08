#include "pch.h"
#include "CppUnitTest.h"
#include "MockDeleter.h"
#include <WinHandle.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Handle
{
	TEST_CLASS(Operations)
	{
	private:
		inline static const HANDLE Handle1 = reinterpret_cast<HANDLE>(1234);
		inline static const HANDLE Handle2 = reinterpret_cast<HANDLE>(1234);

		using handle_type = std::remove_cv_t<decltype(Handle1)>;

	public:
		TEST_METHOD(Valid)
		{
			WinHandle<handle_type> h1;
			Assert::IsFalse(h1.valid());

			WinHandle<handle_type, INVALID_HANDLE_VALUE> h2;
			Assert::IsFalse(h2.valid());

			WinHandle<handle_type> h3{ Handle1, nullptr };
			Assert::IsTrue(h3.valid());

			WinHandle<handle_type, INVALID_HANDLE_VALUE> h4{ Handle1, nullptr };
			Assert::IsTrue(h4.valid());
		}

		TEST_METHOD(Get)
		{
			WinHandle<handle_type> h1;
			Assert::AreEqual(reinterpret_cast<HANDLE>(0), h1.get());

			WinHandle<handle_type, INVALID_HANDLE_VALUE> h2;
			Assert::AreEqual(INVALID_HANDLE_VALUE, h2.get());

			WinHandle<handle_type> h3{ Handle1, nullptr };
			Assert::AreEqual(Handle1, h3.get());

			WinHandle<handle_type, INVALID_HANDLE_VALUE> h4{ Handle1, nullptr };
			Assert::AreEqual(Handle1, h4.get());
		}

		TEST_METHOD(ConstPtr)
		{
			const WinHandle<handle_type> h1{ Handle1, nullptr };
			const handle_type* h = h1.ptr();
			Assert::IsTrue(h != nullptr);
			if (h != nullptr)
				Assert::AreEqual(Handle1, *h);
		}

		TEST_METHOD(Ptr)
		{
			WinHandle<handle_type> h1{ Handle1, nullptr };
			auto h = h1.ptr();
			Assert::IsTrue(h != nullptr);
			if (h != nullptr)
				Assert::AreEqual(Handle1, *h);
		}

		TEST_METHOD(Close)
		{
			MockDeleter<handle_type> deleter{ std::vector<handle_type>{ Handle1 } };
			WinHandle<handle_type> h1{ Handle1, &MockDeleter<handle_type>::Delete, &deleter };

			Assert::IsTrue(h1.valid());

			h1.close();

			Assert::IsFalse(h1.valid());
			Assert::IsTrue(deleter.called() == 1);
		}

		// TODO - See if we can prevent user from assigning output of non-const ptr() to a T*
	};
}