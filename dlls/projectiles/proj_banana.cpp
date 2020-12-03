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

LINK_ENTITY_TO_CLASS(Banana, CBanana);

void CBanana::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_BANANA;
	pev->takedamage = DAMAGE_YES;
	pev->health = 2;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	pev->avelocity.z = -800.0;
	UTIL_SetSize(this, 2.0f);
	pev->nextthink = gpGlobals->time;

	SetTouch(&CBanana::ExplodeTouch);
	SetThink(&CBanana::Fly);
}

CBanana *CBanana::ShootBanana(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg)
{
	CBanana *pNew = GetClassPtr((CBanana *)NULL, "Banana");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_BANANA + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
	}
	return pNew;
}


int CBanana::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

		CGrenade::Blast(0, 0, 0, 0, 0, FX_BANANA_BLAST);
	return 1;
}


void CBanana::ExplodeTouch(CBaseEntity *pOther)
{
	CGrenade::Impact(pOther, TRUE, 0, 0, TRUE, RANDOM_LONG(DECAL_ACID_SPLAT1,DECAL_ACID_SPLAT3), 0, 0, pev->dmg, pev->dmg*2.0, DMG_POISON, 16.0, FX_BANANA_DETONATE);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

void CBanana::Fly(void)
{
	CGrenade::SearchTarget();
	pev->nextthink = gpGlobals->time + 0.075;
}