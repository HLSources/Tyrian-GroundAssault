//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2013
//-----------------------------------------------------------------------------
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "game.h"
#include "gamerules.h"
#include "teamplay_gamerules.h"
#include "ctf_gamerules.h"
#include "globals.h"
#include "hltv.h"

#define CTF_OBJECT_CLASSNAME		"info_capture_obj"
#define CTF_BASE_CLASSNAME			"trigger_cap_point"

//-----------------------------------------------------------------------------
// Purpose: First thing called after constructor. Initialize all data, cvars, etc.
//-----------------------------------------------------------------------------
void CGameRulesCTF::Initialize(void)
{
	m_MapHasCaptureZones = false;// XDM3037: fasterizer
	CGameRulesTeamplay::Initialize();
}

//-----------------------------------------------------------------------------
// Purpose: Initialize client HUD
// Input  : *pPlayer - 
//-----------------------------------------------------------------------------
void CGameRulesCTF::InitHUD(CBasePlayer *pPlayer)
{
	CGameRulesTeamplay::InitHUD(pPlayer);

	if (!pPlayer->IsBot())
	{
		// send flags info
		int c = 0;
		CBaseEntity *pEntity = NULL;
		while ((pEntity = UTIL_FindEntityByClassname(pEntity, CTF_OBJECT_CLASSNAME)) != NULL)
		{
/*			PLAYBACK_EVENT_FULL(FEV_RELIABLE|FEV_HOSTONLY, ENT(pPlayer->pev), g_usCaptureObject, 0.0,
				(float *)&pEntity->pev->origin, (float *)&pEntity->pev->angles,
				0.0, 0.0, pEntity->entindex(), CTF_EV_INIT, pEntity->pev->team, 0);*/
			MESSAGE_BEGIN(MSG_ONE, gmsgFlagInfo, NULL, pPlayer->edict());
				WRITE_SHORT(pEntity->entindex());
				WRITE_BYTE(pEntity->pev->team);
			MESSAGE_END();

			SetTeamBaseEntity(pEntity->pev->team, pEntity);// XDM3035: 20091122
			++c;
		}
		ALERT(at_aiconsole, "CGameRulesCTF: initialized %d capture objects\n", c);
	}
}

//-----------------------------------------------------------------------------
// Purpose: client has been disconnected
// Input  : *pPlayer - 
//-----------------------------------------------------------------------------
void CGameRulesCTF::ClientDisconnected(CBasePlayer *pPlayer)
{
	if (pPlayer && pPlayer->m_pCarryingObject)
		pPlayer->m_pCarryingObject->Use(pPlayer, pPlayer, USE_TOGGLE/*COU_DROP*/, 0.0);

	CGameRulesTeamplay::ClientDisconnected(pPlayer);
}

//-----------------------------------------------------------------------------
// Purpose: player was killed
// Input  : *pVictim - 
//			*pKiller - 
//			*pInflictor - 
//-----------------------------------------------------------------------------
void CGameRulesCTF::PlayerKilled(CBasePlayer *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor)
{
	if (pVictim->m_pCarryingObject)
		pVictim->m_pCarryingObject->Use(pVictim, pVictim, USE_TOGGLE/*COU_DROP*/, 0.0);

	CGameRulesTeamplay::PlayerKilled(pVictim, pKiller, pInflictor);
}

//-----------------------------------------------------------------------------
// Purpose: Overridden for teamplay
// Input  : team - may be TEAM_NONE
// Output : CBasePlayer *
//-----------------------------------------------------------------------------
/*CBasePlayer *CGameRulesCTF::GetBestPlayer(TEAM_ID team)
{
	if (team == TEAM_NONE)
		return NULL;

	return CGameRulesMultiplay::GetBestPlayer(team);
}*/

