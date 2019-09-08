#pragma once

#pragma comment(lib, "hid.lib")
#pragma comment(lib, "Dwmapi.lib")

#define _WINSOCKAPI_

#include <stdio.h>
#include <stdint.h>
#include <cstring>
#include <cassert>
#include <stdlib.h>

typedef uint8_t		byte;

typedef uint8_t		u8;
typedef int8_t		i8;
typedef uint16_t	u16;
typedef int16_t		i16;
typedef uint32_t	u32;
typedef int32_t		i32;
typedef uint64_t	u64;
typedef int64_t		i64;

struct Color
{
	byte r;
	byte g;
	byte b;
	byte a;

	Color(u32 col) :
		a((col & 0xff000000) >> 24),
		r((col & 0x00ff0000) >> 16),
		g((col & 0x0000ff00) >> 8),
		b((col & 0x000000ff))
	{
	}

	Color() {};

	operator u32()
	{
		return *((u32*)this);
	}
};

#define MIN(a,b) (a<b?a:b)
#define MAX(a,b) (a<b?b:a)
#define CLAMP(value, min, max) (MAX(MIN(value,max),min))

#define PI_DOUBLE 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844609550582231725359408128481
#define PI 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844609550582231725359408128481f

#define TAU 2*PI
#define TAU_DOBLE 2*PI_DOUBLE

constexpr static float ToRad(float degree)
{
	return degree * PI / 180;
}

constexpr static float ToDegree(float rad)
{
	return rad * 180 / PI;
}

extern void SetConsoleColor(int col);

#ifdef _DEBUG
#define BREAK_ON_ERROR 1
#else
#define BREAK_ON_ERROR 0
#endif

#if BREAK_ON_ERROR
#define LogError(str,...) (SetConsoleColor(12),printf("ERROR in %s at line %i: " str "\n",__FILE__,__LINE__, __VA_ARGS__),SetConsoleColor(7),__debugbreak())
#define LogErrorRaw(str,...) (SetConsoleColor(12),printf(str "\n", __VA_ARGS__),SetConsoleColor(7),__debugbreak())
#else
#define LogError(str,...) (SetConsoleColor(12),printf("ERROR in %s at line %i: " str "\n",__FILE__,__LINE__, __VA_ARGS__),SetConsoleColor(7))
#define LogErrorRaw(str,...) (SetConsoleColor(12),printf(str "\n", __VA_ARGS__),SetConsoleColor(7))
#endif
#define LogWarning(str,...) (SetConsoleColor(14),printf(str"\n",__VA_ARGS__),SetConsoleColor(7))
#define LogInfo(str,...) (SetConsoleColor(15),printf(str"\n",__VA_ARGS__),SetConsoleColor(7))
#ifdef _DEBUG
#define LogDebug(str,...) (SetConsoleColor(8),printf(str"\n",__VA_ARGS__),SetConsoleColor(7))
#else
#define LogDebug(str,...) ((void)0)
#endif

#define INLINE __forceinline

#define CONSOLE
#define DEVELOPMENT

#ifdef CONSOLE
#pragma comment( linker, "/subsystem:console" )
#endif

#define SetBitMask(variable, mask, value) ((value) ? (variable) |= (mask) : (variable) &= ~(mask))

#include "list.h"