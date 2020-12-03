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

LINK_ENTITY_TO_CLASS(Toilet, CToilet);

void CToilet::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles_alt.mdl");
	pev->body = RANDOM_LONG(0, GetEntBodyCount(edict())-1);
	pev->takedamage = DAMAGE_YES;
	pev->health = 50;
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_SLIDEBOX;
	pev->avelocity.x = -150.0;
	UTIL_SetSize(this, 4.0f);
	pev->nextthink = gpGlobals->time + 0.15;

	SetTouch(&CToilet::ExplodeTouch);
	SetThink(&CToilet::Fly);
}

CToilet *CToilet::ShootToilet(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg, BOOL LowGravity)
{
	CToilet *pNew = GetClassPtr((CToilet *)NULL, "Toilet");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_TOILET + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
		FX_Trail(pNew->pev->origin, pNew->entindex(), FX_TOILET);

		if (LowGravity)
			pNew->pev->gravity = 0.5;
		else
			pNew->pev->gravity = 0.2;
	}
	
	return pNew;
}

int CToilet::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();

		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_CRUSH, FX_TOILET_BLAST);
	return 1;
}

void CToilet::ExplodeTouch(CBaseEntity *pOther)
{
	CGrenade::Impact(pOther, TRUE, 0, 0, TRUE, DECAL_GARGSTOMP1, 0, 0, pev->dmg, pev->dmg*1.2, DMG_CRUSH, 16.0, FX_TOILET_DETONATE);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

void CToilet::Fly(void)
{
	if ( gpGlobals->time >= pev->framerate )
	{
		EMIT_SOUND(ENT(pev), CHAN_BODY, "weapons/fly_toilet.wav", VOL_NORM, ATTN_NORM);
		pev->framerate = gpGlobals->time + 1.45;
	}
	pev->nextthink = gpGlobals->time + 0.1;
}