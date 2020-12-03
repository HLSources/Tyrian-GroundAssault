//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2013
//-----------------------------------------------------------------------------
#ifndef ROUND_GAMERULES_H
#define ROUND_GAMERULES_H

// right now this is a stub

enum
{
	ROUND_STATE_WAITING = 0,// waiting for players to join
	ROUND_STATE_SPAWNING,// players spawn only at this point
	ROUND_STATE_ACTIVE,// the game is on, respawning is not allowed
	ROUND_STATE_FINISHED// round has ended, announce scores
};// round_state_e;


// Round-based game rules
// Rounds can be played without changing map.
// Possible derived game rules: assault, defuse, etc.
class CGameRulesRoundBased : public CGameRulesTeamplay
{
public:
	CGameRulesRoundBased();

	virtual void Initialize(void);
	virtual short GetGameType(void) { return GT_ROUND; };
//	virtual bool ClientConnected(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128]);
//	virtual void ClientDisconnected(CBasePlayer *pPlayer);
//	virtual void InitHUD(CBasePlayer *pPlayer);
	virtual void StartFrame(void);
	virtual const char *GetGameDescription(void) {return "XHL ROUND";}

	virtual bool FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker);

	virtual void PlayerSpawn(CBasePlayer *pPlayer);

	virtual void EndMultiplayerGame(void);
	virtual void ChangeLevel(void);

	virtual void RoundStart(void);
	virtual void RoundEnd(void);

	virtual short GetRoundsLimit(void);
	virtual short GetRoundsPlayed(void) { return m_iRoundsCompleted; };


	float m_fRoundStartTime;
	short m_iRoundsCompleted;// also serves as current round index
	short m_iRoundState;// round_state_e

//	cvar_t	m_cvRoundTime;
};

#endif // ROUND_GAMERULES_H
