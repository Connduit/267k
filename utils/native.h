
#ifndef NATIVE_H
#define NATIVE_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;

#endif