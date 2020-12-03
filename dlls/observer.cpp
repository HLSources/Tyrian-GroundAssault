//=========== (C) Copyright 1996-2002, Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Functionality for the observer chase camera
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "gamerules.h"
#include "game.h"
#include "pm_shared.h"// XDM: observer modes
#include "globals.h"
#include "shake.h"
#include "msg_fx.h"

// Handle buttons in observer mode
void CBasePlayer::Observer_HandleButtons(void)
{
	// Slow down mouse clicks
	if (m_flNextObserverInput > gpGlobals->time)
		return;

	// I don't know how this shit fails and I don't want to know
//	if (m_afButtonReleased + m_afButtonPressed > 0)
//		ALERT(at_console, "Observer_HandleButtons() P = %d, R = %d\n", m_afButtonReleased, m_afButtonPressed);

	if (pev->iuser1 == OBS_INTERMISSION)
		return;

	// Jump changes from modes: Chase to Roaming
	if (m_afButtonPressed & IN_JUMP)
		Observer_SetMode(pev->iuser1 + 1);

	if (pev->iuser1 != OBS_ROAMING)
	{
		// Attack moves to the next player
		if (m_afButtonPressed & IN_ATTACK)
			Observer_FindNextPlayer(FALSE);
		// Attack2 moves to the prev player
		else if (m_afButtonPressed & IN_ATTACK2)
			Observer_FindNextPlayer(TRUE);
	}
	m_flNextObserverInput = gpGlobals->time + 0.02f;
}

// Find the next client in the game for this player to spectate
void CBasePlayer::Observer_FindNextPlayer(bool bReverse)
{
	// MOD AUTHORS: Modify the logic of this function if you want to restrict the observer to watching
	//				only a subset of the players. e.g. Make it check the target's team.
	int iStart = 0;
	if (m_hObserverTarget)
		iStart = ENTINDEX(m_hObserverTarget->edict());
	else
		iStart = ENTINDEX(edict());

	int iCurrent = iStart;
	m_hObserverTarget = NULL;
	int iDir = bReverse ? -1 : 1; 

	do
	{
		iCurrent += iDir;
		// Loop through the clients
		if (iCurrent > gpGlobals->maxClients)
			iCurrent = 1;

		if (iCurrent < 1)
			iCurrent = gpGlobals->maxClients;

		CBasePlayer *client = UTIL_ClientByIndex(iCurrent);

		if (!client)
			continue;
		if (!client->pev)
			continue;
		if (client == this)
			continue;

		// Don't spec observers or invisible players
		if (client->IsObserver() || (client->pev->effects & EF_NODRAW))
			continue;

		if (g_pGameRules->IsTeamplay() && mp_specteammates.value > 0.0f)
		{
			//if ((mp_specteammates.value > 0.0f) && pev->team != TEAM_NONE && client->pev->team != pev->team)
//			if (g_pGameRules->PlayerRelationship(this, client) != GR_TEAMMATE)
			//if (m_iLastTeamID != TEAM_NONE && client->pev->team != m_iLastTeamID)// XDM3035a: since pev->team is rest
			if (pev->playerclass != TEAM_NONE && client->pev->team != pev->playerclass)
				continue;
		}

		// MOD AUTHORS: Add checks on target here.
		Observer_SetTarget(client);
		//m_hObserverTarget = client;
		break;

	} while (iCurrent != iStart);

	// Did we find a target?
	if (m_hObserverTarget)
	{
/*
		// Store the target in pev so the physics DLL can get to it
		pev->iuser2 = ENTINDEX(m_hObserverTarget->edict());
		// Move to the target
		UTIL_SetOrigin(pev, m_hObserverTarget->pev->origin);
//		ALERT(at_console, "Now Tracking %s\n", STRING(m_hObserverTarget->pev->netname));
*/
	}
	else
		ClientPrint(pev, HUD_PRINTNOTIFY, "#Spec_NoTarget");
}

