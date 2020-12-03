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

//	-------------------------------------------
//
//	maprules.cpp
//
//	This module contains entities for implementing/changing game
//	rules dynamically within each map (.BSP)
//
//	-------------------------------------------

#include "extdll.h"
#include "eiface.h"
#include "util.h"
#include "maprules.h"
#include "gamerules.h"
#include "game.h"
#include "cbase.h"
#include "player.h"
#include "globals.h"
#include "shake.h"
#include "pm_shared.h"


class CRuleEntity : public CBaseDelay// XDM3037
{
public:
	virtual void Spawn(void);

protected:
	BOOL CanFireForActivator(CBaseEntity *pActivator);
};

void CRuleEntity::Spawn(void)
{
	CBaseDelay::Spawn();
	pev->solid			= SOLID_NOT;
	pev->movetype		= MOVETYPE_NONE;
	pev->effects		= EF_NODRAW;
}

BOOL CRuleEntity::CanFireForActivator(CBaseEntity *pActivator)
{
	if (m_iszMaster)
	{
		if (UTIL_IsMasterTriggered(m_iszMaster, pActivator))
			return TRUE;
		else
			return FALSE;
	}
	return TRUE;
}


//
// CRulePointEntity -- base class for all rule "point" entities (not brushes)
//
class CRulePointEntity : public CRuleEntity
{
public:
	virtual void Spawn(void);
};

void CRulePointEntity::Spawn(void)
{
	CRuleEntity::Spawn();
	pev->frame = 0;
	pev->model = 0;
	pev->modelindex = 0;
}

//
// CRuleBrushEntity -- base class for all rule "brush" entities (not brushes)
// Default behavior is to set up like a trigger, invisible, but keep the model for volume testing
//
class CRuleBrushEntity : public CRuleEntity
{
public:
	virtual void Spawn(void);
};

void CRuleBrushEntity::Spawn(void)
{
	SET_MODEL(edict(), STRING(pev->model));
	CRuleEntity::Spawn();
}


// CGameScore / game_score	-- award points to player / team
//	Points +/- total
//	Flag: Allow negative scores					SF_SCORE_NEGATIVE
//	Flag: Award points to team in teamplay		SF_SCORE_TEAM

#define SF_SCORE_NEGATIVE			0x0001
#define SF_SCORE_TEAM				0x0002

class CGameScore : public CRulePointEntity
{
public:
	virtual void Spawn(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual void KeyValue(KeyValueData *pkvd);

	inline	BOOL AllowNegativeScore(void) { return pev->spawnflags & SF_SCORE_NEGATIVE; }
	inline	BOOL AwardToTeam(void) { return pev->spawnflags & SF_SCORE_TEAM; }
};

LINK_ENTITY_TO_CLASS( game_score, CGameScore );

void CGameScore::Spawn(void)
{
	CRulePointEntity::Spawn();
}

void CGameScore::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "points"))
	{
		pev->frags = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CRulePointEntity::KeyValue(pkvd);
}

void CGameScore::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (g_pGameRules == NULL)
		return;

	if (!CanFireForActivator(pActivator))
		return;

	// Only players can use this
	if (pActivator->IsPlayer())
	{
		if (AwardToTeam())
			g_pGameRules->AddScoreToTeam(pActivator->pev->team, (int)pev->frags);
		else
			g_pGameRules->AddScore(pActivator, (int)pev->frags);
	}
}


// CGameEnd / game_end	-- Ends the game in MP
class CGameEnd : public CRulePointEntity
{
public:
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
};

LINK_ENTITY_TO_CLASS(game_end, CGameEnd);

void CGameEnd::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (g_pGameRules)
	{
		if (!CanFireForActivator(pActivator))
			return;

		g_pGameRules->EndMultiplayerGame();
	}
}


// CGameText / game_text	-- NON-Localized HUD Message (use env_message to display a titles.txt message)

#define SF_ENVTEXT_ALLPLAYERS			0x0001

class CGameText : public CRulePointEntity
{
public:
	virtual void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	virtual void	KeyValue( KeyValueData *pkvd );
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	inline	BOOL	MessageToAll(void) { return (pev->spawnflags & SF_ENVTEXT_ALLPLAYERS); }
	inline	void	MessageSet( const char *pMessage ) { pev->message = ALLOC_STRING(pMessage); }
	inline	const char *MessageGet(void)	{ return STRING(pev->message); }

private:
	hudtextparms_t	m_textParms;
};

LINK_ENTITY_TO_CLASS( game_text, CGameText );

// Save parms as a block.  Will break save/restore if the structure changes, but this entity didn't ship with Half-Life, so
// it can't impact saved Half-Life games.
TYPEDESCRIPTION	CGameText::m_SaveData[] =
{
	DEFINE_ARRAY( CGameText, m_textParms, FIELD_CHARACTER, sizeof(hudtextparms_t) ),
};

IMPLEMENT_SAVERESTORE( CGameText, CRulePointEntity );

