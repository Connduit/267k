// https://github.com/vxunderground/VX-API

#ifndef MINI_STD_H
#define MINI_STD_H


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#define NO_INLINE __attribute__ ((noinline))



SIZE_T CharStringToWCharString(PWCHAR Destination, PCHAR Source, SIZE_T MaximumAllowed)
{
    printf("inside\n");
    INT Length = MaximumAllowed;

    while (--Length >= 0)
    {
        if (!(*Destination++ = *Source++))  // Wide char = ANSI char
            return MaximumAllowed - Length - 1;
    }

    printf("inside\n");
    return MaximumAllowed - Length;
}

SIZE_T WCharStringToCharString(PCHAR Destination, PWCHAR Source, SIZE_T MaximumAllowed)
{
    INT Length = MaximumAllowed;

    while (--Length >= 0)
    {
        if (!(*Destination++ = *Source++))
            return MaximumAllowed - Length - 1;
    }

    return MaximumAllowed - Length;
}


//BYTE HideChar(BYTE C);
// ignore warnings/errors from ide... this still compiles fine
BYTE NO_INLINE HideChar(BYTE C)
{
    return C;
}


#endif