// Attempt to change the observer mode
void CBasePlayer::Observer_SetMode(int iMode)
{
	// Just abort if we're changing to the mode we're already in
	if (iMode == pev->iuser1)
		return;

	if (iMode > OBS_MAP_CHASE)// XDM: loop
		iMode = OBS_CHASE_LOCKED;

	if (iMode == OBS_CHASE_LOCKED)// Changing to Chase Lock?
	{
		// If changing from Roaming, or starting observing, make sure there is a target
		if (m_hObserverTarget == NULL)
			Observer_FindNextPlayer(FALSE);

		if (m_hObserverTarget)
		{
			pev->iuser1 = OBS_CHASE_LOCKED;
			pev->iuser2 = ENTINDEX(m_hObserverTarget->edict());
//			ClientPrint(pev, HUD_PRINTCENTER, "#Spec_Mode1");
			pev->maxspeed = 0;
		}
		else
		{
//			ClientPrint(pev, HUD_PRINTCENTER, "#Spec_NoTarget");
			Observer_SetMode(OBS_ROAMING);
		}
	}
	else if (iMode == OBS_CHASE_FREE)// Changing to Chase Freelook?
	{
		// If changing from Roaming, or starting observing, make sure there is a target
		if (m_hObserverTarget == NULL)
			Observer_FindNextPlayer(FALSE);

		if (m_hObserverTarget)
		{
			pev->iuser1 = OBS_CHASE_FREE;
			pev->iuser2 = ENTINDEX(m_hObserverTarget->edict());
//			ClientPrint(pev, HUD_PRINTCENTER, "#Spec_Mode2");
			pev->maxspeed = 0;
		}
		else
		{
//			ClientPrint(pev, HUD_PRINTCENTER, "#Spec_NoTarget");
			Observer_SetMode(OBS_ROAMING);
		}
	}
	else if (iMode == OBS_ROAMING)// Changing to Roaming?
	{
		// MOD AUTHORS: If you don't want to allow roaming observers at all in your mod, just abort here.
		pev->iuser1 = OBS_ROAMING;
		pev->iuser2 = 0;
//		ClientPrint(pev, HUD_PRINTCENTER, "#Spec_Mode3");
		pev->maxspeed = g_psv_maxspeed->value;
	}
	else if (iMode == OBS_IN_EYE)// Changing to First Person?
	{
		// If changing from Roaming, or starting observing, make sure there is a target
		if (m_hObserverTarget == NULL)
			Observer_FindNextPlayer(FALSE);

		if (m_hObserverTarget)
		{
			pev->iuser1 = OBS_IN_EYE;
			pev->iuser2 = ENTINDEX(m_hObserverTarget->edict());
//			ClientPrint(pev, HUD_PRINTCENTER, "#Spec_Mode4");
			pev->maxspeed = 0;
		}
		else
		{
//			ClientPrint(pev, HUD_PRINTCENTER, "#Spec_NoTarget");
			Observer_SetMode(OBS_ROAMING);
		}
	}
	else if (iMode == OBS_MAP_FREE)// Changing to Free Map Overview?
	{
		pev->iuser1 = OBS_MAP_FREE;
		pev->iuser2 = 0;
//		ClientPrint(pev, HUD_PRINTCENTER, "#Spec_Mode5");
		pev->maxspeed = g_psv_maxspeed->value;
	}
	else if (iMode == OBS_MAP_CHASE)// Changing to Chase Map Overview?
	{
		pev->iuser1 = OBS_MAP_CHASE;
		pev->iuser2 = 0;
//		ClientPrint(pev, HUD_PRINTCENTER, "#Spec_Mode6");
	}
/*	else// Other modes
	{
		pev->iuser1 = iMode;
		pev->iuser2 = 0;
		char str[38];
		sprintf(str, "Unknown spectator mode: %d\n", iMode);
		ClientPrint(pev, HUD_PRINTCENTER, str);
		pev->maxspeed = g_psv_maxspeed->value;
	}*/
/*	char str[32];
	sprintf(str, "Observer_SetMode: %d\n", pev->iuser1);
	ClientPrint(pev, HUD_PRINTCENTER, str);*/
}

void CBasePlayer::Observer_SetTarget(CBaseEntity *pTarget)
{
	m_hObserverTarget = pTarget;
	if (m_hObserverTarget)
	{
		// Store the target in pev so the physics DLL can get to it
		pev->iuser2 = ENTINDEX(m_hObserverTarget->edict());
		// Move to the target
		UTIL_SetOrigin(pev, m_hObserverTarget->pev->origin);
//		ALERT(at_console, "Now Tracking %s\n", STRING(m_hObserverTarget->pev->netname));
	}
	else
		pev->iuser2 = 0;
}

// Player has become a spectator. Set it up. This was moved from player.cpp.
void CBasePlayer::StartObserver(const Vector &vecPosition, const Vector &vecViewAngle, int mode, CBaseEntity *pTarget)
{
	// Holster weapon immediately, to allow it to cleanup
	if (m_pActiveItem)
		m_pActiveItem->Holster();

	m_pActiveItem = NULL;// XDM: TESTME!

	if (m_pTank != NULL)
	{
		m_pTank->Use(this, this, USE_OFF, 0);
		m_pTank = NULL;
	}

	TrainDetach();

	// clear out the suit message cache so we don't keep chattering
//	SetSuitUpdate(NULL, FALSE, 0);

	// clear any clientside entities attached to this player
	MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_KILLPLAYERATTACHMENTS);
		WRITE_BYTE((byte)entindex());
	MESSAGE_END();

	// Tell Ammo Hud that the player is dead
	MESSAGE_BEGIN(MSG_ONE, gmsgCurWeapon, NULL, edict());
		WRITE_BYTE(0);
		WRITE_BYTE(0XFF);
		WRITE_BYTE(0xFF);
	MESSAGE_END();

	// reset FOV
	m_iFOV = m_iClientFOV = 0;
	pev->fov = m_iFOV;
	MESSAGE_BEGIN(MSG_ONE, gmsgSetFOV, NULL, edict());
		WRITE_BYTE(0);
	MESSAGE_END();

	m_iScoreCombo		= 0;// XDM3035
	m_iLastScoreAward	= 0;
	m_fNextScoreTime	= 0.0f;