void CGameText::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "channel"))
	{
		m_textParms.channel = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "x"))
	{
		m_textParms.x = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "y"))
	{
		m_textParms.y = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "effect"))
	{
		m_textParms.effect = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "color"))
	{
		if (StringToRGBA(pkvd->szValue, m_textParms.r1, m_textParms.g1, m_textParms.b1, m_textParms.a1))
		{
			ALERT(at_console, "Error: %s has bad value %s == \"%s\"!\n", pkvd->szClassName, pkvd->szKeyName, pkvd->szValue);
			pkvd->fHandled = TRUE;
		}
		else
		{
			m_textParms.r1 = 0;
			m_textParms.g1 = 0;
			m_textParms.b1 = 0;
			m_textParms.a1 = 0;
		}
	}
	else if (FStrEq(pkvd->szKeyName, "color2"))
	{
		if (StringToRGBA(pkvd->szValue, m_textParms.r2, m_textParms.g2, m_textParms.b2, m_textParms.a2))
		{
			ALERT(at_console, "Error: %s has bad value %s == \"%s\"!\n", pkvd->szClassName, pkvd->szKeyName, pkvd->szValue);
			pkvd->fHandled = TRUE;
		}
		else
		{
			m_textParms.r2 = 0;
			m_textParms.g2 = 0;
			m_textParms.b2 = 0;
			m_textParms.a2 = 0;
		}
	}
	else if (FStrEq(pkvd->szKeyName, "fadein"))
	{
		m_textParms.fadeinTime = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "fadeout"))
	{
		m_textParms.fadeoutTime = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "holdtime"))
	{
		m_textParms.holdTime = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "fxtime"))
	{
		m_textParms.fxTime = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CRulePointEntity::KeyValue( pkvd );
}

void CGameText::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!CanFireForActivator(pActivator))
		return;

	if (MessageToAll())
	{
		UTIL_HudMessageAll(m_textParms, MessageGet());
	}
	else
	{
		if (pActivator->IsNetClient())
			UTIL_HudMessage(pActivator, m_textParms, MessageGet());
	}
}


//
// CGameTeamMaster / game_team_master	-- "Masters" like multisource, but based on the team of the activator
// Only allows mastered entity to fire if the team matches my team
//
// team index (pulled from server team list "mp_teamlist"
// Flag: Remove on Fire
// Flag: Any team until set?		-- Any team can use this until the team is set (otherwise no teams can use it)
//
// XDM3035a: FIXME: these team indexes are wrong!

#define SF_TEAMMASTER_FIREONCE			0x0001
#define SF_TEAMMASTER_ANYTEAM			0x0002

class CGameTeamMaster : public CRulePointEntity
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual int ObjectCaps(void) { return CRulePointEntity::ObjectCaps() | FCAP_MASTER; }

	virtual BOOL IsTriggered( CBaseEntity *pActivator );
	inline BOOL RemoveOnFire(void) { return (pev->spawnflags & SF_TEAMMASTER_FIREONCE) ? TRUE : FALSE; }
	inline BOOL AnyTeam(void) { return (pev->spawnflags & SF_TEAMMASTER_ANYTEAM) ? TRUE : FALSE; }

private:
	BOOL		TeamMatch( CBaseEntity *pActivator );

	int			m_teamIndex;
	USE_TYPE	triggerType;
};

LINK_ENTITY_TO_CLASS( game_team_master, CGameTeamMaster );

void CGameTeamMaster::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "teamindex"))
	{
		m_teamIndex = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "triggerstate"))
	{
		int type = atoi( pkvd->szValue );
		switch( type )
		{
		case 0:
			triggerType = USE_OFF;
			break;
		case 2:
			triggerType = USE_TOGGLE;
			break;
		default:
			triggerType = USE_ON;
			break;
		}
		pkvd->fHandled = TRUE;
	}
	else
		CRulePointEntity::KeyValue( pkvd );
}

void CGameTeamMaster::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !CanFireForActivator( pActivator ) )
		return;

	if ( useType == USE_SET )
	{
		if ( value < 0 )
		{
			m_teamIndex = -1;
		}
		else
		{
			m_teamIndex = pActivator->pev->team;
		}
		return;
	}

	if ( TeamMatch( pActivator ) )
	{
		SUB_UseTargets( pActivator, triggerType, value );
		if ( RemoveOnFire() )
			UTIL_Remove( this );
	}
}

BOOL CGameTeamMaster::IsTriggered( CBaseEntity *pActivator )
{
	return TeamMatch( pActivator );
}

BOOL CGameTeamMaster::TeamMatch( CBaseEntity *pActivator )
{
	if ( m_teamIndex < 0 && AnyTeam() )
		return TRUE;

	if (pActivator == NULL)
		return FALSE;

	if ( !g_pGameRules->IsTeamplay() )
		return TRUE;

	return (pActivator->pev->team == (m_teamIndex+1));// XDM: TESTME!!
}


//
// CGameTeamSet / game_team_set	-- Changes the team of the entity it targets to the activator's team
// Flag: Fire once
// Flag: Clear team				-- Sets the team to "NONE" instead of activator

#define SF_TEAMSET_FIREONCE			0x0001
#define SF_TEAMSET_CLEARTEAM		0x0002

