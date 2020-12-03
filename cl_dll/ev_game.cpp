#include "hud.h"
#include "cl_util.h"
#include "vgui_XDMViewport.h"
#include "eventscripts.h"
#include "event_api.h"
#include "r_efx.h"
#include "cl_fx.h"
#include "shared_resources.h"
#include "screenfade.h"
#include "shake.h"
#include "gamedefs.h"
#include "pm_shared.h"

#include "RenderManager.h"
#include "RenderSystem.h"
#include "RSCylinder.h"
#include "RSLight.h"
#include "RSModel.h"
#include "ParticleSystem.h"
#include "PSSparks.h"
#include "PSSpawnEffect.h"

extern "C"
{
void EV_CaptureObject(struct event_args_s *args);
void EV_DomPoint(struct event_args_s *args);
void EV_Teleport(struct event_args_s *args);
void EV_PM_Fall(struct event_args_s *args);
void EV_TrainPitchAdjust(struct event_args_s *args);
void EV_ZoomCrosshair(struct event_args_s *args);
}


float flagpulse(float time)
{
	return (0.75f + 0.25f*sinf(time*8.0f));
}

static char g_szCaptureMessage[64];
static client_textmessage_t g_CaptureMessage;
//-----------------------------------------------------------------------------
// Called everytime the flag state changes
//-----------------------------------------------------------------------------
void EV_CaptureObject(struct event_args_s *args)
{
	int idx = args->iparam1;// flag entity index
	int event = args->iparam2;// CTF_EV_TAKEN
	int team = args->bparam1;// flag team

//	int r,g,b;
//	GetTeamColor(team, r,g,b);

	if (idx > 1)
	{
		CRSLight *pLight = NULL;
		pLight = (CRSLight *)g_pRenderManager->FindSystemByFollowEntity(idx);
		if (pLight != NULL)// found, update
		{
			VectorCopy(args->origin, pLight->m_vecOrigin);

				if (event == CTF_EV_TAKEN)
					pLight->RadiusCallback = flagpulse;
				else
					pLight->RadiusCallback = NULL;
		}
	}
//	CON_PRINTF(" EV_CaptureObject: entindex %d event %d team %d\n", args->entindex, event, team);

	client_textmessage_t *msg = NULL;
	//if (args->entindex > 0 && args->entindex <= MAX_PLAYERS)
	if (EV_IsPlayer(args->entindex))
	{
		if (event == CTF_EV_DROP)
		{
			msg = TextMessageGet("CTF_DROPPED");
		}
		else if (event == CTF_EV_RETURN)
		{
			msg = TextMessageGet("CTF_RETURNED");
//			PlaySound("!CTF_RET_PLR", VOL_NORM);
			if (team == gHUD.m_iTeamNumber)
				PlaySound("!CTF_RET_TEAM", VOL_NORM);
			else
				PlaySound("!CTF_RET_ENEMY", VOL_NORM);

			gHUD.m_flNextAnnounceTime = gHUD.m_flTime + 3.0f;
		}
		else if (event == CTF_EV_CAPTURED)
		{
			if (team == gHUD.m_iTeamNumber)// my flag was captured, aww... :(
			{
				PlaySound("!CTF_CAP_ENEMY", VOL_NORM);
			}
			else
			{
				char sentence[16];
				sprintf(sentence, "!SCORESOUND%d\0", RANDOM_LONG(0,3));// HACK: hardcoded because the system only plays sentences by name, no randomization
				PlaySound(sentence, VOL_NORM);
//				PlaySound("game/ctf_captured.wav", VOL_NORM);
//				EMIT_SOUND(args->entindex, args->origin, CHAN_STATIC, "game/ctf_captured.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
			}
			if (g_pCvarTFX->value > 0.0f)
			{
				if (g_pRenderManager)// (args->iparam2 > 0?args->iparam2:g_iModelIndexAnimglow01)
				{
					byte r,g,b;
					GetTeamColor(team, r,g,b);
					g_pRenderManager->AddSystem(new CPSSparks(96, args->origin, 1.0f,0.8f,-0.5f, -80.0f, 2.2f, r,g,b,1.0f,-1.0f, g_iModelIndexAnimglow01, kRenderTransAdd, 1.0), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_NOCLIP);
				}
			}

			gHUD.m_flNextAnnounceTime = gHUD.m_flTime + 3.0f;
			msg = TextMessageGet("CTF_CAPTURED");
		}
		else if (event == CTF_EV_TAKEN)
		{
			if (team == gHUD.m_iTeamNumber)// MY flag has been taken! Loud alarm for all teammates!
			{
				PlaySound("!CTF_GOT_ENEMY", VOL_NORM);
				//PlaySound("game/ctf_alarm.wav", VOL_NORM);
			}
			else
			{
//				if (args->entindex == localplayer)
				if (EV_IsLocal(args->entindex))
				{
//					PlaySound("!CTF_GOT_PLR", VOL_NORM);
					CON_PRINTF("You've got the flag!\n");
				}
				else
				{
					PlaySound("!CTF_GOT_TEAM", VOL_NORM);
				}
				EMIT_SOUND(args->entindex, args->origin, CHAN_STATIC, "game/ctf_alarm.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
			}

			msg = TextMessageGet("CTF_TAKEN");
		}

		if (msg)
			sprintf(g_szCaptureMessage, msg->pMessage, g_PlayerInfoList[args->entindex].name, gViewPort->GetTeamName(team));
	}
	else
	{
		if (event == CTF_EV_RETURN)
		{
			msg = TextMessageGet("CTF_RET_SELF");

			if (msg)
				sprintf(g_szCaptureMessage, msg->pMessage, gViewPort->GetTeamName(team));
		}
	}

//	ConsolePrint("* ");
//	ConsolePrint(g_szCaptureMessage);// XDM3035a
//	ConsolePrint("\n");
	CON_PRINTF("* %s\n", g_szCaptureMessage);
	if (msg)
	{
		memcpy(&g_CaptureMessage, msg, sizeof(client_textmessage_t));
//		g_CaptureMessage = *msg;// copy localized message
		g_CaptureMessage.x = -1;// override some parameters
//		g_CaptureMessage.y = 0.9;
		g_CaptureMessage.a1 = 255;
		msg->holdtime = 3.0;
		g_CaptureMessage.pName = "CTF_MSG";
		g_CaptureMessage.pMessage = g_szCaptureMessage;
		gHUD.m_Message.MessageAdd(&g_CaptureMessage);
	}

	if (&gHUD.m_FlagDisplay)
		gHUD.m_FlagDisplay.SetEntState(idx, idx, event);
}

//-----------------------------------------------------------------------------
// Called when a player (entindex) touches a domination entity
//-----------------------------------------------------------------------------
void EV_DomPoint(struct event_args_s *args)
{
	// others must hear this as well PlaySound("game/dom_touch.wav", VOL_NORM);
	EMIT_SOUND(args->entindex, args->origin, CHAN_STATIC, "game/dom_touch.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
	if (args->iparam2 > 1)// entindex
	{
		int r,g,b;
		GetTeamColor(args->iparam1, r,g,b);

/* :(		cl_entity_t *ent = gEngfuncs.GetEntityByIndex(args->iparam2);
		if (ent != NULL)
		{
			ent->curstate.rendercolor.r = r;
			ent->curstate.rendercolor.g = g;
			ent->curstate.rendercolor.b = b;
		}*/

		CRSLight *pLight = NULL;
		pLight = (CRSLight *)g_pRenderManager->FindSystemByFollowEntity(args->iparam2);
		if (pLight != NULL)// found, update
		{
			pLight->m_color.r = r;
			pLight->m_color.g = g;
			pLight->m_color.b = b;
			pLight->m_fScale = args->fparam1;
		}
		else
		{
			if (g_pRenderManager)
				g_pRenderManager->AddSystem(new CRSLight(args->origin, r,g,b, args->fparam1, NULL, 0.0, 0.0), 0, args->iparam2, RENDERSYSTEM_FFLAG_ICNF_NODRAW);
		}
		if (&gHUD.m_DomDisplay)
			gHUD.m_DomDisplay.SetEntTeam(args->iparam2, args->iparam1);
	}
}

//-----------------------------------------------------------------------------
// Called when a player (entindex) teleports via trigger_teleport
//-----------------------------------------------------------------------------
void EV_Teleport(struct event_args_s *args)
{
//	CON_DPRINTF("EV_Teleport\n");
	if (gHUD.m_iGameType > GT_SINGLE)
	{
	//	gEngfuncs.pEfxAPI->R_ParticleBurst(args->origin, 10, 208, 0.1);
		if (args->bparam1 > 0)
			EMIT_SOUND(-1, args->origin, CHAN_STATIC, "common/teleport.wav", VOL_NORM, ATTN_LOW_HIGH, 0, PITCH_NORM);

		DynamicLight(args->origin, 128, 255,255,255, 1.0, 128);
		gEngfuncs.pEfxAPI->R_TeleportSplash(args->origin);

		if (args->bparam2 > 0 && EV_IsLocal(args->entindex))
		{
			screenfade_s sf;
			GET_SCREEN_FADE(&sf);
			sf.fadeSpeed = 128.0;
			sf.fadeEnd = gEngfuncs.GetClientTime() + 1.0f;
			sf.fader = 255;
			sf.fadeg = 255;
			sf.fadeb = 255;
			sf.fadealpha = 191;
			sf.fadeFlags = FFADE_IN;
			SET_SCREEN_FADE(&sf);
		}

	/*	if (g_pRenderManager)
		{
			g_pRenderManager->AddSystem(new CRSLight(args->origin, 255,255,255, 128, NULL, 100.0, 1.0, 0));
		}*/
	}
}

void EV_PM_Fall(struct event_args_s *args)
{
//	CON_PRINTF(" EV_PM_Fall()\n");
/*
			eargs.fparam1 = pmove->flFallVelocity;
			eargs.fparam2 = fvol;
			eargs.iparam1 = pmove->waterlevel;
			eargs.iparam2 = 0;
			eargs.bparam1 = (pmove->onground == -1)?0:1;
			eargs.bparam2 = g_onladder;
*/
// this will be zero :(	cl_entity_t *ent = gEngfuncs.GetEntityByIndex(args->entindex);
	float flFallVelocity = args->fparam1;
	if (flFallVelocity >= PLAYER_MAX_SAFE_FALL_SPEED*0.75f)
	{
		int waterlevel = args->iparam1;
		int onground = args->bparam1;
		vec3_t org = args->origin;

		if (!onground && waterlevel >= 2)
		{
			if (!UTIL_PointIsFar(args->origin, 1.0))
			{
				Vector halfbox = (VEC_HULL_MAX - VEC_HULL_MIN + Vector(0.0f,0.0f,flFallVelocity*0.05f))/2;
				FX_BubblesBox(org, halfbox, g_iModelIndexBubble, (int)floorf(flFallVelocity*0.15f), RANDOM_FLOAT(30,40));
/*				Vector mins = org + VEC_HULL_MIN - Vector(0.0f,0.0f,flFallVelocity*0.05f);
				Vector maxs = org + VEC_HULL_MAX;
//				gEngfuncs.pEfxAPI->R_ParticleBox(mins, maxs, 255,0,0, 4.0f);
				FX_Bubbles(mins, maxs, g_vecZero, g_iModelIndexBubble, (int)floorf(flFallVelocity*0.15f), RANDOM_FLOAT(10,20));
*/
	/*			float flHeight = UTIL_WaterLevel(org, org.z, org.z + 1024.0f);
				flHeight = flHeight - mins.z;
				gEngfuncs.pEfxAPI->R_Bubbles(mins, maxs, flHeight, g_iModelIndexBubble, (int)floorf(flFallVelocity*0.1f), flFallVelocity*0.2f);
	*/
		//UTIL_Bubbles(pev->origin + pev->mins - Vector(0.0f,0.0f,m_flFallVelocity*0.04f), pev->origin + pev->maxs, floor(m_flFallVelocity*0.1f));
			}
			float vel = flFallVelocity*0.4f;
//			gEngfuncs.pEfxAPI->R_StreakSplash(org+Vector(0,0,6), Vector(0,0,2), 0, 40, 20, -(int)vel, (int)vel);
			color24 c = {127,127,127};
			FX_StreakSplash(org+Vector(0,0,6), Vector(0,0,2), c, 40, vel, true, false, false);
		//	StreakSplash(pev->origin+Vector(0,0,6), Vector(0,0,2), 7, 40, 20, floor(m_flFallVelocity*0.4f));
		}
	}
}

//-----------------------------------------------------------------------------
// Train
//-----------------------------------------------------------------------------
#define SND_CHANGE_PITCH	(1<<7)		// duplicated in protocol.h change sound pitch

void EV_TrainPitchAdjust(event_args_t *args)
{
	CON_DPRINTF("EV_TrainPitchAdjust was called! This should not happen!\n");
	int idx;
	vec3_t origin;
	unsigned short us_params;
	int noise;
	int pitch;
	int stop;
	float m_flVolume;
	char sz[24];

	idx = args->entindex;

	VectorCopy(args->origin, origin);

	us_params = (unsigned short)args->iparam1;
	stop	  = args->bparam1;

	m_flVolume	= (float)(us_params & 0x003f)/40.0f;
	noise		= (int)(((us_params) >> 12) & 0x0007);
	pitch		= (int)(10.0 * (float)((us_params >> 6) & 0x003f));

	switch (noise)
	{
	case 1: strcpy(sz, "plats/ttrain1.wav"); break;
	case 2: strcpy(sz, "plats/ttrain2.wav"); break;
	case 3: strcpy(sz, "plats/ttrain3.wav"); break; 
	case 4: strcpy(sz, "plats/ttrain4.wav"); break;
	case 5: strcpy(sz, "plats/ttrain6.wav"); break;
	case 6: strcpy(sz, "plats/ttrain7.wav"); break;
	default:
		// no sound
		strcpy(sz, "");
		return;
	}

	if (stop)
		gEngfuncs.pEventAPI->EV_StopSound(idx, CHAN_STATIC, sz);
	else
		EMIT_SOUND(idx, origin, CHAN_STATIC, sz, m_flVolume, ATTN_NORM, SND_CHANGE_PITCH, pitch);
}

void EV_ZoomCrosshair(struct event_args_s *args)
{
	qboolean local = EV_IsLocal(args->entindex);
	if (local)
		gHUD.m_ZoomCrosshair.SetParams(args->iparam1, (args->bparam1>0)?args->bparam2:-1, args->fparam1);

	if (local == false || args->bparam1 == 0)// 'zoom in' sound is played in gHUD.m_ZoomCrosshair (for local player only)
		EMIT_SOUND(args->entindex, args->origin, CHAN_WEAPON, "weapons/zoom.wav", VOL_NORM, ATTN_IDLE, 0, (args->bparam1 == 0)?90:PITCH_NORM);
}