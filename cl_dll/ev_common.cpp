/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
// shared event functions
#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"

#include "r_efx.h"

#include "eventscripts.h"
#include "event_api.h"
#include "pm_shared.h"
#include "in_defs.h"

#include "RenderSystem.h"
#include "RenderManager.h"
#include "ParticleSystem.h"
#include "PSBlastCone.h"
#include "msg_fx.h"
#include "shared_resources.h"

#define IS_FIRSTPERSON_SPEC (g_iUser1 == OBS_IN_EYE || (g_iUser1 && (gHUD.m_Spectator.m_iInsetMode == INSET_IN_EYE)) )

extern "C" 
{
	int EXPORT CL_IsThirdPerson(void);
}

/*
=================
EV_IsPlayer

Is the entity's index in the player range?
=================
*/
bool EV_IsPlayer(int idx)
{
	return IsValidPlayerIndex(idx);
}

/*
=================
EV_IsLocal

Is the entity == the local player
=================
*/
bool EV_IsLocal(int idx)
{
	// check if we are in some way in first person spec mode
	if ( IS_FIRSTPERSON_SPEC  )
		return (g_iUser2 == idx);
	else
		return gEngfuncs.pEventAPI->EV_IsLocal( idx - 1 ) ? true : false;
}

/*
=================
EV_GetGunPosition

Figure out the height of the gun
=================
*/
void EV_GetGunPosition( event_args_t *args, float *pos, const float *origin )
{
	vec3_t view_ofs;
	int idx = args->entindex;

	VectorClear(view_ofs);
	view_ofs[2] = VEC_VIEW;// 28

	if (EV_IsPlayer(idx))
	{
		// in spec mode use entity viewheight, not own
		if (EV_IsLocal(idx) && !IS_FIRSTPERSON_SPEC)
		{
			if (CL_IsThirdPerson())// Grab predicted result for local player
			{
				gEngfuncs.pEventAPI->EV_LocalPlayerViewheight( view_ofs );
			}
			else
			{
				cl_entity_s *ent = gEngfuncs.GetViewModel();
				if (ent)
				{
					VectorCopy(ent->attachment[0], pos);// is attachment[] unsafe?
					return;
				}
			}
		}
		else if (args->ducking == 1)
		{
			view_ofs[2] = DUCK_VIEW;
		}
	}

	VectorAdd( origin, view_ofs, pos );
}

/*
=================
EV_MuzzleFlash

Flag weapon/view model for muzzle flash
=================
*/
void EV_MuzzleFlash(int entindex)
{
	cl_entity_t *ent = NULL;

	if (EV_IsLocal(entindex) && !CL_IsThirdPerson())
		ent = gEngfuncs.GetViewModel();
	else
		ent = gEngfuncs.GetEntityByIndex(entindex);

	if (ent)
		ent->curstate.effects |= EF_MUZZLEFLASH;
}

void EV_PrintParams(event_args_t *args)
{
	CON_DPRINTF("EV: ei %d, f1 %f, f2 %f, i1 %d, i2 %d, b1 %d, b2 %d\n", args->entindex, args->fparam1, args->fparam2, args->iparam1, args->iparam2, args->bparam1, args->bparam2);
}

//====================
//Ghoul [BB]
//Bullet shell casings
//====================
void FX_BrassShell(float *origin, float rotation, int soundtype, int body, float iLife)
{
	vec3_t ShellVelocity, src, forward, right, up;

	float fR, fU;
	fR = RANDOM_FLOAT(-60, -100);
	fU = RANDOM_FLOAT(100, 140);

	VectorClear(src);
	src[1] = rotation;
	AngleVectors(src, forward, right, up);

	for (int i = 0; i < 3; i++ )
		ShellVelocity[i] = -forward[i] * fR + up[i] * fU + right[i] * RANDOM_FLOAT(50,80);

	TEMPENTITY *shell = gEngfuncs.pEfxAPI->R_TempModel( origin, ShellVelocity, src, iLife, g_iModelIndexGunShellModel, soundtype );
	if (shell)
		shell->entity.curstate.body = body;			
}