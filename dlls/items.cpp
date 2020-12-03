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
/*

===== items.cpp ========================================================

  functions governing the selection/use of weapons for players

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"
#include "skill.h"
#include "items.h"
#include "gamerules.h"
#include "globals.h"
#include "game.h"
#include "msg_fx.h"
#include "projectiles.h"
#include "pm_shared.h"

//=========================================================
// world_items HACK
//=========================================================
class CWorldItem : public CBaseEntity
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	int m_iType;
};

LINK_ENTITY_TO_CLASS(world_items, CWorldItem);

void CWorldItem::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "type"))
	{
		m_iType = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue(pkvd);
}

void CWorldItem::Spawn(void)
{
	CBaseEntity *pEntity = NULL;

	switch (m_iType)
	{
	case 44: // ITEM_BATTERY:
		pEntity = CBaseEntity::Create( "item_battery", pev->origin, pev->angles );
		break;
	case 42: // ITEM_ANTIDOTE:
		pEntity = CBaseEntity::Create( "item_antidote", pev->origin, pev->angles );
		break;
	case 43: // ITEM_SECURITY:
		pEntity = CBaseEntity::Create( "item_security", pev->origin, pev->angles );
		break;
	case 45: // ITEM_SUIT:
		pEntity = CBaseEntity::Create( "item_suit", pev->origin, pev->angles );
		break;
	}

	if (!pEntity)
	{
		ALERT( at_console, "unable to create world_item %d\n", m_iType );
	}
	else
	{
		pEntity->pev->target = pev->target;
		pEntity->pev->targetname = pev->targetname;
		pEntity->pev->spawnflags = pev->spawnflags;
//		pEntity->pev->scale = UTIL_GetWeaponWorldScale();// XDM3035b
	}

	REMOVE_ENTITY(edict());
}



//=========================================================
// CItem
//=========================================================
void CItem::Spawn(void)
{
	pev->movetype = MOVETYPE_TOSS;
//	pev->solid = SOLID_TRIGGER;//	pev->solid = SOLID_BBOX;
	pev->scale = UTIL_GetWeaponWorldScale();// XDM3035b
	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 16));

	Materialize();// XDM3035c: reuse more code
//	UTIL_SetOrigin(pev, pev->origin);

/*	if (DROP_TO_FLOOR(ENT(pev)) == 0)
	{
		ALERT(at_console, "Item %s fell out of level at %f %f %f\n", STRING(pev->classname), pev->origin.x, pev->origin.y, pev->origin.z);
		UTIL_Remove(this);
		return;
	}*/

//	SetTouch(&CItem::ItemTouch);

	if (g_pGameRules->IsMultiplayer() && mp_wpnboxbrk.value > 0.0f)// XDM
		pev->takedamage = DAMAGE_YES;
	else
		pev->takedamage = DAMAGE_NO;

	pev->health = 250;
	pev->animtime = gpGlobals->time + 0.5f;
	pev->framerate = 1;
	pev->frame = RANDOM_FLOAT(0, (float)MODEL_FRAMES(pev->modelindex)-1);

	if (pev->spawnflags & SF_NOTREAL)
	{
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time + WEAPONBOX_DESTROY_TIME;// XDM
	}
}

void CItem::ItemTouch(CBaseEntity *pOther)
{
	// if it's not a player, ignore
	if (!pOther->IsPlayer())
		return;

	CBasePlayer *pPlayer = (CBasePlayer *)pOther;

	// ok, a player is touching this item, but can he have it?
	if (!g_pGameRules->CanHaveItem(pPlayer, this))
		return;// no? Ignore the touch.

	if (MyTouch(pPlayer))
	{
		SUB_UseTargets(pOther, USE_TOGGLE, 0);
		SetTouchNull();

		// player grabbed the item.
		g_pGameRules->PlayerGotItem(pPlayer, this);
		if (g_pGameRules->ItemShouldRespawn(this))
		{
			Respawn();
		}
		else
			UTIL_Remove(this);
	}
	else if (gEvilImpulse101)
		UTIL_Remove(this);
}

CBaseEntity *CItem::Respawn(void)
{
	SetTouchNull();
	pev->effects |= EF_NODRAW;
	UTIL_SetOrigin(pev, g_pGameRules->VecItemRespawnSpot(this));// blip to whereever you should respawn.
	SetThink(&CItem::Materialize);
	pev->nextthink = g_pGameRules->FlItemRespawnTime(this);
	return this;
}

BOOL CItem::MyTouch(CBasePlayer *pPlayer)
{
	return FALSE;
}

