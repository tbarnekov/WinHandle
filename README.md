# WinHandle

_WinHandle_ is a simple and easy to use C++20 wrapper for managing the lifetime of Windows handles. It mimics a shared pointer to ensure the handle is released when its reference count drops to zero.

## Installation

Download WinHandle.h from this repository and place it in the include path of your project.

## Usage

### Simple example
The _WinHandle_ template takes the type of the owned handle as the first template parameter.
```cpp
#include "WinHandle.h"

int main(int argc, char** argv)
{
    WinHandle<HANDLE> dc{ &DeleteDC };
    dc = CreateDC(TEXT("DISPLAY"), nullptr, nullptr, nullptr);
    ...
}
```

### NULL handle values

Some Windows handles use a value other than __NULL__ to signify an invalid handle value. The null value can be specified as the second template parameter. For instance, __CreateFile__ returns __INVALID_HANDLE_VALUE__ to signal an invalid handle.

```cpp
WinHandle<HANDLE, INVALID_HANDLE_VALUE> hFile { &CloseHandle };
hFile = CreateFile(TEXT("file.txt"), GENERIC_READ, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr);
if (hFile.valid())
{
    ...
}
```

### Getting a handle as an OUT parameter

Some handles are acquired by calling a function with a pointer to the handle as a parameter. _WinHandle_ supports this using the .ptr() member function.

```cpp
WinHandle<HCRYPTPROV> hProv;
if (CryptAcquireContext(hProv.ptr(), TEXT("MyKeyContainer"), NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET))
{
    ...
}

```

If the _WinHandle_ already contains a valid handle, that handle will be released and replaced by the new handle.

### Handle release with additional parameters

Some handles must be released using functions that take more parameters than just the handle. If the handle is the first parameter of the release function, the additional parameters can be specified in the _WinHandle_ constructor. For instance, __CryptReleaseContext__ takes an additional DWORD parameter.

```cpp
WinHandle<HCRYPTPROV> hProv{ &CryptReleaseContext, static_cast<DWORD>(0) };
if (CryptAcquireContext(hProv.ptr(), TEXT("MyKeyContainer"), NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET))
{
    ...
}
```

### Custom release functions

You can specify a custom function to be called when the handle should be released. _WinHandle_ supports std::function, plain function pointers and member function pointers as handle release functions.

```cpp
// std::function
WinHandle<HANDLE> { [](Handle h) { return TRUE; } };

// Function pointer
WinHandle<HANDLE> { &CloseHandle };

// Member function pointer
MyClass cls;
WinHandle<HANDLE> { &MyClass::releaseHandle, &cls };
```

### Assignment

```cpp
// Initialize a WinHandle with a HANDLE to a file
WinHandle<HANDLE, INVALID_HANDLE_VALUE> handle1 { CreateFile(TEXT("File1.txt"), GENERIC_READ, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr), &CloseHandle };

// Assigning a new handle will release the original handle.
handle1 = CreateFile(TEXT("File2.txt", GENERIC_READ, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr);

// Create a new WinHandle
WinHandle<HANDLE, INVALID_HANDLE_VALUE> handle2 { CreateFile(TEXT("File1.txt"), GENERIC_READ, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr), &CloseHandle };

// The handle owned by handle1 is released and handle1 and handle2 will reference the same handle
handle1 = handle2;
```

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License

[MIT](https://choosealicense.com/licenses/mit/)