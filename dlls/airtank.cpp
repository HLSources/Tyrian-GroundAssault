/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "gamerules.h"
#include "items.h"
#include "globals.h"
#include "msg_fx.h"

class CAirtank : public CGrenade
{
	virtual void Spawn( void );
	virtual void Precache( void );
	void EXPORT TankThink( void );
	void EXPORT TankTouch( CBaseEntity *pOther );
	virtual BOOL ShouldRespawn(void);// XDM3035
};

LINK_ENTITY_TO_CLASS( item_airtank, CAirtank );

BOOL CAirtank::ShouldRespawn(void)// XDM3035: special code because it is not a real item
{
	if (g_pGameRules->IsMultiplayer())
	{
		if (pev->spawnflags & SF_NORESPAWN)
			return FALSE;

		//g_pGameRules->FlItemRespawnTime() > 0
		return TRUE;
	}
	return FALSE;
}

void CAirtank :: Spawn( void )
{
	Precache();
	// motor
	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_BBOX;
	SET_MODEL(ENT(pev), "models/w_items.mdl");
	pev->body = ITEM_BODY_AIRTANK;
	UTIL_SetSize(pev, Vector( -16, -16, 0), Vector(16, 16, 36));
	UTIL_SetOrigin(pev, pev->origin);

	SetTouch(&CAirtank::TankTouch);
	SetThink(&CAirtank::TankThink);

//	pev->flags |= FL_MONSTER;
	pev->takedamage = DAMAGE_YES;
	pev->health = 20;
	pev->dmg = 50;
	pev->impulse = 1;
	pev->skin = 0;
}

void CAirtank::Precache(void)
{
	PRECACHE_SOUND("items/airtank1.wav");
}

void CAirtank::TankThink(void)
{
	pev->impulse = 1;
	pev->skin = 0;
	SUB_UseTargets(this, USE_TOGGLE, 0);
}

void CAirtank::TankTouch( CBaseEntity *pOther )
{
	if (!pOther->IsPlayer())
		return;

	if (pev->impulse <= 0)
		return;

	if (pev->teleport_time <= gpGlobals->time)// XDM3035: retouch time
	{
		pOther->pev->air_finished = gpGlobals->time + pev->dmg;// XDM3034
		EMIT_SOUND(ENT(pev), CHAN_BODY, "items/airtank1.wav", VOL_NORM, ATTN_NORM);
		pev->impulse = 0;
		pev->skin = 1;

		MESSAGE_BEGIN(MSG_ONE, gmsgItemPickup, NULL, ENT(pOther->pev));
		WRITE_SHORT(pev->dmg);
		WRITE_STRING(STRING(pev->classname));
		MESSAGE_END();

		SUB_UseTargets(this, USE_TOGGLE, 1);
		pev->nextthink = gpGlobals->time + g_pGameRules->FlHealthChargerRechargeTime();// recharge airtank // XDM3034
		pev->teleport_time = gpGlobals->time + 1.0f;
	}
}
