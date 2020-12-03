//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

// Client side entity management functions
//#include <memory.h>
#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "entity_types.h"
#include "studio_event.h" // def. of mstudioevent_t
#include "event_api.h"
#include "r_efx.h"
#include "pmtrace.h"
#include "pm_defs.h"
#include "pm_shared.h"
#include "pm_materials.h"
#include "com_model.h"// XDM
#include "r_studioint.h"
#include "studio_util.h"
#include "RenderManager.h"
#include "shared_resources.h"
#include "cl_fx.h"
#include "weapondef.h"
#include "..\game_shared\voice_status.h"
#include "msg_fx.h"
#include "decals.h"
#include "RenderManager.h"
#include "RenderSystem.h"
#include "ParticleSystem.h"
#include "PSBlastCone.h"
#include "RSSprite.h"
#include "eventscripts.h"

extern "C"
{
	int DLLEXPORT HUD_AddEntity( int type, struct cl_entity_s *ent, const char *modelname );
	void DLLEXPORT HUD_CreateEntities( void );
	void DLLEXPORT HUD_StudioEvent( const struct mstudioevent_s *event, const struct cl_entity_s *entity );
	void DLLEXPORT HUD_TxferLocalOverrides( struct entity_state_s *state, const struct clientdata_s *client );
	void DLLEXPORT HUD_ProcessPlayerState( struct entity_state_s *dst, const struct entity_state_s *src );
	void DLLEXPORT HUD_TxferPredictionData ( struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd );
	void DLLEXPORT HUD_TempEntUpdate( double frametime, double client_time, double cl_gravity, struct tempent_s **ppTempEntFree, struct tempent_s **ppTempEntActive, int ( *Callback_AddVisibleEntity )( struct cl_entity_s *pEntity ), void ( *Callback_TempEntPlaySound )( struct tempent_s *pTemp, float damp ) );
	struct cl_entity_s DLLEXPORT *HUD_GetUserEntity( int index );
	int EXPORT CL_IsThirdPerson(void);
}


int g_iAlive = 1;
int g_iUser1;
int g_iUser2;
int g_iUser3;
double g_cl_gravity;// XDM3035: value is same as sv_gravity (800)


/*
========================
HUD_AddEntity
	Return 0 to filter entity from visible list for rendering

	type == ET_NORMAL, etc.

	Local player won't be processed here in the first person view.
========================
*/
int DLLEXPORT HUD_AddEntity( int type, struct cl_entity_s *ent, const char *modelname )
{
	// each frame every entity passes this function, so the overview hooks it to filter the overview entities in spectator mode:
	if (gHUD.m_Spectator.ShouldDrawOverview())
	{
		gHUD.m_Spectator.AddOverviewEntity(ent, /*ent->curstate.iuser4 > 0?IEngineStudio.GetModelByIndex(ent->curstate.iuser4):*/NULL, -1);// XDM3035c: iuser4 is now icon sprite index
		if ((g_iUser1 == OBS_IN_EYE || gHUD.m_Spectator.m_iInsetMode == INSET_IN_EYE ) && ent->index == g_iUser2)
			return 0;	// don't draw the player we are following in eye
	}

	if (gHUD.m_iPaused == 0)// XDM: a chace to draw additional entity effects
	{

		if (type == ET_BEAM)//(ent->curstate.entityType == ENTITY_BEAM)
		{
			if (ent->curstate.team > 0 && ent->curstate.renderamt > 127)
			{
				int ei = (ent->curstate.skin & 0xFFF);// from CBeam::GetEndEntity
				cl_entity_t *pEnd = gEngfuncs.GetEntityByIndex(ei);
				Vector d;
				float dl = 0;
				if (pEnd)
				{
					d = pEnd->origin - ent->curstate.origin;
					dl = d.Length();
				}
	//			gEngfuncs.pEfxAPI->R_SparkEffect(ent->curstate.origin + RANDOM_FLOAT(0,1)*d, 6, -180, 180);
	//			gEngfuncs.pEfxAPI->R_ShowLine(ent->curstate.origin, pEnd->origin);//ent->curstate.angles);

				if (ent->curstate.team == 1)// gluon primary
				{
					if (pEnd && RANDOM_FLOAT(0,1600) < dl)// limit number of sparks at close distances
						FX_StreakSplash(ent->curstate.origin + RANDOM_FLOAT(0,1)*d, g_vecZero, ent->curstate.rendercolor, 4, 200.0f, true, true, false);
					if (ent->curstate.playerclass > 0)// hit
						gEngfuncs.pEfxAPI->R_StreakSplash(ent->curstate.origin, (float *)&g_vecZero, 7, 16, 32, -240, 240);// "r_efx.h"
				}
				else if (ent->curstate.team == 2)
				{
					if (pEnd && RANDOM_FLOAT(0,1600) < dl)
						FX_StreakSplash(ent->curstate.origin + RANDOM_FLOAT(0,1)*d, g_vecZero, ent->curstate.rendercolor, 6, 200.0f, true, true, false);
					if (ent->curstate.playerclass > 0)// hit
						gEngfuncs.pEfxAPI->R_StreakSplash(ent->curstate.origin, (float *)&g_vecZero, 7, 12, 40, -240, 240);
				}
				else if (ent->curstate.team == 3)// plasma primary
				{
					if (pEnd && RANDOM_FLOAT(0,1600) < dl)
						gEngfuncs.pEfxAPI->R_SparkEffect(ent->curstate.origin + RANDOM_FLOAT(0,1)*d, 4, -180, 180);// beam
					if (ent->curstate.playerclass > 0)// hit
					{
						FX_StreakSplash(ent->curstate.origin, g_vecZero, gTracerColors[0], 4, 200.0f, true, true, false);
						FX_StreakSplash(ent->curstate.origin, g_vecZero, gTracerColors[15], 3, 320.0f, true, true, false);
					}
				}
				else if (ent->curstate.team == 4)
				{
					if (pEnd && RANDOM_FLOAT(0,1600) < dl)
						gEngfuncs.pEfxAPI->R_SparkEffect(ent->curstate.origin + RANDOM_FLOAT(0,1)*d, 6, -180, 180);// beam
					if (ent->curstate.playerclass > 0)// hit
					{
						FX_StreakSplash(ent->curstate.origin, g_vecZero, gTracerColors[7], 3, 200.0f, true, true, true);
						FX_StreakSplash(ent->curstate.origin, g_vecZero, gTracerColors[17], 4, 320.0f, true, true, false);
					}
				}
			}
		}

	}// !Paused

/* TEST	if (ent->curstate.eflags & EFLAG_HIGHLIGHT)
	{
		dlight_t *dl = gEngfuncs.pEfxAPI->CL_AllocDlight(ent->index);
		if (dl)
		{
			VectorCopy(ent->origin, dl->origin);
			dl->die = gEngfuncs.GetClientTime() + 0.01f;// die on next frame
			UnpackRGB(dl->color.r, dl->color.g, dl->color.b, RGB_GREEN);
			dl->radius = g_fEntityHighlightEffectRadius;
		}
	}*/

	if (ent->player == 0)
	{
		if (type == ET_NORMAL)//(ent->curstate.entityType == ENTITY_)
		{
			if (ent->curstate.effects & EF_DIMLIGHT)// XDM3035c: use rendercolor for light
			{
				if (ent->curstate.rendercolor.r != 0 ||
					ent->curstate.rendercolor.g != 0 ||
					ent->curstate.rendercolor.b != 0)
				{
					// Engine will nullify and return existing light structure with this key
					dlight_t *dl = gEngfuncs.pEfxAPI->CL_AllocDlight(ent->index);
					if (dl)
					{
						VectorCopy(ent->origin, dl->origin);
						dl->die = gEngfuncs.GetClientTime() + 0.01f; // die on next frame
						dl->color = ent->curstate.rendercolor;
			//			dl->color.r = dl->color.g = dl->color.b = br;
						if (ent->curstate.renderamt > 0)
							dl->radius = ent->curstate.renderamt;
						else
							dl->radius = 200;
					}
				}
			}
		}
	}
	return 1;
}

