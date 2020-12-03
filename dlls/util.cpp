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
/*
===== util.cpp ========================================================
  Utility code.  Really not optional after all.
*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "shake.h"
#include "decals.h"
#include "player.h"
#include "weapons.h"
#include "gamerules.h"
#include "colors.h"// XDM
#include "globals.h"
#include "game.h"
#include "sound.h"
#include "msg_fx.h"
#include <assert.h>


#ifdef _DEBUG
bool DBG_AssertFunction(bool fExpr, const char *szExpr, const char *szFile, int szLine, const char *szMessage)
{
	if (fExpr)
		return true;

	if (szMessage != NULL)
		ALERT(at_console, "SV: ASSERT FAILED: %s (%s@%d)\n%s", szExpr, szFile, szLine, szMessage);
	else
		ALERT(at_console, "SV: ASSERT FAILED: %s (%s@%d)\n", szExpr, szFile, szLine);

	if (IS_DEDICATED_SERVER() == 0 && g_pdeveloper && g_pdeveloper->value > 2.0f)// only bring up the "abort retry ignore" dialog if in debug mode!
		_assert((void *)szExpr, (void *)szFile, szLine);

	return fExpr;
}
#else// XDM: ASSERT function for release build
bool NDB_AssertFunction(bool fExpr, const char *szExpr, const char *szMessage)
{
	if (fExpr)
		return true;

	if (szMessage != NULL)
		ALERT(at_console, "SV: ASSERT FAILED: %s %s\n", szExpr, szMessage);
	else
		ALERT(at_console, "SV: ASSERT FAILED: %s\n", szExpr);

	return fExpr;
}
#endif	// DEBUG



float UTIL_WeaponTimeBase(void)
{
	return gpGlobals->time;
}

static unsigned int glSeed = 0; 

unsigned int seed_table[256] =
{
	28985, 27138, 26457, 9451, 17764, 10909, 28790, 8716, 6361, 4853, 17798, 21977, 19643, 20662, 10834, 20103,
	27067, 28634, 18623, 25849, 8576, 26234, 23887, 18228, 32587, 4836, 3306, 1811, 3035, 24559, 18399, 315,
	26766, 907, 24102, 12370, 9674, 2972, 10472, 16492, 22683, 11529, 27968, 30406, 13213, 2319, 23620, 16823,
	10013, 23772, 21567, 1251, 19579, 20313, 18241, 30130, 8402, 20807, 27354, 7169, 21211, 17293, 5410, 19223,
	10255, 22480, 27388, 9946, 15628, 24389, 17308, 2370, 9530, 31683, 25927, 23567, 11694, 26397, 32602, 15031,
	18255, 17582, 1422, 28835, 23607, 12597, 20602, 10138, 5212, 1252, 10074, 23166, 19823, 31667, 5902, 24630,
	18948, 14330, 14950, 8939, 23540, 21311, 22428, 22391, 3583, 29004, 30498, 18714, 4278, 2437, 22430, 3439,
	28313, 23161, 25396, 13471, 19324, 15287, 2563, 18901, 13103, 16867, 9714, 14322, 15197, 26889, 19372, 26241,
	31925, 14640, 11497, 8941, 10056, 6451, 28656, 10737, 13874, 17356, 8281, 25937, 1661, 4850, 7448, 12744,
	21826, 5477, 10167, 16705, 26897, 8839, 30947, 27978, 27283, 24685, 32298, 3525, 12398, 28726, 9475, 10208,
	617, 13467, 22287, 2376, 6097, 26312, 2974, 9114, 21787, 28010, 4725, 15387, 3274, 10762, 31695, 17320,
	18324, 12441, 16801, 27376, 22464, 7500, 5666, 18144, 15314, 31914, 31627, 6495, 5226, 31203, 2331, 4668,
	12650, 18275, 351, 7268, 31319, 30119, 7600, 2905, 13826, 11343, 13053, 15583, 30055, 31093, 5067, 761,
	9685, 11070, 21369, 27155, 3663, 26542, 20169, 12161, 15411, 30401, 7580, 31784, 8985, 29367, 20989, 14203,
	29694, 21167, 10337, 1706, 28578, 887, 3373, 19477, 14382, 675, 7033, 15111, 26138, 12252, 30996, 21409,
	25678, 18555, 13256, 23316, 22407, 16727, 991, 9236, 5373, 29402, 6117, 15241, 27715, 19291, 19888, 19847
};

unsigned int U_Random(void) 
{ 
	glSeed *= 69069; 
	glSeed += seed_table[glSeed & 0xff];
 	return (++glSeed & 0x0fffffff);
} 

void U_Srand(const unsigned int &seed)
{
	glSeed = seed_table[seed & 0xff];
}

/*
=====================
UTIL_SharedRandomLong
=====================
*/
int UTIL_SharedRandomLong(const unsigned int &seed, const int &low, const int &high)
{
	U_Srand((int)seed + low + high);
	unsigned int range = high - low + 1;
	if (!(range - 1))
	{
		return low;
	}
	else
	{
		int rnum = U_Random();
		int offset = rnum % range;
		return (low + offset);
	}
}

/*
=====================
UTIL_SharedRandomFloat
=====================
*/
float UTIL_SharedRandomFloat(const unsigned int &seed, const float &low, const float &high)
{
	U_Srand((int)seed + *(int *)&low + *(int *)&high);
	U_Random();
	U_Random();

	float range = high - low;// XDM3035c: TESTME
	if (range == 0.0f)
	{
		return low;
	}
	else
	{
		int tensixrand = U_Random() & 65535;
		float offset = (float)tensixrand / 65536.0f;
		return (low + offset * range);
	}
}
/*
void UTIL_ParametricRocket(entvars_t *pev, Vector vecOrigin, Vector vecAngles, edict_t *owner)
{	
	pev->startpos = vecOrigin;
	// Trace out line to end pos
	TraceResult tr;
	UTIL_MakeVectors(vecAngles);
	UTIL_TraceLine(pev->startpos, pev->startpos + gpGlobals->v_forward * 8192, ignore_monsters, owner, &tr);
	pev->endpos = tr.vecEndPos;

	// Now compute how long it will take based on current velocity
	Vector vecTravel = pev->endpos - pev->startpos;
	float travelTime = 0.0;
	if (pev->velocity.Length() > 0)
	{
		travelTime = vecTravel.Length() / pev->velocity.Length();
	}
	pev->starttime = gpGlobals->time;
	pev->impacttime = gpGlobals->time + travelTime;
}
*/
int g_groupmask = 0;
int g_groupop = 0;

// Normal overrides
void UTIL_SetGroupTrace(const int &groupmask, const int &op)
{
	g_groupmask		= groupmask;
	g_groupop		= op;

	ENGINE_SETGROUPMASK(g_groupmask, g_groupop);
}

void UTIL_UnsetGroupTrace(void)
{
	g_groupmask		= 0;
	g_groupop		= 0;

	ENGINE_SETGROUPMASK(0, 0);
}

