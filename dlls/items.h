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
#ifndef ITEMS_H
#define ITEMS_H

enum ITEMS
{
	ITEM_TYPE_NONE = 0,
	ITEM_TYPE_HEALTHKIT,
	ITEM_TYPE_ANTIDOTE,
	ITEM_TYPE_SECURITY,
	ITEM_TYPE_BATTERY,
	ITEM_TYPE_LONGJUMP,
	ITEM_TYPE_AIRSTRIKE,
	ITEM_TYPE_AIRTANK,
	ITEM_TYPE_HASTE,
	ITEM_TYPE_RAPIDFIRE,
	ITEM_TYPE_QUADDAMAGE,
	ITEM_TYPE_INVISIBILITY,
	ITEM_TYPE_INVULNERABILITY,
	ITEM_TYPE_SHIELD_REGENERATION,
	ITEM_TYPE_SHIELD_AUG,
	ITEM_TYPE_HEALTH_AUG,
	ITEM_TYPE_GENERATOR_AUG,
	ITEM_TYPE_SUPERWEAPON_AMMO,
	ITEM_TYPE_BANANA,
	ITEM_TYPE_ACCURACY,
	ITEM_TYPE_TRIPMINE,
	ITEM_TYPE_SPIDERMINE,
	ITEM_TYPE_SHIELD_STRENGTH_AUG,
	ITEM_TYPE_ENERGY_CUBE,
	ITEM_TYPE_LIGHTNING_FIELD,
	ITEM_TYPE_SATELLITE_STRIKE,
	ITEM_TYPE_FLASHBANG,
	ITEM_TYPE_RADSHIELD,
	ITEM_TYPE_FIRE_SUPRESSOR,
	ITEM_TYPE_PLASMA_SHIELD,
	ITEM_TYPE_WEAPON_POWER_AUG,
};

// TODO
//class CBasePickup : public CBaseDelay
//class CItem : public CBasePickup

class CItem : public CBaseEntity
{
public:
	virtual void Spawn(void);
	virtual CBaseEntity *Respawn(void);
	virtual bool IsPickup(void) { return true; };// XDM3037
	virtual BOOL MyTouch(CBasePlayer *pPlayer);
	void EXPORT ItemTouch(CBaseEntity *pOther);
	void EXPORT Materialize(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);// XDM
	virtual void Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib);// XDM3035c
};

#endif // ITEMS_H
