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

===== h_cycler.cpp ========================================================

  The Halflife Cycler Monsters

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "animation.h"
#include "player.h"
#include "gamerules.h"
#include "game.h"
#include "shared_resources.h"
#include "globals.h"
#include "msg_fx.h"

#define TEMP_FOR_SCREEN_SHOTS
#ifdef TEMP_FOR_SCREEN_SHOTS //===================================================

class CCycler : public CBaseMonster
{
public:
	void GenericCyclerSpawn(char *szModel, Vector vecMin, Vector vecMax);
	virtual int	ObjectCaps(void) { return (CBaseMonster::ObjectCaps() | FCAP_IMPULSE_USE); }
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void Think(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual BOOL IsAlive(void) { return FALSE; }// Don't treat as a live target
	virtual BOOL IsMonster(void) { return FALSE; }// XDM: ?
	virtual BOOL IsPushable(void) { return FALSE; }// XDM
	virtual BOOL ShouldRespawn(void) { return FALSE; }// XDM3035
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	int			m_animate;
};

TYPEDESCRIPTION	CCycler::m_SaveData[] =
{
	DEFINE_FIELD( CCycler, m_animate, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE( CCycler, CBaseMonster );

// Cycler member functions
void CCycler::GenericCyclerSpawn(char *szModel, Vector vecMin, Vector vecMax)
{
	if (!szModel || !*szModel)
	{
		ALERT(at_error, "%s %s at %g %g %g missing modelname\n", STRING(pev->classname), STRING(pev->targetname), pev->origin.x, pev->origin.y, pev->origin.z );// XDM: targetname
		REMOVE_ENTITY(ENT(pev));
		return;
	}

	pev->classname		= MAKE_STRING("cycler");
	UTIL_PrecacheModel( szModel );
	UTIL_SetModel(ENT(pev),	szModel);

	CCycler::Spawn();

	UTIL_SetSize(pev, vecMin, vecMax);
}

#define SF_CYCLER_NOTSOLID 0x0001// XDM

void CCycler::Spawn(void)
{
	UTIL_FixRenderColor(pev->rendermode, pev->rendercolor);// XDM3035a: IMPORTANT!
	InitBoneControllers();
	if (pev->spawnflags & SF_CYCLER_NOTSOLID)// XDM
		pev->solid		= SOLID_NOT;
	else
		pev->solid		= SOLID_SLIDEBOX;

	pev->movetype		= MOVETYPE_NONE;

	if (!g_pGameRules->IsMultiplayer() && g_pdeveloper->value > 0)// XDM
		pev->takedamage		= DAMAGE_YES;
	else
		pev->takedamage		= DAMAGE_NO;

	pev->health			= 65535;// no cycler should die
	pev->effects		= 0;
	pev->yaw_speed		= 5;
	pev->ideal_yaw		= pev->angles.y;
	ChangeYaw( 360 );

	m_flFrameRate		= 75;
	m_flGroundSpeed		= 0;

	pev->nextthink		+= 1.0;

	ResetSequenceInfo();

	if (pev->sequence != 0 || pev->frame != 0)
	{
		m_animate = 0;
		pev->framerate = 0;
	}
	else
	{
		m_animate = 1;
	}
}

// XDM3035a: may be called externally
void CCycler::Precache(void)
{
	UTIL_PrecacheModel(STRINGV(pev->model));
}

//
// cycler think
//
void CCycler::Think(void)
{
	if (g_pGameRules->FAllowEffects())// XDM
		pev->nextthink = gpGlobals->time + 0.1;
	else
		pev->nextthink = gpGlobals->time + 0.25;

	if (m_animate)
	{
		StudioFrameAdvance();
	}
	if (m_fSequenceFinished && !m_fSequenceLoops)
	{
		// ResetSequenceInfo();
		// hack to avoid reloading model every frame
		pev->animtime = gpGlobals->time;
		pev->framerate = 1.0;
		m_fSequenceFinished = FALSE;
		m_flLastEventCheck = gpGlobals->time;
		pev->frame = 0;
		if (!m_animate)
			pev->framerate = 0.0;	// FIX: don't reset framerate
	}
}

//
// CyclerUse - starts a rotation trend
//
void CCycler::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	m_animate = !m_animate;
	if (m_animate)
		pev->framerate = 1.0;
	else
		pev->framerate = 0.0;
}

//
// CyclerPain , changes sequences when shot
//
int CCycler::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (g_pGameRules->IsMultiplayer())// XDM
		return 0;

	if (m_animate)
	{
		pev->sequence++;

		ResetSequenceInfo( );

		if (m_flFrameRate == 0.0)
		{
			pev->sequence = 0;
			ResetSequenceInfo( );
		}
		pev->frame = 0;
	}
	else
	{
		pev->framerate = 1.0;
		StudioFrameAdvance ( 0.1 );
		pev->framerate = 0;
		ALERT( at_aiconsole, "sequence: %d, frame %.0f\n", pev->sequence, pev->frame );// XDM: don't allow lamers to see this (AIconsole)
	}

	return 0;
}

// we should get rid of all the other cyclers and replace them with this.
class CGenericCycler : public CCycler
{
public:
	virtual void Spawn(void) { GenericCyclerSpawn(STRINGV(pev->model), Vector(-16, -16, 0), Vector(16, 16, 72)); }
};

LINK_ENTITY_TO_CLASS( cycler, CGenericCycler );

#endif // TEMP_FOR_SCREEN_SHOTS




class CCyclerSprite : public CBaseEntity
{
public:
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void Think(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual int	ObjectCaps(void) { return (CBaseEntity::ObjectCaps() | FCAP_DONT_SAVE | FCAP_IMPULSE_USE); }
	virtual int	TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	virtual int SendClientData(CBasePlayer *pClient, int msgtype);
	void Animate( float frames );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	inline int		ShouldAnimate(void) { return m_animate && m_maxFrame > 1.0; }
	int			m_animate;
	float		m_lastTime;
	float		m_maxFrame;
	BOOL		m_bClientOnly;
};

LINK_ENTITY_TO_CLASS( cycler_sprite, CCyclerSprite );

TYPEDESCRIPTION	CCyclerSprite::m_SaveData[] =
{
	DEFINE_FIELD( CCyclerSprite, m_animate, FIELD_INTEGER ),
	DEFINE_FIELD( CCyclerSprite, m_lastTime, FIELD_TIME ),
	DEFINE_FIELD( CCyclerSprite, m_maxFrame, FIELD_FLOAT ),
};

IMPLEMENT_SAVERESTORE( CCyclerSprite, CBaseEntity );

//#include "studio.h"

void CCyclerSprite::Spawn(void)
{
	Precache();
	pev->solid			= SOLID_SLIDEBOX;
	pev->movetype		= MOVETYPE_NONE;
	pev->takedamage		= DAMAGE_YES;
	pev->effects		= 0;

	if (pev->model)
	{
		const char *s = STRING(pev->model);// XDM3035a: some stupid mappers use this entity to place models!
		size_t l = strlen(s);
		if (strnicmp(s+l-4, ".mdl", 4) == 0)
		{
			ALERT(at_console, "ERROR: CCyclerSprite %s: %s is not a sprite!\n", STRING(pev->targetname), s);
			CBaseEntity::CreateCopy(MAKE_STRING("env_static"), pev, pev->spawnflags);
			UTIL_Remove(this);
			return;
		}
		else
		{
			UTIL_SetModel(ENT(pev), STRING(pev->model));
		}
	}
	else
	{
		ALERT(at_console, "CCyclerSprite(%s): removing: no model!\n", STRING(pev->targetname));
		UTIL_Remove(this);
	}

	pev->frame			= 0;
	pev->nextthink		= gpGlobals->time + 0.1;
	m_animate			= 1;
	m_lastTime			= gpGlobals->time;
	UTIL_FixRenderColor(pev->rendermode, pev->rendercolor);// XDM3035a: IMPORTANT!
	m_maxFrame = (float) MODEL_FRAMES( pev->modelindex ) - 1;

	if (sv_clientstaticents.value < 1.0f)
	{
		m_bClientOnly = FALSE;
	}
	else
	{
		m_bClientOnly = TRUE;
		pev->effects |= EF_NODRAW;
	}
}

// XDM3035a: may be called externally
void CCyclerSprite::Precache(void)
{
	// UNDONE: reliably check if this really is a sprite!
	UTIL_PrecacheModel((char *)STRING(pev->model));
}

void CCyclerSprite::Think(void)
{
	if ( ShouldAnimate() )
		Animate( pev->framerate * (gpGlobals->time - m_lastTime) );

	pev->nextthink		= gpGlobals->time + 0.1;
	m_lastTime = gpGlobals->time;
}

void CCyclerSprite::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	m_animate = !m_animate;
//	ALERT( at_console, "Sprite: %s\n", STRING(pev->model) );
	SendClientData(NULL, MSG_ALL);
}

int	CCyclerSprite::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if ( m_maxFrame > 1.0 )
	{
		Animate( 1.0 );
	}
	return 1;
}