void CItem::Materialize(void)
{
	if (pev->effects & EF_NODRAW)
	{
		// changing from invisible state to visible.
		pev->effects &= ~EF_NODRAW;
		pev->effects |= EF_MUZZLEFLASH;

		MESSAGE_BEGIN(MSG_PVS, gmsgItemSpawn, pev->origin+Vector(0,0,4));
			WRITE_BYTE(EV_ITEMSPAWN_ITEM);
			WRITE_SHORT(entindex());
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_ANGLE(pev->angles.x);
			WRITE_ANGLE(pev->angles.y);
			WRITE_ANGLE(pev->angles.z);
			WRITE_SHORT(pev->modelindex);
			WRITE_BYTE((int)pev->scale*10);
			WRITE_BYTE(pev->body);
			WRITE_BYTE(pev->skin);
			WRITE_BYTE(pev->sequence);
		MESSAGE_END();

//		PLAYBACK_EVENT_FULL(0, ENT(pev), g_usItemSpawn, 0.0f, (float *)&pev->origin, (float *)&pev->angles, pev->scale, pev->sequence, EV_ITEMSPAWN_ITEM, pev->modelindex, pev->body, pev->skin);
/*		EMIT_SOUND_DYN(ENT(pev), CHAN_BODY, "items/respawn.wav", VOL_NORM, ATTN_NORM, 0, 150);
		if (g_pGameRules->FAllowEffects())// XDM3035: items respawn effects
		{
			StreakSplash(pev->origin, UTIL_RandomBloodVector(), 7, 16, 48, 160);
			ParticleBurst(pev->origin, 4, 208, 10);
		}*/
	}
	if (g_pGameRules->IsMultiplayer())
		pev->flags |= FL_HIGHLIGHT;// XDM3035c

	pev->solid = SOLID_TRIGGER;
	UTIL_SetOrigin(pev, pev->origin);// link into world.
	SetTouch(&CItem::ItemTouch);
}

int CItem::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pev->takedamage == DAMAGE_NO)
		return 0;

	if (bitsDamageType & DMG_NOSELF)
		return 0;

	if (pev->spawnflags & SF_NORESPAWN)// dropped by somebody
	{
		pev->health -= flDamage;

		if (pev->health <= 0)
		{
			Killed(pInflictor, pAttacker, GIB_NORMAL);
		}
		return 1;
	}
	return 0;
}

void CItem::Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib)// XDM3035c
{
	FX_Trail(pev->origin, NULL, FX_ITEM_BLAST);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

//=========================================================
// item_suit
//=========================================================

class CItemSuit : public CItem
{
	virtual void Spawn(void)
	{
		Precache();
		SET_MODEL(ENT(pev), "models/w_suit.mdl");
		pev->colormap = 0x1E1E;// XDM3035: Half-Life default orange
		CItem::Spawn();
	}
	virtual void Precache(void)
	{
		PRECACHE_MODEL("models/w_suit.mdl");
	}
	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if (pPlayer->pev->weapons & (1<<WEAPON_SUIT))
			return FALSE;
/*
		if (!gEvilImpulse101)// XDM
		{
			if ( pev->spawnflags & SF_SUIT_SHORTLOGON )
				EMIT_SOUND_SUIT(pPlayer->edict(), "!HEV_A0");		// short version of suit logon,
			else
				EMIT_SOUND_SUIT(pPlayer->edict(), "!HEV_AAx");	// long version of suit logon
		}*/
		pPlayer->pev->weapons |= (1<<WEAPON_SUIT);
		return TRUE;
	}
};

LINK_ENTITY_TO_CLASS(item_suit, CItemSuit);


//=========================================================
// item_healthkit
//=========================================================
class CItemHealthKit : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_HEALTHKIT;
		CItem::Spawn();
	}
	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if (!pPlayer->IsAlive())
			return FALSE;

		float MaxHealth = MAX_PLAYER_HEALTH + (pPlayer->m_iItemHealthAug*EXTRA_HEALTH_HP_AUG);

		if (pPlayer->pev->health >= MaxHealth)
			return FALSE;
			
		if (pPlayer->pev->weapons & (1<<WEAPON_SUIT))
		{
			//remove all negative effects
			if (pPlayer->m_bitsDamageType & DMG_TIMEBASED)
			{
				pPlayer->m_bitsDamageType &= ~(DMG_TIMEBASED);
			}

			pPlayer->pev->health += MAX_PLAYER_HEALTH;
			
			if (pPlayer->pev->health > MaxHealth)
				pPlayer->pev->health = MaxHealth;

			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/healthkit.wav", VOL_NORM, ATTN_NORM );

			MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, ENT(pPlayer->pev));
			WRITE_SHORT(0);
			WRITE_STRING( STRING(pev->classname) );
			MESSAGE_END();
			return TRUE;
		}
		return FALSE;
	}
};

LINK_ENTITY_TO_CLASS( item_healthkit, CItemHealthKit );

//=========================================================
// item_health_aug
//=========================================================
class CItemHealthAug : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_HEALTH_AUG;
		CItem::Spawn();
	}

	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if (!pPlayer->IsAlive())
			return FALSE;

		if (pPlayer->m_iItemHealthAug >= MAX_HEALTH_AUG)
			return FALSE;

		if (pPlayer->pev->weapons & (1<<WEAPON_SUIT))
		{
			pPlayer->m_iItemHealthAug++;
			float MaxHealth = MAX_PLAYER_HEALTH + (pPlayer->m_iItemHealthAug*EXTRA_HEALTH_HP_AUG);
			pPlayer->pev->health += EXTRA_HEALTH_HP_AUG;

			if (pPlayer->pev->health > MaxHealth)
				pPlayer->pev->health = MaxHealth;

			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/health_aug.wav", VOL_NORM, ATTN_NORM );

			MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, ENT(pPlayer->pev));
			WRITE_SHORT(0);
			WRITE_STRING( STRING(pev->classname) );
			MESSAGE_END();
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS(item_health_aug, CItemHealthAug);