/*
=========================
HUD_TxferLocalOverrides

The server sends us our origin with extra precision as part of the clientdata structure, not during the normal
playerstate update in entity_state_t.  In order for these overrides to eventually get to the appropriate playerstate
structure, we need to copy them into the state structure at this point.
=========================
*/
void DLLEXPORT HUD_TxferLocalOverrides( struct entity_state_s *state, const struct clientdata_s *client )
{
	VectorCopy(client->origin, state->origin);
//	VectorCopy(client->velocity, state->velocity);
	state->iuser1 = client->iuser1;
	state->iuser2 = client->iuser2;
	state->iuser3 = client->iuser3;
	state->iuser4 = client->iuser4;
	state->fuser1 = client->fuser1;
	state->fuser2 = client->fuser2;
	state->fuser3 = client->fuser3;
	state->fuser4 = client->fuser4;
}

/*
=========================
HUD_ProcessPlayerState

We have received entity_state_t for this player over the network.  We need to copy appropriate fields to the
playerstate structure
=========================
*/
void DLLEXPORT HUD_ProcessPlayerState( struct entity_state_s *dst, const struct entity_state_s *src )
{
	// XDM3035: right now most of this data is invalid and useless
	// Do not copy fields that are possibly set on client side
//	memcpy(dst, src, sizeof(entity_state_s));// DO NOT! Most data in src is invalid!
	// Copy in network data
	dst->entityType				= src->entityType;
/*	dst->number					= src->number;
	dst->msg_time				= src->msg_time;
	dst->messagenum				= src->messagenum;*/
//	VectorCopy(src->origin,		dst->origin);
//	VectorCopy(src->angles,		dst->angles);
	dst->origin					= src->origin;
	dst->angles					= src->angles;
//	CON_PRINTF("RECV angles: %g\t%g\t%g\n", src->angles[0], src->angles[1], src->angles[2]);
	dst->modelindex				= src->modelindex;
	dst->sequence				= src->sequence;
	dst->frame					= src->frame;
	dst->colormap				= src->colormap;
	dst->skin					= src->skin;
	dst->solid					= src->solid;
	dst->effects				= src->effects;
	dst->scale					= src->scale;
	dst->eflags					= src->eflags;
	dst->rendermode				= src->rendermode;
	dst->renderamt				= src->renderamt;
	dst->rendercolor			= src->rendercolor;
	dst->renderfx				= src->renderfx;
	dst->movetype				= src->movetype;
	dst->animtime				= src->animtime;
	dst->framerate				= src->framerate;
	dst->body					= src->body;
	memcpy(&dst->controller[0],	&src->controller[0], 4 * sizeof(byte));
	memcpy(&dst->blending[0],	&src->blending[0], 4 * sizeof(byte));
//	memcpy(&dst->blending[0],	&src->blending[0], 2 * sizeof(byte));
//	VectorCopy(src->velocity,	dst->velocity);
	dst->velocity				= src->velocity;
/*	dst->mins					= src->mins;
	dst->maxs					= src->maxs;
	dst->aiment					= src->aiment;
	dst->owner					= src->owner;*/
	dst->friction				= src->friction;
	dst->gravity				= src->gravity;
	// PLAYER SPECIFIC
	dst->team					= src->team;
	dst->playerclass			= src->playerclass;
	dst->health					= src->health;// XDM: WTF?!! 0??
//	dst->spectator				= src->spectator;
	dst->weaponmodel			= src->weaponmodel;
	dst->gaitsequence			= src->gaitsequence;
//	VectorCopy(src->basevelocity, dst->basevelocity);
	dst->basevelocity			= src->basevelocity;
	dst->usehull				= src->usehull;
//INVALID!	dst->oldbuttons				= src->oldbuttons;// XDM3035c: causes jump bug
	dst->onground				= src->onground;
/*	dst->iStepLeft				= src->iStepLeft;
	dst->flFallVelocity			= src->flFallVelocity;  
	dst->fov					= src->fov;
	dst->weaponanim				= src->weaponanim;*/
//	VectorCopy(src->startpos,	dst->startpos);
//	VectorCopy(src->endpos,		dst->endpos);
	dst->startpos				= src->startpos;
	dst->endpos					= src->endpos;
	dst->impacttime				= src->impacttime;
	dst->starttime				= src->starttime;
	dst->iuser1					= src->iuser1;
	dst->iuser2					= src->iuser2;
	dst->iuser3					= src->iuser3;
	dst->iuser4					= src->iuser4;
/*	dst->fuser1					= src->fuser1;
	dst->fuser2					= src->fuser2;
	dst->fuser3					= src->fuser3;
	dst->fuser4					= src->fuser4;
	dst->vuser1					= src->vuser1;
	dst->vuser2					= src->vuser2;
	dst->vuser3					= src->vuser3;
	dst->vuser4					= src->vuser4;*/
//	CON_PRINTF("RECV health: %d\n", dst->health);
	// Save off some data so other areas of the Client DLL can get to it
	cl_entity_t *player = gEngfuncs.GetLocalPlayer();	// Get the local player's index
	if (dst->number == player->index)
	{
		memcpy(&gHUD.m_LocalPlayerState, src, sizeof(entity_state_s));// XDM3035
//	XDM	g_iPlayerClass = dst->playerclass;
		gHUD.m_iTeamNumber = src->team;
		g_iUser1 = src->iuser1;
		g_iUser2 = src->iuser2;
		g_iUser3 = src->iuser3;
	}
}