// Smart version, it'll clean itself up when it pops off stack
UTIL_GroupTrace::UTIL_GroupTrace(int groupmask, int op)
{
	m_oldgroupmask	= g_groupmask;
	m_oldgroupop	= g_groupop;

	g_groupmask		= groupmask;
	g_groupop		= op;

	ENGINE_SETGROUPMASK(g_groupmask, g_groupop);
}

UTIL_GroupTrace::~UTIL_GroupTrace(void)
{
	g_groupmask		=	m_oldgroupmask;
	g_groupop		=	m_oldgroupop;

	ENGINE_SETGROUPMASK(g_groupmask, g_groupop);
}


#ifdef _DEBUG
edict_t *DBG_EntOfVars(const entvars_t *pev)
{
	if (pev->pContainingEntity != NULL)
		return pev->pContainingEntity;

	ALERT(at_console, "entvars_t pContainingEntity is NULL, calling into engine\n");
	DBG_FORCEBREAK
	edict_t *pent = (*g_engfuncs.pfnFindEntityByVars)((entvars_t *)pev);
	if (pent == NULL)
		ALERT(at_console, "WARNING! FindEntityByVars failed!\n");

	((entvars_t *)pev)->pContainingEntity = pent;
	return pent;
}
#endif // _DEBUG


// ripped this out of the engine
float UTIL_AngleMod(float a)
{
	if (a < 0.0f)
	{
		a = a + 360.0f * ((int)(a / 360.0f) + 1);
	}
	else if (a >= 360.0f)
	{
		a = a - 360.0f * ((int)(a / 360.0f));
	}
	// a = (360.0/65536) * ((int)(a*(65536/360.0)) & 65535);
	return a;
}

Vector UTIL_VecToAngles(const Vector &vec)
{
	float rgflVecOut[3];
	VEC_TO_ANGLES(vec, rgflVecOut);
	return Vector(rgflVecOut);
}

//	float UTIL_MoveToOrigin(edict_t *pent, const Vector vecGoal, float flDist, int iMoveType)
void UTIL_MoveToOrigin(edict_t *pent, const Vector &vecGoal, float flDist, int iMoveType)
{
	float rgfl[3];
	vecGoal.CopyToArray(rgfl);
//		return MOVE_TO_ORIGIN (pent, rgfl, flDist, iMoveType); 
	MOVE_TO_ORIGIN (pent, rgfl, flDist, iMoveType); 
}

int UTIL_EntitiesInBox(CBaseEntity **pList, int listMax, const Vector &mins, const Vector &maxs, int flagMask)
{
	edict_t		*pEdict = INDEXENT(1);
	CBaseEntity *pEntity;
	int			count = 0;

	if (!pEdict)
		return count;

	for (int i = 1; i < gpGlobals->maxEntities; ++i, pEdict++)
	{
		if (!UTIL_IsValidEntity(pEdict))
			continue;

		if (flagMask && !(pEdict->v.flags & flagMask))	// Does it meet the criteria?
			continue;

		if (mins.x > pEdict->v.absmax.x ||
			 mins.y > pEdict->v.absmax.y ||
			 mins.z > pEdict->v.absmax.z ||
			 maxs.x < pEdict->v.absmin.x ||
			 maxs.y < pEdict->v.absmin.y ||
			 maxs.z < pEdict->v.absmin.z)
			 continue;

		pEntity = CBaseEntity::Instance(pEdict);
		if (!pEntity)
			continue;

		pList[ count ] = pEntity;
		++count;

		if (count >= listMax)
			return count;
	}
	return count;
}

/*int UTIL_MonstersInSphere(CBaseEntity **pList, int listMax, const Vector &center, float radius)
{
	edict_t		*pEdict = INDEXENT(1);
	CBaseEntity *pEntity;
	int			count;
	float		distance, delta;

	count = 0;
	float radiusSquared = radius * radius;

	if (!pEdict)
		return count;

	for (int i = 1; i < gpGlobals->maxEntities; ++i, pEdict++)
	{
		if (!UTIL_IsValidEntity(pEdict))
			continue;

		if (!(pEdict->v.flags & (FL_CLIENT|FL_MONSTER)))	// Not a client/monster ?
			continue;

		// Use origin for X & Y since they are centered for all monsters
		// Now X
		delta = center.x - pEdict->v.origin.x;//(pEdict->v.absmin.x + pEdict->v.absmax.x)*0.5;
		delta *= delta;

		if (delta > radiusSquared)
			continue;
		distance = delta;

		// Now Y
		delta = center.y - pEdict->v.origin.y;//(pEdict->v.absmin.y + pEdict->v.absmax.y)*0.5;
		delta *= delta;

		distance += delta;
		if (distance > radiusSquared)
			continue;

		// Now Z
		delta = center.z - (pEdict->v.absmin.z + pEdict->v.absmax.z)*0.5f;
		delta *= delta;

		distance += delta;
		if (distance > radiusSquared)
			continue;

		pEntity = CBaseEntity::Instance(pEdict);
		if (!pEntity)
			continue;

		pList[ count ] = pEntity;
		count++;

		if (count >= listMax)
			return count;
	}
	return count;
}*/

CBaseEntity *UTIL_FindEntityInBox(CBaseEntity *pStartEntity, const Vector &mins, const Vector &maxs)
{
	int startindex;
	if (pStartEntity)
		startindex = pStartEntity->entindex()+1;
	else
		startindex = 0;

	int i;
	edict_t *pEdict;
	for (i = startindex; i < gpGlobals->maxEntities; ++i, pEdict++)
	{
		pEdict = INDEXENT(i);

		if (!UTIL_IsValidEntity(pEdict))
			continue;

		if (mins.x > pEdict->v.absmax.x ||
			mins.y > pEdict->v.absmax.y ||
			mins.z > pEdict->v.absmax.z ||
			maxs.x < pEdict->v.absmin.x ||
			maxs.y < pEdict->v.absmin.y ||
			maxs.z < pEdict->v.absmin.z)
			continue;

		return CBaseEntity::Instance(pEdict);
	}

/* cyclic algorithm which wraps across maxEntities. not what we need.
	edict_t *pEdict = INDEXENT(startindex);
	int i = startindex;
	int c;
	for (c = 0; c < gpGlobals->maxEntities; ++c, ++i, pEdict++)
	{
		if (i >= gpGlobals->maxEntities)// link the circle
			i = 0;

		if (i == startindex)
			break;//return NULL;

		pEdict = INDEXENT(i);

		if (!UTIL_IsValidEntity(pEdict))
			continue;

		if (mins.x > pEdict->v.absmax.x ||
			mins.y > pEdict->v.absmax.y ||
			mins.z > pEdict->v.absmax.z ||
			maxs.x < pEdict->v.absmin.x ||
			maxs.y < pEdict->v.absmin.y ||
			maxs.z < pEdict->v.absmin.z)
			continue;

		return CBaseEntity::Instance(pentEntity);
	}*/
	return NULL;
}

