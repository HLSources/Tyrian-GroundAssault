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
#if !defined( PM_MATERIALSH )
#define PM_MATERIALSH
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif


typedef enum
{
	matGlass = 0,
	matWood,
	matMetal,
	matFlesh,
	matCinderBlock,
	matCeilingTile,
	matComputer,
	matUnbreakableGlass,
	matRocks,
	matGrate,
	matVent,
	matNone,
	matLastMaterial
} Materials;

#define MATFLAG_DO_RICOCHET	1

typedef struct material_s
{
	char texture_id;// CHAR_TEX_CONCRETE
	unsigned short step_id;// STEP_CONCRETE
	unsigned short breakmat_id;// Materials struct
	const char **ShardSounds;
//	int ShardSoundsNum;
	const char **BreakSounds;
//	int BreakSoundsNum;
	const char **PushSounds;
//	int PushSoundsNum;
	const char **StepSounds;
//	int StepSoundsNum;
//	float fVolumeStep;
//	float fVolumeHit;
//	float fVolumeTool;
//	int flags;
} material_t;


#define CBTEXTURENAMEMAX	24// only load first n chars of name
#define CTEXTURESMAX		1600// max number of textures loaded

#define NUM_MATERIALS		24

#define CHAR_TEX_CONCRETE	'C'// texture types
#define CHAR_TEX_METAL		'M'
#define CHAR_TEX_DIRT		'D'
#define CHAR_TEX_VENT		'V'
#define CHAR_TEX_GRATE		'G'
#define CHAR_TEX_TILE		'T'
#define CHAR_TEX_SLOSH		'S'
#define CHAR_TEX_WOOD		'W'
#define CHAR_TEX_COMPUTER	'P'
#define CHAR_TEX_GLASS		'Y'
#define CHAR_TEX_FLESH		'F'
#define CHAR_TEX_SNOW		'N'
#define CHAR_TEX_GRASS		'A'
#define CHAR_TEX_CEILING	'E'
#define CHAR_TEX_WATER		'!'
#define CHAR_TEX_ENERGYSHIELD	'U'
//new
#define CHAR_TEX_BRICK		'B'
#define CHAR_TEX_ROCK		'R'
#define CHAR_TEX_ASPHALT	'Z'
#define CHAR_TEX_SAND		'X'
#define CHAR_TEX_SAND_ROCK	'J'
#define CHAR_TEX_ICE		'O'
#define CHAR_TEX_LEAVES		'L'
#define CHAR_TEX_EMPTY		'K'


#define STEP_CONCRETE		0	// default step sound
#define STEP_METAL			1	// metal floor
#define STEP_DIRT			2	// dirt, sand, rock
#define STEP_VENT			3	// ventillation duct
#define STEP_GRATE			4	// metal grating
#define STEP_SLOSH			5	// shallow liquid puddle
#define STEP_WADE			6	// wading in liquid
#define STEP_LADDER			7	// climbing ladder
#define STEP_SNOW			8	// snow
#define STEP_GRASS			9	// grass
#define STEP_ESHIELD		10	// energy shield

// number of sounds in array for every material
#define NUM_STEP_SOUNDS		4
#define NUM_SHARD_SOUNDS	8// actually it varies
#define NUM_PUSH_SOUNDS		3
#define NUM_BREAK_SOUNDS	2

// sound names
extern const char *gSoundsWood[];
extern const char *gSoundsFlesh[];
extern const char *gSoundsGlass[];
extern const char *gSoundsMetal[];
extern const char *gSoundsConcrete[];
extern const char *gSoundsCeiling[];

#define NUM_SPARK_SOUNDS		6
#define NUM_RICOCHET_SOUNDS		5
#define NUM_BODYDROP_SOUNDS		4

extern const char *gSoundsSparks[];
extern const char *gSoundsRicochet[];
extern const char *gSoundsShell9mm[];
extern const char *gSoundsShellShotgun[];
extern const char *gSoundsDropBody[];

extern const char *gPushSoundsGlass[];
extern const char *gPushSoundsWood[];
extern const char *gPushSoundsMetal[];
extern const char *gPushSoundsFlesh[];
extern const char *gPushSoundsConcrete[];

extern const char *gBreakSoundsGlass[];
extern const char *gBreakSoundsWood[];
extern const char *gBreakSoundsMetal[];
extern const char *gBreakSoundsFlesh[];
extern const char *gBreakSoundsConcrete[];
extern const char *gBreakSoundsCeiling[];

extern const char *gStepSoundsDefault[];
extern const char *gStepSoundsMetal[];
extern const char *gStepSoundsDirt[];
extern const char *gStepSoundsDuct[];
extern const char *gStepSoundsGrate[];
extern const char *gStepSoundsSlosh[];
extern const char *gStepSoundsWade[];
extern const char *gStepSoundsLadder[];
extern const char *gStepSoundsSnow[];
extern const char *gStepSoundsGrass[];
extern const char *gStepSoundsEshield[];

extern const char **gPushSounds[];
extern const char **gBreakSounds[];

extern const char **gStepSounds[];

extern material_t gMaterials[];


int MapTextureTypeStepType(const char &chTextureType);

#endif // !PM_MATERIALSH
