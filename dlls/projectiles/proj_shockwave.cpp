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

LINK_ENTITY_TO_CLASS(ShockWave, CShockWave);

void CShockWave::Spawn(void)
{
	SET_MODEL(ENT(pev), "sprites/anim_spr10.spr"); 
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 20;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	UTIL_SetSize(this, 1.0f);
	pev->nextthink = gpGlobals->time;
}

CShockWave *CShockWave::ShootShockWave(CBaseEntity *pOwner, const Vector &vecStart, float dmg)
{
	CShockWave *pNew = GetClassPtr((CShockWave *)NULL, "ShockWave");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->dmg = dmg;
		pNew->SetThink (&CShockWave::Detonate);
		pNew->pev->nextthink = gpGlobals->time;
	}
	return pNew;
}

void CShockWave::Detonate(void)
{
	CGrenade::Impact(NULL, TRUE, pev->dmg*0.5, pev->dmg*3.0, FALSE, 0, 0, 0, pev->dmg, pev->dmg*2.5, DMG_SHOCK | DMG_NOSELF, 0.0, FX_SHOCKWAVE_DETONATE);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

