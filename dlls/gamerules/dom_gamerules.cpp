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
#include "dom_gamerules.h"
#include "globals.h"
#include "hltv.h"


//-----------------------------------------------------------------------------
// Purpose: Sent initialization messages to client
// Input  : *pPlayer - client
//-----------------------------------------------------------------------------
void CGameRulesDomination::InitHUD(CBasePlayer *pPlayer)
{
	CGameRulesTeamplay::InitHUD(pPlayer);

	if (!pPlayer->IsBot())
	{
		// send checkpoints info
		int c = 0;
		CBaseEntity *pEntity = NULL;
		while ((pEntity = UTIL_FindEntityByClassname(pEntity, "info_dom_target")) != NULL)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgDomInfo, NULL, pPlayer->edict());
				WRITE_SHORT(pEntity->entindex());
				WRITE_BYTE(pEntity->pev->team);// XDM3033: overriddes CInfoDomTarget::SendClientData
				WRITE_STRING(STRING(pEntity->pev->message));
			MESSAGE_END();

			SetTeamBaseEntity(pEntity->pev->team, pEntity);// XDM3035: 20091122
			++c;
		}
		ALERT(at_aiconsole, "CGameRulesDomination: initialized %d checkpoints\n", c);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Get team with the best score (extrascore version)
// Output : TEAM_ID
//-----------------------------------------------------------------------------
TEAM_ID CGameRulesDomination::GetBestTeam(void)
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
// Output : CBaseEntity *can be anything, but now it is info_dom_target
//-----------------------------------------------------------------------------
/*20091121 CBaseEntity *CGameRulesDomination::GetTeamBaseEntity(TEAM_ID team)
{
	if (IsValidTeam(team))
	{
		CBaseEntity *pEntity = NULL;
		while ((pEntity = UTIL_FindEntityByClassname(pEntity, "info_dom_target")) != NULL)
		{
			if (pEntity->pev->team == team)
				return pEntity;
		}
	}
	return NULL;// g_pWorld?
}*/


//=========================================================
// Effect
//=========================================================
class CInfoDomTarget : public CBaseAnimating
{
public:
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void Think(void);
// not needed	virtual void SendClientData(CBasePlayer *pClient, int msgtype);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual BOOL IsGameGoal(void) { return TRUE; };
};

LINK_ENTITY_TO_CLASS(info_dom_target, CInfoDomTarget);

void CInfoDomTarget::Spawn(void)
{
	if (g_pGameRules->GetGameType() != GT_DOMINATION)
	{
		ALERT(at_aiconsole, "CInfoDomTarget: removed because of game rules mismatch\n");
		UTIL_Remove(this);
		return;
	}
	Precache();
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;
	pev->takedamage = DAMAGE_NO;

	if (pev->model)
		SET_MODEL(ENT(pev), STRING(pev->model));
	else
	{
		SET_MODEL(ENT(pev), "models/dom_point.mdl");
		pev->body = RANDOM_LONG(0,4);
	}

	UTIL_SetOrigin(pev, pev->origin);
	UTIL_SetSize(pev, g_vecZero, g_vecZero);
	pev->framerate = 1.0f;
	pev->team = TEAM_NONE;
	pev->rendercolor = Vector(255,255,255);

	if (g_pGameRules->FAllowEffects())
		pev->renderfx = kRenderFxGlowShell;

	CBaseAnimating::Spawn();// starts animation
}

void CInfoDomTarget::Precache(void)
{
	if (pev->model)
		PRECACHE_MODEL(STRINGV(pev->model));
	else
		PRECACHE_MODEL("models/dom_point.mdl");

	PRECACHE_SOUND("game/dom_touch.wav");
}