//	m_hLastKiller		= NULL;
//	m_hLastVictim		= NULL;
	m_iLastKiller		= 0;
	m_iLastVictim		= 0;

	// Setup flags
	m_iHideHUD |= HIDEHUD_ALL;
	m_afPhysicsFlags |= PFLAG_OBSERVER;
	pev->effects = EF_NODRAW;
	pev->view_ofs = g_vecZero;
	pev->angles = pev->v_angle = vecViewAngle;
	pev->fixangle = TRUE;
	pev->solid = SOLID_NOT;
	pev->takedamage = DAMAGE_NO;
	pev->movetype = MOVETYPE_NONE;
	ClearBits(m_afPhysicsFlags, PFLAG_DUCKING);
	ClearBits(pev->flags, FL_DUCKING);
	ClearBits(pev->flags, FL_ONGROUND);
	SetBits(pev->flags, FL_SPECTATOR); // XDM: Should we set Spectator flag? Or is it reserved for people connecting with observer 1?
	pev->deadflag = DEAD_RESPAWNABLE;
	pev->health = 1;
	m_iClientHealth = 1;

	MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, edict());
	WRITE_BYTE(MSG_HEALTH);
	WRITE_SHORT(m_iClientHealth);
	MESSAGE_END();

	// Clear out the status bar
	m_fInitHUD = TRUE;
	// Update Team Status
	// Remove all the player's stuff
	RemoveAllItems(FALSE);
	// Move them to the new position
	UTIL_SetOrigin(pev, vecPosition);
	// Find a player to watch
	Observer_SetMode(OBS_ROAMING);
	m_flNextObserverInput = gpGlobals->time;

// XDM: save current team!!! ?
//	m_iLastTeamID = pev->team;
	pev->playerclass = pev->team;// XDM3035: save in entvars to transmit to local client side
	pev->team = TEAM_NONE;
/* not needed - this player's team gets reset on gmsgSpectator
	MESSAGE_BEGIN(MSG_ALL, gmsgTeamInfo);
		WRITE_BYTE(ENTINDEX(edict()));
		WRITE_BYTE(pev->team);
	MESSAGE_END(); */
	// Tell all clients this player is now a spectator
	MESSAGE_BEGIN(MSG_ALL, gmsgSpectator);
		WRITE_BYTE(ENTINDEX(edict()));
		WRITE_BYTE(1);
	MESSAGE_END();

	g_pGameRules->ClientDisconnected(this);// XDM HACK!!!!!!

	UTIL_ScreenFade(this, Vector(255,255,255), 1.0f, 0.1f, 255, FFADE_IN);// XDM3035: we need this to clear out remaining screen fades

	// Tell the physics code that this player's now in observer mode
	pev->iuser1 = 0;// nust not be equal to 'mode'
	Observer_SetTarget(pTarget);
	Observer_SetMode(mode);

	if (pev->iuser1 != OBS_ROAMING && m_hObserverTarget.Get() == NULL)
		Observer_FindNextPlayer(FALSE);// Find a player to watch
}

// Leave observer mode
void CBasePlayer::StopObserver(void)
{
	if (!g_pGameRules->FAllowSpectatorChange(this))// XDM
		return;

	if (pev->team == TEAM_NONE && g_pGameRules->IsTeamplay())// 3033: player wants to join the game without selecting a team
	{
//		ALERT(at_console, "Spectator %d wanted to join the game\n", entindex());
		CLIENT_COMMAND(ENT(pev), "chooseteam\n");
		return;
	}

	// Turn off spectator
	if (pev->iuser1 || pev->iuser2)
	{
		// Tell all clients this player is not a spectator anymore
		MESSAGE_BEGIN(MSG_ALL, gmsgSpectator);
			WRITE_BYTE(ENTINDEX(edict()));
			WRITE_BYTE(0);
		MESSAGE_END();
		pev->iuser1 = 0;
		pev->iuser2 = 0;
	}

	ClearBits(pev->flags, FL_SPECTATOR);

	m_iHideHUD &= ~HIDEHUD_ALL;
	m_hObserverTarget = NULL;
	m_afPhysicsFlags = 0;
	m_flNextObserverInput = -1;

//	m_hLastKiller		= NULL;
//	m_hLastVictim		= NULL;
	m_iLastKiller		= 0;
	m_iLastVictim		= 0;

	pev->effects = 0;
	pev->fixangle = FALSE;
	pev->maxspeed = g_psv_maxspeed->value;
//	pev->team = TEAM_NONE;// DON'T! This is called after CHalfLifeTeamplay::ChangePlayerTeam

	Spawn();
	g_pGameRules->InitHUD(this);
}

bool CBasePlayer::IsObserver(void)
{
	if (pev->iuser1 != OBS_NONE && pev->iuser1 != OBS_INTERMISSION)
		return true;

	return false;
}