class CGameTeamSet : public CRulePointEntity
{
public:
	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	inline BOOL RemoveOnFire(void) { return (pev->spawnflags & SF_TEAMSET_FIREONCE) ? TRUE : FALSE; }
	inline BOOL ShouldClearTeam(void) { return (pev->spawnflags & SF_TEAMSET_CLEARTEAM) ? TRUE : FALSE; }
};

LINK_ENTITY_TO_CLASS( game_team_set, CGameTeamSet );

void CGameTeamSet::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !CanFireForActivator( pActivator ) )
		return;

	if (ShouldClearTeam())
		SUB_UseTargets( pActivator, USE_SET, -1 );
	else
		SUB_UseTargets( pActivator, USE_SET, 0 );

	if (RemoveOnFire())
		UTIL_Remove(this);
}


//
// CGamePlayerZone / game_player_zone -- players in the zone fire my target when I'm fired
//
// Needs master?
class CGamePlayerZone : public CRuleBrushEntity
{
public:
	virtual void KeyValue( KeyValueData *pkvd );
	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	virtual int Save( CSave &save );
	virtual int Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

private:
	string_t	m_iszInTarget;
	string_t	m_iszOutTarget;
	string_t	m_iszInCount;
	string_t	m_iszOutCount;
};

LINK_ENTITY_TO_CLASS( game_zone_player, CGamePlayerZone );

TYPEDESCRIPTION	CGamePlayerZone::m_SaveData[] =
{
	DEFINE_FIELD( CGamePlayerZone, m_iszInTarget, FIELD_STRING ),
	DEFINE_FIELD( CGamePlayerZone, m_iszOutTarget, FIELD_STRING ),
	DEFINE_FIELD( CGamePlayerZone, m_iszInCount, FIELD_STRING ),
	DEFINE_FIELD( CGamePlayerZone, m_iszOutCount, FIELD_STRING ),
};

IMPLEMENT_SAVERESTORE( CGamePlayerZone, CRuleBrushEntity );

void CGamePlayerZone::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "intarget"))
	{
		m_iszInTarget = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "outtarget"))
	{
		m_iszOutTarget = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "incount"))
	{
		m_iszInCount = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "outcount"))
	{
		m_iszOutCount = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CRuleBrushEntity::KeyValue( pkvd );
}

void CGamePlayerZone::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	int playersInCount = 0;
	int playersOutCount = 0;

	if ( !CanFireForActivator( pActivator ) )
		return;

	CBasePlayer *pPlayer = NULL;

	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		pPlayer = UTIL_ClientByIndex(i);
		if ( pPlayer )
		{
			TraceResult trace;
			int			hullNumber;

			if ( pPlayer->pev->flags & FL_DUCKING )
				hullNumber = head_hull;
			else
				hullNumber = human_hull;

			UTIL_TraceModel( pPlayer->pev->origin, pPlayer->pev->origin, hullNumber, edict(), &trace );

			if ( trace.fStartSolid )
			{
				playersInCount++;
				if ( m_iszInTarget )
				{
					FireTargets( STRING(m_iszInTarget), pPlayer, pActivator, useType, value );
				}
			}
			else
			{
				playersOutCount++;
				if ( m_iszOutTarget )
				{
					FireTargets( STRING(m_iszOutTarget), pPlayer, pActivator, useType, value );
				}
			}
		}
	}

	if ( m_iszInCount )
		FireTargets( STRING(m_iszInCount), pActivator, this, USE_SET, playersInCount );

	if ( m_iszOutCount )
		FireTargets( STRING(m_iszOutCount), pActivator, this, USE_SET, playersOutCount );
}



//
// CGamePlayerHurt / game_player_hurt	-- Damages the player who fires it
// Flag: Fire once

#define SF_PKILL_FIREONCE			0x0001
class CGamePlayerHurt : public CRulePointEntity
{
public:
	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	inline BOOL RemoveOnFire(void) { return (pev->spawnflags & SF_PKILL_FIREONCE) ? TRUE : FALSE; }
};

LINK_ENTITY_TO_CLASS( game_player_hurt, CGamePlayerHurt );

void CGamePlayerHurt::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !CanFireForActivator( pActivator ) )
		return;

	if ( pActivator->IsPlayer() )
	{
		if ( pev->dmg < 0 )
			pActivator->TakeHealth( -pev->dmg, DMG_GENERIC );
		else
			pActivator->TakeDamage( this, this, pev->dmg, DMG_GENERIC );
	}

	SUB_UseTargets( pActivator, useType, value );

	if ( RemoveOnFire() )
		UTIL_Remove( this );
}



//
// CGameCounter / game_counter	-- Counts events and fires target
// Flag: Fire once
// Flag: Reset on Fire

#define SF_GAMECOUNT_FIREONCE			0x0001
#define SF_GAMECOUNT_RESET				0x0002

class CGameCounter : public CRulePointEntity
{
public:
	virtual void Spawn(void);
	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	inline BOOL RemoveOnFire(void) { return (pev->spawnflags & SF_GAMECOUNT_FIREONCE) ? TRUE : FALSE; }
	inline BOOL ResetOnFire(void) { return (pev->spawnflags & SF_GAMECOUNT_RESET) ? TRUE : FALSE; }

