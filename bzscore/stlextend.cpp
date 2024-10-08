// Copyright (c) 2015-2017, Satoshi Tanda. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.

/// @file
/// Implements code to use STL in a driver project

#include "stdafx.h"

#ifdef _KERNEL_MODE
// #include <ntifs.h>
// #include <fltKernel.h>
#include <string>
#undef _HAS_EXCEPTIONS
#define _HAS_EXCEPTIONS 0

// See common.h for details
#pragma prefast(disable : 30030)

////////////////////////////////////////////////////////////////////////////////
//
// macro utilities
//
////////////////////////////////////////////////////////////////////////////////
//
// constants and macros
//

/// A pool tag for this module
static const ULONG kKstlpPoolTag = 'LTSK';

////////////////////////////////////////////////////////////////////////////////
//
// types
//

////////////////////////////////////////////////////////////////////////////////
//
// prototypes
//

////////////////////////////////////////////////////////////////////////////////
//
// variables
//

////////////////////////////////////////////////////////////////////////////////
//
// implementations
//


// An alternative implementation of a C++ exception handler. Issues a bug check.
DECLSPEC_NORETURN static void KernelStlpRaiseException(
    _In_ ULONG bug_check_code) {
    KdBreakPoint();
#pragma warning(push)
#pragma warning(disable : 28159)
    KeBugCheck(bug_check_code);
#pragma warning(pop)
}
//DECLSPEC_NORETURN void __cdecl _invalid_parameter_noinfo_noreturn(void) {
//	KernelStlpRaiseException(KMODE_EXCEPTION_NOT_HANDLED);
//}
//#ifndef DBG
DECLSPEC_NORETURN void __cdecl _invalid_parameter_noinfo_noreturn(void) {
    KernelStlpRaiseException(KMODE_EXCEPTION_NOT_HANDLED);
}
//#endif // !DBG
extern "C" void __cdecl _invoke_watson(
    wchar_t const* const expression,
    wchar_t const* const function_name,
    wchar_t const* const file_name,
    unsigned int   const line_number,
    uintptr_t      const reserved)
{
    UNREFERENCED_PARAMETER(expression);
    UNREFERENCED_PARAMETER(function_name);
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);
    UNREFERENCED_PARAMETER(reserved);
}
namespace std {
    _CRTIMP2_PURE_IMPORT _Prhand _Raise_handler;
   // _CRTIMP2_NCEEPURE_IMPORT _Prhand _Raise_handler;
    DECLSPEC_NORETURN void __cdecl _Xbad_alloc() {
        KernelStlpRaiseException(KMODE_EXCEPTION_NOT_HANDLED);
    }
    DECLSPEC_NORETURN void __cdecl _Xinvalid_argument(_In_z_ const char*) {
        KernelStlpRaiseException(KMODE_EXCEPTION_NOT_HANDLED);
    }
    DECLSPEC_NORETURN void __cdecl _Xlength_error(_In_z_ const char*) {
        KernelStlpRaiseException(KMODE_EXCEPTION_NOT_HANDLED);
    }
    DECLSPEC_NORETURN void __cdecl _Xout_of_range(_In_z_ const char*) {
        KernelStlpRaiseException(KMODE_EXCEPTION_NOT_HANDLED);
    }
    DECLSPEC_NORETURN void __cdecl _Xoverflow_error(_In_z_ const char*) {
        KernelStlpRaiseException(KMODE_EXCEPTION_NOT_HANDLED);
    }
    DECLSPEC_NORETURN void __cdecl _Xruntime_error(_In_z_ const char*) {
        KernelStlpRaiseException(KMODE_EXCEPTION_NOT_HANDLED);
    }

}  // namespace std

// An alternative implementation of the new operator
_IRQL_requires_max_(DISPATCH_LEVEL) void* __cdecl operator new[](
    _In_ size_t size) {
        if (size == 0) {
            size = 1;
        }

        const auto p = ExAllocatePoolWithTag(NonPagedPool, size, kKstlpPoolTag);
        if (!p) {
            KernelStlpRaiseException(MUST_SUCCEED_POOL_EMPTY);
        }
        return p;
}

// An alternative implementation of the new operator
_IRQL_requires_max_(DISPATCH_LEVEL) void __cdecl operator delete[](_In_ void* p) {
    if (p) {
        ExFreePoolWithTag(p, kKstlpPoolTag);
    }
}

// An alternative implementation of the new operator
_IRQL_requires_max_(DISPATCH_LEVEL) void* __cdecl operator new(
    _In_ size_t size) {
    if (size == 0) {
        size = 1;
    }

    const auto p = ExAllocatePoolWithTag(NonPagedPool, size, kKstlpPoolTag);
    if (!p) {
        KernelStlpRaiseException(MUST_SUCCEED_POOL_EMPTY);
    }
    return p;
}

