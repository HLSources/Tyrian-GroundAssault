/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

//
// pm_shared.h
//
#if !defined( PM_SHAREDH )
#define PM_SHAREDH
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif

// HACK: XDM3035: in server dlls: error C2732: linkage specification contradiction
//#ifdef CLIENT_DLL
void PM_Init(struct playermove_s *ppmove);
void PM_Move(struct playermove_s *ppmove, int server);
char PM_FindTextureType(char *name);
//int PM_MapTextureTypeStepType(const char &chTextureType);
int PM_GetVisEntInfo(int ent);
int PM_GetPhysEntInfo(int ent);
//#endif
extern struct playermove_s *pmove;// XDM3038c: careful! This is for client AND server!

// Spectator Movement modes (stored in pev->iuser1, so the physics code can get at them)
#define OBS_NONE			0
#define OBS_CHASE_LOCKED	1// use target origin and angles // usual "in-world" modes
#define OBS_CHASE_FREE		2// use target origin and camera angles
#define OBS_ROAMING			3// use camera origin and camera angles
#define OBS_IN_EYE			4// use target view
#define OBS_MAP_FREE		5// roaming on a map // fullscreen inset modes
#define OBS_MAP_CHASE		6// chase_free on a map
#define OBS_INTERMISSION	7

// XDM 3034: moved here all usable stuff
// Ducking time
#define TIME_TO_DUCK		0.4

// HACK: XDM 3034: in server dlls these should be vectors (float[3])

#define DUCK_HULL_MIN		-18
#define DUCK_HULL_MAX		18
#define DUCK_VIEW			12
#define HULL_MIN			-36
#define HULL_MAX			36
#define HULL_RADIUS			16

#define VEC_VIEW			32// XDM3035

// XDM3035c
#define VEC_HULL_MIN		Vector(-16,-16, HULL_MIN)
#define VEC_HULL_MAX		Vector( 16, 16, HULL_MAX)
#define VEC_HUMAN_HULL_MIN	Vector(-16,-16, 0)
#define VEC_HUMAN_HULL_MAX	Vector( 16, 16, HULL_MAX - HULL_MIN)// 72
#define VEC_HUMAN_HULL_DUCK	Vector( 16, 16, DUCK_HULL_MAX - DUCK_HULL_MIN)// 36
#define VEC_VIEW_OFFSET		Vector(  0,  0, VEC_VIEW)
#define VEC_DUCK_HULL_MIN	Vector(-16,-16, DUCK_HULL_MIN)
#define VEC_DUCK_HULL_MAX	Vector( 16, 16, DUCK_HULL_MAX)
#define VEC_DUCK_VIEW		Vector(  0,  0, DUCK_VIEW)


#define PM_DEAD_VIEWHEIGHT	-8
#define MAX_CLIMB_SPEED		128
#define STUCK_MOVEUP		1
#define STUCK_MOVEDOWN		-1
#define	STOP_EPSILON		0.1

#define PLAYER_FATAL_FALL_SPEED			1024// approx 60 feet
#define PLAYER_MAX_SAFE_FALL_SPEED		580// approx 20 feet
#define DAMAGE_FOR_FALL_SPEED			(float) 100 / ( PLAYER_FATAL_FALL_SPEED - PLAYER_MAX_SAFE_FALL_SPEED )// damage per unit per second.
#define PLAYER_MIN_BOUNCE_SPEED			PLAYER_MAX_SAFE_FALL_SPEED*0.3// ~174
// XDM: don't mix these!
#define PLAYER_FALL_PUNCH_THRESHHOLD	(float)350 // won't punch player's screen/make scrape noise unless player falling at least this fast.
#define PLAYER_LONGJUMP_SPEED			450 // how fast we longjump
#define SPEED_SPECTATOR_MULTIPLIER		5 //how fast spectator moves

#define	CONTENTS_CURRENT_0		-9
#define	CONTENTS_CURRENT_90		-10
#define	CONTENTS_CURRENT_180	-11
#define	CONTENTS_CURRENT_270	-12
#define	CONTENTS_CURRENT_UP		-13
#define	CONTENTS_CURRENT_DOWN	-14
#define CONTENTS_TRANSLUCENT	-15

// XDM: from player.h

// Player PHYSICS FLAGS bits
#define	PFLAG_ONLADDER		( 1<<0 )
#define	PFLAG_ONSWING		( 1<<0 )
#define	PFLAG_ONTRAIN		( 1<<1 )
#define	PFLAG_ONBARNACLE	( 1<<2 )
#define	PFLAG_DUCKING		( 1<<3 )		// In the process of ducking, but totally squatted yet
#define	PFLAG_USING			( 1<<4 )		// Using a continuous entity
#define PFLAG_OBSERVER		( 1<<5 )		// player is locked in stationary cam mode. Spectators can move, observers can't.

// XDM: from monsters.h

// Hit Group standards
#define	HITGROUP_GENERIC	0
#define	HITGROUP_HEAD		1
#define	HITGROUP_CHEST		2
#define	HITGROUP_STOMACH	3
#define HITGROUP_LEFTARM	4
#define HITGROUP_RIGHTARM	5
#define HITGROUP_LEFTLEG	6
#define HITGROUP_RIGHTLEG	7
#define HITGROUP_ARMOR		10// XDM

// SetPhysicsKeyValue
#define PHYSKEY_DEFAULT			"hl"// what does it mean anyway?
#define PHYSKEY_LONGJUMP		"slj"
#define PHYSKEY_BUTTHURT		"buht"
#define PHYSKEY_HASTE			"haste"
#define PHYSKEY_IGNORELADDER	"ild"
#define PHYSKEY_INVISIBILITY	"invis"
#define PHYSKEY_INVULNERABILITY	"invul"
#define PHYSKEY_QUADDAMAGE		"quad"
#define PHYSKEY_BANANA			"banan"
#define PHYSKEY_ACCURACY		"accur"
#define PHYSKEY_ANTIDOTE		"antid"
#define PHYSKEY_SHIELD_REGEN	"sregen"
#define PHYSKEY_RAPIDFIRE		"rapid"
#define PHYSKEY_LIGHTNING_FIELD	"lgtngf"
#define PHYSKEY_RADSHIELD		"rshld"
#define PHYSKEY_FIRE_SUPRESSOR	"firesup"
#define PHYSKEY_PLASMA_SHIELD	"pshld"
#endif