	inline void CountUp(void) { pev->frags++; }
	inline void CountDown(void) { pev->frags--; }
	inline void ResetCount(void) { pev->frags = pev->dmg; }
	inline int  CountValue(void) { return pev->frags; }
	inline int	LimitValue(void) { return pev->health; }

	inline BOOL HitLimit(void) { return CountValue() == LimitValue(); }

private:
	inline void SetCountValue( int value ) { pev->frags = value; }
	inline void SetInitialValue( int value ) { pev->dmg = value; }
};

LINK_ENTITY_TO_CLASS( game_counter, CGameCounter );

void CGameCounter::Spawn(void)
{
	// Save off the initial count
	SetInitialValue( CountValue() );
	CRulePointEntity::Spawn();
}

void CGameCounter::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !CanFireForActivator( pActivator ) )
		return;

	switch( useType )
	{
	case USE_ON:
	case USE_TOGGLE:
		CountUp();
		break;

	case USE_OFF:
		CountDown();
		break;

	case USE_SET:
		SetCountValue( (int)value );
		break;
	}

	if ( HitLimit() )
	{
		SUB_UseTargets( pActivator, USE_TOGGLE, 0 );

		if ( RemoveOnFire() )
			UTIL_Remove( this );
		else if ( ResetOnFire() )
			ResetCount();
	}
}



//
// CGameCounterSet / game_counter_set	-- Sets the counter's value
// Flag: Fire once

#define SF_GAMECOUNTSET_FIREONCE			0x0001

class CGameCounterSet : public CRulePointEntity
{
public:
	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	inline BOOL RemoveOnFire(void) { return (pev->spawnflags & SF_GAMECOUNTSET_FIREONCE) ? TRUE : FALSE; }
};

LINK_ENTITY_TO_CLASS( game_counter_set, CGameCounterSet );

void CGameCounterSet::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if (!CanFireForActivator(pActivator))
		return;

	SUB_UseTargets(pActivator, USE_SET, pev->frags);

	if (RemoveOnFire())
		UTIL_Remove(this);
}


//
// CGamePlayerEquip / game_playerequip	-- Sets the default player equipment
// Flag: USE Only

#define SF_PLAYEREQUIP_USEONLY			0x0001
#define MAX_EQUIP		PLAYER_INVENTORY_SIZE

class CGamePlayerEquip : public CRulePointEntity
{
public:
	virtual void KeyValue( KeyValueData *pkvd );
	virtual void Touch( CBaseEntity *pOther );
	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	inline BOOL	UseOnly(void) { return (pev->spawnflags & SF_PLAYEREQUIP_USEONLY) ? TRUE : FALSE; }

private:
	void		EquipPlayer( CBaseEntity *pPlayer );

	string_t	m_weaponNames[MAX_EQUIP];
	int			m_weaponCount[MAX_EQUIP];
};

LINK_ENTITY_TO_CLASS( game_player_equip, CGamePlayerEquip );

void CGamePlayerEquip::KeyValue( KeyValueData *pkvd )
{
	CRulePointEntity::KeyValue(pkvd);// XDM3036
	if (pkvd->fHandled)
		return;

/*in CRuleEntity	if (FStrEq(pkvd->szKeyName, "master"))
	{
		SetMaster( ALLOC_STRING(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else
	{*/
		for (int i = 0; i < MAX_EQUIP; ++i)
		{
			if (m_weaponNames[i] == NULL)
			{
				char tmp[128];
				UTIL_StripToken(pkvd->szKeyName, tmp);
				m_weaponNames[i] = ALLOC_STRING(tmp);
				m_weaponCount[i] = atoi(pkvd->szValue);
				m_weaponCount[i] = max(1,m_weaponCount[i]);
				pkvd->fHandled = TRUE;
				break;
			}
		}
//	}
}

void CGamePlayerEquip::Touch( CBaseEntity *pOther )
{
	if ( !CanFireForActivator( pOther ) )
		return;

	if ( UseOnly() )
		return;

	EquipPlayer( pOther );
}

void CGamePlayerEquip::EquipPlayer( CBaseEntity *pEntity )
{
	CBasePlayer *pPlayer = NULL;
	if (pEntity->IsPlayer())
		pPlayer = (CBasePlayer *)pEntity;

	if (!pPlayer)
		return;

	for (int i = 0; i < MAX_EQUIP; ++i)
	{
		if (m_weaponNames[i] == NULL)
			break;

		for (int j = 0; j < m_weaponCount[i]; ++j)
 			pPlayer->GiveNamedItem(STRING(m_weaponNames[i]));
	}
}

void CGamePlayerEquip::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	EquipPlayer( pActivator );
}


//
// CGamePlayerTeam / game_player_team	-- Changes the team of the player who fired it
// Flag: Fire once
// Flag: Kill Player
// Flag: Gib Player

#define SF_PTEAM_FIREONCE			0x0001
#define SF_PTEAM_KILL    			0x0002
#define SF_PTEAM_GIB     			0x0004