//-----------------------------------------------------------------------------
// Purpose: Get team with the best score (extrascore version)
// Output : TEAM_ID
//-----------------------------------------------------------------------------
TEAM_ID CGameRulesCTF::GetBestTeam(void)
{
	int bestscore = 0;
	TEAM_ID bestteam = TEAM_NONE;
	int ts = 0;
	for (int i = TEAM_1/* check this */; i < m_iNumTeams; ++i)
	{
		ts = m_Teams[i].extrascore;
		if (ts > bestscore)
		{
			bestscore = ts;
			bestteam = i;
		}
	}
	if (bestscore <= 0)
		return TEAM_NONE;

	return bestteam;
}

//-----------------------------------------------------------------------------
// Purpose: Locate team base. Literally. Home capture point.
// Input  : team - TEAM_ID
// Output : CBaseEntity *can be anything, but now it is trigger_cap_point
//-----------------------------------------------------------------------------
/*20091121CBaseEntity *CGameRulesCTF::GetTeamBaseEntity(TEAM_ID team)
{
	if (IsValidTeam(team))
	{
		CBaseEntity *pEntity = NULL;
		while ((pEntity = UTIL_FindEntityByClassname(pEntity, "trigger_cap_point")) != NULL)
		{
			if (pEntity->pev->team == team)
				return pEntity;
		}
	}
	return NULL;// g_pWorld?
}*/

//-----------------------------------------------------------------------------
// Purpose: Score limit for this game type
//-----------------------------------------------------------------------------
int CGameRulesCTF::GetScoreLimit(void)
{
	return (int)mp_capturelimit.value;
}






//=================================================================
// CCaptureObject
// WARNING! Team indexes start from 1! (TEAM_1)
//=================================================================
class CCaptureObject : public CBaseAnimating
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void Think(void);
	virtual void Touch(CBaseEntity *pOther);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	virtual void Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib);
	virtual int ShouldCollide(CBaseEntity *pOther);
	virtual BOOL IsPushable(void);
	virtual BOOL IsGameGoal(void) { return TRUE; };

	virtual void Captured(CBaseEntity *pPlayer);
	virtual void Taken(CBaseEntity *pPlayer);
	virtual void Drop(CBaseEntity *pPlayer);
	virtual void Return(CBaseEntity *pPlayer);
	virtual void Reset(CBaseEntity *pPlayer);
};

LINK_ENTITY_TO_CLASS(info_capture_obj, CCaptureObject);
LINK_ENTITY_TO_CLASS(item_ctfflag, CCaptureObject);// Opposing Force CTF compatibility
LINK_ENTITY_TO_CLASS(item_flag_team1, CCaptureObject);// stupid AGCTF compatibility
LINK_ENTITY_TO_CLASS(item_flag_team2, CCaptureObject);// stupid AGCTF compatibility

void CCaptureObject::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "goal_no"))// OP4CTF compatibility
	{
		pev->team = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
	{
		CBaseAnimating::KeyValue(pkvd);
	}
}

void CCaptureObject::Spawn(void)
{
	if (g_pGameRules->GetGameType() != GT_CTF)
	{
		ALERT(at_aiconsole, "CCaptureObject: removed because of game rules mismatch\n");
		UTIL_Remove(this);
		return;
	}
	if (strncmp(STRING(pev->classname), "item_flag_team", 14) == 0)// stupid AGCTF compatibility
	{
		if ((STRING(pev->classname))[14] == '1')
			pev->team = TEAM_1;
		else if ((STRING(pev->classname))[14] == '2')
			pev->team = TEAM_2;

		pev->classname = MAKE_STRING(CTF_OBJECT_CLASSNAME);
	}
	Precache();
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_TRIGGER;
	pev->takedamage = DAMAGE_NO;

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model));
	else
		SET_MODEL(ENT(pev), "models/flag.mdl");

	UTIL_SetSize(pev, Vector(-32,-32,0), Vector(32,32,64));
	UTIL_SetOrigin(pev, pev->origin);

