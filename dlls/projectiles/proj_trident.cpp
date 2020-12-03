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
#include "sound.h"

LINK_ENTITY_TO_CLASS(Trident, CTrident);

void CTrident::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_SPHERE;
	pev->rendermode = kRenderTransAdd;
	pev->renderfx = kRenderFxFullBright;
	pev->renderamt = 25;
	pev->health = 10;
	pev->scale = 0.5;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	pev->takedamage = DAMAGE_YES;
	UTIL_SetSize(this, 2.0);
	pev->nextthink = gpGlobals->time;

	SetTouch(&CTrident::ExplodeTouch);
	SetThink(&CTrident::Fly);
}

CTrident *CTrident::ShootTrident(CBaseEntity *pOwner, const Vector &vecStart, const Vector &vecVel, float dmg)
{
	CTrident *pNew = GetClassPtr((CTrident *)NULL, "Trident");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = vecVel;
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
		FX_Trail(pNew->pev->origin, pNew->entindex(), FX_TRIDENT );
	}
	return pNew;
}

int CTrident::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();

		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_ENERGYBLAST, FX_TRIDENT_BLAST);
	return 1;
}

void CTrident::ExplodeTouch(CBaseEntity *pOther)
{
	if (pOther->IsProjectile())
	{
		if (FClassnameIs(pOther->pev, "LightningBall") || FClassnameIs(pOther->pev, "GluonBall") || FClassnameIs(pOther->pev, "FrostBall") || FClassnameIs(pOther->pev, "PlasmaStorm") || FClassnameIs(pOther->pev, "PulseCannon") || FClassnameIs(pOther->pev, "PlasmaShieldCharge") || FClassnameIs(pOther->pev, "DisruptorBall") || FClassnameIs(pOther->pev, "PlasmaBall"))
		{
			CGrenade::Blast(pev->dmg*0.5, pev->dmg*7.0, pev->dmg*4.0, pev->dmg*5.0, DMG_ENERGYBLAST, FX_TRIDENT_COLLIDE);
			return;
		}
	}
	FX_Trail( pev->origin, entindex(), FX_REMOVE );
	TraceResult tr;
	UTIL_TraceLine( pev->origin, pev->origin + pev->velocity.Normalize() * 64, dont_ignore_monsters, ENT(pev), &tr );

	pev->takedamage = DAMAGE_NO;
	pev->velocity = g_vecZero;
	SetTouchNull();
	SetThinkNull();

	CBaseEntity *pOwner = NULL;
	if (pev->owner)
		pOwner = CBaseEntity::Instance(pev->owner);
	else 
		pOwner = g_pWorld;

	::RadiusDamage(pev->origin, this, pOwner, pev->dmg, pev->dmg*2.5, CLASS_NONE, DMG_ENERGYBLAST);
	FX_FireBeam( pev->origin, tr.vecEndPos, tr.vecPlaneNormal, CHAR_TEX_CONCRETE, BULLET_TRIDENT, FALSE);

	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

void CTrident::Fly( void )
{
	if ( POINT_CONTENTS(pev->origin) == CONTENTS_SKY )
	{
		CGrenade::RemoveIfInSky();
		return;
	}
	if (pev->waterlevel > 0)
	{
		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_ENERGYBLAST, FX_TRIDENT_BLAST);
		return;
	}
	CGrenade::SearchTarget();
	pev->nextthink = gpGlobals->time + 0.01;
}