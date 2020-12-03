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
#include "maprules.h"
#include "globals.h"

#define TELEENTER_DETONATE_DELAY	RANDOM_FLOAT(3.0,5.0)

LINK_ENTITY_TO_CLASS(Teleporter, CTeleporter);

CTeleporter *CTeleporter::Fire(CBaseEntity *pOwner, CBaseEntity *pEnemy, const Vector &vecSrc, float dmg)
{
	CTeleporter *pNew = GetClassPtr((CTeleporter *)NULL, "Teleporter");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->dmg_inflictor = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}
		pNew->Spawn();
		pNew->pev->origin = vecSrc;
		pNew->m_hEnemy = pEnemy;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_TELEPORTER;
		pNew->pev->dmg = dmg;
		pNew->pev->impulse = 0;
		FX_Trail(pNew->pev->origin, pNew->entindex(), FX_TELEPORT );

		pNew->SetThink(&CTeleporter::Fly);
		pNew->SetTouch(&CTeleporter::TeleBallTouch);
		pNew->pev->nextthink = gpGlobals->time + 0.1;
	}
	return pNew;
}

CTeleporter *CTeleporter::FireTeleEnter(CBaseEntity *pOwner, const Vector &vecSrc, float dmg, const Vector &vecDestOrigin, const Vector &vecDestAngles)
{
	CTeleporter *pNew = GetClassPtr((CTeleporter *)NULL, "Teleporter");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->dmg_inflictor = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}
		pNew->Spawn();
		pNew->pev->origin = vecSrc;
		pNew->pev->velocity = gpGlobals->v_up * -1;
		pNew->pev->dmg = dmg;
		pNew->pev->impulse = 1;
		pNew->m_vecDestOrigin = vecDestOrigin;
		pNew->m_vecDestAngles = vecDestAngles;
		FX_Trail(pNew->pev->origin, pNew->entindex(), FX_TELEPORT_ENTER);
		pNew->SetThink(&CTeleporter::Blast);
		pNew->SetTouch(&CTeleporter::TeleEnterTouch);
		pNew->pev->nextthink = gpGlobals->time + TELEENTER_DETONATE_DELAY;
	}
	return pNew;
}

void CTeleporter::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_SPHERE;
	pev->rendermode = kRenderTransAdd;
	pev->renderfx = kRenderFxFullBright;
	pev->renderamt = 20;
	pev->health = 10;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	pev->takedamage = DAMAGE_YES;
	UTIL_SetSize(this, 4.0);
}

void CTeleporter::Fly(void)
{
	if (POINT_CONTENTS(pev->origin) == CONTENTS_SKY )
	{
		CGrenade::RemoveIfInSky();
		return;
	}
	if (pev->waterlevel > 0)
	{
		Blast();
		return;
	}

	if (m_hEnemy != NULL)
	{
		if (m_hEnemy->IsAlive())
			CGrenade::MovetoTarget(m_hEnemy->Center(), SPEED_TELEPORTER);
		else
			m_hEnemy = NULL;
	}
	pev->nextthink = gpGlobals->time + 0.05;
}