void CInfoDomTarget::Think(void)
{
	if (pev->team <= TEAM_NONE)
		return;

//	ALERT(at_console, "*** CInfoDomTarget -> AddScoreToTeam %d\n", pev->team);
	g_pGameRules->AddScoreToTeam(pev->team, 1);
	pev->nextthink = gpGlobals->time + mp_domscoreperiod.value;//5.0;
}
/*
// a player has connected and requests update
////// UPDATE: done in CGameRulesDomination::InitHUD
void CInfoDomTarget::SendClientData(CBasePlayer *pClient, int msgtype)
{
	PLAYBACK_EVENT_FULL(FEV_HOSTONLY, ENT(pClient->pev), g_usDomPoint, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, DOM_LIGHT_RADIUS, 0.0, pev->team, entindex(), 0, 0);
}
*/
// used by trigger, activator must be player
void CInfoDomTarget::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!pActivator->IsPlayer())
		return;

	if (value >= 0)// != TEAM_NONE?
	{
		pev->team = (int)value;
		byte r = 127, g = 127, b = 127;
		GetTeamColor(pev->team, r,g,b);
		pev->rendercolor.x = r;
		pev->rendercolor.y = g;
		pev->rendercolor.z = b;
//		pev->body = pev->team;
		pev->skin = pev->team;
		pev->nextthink = gpGlobals->time;
	}
	PLAYBACK_EVENT_FULL(FEV_GLOBAL|FEV_UPDATE, ENT(pActivator->pev), g_usDomPoint, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, DOM_LIGHT_RADIUS, 0.0, pev->team, entindex(), 0, 0);
}


//=========================================================
// Trigger
//=========================================================
class CTriggerDomPoint : public CBaseToggle
{
public:
	virtual void Spawn(void);
	virtual void Touch(CBaseEntity *pOther);
	virtual int	ObjectCaps(void) { return CBaseDelay::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
};

LINK_ENTITY_TO_CLASS(trigger_dom_point, CTriggerDomPoint);

void CTriggerDomPoint::Spawn(void)
{
	if (g_pGameRules->GetGameType() != GT_DOMINATION)
	{
		ALERT(at_aiconsole, "CTriggerDomPoint: removed because of game rules mismatch\n");
		UTIL_Remove(this);
		return;
	}
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_TRIGGER;
	pev->takedamage = DAMAGE_NO;
	SET_MODEL(ENT(pev), STRING(pev->model));
	UTIL_SetOrigin(pev, pev->origin);
	pev->team = TEAM_NONE;

	if (m_flWait < 0.1f)
		m_flWait = 0.5f;

	if (showtriggers.value <= 0.0f)
		SetBits(pev->effects, EF_NODRAW);
}

void CTriggerDomPoint::Touch(CBaseEntity *pOther)
{
	if (g_pGameRules->IsGameOver())// XDM3035a: don't react after the game has ended
		return;

	if (pev->dmgtime > gpGlobals->time)
		return;

	if (!pOther->IsPlayer())
		return;

	if (!pOther->IsAlive())// pev->deadflag != DEAD_NO)
		return;

	if (pOther->pev->team == pev->team)
		return;

	pev->team = pOther->pev->team;
	if (sv_lognotice.value > 0)
	{
		UTIL_LogPrintf("\"%s<%i><%s><%s>\" takes control point \"%s\"\n",  
			STRING(pOther->pev->netname),
			GETPLAYERUSERID(pOther->edict()),
			GETPLAYERAUTHID(pOther->edict()),
			g_pGameRules->GetTeamName(pOther->pev->team),
			STRING(pev->targetname));
	}
	if (!FStringNull(pev->target))
		FireTargets(STRING(pev->target), pOther, this, USE_SET, pev->team);

	MESSAGE_BEGIN(MSG_SPEC, SVC_DIRECTOR);
		WRITE_BYTE(9);// command length in bytes
		WRITE_BYTE(DRC_CMD_EVENT);
		WRITE_SHORT(pOther->entindex());// index number of primary entity
		WRITE_SHORT(entindex());// index number of secondary entity
		WRITE_LONG(14 | DRC_FLAG_DRAMATIC | DRC_FLAG_FACEPLAYER);// eventflags (priority and flags)
	MESSAGE_END();

	pev->dmgtime = gpGlobals->time + m_flWait;
}
