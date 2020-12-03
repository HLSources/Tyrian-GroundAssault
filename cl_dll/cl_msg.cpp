#include "hud.h"
#include "cl_util.h"
#include "cl_fx.h"
#include "vgui_XDMViewport.h"
#include "parsemsg.h"
#include "event_api.h"
#include "in_defs.h"


int __MsgFunc_InitHUD(const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_InitHUD(pszName, iSize, pbuf);
}

int __MsgFunc_ResetHUD(const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_ResetHUD(pszName, iSize, pbuf);
}

int __MsgFunc_SetFOV(const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_SetFOV(pszName, iSize, pbuf);
}

int __MsgFunc_ViewMode(const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_ViewMode(pszName, iSize, pbuf);
}

int __MsgFunc_GameMode(const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_GameMode(pszName, iSize, pbuf);
}

int __MsgFunc_GRInfo(const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_GRInfo(pszName, iSize, pbuf);
}

int __MsgFunc_GREvent(const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_GREvent(pszName, iSize, pbuf);
}

int __MsgFunc_Logo(const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_Logo(pszName, iSize, pbuf);
}

int __MsgFunc_TeamNames(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_TeamNames(pszName, iSize, pbuf);
	return 0;
}

int __MsgFunc_MOTD(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_MOTD(pszName, iSize, pbuf);
	return 0;
}

int __MsgFunc_ShowMenu(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_ShowMenu(pszName, iSize, pbuf);
	return 0;
}

int __MsgFunc_ServerName(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_ServerName(pszName, iSize, pbuf);
	return 0;
}

int __MsgFunc_ScoreInfo(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_ScoreInfo(pszName, iSize, pbuf);
	return 0;
}

int __MsgFunc_TeamScore(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_TeamScore(pszName, iSize, pbuf);
	return 0;
}

int __MsgFunc_TeamInfo(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_TeamInfo(pszName, iSize, pbuf);
	return 0;
}

int __MsgFunc_Spectator(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_Spectator(pszName, iSize, pbuf);
	return 0;
}

int __MsgFunc_AllowSpec(const char *pszName, int iSize, void *pbuf)
{
	if (gViewPort)
		return gViewPort->MsgFunc_AllowSpec(pszName, iSize, pbuf);
	return 0;
}

/*struct player_stats_t 
{
	NumCombos[SCORE_COMBO_MAX];
	iLastScoreAward;
	iComboBreakerCount;
	iRevengeCount;
	iFailCount;
};
player_stats_t g_PlayerStats[MAX_PLAYERS+1];
*/
int __MsgFunc_PlayerStats(const char *pszName, int iSize, void *pbuf)
{
//	if (gViewPort)
//		return gViewPort->MsgFunc_PlayerStats(pszName, iSize, pbuf);
	return 0;
}

int __MsgFunc_SpeakSnd(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	short entindex	= READ_SHORT();
	float volume	= (float)READ_BYTE()/255.0f;
	int pitch		= READ_BYTE();
	char *sample	= READ_STRING();
	END_READ();
	cl_entity_t *ent = gEngfuncs.GetEntityByIndex(entindex);
	if (ent)
		gEngfuncs.pEventAPI->EV_PlaySound(entindex, ent->origin, CHAN_VOICE, sample, volume, ATTN_NORM, 0, pitch);
	return 1;
}

int __MsgFunc_PickedEnt(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	Vector end;
	int ei = READ_SHORT();
	end.x = READ_COORD();
	end.y = READ_COORD();
	end.z = READ_COORD();
	END_READ();
	CL_EntitySelected(ei, end);
	return 1;
}

void CL_RegisterMessages(void)
{
	HOOK_MESSAGE(InitHUD);
	HOOK_MESSAGE(ResetHUD);
	HOOK_MESSAGE(SetFOV);
	HOOK_MESSAGE(ViewMode);
	HOOK_MESSAGE(GameMode);
	HOOK_MESSAGE(GRInfo);// XDM3035
	HOOK_MESSAGE(GREvent);// XDM3035
	HOOK_MESSAGE(TeamNames);
	HOOK_MESSAGE(MOTD);
	HOOK_MESSAGE(ShowMenu);
	HOOK_MESSAGE(ServerName);
	HOOK_MESSAGE(ScoreInfo);
	HOOK_MESSAGE(TeamScore);
	HOOK_MESSAGE(TeamInfo);
	HOOK_MESSAGE(Spectator);
	HOOK_MESSAGE(AllowSpec);
	HOOK_MESSAGE(PlayerStats);
	HOOK_MESSAGE(SpeakSnd);
	HOOK_MESSAGE(PickedEnt);

	HookFXMessages();
}
