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

#define PROTON_MAX_SHARDS	10

LINK_ENTITY_TO_CLASS(ProtonMissile, CProtonMissile);

void CProtonMissile::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_PROTON_MISSILE;
	pev->takedamage = DAMAGE_YES;
	pev->health = 2;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	pev->frags = PROTON_MAX_SHARDS;
	pev->impulse = 0;
	UTIL_SetSize(this, 3.0f);
	pev->nextthink = gpGlobals->time+0.15;
	SetTouch(&CProtonMissile::ExplodeTouch);
	SetThink(&CProtonMissile::Fly);
}

CProtonMissile *CProtonMissile::ShootProtonMissile(CBaseEntity *pOwner, const Vector &vecStart, CBaseEntity *pEnemy, float dmg)
{
	CProtonMissile *pNew = GetClassPtr((CProtonMissile *)NULL, "ProtonMissile");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_PROTON_MISSILE;
		pNew->m_hEnemy = pEnemy;
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
	}
	return pNew;
}

int CProtonMissile::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();

		CGrenade::Blast(0, 0, pev->dmg*0.75, pev->dmg*1.25, DMG_ENERGYBLAST, FX_PROTONMISSILE_BLAST);
	return 1;
}

void CProtonMissile::ExplodeTouch(CBaseEntity *pOther)
{
	CGrenade::Impact(pOther, TRUE, pev->dmg*0.2, pev->dmg*2.5, TRUE, RANDOM_LONG(DECAL_NUCBLOW1,DECAL_NUCBLOW3), pev->dmg*0.25, DMG_MORTAR, pev->dmg, pev->dmg*2.0, DMG_ENERGYBLAST, 32.0, FX_PROTONMISSILE_DETONATE);
	pev->effects |= EF_NODRAW;
	pev->solid = SOLID_NOT;
	pev->velocity = g_vecZero;

	TraceResult tr;
	Vector vecSpot = pev->origin - pev->velocity.Normalize() * 8;
	Vector vecEnd = pev->origin + pev->velocity.Normalize() * 16;
	UTIL_TraceLine( vecSpot, vecEnd, ignore_monsters, ENT(pev), &tr );	
	pev->origin = tr.vecEndPos + (tr.vecPlaneNormal * 64);
	SetThink(ShootShards);
	pev->nextthink = gpGlobals->time + RANDOM_FLOAT(0.2, 0.35);
}

void CProtonMissile::ShootShards( void )
{
	if(pev->frags <=0 )
	{
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time;
		return;
	}
	pev->frags--;
	TraceResult tr;
	UTIL_TraceLine(pev->origin, pev->origin + UTIL_RandomVector()*RANDOM_FLOAT(128, 420), dont_ignore_monsters, ENT(pev), &tr );
	UTIL_DecalTrace(&tr, DECAL_EXPSCORCH1 + RANDOM_LONG(0,2));
	FX_Trail(tr.vecEndPos, entindex(), FX_PROTONMISSILE_SHARD_DETONATE );


	CBaseEntity *pOwner = NULL;
	if (pev->owner)
		pOwner = CBaseEntity::Instance(pev->owner);
	else 
		pOwner = g_pWorld;
	::RadiusDamage(tr.vecEndPos, this, pOwner, pev->dmg/2.5, pev->dmg/1.5, CLASS_NONE, DMG_BLAST | DMG_RADIUS_MAX);
	pev->nextthink = gpGlobals->time + RANDOM_FLOAT(0.25,0.45);
}

void CProtonMissile::Fly( void)
{
	if (pev->impulse == 0)
	{
		FX_Trail(pev->origin, entindex(), FX_PROTONMISSILE);
		pev->velocity = pev->velocity * 1.25;
		pev->impulse = 1;
	}

	if (m_hEnemy != NULL)
	{
		if (m_hEnemy->IsAlive())
			CGrenade::MovetoTarget(m_hEnemy->Center(), SPEED_PROTON_MISSILE);
		else
			m_hEnemy = NULL;
	}
	pev->angles = UTIL_VecToAngles(pev->velocity);
	pev->nextthink = gpGlobals->time + 0.05;
}