//=========================================================
// item_battery
//=========================================================
class CItemBattery : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_ARMOR;
		CItem::Spawn();
	}

	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if (!pPlayer->IsAlive())
			return FALSE;

		if (!pPlayer->m_iShield)
			return FALSE;
		
		float MaxShield = (pPlayer->m_iItemInvulnerability)?MAX_INVULNER_BATTERY:MAX_NORMAL_BATTERY  + (pPlayer->m_iItemShieldAug*EXTRA_BATTERY_SHIELD_AUG);

		if (pPlayer->pev->armorvalue >= MaxShield)
			return FALSE;
			
		if (pPlayer->pev->weapons & (1<<WEAPON_SUIT))
		{
			pPlayer->pev->armorvalue += MAX_NORMAL_BATTERY;

			if (pPlayer->pev->armorvalue > MaxShield)
				pPlayer->pev->armorvalue = MaxShield;

			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/battery.wav", VOL_NORM, ATTN_NORM );

			MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, ENT(pPlayer->pev));
			WRITE_SHORT(0);
			WRITE_STRING( STRING(pev->classname) );
			MESSAGE_END();
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS(item_battery, CItemBattery);

//=========================================================
// item_shield_aug
//=========================================================
class CItemShieldAug : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_SHIELD_AUG;
		CItem::Spawn();
	}

	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if (!pPlayer->IsAlive())
			return FALSE;

		if (!pPlayer->m_iShield)
			return FALSE;

		if (pPlayer->m_iItemShieldAug >= MAX_SHIELD_AUG)
			return FALSE;

		if (pPlayer->pev->weapons & (1<<WEAPON_SUIT))
		{
			pPlayer->m_iItemShieldAug++;
			float MaxShield = (pPlayer->m_iItemInvulnerability)?MAX_INVULNER_BATTERY:MAX_NORMAL_BATTERY  + (pPlayer->m_iItemShieldAug*EXTRA_BATTERY_SHIELD_AUG);

			pPlayer->pev->armorvalue += EXTRA_BATTERY_SHIELD_AUG;

			if (!pPlayer->m_iItemInvulnerability && pPlayer->pev->armorvalue > MaxShield)
				pPlayer->pev->armorvalue = MaxShield;

			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/shield_aug.wav", VOL_NORM, ATTN_NORM );

			MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, ENT(pPlayer->pev));
			WRITE_SHORT(0);
			WRITE_STRING( STRING(pev->classname) );
			MESSAGE_END();
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS(item_shield_aug, CItemShieldAug);

//=========================================================
// item_shield_strength_aug
//=========================================================
class CItemShieldStrengthAug : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_SHIELD_STRENGTH_AUG;
		CItem::Spawn();
	}

	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if (!pPlayer->IsAlive())
			return FALSE;

		if (!pPlayer->m_iShield)
			return FALSE;

		if (pPlayer->m_iItemShieldStrengthAug >= MAX_SHIELD_STRENGTH_AUG)
			return FALSE;

		if (pPlayer->pev->weapons & (1<<WEAPON_SUIT))
		{
			pPlayer->m_iItemShieldStrengthAug++;

			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/shield_strength_aug.wav", VOL_NORM, ATTN_NORM );

			if (!pPlayer->IsBot())
			{
				MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, ENT(pPlayer->pev));
				WRITE_SHORT(0);
				WRITE_STRING( STRING(pev->classname) );
				MESSAGE_END();

				MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pPlayer->pev));
				WRITE_BYTE(MSG_ITEM_SHIELD_STRENGTH);
				WRITE_SHORT(pPlayer->m_iItemShieldStrengthAug);
				MESSAGE_END();	
			}
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS(item_shield_strength_aug, CItemShieldStrengthAug);

//=========================================================
// item_weapon_power_aug
//=========================================================
class CItemWeaponPowerAug : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_WEAPON_POWER_AUG;
		CItem::Spawn();
	}

	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if (!pPlayer->IsAlive())
			return FALSE;

		if (pPlayer->m_iWpnDestroyed == TRUE)
			return FALSE;

		if (pPlayer->m_iItemWeaponPowerAug >= MAX_WEAPON_POWER_AUG)
			return FALSE;

		if (pPlayer->pev->weapons & (1<<WEAPON_SUIT))
		{
			pPlayer->m_iItemWeaponPowerAug++;

			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/weapon_aug.wav", VOL_NORM, ATTN_NORM );

			if (!pPlayer->IsBot())
			{
				MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, ENT(pPlayer->pev));
				WRITE_SHORT(0);
				WRITE_STRING( STRING(pev->classname) );
				MESSAGE_END();

				MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pPlayer->pev));
				WRITE_BYTE(MSG_ITEM_WEAPON_POWER);
				WRITE_SHORT(pPlayer->m_iItemWeaponPowerAug);
				MESSAGE_END();	
			}
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS(item_weapon_power_aug, CItemWeaponPowerAug);
//=========================================================
// item_antidote
//=========================================================
class CItemAntidote : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_ANTIDOTE;
		CItem::Spawn();
	}
	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if(pPlayer->m_rgItems[ITEM_TYPE_ANTIDOTE] >= MAX_ANTIDOTE_CARRY)
			return FALSE;

		EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/gunpickup2.wav", VOL_NORM, ATTN_NORM );
		pPlayer->m_rgItems[ITEM_TYPE_ANTIDOTE] += 1;
		ClientPrint(pPlayer->pev, HUD_PRINTTALK, UTIL_VarArgs("Antidotes left: %d\n", pPlayer->m_rgItems[ITEM_TYPE_ANTIDOTE]));

		MESSAGE_BEGIN(MSG_ONE, gmsgItemPickup, NULL, ENT(pPlayer->pev));
		WRITE_SHORT(1);
		WRITE_STRING(STRING(pev->classname));
		MESSAGE_END();
			
		return TRUE;
	}
};