//-----------------------------------------------------------------------------
/*
=========================
HUD_TxferPredictionData

Because we can predict an arbitrary number of frames before the server responds with an update, we need to be able to copy client side prediction data in
 from the state that the server ack'd receiving, which can be anywhere along the predicted frame path ( i.e., we could predict 20 frames into the future and the server ack's
 up through 10 of those frames, so we need to copy persistent client-side only state from the 10th predicted frame to the slot the server update is occupying.
=========================
*/
// Input  : *ps - player state
//			*pps - predicted player state
//			*pcd - clientdata_s
//			*ppcd - predicted clientdata_s
//			*wd - weapon_data_s
//			*pwd - predicted weapon_data_s
//-----------------------------------------------------------------------------
void DLLEXPORT HUD_TxferPredictionData(struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd)
{
	// entity_state_s
	ps->oldbuttons				= pps->oldbuttons;
	ps->flFallVelocity			= pps->flFallVelocity;
	ps->iStepLeft				= pps->iStepLeft;
	ps->playerclass				= pps->playerclass;

	// clientdata_s
	pcd->viewmodel				= ppcd->viewmodel;
	pcd->m_iId					= ppcd->m_iId;
	pcd->m_flNextAttack			= ppcd->m_flNextAttack;
	pcd->fov					= ppcd->fov;
	pcd->weaponanim				= ppcd->weaponanim;
	pcd->tfstate				= ppcd->tfstate;
	pcd->maxspeed				= ppcd->maxspeed;
	pcd->deadflag				= ppcd->deadflag;
	// Spectating or not dead == get control over view angles.
	g_iAlive = (ppcd->iuser1 || (pcd->deadflag == DEAD_NO))?1:0;

	if (gEngfuncs.IsSpectateOnly())
	{
		// in specator mode we tell the engine who we want to spectate and how
		// iuser3 is not used for duck prevention (since the spectator can't duck at all)
		pcd->iuser1 = g_iUser1;	// observer mode
		pcd->iuser2 = g_iUser2; // first target
		pcd->iuser3 = g_iUser3; // second target
	}
	else
	{
		pcd->iuser1 = ppcd->iuser1;// Spectator
		pcd->iuser2 = ppcd->iuser2;
		pcd->iuser3 = ppcd->iuser3;// Duck prevention
	}
	pcd->iuser4 = ppcd->iuser4;// Fire prevention
	pcd->fuser1 = ppcd->fuser1;
	pcd->fuser2 = ppcd->fuser2;
	pcd->fuser3 = ppcd->fuser3;
	pcd->fuser4 = ppcd->fuser4;
	VectorCopy(ppcd->vuser1, pcd->vuser1);
	VectorCopy(ppcd->vuser2, pcd->vuser2);
	VectorCopy(ppcd->vuser3, pcd->vuser3);
	VectorCopy(ppcd->vuser4, pcd->vuser4);

	if ( (ppcd->flags & FL_FROZEN) && !(ppcd->flags & FL_TELEPORTING) )
		gHUD.m_iPlayerFrozen = 1;
	else 
		gHUD.m_iPlayerFrozen = 0;

	// weapon_data_s
	memcpy(wd, pwd, 32 * sizeof(weapon_data_t));
}


// Quake beams
typedef struct
{
	cl_entity_t	*entity;
	struct model_s	*model;
	float		endtime;
	vec3_t		start, end;
} beam_t;

#define MAX_BEAMS		24
#define BEAM_LENGTH		48	// FIXME: get bounds from model?
beam_t	cl_beams[MAX_BEAMS];

/*
=================
CL_ClearBeams
=================
*/
void CL_ClearBeams( void )
{
	memset( cl_beams, 0, sizeof( cl_beams ));
}

/*
=================
CL_AllocBeam
=================
*/
void CL_AllocBeam( const char *model, int entnum, Vector start, Vector end )
{
//	cl_entity_t *ent;
	beam_t *b;
	float life = 0.15f;
	int i;

	//this should fix "hanging in the air" beams
	cl_entity_t *ent = GetUpdatingEntity(entnum);
	if (!ent)
		return;


//	ent = gEngfuncs.GetEntityByIndex( entnum );

//	if (ent == gEngfuncs.GetLocalPlayer())
//		life = 0.07;
//	else life = 0.1f;

	// override any beam with the same entity
	for( i = 0, b = cl_beams; i < MAX_BEAMS; i++, b++ )
	{
		if( b->entity == ent )
		{
			b->entity = ent;
			b->model = IEngineStudio.Mod_ForName( model, false );
			b->endtime = gEngfuncs.GetClientTime() + life;
			b->start = start;
			b->end = end;
			return;
		}
	}

	// find a free beam
	for( i = 0, b = cl_beams; i < MAX_BEAMS; i++, b++ )
	{
		if( !b->model || b->endtime < gEngfuncs.GetClientTime( ))
		{
			b->entity = ent;
			b->model = IEngineStudio.Mod_ForName( model, false );
			b->endtime = gEngfuncs.GetClientTime() + life;
			b->start = start;
			b->end = end;
			return;
		}
	}

	gEngfuncs.Con_Printf( "beam list overflow!\n" );	
}

/*
=================
CL_UpdateBeams
=================
*/
void CL_UpdateBeams( void )
{
	int		i;
	float		d;
	beam_t		*b;
	vec3_t		dist, org, angles;
	TEMPENTITY	*ent;

	// update lightning
	for (i = 0, b = cl_beams; i< MAX_BEAMS ; i++, b++)
	{
		if (!b->model || b->endtime < gEngfuncs.GetClientTime())
			continue;

		VectorSubtract( b->end, b->start, dist );

		// calculate pitch and yaw
		VectorAngles( dist, angles );
		d = VectorNormalize( dist );

		// add new entities for the lightning
		org = b->start;

		while (d > 0)
		{
			ent = gEngfuncs.pEfxAPI->CL_TempEntAlloc( org, b->model );
			if( !ent ) return;

			ent->entity.angles[0] = angles[0];
			ent->entity.angles[1] = angles[1];
			ent->entity.angles[2] = RANDOM_FLOAT( 0, 360 );
			ent->entity.curstate.body = LGTNTG_FIELD_LIGHTNING_BOLT;
			ent->entity.curstate.rendermode = kRenderTransTexture;
			ent->entity.curstate.renderamt = 128;
			ent->entity.curstate.renderfx = kRenderFxFullBright;
			ent->die = gEngfuncs.GetClientTime(); // die at next frame so we can realloc it again
			VectorMA( org, BEAM_LENGTH, dist, org );
			d -= BEAM_LENGTH;
		}
	}
	
}

