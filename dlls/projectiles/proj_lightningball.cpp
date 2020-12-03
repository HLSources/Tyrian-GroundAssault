#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "gamerules.h"
#include "decals.h"
#include "explode.h"
#include "game.h"
#include "msg_fx.h"
#include "projectiles.h"
#include "globals.h"

#define LIGHTNINGBALL_SEEK_RADIUS	1024

LINK_ENTITY_TO_CLASS(LightningBall, CLightningBall);

CLightningBall *CLightningBall::Fire(CBaseEntity *pOwner, CBaseEntity *pEnemy, const Vector &vecSrc, float dmg, float dmgBeam)
{
	CLightningBall *pNew = GetClassPtr((CLightningBall *)NULL, "LightningBall");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;// XDM3037
		}
		pNew->Spawn();
		pNew->pev->origin = vecSrc;
		pNew->m_hEnemy = pEnemy;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_LIGHTNING_BALL;
		pNew->pev->dmg = dmg;
		pNew->pev->dmg_save = dmgBeam;
		FX_Trail(pNew->pev->origin, pNew->entindex(), FX_LIGHTNINGBALL );
	}
	return pNew;
}

void CLightningBall::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_SPHERE;
	pev->rendermode = kRenderTransAdd;
	pev->renderfx = kRenderFxFullBright;
	pev->renderamt = 200;
	pev->health = 50;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	pev->takedamage = DAMAGE_YES;
	UTIL_SetSize(this, 3.0);

	SetThink(&CLightningBall::Fly);
	SetTouch(&CLightningBall::ExplodeTouch);
	pev->nextthink = gpGlobals->time + 0.1;
}

void CLightningBall::ExplodeTouch(CBaseEntity *pOther)
{
	CGrenade::Impact(pOther, TRUE, pev->dmg*0.1f, pev->dmg*3.5f, TRUE, RANDOM_LONG(DECAL_SCORCH1,DECAL_SCORCH3), 0, 0, pev->dmg, pev->dmg*1.5, DMG_SHOCK, 24.0, FX_LIGHTNINGBALL_DETONATE);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

int CLightningBall::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();

		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_SHOCK, FX_LIGHTNINGBALL_BLAST);
	return 1;
}

void CLightningBall::Fly(void)
{
	if (POINT_CONTENTS(pev->origin) == CONTENTS_SKY )
	{
		CGrenade::RemoveIfInSky();
		return;
	}
	if (pev->waterlevel > 0)
	{
		CGrenade::Blast(0, 0, pev->dmg*2.0, pev->dmg*5.0, DMG_SHOCK, FX_LIGHTNINGBALL_BLAST);
		return;
	}

	CBaseEntity *pEnt = NULL;
	CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);

	if (pev->dmgtime <= gpGlobals->time)
	{
		while((pEnt = UTIL_FindEntityInSphere(pEnt, pev->origin, LIGHTNINGBALL_SEEK_RADIUS)) != NULL)
		{
			if (pEnt->pev->takedamage == DAMAGE_NO)
				continue;

			if (pEnt == this || pEnt->pev == pev)
				continue;

			if (!pEnt->IsAlive())
				continue;

			if (pEnt->edict() == pev->owner)// don't hit my owner
				continue;

			if (pEnt->pev->team == pOwner->pev->team) // don't hit my teammates
				continue;

			if (pEnt->pev->waterlevel > 1)// UPD hit even when feet in water
				continue;

			if (pEnt->IsMonster() || pEnt->IsPlayer())
			{
				TraceResult tr;
				UTIL_TraceLine(pev->origin, pEnt->Center(), ignore_monsters, ignore_glass, pev->owner, &tr);

				if (tr.flFraction == 1.0)// visible
				{
					FX_FireLgtng(pev->origin, tr.vecEndPos, entindex(), TYPE_LIGHTNING_BALL_BEAM);
					CGrenade::Impact(pEnt, FALSE, 0, 0, FALSE, 0, pev->dmg_save, DMG_SHOCK, 0, 0, 0, 0.0, 0);
				}
			}
			else if (pEnt->IsProjectile())
			{
				if (pEnt->pev->classname == pev->classname)// another ball
				{
					if (pEnt->pev->owner == pev->owner)// same owner
						continue;
				}
				pEnt->TakeDamage(this, Instance(pev->owner), pev->dmg, DMG_SHOCK);
			}
		}
		pev->dmgtime = gpGlobals->time + 0.25f;
	}

	if (m_hEnemy != NULL)
	{
		if (m_hEnemy->IsAlive())
			CGrenade::MovetoTarget(m_hEnemy->Center(), SPEED_LIGHTNING_BALL);
		else
			m_hEnemy = NULL;
	}
	pev->angles = UTIL_VecToAngles(pev->velocity);
	pev->nextthink = gpGlobals->time + 0.05;
}

int CLightningBall::ShouldCollide(CBaseEntity *pOther)// XDM3035
{
	if (pOther->pev->solid == SOLID_NOT)
		return 0;

	if (pOther->IsPlayer() && !pOther->IsAlive())// some disintegrating players block entities
		return 0;

	return CGrenade::ShouldCollide(pOther);
}