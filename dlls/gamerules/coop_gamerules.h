#ifndef COOP_GAMERULES_H
#define COOP_GAMERULES_H

// don't have time to use something better
#include <vector>


class CGameRulesCoOp : public CGameRulesMultiplay
{
public:
	CGameRulesCoOp();
	virtual ~CGameRulesCoOp();

	virtual short GetGameType(void) { return GT_COOP; };// XDM3035
	virtual void InitHUD(CBasePlayer *pPlayer);
	virtual void ServerActivate(edict_t *pEdictList, int edictCount, int clientMax);
	virtual void Initialize(void);
	virtual void StartFrame(void);

	virtual int IPointsForKill(CBaseEntity *pAttacker, CBaseEntity *pKilled);
	virtual void PlayerKilled(CBasePlayer *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor);
	virtual void MonsterKilled(CBaseMonster *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor);

	virtual bool FAllowLevelChange(CBasePlayer *pActivator, char *szNextMap, edict_t *pEntLandmark);
	virtual bool FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker);

	virtual bool IsAllowedToSpawn(CBaseEntity *pEntity);
	virtual bool FAllowMonsters(void) { return true; };
	virtual bool FAllowSpectatorChange(CBasePlayer *pPlayer);
	virtual bool FPersistBetweenMaps(void);

	virtual int PlayerRelationship(CBaseEntity *pPlayer, CBaseEntity *pTarget);
	virtual CBasePlayer *GetBestPlayer(TEAM_ID team);
	virtual int GetScoreLimit(void);
	virtual int GetScoreRemaining(void);

	virtual const char *GetGameDescription(void) {return "XHL CoOp";}
//no!	virtual const char *GetDefaultSpawnEntity(void) { return "info_player_start"; }

	virtual void ChangeLevel(void);

	virtual bool IsCoOp(void) { return TRUE; };
//	virtual CBasePlayer *GetBestPlayer(int team);
//	virtual int GetBestTeam(void);
	bool CheckPlayersTouchedTriggers(bool bCheckTransitionVolume);

protected:
	int		m_iChangeLevelTriggers;
	char	m_szLastMap[MAX_MAPNAME];
	char	m_szNextMap[MAX_MAPNAME];
	edict_t	*m_pEntLandmark;
	int		m_iLastCheckedNumActivePlayers;
	int		m_iLastCheckedNumFinishedPlayers;
	CBasePlayer *m_pFirstPlayer;// player who reached end of level first

	int m_iRegisteredTargets;
	std::vector<CBaseEntity *> m_RegisteredTargets;
	std::vector<CBaseEntity *>::iterator m_TargetIterator;
};

#endif // COOP_GAMERULES_H
