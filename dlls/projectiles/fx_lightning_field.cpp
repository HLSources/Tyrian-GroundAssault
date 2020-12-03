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
#include "sound.h"
#include "gamerules.h"

LINK_ENTITY_TO_CLASS(LightningField, CLightningField);
LINK_ENTITY_TO_CLASS(LightningBlast, CLightningField);

CLightningField *CLightningField::CreateNew(CBaseEntity *pOwner, const Vector &vecStart, float dmg, float life)
{
	CLightningField *pNew = GetClassPtr((CLightningField *)NULL, "LightningField");
	if (pOwner)
	{
		pNew->pev->owner = pOwner->edict();
		pNew->pev->aiment = pNew->pev->owner;
	}

	pNew->pev->dmgtime = life - 1.0;
	pNew->pev->origin = vecStart;
	pNew->pev->velocity = g_vecZero;
	pNew->pev->scale = 4;
	pNew->pev->dmg = dmg;
	pNew->pev->framerate = 1;

	if (pNew->pev->aiment)
		pNew->pev->movetype = MOVETYPE_FOLLOW;
	else
		pNew->pev->movetype = MOVETYPE_NONE;

	pNew->Spawn();
	pNew->SetThink(&CLightningField::LightningFieldThink);
	return pNew;
}

CLightningField *CLightningField::CreateLgtngGib(CBaseEntity *pOwner, const Vector &vecStart, float life)
{
	CLightningField *pNew = GetClassPtr((CLightningField *)NULL, "LightningBlast");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
		}

		pNew->pev->dmgtime = life - 1.0;
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = g_vecZero;
		pNew->pev->scale = 1.5;
		pNew->pev->framerate = 2;
		pNew->pev->dmg = gSkillData.DmgPlrExplode;
		pNew->pev->movetype = MOVETYPE_NOCLIP;
		pNew->Spawn();
		pNew->SetThink(&CLightningField::PlayerElecroDeathThink);
	}
	return pNew;
}

void CLightningField::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/effects/ef_lightning_field.mdl");
	pev->rendermode = kRenderNormal;
	pev->renderfx = kRenderFxFullBright;
	pev->animtime = gpGlobals->time;
	pev->sequence = 0;
	pev->takedamage = DAMAGE_NO;
	pev->solid = SOLID_NOT;
	pev->frags = gpGlobals->time;

	ResetSequenceInfo();
	UTIL_SetSize(this, 0.0f);
	pev->nextthink = gpGlobals->time;
}

void CLightningField::LightningFieldThink(void)
{
	CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
	CBaseEntity *pEnt = NULL;

	if (pev->waterlevel > 0)
	{
		CGrenade::Blast(0, 0, pev->dmg*2.0, pev->dmg*4.0, DMG_SHOCK, 0);
		return;
	}
	if (gpGlobals->time - pev->frags >= pev->dmgtime || (pOwner != NULL && !pOwner->IsAlive()) || (pOwner != NULL && FBitSet(pOwner->pev->flags, FL_FROZEN)))
	{
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time;
		return;
	}

	while((pEnt = UTIL_FindEntityInSphere(pEnt, pev->origin, 768)) != NULL)
	{
		if (pEnt->pev->takedamage == DAMAGE_NO)
			continue;
		if (pEnt == this || pEnt->pev == pev)
			continue;
		if (!pEnt->IsAlive())
			continue;
		if (pEnt->edict() == pev->owner)// don't hit my owner
			continue;
		if (pEnt->pev->owner == pev->owner) // don't hit my projectiles
			continue;
		if (g_pGameRules->IsTeamplay() && pOwner->pev->team == pEnt->pev->team)
			continue;
		
		if (pEnt->pev->waterlevel > 1)// UPD hit even when feet in water
			continue;
		if (pEnt->IsMonster() || pEnt->IsPlayer())
		{
			TraceResult tr;
			UTIL_TraceLine(pev->origin, pEnt->Center(), ignore_monsters, ignore_glass, pev->owner, &tr);
			if (tr.flFraction == 1.0)// visible
			{
				FX_FireLgtng(pev->origin, tr.vecEndPos, entindex(), TYPE_LGTNG_BEAM);
				CGrenade::Impact(pEnt, FALSE, 0, 0, FALSE, 0, pev->dmg, DMG_SHOCK, 0, 0, 0, 0.0, 0);
			}
		}
		else if (pEnt->IsProjectile())
		{
			if (FClassnameIs(pEnt->pev, "monster_tripmine"))
				continue;

			pEnt->TakeDamage(this, pOwner, pev->dmg*2.0, DMG_SHOCK);
		}
	}
	pev->nextthink = gpGlobals->time + 0.2;
}

void CLightningField::PlayerElecroDeathThink(void)
{
	if (!IsInWorld())
	{
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time;
		return;
	}
	if (pev->waterlevel > 0 || gpGlobals->time - pev->frags >= pev->dmgtime)
	{
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time;
		return;
	}
	pev->scale += 0.25;
	CGrenade::Impact(NULL, FALSE, 0, 0, FALSE, 0, 0, 0, pev->dmg*0.2, pev->dmg+(pev->scale*20), DMG_SHOCK, 0.0, 0);
	pev->nextthink = gpGlobals->time + 0.25f;
}