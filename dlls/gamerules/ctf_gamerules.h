//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2013
//-----------------------------------------------------------------------------
#ifndef CTF_GAMERULES_H
#define CTF_GAMERULES_H

class CGameRulesCTF : public CGameRulesTeamplay
{
public:
//	CGameRulesCTF();
	virtual short GetGameType(void) { return GT_CTF; };// XDM3035
	virtual void Initialize(void);// XDM3037
	virtual void InitHUD(CBasePlayer *pPlayer);
	virtual void ClientDisconnected(CBasePlayer *pPlayer);
	virtual void PlayerKilled(CBasePlayer *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor);
	virtual const char *GetGameDescription(void) {return "XHL CTF";}
//	virtual CBasePlayer *GetBestPlayer(TEAM_ID team);
	virtual TEAM_ID GetBestTeam(void);
//	virtual CBaseEntity *GetTeamBaseEntity(TEAM_ID team);
	virtual int GetScoreLimit(void);
	virtual int MaxTeams(void) {return MAX_CTF_TEAMS;};
	bool m_MapHasCaptureZones;
};

#endif // CTF_GAMERULES_H
