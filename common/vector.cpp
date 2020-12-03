//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2012
//
// vector.cpp
// Must be compatible with HL1/HL2
//-----------------------------------------------------------------------------
#include "vector.h"
//#include <xmmintrin.h>
/*
#if defined(_LINUX)
#define USE_STDC_FOR_SIMD 0
#else
#define USE_STDC_FOR_SIMD 0
#endif
*/
// Use this name instead of vec3_origin because that name is used by PM code, leave it alone
//const vec3_t vec3_zero(0.0f,0.0f,0.0f);
const vec3_t g_vecZero(0.0f,0.0f,0.0f);

// TODO: make use of SIMD instructions globally
// TODO: make expressions like Vector c = a + 100*b; use less temporary objects...
