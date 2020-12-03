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

#ifndef MAPRULES_H
#define MAPRULES_H

#define SF_WORLD_DARK		0x0001		// Fade from black at startup
#define SF_WORLD_TITLE		0x0002		// Display game title at startup
#define SF_WORLD_FORCETEAM	0x0004		// Force teams
#define SF_WORLD_STARTSUIT	0x0008		// XDM: start with HEV suit on
#define SF_WORLD_NOMONSTERS	0x0010		// XDM: does the map allow monsters?

BOOL IsSpawnPointValid(CBaseEntity *pPlayer, CBaseEntity *pSpot);
void SpawnPointInitialize(void);
CBaseEntity *SpawnPointEntSelect(CBaseEntity *pPlayer);
CBaseEntity *SpawnPointEntSelectSpectator(CBaseEntity *pLast);

extern CBaseEntity *g_pLastSpawn;

#endif		// MAPRULES_H
