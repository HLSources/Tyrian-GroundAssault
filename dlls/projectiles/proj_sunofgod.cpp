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

#define SOG_RADIATION_TIME	7.5

LINK_ENTITY_TO_CLASS(SunOfGod, CSunOfGod);

void CSunOfGod::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_SPHERE;
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 255;
	pev->frags = 32;
	pev->health = 50;
	pev->impulse = 0;
	pev->movetype = MOVETYPE_FLY;
	pev->takedamage	= DAMAGE_YES;
	pev->solid = SOLID_SLIDEBOX;
	UTIL_SetSize(this, 3.0f);
}

CSunOfGod *CSunOfGod::ShootSunOfGod(CBaseEntity *pOwner, const Vector &vecStart, float dmg)
{
	CSunOfGod *pNew = GetClassPtr((CSunOfGod *)NULL, "SunOfGod");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_SUN_OF_GOD;
		pNew->pev->dmg = dmg;
		pNew->pev->nextthink = gpGlobals->time + 5.0;
		pNew->SetThink(DestroyContainer);
		pNew->SetTouch(BounceTouch);
		FX_Trail(pNew->pev->origin, pNew->entindex(), FX_SUNOFGOD);
	}
	return pNew;
}

void CSunOfGod::BounceTouch(CBaseEntity *pOther)
{
	if (pOther->edict() == pev->owner)
		return;

	TraceResult tr;
	Vector vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	Vector vecEnd = pev->origin + pev->velocity.Normalize() * 64;
	UTIL_TraceLine( vecSpot, vecEnd, ignore_monsters, ENT(pev), &tr );
	UTIL_DecalTrace(&tr, DECAL_SCORCH1 + RANDOM_LONG(0,2));
	pev->origin = tr.vecEndPos + (tr.vecPlaneNormal * 60);

	SetThink(&CSunOfGod::DestroyContainer);
	pev->nextthink = gpGlobals->time;
}

void CSunOfGod::DestroyContainer( void )
{
	if (pev->waterlevel > 0)
	{
		CGrenade::Blast(0, 0, pev->dmg*0.75, pev->dmg*0.5, DMG_ENERGYBLAST, FX_SUNOFGOD_BLAST);
		return;
	}
	SetTouchNull();
	pev->effects |= EF_NODRAW;
	pev->velocity = g_vecZero;
	pev->takedamage	= DAMAGE_NO;
	EMIT_SOUND(ENT(pev), CHAN_ITEM, "weapons/fire_sunofgod.wav", 1, ATTN_LOW); 
	FX_Trail(pev->origin, entindex(), FX_SUNOFGOD_PREPARE_TO_DETONATE );
	SetThink(BurnThink);
	pev->nextthink = gpGlobals->time;
}

void CSunOfGod::BurnThink( void )
{
	if (!IsInWorld())
	{
		FX_Trail( pev->origin, entindex(), FX_REMOVE );
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time;
		return;
	}
	if ( gpGlobals->time >= pev->framerate )
	{
		EMIT_SOUND(ENT(pev), CHAN_STATIC, "weapons/fly_sunofgod.wav", 1, ATTN_LOW_HIGH); 
		pev->framerate = gpGlobals->time + 2.1;
	}
	CGrenade::Impact(NULL, FALSE, 0, 0, FALSE, 0, 0, 0, 256, 256, DMG_BURN | DMG_SLOWBURN, 0.0, 0);
	pev->frags--;

	if (pev->frags <= 0)
	{
		STOP_SOUND( edict(), CHAN_STATIC, "weapons/fly_sunofgod.wav" );
		CGrenade::Impact(NULL, TRUE, pev->dmg*0.15, pev->dmg*4.5, FALSE, 0, 0, 0, pev->dmg, pev->dmg, DMG_VAPOURIZING, 0.0, FX_SUNOFGOD_DETONATE);
		pev->velocity = g_vecZero;
		pev->effects |= EF_NODRAW;
		SetThink(&CSunOfGod::RadiationThink);
		pev->nextthink = gpGlobals->time;
	}

	pev->nextthink = gpGlobals->time + 0.25;
}

int CSunOfGod::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();

		CGrenade::Blast(0, 0, pev->dmg*0.75, pev->dmg*0.5, DMG_ENERGYBLAST, FX_SUNOFGOD_BLAST);
	return 1;
}

void CSunOfGod::RadiationThink(void)
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
			pev->dmgtime = gpGlobals->time + SOG_RADIATION_TIME;
		}
	}
	if (pev->impulse > 0)
	{
		CGrenade::Impact(NULL, FALSE, 0, 0, FALSE, 0, 0, 0, pev->dmg*0.02, pev->dmg*0.8, DMG_RADIATION, 0.0, 0);
	}
	pev->nextthink = gpGlobals->time + 0.25f;
}