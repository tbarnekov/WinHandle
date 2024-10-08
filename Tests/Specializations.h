#pragma once
#include "pch.h"
#include "CppUnitTest.h"

namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework {

			// Specialization for HANDLE
			template<> static std::wstring ToString<HANDLE>(HANDLE* t)
			{
				RETURN_WIDE_STRING(t);
			}

			// Specialization for HDC
			template<> static std::wstring ToString<HDC__>(HDC__* t)
			{
				RETURN_WIDE_STRING(t);
			}

		}
	}
}
