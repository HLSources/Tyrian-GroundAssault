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

#define GHOST_MISSILE_DETONATE_DELAY	7

LINK_ENTITY_TO_CLASS(GhostMissile, CGhostMissile);

CGhostMissile *CGhostMissile::ShootGhostMissile(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg)
{
	CGhostMissile *pNew = GetClassPtr((CGhostMissile *)NULL, "GhostMissile");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_GHOST_MISSILE + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
	}
	return pNew;
}

void CGhostMissile::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_GHOST_MISSILE;
	pev->takedamage = DAMAGE_YES;
	pev->health = 12;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	pev->dmgtime = gpGlobals->time + GHOST_MISSILE_DETONATE_DELAY;
	pev->impulse = 0;
	UTIL_SetSize(this, 3.0f);

	pev->nextthink = gpGlobals->time+0.1;
	SetTouch(&CGhostMissile::ExplodeTouch);
	SetThink(&CGhostMissile::Fly);
}

int CGhostMissile::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();

		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_BLAST, FX_GHOSTMISSILE_BLAST);
	return 1;
}

void CGhostMissile::ExplodeTouch(CBaseEntity *pOther)
{
	if (pOther->pev->takedamage == DAMAGE_NO)
	{
		pev->movetype = MOVETYPE_NOCLIP;
		return;
	}
	CGrenade::Impact(pOther, TRUE, pev->dmg*0.2, pev->dmg*1.5, TRUE, RANDOM_LONG(DECAL_SCORCH1,DECAL_SCORCH3), pev->dmg*0.25, DMG_MORTAR, pev->dmg, pev->dmg*2.0, DMG_BLAST, 24.0, FX_GHOSTMISSILE_DETONATE);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

void CGhostMissile::Fly(void)
{
	if (pev->dmgtime <= gpGlobals->time)
	{
		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_BLAST, FX_GHOSTMISSILE_BLAST);
		return;
	}

	if ( gpGlobals->time >= pev->framerate )
	{
		EMIT_SOUND(ENT(pev), CHAN_BODY, "weapons/fly_ghostmissile.wav", VOL_NORM, ATTN_NORM);
		pev->framerate = gpGlobals->time + 2.1;
	}

	if (pev->impulse == 0)
	{
		FX_Trail(pev->origin, entindex(), FX_GHOSTMISSILE);
		pev->velocity = pev->velocity * 1.25;
		pev->impulse = 1;
	}

	TraceResult tr;
	Vector vecSpot = pev->origin;
	Vector vecEnd = pev->origin + pev->velocity.Normalize() * g_psv_zmax->value;
	UTIL_TraceHull(vecSpot, vecEnd, dont_ignore_monsters, large_hull, ENT(pev), &tr);
	CBaseEntity *pHit = CBaseEntity::Instance(tr.pHit);
	CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);

	if (g_pGameRules->IsTeamplay() && pOwner->pev->team == pHit->pev->team)
	{
		pev->nextthink = gpGlobals->time + 0.1;
		return;
	}

	if (pHit->IsMonster() || pHit->IsPlayer() && pHit->pev->takedamage != DAMAGE_NO && pHit->IsAlive() && pHit->edict() != pev->owner )
	{
		FX_FireLgtng(pev->origin, tr.vecEndPos, entindex(), TYPE_GHOSTMISSILE_BEAM);
		UTIL_SetOrigin(pev, pHit->pev->origin);
		return;
	}
	pev->nextthink = gpGlobals->time + 0.1;
}