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

#define NUKE_RADIATION_TIME	10

LINK_ENTITY_TO_CLASS(NuclearMissile, CNuclearMissile);

void CNuclearMissile::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_NUKE;
	pev->takedamage = DAMAGE_YES;
	pev->health = 2;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	pev->impulse = 0;
	pev->button = 0;
	UTIL_SetSize(this, 3.0f);
	pev->nextthink = gpGlobals->time+0.3;

	SetTouch(&CNuclearMissile::ExplodeTouch);
	SetThink (&CNuclearMissile::Fly);
}

CNuclearMissile *CNuclearMissile::ShootNuclearMissile(CBaseEntity *pOwner, const Vector &vecStart, CBaseEntity *pEnemy, float dmg)
{
	CNuclearMissile *pNew = GetClassPtr((CNuclearMissile *)NULL, "NuclearMissile");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_NUCLEAR_MISSILE;
		pNew->m_hEnemy = pEnemy;
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
	}
	return pNew;
}

void CNuclearMissile::Fly( void)
{
	if (m_hEnemy != NULL)
	{
		if (m_hEnemy->IsAlive())
			CGrenade::MovetoTarget(m_hEnemy->Center(), SPEED_NUCLEAR_MISSILE);
		else
			m_hEnemy = NULL;
	}

	if (pev->impulse == 0)
	{
		FX_Trail(pev->origin, entindex(), FX_NUCLEARMISSILE);
		pev->impulse = 1;
	}
	pev->angles = UTIL_VecToAngles(pev->velocity);
	pev->nextthink = gpGlobals->time + 0.05;
}

int CNuclearMissile::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();

		CGrenade::Blast(0, 0, pev->dmg*0.75, pev->dmg*0.75, DMG_VAPOURIZING, FX_NUCLEARMISSILE_BLAST);
	return 1;
}

void CNuclearMissile::ExplodeTouch(CBaseEntity *pOther)
{
	CGrenade::Impact(pOther, TRUE, pev->dmg*0.1, pev->dmg*5.0, TRUE, DECAL_BLOW, pev->dmg*0.1, DMG_MORTAR, pev->dmg, pev->dmg*1.25, DMG_VAPOURIZING, 32.0, FX_NUCLEARMISSILE_DETONATE);
	pev->effects |= EF_NODRAW;
	pev->velocity = g_vecZero;
	SetThink(BlastRing);
	pev->nextthink = gpGlobals->time + 1.25;
}

void CNuclearMissile::BlastRing( void )
{
	CGrenade::Impact(NULL, FALSE, pev->dmg*0.2, pev->dmg*5.0, FALSE, 0, 0, 0, pev->dmg*0.5, pev->dmg*2.0, DMG_VAPOURIZING, 4.0, FX_NUCLEARMISSILE_RING_DETONATE);
	SetThink(&CNuclearMissile::RadiationThink);
	pev->nextthink = gpGlobals->time;	
}

void CNuclearMissile::RadiationThink(void)
{
	if (!IsInWorld())
	{
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time;
		return;
	}

	if (pev->dmgtime <= gpGlobals->time)
	{
		if (pev->button > 0)
		{
			SetThink(&CBaseEntity::SUB_Remove);
			pev->nextthink = gpGlobals->time;
		}
		else
		{
			pev->button = 1;
			pev->dmgtime = gpGlobals->time + NUKE_RADIATION_TIME;
		}
	}
	if (pev->button > 0)
	{
		CGrenade::Impact(NULL, FALSE, 0, 0, FALSE, 0, 0, 0, pev->dmg*0.02, pev->dmg*1.5, DMG_RADIATION, 0.0, 0);
	}
	pev->nextthink = gpGlobals->time + 0.25f;
}