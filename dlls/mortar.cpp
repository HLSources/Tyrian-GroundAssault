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

===== mortar.cpp ========================================================

  the "LaBuznik" mortar device

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"
#include "weapons.h"
#include "decals.h"
#include "soundent.h"
#include "skill.h"// XDM
#include "game.h"
#include "gamerules.h"
#include "msg_fx.h"

class CFuncMortarField : public CBaseToggle
{
public:
	virtual void Spawn( void );
	virtual void Precache( void );
	virtual void KeyValue( KeyValueData *pkvd );
	// Bmodels don't go across transitions
	virtual int	ObjectCaps( void ) { return CBaseToggle :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	virtual int Save( CSave &save );
	virtual int Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];
	void EXPORT FieldUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	int m_iszXController;
	int m_iszYController;
	float m_flSpread;
	float m_flDelay;
	int m_iCount;
	int m_fControl;
};

LINK_ENTITY_TO_CLASS( func_mortar_field, CFuncMortarField );

TYPEDESCRIPTION	CFuncMortarField::m_SaveData[] =
{
	DEFINE_FIELD( CFuncMortarField, m_iszXController, FIELD_STRING ),
	DEFINE_FIELD( CFuncMortarField, m_iszYController, FIELD_STRING ),
	DEFINE_FIELD( CFuncMortarField, m_flSpread, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncMortarField, m_flDelay, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncMortarField, m_iCount, FIELD_INTEGER ),
	DEFINE_FIELD( CFuncMortarField, m_fControl, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE( CFuncMortarField, CBaseToggle );

void CFuncMortarField :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "m_iszXController"))
	{
		m_iszXController = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_iszYController"))
	{
		m_iszYController = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_flSpread"))
	{
		m_flSpread = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_fControl"))
	{
		m_fControl = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_iCount"))
	{
		m_iCount = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
}

// Drop bombs from above
void CFuncMortarField :: Spawn( void )
{
	pev->solid = SOLID_NOT;
	SET_MODEL(ENT(pev), STRING(pev->model));    // set size and link into world
	pev->movetype = MOVETYPE_NONE;
	SetBits( pev->effects, EF_NODRAW );
	SetUse(&CFuncMortarField::FieldUse);
	Precache();
}

void CFuncMortarField :: Precache( void )
{
	PRECACHE_SOUND("weapons/airstrike_mortar_whistle1.wav");
	PRECACHE_SOUND("weapons/airstrike_mortar_whistle2.wav");
	PRECACHE_SOUND("weapons/airstrike_mortar_whistle3.wav");
	UTIL_PrecacheOther("monster_mortar");// XDM
}

// If connected to a table, then use the table controllers, else hit where the trigger is.
void CFuncMortarField :: FieldUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	Vector vecStart;

	vecStart.x = RANDOM_FLOAT( pev->mins.x, pev->maxs.x );
	vecStart.y = RANDOM_FLOAT( pev->mins.y, pev->maxs.y );
	vecStart.z = pev->maxs.z;

	if (m_fControl == 1)// Trigger Activator
	{
		if (pActivator != NULL)
		{
			vecStart.x = pActivator->pev->origin.x;
			vecStart.y = pActivator->pev->origin.y;
		}
	}
	else if (m_fControl == 2)// table
	{
		CBaseEntity *pController;

		if (!FStringNull(m_iszXController))
		{
			pController = UTIL_FindEntityByTargetname( NULL, STRING(m_iszXController));
			if (pController != NULL)
			{
				vecStart.x = pev->mins.x + pController->pev->ideal_yaw * (pev->size.x);
			}
		}
		if (!FStringNull(m_iszYController))
		{
			pController = UTIL_FindEntityByTargetname( NULL, STRING(m_iszYController));
			if (pController != NULL)
			{
				vecStart.y = pev->mins.y + pController->pev->ideal_yaw * (pev->size.y);
			}
		}
	}

	switch (RANDOM_LONG(0,2))
	{
		case 0:	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "weapons/airstrike_mortar_whistle1.wav", VOL_NORM, ATTN_NONE, 0, RANDOM_LONG(95,120)); break;
		case 1:	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "weapons/airstrike_mortar_whistle2.wav", VOL_NORM, ATTN_NONE, 0, RANDOM_LONG(95,120)); break;
		case 2:	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "weapons/airstrike_mortar_whistle3.wav", VOL_NORM, ATTN_NONE, 0, RANDOM_LONG(95,120)); break;
	}

	float t = 2.5;
	for (int i = 0; i < m_iCount; i++)
	{
		Vector vecSpot = vecStart;
		vecSpot.x += RANDOM_FLOAT(-m_flSpread, m_flSpread);
		vecSpot.y += RANDOM_FLOAT(-m_flSpread, m_flSpread);

		TraceResult tr;
		UTIL_TraceLine(vecSpot, vecSpot + Vector(0, 0, -1) * 4096, ignore_monsters, ENT(pev), &tr);

		edict_t *pentOwner = NULL;
		if (pActivator)	pentOwner = pActivator->edict();

		CBaseEntity *pMortar = Create("monster_mortar", tr.vecEndPos, g_vecZero, pentOwner);
		if (pMortar)
		{
			pMortar->pev->nextthink = gpGlobals->time + t;
			t += RANDOM_FLOAT(0.2, 0.5);
		}
		if (i == 0)
			CSoundEnt::InsertSound(bits_SOUND_DANGER, tr.vecEndPos, 400, 0.3);
	}
}