CBaseEntity *UTIL_FindEntityInSphere(CBaseEntity *pStartEntity, const Vector &vecCenter, float flRadius)
{
	edict_t	*pentEntity;

	if (pStartEntity)
		pentEntity = pStartEntity->edict();
	else
		pentEntity = NULL;

	pentEntity = FIND_ENTITY_IN_SPHERE(pentEntity, vecCenter, flRadius);

	if (!FNullEnt(pentEntity))
		return CBaseEntity::Instance(pentEntity);
	return NULL;
}

CBaseEntity *UTIL_FindEntityByString(CBaseEntity *pStartEntity, const char *szKeyword, const char *szValue)
{
	edict_t	*pentEntity;

	if (pStartEntity)
		pentEntity = pStartEntity->edict();
	else
		pentEntity = NULL;

	pentEntity = FIND_ENTITY_BY_STRING(pentEntity, szKeyword, szValue);

	if (!FNullEnt(pentEntity))
		return CBaseEntity::Instance(pentEntity);

	return NULL;
}

CBaseEntity *UTIL_FindEntityByClassname(CBaseEntity *pStartEntity, const char *szName)
{
	return UTIL_FindEntityByString(pStartEntity, "classname", szName);
}

CBaseEntity *UTIL_FindEntityByTargetname(CBaseEntity *pStartEntity, const char *szName)
{
	return UTIL_FindEntityByString(pStartEntity, "targetname", szName);
}

// Spirit of Half-Life compatibility
CBaseEntity *UTIL_FindEntityByTargetname(CBaseEntity *pStartEntity, const char *szName, CBaseEntity *pActivator)
{
	if (FStrEq(szName, "*locus"))
	{
		if (pActivator && (pStartEntity == NULL || pActivator->eoffset() > pStartEntity->eoffset()))
			return pActivator;
		else
			return NULL;
	}
	else 
		return UTIL_FindEntityByTargetname(pStartEntity, szName);
}

CBaseEntity *UTIL_FindEntityByTarget(CBaseEntity *pStartEntity, const char *szName)
{
	return UTIL_FindEntityByString(pStartEntity, "target", szName);
}

//-----------------------------------------------------------------------------
// Purpose: The universal version
// Input  : *pStartEntity - 
//			*szKeyword - 
//			*szValue - 
//			&vecCenter - 
//			flRadius - 
//-----------------------------------------------------------------------------
CBaseEntity	*UTIL_FindEntities(CBaseEntity *pStartEntity, const char *szKeyword, const char *szValue, const Vector &vecCenter, float flRadius)
{
	CBaseEntity *pEntity = pStartEntity;
	CBaseEntity *pFound = NULL;
	while ((pEntity = UTIL_FindEntityByString(pEntity, szKeyword, szValue)) != NULL)
	{
		if (flRadius > 0.0f)// limit by radius
		{
			float flDist = (pEntity->pev->origin - vecCenter).Length();
			if (flDist <= flRadius)
			{
				pFound = pEntity;
				flDist = flRadius;// limit more
			}
		}
		else// first found is ok
		{
			pFound = pEntity;
			break;
		}
	}
	return pFound;
}

void UTIL_MakeAimVectors(const Vector &vecAngles)
{
	float rgflVec[3];
	vecAngles.CopyToArray(rgflVec);
	rgflVec[0] = -rgflVec[0];
	MAKE_VECTORS(rgflVec);
}

void UTIL_MakeInvVectors(const Vector &vec, globalvars_t *pgv)
{
	MAKE_VECTORS(vec);
	float tmp;
	pgv->v_right = pgv->v_right * -1;
	SWAP(pgv->v_forward.y, pgv->v_right.x, tmp);
	SWAP(pgv->v_forward.z, pgv->v_up.x, tmp);
	SWAP(pgv->v_right.z, pgv->v_up.y, tmp);
}

// UNDONE: should be inline... anyway, currently only one and a half entities use it
void UTIL_SetOrigin(CBaseEntity *pEntity, const Vector &vecOrigin)
{
	SET_ORIGIN(ENT(pEntity->pev), vecOrigin);
}

// UNDONE: same here
void UTIL_SetAngles(CBaseEntity *pEntity, const Vector &vecAngles)
{
	pEntity->pev->angles = vecAngles;
}

void UTIL_EmitAmbientSound(edict_t *entity, const Vector &vecOrigin, const char *samp, float vol, float attenuation, int fFlags, int pitch)
{
	float rgfl[3];
	vecOrigin.CopyToArray(rgfl);
	if (samp && *samp == '!')// replace sentence name with index
	{
		char sentence_id[32];
		if (SENTENCEG_Lookup(samp, sentence_id) >= 0)
			EMIT_AMBIENT_SOUND(entity, rgfl, sentence_id, vol, attenuation, fFlags, pitch);
	}
	else
		EMIT_AMBIENT_SOUND(entity, rgfl, samp, vol, attenuation, fFlags, pitch);
}

static unsigned short FixedUnsigned16(float value, float scale)
{
	int output = value * scale;
	if (output < 0)
		output = 0;
	if (output > 0xFFFF)
		output = 0xFFFF;

	return (unsigned short)output;
}

static short FixedSigned16(float value, float scale)
{
	int output = value * scale;

	if (output > 32767)
		output = 32767;

	if (output < -32768)
		output = -32768;

	return (short)output;
}

void UTIL_ScreenShakeOne(CBaseEntity *pPlayer, const Vector &center, float amplitude, float frequency, float duration)
{
	ScreenShake	shake;
	shake.duration = FixedUnsigned16(duration, 1<<12);// 4.12 fixed
	shake.frequency = FixedUnsigned16(frequency, 1<<8);// 8.8 fixed
	shake.amplitude = FixedUnsigned16(amplitude, 1<<12);// 4.12 fixed
	MESSAGE_BEGIN(MSG_ONE, gmsgShake, center, pPlayer->edict());
		WRITE_SHORT(shake.amplitude);				// shake amount
		WRITE_SHORT(shake.duration);				// shake lasts this long
		WRITE_SHORT(shake.frequency);				// shake noise frequency
	MESSAGE_END();
}

// Shake the screen of all clients within radius
// radius == 0, shake all clients
// XDM: fixed
// UNDONE: Fix falloff model (disabled)?
// UNDONE: Affect user controls?
void UTIL_ScreenShake(const Vector &center, float amplitude, float frequency, float duration, float radius)
{
	int			i;
	float		localAmplitude;
	ScreenShake	shake;

	shake.duration = FixedUnsigned16(duration, 1<<12);		// 4.12 fixed
	shake.frequency = FixedUnsigned16(frequency, 1<<8);	// 8.8 fixed

	for (i = 1; i <= gpGlobals->maxClients; ++i)
	{
		CBasePlayer *pPlayer = UTIL_ClientByIndex(i);
		if (pPlayer == NULL)
			continue;

		localAmplitude = 0;

		if (radius <= 0)
			localAmplitude = amplitude;
		else
		{
			Vector delta = center - pPlayer->pev->origin;
			float distance = delta.Length();

			// Had to get rid of this falloff - it didn't work well
			if (distance < radius)
				localAmplitude = amplitude;//radius - distance;
		}
		if (localAmplitude)
		{
			if (!(pPlayer->pev->flags & FL_ONGROUND))// XDM: FIXed: not on ground?
				localAmplitude *= 0.5f;

			shake.amplitude = FixedUnsigned16(localAmplitude, 1<<12);		// 4.12 fixed

			MESSAGE_BEGIN(MSG_ONE, gmsgShake, NULL, pPlayer->edict());		// use the magic #1 for "one client"
				WRITE_SHORT(shake.amplitude);				// shake amount
				WRITE_SHORT(shake.duration);				// shake lasts this long
				WRITE_SHORT(shake.frequency);				// shake noise frequency
			MESSAGE_END();
//			UTIL_ScreenShakeOne(pPlayer, center, localAmplitude, duration, frequency);// XDM: change method
		}
	}
}