LINK_ENTITY_TO_CLASS(item_antidote, CItemAntidote);


//=========================================================
// item_fire_supressor
//=========================================================
class CItemFireSupressor : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_FIRE_SUPRESSOR;
		CItem::Spawn();
	}
	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if(pPlayer->m_rgItems[ITEM_TYPE_FIRE_SUPRESSOR] >= MAX_FIRE_SUPRESSORS_CARRY)
			return FALSE;

		EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/firesupressor.wav", VOL_NORM, ATTN_NORM );
		pPlayer->m_rgItems[ITEM_TYPE_FIRE_SUPRESSOR] += 1;
		ClientPrint(pPlayer->pev, HUD_PRINTTALK, UTIL_VarArgs("Fire Supressors left: %d\n", pPlayer->m_rgItems[ITEM_TYPE_FIRE_SUPRESSOR]));

		MESSAGE_BEGIN(MSG_ONE, gmsgItemPickup, NULL, ENT(pPlayer->pev));
		WRITE_SHORT(1);
		WRITE_STRING(STRING(pev->classname));
		MESSAGE_END();
			
		return TRUE;
	}
};

LINK_ENTITY_TO_CLASS(item_fire_supressor, CItemFireSupressor);


//=========================================================
// item_security
//=========================================================
class CItemSecurity : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_SECURITY;
		CItem::Spawn();
	}
	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		pPlayer->m_rgItems[ITEM_TYPE_SECURITY] ++;
		EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/gunpickup2.wav", VOL_NORM, ATTN_NORM );

		MESSAGE_BEGIN(MSG_ONE, gmsgItemPickup, NULL, ENT(pPlayer->pev));
		WRITE_SHORT(0);
		WRITE_STRING(STRING(pev->classname));
		MESSAGE_END();

		return TRUE;
	}
};

LINK_ENTITY_TO_CLASS(item_security, CItemSecurity);


//=========================================================
// item_longjump
//=========================================================
class CItemLongJump : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_LONGJUMP;
		CItem::Spawn();
	}

	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if (pPlayer->m_fLongJump)
			return FALSE;

		if ((pPlayer->pev->weapons & (1<<WEAPON_SUIT)))
		{
			pPlayer->m_fLongJump = TRUE;// player now has longjump module

			ENGINE_SETPHYSKV(pPlayer->edict(), PHYSKEY_LONGJUMP, "1");
			MESSAGE_BEGIN(MSG_ONE, gmsgItemPickup, NULL, ENT(pPlayer->pev));
			WRITE_SHORT(0);
			WRITE_STRING(STRING(pev->classname));
			MESSAGE_END();

			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/longjump.wav", VOL_NORM, ATTN_NORM );
			return TRUE;
		}
		return FALSE;
	}
};

LINK_ENTITY_TO_CLASS( item_longjump, CItemLongJump );

//=========================================================
// item_airstrike
//=========================================================
class CItemAirStrike : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_AIRSTRIKE;
		CItem::Spawn();
	}
	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if(pPlayer->m_rgItems[ITEM_TYPE_AIRSTRIKE] >= MAX_AIRSTRIKE_CARRY)
			return FALSE;

		EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/air_strike.wav", VOL_NORM, ATTN_NORM );
		pPlayer->m_rgItems[ITEM_TYPE_AIRSTRIKE] += 1;

		if (!pPlayer->IsBot())
		{
			FX_WpnIcon(ENT(pPlayer->pev), pPlayer->m_rgItems[ITEM_TYPE_FLASHBANG], pPlayer->m_rgItems[ITEM_TYPE_TRIPMINE], pPlayer->m_rgItems[ITEM_TYPE_SPIDERMINE], pPlayer->m_rgItems[ITEM_TYPE_AIRSTRIKE], pPlayer->m_rgItems[ITEM_TYPE_SATELLITE_STRIKE], pPlayer->m_rgItems[ITEM_TYPE_ENERGY_CUBE]);

			MESSAGE_BEGIN(MSG_ONE, gmsgItemPickup, NULL, ENT(pPlayer->pev));
			WRITE_SHORT(1);
			WRITE_STRING(STRING(pev->classname));
			MESSAGE_END();
		}
		return TRUE;
	}
};

LINK_ENTITY_TO_CLASS(item_airstrike, CItemAirStrike);

