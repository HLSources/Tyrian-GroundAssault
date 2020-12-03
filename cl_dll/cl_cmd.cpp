#include "hud.h"
#include "cl_util.h"
#include "vgui_XDMViewport.h"
#include "vgui_scorepanel.h"
#include "RenderManager.h"
#include "in_defs.h"
#include "event_api.h"
#include "cl_fx.h"


void __CmdFunc_MapBriefing(void)// XDM
{
	if (gViewPort)
		gViewPort->ShowMenu(MENU_MAPBRIEFING);
}

void __CmdFunc_MapInfo(void)// XDM
{
	if (gViewPort)
		gViewPort->ShowMenu(MENU_MAPINFO);
}

void __CmdFunc_OpenCommandMenu(void)
{
	if (gViewPort)
		gViewPort->ShowCommandMenu(gViewPort->m_StandardMenu);
}

void __CmdFunc_CloseCommandMenu(void)
{
	if (gViewPort)
		gViewPort->InputSignalHideCommandMenu();
}

void __CmdFunc_ForceCloseCommandMenu(void)
{
	if (gViewPort)
		gViewPort->HideCommandMenu();
}

void __CmdFunc_ToggleServerBrowser(void)
{
	if (gViewPort)
		gViewPort->ToggleServerBrowser();
}

void __CmdFunc_ToggleMusicPlayer(void)// XDM
{
	if (gViewPort)
		gViewPort->ToggleMusicPlayer();
}

void __CmdFunc_ShowMOTD(void)// XDM
{
	if (gViewPort)
		gViewPort->ShowMenu(MENU_INTRO);
}

void __CmdFunc_VoiceTweak(void)// XDM
{
	if (gViewPort)
		gViewPort->ShowMenu(MENU_VOICETWEAK);
}

void __CmdFunc_VGUIMenu(void)
{
	if (gViewPort)
		gViewPort->ShowMenu(atoi(CMD_ARGV(1)));
}

void __CmdFunc_ChooseTeam(void)
{
	if (gViewPort)
		gViewPort->ShowMenu(MENU_TEAM);
}

void __CmdFunc_Dir(void)
{
	UTIL_ListFiles(CMD_ARGV(1));
}

void __CmdFunc_UI_ReloadScheme(void)
{
	if (gViewPort)
	{
		gViewPort->GetSchemeManager()->LoadScheme();
		gViewPort->LoadScheme();
	}
}

void __CmdFunc_UI_BuildMode(void)
{
	if (gViewPort)
	{
		App::getInstance()->enableBuildMode();
	}
}

void __CmdFunc_ToggleSelectionMode(void)
{
	if (g_pCvarDeveloper && g_pCvarDeveloper->value <= 0.0f)
		return;

	if (CMD_ARGC() > 1)
	{
		int newmode = atoi(CMD_ARGV(1));
		if (newmode == g_iMouseManipulationMode)// XDM3037: toggle
			g_iMouseManipulationMode = 0;
		else
			g_iMouseManipulationMode = newmode;
	}
	else
	{
		if (g_iMouseManipulationMode == 0)
			g_iMouseManipulationMode = 1;
		else // if (g_iMouseManipulationMode == 1)
			g_iMouseManipulationMode = 0;
	}
	gViewPort->UpdateCursorState();// !
}

void __CmdFunc_SetTmp(void)// XDM3034: will this be useful...ever?
{
/*	cvar_t *pv = NULL:
	pv = CVAR_GET_POINTER(CMD_ARGV(1));
	if (pv)
		pv->string = ALLOCATE BUFFER??? g_pCvarTMP->string;*/
	char cmd[256];
	_snprintf(cmd, 255, "%s \"%s\"\0", CMD_ARGV(1), g_pCvarTmp->string);
	CLIENT_COMMAND(cmd);
}

void __CmdFunc_CVarCpy(void)
{
	if (CMD_ARGC() == 2)
	{
#ifdef CLDLL_NEWFUNCTIONS
		gEngfuncs.Cvar_Set(CMD_ARGV(1), CVAR_GET_STRING(CMD_ARGV(2)));
#else
		CVAR_SET_FLOAT(CMD_ARGV(1), CVAR_GET_FLOAT(CMD_ARGV(2)));
#endif
	}
	else
		CON_PRINTF("usage: %s <destination> <source>\n", CMD_ARGV(0));
}