void UTIL_ScreenShakeAll(const Vector &center, float amplitude, float frequency, float duration)
{
	UTIL_ScreenShake(center, amplitude, frequency, duration, 0);
}

void UTIL_ScreenFadeBuild(ScreenFade &fade, const Vector &color, float fadeTime, float fadeHold, int alpha, int flags)
{
	fade.duration = FixedUnsigned16(fadeTime, 1<<12);		// 4.12 fixed
	fade.holdTime = FixedUnsigned16(fadeHold, 1<<12);		// 4.12 fixed
	fade.r = (int)color.x;
	fade.g = (int)color.y;
	fade.b = (int)color.z;
	fade.a = alpha;
	fade.fadeFlags = flags;
}

void UTIL_ScreenFadeWrite(const ScreenFade &fade, CBaseEntity *pEntity)
{
	if (!pEntity || !pEntity->IsNetClient())
		return;

	MESSAGE_BEGIN(MSG_ONE, gmsgFade, NULL, pEntity->edict());		// use the magic #1 for "one client"
		WRITE_SHORT(fade.duration);		// fade lasts this long
		WRITE_SHORT(fade.holdTime);		// fade lasts this long
		WRITE_SHORT(fade.fadeFlags);		// fade type (in / out)
		WRITE_BYTE(fade.r);				// fade red
		WRITE_BYTE(fade.g);				// fade green
		WRITE_BYTE(fade.b);				// fade blue
		WRITE_BYTE(fade.a);				// fade blue
	MESSAGE_END();
}

void UTIL_ScreenFade(CBaseEntity *pEntity, const Vector &color, float fadeTime, float fadeHold, int alpha, int flags)
{
	ScreenFade	fade;
	UTIL_ScreenFadeBuild(fade, color, fadeTime, fadeHold, alpha, flags);
	UTIL_ScreenFadeWrite(fade, pEntity);
}

void UTIL_ScreenFadeAll(const Vector &color, float fadeTime, float fadeHold, int alpha, int flags)
{
	int			i;
	ScreenFade	fade;
	UTIL_ScreenFadeBuild(fade, color, fadeTime, fadeHold, alpha, flags);
	for (i = 1; i <= gpGlobals->maxClients; ++i)
	{
		CBasePlayer *pPlayer = UTIL_ClientByIndex(i);
		UTIL_ScreenFadeWrite(fade, pPlayer);
	}
}

void UTIL_HudMessage(CBaseEntity *pEntity, const hudtextparms_t &textparms, const char *pMessage)
{
	if (!pEntity || !pEntity->IsNetClient())
		return;

	MESSAGE_BEGIN(MSG_ONE, SVC_TEMPENTITY, NULL, pEntity->edict());
		WRITE_BYTE(TE_TEXTMESSAGE);
		WRITE_BYTE(textparms.channel & 0xFF);
		WRITE_SHORT(FixedSigned16(textparms.x, 1<<13));
		WRITE_SHORT(FixedSigned16(textparms.y, 1<<13));
		WRITE_BYTE(textparms.effect);
		WRITE_BYTE(textparms.r1);
		WRITE_BYTE(textparms.g1);
		WRITE_BYTE(textparms.b1);
		WRITE_BYTE(textparms.a1);
		WRITE_BYTE(textparms.r2);
		WRITE_BYTE(textparms.g2);
		WRITE_BYTE(textparms.b2);
		WRITE_BYTE(textparms.a2);
		WRITE_SHORT(FixedUnsigned16(textparms.fadeinTime, 1<<8));
		WRITE_SHORT(FixedUnsigned16(textparms.fadeoutTime, 1<<8));
		WRITE_SHORT(FixedUnsigned16(textparms.holdTime, 1<<8));

		if (textparms.effect == 2)
			WRITE_SHORT(FixedUnsigned16(textparms.fxTime, 1<<8));
		
		if (strlen(pMessage) < MAX_MESSAGE_STRING)
		{
			WRITE_STRING(pMessage);
		}
		else
		{
			char tmp[MAX_MESSAGE_STRING];
			strncpy(tmp, pMessage, MAX_MESSAGE_STRING-1);
			tmp[MAX_MESSAGE_STRING-1] = 0;
			WRITE_STRING(tmp);
		}
	MESSAGE_END();
}

void UTIL_HudMessageAll(const hudtextparms_t &textparms, const char *pMessage)
{
	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		CBasePlayer *pPlayer = UTIL_ClientByIndex(i);
		if (pPlayer)
			UTIL_HudMessage(pPlayer, textparms, pMessage);
	}
}

void ClientPrint(entvars_t *client, int msg_dest, const char *msg_name, const char *param1, const char *param2, const char *param3, const char *param4)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgTextMsg, NULL, ENT(client));
		WRITE_BYTE(msg_dest);
		WRITE_STRING(msg_name);
		if (param1)
			WRITE_STRING(param1);
		if (param2)
			WRITE_STRING(param2);
		if (param3)
			WRITE_STRING(param3);
		if (param4)
			WRITE_STRING(param4);

	MESSAGE_END();
}

void UTIL_ClientPrintAll(int msg_dest, const char *msg_name, const char *param1, const char *param2, const char *param3, const char *param4)
{
	MESSAGE_BEGIN(MSG_ALL, gmsgTextMsg);
		WRITE_BYTE(msg_dest);
		WRITE_STRING(msg_name);
		if (param1)
			WRITE_STRING(param1);
		if (param2)
			WRITE_STRING(param2);
		if (param3)
			WRITE_STRING(param3);
		if (param4)
			WRITE_STRING(param4);

	MESSAGE_END();
}

/*void UTIL_SayText(const char *pText, CBaseEntity *pEntity, bool reliable)
{
	if (!pEntity->IsNetClient())
		return;

	MESSAGE_BEGIN(reliable?MSG_ONE:MSG_ONE_UNRELIABLE, gmsgSayText, NULL, pEntity->edict());
		WRITE_BYTE(pEntity->entindex());
		WRITE_STRING(pText);
	MESSAGE_END();
}

void UTIL_SayTextAll(const char *pText, CBaseEntity *pEntity, bool reliable)
{
	MESSAGE_BEGIN(reliable?MSG_ALL:MSG_BROADCAST, gmsgSayText, NULL);
		WRITE_BYTE(pEntity?pEntity->entindex():0);// XDM
		WRITE_STRING(pText);
	MESSAGE_END();
}*/