class CGamePlayerTeam : public CRulePointEntity
{
public:
	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

private:
	inline bool RemoveOnFire(void) { return (pev->spawnflags & SF_PTEAM_FIREONCE) ? true : false; }
	inline bool ShouldKillPlayer(void) { return (pev->spawnflags & SF_PTEAM_KILL) ? true : false; }
	inline bool ShouldGibPlayer(void) { return (pev->spawnflags & SF_PTEAM_GIB) ? true : false; }
	TEAM_ID GetTargetTeam(const char *pszTargetName);
};

LINK_ENTITY_TO_CLASS( game_player_team, CGamePlayerTeam );

TEAM_ID CGamePlayerTeam::GetTargetTeam(const char *pszTargetName)
{
	CBaseEntity *pTeamEntity = NULL;
	while ((pTeamEntity = UTIL_FindEntityByTargetname(pTeamEntity, pszTargetName)) != NULL)
	{
		if (FClassnameIs(pTeamEntity->pev, "game_team_master"))
			return pTeamEntity->pev->team;//g_pGameRules->GetTeamName(pTeamEntity->pev->team);
	}
	return NULL;
}

void CGamePlayerTeam::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if (!CanFireForActivator(pActivator))
		return;

	if (pActivator->IsPlayer())
	{
//		const char *pszTargetTeam = TargetTeamName(STRING(pev->target));
//		if (pszTargetTeam)
		TEAM_ID target_team = GetTargetTeam(STRING(pev->target));
// done inside		if (g_pGameRules->IsValidTeam(target_team))
		{
			CBasePlayer *pPlayer = (CBasePlayer *)pActivator;
			g_pGameRules->ChangePlayerTeam(pPlayer, target_team, ShouldKillPlayer(), ShouldGibPlayer());
		}
	}

	if (RemoveOnFire())
		UTIL_Remove(this);
}


class CGameRestart : public CRulePointEntity
{
public:
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
};

LINK_ENTITY_TO_CLASS( game_restart, CGameRestart );

void CGameRestart::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!CanFireForActivator(pActivator))
		return;

	SERVER_COMMAND("restart\n");
	UTIL_Remove(this);
}


class CStripWeapons : public CPointEntity
{
public:
	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
};

LINK_ENTITY_TO_CLASS( player_weaponstrip, CStripWeapons );

void CStripWeapons :: Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CBasePlayer *pPlayer = NULL;

	if ( pActivator && pActivator->IsPlayer() )
	{
		pPlayer = (CBasePlayer *)pActivator;
	}
	else if ( !g_pGameRules->IsMultiplayer() )
	{
		pPlayer = UTIL_ClientByIndex(1);//(CBasePlayer *)CBaseEntity::Instance( INDEXENT( 1 ) );
	}

	if ( pPlayer )
		pPlayer->RemoveAllItems( FALSE );
}


class CRevertSaved : public CPointEntity
{
public:
	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	virtual void KeyValue( KeyValueData *pkvd );
	virtual int Save( CSave &save );
	virtual int Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];
	void EXPORT MessageThink(void);
	void EXPORT LoadThink(void);
	inline float Duration(void) { return pev->dmg_take; }
	inline float HoldTime(void) { return pev->dmg_save; }
	inline float MessageTime(void) { return m_messageTime; }
	inline float LoadTime(void) { return m_loadTime; }
	inline void SetDuration( float duration ) { pev->dmg_take = duration; }
	inline void SetHoldTime( float hold ) { pev->dmg_save = hold; }
	inline void SetMessageTime( float time ) { m_messageTime = time; }
	inline void SetLoadTime( float time ) { m_loadTime = time; }

private:
	float m_messageTime;
	float m_loadTime;
};

LINK_ENTITY_TO_CLASS( player_loadsaved, CRevertSaved );

TYPEDESCRIPTION	CRevertSaved::m_SaveData[] =
{
	DEFINE_FIELD( CRevertSaved, m_messageTime, FIELD_FLOAT ),	// These are not actual times, but durations, so save as floats
	DEFINE_FIELD( CRevertSaved, m_loadTime, FIELD_FLOAT ),
};

IMPLEMENT_SAVERESTORE( CRevertSaved, CPointEntity );

void CRevertSaved :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "duration"))
	{
		SetDuration( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "holdtime"))
	{
		SetHoldTime( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "messagetime"))
	{
		SetMessageTime( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "loadtime"))
	{
		SetLoadTime( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else
		CPointEntity::KeyValue( pkvd );
}

void CRevertSaved :: Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	UTIL_ScreenFadeAll( pev->rendercolor, Duration(), HoldTime(), pev->renderamt, FFADE_OUT );
	pev->nextthink = gpGlobals->time + MessageTime();
	SetThink(&CRevertSaved::MessageThink);
}

void CRevertSaved :: MessageThink(void)
{
	UTIL_ShowMessageAll( STRING(pev->message) );
	float nextThink = LoadTime() - MessageTime();
	if ( nextThink > 0 )
	{
		pev->nextthink = gpGlobals->time + nextThink;
		SetThink(&CRevertSaved::LoadThink);
	}
	else
		LoadThink();
}

void CRevertSaved :: LoadThink(void)
{
	if ( !gpGlobals->deathmatch )
	{
		SERVER_COMMAND("reload\n");
	}
}


