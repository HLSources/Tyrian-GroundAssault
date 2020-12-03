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

#define SCORCHER_DETONATE_DELAY	3.0

LINK_ENTITY_TO_CLASS(Scorcher, CScorcher);

void CScorcher::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_SPHERE;
	pev->rendermode = kRenderTransAdd;
	pev->renderfx = kRenderFxFullBright;
	pev->renderamt = 32;
	pev->scale = 0.25;
	pev->takedamage = DAMAGE_YES;
	pev->health = 10;
	pev->frags = gpGlobals->time;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	UTIL_SetSize(this, 3.0f);
	pev->nextthink = gpGlobals->time;

	SetTouch(&CScorcher::ExplodeTouch);
	SetThink(&CScorcher::Fly);
}

CScorcher *CScorcher::ShootScorcher(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg)
{
	CScorcher *pNew = GetClassPtr((CScorcher *)NULL, "Scorcher");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_SCORCHER;
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
		FX_Trail(pNew->pev->origin, pNew->entindex(), FX_SCORCHER );
	}
	return pNew;
}


int CScorcher::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();

		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_BURN, FX_SCORCHER_BLAST);
	return 1;
}

void CScorcher::ExplodeTouch(CBaseEntity *pOther)
{
	CGrenade::Impact(pOther, TRUE, pev->dmg*0.2, pev->dmg*2.5, TRUE, RANDOM_LONG(DECAL_EXPSCORCH1,DECAL_EXPSCORCH3), 0, 0, pev->dmg, pev->dmg*2.0, DMG_BURN | DMG_SLOWBURN, 20.0, FX_SCORCHER_DETONATE);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

void CScorcher::Fly( void )
{
	if ( POINT_CONTENTS(pev->origin) == CONTENTS_SKY )
	{
		CGrenade::RemoveIfInSky();
		return;
	}
	if (pev->waterlevel > 0)
	{
		CGrenade::Blast(0, 0, 0, 0, 0, FX_SCORCHER_BLAST);
		return;
	}
	CGrenade::SearchTarget();
	pev->nextthink = gpGlobals->time + 0.01;
}