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

#define RIPPER_DETONATE_DELAY	3.0

LINK_ENTITY_TO_CLASS(Ripper, CRipper);

void CRipper::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_RIPPER;
	pev->movetype = MOVETYPE_BOUNCEMISSILE;
	pev->solid = SOLID_SLIDEBOX;
	pev->takedamage = DAMAGE_YES;
	pev->health = 2;
	pev->frags = gpGlobals->time;
	pev->button = 5;
	UTIL_SetSize(this, 3.0f);
	pev->avelocity.y = 2048.0;
	pev->nextthink = gpGlobals->time;

	SetTouch(&CRipper::ExplodeTouch);
	SetThink(&CRipper::Fly);
}

CRipper *CRipper::ShootRipper(CBaseEntity *pOwner, const Vector &vecSrc, float spread, float dmg)
{
	CRipper *pNew = GetClassPtr((CRipper *)NULL, "Ripper");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecSrc;
		pNew->pev->dmg = dmg;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_RIPPER  + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		FX_Trail(pNew->pev->origin, pNew->entindex(), FX_RIPPER);
	}
	return pNew;
}

int CRipper::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;


		CGrenade::Blast(0, 0, 0, 0, 0, FX_RIPPER_DETONATE);
	return 1;
}

void CRipper::ExplodeTouch(CBaseEntity *pOther)
{
	TraceResult tr = UTIL_GetGlobalTrace();
	UTIL_TraceLine( pev->origin, pev->origin + pev->velocity.Normalize() * 64, dont_ignore_monsters, ENT(pev), &tr );
	CGrenade::Impact(pOther, FALSE, 0, 0, TRUE, DECAL_GARGSTOMP1, pev->dmg, DMG_SLASH, 0, 0, 0, 12.0, FX_RIPPER_HIT);
	pev->button--;

	if (pev->button <= 0)
		CGrenade::Blast(0, 0, 0, 0, 0, FX_RIPPER_DETONATE);

	pev->velocity = 2*tr.vecPlaneNormal + pev->velocity;
}

void CRipper::Fly(void)
{
	if (gpGlobals->time - pev->frags >= RIPPER_DETONATE_DELAY)
	{
		CGrenade::Blast(0, 0, 0, 0, 0, FX_RIPPER_DETONATE);
		return;
	}
	CGrenade::SearchTarget();
	pev->nextthink = gpGlobals->time + 0.05;
}