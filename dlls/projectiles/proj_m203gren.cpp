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

LINK_ENTITY_TO_CLASS(m203grenade, CM203Grenade);

void CM203Grenade::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_M203;
	pev->takedamage = DAMAGE_YES;
	pev->health = 2;
	pev->solid = SOLID_SLIDEBOX;
	UTIL_SetSize(this, 2.0f);

	pev->nextthink = gpGlobals->time;
	SetTouch(&CM203Grenade::ExplodeTouch);
	SetThink(&CM203Grenade::Fly);
}

CM203Grenade *CM203Grenade::ShootM203Grenade(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg, BOOL UseGravity)
{
	CM203Grenade *pNew = GetClassPtr((CM203Grenade *)NULL, "m203grenade");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_M203_GRENADE + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;

		if (UseGravity)
		{
			pNew->pev->movetype = MOVETYPE_TOSS;
			pNew->pev->gravity = 0.25;
		}
		else
		{
			pNew->pev->movetype = MOVETYPE_FLY;
			pNew->pev->gravity = 0;
		}

		FX_Trail(pNew->pev->origin, pNew->entindex(), FX_M203 );
	}
	return pNew;
}


int CM203Grenade::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();

		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_BLAST, FX_M203_BLAST);
	return 1;
}

void CM203Grenade::ExplodeTouch(CBaseEntity *pOther)
{
	CGrenade::Impact(pOther, TRUE, pev->dmg*0.3, pev->dmg*2.0, TRUE, RANDOM_LONG(DECAL_EXPSCORCH1,DECAL_EXPSCORCH3), pev->dmg*0.25, DMG_MORTAR, pev->dmg, pev->dmg*2.0, DMG_BLAST, 24.0, FX_M203_DETONATE);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

void CM203Grenade::Fly(void)
{
	CGrenade::SearchTarget();
	pev->nextthink = gpGlobals->time + 0.025;
}