//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#if !defined( STUDIO_UTIL_H )
#define STUDIO_UTIL_H
#if defined( WIN32 )
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif
/*
#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

#ifndef PITCH
// MOVEMENT INFO
// up / down
#define	PITCH	0
// left / right
#define	YAW		1
// fall over
#define	ROLL	2
#endif
*/

int Mod_GetType(int modelIndex);

extern struct engine_studio_api_s IEngineStudio;

#endif // STUDIO_UTIL_H
