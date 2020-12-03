#include "hud.h"
#include "cl_util.h"
//#include "camera.h"
#include "in_defs.h"
#include "event_api.h"
#include "pm_defs.h"
#include "pmtrace.h"
#include "com_model.h"
#include "triangleapi.h"
#include "r_studioint.h"
#include "r_efx.h"
#include "shared_resources.h"
//#include "cl_fx.h"
#include "vgui_XDMViewport.h"
#include "vgui_EntityEntryPanel.h"
#include "RenderManager.h"
#include "RenderSystem.h"
#include "RSModel.h"
#include "msg_fx.h"

extern int mouse_x, mouse_y;
//extern vec3_t g_vecViewForward;

//const float cam_offset[3] = {0.0f,0.0f,64.0f};// XDM: 3034 ?
const vec3_t cam_offset(0.0f,0.0f,64.0f);

extern "C" 
{
	void EXPORT CAM_Think(void);
	void EXPORT CL_CameraOffset(float *ofs);
	int EXPORT CL_IsThirdPerson(void);
}

//-----------------------------------------------------------------------------
// Purpose: this code is obsolete. XDM uses V_CalcThirdPersonRefdef
//-----------------------------------------------------------------------------
void EXPORT CAM_Think(void)
{
}

//-----------------------------------------------------------------------------
// Purpose: ???
// Input  : *ofs - 
//-----------------------------------------------------------------------------
void EXPORT CL_CameraOffset(float *ofs)
{
	VectorCopy(cam_offset, ofs);
}

//-----------------------------------------------------------------------------
// Purpose: Tells the engine that current view is not in first person mode
// Output : int 1 true 0 false
//-----------------------------------------------------------------------------
int EXPORT CL_IsThirdPerson(void)
{
	if (g_ThirdPersonView)// XDM: this tells if we are REALLY TECHNICALLY watching in 3rd person
		return 1;
/*
	if (gHUD.m_iCameraMode > 0)
		return gHUD.m_iCameraMode;

	if (gHUD.m_pCvarDeathView->value && CL_IsDead())
		return 1;

	if (g_iUser1 && (g_iUser2 == gEngfuncs.GetLocalPlayer()->index))
		return 1;
*/
	return 0;
}

byte CL_IsThirdPersonAlloed(void)
{
	if (gHUD.m_iGameType == GT_SINGLE || (gHUD.m_iGameFlags & GAME_FLAG_ALLOW_CAMERA))//gHUD.m_pCvarDeveloper->value > 0.0f)
		return 1;

	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: Trace mous click into real world
// Input  : &mouse_x &mouse_y - mouse coordinates
//			traceFlags - PM_STUDIO_BOX|PM_WORLD_ONLY
//			*ptrace - any existing pmtrace_t, will be modified!
// Output : Returns true on hit in empty space, false otherwise.
//-----------------------------------------------------------------------------
bool TraceClick(int &mouse_x, int &mouse_y, int traceFlags, pmtrace_t *ptrace, Vector *tracestart = NULL)
{
	if (ptrace)
	{
		memset(ptrace, 0, sizeof(pmtrace_t));
		vec3_t screen, src, end;
		screen[0] = XUNPROJECT((float)mouse_x);
		screen[1] = -YUNPROJECT((float)mouse_y);
		screen[2] = 0.0f;
		gEngfuncs.pTriAPI->ScreenToWorld(screen, src);// WARNING! This doesn't work properly when minimap or spectator windows are active!
		screen[2] = 1.0f;
		gEngfuncs.pTriAPI->ScreenToWorld(screen, end);// end goes into infinity
		gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, false);// skip local client in THIS case only!
		gEngfuncs.pEventAPI->EV_PushPMStates();
		gEngfuncs.pEventAPI->EV_SetSolidPlayers(-1);
		gEngfuncs.pEventAPI->EV_SetTraceHull(2);
//		pmtrace_t pmtrace;
//		pmtrace_t *ptrace = &pmtrace;

		// find local player's physent to ignore
		int pe_ignore = GetPhysent(gEngfuncs.GetLocalPlayer()->index);

		ASSERT(pe_ignore > 0);

		gEngfuncs.pEfxAPI->R_BeamPoints(src, end, g_iModelIndexBeamsAll, 2.0f, 0.25, 0.0f, 1.0f, 10.0, BLAST_SKIN_SHOCKWAVE, 0, 1,0,0);

		gEngfuncs.pEventAPI->EV_PlayerTrace(src.As3f(), end.As3f(), traceFlags, pe_ignore, ptrace);// 1st, this can hit players
//		ptrace = gEngfuncs.PM_TraceLine(src, end, PM_TRACELINE_PHYSENTSONLY, 2, -1);
		gEngfuncs.pEventAPI->EV_PopPMStates();

		if (tracestart)
			*tracestart = src;

		if (ptrace->inopen && ptrace->fraction < 1.0f)
			return true;
	}
	return false;
}



