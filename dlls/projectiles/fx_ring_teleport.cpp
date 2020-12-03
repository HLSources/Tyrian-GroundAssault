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
#include "player.h"

enum teleport_anim
{
	TELEPORT_IDLE = 0,
	TELEPORT_WORK,
};

LINK_ENTITY_TO_CLASS(RingTeleport, CRingTeleport);

CRingTeleport *CRingTeleport::CreateNew(CBaseEntity *pOwner, const Vector &vecStart, BOOL SummonBomb)
{
	CRingTeleport *pNew = GetClassPtr((CRingTeleport *)NULL, "RingTeleport");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = g_vecZero;
		pNew->pev->button = SummonBomb;
	}
	return pNew;
}

void CRingTeleport::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/effects/ef_teleporter.mdl");
	pev->rendermode = kRenderTransAdd;
	pev->renderfx = kRenderFxFullBright;
	pev->renderamt = 200;
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_BBOX;

	pev->animtime = gpGlobals->time;
	pev->framerate = 1;
	pev->sequence = TELEPORT_WORK;

	ResetSequenceInfo();
	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 64));
	SetThink(&CRingTeleport::StartFx);
	pev->nextthink = gpGlobals->time;
}

void CRingTeleport::StartFx(void)
{
	CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
	if (pOwner && !pev->button)
	{
		((CBasePlayer *)pOwner)->pev->flags	|= (FL_NOTARGET | FL_TELEPORTING);
		((CBasePlayer *)pOwner)->EnableControl(FALSE);
		((CBasePlayer *)pOwner)->pev->takedamage = DAMAGE_NO;
		((CBasePlayer *)pOwner)->pev->renderfx = kRenderFxGlowShell;
		((CBasePlayer *)pOwner)->pev->rendermode = kRenderTransColor;
		((CBasePlayer *)pOwner)->pev->renderamt = INVISIBILITY_RENDERAMT;
	}
	DoDmg();

	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_ringteleport.wav", VOL_NORM, ATTN_LOW_HIGH);

	SetThink(&CRingTeleport::TeleportFx);
	pev->nextthink = gpGlobals->time+1.8;
}

void CRingTeleport::TeleportFx(void)
{
	FX_Trail(pev->origin+Vector(0,0,32), entindex(), FX_PLAYER_SPAWN_RINGTELEPORT);
	SetThink(&CRingTeleport::EndFx);
	pev->nextthink = gpGlobals->time+0.5;
}

void CRingTeleport::EndFx(void)
{
	CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
	if (pOwner && !pev->button)
	{
		((CBasePlayer *)pOwner)->pev->renderfx = 0;
		((CBasePlayer *)pOwner)->pev->rendermode = kRenderNormal;
		((CBasePlayer *)pOwner)->pev->renderamt = 255;
	}
	DoDmg();
	pev->skin = 1;

	//summon atom bomb
	if (pev->button)
	{
		CBaseEntity::Create("AtomBomb", pev->origin + Vector(0,0,8), g_vecZero, g_vecZero, pOwner->edict());
		CBaseEntity::Create( MAKE_STRING("item_radshield"), pev->origin + Vector(0,0,128), g_vecZero, g_vecZero, NULL, SF_NOTREAL|SF_NORESPAWN);
	}
	SetThink(&CRingTeleport::Destroy);
	pev->nextthink = gpGlobals->time+1.7;
}

void CRingTeleport::Destroy(void)
{
	CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
	if (pOwner && !pev->button)
	{
		((CBasePlayer *)pOwner)->EnableControl(TRUE);
		((CBasePlayer *)pOwner)->pev->takedamage = DAMAGE_AIM;
		((CBasePlayer *)pOwner)->pev->flags	&= ~(FL_NOTARGET | FL_TELEPORTING);
	}
	DoDmg();

	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

void CRingTeleport::DoDmg(void)
{
	CGrenade::Impact(NULL, FALSE, 0, 0, FALSE, 0, 0, 0, 999, 128, DMG_DISINTEGRATING | DMG_IGNOREARMOR | DMG_NOSELF, 0.0, 0);
}