//=========================================================
// item_flashbang
//=========================================================
class CItemFlashBang : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_FLASHBANG;
		CItem::Spawn();
	}
	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if(pPlayer->m_rgItems[ITEM_TYPE_FLASHBANG] >= MAX_FLASHBANG_CARRY)
			return FALSE;

		EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/gunpickup2.wav", VOL_NORM, ATTN_NORM );
		pPlayer->m_rgItems[ITEM_TYPE_FLASHBANG] += 1;

		if (!pPlayer->IsBot())
		{
			FX_WpnIcon(ENT(pPlayer->pev), pPlayer->m_rgItems[ITEM_TYPE_FLASHBANG], pPlayer->m_rgItems[ITEM_TYPE_TRIPMINE], pPlayer->m_rgItems[ITEM_TYPE_SPIDERMINE], pPlayer->m_rgItems[ITEM_TYPE_AIRSTRIKE], pPlayer->m_rgItems[ITEM_TYPE_SATELLITE_STRIKE], pPlayer->m_rgItems[ITEM_TYPE_ENERGY_CUBE]);

			MESSAGE_BEGIN(MSG_ONE, gmsgItemPickup, NULL, ENT(pPlayer->pev));
			WRITE_SHORT(1);
			WRITE_STRING(STRING(pev->classname));
			MESSAGE_END();
		}
		return TRUE;
	}
};

LINK_ENTITY_TO_CLASS(item_flashbang, CItemFlashBang);

//=========================================================
// item_satellite_strike
//=========================================================
class CItemSatelliteStrike : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_SATELLITE_STRIKE;
		CItem::Spawn();
	}
	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if(pPlayer->m_rgItems[ITEM_TYPE_SATELLITE_STRIKE] >= MAX_SATELLITE_STRIKE_CARRY)
			return FALSE;

		EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/satellite_strike.wav", VOL_NORM, ATTN_NORM );
		pPlayer->m_rgItems[ITEM_TYPE_SATELLITE_STRIKE] += 1;

		if (!pPlayer->IsBot())
		{
			FX_WpnIcon(ENT(pPlayer->pev), pPlayer->m_rgItems[ITEM_TYPE_FLASHBANG], pPlayer->m_rgItems[ITEM_TYPE_TRIPMINE], pPlayer->m_rgItems[ITEM_TYPE_SPIDERMINE], pPlayer->m_rgItems[ITEM_TYPE_AIRSTRIKE], pPlayer->m_rgItems[ITEM_TYPE_SATELLITE_STRIKE], pPlayer->m_rgItems[ITEM_TYPE_ENERGY_CUBE]);

			MESSAGE_BEGIN(MSG_ONE, gmsgItemPickup, NULL, ENT(pPlayer->pev));
			WRITE_SHORT(1);
			WRITE_STRING(STRING(pev->classname));
			MESSAGE_END();
		}
		return TRUE;
	}
};

LINK_ENTITY_TO_CLASS(item_satellite_strike, CItemSatelliteStrike);

//=========================================================
// item_tripmine
//=========================================================
class CItemTripMine : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_TRIPMINE;
		CItem::Spawn();
	}
	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if(pPlayer->m_rgItems[ITEM_TYPE_TRIPMINE] >= MAX_TRIPMINE_CARRY)
			return FALSE;

		pPlayer->m_rgItems[ITEM_TYPE_TRIPMINE] += 2;
		EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "weapons/mine_beep.wav", VOL_NORM, ATTN_NORM );

		if (!pPlayer->IsBot())
		{
			FX_WpnIcon(ENT(pPlayer->pev), pPlayer->m_rgItems[ITEM_TYPE_FLASHBANG], pPlayer->m_rgItems[ITEM_TYPE_TRIPMINE], pPlayer->m_rgItems[ITEM_TYPE_SPIDERMINE], pPlayer->m_rgItems[ITEM_TYPE_AIRSTRIKE], pPlayer->m_rgItems[ITEM_TYPE_SATELLITE_STRIKE], pPlayer->m_rgItems[ITEM_TYPE_ENERGY_CUBE]);

			MESSAGE_BEGIN(MSG_ONE, gmsgItemPickup, NULL, ENT(pPlayer->pev));
			WRITE_SHORT(2);
			WRITE_STRING(STRING(pev->classname));
			MESSAGE_END();
		}		
		return TRUE;
	}
};
LINK_ENTITY_TO_CLASS(item_tripmine, CItemTripMine);

//=========================================================
// item_spidermine
//=========================================================
class CItemSpiderMine : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_SPIDERMINE;
		CItem::Spawn();
	}
	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if(pPlayer->m_rgItems[ITEM_TYPE_SPIDERMINE] >= MAX_SPIDERMINE_CARRY)
			return FALSE;

		pPlayer->m_rgItems[ITEM_TYPE_SPIDERMINE] += 1;
		EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/spidermine.wav", VOL_NORM, ATTN_NORM );

		if (!pPlayer->IsBot())
		{
			FX_WpnIcon(ENT(pPlayer->pev), pPlayer->m_rgItems[ITEM_TYPE_FLASHBANG], pPlayer->m_rgItems[ITEM_TYPE_TRIPMINE], pPlayer->m_rgItems[ITEM_TYPE_SPIDERMINE], pPlayer->m_rgItems[ITEM_TYPE_AIRSTRIKE], pPlayer->m_rgItems[ITEM_TYPE_SATELLITE_STRIKE], pPlayer->m_rgItems[ITEM_TYPE_ENERGY_CUBE]);

			MESSAGE_BEGIN(MSG_ONE, gmsgItemPickup, NULL, ENT(pPlayer->pev));
			WRITE_SHORT(1);
			WRITE_STRING(STRING(pev->classname));
			MESSAGE_END();
		}		
		return TRUE;
	}
};
LINK_ENTITY_TO_CLASS(item_spidermine, CItemSpiderMine);