//	pev->flags |= FL_DRAW_ALWAYS;// XDM3034: TESTME: always send position to clients!
	pev->renderfx = kRenderFxGlowShell;
	pev->renderamt = 4;

	byte r = 255, g = 255, b = 255;
	GetTeamColor(pev->team, r,g,b);
	pev->rendercolor.x = r;
	pev->rendercolor.y = g;
	pev->rendercolor.z = b;
	if (pev->team == TEAM_1)
	{
		pev->targetname = MAKE_STRING(CTF_OBJ_TARGETNAME1);
//		pev->rendercolor = g_iTeamColors[pev->team];//Vector(0,255,0);
		pev->skin = 0;
	}
	else if (pev->team == TEAM_2)
	{
		pev->targetname = MAKE_STRING(CTF_OBJ_TARGETNAME2);
//		pev->rendercolor = Vector(0,0,255);
		pev->skin = 1;
	}
//	else
//		pev->rendercolor = Vector(127,255,255);

	m_vecSpawnSpot= pev->origin;
// XDM3035c: avoid using these!	pev->startpos = pev->origin;
	pev->v_angle = pev->angles;
	pev->animtime = gpGlobals->time + 0.5f;
	Reset(NULL);

	pev->impulse = CO_STAY;
	pev->nextthink = gpGlobals->time + 0.5f;
}

void CCaptureObject::Precache(void)
{
	if (pev->model)
		PRECACHE_MODEL(STRINGV(pev->model));
	else
		PRECACHE_MODEL("models/flag.mdl");

	PRECACHE_SOUND("game/ctf_alarm.wav");
	PRECACHE_SOUND("game/ctf_captured.wav");
}

void CCaptureObject::Touch(CBaseEntity *pOther)
{
	if (g_pGameRules->IsGameOver())// XDM3035a: don't react after the game has ended
		return;

//	ALERT(at_console, "CCaptureObject::Touch(%d %s %d)\n", pOther->entindex(), STRING(pOther->pev->netname), pOther->pev->team);
	if (pev->impulse == CO_CARRIED)
		return;

	if (!pOther->IsPlayer())
		return;

	if (!pOther->IsAlive())
		return;

	if (pOther->pev->team == pev->team)
	{
		if (pev->impulse == CO_DROPPED)// pick up our dropped flag
		{
			Return(pOther);
		}
		else if (pev->impulse == CO_STAY)// teammate touched home flag
		{
			// consider this as touchnig the trigger
			CBasePlayer *pPlayer = (CBasePlayer *)pOther;
			if (pPlayer->m_pCarryingObject)
			{
//				if (UTIL_FindEntityByClassname(NULL, CTF_BASE_CLASSNAME) == NULL)// SLOW!!! only if there's no real trigger! This may ruin mapper's intentions.
				if (g_pGameRules->GetGameType() == GT_CTF)// extra check
				{
					if (((CGameRulesCTF *)g_pGameRules)->m_MapHasCaptureZones == false)
						pPlayer->m_pCarryingObject->Use(pOther, this, (USE_TYPE)COU_CAPTURE, 0.0f);
				}
			}
		}
	}
	else
	{
		if (pev->dmgtime <= gpGlobals->time)// prevent self-touching
		{
			Taken(pOther);
		}
	}
}

void CCaptureObject::Think(void)
{
	if (pev->impulse == CO_DROPPED)
	{
		if (pev->teleport_time > 0.0f && pev->teleport_time <= gpGlobals->time)
			Return(NULL);
	}
	else if (pev->impulse == CO_CARRIED)
	{
		if (pev->aiment)// player
		{
			pev->origin = pev->aiment->v.origin;

			if (pev->aiment->v.velocity.Length() > 220)// value from CBasePlayer::SetAnimation
				pev->sequence = FANIM_CARRIED;
			else
				pev->sequence = FANIM_CARRIED_IDLE;
		}
		else// ?!!
			Return(NULL);
	}

	pev->nextthink = gpGlobals->time + 0.1;
}

