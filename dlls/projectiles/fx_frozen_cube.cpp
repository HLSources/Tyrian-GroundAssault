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

LINK_ENTITY_TO_CLASS(FrozenCube, CFrozenCube);

CFrozenCube *CFrozenCube::CreateNew(CBaseEntity *pOwner, const Vector &vecStart, float life)
{
	CFrozenCube *pFrozenCube = GetClassPtr((CFrozenCube *)NULL, "FrozenCube");
	if (pOwner)
	{
		pFrozenCube->pev->owner = pOwner->edict();
		pFrozenCube->pev->aiment = pFrozenCube->pev->owner;
	}
	pFrozenCube->Spawn();
	pFrozenCube->pev->animtime = life + 1.0f;
	pFrozenCube->pev->origin = vecStart;
	pFrozenCube->pev->velocity = g_vecZero;
	return pFrozenCube;
}

void CFrozenCube::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/effects/ef_explosions.mdl");
	pev->body = BLAST_MDL_FROZEN_CUBE;
	pev->rendermode = kRenderNormal;

	pev->takedamage = DAMAGE_NO;
	if (pev->aiment)
		pev->movetype = MOVETYPE_FOLLOW;
	else
		pev->movetype = MOVETYPE_NONE;

	pev->solid = SOLID_NOT;
	pev->frags = gpGlobals->time;

	UTIL_SetSize(this, 0.0f);
	SetThink(&CFrozenCube::OnUpdate);
	pev->nextthink = gpGlobals->time;
}

void CFrozenCube::OnUpdate(void)
{
	CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);

	if (gpGlobals->time - pev->frags >= pev->animtime || (pOwner != NULL && !pOwner->IsAlive()) || (pOwner != NULL && !FBitSet(pOwner->pev->flags, FL_FROZEN)))
	{
		Destroy();
		return;
	}
	pev->nextthink = gpGlobals->time + 0.05;
}

void CFrozenCube::Destroy(void)
{
	FX_Trail( pev->origin, entindex(), FX_PLAYER_FROZENCUBE_BLAST);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}