//=========================================================
// item_haste
//=========================================================
class CItemHaste : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_HASTE;
		CItem::Spawn();
	}

	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if (pPlayer->m_iItemHaste)
			return FALSE;

		if ((pPlayer->pev->weapons & (1<<WEAPON_SUIT)))
		{
			pPlayer->m_iItemHaste = TIME_HASTE;

			ENGINE_SETPHYSKV(pPlayer->edict(), PHYSKEY_HASTE, "1");
			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/haste.wav", VOL_NORM, ATTN_NORM );
			return TRUE;
		}
		return FALSE;
	}
};

LINK_ENTITY_TO_CLASS( item_haste, CItemHaste );

//=========================================================
// item_rapidfire
//=========================================================
class CItemRapidFire : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_RAPIDFIRE;
		CItem::Spawn();
	}

	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if (pPlayer->m_iItemRapidFire)
			return FALSE;

		if ((pPlayer->pev->weapons & (1<<WEAPON_SUIT)))
		{
			pPlayer->m_iItemRapidFire = TIME_RAPID_FIRE;
			ENGINE_SETPHYSKV(pPlayer->edict(), PHYSKEY_RAPIDFIRE, "1");
			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/rapidfire.wav", VOL_NORM, ATTN_NORM );
			return TRUE;
		}
		return FALSE;
	}
};

LINK_ENTITY_TO_CLASS(item_rapidfire, CItemRapidFire );

//=========================================================
// item_quaddamage
//=========================================================
class CItemQuadDamage : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_QUADDAMAGE;
		CItem::Spawn();
	}

	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if (pPlayer->m_iItemQuadDamage)
			return FALSE;

		if ((pPlayer->pev->weapons & (1<<WEAPON_SUIT)))
		{
			pPlayer->m_iItemQuadDamage = TIME_QUAD_DAMAGE;
			ENGINE_SETPHYSKV(pPlayer->edict(), PHYSKEY_QUADDAMAGE, "1");

			pPlayer->pev->renderfx = kRenderFxGlowShell;
			pPlayer->pev->rendercolor = Vector(200,0,0);// RGB

			if (pPlayer->m_iItemInvisibility)
				pPlayer->pev->renderamt = INVISIBILITY_RENDERAMT;//hack, to awoid weapon drawing
			else
				pPlayer->pev->renderamt = 15;	// Shell size

			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/quaddamage.wav", VOL_NORM, ATTN_NORM );
			return TRUE;
		}
		return FALSE;
	}
};

LINK_ENTITY_TO_CLASS(item_quaddamage, CItemQuadDamage );

//=========================================================
// item_invisibility
//=========================================================
class CItemInvisibility : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_INVISIBILITY;
		CItem::Spawn();
	}

	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if (pPlayer->m_iItemInvisibility)
			return FALSE;

		if ((pPlayer->pev->weapons & (1<<WEAPON_SUIT)))
		{
			pPlayer->m_iItemInvisibility = TIME_INVISIBILITY;
			ENGINE_SETPHYSKV(pPlayer->edict(), PHYSKEY_INVISIBILITY, "1");

			pPlayer->pev->renderfx = kRenderFxGlowShell;
			pPlayer->pev->rendermode = kRenderTransColor;
			pPlayer->pev->renderamt = INVISIBILITY_RENDERAMT;

			if (pPlayer->m_iItemQuadDamage)
				pPlayer->pev->rendercolor = Vector(100,0,0);
			else
				pPlayer->pev->rendercolor = Vector(1,1,1);

			pPlayer->pev->flags	|= FL_NOTARGET;
			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/invisibility.wav", VOL_NORM, ATTN_NORM );
			return TRUE;
		}
		return FALSE;
	}
};

LINK_ENTITY_TO_CLASS(item_invisibility, CItemInvisibility );

//=========================================================
// item_invulnerability
//=========================================================
class CItemInvulnerability : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_INVULNERABILITY;
		CItem::Spawn();
	}

	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if (pPlayer->m_iItemInvulnerability)
			return FALSE;

		if (pPlayer->m_iItemPlasmaShield)
			return FALSE;

		if ((pPlayer->pev->weapons & (1<<WEAPON_SUIT)))
		{
			//restore shield generator, if it's broken
			if (!pPlayer->m_iShield)
			{
				pPlayer->m_iShield = TRUE;

				if (!pPlayer->IsBot())
				{
					MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pPlayer->pev));
					WRITE_BYTE( MSG_ARMOR );
					WRITE_SHORT(MAX_INVULNER_BATTERY);
					MESSAGE_END();
				}
			}

			pPlayer->m_iItemInvulnerability = TIME_INVULNERABILITY;
			pPlayer->m_iOldShieldAmount = pPlayer->pev->armorvalue;
			ENGINE_SETPHYSKV(pPlayer->edict(), PHYSKEY_INVULNERABILITY, "1");

			pPlayer->pev->armorvalue += MAX_INVULNER_BATTERY;
			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/invulnerability.wav", VOL_NORM, ATTN_NORM );
			return TRUE;
		}
		return FALSE;
	}
};

