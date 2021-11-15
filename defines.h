#ifndef __IF_DEFINES_H_
#define __IF_DEFINES_H_

#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#define _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <tchar.h>
#include <windows.h>
#include <stdio.h>

#define MIN(a, b) (((a) <= (b))?(a):(b))


typedef enum _RETC
{
    RETC_SUCCESS,
    RETC_MEMALLOC_ERROR,
    RETC_INPUT_ERROR,
    RETC_LOGIC_ERROR,
} RETC;



int _MyDebugPrintln(const char* str);
int LogerPrintRETC(RETC rc);

#endif // __IF_DEFINES_H_
