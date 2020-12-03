#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "soundent.h"
#include "globals.h"
#include "game.h"
#include "msg_fx.h"
#include "projectiles.h"
#include "decals.h"
#include "gamerules.h"

#define HELLFIRE_BURN_TIME		5
#define HELLFIRE_DETONATE_DELAY	5
#define HELLFIRE_SEEK_RADIUS	1024

LINK_ENTITY_TO_CLASS(HellFire, CHellFire);

void CHellFire::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_HELLFIRE;
	pev->takedamage = DAMAGE_NO;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	pev->health = 2;
	pev->impulse = 0;
	pev->frags = 0;
	pev->ltime = gpGlobals->time + HELLFIRE_DETONATE_DELAY;
	UTIL_SetSize(this, 2.0f);

	pev->nextthink = gpGlobals->time + 0.3;
	SetTouch(&CHellFire::ExplodeTouch);
	SetThink(&CHellFire::Fly);
}

CHellFire *CHellFire::ShootHellFire(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg)
{
	CHellFire *pNew = GetClassPtr((CHellFire *)NULL, "HellFire");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_HELLFIRE  + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
	}
	return pNew;
}

int CHellFire::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();

		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_BURN | DMG_SLOWBURN, FX_HELLFIRE_BLAST);
	return 1;
}

void CHellFire::ExplodeTouch(CBaseEntity *pOther)
{
	CGrenade::Impact(pOther, TRUE, pev->dmg*0.25, pev->dmg*2.5, TRUE, RANDOM_LONG(DECAL_SCORCH1,DECAL_SCORCH3), pev->dmg*0.25, DMG_MORTAR, pev->dmg, pev->dmg*2.0, DMG_BURN | DMG_SLOWBURN, 24.0, FX_HELLFIRE_DETONATE);
	pev->velocity = g_vecZero;
	pev->effects |= EF_NODRAW;
	SetThink(BurnThink);
	pev->nextthink = gpGlobals->time;
}

void CHellFire::BurnThink(void)
{
	if (!IsInWorld() || POINT_CONTENTS(pev->origin) == CONTENTS_WATER)
	{
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time;
		return;
	}

	if (pev->dmgtime <= gpGlobals->time)
	{
		if (pev->impulse > 0)
		{
			SetThink(&CBaseEntity::SUB_Remove);
			pev->nextthink = gpGlobals->time;
		}
		else
		{
			pev->impulse = 1;
			pev->dmgtime = gpGlobals->time + HELLFIRE_BURN_TIME;
		}
	}
	if (pev->impulse > 0)
	{
		CGrenade::Impact(NULL, FALSE, 0, 0, FALSE, 0, 0, 0, pev->dmg*0.15, 64+(pev->dmg*2.0), DMG_BURN | DMG_SLOWBURN, 0.0, 0);
	}
	pev->nextthink = gpGlobals->time + 0.25f;
}

void CHellFire::Fly(void)
{
	if (pev->ltime <= gpGlobals->time)
	{
		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_BURN | DMG_SLOWBURN, FX_HELLFIRE_BLAST);
		return;
	}
	if (pev->frags == 0)
	{
		FX_Trail(pev->origin, entindex(), FX_HELLFIRE);
		pev->velocity = pev->velocity * 1.25;
		pev->frags = 1;
		pev->takedamage = DAMAGE_YES;
	}
	if ( gpGlobals->time >= pev->animtime )
	{
		EMIT_SOUND(ENT(pev), CHAN_BODY, "weapons/fly_hellfire.wav", VOL_NORM, ATTN_NORM);
		pev->animtime = gpGlobals->time + 2.1;
	}

	CGrenade::SeekTarget(HELLFIRE_SEEK_RADIUS, VIEW_FIELD_ULTRA_NARROW);

	if (m_hEnemy != NULL)
	{
		if (m_hEnemy->IsAlive())
		{
			if (FVisible(m_hEnemy))
				CGrenade::MovetoTarget(m_hEnemy->Center(), SPEED_HELLFIRE);
		}
		else
			m_hEnemy = NULL;
	}
	pev->angles = UTIL_VecToAngles(pev->velocity);
	pev->nextthink = gpGlobals->time + 0.05;
}