class CMortar : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual void Precache(void);
	void EXPORT MortarExplode( void );
};

LINK_ENTITY_TO_CLASS( monster_mortar, CMortar );

void CMortar::Spawn(void)
{
	pev->takedamage	= DAMAGE_NO;
	pev->movetype	= MOVETYPE_NONE;
	pev->solid		= SOLID_NOT;
	pev->dmg		= gSkillData.DmgAirStrike;
	SetThink(&CMortar::MortarExplode);
	pev->nextthink = 0;
	Precache();
}

void CMortar::Precache(void)
{
	pev->noise = MAKE_STRING("weapons/mortarhit.wav");// custom explosion sound
	PRECACHE_SOUND(STRINGV(pev->noise));
}

void CMortar::MortarExplode(void)
{
	TraceResult tr;
	UTIL_TraceLine(pev->origin, pev->origin + Vector(0.0f, 0.0f, g_psv_zmax->value), dont_ignore_monsters, ENT(pev), &tr);
	// mortar beam
	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(TE_BEAMPOINTS);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_COORD(tr.vecEndPos.x);
		WRITE_COORD(tr.vecEndPos.y);
		WRITE_COORD(tr.vecEndPos.z);
		WRITE_SHORT(g_iModelIndexBeamsAll);
		WRITE_BYTE(BLAST_SKIN_SHOCKWAVE); // frame
		WRITE_BYTE(0); // framerate
		WRITE_BYTE(1); // life
		WRITE_BYTE(40); // width
		WRITE_BYTE(0); // noise
		WRITE_BYTE(255); // r
		WRITE_BYTE(191); // g
		WRITE_BYTE(127); // b
		WRITE_BYTE(95); // brightness
		WRITE_BYTE(0); // speed
	MESSAGE_END();

	UTIL_TraceLine(pev->origin + Vector(0.0f, 0.0f, 1024.0f), pev->origin - Vector(0.0f, 0.0f, g_psv_zmax->value), dont_ignore_monsters, ENT(pev), &tr);

	::RadiusDamage(tr.vecEndPos, this, Instance(pev->owner), pev->dmg, pev->dmg*2.5, CLASS_NONE, DMG_BLAST);
	FX_Trail(tr.vecEndPos + Vector(0.0f, 0.0f, 40.0f), entindex(), FX_MORTAR_DETONATE);
	UTIL_DecalTrace(&tr, DECAL_BLOW);

	pev->health = 0;
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time + 0.1;
}