void UTIL_ShowMessage(const char *pString, CBaseEntity *pEntity)
{
	if (!pEntity || !pEntity->IsNetClient() || pEntity->IsBot())
		return;

	MESSAGE_BEGIN(MSG_ONE, gmsgHudText, NULL, pEntity->edict());
		WRITE_STRING(pString);
	MESSAGE_END();
}

void UTIL_ShowMessageAll(const char *pString)
{
	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		CBasePlayer *pPlayer = UTIL_ClientByIndex(i);
		if (pPlayer)
			UTIL_ShowMessage(pString, pPlayer);
	}
}

char *UTIL_dtos1(int d)
{
	static char buf[8];
	sprintf(buf, "%d", d);
	return buf;
}
/*
char *UTIL_dtos2(int d)
{
	static char buf[8];
	sprintf(buf, "%d", d);
	return buf;
}

char *UTIL_dtos3(int d)
{
	static char buf[8];
	sprintf(buf, "%d", d);
	return buf;
}

char *UTIL_dtos4(int d)
{
	static char buf[8];
	sprintf(buf, "%d", d);
	return buf;
}
*/

//=========================================================
// UTIL_LogPrintf - Prints a logged message to console.
// Preceded by LOG: (timestamp) < message >
//=========================================================
void UTIL_LogPrintf(char *fmt, ...)
{
	va_list			argptr;
	static char		string[1024];
	va_start (argptr, fmt);
	vsprintf (string, fmt, argptr);
	va_end   (argptr);
	// Print to server console
	ALERT(at_logged, "%s", string);
}

// Overloaded to add IGNORE_GLASS
void UTIL_TraceLine(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, IGNORE_GLASS ignoreGlass, edict_t *pentIgnore, TraceResult *ptr)
{
	TRACE_LINE(vecStart, vecEnd, (igmon == ignore_monsters ? TRUE : FALSE) | (ignoreGlass?0x100:0), pentIgnore, ptr);
}

void UTIL_TraceLine(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, edict_t *pentIgnore, TraceResult *ptr)
{
	TRACE_LINE(vecStart, vecEnd, (igmon == ignore_monsters ? TRUE : FALSE), pentIgnore, ptr);
}

void UTIL_TraceHull(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, int hullNumber, edict_t *pentIgnore, TraceResult *ptr)
{
	TRACE_HULL(vecStart, vecEnd, (igmon == ignore_monsters ? TRUE : FALSE), hullNumber, pentIgnore, ptr);
}

void UTIL_TraceModel(const Vector &vecStart, const Vector &vecEnd, int hullNumber, edict_t *pentModel, TraceResult *ptr)
{
	TRACE_MODEL(vecStart, vecEnd, hullNumber, pentModel, ptr);
}

TraceResult UTIL_GetGlobalTrace(void)
{
	static TraceResult tr;// XDM3035c: must be kept until next call!
	tr.fAllSolid		= gpGlobals->trace_allsolid;
	tr.fStartSolid		= gpGlobals->trace_startsolid;
	tr.fInOpen			= gpGlobals->trace_inopen;
	tr.fInWater			= gpGlobals->trace_inwater;
	tr.flFraction		= gpGlobals->trace_fraction;
	tr.flPlaneDist		= gpGlobals->trace_plane_dist;
	tr.pHit			= gpGlobals->trace_ent;
	tr.vecEndPos		= gpGlobals->trace_endpos;
	tr.vecPlaneNormal	= gpGlobals->trace_plane_normal;
	tr.iHitgroup		= gpGlobals->trace_hitgroup;
	return tr;
}

void UTIL_SetSize(entvars_t *pev, const Vector &vecMin, const Vector &vecMax)
{
	SET_SIZE(ENT(pev), vecMin, vecMax);
}

void UTIL_SetSize(CBaseEntity *pEntity, const Vector &vecMin, const Vector &vecMax)// XDM
{
	if (pEntity)
		SET_SIZE(pEntity->edict(), vecMin, vecMax);
}

void UTIL_SetSize(CBaseEntity *pEntity, const float &radius)// XDM
{
	if (pEntity)
		SET_SIZE(pEntity->edict(), Vector(-radius, -radius, -radius), Vector(radius, radius, radius));
}

float UTIL_VecToYaw(const Vector &vec)
{
	return VEC_TO_YAW(vec);
}
/*
void UTIL_SetOrigin(entvars_t *pev, const Vector &vecOrigin)
{
	SET_ORIGIN(ENT(pev), vecOrigin);
}
*/

float UTIL_Approach(const float &target, float value, const float &speed)
{
	float delta = target - value;
	if (delta > speed)
		value += speed;
	else if (delta < -speed)
		value -= speed;
	else 
		value = target;

	return value;
}

float UTIL_ApproachAngle(float target, float value, float speed)
{
	target = UTIL_AngleMod(target);
	value = UTIL_AngleMod(target);

	float delta = target - value;

	// Speed is assumed to be positive
	if (speed < 0)
		speed = -speed;

	if (delta < -180)
		delta += 360;
	else if (delta > 180)
		delta -= 360;

	if (delta > speed)
		value += speed;
	else if (delta < -speed)
		value -= speed;
	else 
		value = target;

	return value;
}

float UTIL_AngleDistance(const float &next, const float &cur)
{
	float delta = next - cur;

	if (delta < -180)
		delta += 360;
	else if (delta > 180)
		delta -= 360;

	return delta;
}

float UTIL_SplineFraction(float value, const float &scale)
{
	value = scale * value;
	float valueSquared = value * value;
	// Nice little ease-in, ease-out spline-like curve
	return 3 * valueSquared - 2 * valueSquared * value;
}

char *UTIL_VarArgs(char *format, ...)
{
	va_list		argptr;
	static char		string[1024];
	va_start (argptr, format);
	vsprintf (string, format,argptr);
	va_end (argptr);
	return string;	
}

Vector UTIL_GetAimVector(edict_t *pent, float flSpeed)
{
	Vector tmp;
	GET_AIM_VECTOR(pent, flSpeed, tmp);
	return tmp;
}

