//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2013
//-----------------------------------------------------------------------------
#ifndef LMS_GAMERULES_H
#define LMS_GAMERULES_H

// Last Man Standing
// Subclass CGameRulesRoundBased to inherit players join mechanism
class CGameRulesLMS : public CGameRulesRoundBased
{
public:
	virtual const char *GetGameDescription(void) { return "XHL LMS"; }
	virtual short GetGameType(void) { return GT_LMS; };// XDM3035

	virtual void InitHUD(CBasePlayer *pPlayer);
	virtual bool CheckLimits(void);
	virtual void PlayerKilled(CBasePlayer *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor);
	virtual bool FForceRespawnPlayer(void) { return true; };
	virtual bool FAllowSpectatorChange(CBasePlayer *pPlayer)  { return false; };
	virtual CBasePlayer *GetBestPlayer(TEAM_ID team);

	int		m_iLastCheckedNumActivePlayers;
	int		m_iLastCheckedNumFinishedPlayers;
};

#endif // LMS_GAMERULES_H