void CCaptureObject::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (useType == COU_CAPTURE)
	{
		Captured(pActivator);
	}
	else if (useType == COU_TAKEN)
	{
		Taken(pActivator);
	}
	else if (useType == COU_DROP)
	{
		Drop(pActivator);
//		if (value > 0.0)
//			pev->velocity = pev->velocity*value;
	}
	else if (useType == COU_RETURN)
	{
		Return(pActivator);
	}
	else
	{
		ALERT(at_aiconsole, "CCaptureObject: unknown use type %d\n", useType);
	}
}

int CCaptureObject::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pev->impulse == CO_DROPPED)
	{
		if (pInflictor && pInflictor->IsBSPModel())
		{
			if (!pAttacker || !pAttacker->IsPlayer())
			{
				pev->health -= flDamage;
				if (pev->health <= 0.0f)
				{
					Return(NULL);// this will call Reset() and restore pev->health
				}
				return 1;
			}
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: never let this object be destroyed!
//-----------------------------------------------------------------------------
void CCaptureObject::Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib)
{
//	ALERT(at_aiconsole, "CCaptureObject: Killed()!\n");
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pOther - 
// Output : int
//-----------------------------------------------------------------------------
int CCaptureObject::ShouldCollide(CBaseEntity *pOther)// XDM3036
{
	if (pOther->IsPlayer() && !pOther->IsAlive())// ignore/fall through dead players
		return 0;

	return CBaseAnimating::ShouldCollide(pOther);
}

//-----------------------------------------------------------------------------
// Purpose: apply physics on dropped flags
//-----------------------------------------------------------------------------
BOOL CCaptureObject::IsPushable(void)
{
	if (pev->impulse == CO_DROPPED)
		return TRUE;

	return FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: detach flag from a player (if any) and reset state to 'not carried'
// Input  : *pPlayer - a player the flag will be detached from (can be NULL)
//-----------------------------------------------------------------------------
void CCaptureObject::Reset(CBaseEntity *pPlayer)
{
	if (pPlayer && pPlayer->IsPlayer())
	{
		CBasePlayer *pClient = (CBasePlayer *)pPlayer;
		if (pClient)
		{
			if (pClient->m_pCarryingObject == this)// WARNING: Reset() may be called for returning ANOTHER, NON-CARRIED player team's flag!
				pClient->m_pCarryingObject = NULL;
		}
	}

	pev->health = 100;
	ResetSequenceInfo();
	pev->movetype = MOVETYPE_NONE;
	pev->aiment = NULL;
	pev->solid = SOLID_TRIGGER;
	pev->sequence = FANIM_NOT_CARRIED;
	pev->owner = NULL;
	SUB_UseTargets(pPlayer, USE_OFF, 1.0f);// XDM3035a: in case mapper wants to use this
}

//-----------------------------------------------------------------------------
// Purpose: drop and detach flag
// Input  : *pPlayer - a player that dropped the flag (can be NULL)
//-----------------------------------------------------------------------------
void CCaptureObject::Drop(CBaseEntity *pPlayer)
{
	edict_t *e = NULL;
	if (pPlayer)
	{
		pev->origin = pPlayer->pev->origin;
		UTIL_SetOrigin(pev, pev->origin);
		if (pPlayer->IsAlive())// player used 'drop' command
		{
//bugbug			pev->origin = pPlayer->pev->origin + gpGlobals->v_forward * 32;
			Vector fwd;
			ANGLE_VECTORS(pPlayer->pev->angles, fwd, NULL, NULL);
			pev->velocity = pPlayer->pev->velocity + fwd * 160.0f;
		}
		else
		{
			pev->velocity = pPlayer->pev->velocity * 0.85f;
		}
		pev->flags |= FL_DRAW_ALWAYS;// send to client, light need to follow this entity!
		pev->dmgtime = gpGlobals->time + 0.6f;// prevent self-touching

//test		UTIL_ClientPrintAll(HUD_PRINTTALK, "* %s dropped %s flag\n", STRING(pPlayer->pev->netname), g_pGameRules->GetTeamName(pev->team));
		if (sv_lognotice.value > 0)
		{
			UTIL_LogPrintf("\"%s<%i><%s><%s>\" dropped \"%s\" flag\n",  
				STRING(pPlayer->pev->netname),
				GETPLAYERUSERID(pPlayer->edict()),
				GETPLAYERAUTHID(pPlayer->edict()),
				g_pGameRules->GetTeamName(pPlayer->pev->team),
				g_pGameRules->GetTeamName(pev->team));
		}
		e = pPlayer->edict();
	}
	else
	{
//test		UTIL_ClientPrintAll(HUD_PRINTTALK, "* %s flag dropped\n", g_pGameRules->GetTeamName(pev->team));
		if (sv_lognotice.value > 0)
			UTIL_LogPrintf("\"%s\" flag dropped\n", g_pGameRules->GetTeamName(pev->team));
	}

	Reset(pPlayer);
	pev->impulse = CO_DROPPED;
	pev->movetype = MOVETYPE_TOSS;
	pev->sequence = FANIM_ON_GROUND;
	pev->solid = SOLID_TRIGGER;// XDM3035a
	pev->teleport_time = gpGlobals->time + mp_flagstay.value;
	PLAYBACK_EVENT_FULL(FEV_RELIABLE|FEV_GLOBAL|FEV_UPDATE, e, g_usCaptureObject, 0.0, (float *)&pev->origin, (float *)&pev->angles, 0.0, 0.0, entindex(), CTF_EV_DROP, pev->team, 0);
}

//-----------------------------------------------------------------------------
// Purpose: return flag to its base
// Input  : *pPlayer - the one who returns this flag (can be NULL)
//-----------------------------------------------------------------------------
void CCaptureObject::Return(CBaseEntity *pPlayer)
{
	Reset(pPlayer);
	pev->impulse = CO_STAY;
	pev->origin = m_vecSpawnSpot;
	UTIL_SetOrigin(pev, pev->origin);
	pev->angles = pev->v_angle;

	edict_t *e = NULL;
	if (pPlayer == NULL)
	{
		if (sv_lognotice.value > 0)
			UTIL_LogPrintf("\"%s\" flag returned\n", g_pGameRules->GetTeamName(pev->team));
	}
	else
	{
		e = ENT(pPlayer->pev);

//test		UTIL_ClientPrintAll(HUD_PRINTTALK, "* %s returned %s flag\n", STRING(pPlayer->pev->netname), g_pGameRules->GetTeamName(pev->team));
		if (sv_lognotice.value > 0)
		{
			UTIL_LogPrintf("\"%s<%i><%s><%s>\" returned \"%s\" flag\n",  
				STRING(pPlayer->pev->netname),
				GETPLAYERUSERID(pPlayer->edict()),
				GETPLAYERAUTHID(pPlayer->edict()),
				g_pGameRules->GetTeamName(pPlayer->pev->team),
				g_pGameRules->GetTeamName(pev->team));
		}
	}

	PLAYBACK_EVENT_FULL(FEV_RELIABLE|FEV_GLOBAL|FEV_UPDATE, e, g_usCaptureObject, 0.0, (float *)&pev->origin, (float *)&pev->angles, 0.0, 0.0, entindex(), CTF_EV_RETURN, pev->team, 0);
}

//-----------------------------------------------------------------------------
// Purpose: flag is being captured, add team score and reset the flag
// Input  : *pPlayer - the one who captured this flag
//-----------------------------------------------------------------------------
void CCaptureObject::Captured(CBaseEntity *pPlayer)
{
	if (!pPlayer)
		return;

	g_pGameRules->AddScoreToTeam(pPlayer->pev->team, 1);
	PLAYBACK_EVENT_FULL(FEV_RELIABLE|FEV_GLOBAL|FEV_UPDATE, ENT(pPlayer->pev), g_usCaptureObject, 0.0, (float *)&pev->origin, (float *)&pev->angles, 0.0, 0.0, entindex(), CTF_EV_CAPTURED, pev->team, 0);

	MESSAGE_BEGIN(MSG_SPEC, SVC_DIRECTOR);
		WRITE_BYTE(9);// command length in bytes
		WRITE_BYTE(DRC_CMD_EVENT);
		WRITE_SHORT(pPlayer->entindex());// index number of primary entity
		WRITE_SHORT(entindex());// index number of secondary entity
		WRITE_LONG(14 | DRC_FLAG_DRAMATIC | DRC_FLAG_FACEPLAYER);// eventflags (priority and flags)
	MESSAGE_END();

	Reset(pPlayer);
	if (sv_lognotice.value > 0)
	{
		UTIL_LogPrintf("\"%s<%i><%s><%s>\" captured \"%s\" flag\n",  
			STRING(pPlayer->pev->netname),
			GETPLAYERUSERID(pPlayer->edict()),
			GETPLAYERAUTHID(pPlayer->edict()),
			g_pGameRules->GetTeamName(pPlayer->pev->team),
			g_pGameRules->GetTeamName(pev->team));
	}
	pev->origin = m_vecSpawnSpot;
	UTIL_SetOrigin(pev, pev->origin);
	pev->angles = pev->v_angle;
	pev->impulse = CO_STAY;
}

//-----------------------------------------------------------------------------
// Purpose: flag was picked up by
// Input  : *pPlayer - must be somebody!
//-----------------------------------------------------------------------------
void CCaptureObject::Taken(CBaseEntity *pPlayer)
{
	if (pPlayer && pPlayer->IsPlayer())
	{
		CBasePlayer *pClient = (CBasePlayer *)pPlayer;
		if (pClient)
		{
			if (pClient->m_pCarryingObject == NULL)
			{
				pClient->m_pCarryingObject = this;
				pev->owner = pClient->edict();
				m_flFrameRate = pClient->m_flFrameRate;
			}
			else
			{
				ALERT(at_console, "Flag failed to attach: player %d already has a flag!\n", pPlayer->entindex());
				return;
			}
		}

		if (sv_lognotice.value > 0)
		{
			UTIL_LogPrintf("\"%s<%i><%s><%s>\" takes \"%s\" flag\n",  
				STRING(pPlayer->pev->netname),
				GETPLAYERUSERID(pPlayer->edict()),
				GETPLAYERAUTHID(pPlayer->edict()),
				g_pGameRules->GetTeamName(pPlayer->pev->team),
				g_pGameRules->GetTeamName(pev->team));
		}
		pev->impulse = CO_CARRIED;
		pev->solid = SOLID_NOT;
		pev->aiment = pPlayer->edict();
		pev->framerate = pPlayer->pev->framerate;
		pev->movetype = MOVETYPE_FOLLOW;
		pev->sequence = FANIM_CARRIED;
		PLAYBACK_EVENT_FULL(FEV_RELIABLE|FEV_GLOBAL|FEV_UPDATE, ENT(pPlayer->pev), g_usCaptureObject, 0.0, (float *)&pev->origin, (float *)&pev->angles, 0.0, 0.0, entindex(), CTF_EV_TAKEN, pev->team, 0);

		SUB_UseTargets(pPlayer, USE_ON, 1.0f);// XDM3035a: in case mapper wants to use this
	}
}





//=================================================================
// CCaptureZone
// A brush entity (trigger) to carry the flag to
// WARNING! Team indexes start from 1!!!
//=================================================================
class CCaptureZone : public CBaseEntity
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void Touch(CBaseEntity *pOther);
};

LINK_ENTITY_TO_CLASS(trigger_cap_point, CCaptureZone);
LINK_ENTITY_TO_CLASS(item_ctfbase, CCaptureZone);// OP4CTF compatibility

void CCaptureZone::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "goal_no"))// OP4CTF compatibility
	{
		pev->team = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
	{
		CBaseEntity::KeyValue(pkvd);
	}
}