// An alternative implementation of the new operator
_IRQL_requires_max_(DISPATCH_LEVEL) void __cdecl operator delete(_In_ void* p) {
    if (p) {
        ExFreePoolWithTag(p, kKstlpPoolTag);
    }
}

// An alternative implementation of the new operator
_IRQL_requires_max_(DISPATCH_LEVEL) void __cdecl operator delete(
    _In_ void* p, _In_ size_t size) {
    UNREFERENCED_PARAMETER(size);
    if (p) {
        ExFreePoolWithTag(p, kKstlpPoolTag);
    }
}

// An alternative implementation of __stdio_common_vsprintf_s
_Success_(return >= 0) EXTERN_C int __cdecl __stdio_common_vsprintf_s(
    _In_ unsigned __int64 _Options, _Out_writes_z_(_BufferCount) char* _Buffer,
    _In_ size_t _BufferCount,
    _In_z_ _Printf_format_string_params_(2) char const* _Format,
    _In_opt_ _locale_t _Locale, va_list _ArgList) {
    UNREFERENCED_PARAMETER(_Options);
    UNREFERENCED_PARAMETER(_Locale);

    // Calls _vsnprintf exported by ntoskrnl
    using _vsnprintf_type = int __cdecl(char*, size_t, const char*, va_list);
    static _vsnprintf_type* local__vsnprintf = nullptr;
    if (!local__vsnprintf) {
        UNICODE_STRING proc_name_U = {};
        RtlInitUnicodeString(&proc_name_U, L"_vsnprintf");
        local__vsnprintf = reinterpret_cast<_vsnprintf_type*>(
            MmGetSystemRoutineAddress(&proc_name_U));
    }

    return local__vsnprintf(_Buffer, _BufferCount, _Format, _ArgList);
}

// An alternative implementation of __stdio_common_vswprintf_s
_Success_(return >= 0) _Check_return_opt_ EXTERN_C
int __cdecl __stdio_common_vswprintf_s(
    _In_ unsigned __int64 _Options,
    _Out_writes_z_(_BufferCount) wchar_t* _Buffer, _In_ size_t _BufferCount,
    _In_z_ _Printf_format_string_params_(2) wchar_t const* _Format,
    _In_opt_ _locale_t _Locale, va_list _ArgList) {
    UNREFERENCED_PARAMETER(_Options);
    UNREFERENCED_PARAMETER(_Locale);

    // Calls _vsnwprintf exported by ntoskrnl
    using _vsnwprintf_type =
        int __cdecl(wchar_t*, size_t, const wchar_t*, va_list);
    static _vsnwprintf_type* local__vsnwprintf = nullptr;
    if (!local__vsnwprintf) {
        UNICODE_STRING proc_name_U = {};
        RtlInitUnicodeString(&proc_name_U, L"_vsnwprintf");
        local__vsnwprintf = reinterpret_cast<_vsnwprintf_type*>(
            MmGetSystemRoutineAddress(&proc_name_U));
    }

    return local__vsnwprintf(_Buffer, _BufferCount, _Format, _ArgList);
}

// An alternative implementation of __stdio_common_vswprintf
_Success_(return >= 0) _Check_return_opt_ EXTERN_C
int __cdecl __stdio_common_vswprintf(
    _In_ unsigned __int64 _Options,
    _Out_writes_z_(_BufferCount) wchar_t* _Buffer, _In_ size_t _BufferCount,
    _In_z_ _Printf_format_string_params_(2) wchar_t const* _Format,
    _In_opt_ _locale_t _Locale, va_list _ArgList) {
    UNREFERENCED_PARAMETER(_Options);
    UNREFERENCED_PARAMETER(_Locale);

    // Calls _vsnwprintf exported by ntoskrnl
    using _vsnwprintf_type =
        int __cdecl(wchar_t*, size_t, const wchar_t*, va_list);
    static _vsnwprintf_type* local__vsnwprintf = nullptr;
    if (!local__vsnwprintf) {
        UNICODE_STRING proc_name_U = {};
        RtlInitUnicodeString(&proc_name_U, L"_vsnwprintf");
        local__vsnwprintf = reinterpret_cast<_vsnwprintf_type*>(
            MmGetSystemRoutineAddress(&proc_name_U));
    }

    return local__vsnwprintf(_Buffer, _BufferCount, _Format, _ArgList);
}

int _memicmp(const void *buf1, const void *buf2, size_t count) {
    const unsigned char *p1 = (const unsigned char *)buf1;
    const unsigned char *p2 = (const unsigned char *)buf2;

    for (size_t i = 0; i < count; i++) {
        int c1 = tolower(p1[i]);
        int c2 = tolower(p2[i]);

        if (c1 != c2) {
            return c1 - c2;
        }
    }
    return 0;
}
#endif