//=========================================================
// Multiplayer intermission spots.
//=========================================================
class CInfoIntermission:public CPointEntity
{
	virtual void Spawn(void);
	virtual void Think(void);
};

void CInfoIntermission::Spawn(void)
{
	UTIL_SetOrigin(pev, pev->origin);
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;
	pev->v_angle = g_vecZero;
	pev->nextthink = gpGlobals->time + 2;// let targets spawn!
}

void CInfoIntermission::Think(void)
{
	edict_t *pTarget = FIND_ENTITY_BY_TARGETNAME(NULL, STRING(pev->target));
	if (!FNullEnt(pTarget))
	{
		pev->v_angle = UTIL_VecToAngles((pTarget->v.origin - pev->origin).Normalize());
		pev->v_angle.x = -pev->v_angle.x;
	}
}

LINK_ENTITY_TO_CLASS( info_intermission, CInfoIntermission );




LINK_ENTITY_TO_CLASS(info_landmark, CPointEntity);// special case
LINK_ENTITY_TO_CLASS(info_player_start, CPointEntity);


class CBasePlayerStart : public CBaseDelay
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	virtual int	ObjectCaps(void) { return CBaseDelay::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }// XDM3035c: fixfix
	virtual BOOL IsTriggered(CBaseEntity *pEntity);
};

LINK_ENTITY_TO_CLASS(info_player_deathmatch, CBasePlayerStart);
LINK_ENTITY_TO_CLASS(info_ctfspawn, CBasePlayerStart);// OP4CTF compatibility
LINK_ENTITY_TO_CLASS(info_player_team1, CBasePlayerStart);// stupid AGCTF compatibility
LINK_ENTITY_TO_CLASS(info_player_team2, CBasePlayerStart);

void CBasePlayerStart::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "team_no"))// OP4CTF compatibility
	{
		pev->team = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "checkpoint"))// XDM3035c: works like master
	{
		pev->noise = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue(pkvd);
}

void CBasePlayerStart::Spawn(void)
{
	// Convert to normal XDM player start
	if (strncmp(STRING(pev->classname), "info_player_team", 16) == 0)
	{
		if ((STRING(pev->classname))[16] == '1')
			pev->team = TEAM_1;
		else if ((STRING(pev->classname))[16] == '2')
			pev->team = TEAM_2;

		pev->classname = MAKE_STRING("info_player_deathmatch");
	}
	CBaseDelay::Spawn();//Precache();
	pev->takedamage = DAMAGE_NO;
	pev->movetype = MOVETYPE_NONE;// this ensures entity won't move even with non-zero velocity (which is possible)
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;
	pev->frame = 0;
	pev->model = 0;
	pev->modelindex = 0;
	UTIL_SetSize(pev, VEC_HULL_MIN, VEC_HULL_MAX);// XDM3035b: should (only) be useful for realtime placement
}

BOOL CBasePlayerStart::IsTriggered(CBaseEntity *pEntity)
{
	if (!FStringNull(pev->noise))// XDM3035c: checkpoint must be activated for this to pass
	{
		if (pEntity->IsPlayer())
		{
			CBaseEntity *pCheckPoint = UTIL_FindEntityByTargetname(NULL, STRING(pev->noise));
			if (pCheckPoint)
			{
				if (((CBasePlayer *)pEntity)->PassedCheckPoint(pCheckPoint) == false)
					return FALSE;
			}
		}
	}
	return !IsLockedByMaster();// XDM3035c
}


//=========================================================
// XDM: spawn point selection
//=========================================================

//DLL_GLOBAL CBaseEntity	*g_pLastSpawnCleared = NULL;

