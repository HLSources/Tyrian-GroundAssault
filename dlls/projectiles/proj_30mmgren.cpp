#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "soundent.h"
#include "globals.h"
#include "game.h"
#include "projectiles.h"
#include "msg_fx.h"
#include "decals.h"

LINK_ENTITY_TO_CLASS(Grenade30mm, C30mmGrenade);

void C30mmGrenade::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_30MM_GREN;
	pev->solid = SOLID_SLIDEBOX;
	pev->takedamage = DAMAGE_YES;
	pev->health = 2;
	UTIL_SetSize(this, 2.0f);
	pev->avelocity.z = -256.0;
	pev->nextthink = gpGlobals->time;
	SetTouch(&C30mmGrenade::ExplodeTouch);
	SetThink(&C30mmGrenade::Fly);
}

C30mmGrenade *C30mmGrenade::ShootContact(CBaseEntity *pOwner, const Vector &vecSrc, float spread, float dmg, BOOL UseGravity)
{
	C30mmGrenade *pNew = GetClassPtr((C30mmGrenade *)NULL, "Grenade30mm");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecSrc;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_30MM_GRENADE + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
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
		FX_Trail(pNew->pev->origin, pNew->entindex(), FX_30MMGREN );
	}
	return pNew;
}

int C30mmGrenade::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();

		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_BLAST, FX_30MMGREN_BLAST);
		return 1;
}

void C30mmGrenade::ExplodeTouch(CBaseEntity *pOther)
{
	CGrenade::Impact(pOther, TRUE, pev->dmg*0.33, pev->dmg*1.5, TRUE, RANDOM_LONG(DECAL_MDSCORCH1,DECAL_MDSCORCH3), pev->dmg*0.25, DMG_MORTAR, pev->dmg, pev->dmg*2.0, DMG_BLAST, 24.0, FX_30MMGREN_DETONATE);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

void C30mmGrenade::Fly(void)
{
	CGrenade::SearchTarget();
	pev->nextthink = gpGlobals->time + 0.05;
}