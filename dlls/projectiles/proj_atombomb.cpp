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

#define NDD_POWERUP_TIME	9.0
#define NDD_PITCH_DELTA		3

LINK_ENTITY_TO_CLASS(AtomBomb, CAtomBomb);

void CAtomBomb::Spawn(void)
{
	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_BBOX;
	pev->takedamage = DAMAGE_NO;//DAMAGE_YES;

	SET_MODEL(ENT(pev), "models/w_atombomb.mdl");
	UTIL_SetSize(pev, Vector(-4, -4, 0), Vector(4, 4, 4));
	UTIL_SetOrigin(pev, pev->origin);

	SetThink(&CAtomBomb::BombThink);
	SetTouch(&CAtomBomb::BombTouch);

	pev->friction		= 0.8f;
	pev->health			= 20;
	pev->button			= PITCH_NORM;
	pev->dmg			= gSkillData.DmgAtomBomb;
	pev->dmgtime		= gpGlobals->time + NDD_POWERUP_TIME;
	pev->nextthink		= gpGlobals->time + 0.1f;
	pev->spawnflags		|= SF_NORESPAWN;
	pev->spawnflags		|= SF_NORESPAWN;
}

void CAtomBomb::BombThink(void)
{
	if (!IsInWorld())
	{
		UTIL_Remove(this);
		return;
	}
	if (pev->dmgtime <= gpGlobals->time)
	{
		Explode();
	}
	else
	{
		EMIT_SOUND_DYN(ENT(pev), CHAN_BODY, "weapons/beep_atombomb.wav", VOL_NORM, ATTN_NORM, 0, pev->button);
		pev->button += NDD_PITCH_DELTA;
		pev->effects |= EF_MUZZLEFLASH;
		pev->skin++;
		pev->nextthink = gpGlobals->time + 1.0f;
	}
}

void CAtomBomb::BombTouch(CBaseEntity *pOther)
{
	if (pOther->IsBSPModel())
	{
		AlignToFloor();
		SetTouchNull();
	}
}

void CAtomBomb::Explode(void)
{
	CBaseEntity *pEnt = NULL;
	pEnt = UTIL_FindEntityByClassname(pEnt, "item_radshield");
	if (pEnt != NULL)
	{
		UTIL_Remove(pEnt);
	}
	pev->effects = EF_NODRAW;
	pev->movetype = MOVETYPE_NONE;
	pev->takedamage = DAMAGE_NO;
	SetTouchNull();
	SetThink(&CGrenade::NuclearExplodeThink);
	pev->nextthink = gpGlobals->time;
}

void CAtomBomb::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	Explode();
}
