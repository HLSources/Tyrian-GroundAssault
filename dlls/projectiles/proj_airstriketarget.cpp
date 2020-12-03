//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2013
//-----------------------------------------------------------------------------
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "game.h"
#include "gamerules.h"
#include "skill.h"
#include "decals.h"
#include "projectiles.h"
#include "msg_fx.h"

#define MORTAR_CALL_TIME	3.0f
#define MORTAR_PITCH_DELTA	5.0

#define SATELLITE_CALL_TIME		6.0f
#define SATELLITE_PITCH_DELTA	3.0

#define SATELLITE_DMG_STAY_TIME		4.0f

LINK_ENTITY_TO_CLASS(strtarget, CAirStrikeTarget);

CAirStrikeTarget *CAirStrikeTarget::CreateNew(CBaseEntity *pOwner, const Vector &vecStart, float dmg, int type)
{
	CAirStrikeTarget *pNew = GetClassPtr((CAirStrikeTarget *)NULL, "strtarget");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->frags = type;
		pNew->pev->dmg = dmg;
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_AIRSTRIKE_TARGET;
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
	}
	return pNew;
}

void CAirStrikeTarget::Spawn(void)
{
	Precache();
	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_BBOX;
	pev->takedamage = DAMAGE_NO;

	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_MORTAR_TARGET;

	UTIL_SetSize(pev, Vector(-4, -4, 0), Vector(4, 4, 2));
	UTIL_SetOrigin(pev, pev->origin);

	SetTouch(&CAirStrikeTarget::ATTouch);
	pev->friction		= 0.8;
	pev->health			= 10;
	pev->button			= PITCH_NORM;
	pev->nextthink		= gpGlobals->time + 0.1f;
	pev->spawnflags		|= SF_NORESPAWN;// XDM3035
}

void CAirStrikeTarget::ATTouch(CBaseEntity *pOther)
{
	if (pOther->IsBSPModel())
	{
		EMIT_SOUND( ENT(pev), CHAN_VOICE, "weapons/mine_deploy.wav", VOL_NORM, ATTN_NORM);
		AlignToFloor();
	}

	SetThink(&CAirStrikeTarget::ATThink);

	if (pev->frags == AIRSTRIKE_MORTAR)
		pev->dmgtime = gpGlobals->time + MORTAR_CALL_TIME;
	else if (pev->frags == AIRSTRIKE_SATELLITE)
		pev->dmgtime = gpGlobals->time + SATELLITE_CALL_TIME;

	pev->nextthink = gpGlobals->time;	
	SetTouchNull();
}

void CAirStrikeTarget::ATThink(void)
{
	if (!IsInWorld())
	{
		UTIL_Remove(this);
		return;
	}

	if (pev->dmgtime <= gpGlobals->time)
	{
		if (g_pGameRules->IsGameOver())// XDM3035c
		{
			Disintegrate();
			return;
		}

		if (CheckSatellite())
		{
			Vector vecSpot;
			TraceResult tr;

			if (pev->frags == AIRSTRIKE_MORTAR)
			{
				switch (RANDOM_LONG(0,2))
				{
					case 0:	EMIT_SOUND_DYN(edict(), CHAN_ITEM, "weapons/airstrike_mortar_whistle1.wav", VOL_NORM, ATTN_NONE, 0, RANDOM_LONG(95,120)); break;
					case 1:	EMIT_SOUND_DYN(edict(), CHAN_ITEM, "weapons/airstrike_mortar_whistle2.wav", VOL_NORM, ATTN_NONE, 0, RANDOM_LONG(95,120)); break;
					case 2:	EMIT_SOUND_DYN(edict(), CHAN_ITEM, "weapons/airstrike_mortar_whistle3.wav", VOL_NORM, ATTN_NONE, 0, RANDOM_LONG(95,120)); break;
				}

				for (int i=0; i<5; ++i)
				{
					vecSpot = pev->origin + UTIL_RandomBloodVector()*384.0f;
					vecSpot.z = pev->origin.z + 2.0f;

					// find the sky
					UTIL_TraceLine(vecSpot, vecSpot + Vector(0,0,2048), ignore_monsters, ENT(pev), &tr);
					pev->endpos = tr.vecEndPos;
					// trace down from 'endpos' to find the explosion position
					UTIL_TraceLine(pev->endpos, vecSpot - Vector(0,0,2048), ignore_monsters, ENT(pev), &tr);

					CBaseEntity *pMortar = Create("monster_mortar", tr.vecEndPos, g_vecZero, pev->owner);
					if (pMortar)
					{
						pMortar->pev->nextthink = gpGlobals->time + 2.5f + (float)i;
						pMortar->pev->dmg = pev->dmg;
					}
				}
			}
			else if (pev->frags == AIRSTRIKE_SATELLITE)
			{
				pev->velocity = g_vecZero;
				pev->effects |= EF_NODRAW;

				CGrenade::Impact(NULL, TRUE, pev->dmg*0.1, pev->dmg*3.0, FALSE, 0, 0, 0, pev->dmg, pev->dmg*1.25, DMG_DISINTEGRATING, 2.0, FX_SATELLITE_STRIKE);
				SetThink(&CAirStrikeTarget::RadiationThink);
				pev->nextthink = gpGlobals->time+0.25;			
			}
			else if (pev->frags == AIRSTRIKE_PARACHUTE)
			{
				//nothing here yet
			}
		}
		if (pev->frags == AIRSTRIKE_MORTAR)
			Disintegrate();
	}
	else
	{
		Ping();
		pev->nextthink = gpGlobals->time + 0.75f;
	}
}