BOOL UTIL_IsMasterTriggered(string_t iszMaster, CBaseEntity *pActivator)
{
	if (FStringNull(iszMaster))
		return TRUE;

	int i, j;
	const char *szMaster;
	char szBuf[80];
	CBaseEntity *pMaster;
	bool found = false;
	bool reverse = false;

//	if (pActivator == NULL)// XDM3035a
//		return TRUE;// TRUE?!!

//	ALERT(at_console, "IsMasterTriggered(%s, %s \"%s\")\n", STRING(iszMaster), STRING(pActivator->pev->classname), STRING(pActivator->pev->targetname));
	szMaster = STRING(iszMaster);
	if (szMaster[0] == '~') //inverse master
	{
		reverse = true;
		szMaster++;
	}

	pMaster = UTIL_FindEntityByTargetname(NULL, szMaster);
	if (!pMaster)
	{
		for (i = 0; szMaster[i]; ++i)
		{
			if (szMaster[i] == '(')
			{
				for (j = i+1; szMaster[j]; ++j)
				{
					if (szMaster[j] == ')')
					{
						strncpy(szBuf, szMaster+i+1, (j-i)-1);
						szBuf[(j-i)-1] = 0;
						/*pActivator*/pMaster = UTIL_FindEntityByTargetname(NULL, szBuf);// XDM3035a: TODO: was this wrong?!?!
						found = true;
						break;
					}
				}
				if (!found) // no) found
				{
					ALERT(at_error, "Missing ')' in master \"%s\"\n", szMaster);
					return FALSE;
				}
				break;
			}
		}
		if (!found) // no (found
		{
			ALERT(at_console, "Master \"%s\" not found!\n",szMaster);
			return TRUE;
		}

		strncpy(szBuf, szMaster, i);
		szBuf[i] = 0;
		pMaster = UTIL_FindEntityByTargetname(NULL, szBuf);
	}

	if (pMaster)
	{
		if (reverse)
			return !pMaster->IsTriggered(pActivator);
		else
			return pMaster->IsTriggered(pActivator);

//			ALERT(at_console, "Master was null or not a master!\n");
	}
	// if this isn't a master entity, just say yes.
	return TRUE;
}

bool UTIL_ShouldShowBlood(const int &color)
{
	if (color != DONT_BLEED)
	{
		if (g_pGameRules->IsMultiplayer())// XDM3035c: nobody cares these days. Performance!
		{
//			return (g_pGameRules->FAllowEffects())
			return true;
		}
		if (color == BLOOD_COLOR_RED)
		{
				return true;
		}
		else
		{
				return true;
		}
	}
	return false;
}

void UTIL_ParticleEffect(const Vector &vecOrigin, const Vector &vecDirection, ULONG ulColor, ULONG ulCount)
{
	PARTICLE_EFFECT(vecOrigin, vecDirection, (float)ulColor, (float)ulCount);
}

void UTIL_BloodStream(const Vector &origin, const Vector &direction, const int &color, const int &amount)
{
	if (!UTIL_ShouldShowBlood(color))
		return;

	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, origin);
		WRITE_BYTE(TE_BLOODSTREAM);
		WRITE_COORD(origin.x);
		WRITE_COORD(origin.y);
		WRITE_COORD(origin.z);
		WRITE_COORD(direction.x);
		WRITE_COORD(direction.y);
		WRITE_COORD(direction.z);
		WRITE_BYTE(color);
		WRITE_BYTE(min(amount, 255));
	MESSAGE_END();
}				

void UTIL_BloodDrips(const Vector &origin, const Vector &direction, const int &color, const int &amount)
{
	if (!UTIL_ShouldShowBlood(color))
		return;

	if (color == DONT_BLEED || amount == 0)
		return;

/*	if (g_pGameRules->IsMultiplayer())
	{
		// scale up blood effect in multiplayer for better visibility
		amount *= 2;
	}*/

//	if (amount > 255)
//		amount = 255;

	FX_Trail(origin, NULL, FX_BLOOD_RED);
//Ghoul: Moved to client
	/* 
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, origin);
		WRITE_BYTE(TE_BLOODSPRITE);
		WRITE_COORD(origin.x);								// pos
		WRITE_COORD(origin.y);
		WRITE_COORD(origin.z);
		WRITE_SHORT(g_iModelIndexAnimglow01);				// initial sprite model
		WRITE_SHORT(g_iModelIndexAnimglow01);				// droplet sprite models
		WRITE_BYTE(color);								// color index into host_basepal
		WRITE_BYTE(min(max(3, min(255,amount)/10), 16));		// size
	MESSAGE_END();*/
}			

Vector UTIL_RandomBloodVector(void)
{
	Vector direction;
	direction.x = RANDOM_FLOAT(-1.0f, 1.0f);
	direction.y = RANDOM_FLOAT(-1.0f, 1.0f);
	direction.z = RANDOM_FLOAT(0.0f, 1.0f);
	return direction;
}

int UTIL_BloodDecalIndex(const int &bloodColor)
{
	switch (bloodColor)
	{
	default:
		return RANDOM_LONG(DECAL_YBLOOD1,DECAL_YBLOOD6); break;
	case BLOOD_COLOR_RED:
	case BLOOD_COLOR_RED1:
	case BLOOD_COLOR_RED2:
	case BLOOD_COLOR_RED3:
	case BLOOD_COLOR_RED4:
	case BLOOD_COLOR_MAGENTA:
		return RANDOM_LONG(DECAL_BLOOD1,DECAL_BLOOD6); break;
	case BLOOD_COLOR_BLUE:
	case BLOOD_COLOR_BLUE1:
	case BLOOD_COLOR_BLUE2:
	case BLOOD_COLOR_BLUE3:
	case BLOOD_COLOR_BLUE4:
	case BLOOD_COLOR_BLUE5:
	case BLOOD_COLOR_BLUE6:
	case BLOOD_COLOR_BLUE7:
	case BLOOD_COLOR_CYAN:
		return RANDOM_LONG(DECAL_BBLOOD1,DECAL_BBLOOD3); break;
	case BLOOD_COLOR_GREEN:
	case BLOOD_COLOR_GREEN1:
	case BLOOD_COLOR_GREEN2:
	case BLOOD_COLOR_GREEN3:
	case BLOOD_COLOR_GREEN4:
	case BLOOD_COLOR_GREEN5:
	case BLOOD_COLOR_GREEN6:
	case BLOOD_COLOR_GREEN7:
		return RANDOM_LONG(DECAL_GBLOOD1,DECAL_GBLOOD3); break;
	}
}

void UTIL_BloodDecalTrace(TraceResult *pTrace, const int &bloodColor)
{
	if (UTIL_ShouldShowBlood(bloodColor))
		UTIL_DecalTrace(pTrace, UTIL_BloodDecalIndex(bloodColor));
}

void UTIL_DecalTrace(TraceResult *pTrace, int decalNumber)
{
	short entityIndex;
	int index;
	int message;

	if (decalNumber < 0)
		return;

	index = g_Decals[decalNumber].index;

	if (index < 0)
		return;

	if (pTrace->flFraction == 1.0)
		return;

	// Only decal BSP models
	if (pTrace->pHit)
	{
		CBaseEntity *pEntity = CBaseEntity::Instance(pTrace->pHit);
		if (pEntity && !pEntity->IsBSPModel())
			return;
		entityIndex = ENTINDEX(pTrace->pHit);
	}
	else 
		entityIndex = 0;

	message = TE_DECAL;
	if (entityIndex != 0)
	{
		if (index > 255)
		{
			message = TE_DECALHIGH;
			index -= 256;
		}
	}
	else
	{
		message = TE_WORLDDECAL;
		if (index > 255)
		{
			message = TE_WORLDDECALHIGH;
			index -= 256;
		}
	}
	
	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(message);
		WRITE_COORD(pTrace->vecEndPos.x);
		WRITE_COORD(pTrace->vecEndPos.y);
		WRITE_COORD(pTrace->vecEndPos.z);
		WRITE_BYTE(index);
		if (entityIndex)
			WRITE_SHORT(entityIndex);
	MESSAGE_END();
}