void CCyclerSprite::Animate( float frames )
{
	pev->frame += frames;
	if ( m_maxFrame > 0 )
		pev->frame = fmod( pev->frame, m_maxFrame );
}

// XDM3035a: awesome traffic economy on some maps!
// Called by clients connecting to the game
int CCyclerSprite::SendClientData(CBasePlayer *pClient, int msgtype)
{
	if (m_bClientOnly == FALSE)// server entity mode
		return 0;

#ifdef _DEBUG
	ALERT(at_aiconsole, "CCyclerSprite: Creating client sprite %s\n", STRING(pev->model));
#endif

	if (msgtype == MSG_ONE)// a client has connected and needs an update
	{
		if (pClient == NULL)
			return 0;

		if (pClient->IsBot())// bots don't need sprites =)
			return 0;
	}
	else if (msgtype == MSG_BROADCAST)
		msgtype = MSG_ALL;// we need this fix in case someone will try to put this update into unreliable message stream

	if (pev->impulse > 0)// sprite is turned off
	{
	MESSAGE_BEGIN(msgtype, gmsgStaticSprite, pev->origin, (pClient == NULL)?NULL : ENT(pClient->pev));
		WRITE_SHORT(entindex());// TODO: use more reliable pev->pContainingEntity->serialnumber
		WRITE_SHORT(pev->modelindex);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_ANGLE(pev->angles.x);
		WRITE_ANGLE(pev->angles.y);
		WRITE_ANGLE(pev->angles.z);
		WRITE_BYTE(pev->rendermode);
		WRITE_BYTE(pev->renderfx);
		WRITE_BYTE(pev->rendercolor.x);
		WRITE_BYTE(pev->rendercolor.y);
		WRITE_BYTE(pev->rendercolor.z);
		WRITE_BYTE(pev->renderamt);
		WRITE_BYTE(pev->effects);
		WRITE_COORD(pev->scale);
		WRITE_BYTE(m_animate?(int)pev->framerate:0);
	if (pev->pContainingEntity->num_leafs > 0)// just send one because it's a point entity
		WRITE_SHORT(pev->pContainingEntity->leafnums[0]);
	else
		WRITE_SHORT(0);
	MESSAGE_END();
	}
	return 1;
}




