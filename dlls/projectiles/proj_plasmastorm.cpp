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

#define PLASMASTORM_RAD_TIME	3

LINK_ENTITY_TO_CLASS(PlasmaStorm, CPlasmaStorm);

void CPlasmaStorm::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_SPHERE;
	pev->rendermode = kRenderTransAdd;
	pev->renderfx = kRenderFxFullBright;
	pev->renderamt = 200;

	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	pev->takedamage = DAMAGE_YES;
	pev->health = 2;
	pev->impulse = 0;
	UTIL_SetSize(this, 3.0f);
	pev->avelocity.z = 1000.0;
	pev->nextthink = gpGlobals->time;

	SetTouch(&CPlasmaStorm::ExplodeTouch);
	SetThink(&CPlasmaStorm::Fly);
}

CPlasmaStorm *CPlasmaStorm::ShootPlasmaStorm(CBaseEntity *pOwner, const Vector &vecSrc, float spread, float dmg)
{
	CPlasmaStorm *pNew = GetClassPtr((CPlasmaStorm *)NULL, "PlasmaStorm");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_PLASMA_STORM + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
		pNew->pev->origin = vecSrc;
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
		FX_Trail(pNew->pev->origin, pNew->entindex(), FX_PLASMASTORM );
	}
	return pNew;
}

int CPlasmaStorm::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();

		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_PLASMA, FX_PLASMASTORM_BLAST);
	return 1;
}

void CPlasmaStorm::ExplodeTouch(CBaseEntity *pOther)
{
	if (pOther->IsProjectile() && pOther->pev->takedamage != DAMAGE_NO)
	{
		if (FClassnameIs(pOther->pev, "GluonBall") || FClassnameIs(pOther->pev, "DisruptorBall") || FClassnameIs(pOther->pev, "Teleporter") || FClassnameIs(pOther->pev, "LightningBall") || FClassnameIs(pOther->pev, "Scorcher") || FClassnameIs(pOther->pev, "PlasmaBall") || FClassnameIs(pOther->pev, "PlasmaStorm") || FClassnameIs(pOther->pev, "PlasmaShieldCharge"))
		{
			CGrenade::Blast(pev->dmg*0.8, pev->dmg*4.0, pev->dmg*3.0, pev->dmg*5.0, DMG_ENERGYBLAST, FX_PLASMASTORM_COLLIDE);
			return;
		}
	}
	CGrenade::Impact(pOther, TRUE, pev->dmg*0.3, pev->dmg*3.0, TRUE, RANDOM_LONG(DECAL_GLUONSCORCH1,DECAL_GLUONSCORCH3), 0, 0, pev->dmg, pev->dmg*2.5, DMG_PLASMA, 24.0, FX_PLASMASTORM_DETONATE);
	pev->velocity = g_vecZero;
	pev->effects |= EF_NODRAW;
	SetThink(&CPlasmaStorm::RadiationThink);
	pev->nextthink = gpGlobals->time;
}

void CPlasmaStorm::RadiationThink(void)
{
	if (!IsInWorld())
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
			pev->dmgtime = gpGlobals->time + PLASMASTORM_RAD_TIME;
		}
	}
	if (pev->impulse > 0)
	{
		CGrenade::Impact(NULL, FALSE, 0, 0, FALSE, 0, 0, 0, pev->dmg*0.1, pev->dmg*2.0, DMG_RADIATION, 0.0, 0);
	}
	pev->nextthink = gpGlobals->time + 0.25f;
}

void CPlasmaStorm::Fly(void)
{
	if ( POINT_CONTENTS(pev->origin) == CONTENTS_SKY )
	{
		CGrenade::RemoveIfInSky();
		return;
	}
	if (pev->waterlevel > 0)
	{
		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_PLASMA, FX_PLASMASTORM_BLAST);
		return;
	}
	CGrenade::SearchTarget();
	pev->nextthink = gpGlobals->time + 0.01;
}