void __CmdFunc_DBG_SetFog(void)
{
	if (g_pCvarDeveloper->value > 0.0)
	{
		if (gHUD.m_iGameType == GT_SINGLE)
		{
			if (CMD_ARGC() < 5)
				CON_PRINTF("usage: %s r g b StartDist EndDist\n", CMD_ARGV(0));
			else
			{
				if (atoi(CMD_ARGV(5)) == 0)
				{
					ResetFog();
					gHUD.m_iFogMode = 0;
				}
				else
				{
					RenderFog(atoi(CMD_ARGV(1)),atoi(CMD_ARGV(2)),atoi(CMD_ARGV(3)), atof(CMD_ARGV(4)), atof(CMD_ARGV(5)), false);
					gHUD.m_iFogMode = 1;
				}
			}
		}
		else
			CON_PRINTF("command not allowed\n");
	}
}

void __CmdFunc_DBG_ApplyForce(void)
{
	if (g_pRenderManager)
		g_pRenderManager->ApplyForce(gHUD.m_vecOrigin, Vector(0,128,0), 256.0, atoi(CMD_ARGV(1))>0?true:false);
}

void __CmdFunc_DBG_DumpSystems(void)
{
	if (g_pRenderManager)
		g_pRenderManager->DumpSystems();
}

void __CmdFunc_DBG_DumpScoreBoard(void)
{
	if (gViewPort && gViewPort->GetScoreBoard())
		gViewPort->GetScoreBoard()->DumpInfo();
}
/*
void __CmdFunc_DBG_PlaySound(void)
{
	PlaySound(CMD_ARGV(1), VOL_NORM);
}
*/
void __CmdFunc_DBG_SaySound(void)
{
	gEngfuncs.pEventAPI->EV_PlaySound(gEngfuncs.GetLocalPlayer()->index, gHUD.m_LocalPlayerState.origin, CHAN_VOICE, CMD_ARGV(1), VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
}


void CL_RegisterCommands(void)
{
	HOOK_COMMAND("+commandmenu", OpenCommandMenu);
	HOOK_COMMAND("-commandmenu", CloseCommandMenu);
	HOOK_COMMAND("ForceCloseCommandMenu", ForceCloseCommandMenu);
	HOOK_COMMAND("togglebrowser", ToggleServerBrowser);
	HOOK_COMMAND("togglemplayer", ToggleMusicPlayer);// XDM
	HOOK_COMMAND("showmotd", ShowMOTD);
	HOOK_COMMAND("showmapbriefing", MapBriefing);// XDM3035: autocompletion made me go mad
	HOOK_COMMAND("showmapinfo", MapInfo);
	HOOK_COMMAND("voicetweak", VoiceTweak);
	HOOK_COMMAND("vguimenu", VGUIMenu);
	HOOK_COMMAND("chooseteam", ChooseTeam);
	HOOK_COMMAND("dir", Dir);
	HOOK_COMMAND("ls", Dir);
	HOOK_COMMAND("ui_reloadscheme", UI_ReloadScheme);
//bad	HOOK_COMMAND("ui_buildmode", UI_BuildMode);
	HOOK_COMMAND("toggleselmode", ToggleSelectionMode);
	HOOK_COMMAND("settmp", SetTmp);
	HOOK_COMMAND("cvarcpy", CVarCpy);// copy arg2 to arg1
#ifdef _DEBUG
	HOOK_COMMAND("dbg_setfog", DBG_SetFog);
	HOOK_COMMAND("dbg_appforce", DBG_ApplyForce);
	HOOK_COMMAND("dbg_dumpsb", DBG_DumpScoreBoard);
//	HOOK_COMMAND("dbg_playsnd", DBG_PlaySound);
	HOOK_COMMAND("dbg_saysnd", DBG_SaySound);
#endif
	HOOK_COMMAND("dbg_dumprs", DBG_DumpSystems);
}