int g_iMouseManipulationMode = MMM_NONE;
int g_iMouseLastEvent = 0;
unsigned int g_iEntityCreationRS = 0;
cl_entity_t *g_pPickedEntity = NULL;// TODO: WARNING! This using this pointer is very risky! It may become invalid if the entity gets removed (or the player disconnected)
//cl_entity_t *g_pUsedEntity = NULL;
int g_iUsedEntity = 0;

// Test: converts to [0...1] range
//#define XUNPROJECT2(x)	(0.5 + (x/ScreenWidth))
//#define YUNPROJECT2(y)	(0.5 + (y/ScreenHeight))

//-----------------------------------------------------------------------------
// Purpose: Mouse event
// Input  : &button - which button (starting from 0)
//			state - pressed or released
//-----------------------------------------------------------------------------
void CL_MouseEvent(const int &button, byte state)
{
//	CON_PRINTF("CL_MouseEvent(%d %d)\n", button, state);
	char str[128];
	str[0] = 0;

	if (g_iMouseManipulationMode != MMM_NONE && gHUD.m_Spectator.ShouldDrawOverview())
	{
		sprintf(str, "* Disable all inset views!\n(minimap, overview, spectator, etc.)\n");
		g_iMouseManipulationMode = MMM_NONE;
//		return;
	}

//	if (g_pRefParams->viewport ...?)// do something?

	if (g_iMouseManipulationMode == MMM_PICK)
	{
		if (button == 0)// MOUSE1: pick
		{
			if (state == 1)// pressed //UNDONE: event should be triggered when the button is released
			{
				vec3_t screen, src, end;
				screen[0] = XUNPROJECT((float)mouse_x);
				screen[1] = -YUNPROJECT((float)mouse_y);
				screen[2] = 0.0f;
				gEngfuncs.pTriAPI->ScreenToWorld(screen, src);
				screen[2] = 1.0f;
				gEngfuncs.pTriAPI->ScreenToWorld(screen, end);// end goes into infinity
				char scmd[128];
				sprintf(scmd, ".p %g %g %g %g %g %g\0", src[0], src[1], src[2], end[0], end[1], end[2]);
				SERVER_COMMAND(scmd);
			}
		}
		else if (button == 1)// MOUSE2: move
		{
			if (g_pPickedEntity)
			{
				if (state == 1)// pressed // same here
				{
					pmtrace_t pmtrace;
					pmtrace_t *ptrace = &pmtrace;
					Vector src;
					if (TraceClick(mouse_x, mouse_y, PM_STUDIO_BOX|PM_WORLD_ONLY, &pmtrace, &src))
					{
						gEngfuncs.pEfxAPI->R_BeamPoints(src, pmtrace.endpos, g_iModelIndexBeamsAll, 2.0f, 0.25, 0.0f, 1.0f, 10.0, BLAST_SKIN_SHOCKWAVE, 0, 0,0,1);

						sprintf(str, "* Moving entity %d to %f %f %f\n", g_pPickedEntity->index, ptrace->endpos.x, ptrace->endpos.y, ptrace->endpos.z);
						//TODO: sprintf(scmd, ".m %g %g %g %g %g %g\0", ptrace->endpos[0], ptrace->endpos[1], ptrace->endpos[2], ang[0], ang[1], ang[2]);
						sprintf(str, "setkvbyindex %d \"origin\" \"%g %g %g\"", g_pPickedEntity->index, ptrace->endpos.x, ptrace->endpos.y, ptrace->endpos.z);
						SERVER_COMMAND(str);
					}
				}
				else
					str[0] = 0;
			}
			else
				sprintf(str, "* Nothing is selected\n");
		}
	}
	else if (g_iMouseManipulationMode == MMM_USE)// +USE
	{
		if (button == 0)// MOUSE1
		{
			if (state == 1)// pressed
			{
				if (g_iUsedEntity == 0)
				{
					pmtrace_t pmtrace;// trace new location
					Vector src;
					if (TraceClick(mouse_x, mouse_y, PM_STUDIO_BOX, &pmtrace, &src))// PM_WORLD_ONLY ignores func_walls, but otherwise trace hits the player himself
					{
						if (pmtrace.ent)
						{
							g_iUsedEntity = gEngfuncs.pEventAPI->EV_IndexFromTrace(&pmtrace);
							sprintf(str, ".u %d %d %g", g_iUsedEntity, state, 1);// TODO: continuous use?
							SERVER_COMMAND(str);
							str[0] = 0;
//							sprintf(str, "* Using entity %d\n", g_iUsedEntity);
						}
					}
				}
//				else
//					sprintf(str, "* Already using an entity\n");
			}
			else if (state == 0)// released
			{
				if (g_iUsedEntity)
				{
					sprintf(str, ".u %d %d %g", g_iUsedEntity, state, 0);
					SERVER_COMMAND(str);
					str[0] = 0;
//					sprintf(str, "* Unusing entity %d\n", g_iUsedEntity);
					g_iUsedEntity = 0;
				}
			}
		}
	}
	else if (g_iMouseManipulationMode == MMM_MOVE)
	{
		sprintf(str, "* Use right mouse button in selection mode to move objects\n");
		// same as moving by right-click?
	}
	else if (g_iMouseManipulationMode == MMM_EDIT)
	{
		if (g_pPickedEntity)
		{
			if (state == 1)// pressed // same here
			{
				sprintf(str, "* TODO: make a toolbar, make entity edit dialog, catch server callback\n");
			}
		}
		else
			sprintf(str, "* Nothing is selected\n");
	}
	else if (g_iMouseManipulationMode == MMM_CREATE)
	{
		if (state == 1)// pressed // same here
		{
			if (g_iEntityCreationRS != 0)// check for old/bogus/invalid entity
			{
				if (g_pRenderManager->FindSystem(g_iEntityCreationRS) == NULL)
					g_iEntityCreationRS = 0;
			}
			if (g_iEntityCreationRS == 0)// if not in the process
			{
				pmtrace_t pmtrace;// trace new location
				Vector src;
				if (TraceClick(mouse_x, mouse_y, PM_STUDIO_IGNORE|PM_STUDIO_BOX|PM_WORLD_ONLY, &pmtrace, &src))// PM_WORLD_ONLY ignores func_walls, but otherwise trace hits the player himself
				{
					gEngfuncs.pEfxAPI->R_BeamPoints(src, pmtrace.endpos, g_iModelIndexBeamsAll, 2.0f, 0.25, 0.0f, 1.0f, 10.0, BLAST_SKIN_SHOCKWAVE, 0, 0,0,1);
					gEngfuncs.pEfxAPI->R_TempSprite(pmtrace.endpos, (float *)g_vecZero, 0.5f, g_iModelIndexAnimSpr10, kRenderTransAdd, kRenderFxNone, 1.0f, 2.0f, FTENT_FADEOUT);

					int modelindex = LoadModel("models/projectiles.mdl");
					ASSERT(modelindex > 0);

//					Vector ang = gHUD.m_vecAngles;
//					ang[0] = -ang[0];
					Vector ang;// = UTIL_VecToAngles(pmtrace.plane.normal);
					VectorAngles(pmtrace.plane.normal, ang.As3f());
					ang.x += 270.0f;
					g_iEntityCreationRS = g_pRenderManager->AddSystem(new CRSModel(pmtrace.endpos + pmtrace.plane.normal*4, ang, g_vecZero, 0, modelindex, 0, 0, 0, kRenderTransTexture, kRenderFxStrobeFast, 191,191,255, 0.75f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f), RENDERSYSTEM_FLAG_NOCLIP);
					if (g_iEntityCreationRS != 0)
						gViewPort->ShowMenu(new CEntityEntryPanel(-1,-1, XRES(EEP_WIDTH), YRES(EEP_HEIGHT)));

					g_iMouseManipulationMode = MMM_NONE;// WARNING! Disallow clicking!
				}
				else
					sprintf(str, "* Unreachable location\n");
			}
		}
	}

	if (str[0] != 0)
	{
		CenterPrint(str);
		ConsolePrint(str);
	}
}