/*
=========================
HUD_CreateEntities

Gives us a chance to add additional entities to the render this frame
=========================
*/
void DLLEXPORT HUD_CreateEntities( void )
{
//	if (g_pRefParams)
	if (g_pCvarFlashLightMode && g_pCvarFlashLightMode->value > 0.0f)
	{
		for (int i=1; i<=gEngfuncs.GetMaxClients(); ++i)//g_pRefParams->maxclients; ++i)
		{
			cl_entity_t *pPlayer = GetUpdatingEntity(i);
			if (pPlayer && pPlayer->player)//type == ET_PLAYER)
			{
				if (pPlayer->curstate.effects & EF_DIMLIGHT)// XDM3035c: light that is blocked by players!
				{
					CL_UpdateFlashlight(pPlayer);// this will get called AFTER the engine creates its light, so that light will be updated... probably.
				}
/*				if (ent->curstate.effects & EF_DIMLIGHT)// XDM3035c: light that is blocked by players!
				{
					CL_UpdateFlashlight(ent);
					ent->curstate.effects &= ~EF_DIMLIGHT;// don't let the engine redraw the light
				}*/
			}
		}
	}
	// e.g., create a persistent cl_entity_t somewhere.
	// Load an appropriate model into it ( gEngfuncs.CL_LoadModel )
	// Call gEngfuncs.CL_CreateVisibleEntity to add it to the visedicts list

	if (g_pRenderManager)// XDM
		g_pRenderManager->CreateEntities();

	CL_UpdateBeams ();

	// Add in any game specific objects
	try
	{
		GetClientVoiceMgr()->CreateEntities();
	}
	catch (...)
	{
		CON_PRINTF("GetClientVoiceMgr()->CreateEntities() exception!\n");
	}
}


//-----------------------------------------------------------------------------
// Purpose: Old HL-compatible version for events 5001, 5011, 5021, 5031
// Input  : *pEntity - 
//			attachment - 
//			arguments - 
//-----------------------------------------------------------------------------
void FX_MuzzleFlashStudioEvent(const struct cl_entity_s *pEntity, short attachment, int arguments)
{
	if (pEntity)
	{
		short scale = (int)(arguments/10);// 2
		short spr = arguments-(scale*10);// 1
		FX_MuzzleFlashSprite(pEntity->attachment[attachment], pEntity->index, attachment, *g_iMuzzleFlashSprites[spr], 0, 25, 0.1f*(float)scale, true);
		DynamicLight(pEntity->attachment[attachment], 200, 230,210,0, 0.1, 0.0);
	}
}


/*
=========================
HUD_StudioEvent

The entity's studio model description indicated an event was
fired during this frame, handle the event by it's tag ( e.g., muzzleflash, sound )
=========================
*/
void DLLEXPORT HUD_StudioEvent(const struct mstudioevent_s *event, const struct cl_entity_s *entity)
{
/*ok, but useless
	if (entity == gEngfuncs.GetViewModel())
	{
		if (CL_IsThirdPerson())
			return;
	}*/

//	g_pRenderManager->AddSystem(new CRSSprite(entity->attachment[0], g_vecZero, g_iMuzzleFlashSprites[atoi(event->options)], kRenderTransAdd, 255,255,255, 1.0f,-10.0f, 1.0f,2.0f, 16.0f, 0.0f));
	switch (event->event)
	{
	case 5001: FX_MuzzleFlashStudioEvent(entity, 0, atoi(event->options)); break;
	case 5011: FX_MuzzleFlashStudioEvent(entity, 1, atoi(event->options)); break;
	case 5021: FX_MuzzleFlashStudioEvent(entity, 2, atoi(event->options)); break;
	case 5031: FX_MuzzleFlashStudioEvent(entity, 3, atoi(event->options)); break;

	case 5002: gEngfuncs.pEfxAPI->R_SparkEffect((float *)&entity->attachment[0], atoi(event->options), -100, 100); break;
	case 5012: gEngfuncs.pEfxAPI->R_SparkEffect((float *)&entity->attachment[1], atoi(event->options), -100, 100); break;
	case 5022: gEngfuncs.pEfxAPI->R_SparkEffect((float *)&entity->attachment[2], atoi(event->options), -100, 100); break;
	case 5032: gEngfuncs.pEfxAPI->R_SparkEffect((float *)&entity->attachment[3], atoi(event->options), -100, 100); break;

	case 5100:
		{
			// args: "<attachment> <sprite muzzle index> <scale*10>"
			int aa, ai, as;
			char aflags[8];// flags as letters
			memset(aflags, 0, sizeof(aflags));
			bool rotate = false;
			if (sscanf(event->options, "%d %d %d %s", &aa, &ai, &as, &aflags[0]) >= 3)// 4th is optional
			{
				for (byte i = 0; i<sizeof(aflags); ++i)
					if (aflags[i] == 'r')
						rotate = true;

				FX_MuzzleFlashSprite(entity->attachment[aa], entity->index, aa, *g_iMuzzleFlashSprites[ai], 0, 25, 0.01f*(float)as, rotate);
			}
		}
		break;

	case 5004: gEngfuncs.pfnPlaySoundByNameAtLocation((char *)event->options, 1.0, (float *)&entity->attachment[0]); break;
//	case 5005: EMIT_SOUND(0, entity->origin, CHAN_STATIC, (char *)event->options, 1, ATTN_NORM, 0, 110 );break;

	
	case 7000: gEngfuncs.pEfxAPI->R_RicochetSound((float *)&entity->attachment[atoi(event->options)]); break;// XDM
	case 7001: gEngfuncs.pEfxAPI->R_BulletImpactParticles((float *)&entity->attachment[atoi(event->options)]); break;
	case 7002: gEngfuncs.pEfxAPI->R_BlobExplosion((float *)&entity->attachment[atoi(event->options)]); break;
	case 7003: gEngfuncs.pEfxAPI->R_RocketFlare((float *)&entity->attachment[atoi(event->options)]); break;
	case 7004: gEngfuncs.pEfxAPI->R_LargeFunnel((float *)&entity->attachment[atoi(event->options)], 0); break;
	case 7005: gEngfuncs.pEfxAPI->R_LargeFunnel((float *)&entity->attachment[atoi(event->options)], 1); break;
	case 7006: gEngfuncs.pEfxAPI->R_SparkShower((float *)&entity->attachment[atoi(event->options)]); break;
	case 7007: gEngfuncs.pEfxAPI->R_FlickerParticles((float *)&entity->attachment[atoi(event->options)]); break;
	case 7008: gEngfuncs.pEfxAPI->R_TeleportSplash((float *)&entity->attachment[atoi(event->options)]); break;
	case 7009: gEngfuncs.pEfxAPI->R_ParticleExplosion((float *)&entity->attachment[atoi(event->options)]); break;
/*	case 7010:
		{
			int a1,a2;
			if (sscanf(event->options, "%d %d", &a1, &a2) == 2 && a1 != a2)
				gEngfuncs.pEfxAPI->R_ShowLine((float *)&entity->attachment[a1], (float *)&entity->attachment[a2]);
		}
		break;*/
	case 7011: gEngfuncs.pEfxAPI->R_StreakSplash((float *)&entity->attachment[0], (float *)&g_vecZero, atoi(event->options), 32, 100, -200, 200); break;
	default: CON_DPRINTF("Unknown event '%d' in model '%s'!\n", event->event, entity->model->name); break;
	}
}

