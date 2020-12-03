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
#include "sound.h"

LINK_ENTITY_TO_CLASS(PlasmaShieldCharge, CPlasmaShieldCharge);

void CPlasmaShieldCharge::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_PLASMA_CHARGE;
	pev->movetype = MOVETYPE_FLYMISSILE;
	pev->solid = SOLID_SLIDEBOX;
	pev->takedamage = DAMAGE_NO;
	pev->health = 2;
	pev->avelocity.z = -1024.0;
	UTIL_SetSize(this, 3.0f);
	pev->nextthink = gpGlobals->time+0.05;

	SetTouch(&CPlasmaShieldCharge::ExplodeTouch);
	SetThink(&CPlasmaShieldCharge::Fly);
}

CPlasmaShieldCharge *CPlasmaShieldCharge::CreateNew(const Vector &vecSrc, const Vector &vecAng, const Vector &vecDir, CBaseEntity *pOwner, float dmg, float spread)
{
	CPlasmaShieldCharge *pNew = GetClassPtr((CPlasmaShieldCharge *)NULL, "PlasmaShieldCharge");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->pev->origin = vecSrc;
		pNew->pev->angles = vecAng;
		pNew->pev->movedir = vecDir;
		pNew->pev->dmg = dmg;
		pNew->Spawn();

		pNew->pev->basevelocity = pNew->pev->velocity = vecDir * SPEED_PLASMA_SHIELD_CHARGE + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
		FX_Trail(pNew->pev->origin, pNew->entindex(), FX_PLASMA_SHIELD_CHARGE);
	}
	return pNew;
}

int CPlasmaShieldCharge::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg*0.75, DMG_PLASMA, FX_PLASMA_SHIELD_CHARGE_BLAST);
	return 1;
}

void CPlasmaShieldCharge::ExplodeTouch(CBaseEntity *pOther)
{
	CGrenade::Impact(pOther, TRUE, 0, 0, TRUE, RANDOM_LONG(DECAL_MINISCORCH1,DECAL_MINISCORCH3), 0, 0, pev->dmg, pev->dmg*1.5, DMG_PLASMA, 24.0, FX_PLASMA_SHIELD_CHARGE_DETONATE);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

void CPlasmaShieldCharge::Fly(void)
{
	if (POINT_CONTENTS(pev->origin) == CONTENTS_SKY )
	{
		CGrenade::RemoveIfInSky();
		return;
	}
	if (pev->waterlevel > 0)
	{
		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg*0.5, DMG_PLASMA, FX_PLASMA_SHIELD_CHARGE_BLAST);
		return;
	}
	pev->takedamage = DAMAGE_YES;
	CGrenade::SearchTarget();
	pev->nextthink = gpGlobals->time + 0.01;
}