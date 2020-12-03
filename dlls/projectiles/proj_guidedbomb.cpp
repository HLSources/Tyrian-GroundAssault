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

#define GBOMB_DETONATE_DELAY	5
#define GBOMB_SEEK_RADIUS		3072

LINK_ENTITY_TO_CLASS(GuidedBomb, CGuidedBomb);

void CGuidedBomb::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");

	if (pev->frags)
	{
		pev->body = PROJ_BODY_GUIDED_BOMB;
		pev->nextthink = gpGlobals->time + 0.15;
		UTIL_SetSize(this, 4.0f);
	}
	else
	{
		pev->body = PROJ_BODY_GUIDED_SHARD;
		pev->nextthink = gpGlobals->time + RANDOM_FLOAT(0.8,1.75);
		UTIL_SetSize(this, 2.0f);
	}
	pev->animtime = gpGlobals->time;
	pev->takedamage = DAMAGE_YES;
	pev->health = 20;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	pev->dmgtime = gpGlobals->time + GBOMB_DETONATE_DELAY;

	SetThink(&CGuidedBomb::Fly);
	SetTouch(&CGuidedBomb::ExplodeTouch);
}

CGuidedBomb *CGuidedBomb::ShootGuidedBomb(CBaseEntity *pOwner, const Vector &vecStart, float spread, int numShards, float dmg)
{
	CGuidedBomb *pNew = GetClassPtr((CGuidedBomb *)NULL, "GuidedBomb");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_GUIDED_BOMB  + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
		pNew->pev->frags = 1;
		pNew->m_iCount = numShards;
		pNew->Spawn();

		FX_Trail(pNew->pev->origin, pNew->entindex(), FX_GUIDED_BOMB);
	}
	return pNew;
}

CGuidedBomb *CGuidedBomb::ShootGuidedShard(CBaseEntity *pOwner, const Vector &vecStart, const Vector &vecVel, float time, float dmg)
{
	CGuidedBomb *pNew = GetClassPtr((CGuidedBomb *)NULL, "GuidedBomb");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->pev->origin = vecStart;
		pNew->pev->velocity = vecVel;
		pNew->pev->angles = UTIL_VecToAngles(pNew->pev->velocity);
		pNew->m_iCount = 0;
		pNew->pev->dmg = dmg;
		pNew->pev->frags = 0;
		pNew->Spawn();

		FX_Trail(pNew->pev->origin, pNew->entindex(), FX_GUIDED_SHARD);
	}
	return pNew;
}

void CGuidedBomb::DissociateThink(void)
{
	if (m_iCount <= 0)
	{
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time;
		return;
	}
	CGuidedBomb::ShootGuidedShard(Instance(pev->owner), pev->origin + m_vecNormal * RANDOM_FLOAT(8.0, 24.0) + UTIL_RandomBloodVector(), UTIL_RandomBloodVector() * (SPEED_GUIDED_SHARD + RANDOM_FLOAT(-150, 150)), RANDOM_FLOAT(3, 5), pev->dmg*0.25);
	--m_iCount;
	pev->nextthink = gpGlobals->time + RANDOM_FLOAT(0.1f, 0.2f);
}

int CGuidedBomb::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();

	if (pev->frags)
		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_BLAST, FX_GUIDED_BOMB_BLAST);
	else
		CGrenade::Blast(0, 0, 0, 0, 0, FX_GUIDED_SHARD_BLAST);
	return 1;
}

void CGuidedBomb::ExplodeTouch(CBaseEntity *pOther)
{
	if (pev->frags)
	{
		CGrenade::Impact(pOther, TRUE, pev->dmg*0.2, pev->dmg*2.0, TRUE, RANDOM_LONG(DECAL_SCORCH1,DECAL_SCORCH3), pev->dmg*0.25, DMG_MORTAR, pev->dmg, pev->dmg*2.0, DMG_BLAST, 40.0, FX_GUIDED_BOMB_DETONATE);
		pev->effects |= EF_NODRAW;
		pev->solid = SOLID_NOT;
		pev->velocity = g_vecZero;

		TraceResult tr;
		Vector vecSpot = pev->origin - pev->velocity.Normalize() * 32;
		Vector vecEnd = pev->origin + pev->velocity.Normalize() * 64;
		UTIL_TraceLine( vecSpot, vecEnd, ignore_monsters, ENT(pev), &tr );		
		pev->origin = tr.vecEndPos + (tr.vecPlaneNormal * 96);
		SetThink(DissociateThink);
		pev->nextthink = gpGlobals->time + 0.1;		
	}
	else
	{
		CGrenade::Impact(pOther, TRUE, 0, 0, TRUE, RANDOM_LONG(DECAL_EXPSCORCH1,DECAL_EXPSCORCH3), pev->dmg*0.25, DMG_MORTAR, pev->dmg, pev->dmg*2.0, DMG_BLAST, 24.0, FX_GUIDED_SHARD_DETONATE);
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time;
	}
}

void CGuidedBomb::Fly(void)
{
	if (pev->dmgtime <= gpGlobals->time)
	{
		if (pev->frags)
			CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_BLAST, FX_GUIDED_BOMB_BLAST);
		else
			CGrenade::Blast(0, 0, 0, 0, 0, FX_GUIDED_SHARD_BLAST);
		return;
	}

	if(pev->frags)//main bomb
	{
		if ( gpGlobals->time >= pev->animtime )
		{
			EMIT_SOUND(ENT(pev), CHAN_BODY, "weapons/fly_guidedbomb.wav", VOL_NORM, ATTN_NORM);
			pev->animtime = gpGlobals->time + 0.95;
		}
	}

	CGrenade::SeekTarget(GBOMB_SEEK_RADIUS, pev->frags?VIEW_FIELD_QUITE_NARROW:VIEW_FIELD_FULL);

	if (m_hEnemy != NULL)
	{
		if (m_hEnemy->IsAlive())
			CGrenade::MovetoTarget(m_hEnemy->Center(), SPEED_GUIDED_SHARD);
		else
			m_hEnemy = NULL;
	}
	pev->angles = UTIL_VecToAngles(pev->velocity);
	pev->nextthink = gpGlobals->time + 0.05;
}