void CTeleporter::TeleBallTouch(CBaseEntity *pOther)
{
	if (pOther->IsProjectile())
	{
		if (FClassnameIs(pOther->pev, "NeedleLaser") || FClassnameIs(pOther->pev, "Teleporter") || FClassnameIs(pOther->pev, "ChargeCannon") || FClassnameIs(pOther->pev, "PulseCannon") || FClassnameIs(pOther->pev, "PlasmaStorm") || FClassnameIs(pOther->pev, "PlasmaBall") || FClassnameIs(pOther->pev, "PlasmaShieldCharge") || FClassnameIs(pOther->pev, "LightningBall"))
		{
			CGrenade::Impact(NULL, TRUE, pev->dmg*0.2, pev->dmg*4.0, FALSE, 0, 0, 0, pev->dmg*2.0, pev->dmg*3.0, DMG_ENERGYBLAST, 0.0, FX_TELEPORT_COLLIDE);
			SetThink(&CBaseEntity::SUB_Remove);
			pev->nextthink = gpGlobals->time;
			return;
		}
	}

	if (pOther->IsMonster() || pOther->IsPlayer())
	{
		CBaseEntity *pOwner = NULL;
		if (pev->owner)
			pOwner = CBaseEntity::Instance(pev->owner);
		else 
			pOwner = g_pWorld;

		vec_t flDist; 
		vec_t fMaxDist = 512;
		CBaseEntity *pSpot = NULL;
		CBaseEntity *pSpotSlected = NULL;
		while ((pSpot = UTIL_FindEntityByClassname(pSpot, "info_player_deathmatch")) != NULL)
		{
			if (pSpot->pev->team != TEAM_NONE && pSpot->pev->team == pOwner->pev->team)
				continue;

			flDist = (pSpot->pev->origin - pOwner->pev->origin).Length();
			if (flDist > fMaxDist)
			{
				fMaxDist = flDist;
				pSpotSlected = pSpot;
			}
		}

		if (pSpotSlected)
		{
			CTeleporter::FireTeleEnter(pOwner, pev->origin, pev->dmg, pSpotSlected->pev->origin,pSpotSlected->pev->angles);
			UTIL_SetOrigin(pOther->pev, pSpotSlected->pev->origin);
			UTIL_SetOrigin(pev, pSpotSlected->pev->origin);
			pOther->pev->angles = pSpotSlected->pev->angles;
			pOther->pev->velocity = pOther->pev->basevelocity = g_vecZero;
			CGrenade::Blast(0, 0, pev->dmg, pev->dmg*2.0, DMG_DISINTEGRATING, FX_TELEPORT_USE);
		}
		return;
	}
	CGrenade::Impact(NULL, TRUE, pev->dmg*0.4, pev->dmg*2.5, TRUE, RANDOM_LONG(DECAL_NUCBLOW1,DECAL_NUCBLOW3), 0, 0, pev->dmg, pev->dmg*1.75, DMG_DISINTEGRATING, 24.0, FX_TELEPORT_DETONATE);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

void CTeleporter::TeleEnterTouch(CBaseEntity *pOther)
{
	if (pOther->IsMonster() || pOther->IsPlayer())
	{
		FX_Trail( pev->origin, entindex(), FX_TELEPORT_ENTER_USE);
		UTIL_SetOrigin(pOther->pev, m_vecDestOrigin);
		pOther->pev->angles = m_vecDestAngles;
		pOther->pev->velocity = pOther->pev->basevelocity = g_vecZero;
	}
	else
		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_DISINTEGRATING, FX_TELEPORT_ENTER_DETONATE);
}

void CTeleporter::Blast(void)
{
	pev->takedamage	= DAMAGE_NO;
	FX_Trail(pev->origin, entindex(), pev->impulse?FX_TELEPORT_ENTER_DETONATE:FX_TELEPORT_BLAST);
	::RadiusDamage(pev->origin, this, Instance(pev->dmg_inflictor), pev->dmg/2, pev->dmg, CLASS_NONE, DMG_DISINTEGRATING);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}


int CTeleporter::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();

	if (bitsDamageType & (DMG_ENERGYBLAST | DMG_ENERGYBEAM))
	{
		CGrenade::Impact(NULL, TRUE, pev->dmg*0.2, pev->dmg*4.0, FALSE, 0, 0, 0, pev->dmg*2.0, pev->dmg*3.0, DMG_ENERGYBLAST, 0.0, FX_TELEPORT_COLLIDE);
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time;
		return 0;
	}
		Blast();
	return 1;
}

int CTeleporter::ShouldCollide(CBaseEntity *pOther)// XDM3035
{
	if (pOther->pev->solid == SOLID_NOT)
		return 0;

	if (pOther->IsPlayer() && !pOther->IsAlive())// some disintegrating players block entities
		return 0;

	return CGrenade::ShouldCollide(pOther);
}