#define TE_FX_UPDATE_PERIOD 0.15
static float fAccumulatedPeriod = 0;// accumulate frametime until period is reached

/*
=================
CL_UpdateTEnts

Simulation and cleanup of temporary entities

  Oh, now I get it! pTemp->entity.baseline.origin is actually velocity!!
=================
*/
void DLLEXPORT HUD_TempEntUpdate(
	double frametime,   // Simulation time
	double client_time, // Absolute time on client
	double cl_gravity,  // True gravity on client
	TEMPENTITY **ppTempEntFree,   // List of freed temporary ents
	TEMPENTITY **ppTempEntActive, // List 
	int ( *Callback_AddVisibleEntity )( cl_entity_t *pEntity ),
	void ( *Callback_TempEntPlaySound )( TEMPENTITY *pTemp, float damp ) )
{
//	static int gTempEntFrame = 0;
	int			i;
	TEMPENTITY	*pTemp, *pnext, *pprev;
	float		freq, gravity, life, fastFreq;

	g_cl_gravity = cl_gravity;// XDM3035
	// Nothing to simulate
	if ( !*ppTempEntActive )		
		return;

	// in order to have tents collide with players, we have to run the player prediction code so
	// that the client has the player list. We run this code once when we detect any COLLIDEALL 
	// tent, then set this BOOL to true so the code doesn't get run again if there's more than
	// one COLLIDEALL ent for this update. (often are).
	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);
	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();
	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers(-1);

	// !!!BUGBUG	-- This needs to be time based
