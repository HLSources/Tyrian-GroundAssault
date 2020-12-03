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
#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "pm_defs.h"
#include "pm_materials.h"
#include "eventscripts.h"
#include "r_efx.h"
#include "event_api.h"
#include "in_defs.h"
#include "shared_resources.h"
#include "cl_fx.h"
#include "weapondef.h"
#include "com_model.h"
#include "con_nprint.h"
#include "decals.h"
#include "RenderManager.h"
#include "RenderSystem.h"
#include "RSBeam.h"
#include "RSLight.h"
#include "RSSprite.h"
#include "ParticleSystem.h"
#include "PSFlameCone.h"

char PM_FindTextureType(char *name);
//void V_PunchAxis(int axis, float punch);

// play a strike sound based on the texture that was hit by the attack traceline.  VecSrc/VecEnd are the
// original traceline endpoints used by the attacker, iBulletType is the type of bullet that hit the texture.
// returns volume of strike instrument (crowbar) to play
float PlayTextureSound(char chTextureType, float *origin)
{
	if (!chTextureType)
		return 0.0f;

//	char *rgsz[4];
	float fvol = VOL_NORM;
	float fvolbar = 0.0;
	float fattn = ATTN_NORM;

	switch (chTextureType)
	{
	default:
		CON_DPRINTF("Unknown material: '%c'!\n", chTextureType);
		// use concrete sounds
	case CHAR_TEX_CONCRETE: fvol = 0.9;	fvolbar = 0.6;
		break;
	case CHAR_TEX_METAL: fvol = 0.9; fvolbar = 0.3;
		break;
	case CHAR_TEX_DIRT:	fvol = 0.9; fvolbar = 0.1;
		fattn = ATTN_STATIC;
		break;
	case CHAR_TEX_SAND:	fvol = 0.9; fvolbar = 0.1;
		fattn = ATTN_STATIC;
		break;
	case CHAR_TEX_VENT:	fvol = 0.5; fvolbar = 0.3;
		break;
	case CHAR_TEX_GRATE: fvol = 0.9; fvolbar = 0.5;
		break;
	case CHAR_TEX_TILE:	fvol = 0.8; fvolbar = 0.2;
		break;
	case CHAR_TEX_ASPHALT:	fvol = 0.8; fvolbar = 0.2;
		break;
	case CHAR_TEX_ROCK:	fvol = 0.8; fvolbar = 0.3;
		break;
	case CHAR_TEX_SAND_ROCK:	fvol = 0.8; fvolbar = 0.3;
		break;
	case CHAR_TEX_BRICK:	fvol = 0.8; fvolbar = 0.2;
		break;
	case CHAR_TEX_SLOSH: fvol = 0.9; fvolbar = 0.0;
		fattn = ATTN_STATIC;
		break;
	case CHAR_TEX_WOOD: fvol = 0.9; fvolbar = 0.2;
		break;
	case CHAR_TEX_COMPUTER: fvol = 0.7; fvolbar = 0.4;
		fattn = ATTN_NORM;
		break;
	case CHAR_TEX_GLASS: fvol = 0.8; fvolbar = 0.3;
		fattn = ATTN_NORM;
		break;
	case CHAR_TEX_ICE: fvol = 0.8; fvolbar = 0.3;
		fattn = ATTN_NORM;
		break;
	case CHAR_TEX_FLESH: fvol = 0.6; fvolbar = 0.0;
		fattn = ATTN_STATIC;
		break;
	case CHAR_TEX_SNOW: fvol = 0.1; fvolbar = 0.0;
		fattn = ATTN_IDLE;
		break;
	case CHAR_TEX_GRASS: fvol = 0.3; fvolbar = 0.1;
		fattn = ATTN_IDLE;
		chTextureType = CHAR_TEX_DIRT;// XDM3035: use dirt sounds because bullets really don't hit grass :)
		break;
	case CHAR_TEX_LEAVES: fvol = 0.3; fvolbar = 0.1;
		fattn = ATTN_STATIC;
		chTextureType = CHAR_TEX_DIRT;
		break;
	case CHAR_TEX_ENERGYSHIELD: fvol = 0.8; fvolbar = 0.0;
		fattn = ATTN_NORM;
		break;
	case CHAR_TEX_EMPTY: fvol = 0.5; fvolbar = 0.0;
		fattn = ATTN_STATIC;
		break;
	}

/* how bout these?
	for (int i=0; i<NUM_MATERIALS; ++i)
	{
		if (gMaterials[i].texture_id == chTextureType)
			EMIT_SOUND(0, origin, CHAN_STATIC, gMaterials[i].ShardSounds[RANDOM_LONG(0,NUM_SHARD_SOUNDS-1)], fvol, fattn, 0, RANDOM_LONG(96,104));
	}*/
	EMIT_SOUND(0, origin, CHAN_STATIC, gStepSounds[MapTextureTypeStepType(chTextureType)][RANDOM_LONG(0,3)], fvol, fattn, 0, RANDOM_LONG(96,104));

//	EMIT_SOUND(0, origin, CHAN_STATIC, rgsz[RANDOM_LONG(0,3)], fvol, fattn, 0, RANDOM_LONG(96,104));
//	CON_PRINTF("PlayTextureSound: %c %s vol %f att %f volbar %f\n", chTextureType, rgsz[i], fvol, fattn, fvolbar);
	return fvolbar;
}
/*
float EV_PlayTextureSound(pmtrace_t *ptr, float *vecSrc, float *vecEnd, int iBulletType)
{
	// hit the world, try to play sound based on texture material type
	char chTextureType = CHAR_TEX_CONCRETE;
	char *pTextureName;
	char texname[64];
	char szbuffer[64];
	int entity = gEngfuncs.pEventAPI->EV_IndexFromTrace(ptr);
	// FIXME check if playtexture sounds movevar is set
//	chTextureType = 0;
	if (entity >= 1 && entity <= gEngfuncs.GetMaxClients())
	{
		chTextureType = CHAR_TEX_FLESH;// hit body
	}
	else if (entity == 0)
	{
		// get texture from entity or world (world is ent(0))
		pTextureName = (char *)gEngfuncs.pEventAPI->EV_TraceTexture( ptr->ent, vecSrc, vecEnd );
		if (pTextureName)
		{
			strncpy(texname, pTextureName, 64);
			pTextureName = texname;

			// strip leading '-0' or '+0~' or '{' or '!'
			if (*pTextureName == '-' || *pTextureName == '+')
				pTextureName += 2;

			if (*pTextureName == '{' || *pTextureName == '!' || *pTextureName == '~' || *pTextureName == ' ')
				pTextureName++;

			// '}}'
			strcpy(szbuffer, pTextureName);
			szbuffer[CBTEXTURENAMEMAX - 1] = 0;
			// get texture type
			chTextureType = PM_FindTextureType(szbuffer);
		}
	}
	return PlayTextureSound(chTextureType, ptr->endpos);
}
*/