void CCaptureZone::Spawn(void)
{
	if (g_pGameRules->GetGameType() == GT_CTF)
	{
		((CGameRulesCTF *)g_pGameRules)->m_MapHasCaptureZones = true;
	}
	else
	{
		ALERT(at_aiconsole, "CCaptureZone: removed because of game rules mismatch\n");
		UTIL_Remove(this);
		return;
	}
//	pev->classname = ALLOC_STRING(CTF_BASE_CLASSNAME);
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_TRIGGER;
	pev->takedamage = DAMAGE_NO;
	SET_MODEL(ENT(pev), STRING(pev->model));
	UTIL_SetOrigin(pev, pev->origin);
	pev->angles = pev->v_angle;
	pev->nextthink = 0;

	if (pev->team == TEAM_1)
		pev->target = MAKE_STRING(CTF_OBJ_TARGETNAME1);
	else if (pev->team == TEAM_2)
		pev->target = MAKE_STRING(CTF_OBJ_TARGETNAME2);

	if ((STRING(pev->model))[0] == '*')// is this a trigger?
	{
	if (showtriggers.value <= 0.0f)
		SetBits(pev->effects, EF_NODRAW);
	}
}

// OP4CTF compatibility: this entity may be both a trigger and a studio model
void CCaptureZone::Precache(void)
{
	PRECACHE_MODEL(STRINGV(pev->model));
}