LINK_ENTITY_TO_CLASS(item_invulnerability, CItemInvulnerability );

//=========================================================
// item_shield_regeneration
//=========================================================
class CItemShieldRegeneration : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_SHIELD_REGENERATION;
		CItem::Spawn();
	}

	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if (pPlayer->m_iItemShieldRegen)
			return FALSE;

		if (!pPlayer->m_iShield)
			return FALSE;

		if ((pPlayer->pev->weapons & (1<<WEAPON_SUIT)))
		{
			pPlayer->m_iItemShieldRegen = TIME_SHIELD_REGEN;
			ENGINE_SETPHYSKV(pPlayer->edict(), PHYSKEY_SHIELD_REGEN, "1");
			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/regeneration.wav", VOL_NORM, ATTN_NORM );
			return TRUE;
		}
		return FALSE;
	}
};

LINK_ENTITY_TO_CLASS(item_shield_regeneration, CItemShieldRegeneration);

//=========================================================
// item_banana
//=========================================================
class CItemBanana : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_BANANA;
		CItem::Spawn();
	}

	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if (pPlayer->m_iItemBanana)
			return FALSE;

		if (mp_bananamode.value > 0.0f && pPlayer->m_iWpnDestroyed == FALSE)
			return FALSE;

		if ((pPlayer->pev->weapons & (1<<WEAPON_SUIT)))
		{
			pPlayer->m_iItemBanana = TIME_BANANA_WEAPON;
			ENGINE_SETPHYSKV(pPlayer->edict(), PHYSKEY_BANANA, "1");

			//restore weapon if it's broken
			if (pPlayer->m_iWpnDestroyed == TRUE)
			{
				ClientPrint(pPlayer->pev, HUD_PRINTTALK, "The Weapon Repared!");
				pPlayer->m_iWpnDestroyed = FALSE;
				if (pPlayer->m_pActiveItem)
					pPlayer->m_pActiveItem->Deploy();
			}
			else
			{
				pPlayer->m_pNextItem = pPlayer->m_pActiveItem;
				if (pPlayer->m_pActiveItem)
				{
					pPlayer->m_pActiveItem->Holster();
					pPlayer->m_pActiveItem = NULL;
				}
			}

			//restore shield if it's broken
			if (!pPlayer->m_iShield)
			{
				pPlayer->m_iShield = TRUE;

				MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pPlayer->pev));
					WRITE_BYTE( MSG_ARMOR );
					WRITE_SHORT(0);
				MESSAGE_END();
			}

			//restore power generator, if it's broken
			if (!pPlayer->m_iGenerator)
			{
				pPlayer->m_iGenerator = TRUE;
				pPlayer->m_rgAmmo[GetAmmoIndexFromRegistry("generator_power")] = 0;
			}
			//reset weapon augmentations
			if (pPlayer->m_iItemWeaponPowerAug == WEAPON_IS_DEAD)
			{
				pPlayer->m_iItemWeaponPowerAug = 0;
				MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pPlayer->pev));
				WRITE_BYTE(MSG_ITEM_WEAPON_POWER);
				WRITE_SHORT(pPlayer->m_iItemWeaponPowerAug);
				MESSAGE_END();
			}
			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/banana.wav", VOL_NORM, ATTN_NORM );
			return TRUE;
		}
		return FALSE;
	}
};

LINK_ENTITY_TO_CLASS(item_banana, CItemBanana);

//=========================================================
// item_generator_aug
//=========================================================
class CItemGeneratorAug : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_GENERATOR_AUG;
		CItem::Spawn();
	}

	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if ((pPlayer->pev->weapons & (1<<WEAPON_SUIT)) && pPlayer->AmmoInventory(GetAmmoIndexFromRegistry("generator_power")) < MAX_GENERATOR_AUG)
		{
			//restore power generator, if it's broken
			if (!pPlayer->m_iGenerator)
			{
				pPlayer->m_iGenerator = TRUE;
				pPlayer->m_rgAmmo[GetAmmoIndexFromRegistry("generator_power")] = 0;
			}
			pPlayer->GiveAmmo(1, "generator_power", MAX_GENERATOR_AUG);
			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/generator_aug.wav", VOL_NORM, ATTN_NORM );
			return TRUE;
		}
		return FALSE;
	}
};

LINK_ENTITY_TO_CLASS(item_generator_aug, CItemGeneratorAug);

//=========================================================
// item_accuracy
//=========================================================
class CItemAccuracy : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_ACCURACY;
		CItem::Spawn();
	}

	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if (pPlayer->pev->weapons & (1<<WEAPON_SUIT))
		{
			if (pPlayer->m_iItemAccuracy)
				return FALSE;

			pPlayer->m_iItemAccuracy = TIME_ACCURACY;
			ENGINE_SETPHYSKV(pPlayer->edict(), PHYSKEY_ACCURACY, "1");

			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/accuracy.wav", VOL_NORM, ATTN_NORM );
			return TRUE;
		}
		return FALSE;
	}
};