// Flaming Wreakage
class CWreckage : public CCycler
{
	virtual void Spawn(void);
//	virtual void Precache(void);
	virtual void Think(void);
//	virtual BOOL ShouldRespawn(void) { return FALSE; }// XDM3035
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	static TYPEDESCRIPTION m_SaveData[];
	int m_flStartTime;
};

TYPEDESCRIPTION	CWreckage::m_SaveData[] =
{
	DEFINE_FIELD( CWreckage, m_flStartTime, FIELD_TIME ),
};
IMPLEMENT_SAVERESTORE( CWreckage, CCycler );


LINK_ENTITY_TO_CLASS( cycler_wreckage, CWreckage );

void CWreckage::Spawn(void)
{
	GenericCyclerSpawn((char *)STRING(pev->model), Vector(-16, -16, 0), Vector(16, 16, 32));
	m_flStartTime = gpGlobals->time;
}

void CWreckage::Think(void)
{
	StudioFrameAdvance();
	pev->nextthink = gpGlobals->time + 0.2;

	if (pev->dmgtime)
	{
		if (pev->dmgtime < gpGlobals->time)
		{
			UTIL_Remove(this);
			return;
		}
		else if (RANDOM_FLOAT(0, pev->dmgtime - m_flStartTime) > pev->dmgtime - gpGlobals->time)
		{
			return;
		}
	}

	Vector VecSrc;
	VecSrc.x = RANDOM_FLOAT( pev->absmin.x, pev->absmax.x );
	VecSrc.y = RANDOM_FLOAT( pev->absmin.y, pev->absmax.y );
	VecSrc.z = RANDOM_FLOAT( pev->absmin.z, pev->absmax.z );

	FX_Trail(VecSrc, RANDOM_LONG(40,50), FX_BLACKSMOKE);
}