//-----------------------------------------------------------------------------
// Purpose: A reply from server arrives: entity picked
// Input  : entindex - 
//			&hitpoint - 
//-----------------------------------------------------------------------------
void CL_EntitySelected(int entindex, vec3_t &hitpoint)
{
	if (entindex <= 0)
	{
//?		g_pPickedEntity = NULL;
		return;
	}

	cl_entity_t *pEntity = gEngfuncs.GetEntityByIndex(entindex);
	if (pEntity)
	{
		gEngfuncs.pEfxAPI->R_TempSprite(hitpoint, (float *)g_vecZero, 0.1f, g_iModelIndexAnimSpr10, kRenderTransAdd, kRenderFxNone, 1.0f, 2.0f, FTENT_FADEOUT);
		char str[128];

//		if (button == 0)// pick
		{
			if (pEntity == g_pPickedEntity)
			{
				sprintf(str, "* Released %d\n", g_pPickedEntity->index);
				g_pPickedEntity = NULL;
			}
			else
			{
				g_pPickedEntity = pEntity;
				char *str2 = NULL;

				// EV_GetPhysent returns NULL in multiplayer
//				physent_t *pe = gEngfuncs.pEventAPI->EV_GetPhysent(ptrace->ent);
//				if (pe)
//					str2 = pe->name;
//				else
				if (IsActivePlayer(pEntity))
					str2 = g_PlayerInfoList[g_iUser2].name;
				else if (pEntity->model)
					str2 = pEntity->model->name;
				else
					str2 = "no info";

				sprintf(str, "* Picked entity %d (%s) at (%g %g %g)\n", pEntity->index, str2, pEntity->origin[0], pEntity->origin[1], pEntity->origin[2]);
			}
		}

		CenterPrint(str);
		ConsolePrint(str);
	}
}