LINK_ENTITY_TO_CLASS(item_accuracy, CItemAccuracy);

//=========================================================
// item_energy_cube
//=========================================================
class CItemEnergyCube : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_ENERGY_CUBE;
		CItem::Spawn();
	}
	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if(pPlayer->m_rgItems[ITEM_TYPE_ENERGY_CUBE] >= MAX_ENERGY_CUBE_CARRY)
			return FALSE;

		int m_iCount;
		if (mp_extra_nuke.value > 0.0f)
			m_iCount = 5;
		else
			m_iCount =25;
		
		EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/energy_cube.wav", VOL_NORM, ATTN_NORM );
		pPlayer->m_rgItems[ITEM_TYPE_ENERGY_CUBE] += m_iCount;

		if (!pPlayer->IsBot())
		{
			FX_WpnIcon(ENT(pPlayer->pev), pPlayer->m_rgItems[ITEM_TYPE_FLASHBANG], pPlayer->m_rgItems[ITEM_TYPE_TRIPMINE], pPlayer->m_rgItems[ITEM_TYPE_SPIDERMINE], pPlayer->m_rgItems[ITEM_TYPE_AIRSTRIKE], pPlayer->m_rgItems[ITEM_TYPE_SATELLITE_STRIKE], pPlayer->m_rgItems[ITEM_TYPE_ENERGY_CUBE]);

			MESSAGE_BEGIN(MSG_ONE, gmsgItemPickup, NULL, ENT(pPlayer->pev));
			WRITE_SHORT(m_iCount);
			WRITE_STRING(STRING(pev->classname));
			MESSAGE_END();
		}		
		return TRUE;
	}
};

LINK_ENTITY_TO_CLASS(item_energy_cube, CItemEnergyCube);

//=========================================================
// item_lightning_field
//=========================================================
class CItemLightningField : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_LIGHTNING_FIELD;
		CItem::Spawn();
	}
	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if (pPlayer->pev->weapons & (1<<WEAPON_SUIT))
		{
			if (pPlayer->m_iItemLightningField)
				return FALSE;

			CLightningField::CreateNew(pPlayer, pPlayer->pev->origin, gSkillData.DmgPlrLightningField + (gSkillData.DmgPlrLightningField * (pPlayer->m_iItemWeaponPowerAug*0.1)), TIME_LIGHTNING_FIELD);
			ENGINE_SETPHYSKV(pPlayer->edict(), PHYSKEY_LIGHTNING_FIELD, "1");

			pPlayer->m_iItemLightningField = TIME_LIGHTNING_FIELD;
			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/lightning_field.wav", VOL_NORM, ATTN_NORM );
			return TRUE;
		}
		return FALSE;
	}
};

LINK_ENTITY_TO_CLASS(item_lightning_field, CItemLightningField);

//=========================================================
// item_radshield
//=========================================================
class CItemRadShield : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_RADSHIELD;
		pev->sequence = 1;
		CItem::Spawn();
	}
	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if (pPlayer->pev->weapons & (1<<WEAPON_SUIT))
		{
			if (pPlayer->m_iItemRadShield)
				return FALSE;

			ENGINE_SETPHYSKV(pPlayer->edict(), PHYSKEY_RADSHIELD, "1");

			pPlayer->m_iItemRadShield = TIME_RADSHIELD;
			EMIT_SOUND( pPlayer->edict(), CHAN_STREAM, "items/gunpickup2.wav", VOL_NORM, ATTN_NORM );
			return TRUE;
		}
		return FALSE;
	}
};

LINK_ENTITY_TO_CLASS(item_radshield, CItemRadShield);

//=========================================================
// item_plasma_shield
//=========================================================
class CItemPlasmaShield : public CItem
{
	virtual void Spawn(void)
	{
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_PLASMA_SHIELD;
		CItem::Spawn();
	}
	virtual BOOL MyTouch(CBasePlayer *pPlayer)
	{
		if (pPlayer->pev->weapons & (1<<WEAPON_SUIT))
		{
			if (pPlayer->m_iItemPlasmaShield)
				return FALSE;

			if (pPlayer->m_iItemInvulnerability)
				return FALSE;

			//restore shield generator, if it's broken
			if (!pPlayer->m_iShield)
			{
				pPlayer->m_iShield = TRUE;

				if (!pPlayer->IsBot())
				{
					MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pPlayer->pev));
					WRITE_BYTE( MSG_ARMOR );
					WRITE_SHORT(MAX_INVULNER_BATTERY);
					MESSAGE_END();
				}
			}

			float MaxShield = MAX_NORMAL_BATTERY + (pPlayer->m_iItemShieldAug*EXTRA_BATTERY_SHIELD_AUG);
			pPlayer->pev->armorvalue = MaxShield;

			ENGINE_SETPHYSKV(pPlayer->edict(), PHYSKEY_PLASMA_SHIELD, "1");

			pPlayer->m_iItemPlasmaShield = TIME_PLASMA_SHIELD;
			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/plasma_shield.wav", VOL_NORM, ATTN_NORM );
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS(item_plasma_shield, CItemPlasmaShield);