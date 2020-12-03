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

LINK_ENTITY_TO_CLASS(SonicWave, CSonicWave);

void CSonicWave::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->takedamage = DAMAGE_NO;
	pev->body = PROJ_BODY_SONICWAVE;
	pev->health = 2;
	pev->scale = 1;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 250;
	pev->renderfx = kRenderFxFullBright;
	UTIL_SetSize(this, 3.0f);
	pev->nextthink = gpGlobals->time;

	SetTouch(&CSonicWave::ExplodeTouch);
	SetThink(&CSonicWave::Fly);
}

CSonicWave *CSonicWave::ShootSonicWave(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg)
{
	CSonicWave *pNew = GetClassPtr((CSonicWave *)NULL, "SonicWave");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_SONIC_WAVE  + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
		FX_Trail(pNew->pev->origin, pNew->entindex(), FX_SONICWAVE );
	}
	return pNew;
}


void CSonicWave::ExplodeTouch(CBaseEntity *pOther)
{
	if (pOther->pev->takedamage)
	{
		if (pOther->IsBreakable || pOther->IsPushable)
		{
			pOther->TakeDamage(this, Instance(pev->owner), pev->dmg, DMG_SONIC);
		}
	}

	CGrenade::Impact(pOther, TRUE, 0, 0, FALSE, 0, 0, 0, pev->dmg*0.5, pev->dmg*1.5, DMG_SONIC | DMG_WALLPIERCING, 20.0, FX_SONICWAVE_DETONATE);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

void CSonicWave::Fly(void)
{
	if (POINT_CONTENTS(pev->origin) == CONTENTS_SKY )
	{
		CGrenade::RemoveIfInSky();
		return;
	}

	CBaseEntity *pEnt = NULL;
	while((pEnt = UTIL_FindEntityInSphere(pEnt, pev->origin, PROJ_SEARCH_RADIUS + (pev->scale*10))) != NULL)
	{
		if (pEnt->pev->takedamage == DAMAGE_NO)
			continue;

		if (pEnt == this || pEnt->pev == pev)
			continue;

		if (!pEnt->IsAlive())
			continue;

		if (pEnt->edict() == pev->owner)
			continue;

		if (pEnt->pev->waterlevel > 1)
			continue;

		if (pEnt->IsMonster() || pEnt->IsPlayer())
		{
			CGrenade::Impact(pEnt, FALSE, 0, 0, FALSE, 0, pev->dmg, DMG_SONIC, 0, 0, 0, 0.0, 0);
		}
		else if (pEnt->IsProjectile())
		{
			if (pEnt->pev->classname == pev->classname)// another wave
				continue;

			pEnt->TakeDamage(this, Instance(pev->owner), pev->dmg, DMG_SONIC);
		}
	}
	if (pev->scale > 10)
		pev->scale = 10;
	else
		pev->scale += 0.15;

	if (pev->renderamt < 20)
		pev->renderamt = 20;
	else
		pev->renderamt -= 5;

	pev->dmg-=2;
	if (pev->dmg <=1)
		CGrenade::Blast(0, 0, 0, 0, 0, FX_SONICWAVE_DETONATE);

	pev->nextthink = gpGlobals->time + 0.01;
}