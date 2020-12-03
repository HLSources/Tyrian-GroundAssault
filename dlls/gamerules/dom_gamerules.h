//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2013
//-----------------------------------------------------------------------------
#ifndef DOM_GAMERULES_H
#define DOM_GAMERULES_H

class CGameRulesDomination : public CGameRulesTeamplay
{
public:
//	CGameRulesDomination(); WARNING!! This causes weird network error on level change!
	virtual short GetGameType(void) { return GT_DOMINATION; };// XDM3035
	virtual void InitHUD(CBasePlayer *pPlayer);
	virtual const char *GetGameDescription(void) {return "XHL Domination";}
	virtual TEAM_ID GetBestTeam(void);
//	virtual CBaseEntity *GetTeamBaseEntity(TEAM_ID team);
};

#endif // DOM_GAMERULES_H