void CCaptureZone::Touch(CBaseEntity *pOther)
{
	if (g_pGameRules->IsGameOver())// XDM3035a: don't react after the game has ended
		return;

	if (!pOther->IsPlayer())
		return;

	if (!pOther->IsAlive())// pev->deadflag != DEAD_NO)
		return;

	if (pev->team > 0 && pOther->pev->team != pev->team)// allow pev->team == 0 - universal capture point! XDM3034
		return;

	CBasePlayer *pPlayer = (CBasePlayer *)pOther;
	if (pPlayer->m_pCarryingObject)
	{
		if (pPlayer->m_pCarryingObject->pev->team != pev->team)// both indexes start from 1
		{
			edict_t *pEnt = FIND_ENTITY_BY_TARGETNAME(NULL, STRING(pev->target));
			if (!FNullEnt(pEnt) && pEnt->v.impulse == CO_STAY)// local flag found and is at base
				pPlayer->m_pCarryingObject->Use(pOther, this, (USE_TYPE)COU_CAPTURE, 0.0f);
		}
	}
}

// OP4CTF compatibility
class CBaseCTFDetect : public CPointEntity
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
};

LINK_ENTITY_TO_CLASS(info_ctfdetect, CBaseCTFDetect);

void CBaseCTFDetect::KeyValue(KeyValueData *pkvd)
{
/*
	score_icon_namebm
	score_icon_nameof
	basedefenddist
	defendcarriertime
	captureassisttime
	poweruprespawntime
	map_score_max
*/
	if (FStrEq(pkvd->szKeyName, "flagreturntime"))
	{
		if (atof(pkvd->szValue) > 0.0f)
			CVAR_DIRECT_SET(&mp_flagstay, pkvd->szValue);

		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "poweruprespawntime"))
	{
		if (atof(pkvd->szValue) > 0.0f)
			CVAR_DIRECT_SET(&mp_itm_resp_time, pkvd->szValue);

		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "basedefenddist"))
	{
		pev->speed = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CPointEntity::KeyValue(pkvd);
}
