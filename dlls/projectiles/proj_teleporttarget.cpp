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
#include "projectiles.h"
#include "msg_fx.h"
#include "globals.h"

#define TARGET_CALL_TIME		7.5f
#define TARGET_PITCH_DELTA		5

LINK_ENTITY_TO_CLASS(teleporttarget, CTeleportTarget);


void CTeleportTarget::Spawn(void)
{
	Precache();
	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_BBOX;
	pev->takedamage = DAMAGE_NO;

	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_TELEPORT_TARGET;

	UTIL_SetSize(pev, Vector(-4, -4, 0), Vector(4, 4, 2));
	UTIL_SetOrigin(pev, pev->origin);

	SetTouch(&CTeleportTarget::TTTouch);
	pev->friction		= 0.8;
	pev->health			= 10;
	pev->button			= PITCH_NORM;
	pev->dmgtime		= gpGlobals->time + TARGET_CALL_TIME;
	pev->nextthink		= gpGlobals->time + 0.1f;
	pev->spawnflags		|= SF_NORESPAWN;
}

void CTeleportTarget::TTTouch(CBaseEntity *pOther)
{
	if (pOther->IsBSPModel())
		AlignToFloor();

	FX_Trail(pev->origin+Vector(0,0,32), entindex(), FX_TELEPORT_TARGET_ACTIVATE);

	SET_MODEL(ENT(pev), "models/effects/ef_atombomb_call.mdl");
	pev->renderfx = kRenderFxFullBright;
	pev->solid = SOLID_NOT;
	pev->animtime = gpGlobals->time;
	pev->framerate = 1;
	SetTouchNull();
	SetThink(&CTeleportTarget::TTThink);
	pev->nextthink = gpGlobals->time + 0.5f;
}

void CTeleportTarget::TTThink(void)
{
	if (!IsInWorld())
	{
		UTIL_Remove(this);
		return;
	}
	CBaseEntity *pOwner = NULL;
	if (pev->owner)
		pOwner = CBaseEntity::Instance(pev->owner);
	else 
	pOwner = g_pWorld;

	if (pev->dmgtime <= gpGlobals->time)
	{
		if (g_pGameRules->IsGameOver())// XDM3035c
		{
			Disintegrate();
			return;
		}

		if (CheckCeiling())
		{
			FX_Trail(pev->origin, entindex(), FX_PLAYER_SPAWN_RINGTELEPORT_PART2);
			CRingTeleport::CreateNew(pOwner, pev->origin, TRUE);
		}
		else //not enough space from the top
		{
			FX_Trail(pev->origin+Vector(0,0,32), entindex(), FX_PLAYER_SPAWN);
			CBaseEntity::Create("AtomBomb", pev->origin + Vector(0,0,8), g_vecZero, g_vecZero, pOwner->edict());
			CBaseEntity::Create( MAKE_STRING("item_radshield"), pev->origin + Vector(0,0,64), g_vecZero, g_vecZero, NULL, SF_NOTREAL|SF_NORESPAWN);
		}

		if (!CheckCeiling())
			FX_Trail(pev->origin, entindex(), FX_PLAYER_SPAWN_RINGTELEPORT_PART1);

		::RadiusDamage(pev->origin, this, pOwner, 999, 128, CLASS_NONE, DMG_DISINTEGRATING | DMG_IGNOREARMOR);
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time;
	}
	else
	{
		Ping();
		pev->nextthink = gpGlobals->time + 0.75f;
	}
}

void CTeleportTarget::Ping(void)
{
	EMIT_SOUND_DYN(ENT(pev), CHAN_BODY, "weapons/atombomb_call.wav", VOL_NORM, ATTN_NORM, 0, pev->button);
	pev->button += TARGET_PITCH_DELTA;
}

BOOL CTeleportTarget::CheckCeiling(void)
{
	Vector start = pev->origin;
	Vector end = start + Vector(0.0f, 0.0f, 300.0f);
	TraceResult tr;
	UTIL_TraceLine(start, end, ignore_monsters, ENT(pev), &tr);
	if (tr.flFraction == 1.0)
		return TRUE;

	return FALSE;
}