// checks if the spot is clear of players
bool ValidateSpawnPoint(CBaseEntity *pPlayer, CBaseEntity *pSpot, bool clear)
{
	if (pSpot == NULL)
		return false;

	// WHAT THE FUCK! Some mappers don't care about having untriggered spawn spots! So ignore this rule if desperate!
	if (!pSpot->IsTriggered(pPlayer) && !clear)
	{
		ALERT(at_aiconsole, "ValidateSpawnPoint(%d %d %d) failed: not triggered!\n", pPlayer->entindex(), pSpot->entindex(), clear);
		return false;
	}

	if (g_pGameRules->IsTeamplay())
	{
		if (pSpot->pev->team > 0 && (pSpot->pev->team != pPlayer->pev->team))
		{
			ALERT(at_aiconsole, "ValidateSpawnPoint(%d %d %d) failed: wrong team %d (req %d)!\n", pPlayer->entindex(), pSpot->entindex(), clear, pSpot->pev->team ,pPlayer->pev->team);
			return false;
		}
	}

	CBaseEntity *pEntity = NULL;
	CBaseEntity *pList[32];
	int count = UTIL_EntitiesInBox(pList, 32, pSpot->pev->origin+Vector(-32,-32,-32), pSpot->pev->origin+Vector(32,32,32), FL_CLIENT|FL_MONSTER);
//	while ((ent = UTIL_FindEntityInSphere(ent, pSpot->pev->origin, 64.0f)) != NULL)
	for (int i = 0; i < count; ++i)
	{
		pEntity = pList[i];
		if (pEntity->pev->solid == SOLID_NOT || pEntity->pev->solid == SOLID_TRIGGER)// something non-solid
			continue;
		if (pEntity == pPlayer)// found self
			continue;
		if (pEntity->ShouldCollide(pPlayer) == 0)// not an obstacle WARNING: normally we need to call top-level engine collision checking function, which will eventually call into these
			continue;
//		ALERT(at_console, "Spot %s entindex %d found %s %d\n", STRING(pSpot->pev->classname), pSpot->entindex(), STRING(ent->pev->classname), ent->entindex());
		if (pEntity->IsPlayer() || pEntity->IsMonster())
		{
			if (clear)// && g_pLastSpawnCleared != pSpot)// don't clear two times in a row
			{
				pEntity->TakeDamage(g_pWorld, g_pWorld, pEntity->pev->max_health*2.0f, (DMG_CRUSH|DMG_IGNOREARMOR));
				// delay respawn of it a little bit
				if (pEntity->IsPlayer())
					((CBasePlayer *)pEntity)->m_flLastSpawnTime = gpGlobals->time + 2.0f;
				else if (pEntity->pev->nextthink > 0)
					pEntity->pev->nextthink += 2.0f;

//				g_pLastSpawnCleared = pSpot;
			}
			else
			{
				ALERT(at_aiconsole, "ValidateSpawnPoint(%d %d %d) failed: occupied by %d!\n", pPlayer->entindex(), pSpot->entindex(), clear, pEntity->entindex());
				return false;// if ent is a client, don't spawn on 'em
			}
		}
	}
	ALERT(at_aiconsole, "ValidateSpawnPoint(%d %d %d) accepted.\n", pPlayer->entindex(), pSpot->entindex(), clear);
	return true;
}

//-----------------------------------------------------------------------------
// XDM: this whole system is still not good!!
// It only supports ONE spot type for the game (map), but some shitty mods
// like AGCTF use different entities for different teams (which is stupid,
// but nobody cares). I don't want to spend my time digging into that shit.
//-----------------------------------------------------------------------------

#define SPAWNSPOT_RANDOMIZE		1// 1/2 chance to use this entity (if exists)
#define SPAWNSPOT_FORCE			2// disintegrate everybody in the way (on second try)
#define SPAWNSPOT_DONTSAVE		4// don't save into g_pLastSpawn
//#define SPAWNSPOT_TEAMSPECIFIC	8// for specific team only

typedef struct
{
	const char *classname;
	byte flags;
} spawnspot_t;

// WARNING! Look carefully where these are used before changing!
spawnspot_t gSpawnSpotsSpectator[] =
{
	{"info_intermission", 0},
	{"light_spot", 0},
	{"light_environment", 0},
	{"info_player_start", 0},
	{"info_player_deathmatch", 0},
	{"info_landmark", 0}
};

spawnspot_t gSpawnSpotsSingleplayer[] =
{
	{"info_player_start", 0},
	{"info_landmark", 0},
	{"info_player_deathmatch", 0}
//	{"info_intermission", 0}
};
/*
spawnspot_t gSpawnSpotsChangeLevel[] =
{
	{"info_landmark", SPAWNSPOT_FORCE},
};
*/
// the rarest and best go first!
spawnspot_t gSpawnSpotsMultiplayer[] =
{
	{"info_ctfspawn",			SPAWNSPOT_RANDOMIZE|SPAWNSPOT_FORCE},
	{"info_player_deathmatch",	SPAWNSPOT_RANDOMIZE|SPAWNSPOT_FORCE},
	{"info_player_start",		SPAWNSPOT_DONTSAVE|SPAWNSPOT_FORCE}// SPAWNSPOT_DONTSAVE?
	// DO NOT add non-player-start entities here!
};

// experimental
spawnspot_t gSpawnSpotsCoOp[] =
{
	{"info_player_deathmatch",	SPAWNSPOT_RANDOMIZE|SPAWNSPOT_FORCE},
//UNDONE	{"trigger_autosave",		SPAWNSPOT_FORCE},
	{"info_player_start",		SPAWNSPOT_DONTSAVE|SPAWNSPOT_FORCE}
};

DLL_GLOBAL spawnspot_t		*g_pSpotList = NULL;
DLL_GLOBAL CBaseEntity		*g_pLastSpawn = NULL;
DLL_GLOBAL unsigned short	g_usSpawnPointType = 0;

//-----------------------------------------------------------------------------
// Purpose: determine which spawn spots will be used in this map
//-----------------------------------------------------------------------------
void SpawnPointInitialize(void)
{
	if (g_pGameRules->GetGameType() == GT_COOP)// XDM3035c: experimental
		g_pSpotList = gSpawnSpotsCoOp;
	else if (g_pGameRules->IsMultiplayer())
		g_pSpotList = gSpawnSpotsMultiplayer;
//	else if (gpGlobals->startspot)// has targetname // This function is not used during level change anyway
//		g_pSpotList = gSpawnSpotsChangeLevel;
	else
		g_pSpotList = gSpawnSpotsSingleplayer;

	// NEW system: now we detect which spot type to use on this map. Other possible types will be ignored.
	unsigned short s = sizeof(g_pSpotList);
	g_usSpawnPointType = 0;
	// maybe count each type?
	for (; g_usSpawnPointType<s; ++g_usSpawnPointType)// try all available entity types sequentially
		if (UTIL_FindEntityByClassname(NULL, g_pSpotList[g_usSpawnPointType].classname))
			break;// found prioritized type

	SERVER_PRINT(UTIL_VarArgs("SpawnPointInitialize: %s will be used for this map.\n", g_pSpotList[g_usSpawnPointType].classname));
}

