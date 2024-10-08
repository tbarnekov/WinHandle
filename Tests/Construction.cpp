#include "pch.h"
#include "CppUnitTest.h"
#include <WinHandle.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Construction
{
	TEST_CLASS(ConstructorOverloads)
	{
	private:
#pragma warning(push)
#pragma warning(disable: 4312) // 'reinterpret_cast': conversion from 'unsigned int' to 'void *' of greater size
		inline static const HANDLE HandleValue = reinterpret_cast<HANDLE>(1234);
		inline static const DWORD Param1 = 17;
		inline static const void* Param2 = reinterpret_cast<void*>(0xdeadbeef);
#pragma warning(pop)

		using handle_type = std::remove_cv_t<decltype(HandleValue)>;
		using param1_type = std::remove_cv_t<decltype(Param1)>;
		using param2_type = std::remove_cv_t<decltype(Param2)>;

	public:
		// Function pointers
		static BOOL __stdcall fp0(handle_type h)
		{
			Assert::AreEqual(HandleValue, h);
			return TRUE;
		}
		static BOOL __stdcall fp1(handle_type h, param1_type p1)
		{
			Assert::AreEqual(HandleValue, h);
			Assert::AreEqual(Param1, p1);
			return TRUE;
		}

		static BOOL __stdcall fp2(handle_type h, param1_type p1, param2_type p2)
		{
			Assert::AreEqual(HandleValue, h);
			Assert::AreEqual(Param1, p1);
			Assert::AreEqual(Param2, p2);
			return TRUE;
		}

		// Member function pointers
		BOOL __stdcall mfp0(handle_type h)
		{
			Assert::AreEqual(HandleValue, h);
			return TRUE;
		}
		BOOL __stdcall mfp1(handle_type h, param1_type p1)
		{
			Assert::AreEqual(HandleValue, h);
			Assert::AreEqual(Param1, p1);
			return TRUE;
		}

		BOOL __stdcall mfp2(handle_type h, param1_type p1, param2_type p2)
		{
			Assert::AreEqual(HandleValue, h);
			Assert::AreEqual(Param1, p1);
			Assert::AreEqual(Param2, p2);
			return TRUE;
		}

		TEST_METHOD(DefaultConstructor)
		{
			// Default constructor (non-owning)
			WinHandle<handle_type> h10;
			Assert::IsFalse(h10.valid());
		}

		TEST_METHOD(NonOwningConstructor)
		{
			// Non-owning constructor
			WinHandle<handle_type> h21(HandleValue, nullptr);
			Assert::IsTrue(h21.valid());
			Assert::AreEqual(HandleValue, h21.get());
		}

		TEST_METHOD(StdFunction)
		{
			// std::function
			WinHandle<handle_type> h31([](handle_type h) -> BOOL { Assert::AreEqual(HandleValue, h); return TRUE; }); h31 = HandleValue;
			WinHandle<handle_type> h32(HandleValue, [](handle_type h) -> BOOL { Assert::AreEqual(HandleValue, h); return TRUE; });
		}

		TEST_METHOD(FunctionPointer)
		{
			// Function pointer
			WinHandle<handle_type> h41(&fp0); h41 = HandleValue;
			WinHandle<handle_type> h42(HandleValue, &fp0);
			WinHandle<handle_type> h43(&fp1, static_cast<param1_type>(Param1)); h43 = HandleValue;
			WinHandle<handle_type> h44(HandleValue, &fp1, static_cast<param1_type>(Param1));
			WinHandle<handle_type> h45(&fp2, static_cast<param1_type>(Param1), static_cast<param2_type>(Param2)); h45 = HandleValue;
			WinHandle<handle_type> h46(HandleValue, &fp2, static_cast<param1_type>(Param1), static_cast<param2_type>(Param2));
		}

		TEST_METHOD(MemberFunctionPointer)
		{
			// Member function pointer
			using Class = std::remove_reference_t<decltype(*this)>;
			WinHandle<handle_type> h51(&Class::mfp0, this); h51 = HandleValue;
			WinHandle<handle_type> h52(HandleValue, &Class::mfp0, this);
			WinHandle<handle_type> h53(&Class::mfp1, this, static_cast<param1_type>(Param1)); h53 = HandleValue;
			WinHandle<handle_type> h54(HandleValue, &Class::mfp1, this, static_cast<param1_type>(Param1));
			WinHandle<handle_type> h55(&Class::mfp2, this, static_cast<param1_type>(Param1), static_cast<param2_type>(Param2)); h55 = HandleValue;
			WinHandle<handle_type> h56(HandleValue, &Class::mfp2, this, static_cast<param1_type>(Param1), static_cast<param2_type>(Param2));
		}

		TEST_METHOD(CopyConstructor)
		{
			WinHandle<handle_type> h10(HandleValue, &fp0);
			WinHandle<handle_type> h20(h10);

			Assert::IsTrue(h10.valid());
			Assert::IsTrue(h20.valid());

			Assert::AreEqual(HandleValue, h10.get());
			Assert::AreEqual(HandleValue, h20.get());

			Assert::AreEqual(2l, h10.use_count());
			Assert::AreEqual(2l, h20.use_count());
		}

		TEST_METHOD(CopyAssignmentConstructor)
		{
			WinHandle<handle_type> h10(HandleValue, &fp0);
			WinHandle<handle_type> h20 = h10;

			Assert::IsTrue(h10.valid());
			Assert::IsTrue(h20.valid());

			Assert::AreEqual(HandleValue, h10.get());
			Assert::AreEqual(HandleValue, h20.get());

			Assert::AreEqual(2l, h10.use_count());
			Assert::AreEqual(2l, h20.use_count());
		}

		TEST_METHOD(MoveConstructor)
		{
			WinHandle<handle_type> h10(HandleValue, &fp0);
			WinHandle<handle_type> h20(std::move(h10));

			Assert::IsFalse(h10.valid());
			Assert::IsTrue(h20.valid());

			Assert::AreEqual(reinterpret_cast<handle_type>(0), h10.get());
			Assert::AreEqual(HandleValue, h20.get());

			Assert::AreEqual(1l, h10.use_count());
			Assert::AreEqual(1l, h20.use_count());
		}

		TEST_METHOD(MoveAssignmentConstructor)
		{
			WinHandle<handle_type> h10(HandleValue, &fp0);
			WinHandle<handle_type> h20 = std::move(h10);

			Assert::IsFalse(h10.valid());
			Assert::IsTrue(h20.valid());

			Assert::AreEqual(reinterpret_cast<handle_type>(0), h10.get());
			Assert::AreEqual(HandleValue, h20.get());

			Assert::AreEqual(1l, h10.use_count());
			Assert::AreEqual(1l, h20.use_count());
		}
	};


	TEST_CLASS(ConstructorOverloadsInitializers)
	{
	private:
#pragma warning(push)
#pragma warning(disable: 4312) // 'reinterpret_cast': conversion from 'unsigned int' to 'void *' of greater size
		inline static const HANDLE HandleValue = reinterpret_cast<HANDLE>(1234);
		inline static const DWORD Param1 = 17;
		inline static const void* Param2 = reinterpret_cast<void*>(0xdeadbeef);

		using handle_type = std::remove_cv_t<decltype(HandleValue)>;
		using param1_type = std::remove_cv_t<decltype(Param1)>;
		using param2_type = std::remove_cv_t<decltype(Param2)>;

#pragma warning(pop)

	public:
		// Function pointers
		static BOOL __stdcall fp0(handle_type h)
		{
			Assert::AreEqual(HandleValue, h);
			return TRUE;
		}
		static BOOL __stdcall fp1(handle_type h, param1_type p1)
		{
			Assert::AreEqual(HandleValue, h);
			Assert::AreEqual(Param1, p1);
			return TRUE;
		}

		static BOOL __stdcall fp2(handle_type h, param1_type p1, param2_type p2)
		{
			Assert::AreEqual(HandleValue, h);
			Assert::AreEqual(Param1, p1);
			Assert::AreEqual(Param2, p2);
			return TRUE;
		}

		// Member function pointers
		BOOL __stdcall mfp0(handle_type h)
		{
			Assert::AreEqual(HandleValue, h);
			return TRUE;
		}
		BOOL __stdcall mfp1(handle_type h, param1_type p1)
		{
			Assert::AreEqual(HandleValue, h);
			Assert::AreEqual(Param1, p1);
			return TRUE;
		}

		BOOL __stdcall mfp2(handle_type h, param1_type p1, param2_type p2)
		{
			Assert::AreEqual(HandleValue, h);
			Assert::AreEqual(Param1, p1);
			Assert::AreEqual(Param2, p2);
			return TRUE;
		}

		TEST_METHOD(DefaultConstructor)
		{
			// Default constructor (non-owning)
			WinHandle<handle_type> h1;
			Assert::IsFalse(h1.valid());
		}

		TEST_METHOD(NonOwningConstructor)
		{
			// Non-owning constructor
			WinHandle<handle_type> h21{ HandleValue, nullptr };
			Assert::IsTrue(h21.valid());
			Assert::AreEqual(HandleValue, h21.get());
		}

		TEST_METHOD(StdFunction)
		{
			// std::function
			WinHandle<handle_type> h31{ [](handle_type h) -> BOOL { Assert::AreEqual(HandleValue, h); return TRUE; } }; h31 = HandleValue;
			WinHandle<handle_type> h32{ HandleValue, [](handle_type h) -> BOOL { Assert::AreEqual(HandleValue, h); return TRUE; } };
		}

		TEST_METHOD(FunctionPointer)
		{
			// Function pointer
			WinHandle<handle_type> h41{ &fp0 }; h41 = HandleValue;
			WinHandle<handle_type> h42{ HandleValue, &fp0 };
			WinHandle<handle_type> h43{ &fp1, static_cast<param1_type>(Param1) }; h43 = HandleValue;
			WinHandle<handle_type> h44{ HandleValue, &fp1, static_cast<param1_type>(Param1) };
			WinHandle<handle_type> h45{ &fp2, static_cast<param1_type>(Param1), static_cast<param2_type>(Param2) }; h45 = HandleValue;
			WinHandle<handle_type> h46{ HandleValue, &fp2, static_cast<param1_type>(Param1), static_cast<param2_type>(Param2) };
		}

		TEST_METHOD(MemberFunctionPointer)
		{
			// Member function pointer
			using Class = std::remove_reference_t<decltype(*this)>;
			WinHandle<handle_type> h51{ &Class::mfp0, this }; h51 = HandleValue;
			WinHandle<handle_type> h52{ HandleValue, &Class::mfp0, this };
			WinHandle<handle_type> h53{ &Class::mfp1, this, static_cast<param1_type>(Param1) }; h53 = HandleValue;
			WinHandle<handle_type> h54{ HandleValue, &Class::mfp1, this, static_cast<param1_type>(Param1) };
			WinHandle<handle_type> h55{ &Class::mfp2, this, static_cast<param1_type>(Param1), static_cast<param2_type>(Param2) }; h55 = HandleValue;
			WinHandle<handle_type> h56{ HandleValue, &Class::mfp2, this, static_cast<param1_type>(Param1), static_cast<param2_type>(Param2) };
		}

		TEST_METHOD(CopyConstructor)
		{
			WinHandle<handle_type> h10{ HandleValue, &fp0 };
			WinHandle<handle_type> h20{ h10 };

			Assert::IsTrue(h10.valid());
			Assert::IsTrue(h20.valid());

			Assert::AreEqual(HandleValue, h10.get());
			Assert::AreEqual(HandleValue, h20.get());

			Assert::AreEqual(2l, h10.use_count());
			Assert::AreEqual(2l, h20.use_count());
		}

		TEST_METHOD(CopyAssignmentConstructor)
		{
			WinHandle<handle_type> h10{ HandleValue, &fp0 };
			WinHandle<handle_type> h20 = h10;

			Assert::IsTrue(h10.valid());
			Assert::IsTrue(h20.valid());

			Assert::AreEqual(HandleValue, h10.get());
			Assert::AreEqual(HandleValue, h20.get());

			Assert::AreEqual(2l, h10.use_count());
			Assert::AreEqual(2l, h20.use_count());
		}

		TEST_METHOD(MoveConstructor)
		{
			WinHandle<handle_type> h10{ HandleValue, &fp0 };
			WinHandle<handle_type> h20{ std::move(h10) };

			Assert::IsFalse(h10.valid());
			Assert::IsTrue(h20.valid());

			Assert::AreEqual(reinterpret_cast<handle_type>(0), h10.get());
			Assert::AreEqual(HandleValue, h20.get());

			Assert::AreEqual(1l, h10.use_count());
			Assert::AreEqual(1l, h20.use_count());
		}
	};
}