void CAirStrikeTarget::Ping(void)
{
	if (CheckSatellite() && pev->frags == AIRSTRIKE_MORTAR)
	{
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_USERTRACER);
		WRITE_COORD(pev->origin.x);// pos
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_COORD(0);// vel
		WRITE_COORD(0);
		WRITE_COORD(3072.0f + pev->button*2.0f);
		WRITE_BYTE(15);// life
		WRITE_BYTE(2);// green color "r_efx.h"
		WRITE_BYTE(30);// length
	MESSAGE_END();
	}

	if (pev->frags == AIRSTRIKE_MORTAR)
	{
		FX_Trail(pev->origin, entindex(), FX_STR_TARGET_MORTAR);
		EMIT_SOUND_DYN(ENT(pev), RANDOM_LONG(CHAN_WEAPON,CHAN_BODY), "weapons/airstrike_call.wav", VOL_NORM, ATTN_NORM, 0, pev->button);
		pev->button += MORTAR_PITCH_DELTA;
	}
	else if (pev->frags == AIRSTRIKE_SATELLITE)
	{
		FX_Trail(pev->origin, entindex(), FX_STR_TARGET_SATELLITE);
		EMIT_SOUND_DYN(ENT(pev), RANDOM_LONG(CHAN_WEAPON,CHAN_BODY), "weapons/satellite_strike_call.wav", VOL_NORM, ATTN_LOW_HIGH, 0, pev->button);
		pev->button += SATELLITE_PITCH_DELTA;
	}
}

BOOL CAirStrikeTarget::CheckSatellite(void)
{
	Vector start = pev->origin;
	Vector end = start + Vector(0.0f, 0.0f, g_psv_zmax->value);
	TraceResult tr;
	UTIL_TraceLine(start, end, ignore_monsters, ENT(pev), &tr);
	if (tr.pHit != NULL)
	{
		const char *tex = NULL;// runtime libraries fault protection
		if (tr.pHit->v.solid == SOLID_BSP)
			tex = TRACE_TEXTURE(tr.pHit, start, end);

		if (tex != NULL && stricmp(tex, "sky") == 0)// Texture name can be 'sky' or 'SKY' so use strIcmp!
			return TRUE;
	}
	return FALSE;
}

void CAirStrikeTarget::RadiationThink(void)
{
	if (!IsInWorld())
	{
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time;
		return;
	}

	if (pev->teleport_time <= gpGlobals->time)
	{
		if (pev->impulse > 0)
		{
			SetThink(&CBaseEntity::SUB_Remove);
			pev->nextthink = gpGlobals->time;
		}
		else
		{
			pev->impulse = 1;
			pev->teleport_time = gpGlobals->time + SATELLITE_DMG_STAY_TIME;
		}
	}
	if (pev->impulse > 0)
	{
		CGrenade::Impact(NULL, FALSE, 0, 0, FALSE, 0, 0, 0, pev->dmg*0.05, pev->dmg, DMG_RADIATION, 0.0, 0);
	}
	pev->nextthink = gpGlobals->time + 0.25f;
}