//-----------------------------------------------------------------------------
// Purpose: Returns the entity to spawn at
// USES AND SETS GLOBAL g_pLastSpawn
// Input  : *pPlayer - 
// Output : CBaseEntity
//-----------------------------------------------------------------------------
CBaseEntity *SpawnPointEntSelect(CBaseEntity *pPlayer)
{
	CBaseEntity *pSpot = NULL;
	spawnspot_t *pSpotList = NULL;
	bool save;

 	if (g_pGameRules->IsTeamplay() && pPlayer->pev->team == TEAM_NONE)// XDM: unassigned players (spectators)
	{
		pSpotList = gSpawnSpotsSpectator;
		save = false;// don't save last spot for spectators
	}
	else
	{
		pSpotList = g_pSpotList;
		save = true;
	}

	ASSERT(pSpotList != NULL);

	unsigned short etype=g_usSpawnPointType;
	unsigned short searchpass = 0;
	unsigned short retries = 2;
//	CBaseEntity *pFoundSpot = NULL;
	pSpot = g_pLastSpawn;// continue
	for (searchpass=0; searchpass<retries; ++searchpass)// start wiping out only after searching ALL of these spots
	{
// No! Otherwise entities from NULL to LastSpawn will be skipped!	pSpot = g_pLastSpawn;// restart

//		for (unsigned int etype=0; etype<s; ++etype)// OLD system: try all available entity types sequentially
		{
			while ((pSpot = UTIL_FindEntityByClassname(pSpot, pSpotList[etype].classname)) != g_pLastSpawn)// search all spots of this type
			{
				if (pSpot == NULL)
					continue;// just restart
				if (searchpass == 0 && (pSpotList[etype].flags & SPAWNSPOT_RANDOMIZE) && RANDOM_LONG(0,2) == 0)// try randomizing for the first time
					continue;// bad luck, skip

//				if ((pSpotList[etype].flags & SPAWNSPOT_TEAMSPECIFIC) && (pSpot->pev->team != pPlayer->pev->team))
//					continue;

				if (ValidateSpawnPoint(pPlayer, pSpot, ((pSpotList[etype].flags & SPAWNSPOT_FORCE) && (searchpass > 0))))// clear out the area only on the second pass
				{
//					if (pSpotList[etype].flags & SPAWNSPOT_DONTSAVE)// TESTME!
//						save = FALSE;

					goto sps_end;// break all cycles, save and exit
					break;
				}
/* eventually this becomes problem along with g_pLastSpawnCleared
				else if (searchpass > 0)// XDM3035c: no valid spots even after trying with telekill
				{
					unsigned short s = sizeof(g_pSpotList);
					if (s > 1)
					{
						++etype;
						pSpot = NULL;// start from 0
						if (etype >= s)
							etype = 0;

						if (etype == g_usSpawnPointType)// no endless loops
							break;
					}
				}*/
			}
		}
	}

sps_end:
	ASSERTSZ(pSpot != NULL, "Error: SpawnPointEntSelect: no spawn spot!\n");
	if (pSpot == NULL)// shoud never be true!
	{
		pSpot = UTIL_FindEntityByClassname(NULL, g_pSpotList[g_usSpawnPointType].classname);//"info_player_deathmatch");

		if (pSpot == NULL)
			pSpot = UTIL_FindEntityByClassname(NULL, gSpawnSpotsSingleplayer[0].classname);//"info_player_start");
		if (pSpot == NULL)
			pSpot = g_pWorld;
	}
	else
	{
		if (save)
			g_pLastSpawn = pSpot;
	}
	return pSpot;// XDM
}

CBaseEntity *SpawnPointEntSelectSpectator(CBaseEntity *pLast)
{
	CBaseEntity *pSpot = pLast;

	if (pSpot == NULL)
	{
		while ((pSpot = UTIL_FindEntityByClassname(pSpot, "info_intermission")) != NULL)
		{
			if (pLast != NULL || RANDOM_LONG(0,1) == 0)
				break;
		}
	}
	while ((pSpot = UTIL_FindEntityByClassname(pSpot, "info_player_deathmatch")) != NULL)
	{
		if (pLast != NULL || RANDOM_LONG(0,1) == 0)
			break;
	}

	if (pSpot == NULL)
		pSpot = UTIL_FindEntityByClassname(pLast, "light_spot");

	if (pSpot == NULL)
		pSpot = UTIL_FindEntityByClassname(pLast, "light_environment");

	if (pSpot == NULL)
		pSpot = UTIL_FindEntityByClassname(pLast, gSpawnSpotsSingleplayer[0].classname);//"info_player_start");

	return pSpot;// don't save
}