//-----------------------------------------------------------------------------
// Purpose: Pick decal index by bullet type
// Input  : iBulletType - 
//			chTextureType - 
// Output : int decal NOT an engine decal index!
//-----------------------------------------------------------------------------
int EV_DamageDecal(int iBulletType, char chTextureType)
{
	bool bigshot = false;
	if (iBulletType == BULLET_SUPERRAILGUN || iBulletType == BULLET_VULCAN || iBulletType == BULLET_MINIGUN)
		bigshot = true;

	int decal = -1;// this is NOT an engine decal index! Default: no decal.
	switch (chTextureType)
	{
	default: break;
	case CHAR_TEX_CONCRETE:
	case CHAR_TEX_METAL:
	case CHAR_TEX_DIRT:
	case CHAR_TEX_VENT:
	case CHAR_TEX_GRATE:
	case CHAR_TEX_TILE:
		{
			if (bigshot)
				decal = RANDOM_LONG(DECAL_BIGSHOT1, DECAL_BIGSHOT5);
			else
				decal = RANDOM_LONG(DECAL_GUNSHOT1, DECAL_GUNSHOT5);
		}
		break;

	case CHAR_TEX_COMPUTER:
		{
			if (bigshot)
				decal = RANDOM_LONG(DECAL_LARGESHOT1, DECAL_LARGESHOT5);
			else
				decal = RANDOM_LONG(DECAL_BIGSHOT1, DECAL_BIGSHOT5);
		}
		break;

	case CHAR_TEX_GLASS:
		{
			if (bigshot)
				decal = RANDOM_LONG(DECAL_GLASSBREAK1, DECAL_GLASSBREAK3);
			else
				decal = DECAL_BPROOF1;
		}
		break;

	case CHAR_TEX_WOOD:
		{
			if (bigshot)
				decal = RANDOM_LONG(DECAL_WOODBREAK1, DECAL_WOODBREAK3);
			else
				decal = RANDOM_LONG(DECAL_BIGSHOT1, DECAL_BIGSHOT5);
		}
		break;
	}
	return decal;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : iBulletType - 
//			chTextureType - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool EV_ShouldDoSmoke(int iBulletType, char chTextureType)
{
/* UNDONE
	for (int i=0; i<NUM_MATERIALS; ++i)
	{
		if (gMaterials[i].texture_id == chTextureType)
			return gMaterials[i].bulletsmoke;
	}
	*/
/*	bool bigshot = false;
	if (iBulletType == BULLET_SUPER_RAILGUN || iBulletType == BULLET_VULCAN || iBulletType == BULLET_MINIGUN)
		bigshot = true;
*/
	switch (chTextureType)
	{
	default: break;
	case CHAR_TEX_CONCRETE:
	case CHAR_TEX_METAL:
	case CHAR_TEX_VENT:
	case CHAR_TEX_GRATE:
	case CHAR_TEX_TILE:
	case CHAR_TEX_COMPUTER:
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: OBSOLETE
// Input  : *pTrace - 
//			decalindex - 
//-----------------------------------------------------------------------------
/*void EV_GunshotDecalTrace(pmtrace_t *pTrace, int decalindex)
{
	if (pTrace && decalindex > 0)// && CVAR_GET_FLOAT("r_decals") > 0)
	{
		int entindex = gEngfuncs.pEventAPI->EV_IndexFromTrace(pTrace);// XDM3035: compatibility fix
		if (entindex >= 0)// XDM3035c
		{
			cl_entity_t *pEnt = gEngfuncs.GetEntityByIndex(entindex);
			if (pEnt)
			{
				if (pEnt->model->type == mod_brush)
				{
//		physent_t *pe = gEngfuncs.pEventAPI->EV_GetPhysent(pTrace->ent);
		// Only decal brush models such as the world etc.
//		if (pe && (pe->solid == SOLID_BSP || pe->movetype == MOVETYPE_PUSHSTEP))
					gEngfuncs.pEfxAPI->R_DecalShoot(gEngfuncs.pEfxAPI->Draw_DecalIndex(decalindex), entindex, 0, pTrace->endpos, 0);
				}
			}
		}
	}
}*/