//	gTempEntFrame = (gTempEntFrame+1) & 31;

	pTemp = *ppTempEntActive;

	// !!! Don't simulate while paused....  This is sort of a hack, revisit.
	if (gHUD.m_iPaused > 0)// XDM: revisited :)
	{
		while (pTemp)
		{
			if (!(pTemp->flags & FTENT_NOMODEL))
				Callback_AddVisibleEntity(&pTemp->entity);

			pTemp = pTemp->next;
		}
		goto finish;
	}

	pprev = NULL;
	freq = client_time * 0.01;
	fastFreq = client_time * 5.5;
	gravity = -frametime * cl_gravity;

	if (fAccumulatedPeriod > TE_FX_UPDATE_PERIOD)// previous frame overcame one second interval
		fAccumulatedPeriod -= TE_FX_UPDATE_PERIOD;

	fAccumulatedPeriod += frametime;

	while (pTemp)
	{
		int active = 1;

		if (pTemp->die >= 0)
			life = pTemp->die - client_time;
		else
			life = 1.0;

		pnext = pTemp->next;
		if (life < 0)
		{
			if (pTemp->flags & FTENT_FADEOUT)
			{
				if (pTemp->entity.curstate.rendermode == kRenderNormal)
					pTemp->entity.curstate.rendermode = kRenderTransTexture;

				pTemp->entity.curstate.renderamt = (int)((float)pTemp->entity.baseline.renderamt * (1.0f + life * pTemp->fadeSpeed));
				if (pTemp->entity.curstate.renderamt <= 0)
					active = 0;
			}
			else 
				active = 0;
		}
		if (!active)		// Kill it
		{
			pTemp->next = *ppTempEntFree;
			*ppTempEntFree = pTemp;
			if (!pprev)// Deleting at head of list
				*ppTempEntActive = pnext;
			else
				pprev->next = pnext;
		}
		else
		{
			pprev = pTemp;
			VectorCopy( pTemp->entity.origin, pTemp->entity.prevstate.origin );
			if (pTemp->flags & FTENT_SPARKSHOWER)
			{
				// Adjust speed if it's time
				// Scale is next think time
				if ( client_time > pTemp->entity.baseline.scale )
				{
					// Show Sparks
					gEngfuncs.pEfxAPI->R_SparkEffect(pTemp->entity.origin, 8, -200, 200);
					// Reduce life
					pTemp->entity.baseline.framerate -= 0.1;
					if (pTemp->entity.baseline.framerate <= 0.0)
					{
						pTemp->die = client_time;
					}
					else
					{
						// So it will die no matter what
						pTemp->die = client_time + 0.5;
						// Next think
						pTemp->entity.baseline.scale = client_time + 0.1;
					}
				}
			}
			else if (pTemp->flags & FTENT_PLYRATTACHMENT)
			{
				cl_entity_t *pClient = GetUpdatingEntity(pTemp->clientIndex);// XDM3035c: TESTME gEngfuncs.GetEntityByIndex( pTemp->clientIndex );
				if (pClient)
				{
/*					if (pTemp->entity.curstate.aiment)// XDM: UNDONE
						VectorAdd( pClient->attachment[pTemp->entity.curstate.aiment], pTemp->tentOffset, pTemp->entity.origin );
					else*/
						VectorAdd( pClient->origin, pTemp->tentOffset, pTemp->entity.origin );
				}
				else// XDM: remove TENT if entity not found
					pTemp->die = client_time;
			}
			else if (pTemp->flags & FTENT_SINEWAVE)
			{
				// pTemp->xyz += pTemp->entity.baseline.origin * frametime;
				// pTemp->entity.origin = pTemp->xyz + sin(pTemp->entity.baseline.origin + client_time*pTemp->entity.prevstate.frame) * (10*pTemp->entity.curstate.framerate);
				pTemp->x += pTemp->entity.baseline.origin[0] * frametime;
				pTemp->y += pTemp->entity.baseline.origin[1] * frametime;
//				pTemp->z += pTemp->entity.baseline.origin[2] * frametime;
				pTemp->entity.origin[0] = pTemp->x + sin(pTemp->entity.baseline.origin[2] + client_time * pTemp->entity.prevstate.frame) * (10*pTemp->entity.curstate.framerate);
				pTemp->entity.origin[1] = pTemp->y + sin(pTemp->entity.baseline.origin[2] + fastFreq + 0.7) * (8*pTemp->entity.curstate.framerate);
//				pTemp->entity.origin[2] = pTemp->z;
				pTemp->entity.origin[2] += pTemp->entity.baseline.origin[2] * frametime;
			}
			else if (pTemp->flags & FTENT_SPIRAL)
			{
				float s, c;
				SinCos(pTemp->entity.baseline.origin[2] + fastFreq, &s, &c);// XDM
				pTemp->entity.origin[0] += pTemp->entity.baseline.origin[0] * frametime + 8 * sin(client_time * 20 + (int)pTemp);
				pTemp->entity.origin[1] += pTemp->entity.baseline.origin[1] * frametime + 4 * sin(client_time * 30 + (int)pTemp);
				pTemp->entity.origin[2] += pTemp->entity.baseline.origin[2] * frametime;
			}
			else
			{
				for (i = 0; i < 3; i++)
					pTemp->entity.origin[i] += pTemp->entity.baseline.origin[i] * frametime;
			}

			if (pTemp->flags & FTENT_SPRANIMATE)
			{
				pTemp->entity.curstate.frame += frametime * pTemp->entity.curstate.framerate;
				if (pTemp->entity.curstate.frame >= pTemp->frameMax)
				{
					pTemp->entity.curstate.frame = pTemp->entity.curstate.frame - (int)(pTemp->entity.curstate.frame);
					if (!(pTemp->flags & FTENT_SPRANIMATELOOP))
					{
						// this animating sprite isn't set to loop, so destroy it.
						pTemp->die = client_time;
						pTemp = pnext;
						continue;
					}
				}
			}
			else if (pTemp->flags & FTENT_SPRCYCLE)
			{
				pTemp->entity.curstate.frame += frametime * 10;
				if (pTemp->entity.curstate.frame >= pTemp->frameMax)
					pTemp->entity.curstate.frame = pTemp->entity.curstate.frame - (int)(pTemp->entity.curstate.frame);
			}

			if (pTemp->flags & FTENT_ROTATE)
			{
				pTemp->entity.angles[0] += pTemp->entity.baseline.angles[0] * frametime;
				pTemp->entity.angles[1] += pTemp->entity.baseline.angles[1] * frametime;
				pTemp->entity.angles[2] += pTemp->entity.baseline.angles[2] * frametime;
				VectorCopy(pTemp->entity.angles, pTemp->entity.latched.prevangles);
			}

			if (pTemp->flags & (FTENT_COLLIDEALL | FTENT_COLLIDEWORLD))
			{
				vec3_t	traceNormal;
				float	traceFraction = 1;

				if (pTemp->flags & FTENT_COLLIDEALL)
				{
					pmtrace_t pmtrace;
					physent_t *pe;
					gEngfuncs.pEventAPI->EV_SetTraceHull(2);
					gEngfuncs.pEventAPI->EV_PlayerTrace(pTemp->entity.prevstate.origin, pTemp->entity.origin, PM_STUDIO_BOX, -1, &pmtrace);

					if (pmtrace.fraction != 1)
					{
						pe = gEngfuncs.pEventAPI->EV_GetPhysent(pmtrace.ent);
						if (!pmtrace.ent || (pe->info != pTemp->clientIndex))
						{
							traceFraction = pmtrace.fraction;
							VectorCopy(pmtrace.plane.normal, traceNormal);

							if (pTemp->hitcallback)
								(*pTemp->hitcallback)(pTemp, &pmtrace);
						}
					}
				}
				else if (pTemp->flags & FTENT_COLLIDEWORLD)
				{
					pmtrace_t pmtrace;
					gEngfuncs.pEventAPI->EV_SetTraceHull(2);
					gEngfuncs.pEventAPI->EV_PlayerTrace(pTemp->entity.prevstate.origin, pTemp->entity.origin, PM_STUDIO_BOX | PM_WORLD_ONLY, -1, &pmtrace);

					if (pmtrace.fraction != 1)
					{
						traceFraction = pmtrace.fraction;
						VectorCopy(pmtrace.plane.normal, traceNormal);

						if (pTemp->flags & FTENT_SPARKSHOWER)
						{
							// Chop spark speeds a bit more
							VectorScale(pTemp->entity.baseline.origin, 0.6, pTemp->entity.baseline.origin);

							if (Length(pTemp->entity.baseline.origin) < 10)
								pTemp->entity.baseline.framerate = 0.0;								
						}

						if (pTemp->hitcallback)
							(*pTemp->hitcallback)(pTemp, &pmtrace);
					}
				}

				if (traceFraction != 1.0f)	// Decent collision now, and damping works
				{
					float  proj, damp;
					// Place at contact point
					VectorMA(pTemp->entity.prevstate.origin, traceFraction*frametime, pTemp->entity.baseline.origin, pTemp->entity.origin);
					// Damp velocity
					damp = pTemp->bounceFactor;
					if ( pTemp->flags & (FTENT_GRAVITY|FTENT_SLOWGRAVITY))
					{
						damp *= 0.5;
						if (traceNormal[2] > 0.9)		// Hit floor?
						{
							if (pTemp->entity.baseline.origin[2] <= 0 && pTemp->entity.baseline.origin[2] >= gravity*3)
							{
								damp = 0;		// Stop
								pTemp->flags &= ~(FTENT_ROTATE|FTENT_GRAVITY|FTENT_SLOWGRAVITY|FTENT_COLLIDEWORLD|FTENT_SMOKETRAIL);
								pTemp->entity.angles[0] = 0.0f;
								pTemp->entity.angles[2] = 0.0f;
							}
						}
					}

					if (pTemp->hitSound)
						CL_TempEntPlaySound(pTemp, damp);// XDM: this one is a lot better

					if (pTemp->flags & FTENT_COLLIDEKILL)
					{
						// die on impact
						pTemp->flags &= ~FTENT_FADEOUT;	
						pTemp->die = client_time;			
					}
					else// Reflect velocity
					{
						if (damp != 0)
						{
							proj = DotProduct(pTemp->entity.baseline.origin, traceNormal);
							VectorMA( pTemp->entity.baseline.origin, -proj*2, traceNormal, pTemp->entity.baseline.origin );
							// Reflect rotation (fake)
							pTemp->entity.angles[1] = -pTemp->entity.angles[1];
						}

						if (damp != 1)
						{
							VectorScale(pTemp->entity.baseline.origin, damp, pTemp->entity.baseline.origin);
							VectorScale(pTemp->entity.angles, 0.9, pTemp->entity.angles);
						}
					}
				}
/*XDM: sometimes it affects gibs in the air :(
				else
				{
					int contents = gEngfuncs.PM_PointContents(pTemp->entity.origin, NULL);
					if (contents <= CONTENTS_WATER)
					{
//						pev->velocity = pev->velocity * 0.5;
//						pev->avelocity = pev->avelocity * 0.9;
						// what the shit is this?
						VectorScale(pTemp->entity.baseline.origin, 0.95, pTemp->entity.baseline.origin);// this is VELOCITY!
						VectorScale(pTemp->entity.angles, 0.99, pTemp->entity.angles);
						gravity *= 0.01f;
						if (pTemp->hitSound == BOUNCE_WOOD)
							pTemp->entity.origin[2] += frametime*16;
						else if (pTemp->hitSound == BOUNCE_FLESH)
							pTemp->entity.origin[2] += frametime*8;
							//pev->velocity.z += 8.0;

					}
				}*/
			}// (pTemp->flags & (FTENT_COLLIDEALL | FTENT_COLLIDEWORLD))

			if ((pTemp->flags & FTENT_FLICKER))// && gTempEntFrame == pTemp->entity.curstate.effects )
			{
				dlight_t *dl = gEngfuncs.pEfxAPI->CL_AllocDlight(0);
				VectorCopy(pTemp->entity.origin, dl->origin);
				dl->radius = (float)pTemp->entity.baseline.renderamt;// XDM: 60
				dl->color.r = pTemp->entity.baseline.rendercolor.r;// XDM: 255
				dl->color.g = pTemp->entity.baseline.rendercolor.g;// XDM: 120
				dl->color.b = pTemp->entity.baseline.rendercolor.b;// XDM: 0
				dl->die = client_time + 0.01;
			}


			if (pTemp->flags & FTENT_SMOKETRAIL)
			{
				int contents = gEngfuncs.PM_PointContents(pTemp->entity.origin, NULL);
				if (contents >= CONTENTS_EMPTY)
				{
					switch (pTemp->entity.curstate.usehull)
					{
						case TENT_TRAIL_FIRESMOKE:
							gEngfuncs.pEfxAPI->R_RocketTrail(pTemp->entity.prevstate.origin, pTemp->entity.origin, 0);
						break;

						case TENT_TRAIL_SMOKE:
							gEngfuncs.pEfxAPI->R_RocketTrail(pTemp->entity.prevstate.origin, pTemp->entity.origin, 1);
						break;

						case TENT_TRAIL_BLOODTRAIL:
							gEngfuncs.pEfxAPI->R_RocketTrail(pTemp->entity.prevstate.origin, pTemp->entity.origin, 2);
						break;

						case TENT_TRAIL_SPARKS:
							gEngfuncs.pEfxAPI->R_SparkEffect(pTemp->entity.origin, 1, -128, 128);
						break;

						case TENT_TRAIL_STREAKS:
							gEngfuncs.pEfxAPI->R_StreakSplash (pTemp->entity.origin, Vector(0,0,0), 5, RANDOM_FLOAT(3,7), 400, -150, 150);
						break;

						case TENT_TRAIL_SPARKSMOKE:
							gEngfuncs.pEfxAPI->R_BulletImpactParticles(pTemp->entity.origin);
						break;

						case TENT_TRAIL_RS_FIRE:
							g_pRenderManager->AddSystem(new CPSBlastCone(1, 30, pTemp->entity.origin, pTemp->entity.origin, Vector(1,1,1), 5, 15, 255,255,255, 0.8, -1.2, g_iModelIndexFlame, TRUE, 0, kRenderTransAdd, 0.01), RENDERSYSTEM_FLAG_RANDOMFRAME, -1);
						break;

						case TENT_TRAIL_RS_FIRE2:
							g_pRenderManager->AddSystem(new CPSBlastCone(1, 30, pTemp->entity.origin, pTemp->entity.origin, Vector(1,1,1), 5, 15, 255,255,255, 1.0, -1.0, g_iModelIndexFire, TRUE, 0, kRenderTransAdd, 0.01), RENDERSYSTEM_FLAG_RANDOMFRAME, -1);
						break;

						case TENT_TRAIL_RS_BLACKSMOKE:
							g_pRenderManager->AddSystem(new CPSBlastCone(1, 40, pTemp->entity.origin, Vector(0,0,1), VECTOR_CONE_20DEGREES, 5, 15, 0,0,0, 0.75, -0.75, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_0, kRenderTransAlpha, 0.01), 0, -1);
						break;

						case TENT_TRAIL_RS_WHITESMOKE:
							g_pRenderManager->AddSystem(new CPSBlastCone(1, 30, pTemp->entity.origin, pTemp->entity.origin, Vector(1,1,1), 5, 18, 128,128,128, 0.75, -0.75, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_1, kRenderTransAdd, 0.01), 0, -1);
						break;	
						
						case TENT_TRAIL_RS_GREENSMOKE:
							g_pRenderManager->AddSystem(new CPSBlastCone(1, 30, pTemp->entity.origin, pTemp->entity.origin, Vector(1,1,1), 5, 20, 0,128,0, 0.75, -0.9, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_1, kRenderTransAdd, 0.01), 0, -1);
						break;	

						case TENT_TRAIL_RS_BLUESMOKE:
							g_pRenderManager->AddSystem(new CPSBlastCone(1, 30, pTemp->entity.origin, pTemp->entity.origin, Vector(1,1,1), 5, 20, 128,128,128, 0.75, -0.9, g_iModelIndexAnimSpr5, TRUE, 0, kRenderTransAdd, 0.01), 0, -1);
						break;

					}
				}
				else if (contents == CONTENTS_WATER || contents == CONTENTS_SLIME)
				{
					if (fAccumulatedPeriod >= TE_FX_UPDATE_PERIOD)// once per period
						FX_BubblesPoint(pTemp->entity.origin, VECTOR_CONE_15DEGREES, g_iModelIndexBubble, 4, 25);
				}
			}

			if (pTemp->flags & FTENT_GRAVITY)
				pTemp->entity.baseline.origin[2] += gravity;
			else if (pTemp->flags & FTENT_SLOWGRAVITY)
				pTemp->entity.baseline.origin[2] += gravity*0.5f;

			if (pTemp->flags & FTENT_CLIENTCUSTOM)
			{
				if (pTemp->callback)
					(*pTemp->callback)(pTemp, frametime, client_time);
			}

			// Cull to PVS (not frustum cull, just PVS)
			if (!(pTemp->flags & FTENT_NOMODEL))
			{
				// TODO: check bounding box
				if (UTIL_PointIsVisible(pTemp->entity.origin, false))// XDM3035c: BUGBUG: ents disappear when player enters and exits "world" water
				{
					if (!Callback_AddVisibleEntity(&pTemp->entity))
					{
						if (!(pTemp->flags & FTENT_PERSIST)) 
						{
							pTemp->die = client_time;			// If we can't draw it this frame, just dump it.
							pTemp->flags &= ~FTENT_FADEOUT;	// Don't fade out, just die
						}
						else
						{
							// XDM3035: finally!
							if (!(pTemp->flags & FTENT_NOMODEL) && pTemp->entity.model && (pTemp->entity.baseline.eflags & EFLAG_DRAW_ALWAYS))
							{
								// prevent drawing static entities behind walls UNDONE: check REAL FL_DRAW_ALWAYS for skybox tempents
								pmtrace_t pmtrace;
								gEngfuncs.pEventAPI->EV_SetTraceHull(2);
								gEngfuncs.pEventAPI->EV_PlayerTrace(pTemp->entity.origin, g_vecViewOrigin, PM_STUDIO_IGNORE|PM_GLASS_IGNORE|PM_WORLD_ONLY, -1, &pmtrace);
								// since we can't check BSP leafs and nodes (all NULL), we use this simple method
								if (pmtrace.fraction >= 1.0f || (pmtrace.startsolid && pmtrace.fraction == 0.0f))
									gEngfuncs.CL_CreateVisibleEntity(ET_TEMPENTITY, &pTemp->entity);
							}
						}
					}
				}
			}
		}
		pTemp = pnext;
	}

