//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2013
//-----------------------------------------------------------------------------
#ifndef TEAMPLAY_GAMERULES_H
#define TEAMPLAY_GAMERULES_H


#include "colors.h"
/*
Colormap is Hue
0 - red
40 - yellow
80 - green
120 - cyan
160 - blue
200 - violet
256 == 0 (hue 360 degrees)
*/
static int teamcolormapdefault[MAX_TEAMS+1] =
{
	COLORMAP_CYAN,
	COLORMAP_GREEN,
	COLORMAP_BLUE,
	COLORMAP_RED,
	COLORMAP_YELLOW, // XDM3035: was COLORMAP_CYAN,
/*	40,
	200,
	280,
	280,
	280,*/
};

typedef struct team_s
{
	char name[MAX_TEAMNAME_LENGTH];
	int score;// frags
	int extrascore;// goals
	int looses;// deaths
	short playercount;
	byte color[3];
	unsigned short colormap;
} team_t;

// really should not be used externally
void GetTeamColor(TEAM_ID team, byte &r, byte &g, byte &b);

class CGameRulesTeamplay : public CGameRulesMultiplay
{
public:
	CGameRulesTeamplay();

	virtual short GetGameType(void) { return GT_TEAMPLAY; };// XDM3035

//	virtual void StartFrame(void);
	virtual void Initialize(void);

	virtual bool IsTeamplay(void) { return true; };
	virtual const char *GetGameDescription(void) { return "XHL Teamplay"; }  // this is the game name that gets seen in the server browser

//	virtual bool ClientConnected(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128]);
	virtual void ClientDisconnected(CBasePlayer *pPlayer);
	virtual bool ClientCommand(CBasePlayer *pPlayer, const char *pcmd);
	virtual void ClientUserInfoChanged(CBasePlayer *pPlayer, char *infobuffer);
	virtual void InitHUD(CBasePlayer *pPlayer);
	virtual bool CheckLimits(void);

	virtual bool FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker);
	virtual bool ShouldAutoAim(CBasePlayer *pPlayer, CBaseEntity *pTarget);

	virtual void PlayerSpawn(CBasePlayer *pPlayer);

	virtual int IPointsForKill(CBaseEntity *pAttacker, CBaseEntity *pKilled);
	virtual void PlayerKilled(CBasePlayer *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor);
	virtual void MonsterKilled(CBaseMonster *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor);// XDM3035a
//	virtual void DeathNotice(CBasePlayer *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor);

	virtual int PlayerRelationship(CBaseEntity *pPlayer, CBaseEntity *pTarget);
	virtual int GetTeamIndex(const char *pTeamName);
	virtual const char *GetTeamName(int teamIndex);
	virtual bool IsValidTeam(const char *pTeamName);
	virtual bool IsValidTeam(int team);
	virtual bool IsRealTeam(int team);
	virtual void ChangePlayerTeam(CBasePlayer *pPlayer, const char *pTeamName, bool bKill, bool bGib);
	virtual void ChangePlayerTeam(CBasePlayer *pPlayer, TEAM_ID teamindex, bool bKill, bool bGib);
	virtual void AddScoreToTeam(TEAM_ID teamIndex, int score);
	virtual int NumPlayersInTeam(TEAM_ID teamIndex);
	virtual int MaxTeams(void) {return MAX_TEAMS;};
	virtual int GetNumberOfTeams(void) { return (m_iNumTeams - 1); }

	virtual CBasePlayer *GetBestPlayer(TEAM_ID team);
	virtual TEAM_ID GetBestTeam(void);

	virtual CBaseEntity *GetTeamBaseEntity(TEAM_ID team);
	virtual void SetTeamBaseEntity(TEAM_ID team, CBaseEntity *pEntity);
//	virtual void EndMultiplayerGame(void);
	virtual int GetScoreLimit(void);
	virtual void DumpInfo(void);

	team_t *GetTeamByID(TEAM_ID team);
	TEAM_ID TeamWithFewestPlayers(void);
	int PlayerIsInTeam(CBasePlayer *pPlayer, TEAM_ID teamIndex);

private:
	void AssignPlayer(CBasePlayer *pPlayer, bool bSend);
	void RecountTeams(bool bResendInfo);

	int CreateNewTeam(const char *pTeamName);
	bool AddPlayerToTeam(CBasePlayer *pPlayer, TEAM_ID teamIndex);
	bool RemovePlayerFromTeam(CBasePlayer *pPlayer, TEAM_ID teamIndex);

	char *GetPlayerTeamName(CBasePlayer *pPlayer);
	void SetPlayerTeamParams(CBasePlayer *pPlayer);

	bool m_DisableDeathMessages;
	bool m_DisableDeathPenalty;
	bool m_teamLimit;// This means the server set only some teams as valid
	TEAM_ID m_LeadingTeam;

protected:
	int m_iNumTeams;// including team 0
	team_t m_Teams[MAX_TEAMS+1];// 0 = unassigned
	CBaseEntity *m_pBaseEntities[MAX_TEAMS+1];
};

#endif // TEAMPLAY_GAMERULES_H