/*
==============
UTIL_PlayerDecalTrace

A player is trying to apply his custom decal for the spray can.
Tell connected clients to display it, or use the default spray can decal
if the custom can't be loaded.
==============
*/
void UTIL_PlayerDecalTrace(TraceResult *pTrace, int playernum, int decalNumber, bool bIsCustom)
{
	int index;
	
	if (!bIsCustom)
	{
		if (decalNumber < 0)
			return;

		index = g_Decals[decalNumber].index;
		if (index < 0)
			return;
	}
	else
		index = decalNumber;

	if (pTrace->flFraction == 1.0)
		return;

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(TE_PLAYERDECAL);
		WRITE_BYTE(playernum);
		WRITE_COORD(pTrace->vecEndPos.x);
		WRITE_COORD(pTrace->vecEndPos.y);
		WRITE_COORD(pTrace->vecEndPos.z);
		WRITE_SHORT((short)ENTINDEX(pTrace->pHit));
		WRITE_BYTE(index);
	MESSAGE_END();
}

void UTIL_GunshotDecalTrace(TraceResult *pTrace, int decalNumber)
{
	if (decalNumber < 0)
		return;

	int index = g_Decals[decalNumber].index;
	if (index < 0)
		return;

	if (pTrace->flFraction == 1.0)
		return;

	MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pTrace->vecEndPos);
		WRITE_BYTE(TE_GUNSHOTDECAL);
		WRITE_COORD(pTrace->vecEndPos.x);
		WRITE_COORD(pTrace->vecEndPos.y);
		WRITE_COORD(pTrace->vecEndPos.z);
		WRITE_SHORT((short)ENTINDEX(pTrace->pHit));
		WRITE_BYTE(index);
	MESSAGE_END();
}

void UTIL_Sparks(const Vector &position)
{
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, position);
		WRITE_BYTE(TE_SPARKS);
		WRITE_COORD(position.x);
		WRITE_COORD(position.y);
		WRITE_COORD(position.z);
	MESSAGE_END();
}

void UTIL_Ricochet(const Vector &position, float scale)
{
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, position);
		WRITE_BYTE(TE_ARMOR_RICOCHET);
		WRITE_COORD(position.x);
		WRITE_COORD(position.y);
		WRITE_COORD(position.z);
		WRITE_BYTE((int)(scale*10));
	MESSAGE_END();
}
/* wheel
BOOL UTIL_TeamsMatch(const char *pTeamName1, const char *pTeamName2)
{
	// Everyone matches unless it's teamplay
	if (!g_pGameRules->IsTeamplay())
		return TRUE;

	// Both on a team?
	if (*pTeamName1 != 0 && *pTeamName2 != 0)
	{
		if (!stricmp(pTeamName1, pTeamName2))	// Same Team?
			return TRUE;
	}

	return FALSE;
}
*/
/* wheel
void UTIL_StringToVector(float *pVector, const char *pString)
{
	char *pstr, *pfront, tempString[128];
	int	j;

	strcpy(tempString, pString);
	pstr = pfront = tempString;

	for (j = 0; j < 3; ++j)// lifted from pr_edict.c
	{
		pVector[j] = atof(pfront);

		while (*pstr && *pstr != ' ')
			pstr++;
		if (!*pstr)
			break;
		pstr++;
		pfront = pstr;
	}
	if (j < 2)
	{
		/
		ALERT(at_error, "Bad field in entity!! %s:%s == \"%s\"\n",
			pkvd->szClassName, pkvd->szKeyName, pkvd->szValue);
		/
		for (j = j+1;j < 3; ++j)
			pVector[j] = 0;
	}
}

wheel
void UTIL_StringToIntArray(int *pVector, int count, const char *pString)
{
	char *pstr, *pfront, tempString[128];
	int	j;

	strcpy(tempString, pString);
	pstr = pfront = tempString;

	for (j = 0; j < count; ++j)// lifted from pr_edict.c
	{
		pVector[j] = atoi(pfront);

		while (*pstr && *pstr != ' ')
			pstr++;
		if (!*pstr)
			break;
		pstr++;
		pfront = pstr;
	}

	for (j++; j < count; j++)
	{
		pVector[j] = 0;
	}
}
*/
Vector UTIL_ClampVectorToBox(const Vector &input, const Vector &clampSize)
{
	Vector sourceVector = input;

	if (sourceVector.x > clampSize.x)
		sourceVector.x -= clampSize.x;
	else if (sourceVector.x < -clampSize.x)
		sourceVector.x += clampSize.x;
	else
		sourceVector.x = 0;

	if (sourceVector.y > clampSize.y)
		sourceVector.y -= clampSize.y;
	else if (sourceVector.y < -clampSize.y)
		sourceVector.y += clampSize.y;
	else
		sourceVector.y = 0;
	
	if (sourceVector.z > clampSize.z)
		sourceVector.z -= clampSize.z;
	else if (sourceVector.z < -clampSize.z)
		sourceVector.z += clampSize.z;
	else
		sourceVector.z = 0;

	return sourceVector.Normalize();
}

float UTIL_WaterLevel(const Vector &position, float minz, float maxz)
{
	Vector midUp = position;

	midUp.z = minz;
	if (POINT_CONTENTS(midUp) != CONTENTS_WATER)// no water at bottom point means no water at all
		return minz;

	midUp.z = maxz;
	if (POINT_CONTENTS(midUp) == CONTENTS_WATER)// top point is in water, everything is in water
		return maxz;

	float diff = maxz - minz;
	while (diff > 1.0f)
	{
		midUp.z = minz + diff/2.0f;
		if (POINT_CONTENTS(midUp) == CONTENTS_WATER)
		{
			minz = midUp.z;
		}
		else
		{
			maxz = midUp.z;
		}
		diff = maxz - minz;
	}

	return midUp.z;
}

// Obsolete
void UTIL_Bubbles(const Vector &mins, const Vector &maxs, int count)
{
	Vector mid = (mins + maxs) * 0.5;
	float flHeight = UTIL_WaterLevel(mid,  mid.z, mid.z + 1024);
	flHeight = flHeight - mins.z;
	MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, mid);
		WRITE_BYTE(TE_BUBBLES);
		WRITE_COORD(mins.x);	// mins
		WRITE_COORD(mins.y);
		WRITE_COORD(mins.z);
		WRITE_COORD(maxs.x);	// maxz
		WRITE_COORD(maxs.y);
		WRITE_COORD(maxs.z);
		WRITE_COORD(flHeight);			// height
		WRITE_SHORT(g_iModelIndexBubble);
		WRITE_BYTE(count); // count
		WRITE_COORD(8); // speed
	MESSAGE_END();
}