finish:
	// Restore state info
	gEngfuncs.pEventAPI->EV_PopPMStates();
}

/*
=================
HUD_GetUserEntity

If you specify negative numbers for beam start and end point entities, then
  the engine will call back into this function requesting a pointer to a cl_entity_t 
  object that describes the entity to attach the beam onto.

Indices must start at 1, not zero.
=================
*/
cl_entity_t DLLEXPORT *HUD_GetUserEntity( int index )
{
#if defined( BEAM_TEST )
	// None by default, you would return a valic pointer if you create a client side
	//  beam and attach it to a client side entity.
	if ( index > 0 && index <= 1 )
	{
		return &beams[ index ];
	}
	else
	{
		return NULL;
	}
#else
//	if (index == -1)// XDM
//		return gEngfuncs.GetViewModel();
//	else
		return NULL;
#endif
}


void CL_TempEntPlaySound(TEMPENTITY *pTemp, float damp)
{
	if (pTemp == NULL || damp <= 0.0f)
		return;

	bool isshell = false;
	float fattn = ATTN_STATIC;
	const char *sample = NULL;

	switch (pTemp->hitSound)
	{
	case BOUNCE_GLASS:
		fattn = ATTN_NORM;
		sample = gSoundsGlass[RANDOM_LONG(0,NUM_SHARD_SOUNDS-1)];
		break;
	case BOUNCE_METAL:
		fattn = ATTN_NORM;
		sample = gSoundsMetal[RANDOM_LONG(0,NUM_SHARD_SOUNDS-1)];
		break;
	case BOUNCE_FLESH:
		fattn = ATTN_IDLE;
		sample = gSoundsFlesh[RANDOM_LONG(0,NUM_SHARD_SOUNDS-1)];
		break;
	case BOUNCE_WOOD:
		sample = gSoundsWood[RANDOM_LONG(0,NUM_SHARD_SOUNDS-1)];
		break;
	case BOUNCE_SHRAP:
		fattn = ATTN_NORM;
		sample = gSoundsRicochet[RANDOM_LONG(0,4)];//ARRAYSIZE(gSoundsRicochet)-1)];
		break;
	case BOUNCE_SHOTSHELL:
		sample = gSoundsShellShotgun[RANDOM_LONG(0,2)];//ARRAYSIZE(gSoundsShellShotgun)-1)];
		isshell = true;
		break;
	case BOUNCE_SHELL:
		sample = gSoundsShell9mm[RANDOM_LONG(0,2)];//ARRAYSIZE(gSoundsShell9mm)-1)];
		isshell = true;
		break;
	case BOUNCE_CONCRETE:
		sample = gSoundsConcrete[RANDOM_LONG(0,NUM_SHARD_SOUNDS-1)];
		break;
	default:
		{
		return;
		break;
		}
	}

	float zvel = fabsf(pTemp->entity.baseline.origin[2]);
	if (isshell)
	{	
		if (zvel < 200 && RANDOM_LONG(0,3))
			return;
	}
	else
	{
		if (RANDOM_LONG(0,3)) 
			return;
	}

	float fvol = 1.0f;
	if (isshell)
		fvol *= min((float)VOL_NORM, zvel / 350.0f); 
	else
		fvol *= min((float)VOL_NORM, zvel / 450.0f); 

	int	pitch;
	if (!isshell && RANDOM_LONG(0,3) == 0)
		pitch = RANDOM_LONG(95, 105);
	else
		pitch = PITCH_NORM;

//	CON_DPRINTF("CL_TempEntPlaySound() sample %s hitSound %d\n", sample, pTemp->hitSound);
	gEngfuncs.pEventAPI->EV_PlaySound(pTemp->entity.index, pTemp->entity.origin, CHAN_STATIC, sample, fvol, fattn, 0, pitch);
}

void CL_Precache(void)
{
	PrecacheSharedResources();
}