// Obsolete
void UTIL_BubbleTrail(const Vector &from, const Vector &to, int count)
{
	float flHeight = UTIL_WaterLevel(from,  from.z, from.z + 256);
	flHeight = flHeight - from.z;

	if (flHeight < 8)
	{
		flHeight = UTIL_WaterLevel(to,  to.z, to.z + 256);
		flHeight = flHeight - to.z;
		if (flHeight < 8)
			return;

		// UNDONE: do a ploink sound
		flHeight = flHeight + to.z - from.z;
	}

	if (count > 255) 
		count = 255;

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(TE_BUBBLETRAIL);
		WRITE_COORD(from.x);	// mins
		WRITE_COORD(from.y);
		WRITE_COORD(from.z);
		WRITE_COORD(to.x);	// maxz
		WRITE_COORD(to.y);
		WRITE_COORD(to.z);
		WRITE_COORD(flHeight);			// height
		WRITE_SHORT(g_iModelIndexBubble);
		WRITE_BYTE(count); // count
		WRITE_COORD(8); // speed
	MESSAGE_END();
}

//-----------------------------------------------------------------------------
// Purpose: Properly remove entity
// Input  : *pEntity - 
//-----------------------------------------------------------------------------
void UTIL_Remove(CBaseEntity *pEntity)
{
	if (pEntity == NULL)
		return;

	if (pEntity->ShouldRespawn())// XDM3035
	{
		pEntity->SetThink(&CBaseEntity::SUB_Respawn);
		pEntity->pev->nextthink = gpGlobals->time + mp_monsrespawntime.value;// XDM: TODO
	}
	else
	{
		pEntity->SetThinkNull();// XDM3035c
		pEntity->UpdateOnRemove();
		pEntity->pev->flags |= FL_KILLME;
		pEntity->pev->targetname = 0;
//		ALERT(at_console, "UTIL_Remove(%s)\n", STRING(pEntity->pev->classname));
//crash		REMOVE_ENTITY(ENT(pEntity->pev));// XDM3035
	}
}

//-----------------------------------------------------------------------------
// Purpose: Precache resources used by specified entity
// XDM: TODO: FIXME: this is very slow and eats resources!
// Avoid using this at all costs! Especially at runtime!
// Input  : *szClassname - 
//-----------------------------------------------------------------------------
void UTIL_PrecacheOther(const char *szClassname)
{
	if (szClassname == NULL)
		return;

	edict_t	*pent = CREATE_NAMED_ENTITY(MAKE_STRING(szClassname));// should be safe
	if (FNullEnt(pent))
	{
		ALERT(at_console, "NULL Ent in UTIL_PrecacheOther\n");
		return;
	}
	pent->v.flags = FL_DORMANT;
	pent->v.effects = EF_NODRAW;
	CBaseEntity *pEntity = CBaseEntity::Instance(VARS(pent));
	if (pEntity)
	{
		pEntity->Precache();
		REMOVE_ENTITY(pent);
	}
}

//=========================================================
// UTIL_DotPoints - returns the dot product of a line from
// src to check and vecdir.
//=========================================================
float UTIL_DotPoints(const Vector &vecSrc, const Vector &vecCheck, const Vector &vecDir)
{
	Vector2D vec2LOS = (vecCheck - vecSrc).Make2D();
	vec2LOS = vec2LOS.Normalize();
	return DotProduct(vec2LOS, (vecDir.Make2D()));
}

//=========================================================
// UTIL_StripToken - for redundant keynames
// The game editor cannot add duplicate key names, it adds
// keyname#1, keyname#2 instead. Get rid of these numbers.
//=========================================================
void UTIL_StripToken(const char *pKey, char *pDest)
{
	int i = 0;
	while (pKey[i] && pKey[i] != '#')
	{
		pDest[i] = pKey[i];
		++i;
	}
	pDest[i] = 0;
}

char *memfgets(byte *pMemFile, int fileSize, int &filePos, char *pBuffer, int bufferSize)
{
	// Bullet-proofing
	if (!pMemFile || !pBuffer)
		return NULL;

	if (filePos >= fileSize)
		return NULL;

	int i = filePos;
	int last = fileSize;

	// fgets always NULL terminates, so only read bufferSize-1 characters
	if (last - filePos > (bufferSize-1))
		last = filePos + (bufferSize-1);

	int stop = 0;
	// Stop at the next newline (inclusive) or end of buffer
	while (i < last && !stop)
	{
		if (pMemFile[i] == '\n')
			stop = 1;
		++i;
	}
	// If we actually advanced the pointer, copy it over
	if (i != filePos)
	{
		// We read in size bytes
		int size = i - filePos;
		// copy it out
		memcpy(pBuffer, pMemFile + filePos, sizeof(byte)*size);
		// If the buffer isn't full, terminate (this is always true)
		if (size < bufferSize)
			pBuffer[size] = 0;

		// Update file pointer
		filePos = i;
		return pBuffer;
	}
	// No data read, bail
	return NULL;
}

bool IsFacing(const Vector &origin, const Vector &v_angle, const Vector &reference)
{
	Vector forward, angle;
	Vector vecDir = reference - origin;
	vecDir.z = 0;
	vecDir = vecDir.Normalize();
	angle = v_angle;
	angle.x = 0;
	ANGLE_VECTORS(angle, forward, NULL, NULL);
	// He's facing me, he meant it
	// TODO: use FOV!
	if (DotProduct(forward, vecDir) > 0.96)	// +/- 15 degrees or so
		return true;

	return false;
}

void UTIL_SetModel( edict_t *e, const char *model )
{
    if (!model || !*model)
    {
        ALERT(at_console, "Error! Model not specified\n" );
        PRECACHE_MODEL( "models/effects/ef_explosions.mdl");
        SET_MODEL( e, "models/effects/ef_explosions.mdl" );
        return;
    }
    
    int length;
    const char *pFile = (char*)LOAD_FILE_FOR_ME( (char*)model, &length );
    
    if (pFile && length)
    {
        FREE_FILE( (char *)pFile );
        PRECACHE_MODEL( (char*)model);
        SET_MODEL( e, model );
    }
    else //invalid path ?
    {
        FREE_FILE( (char *)pFile );
        ALERT(at_console, "Error! Model %s not found\n", model );
        PRECACHE_MODEL( "models/effects/ef_explosions.mdl");
        SET_MODEL( e, "models/effects/ef_explosions.mdl" );
        return;
    }
}

int UTIL_PrecacheModel( char* s )
{
    if (FStringNull(MAKE_STRING(s))) //model not specified ???
        return NULL;
    
    int length;
    int model;
    char *pFile = (char*)LOAD_FILE_FOR_ME( (char*)s, &length );
    
    if (pFile && length)
    {
        FREE_FILE( pFile );
        model = PRECACHE_MODEL( (char*)s);
    }
    else
    {
        FREE_FILE( (char *)pFile );
        model = PRECACHE_MODEL( "models/effects/ef_explosions.mdl" );
    }
    return model;
    
}

