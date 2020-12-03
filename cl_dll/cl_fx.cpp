#include "hud.h"
#include "cl_util.h"
#include "cdll_int.h"
#include "r_efx.h"
#include "event_api.h"
#include "pm_defs.h"
#include "pmtrace.h"
#include "shared_resources.h"
#include "cl_fx.h"
#include "decals.h"
#include "RenderManager.h"
#include "RenderSystem.h"
#include "RSSprite.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "PSFlameCone.h"
#include "PSBlastCone.h"
#include "PSBubbles.h"
#include "PSSparks.h"
#include "weapondef.h"
#include "screenfade.h"
#include "shake.h"
#include "triangleapi.h"
#include "msg_fx.h"

#define FLASHLIGHT_DISTANCE		1280
#define FLASHLIGHT_RADIUS1		56
#define FLASHLIGHT_RADIUS2		480

color24 gTracerColors[] =
{
	{ 255, 255, 255 },		// 0 White
	{ 255, 0, 0 },			// Red
	{ 0, 255, 0 },			// Green
	{ 0, 0, 255 },			// Blue
	{ 0, 0, 0 },			// Tracer default, filled in from cvars, etc.
	{ 255, 167, 17 },		// Yellow-orange sparks
	{ 255, 130, 90 },		// Yellowish streaks (garg)
	{ 55, 60, 144 },		// Blue egon streak
	{ 255, 130, 90 },		// More Yellowish streaks (garg)
	{ 255, 140, 90 },		// More Yellowish streaks (garg)
	{ 200, 130, 90 },		// 10 More red streaks (garg)
	{ 255, 120, 70 },		// Darker red streaks (garg)
	// Here goes XDM! >:)
	{ 255, 191, 191 },		// Very bright red
	{ 191, 255, 191 },		// Very bright green
	{ 191, 191, 255 },		// Very bright blue
	{ 255, 255, 191 },		// Very bright yellow
	{ 255, 191, 255 },		// Very bright magenta
	{ 191, 255, 255 },		// Very bright cyan
};


int *g_iMuzzleFlashSprites[] =
{
	&g_iModelIndexMuzzleFlash1,
	&g_iModelIndexMuzzleFlash2,
	&g_iModelIndexMuzzleFlash3
};


//-----------------------------------------------------------------------------
// Purpose: Flashlight effect override, so players can block the light!
// Assumes that engine uses player indexes for light keys
// Thanks, uncle mike
// Input  : *pEnt - 
//-----------------------------------------------------------------------------
dlight_t *CL_UpdateFlashlight(cl_entity_t *pEnt)
{
	vec3_t vecSrc, vecEnd;
	vec3_t vecForward, view_ofs;

	VectorClear(view_ofs);

	if (pEnt->index == gEngfuncs.GetLocalPlayer()->index)// cl.playernum)
	{
//		vecForward = g_vecViewForward;
		gEngfuncs.pEventAPI->EV_LocalPlayerViewheight(view_ofs);
	}
/*	else
	{
		vec3_t	v_angle;
		// restore viewangles from angles
		v_angle[PITCH] = -pEnt->angles[PITCH] * 3;
		v_angle[YAW] = pEnt->angles[YAW];
		v_angle[ROLL] = 0; 	// no roll
		AngleVectors(v_angle, vecForward, NULL, NULL);
	}*/

	vec3_t v_angle;
	v_angle[PITCH] = pEnt->angles[PITCH] * 9.0f;// HACK: what the shit is this?!
	v_angle[YAW] = pEnt->angles[YAW];
	v_angle[ROLL] = pEnt->angles[ROLL];
	AngleVectors(v_angle, vecForward, NULL, NULL);

	VectorAdd(pEnt->origin, view_ofs, vecSrc);
//	VectorMA(vecSrc, FLASHLIGHT_DISTANCE, forward, vecEnd);
	vecEnd = vecSrc + vecForward*FLASHLIGHT_DISTANCE;

	pmtrace_t trace;
	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);
	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetSolidPlayers(-1);// the main purpose of the entire function
	gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	int pe_ignore = GetPhysent(pEnt->index);
	gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc.As3f(), vecEnd.As3f(), PM_STUDIO_BOX/*PM_GLASS_IGNORE*/, pe_ignore, &trace);
	gEngfuncs.pEventAPI->EV_PopPMStates();

//	if (trace.fraction == 1.0f)// XDM3035c: don't
//		return NULL;

	// EXPERIMENTAL: Idea is great, but looks very bad. Needs glow, not additive overlay.
/*	if (trace.ent)// XDM3037: someone's pointing flashlight at my face
	{
		int hitentindex = gEngfuncs.pEventAPI->EV_IndexFromTrace(&trace);
		if (IsValidPlayerIndex(hitentindex))
		{
			if (hitentindex != pEnt->index && hitentindex == gEngfuncs.GetLocalPlayer()->index)
			{
				vec3_t screen;
				if (gEngfuncs.pTriAPI->WorldToScreen(vecSrc, screen) != 1)// TODO: also check if player is looking at me
				{
					screenfade_s sf;
					GET_SCREEN_FADE(&sf);
					sf.fadeSpeed = 800;
					sf.fadeEnd = gEngfuncs.GetClientTime() + 0.05;
					sf.fader = 255;
					sf.fadeg = 255;
					sf.fadeb = 255;
					sf.fadealpha = 255;
					sf.fadeFlags = FFADE_IN;
					SET_SCREEN_FADE(&sf);
				}
			}
		}
	}*/
//TEST	gEngfuncs.pEfxAPI->R_ShowLine(vecSrc, trace.endpos);
/*
	float falloff = trace.fraction * FLASHLIGHT_DISTANCE;

	if (falloff < 250.0f)
		falloff = 1.0f;
	else
	{
		falloff = 250.0f / falloff;
		falloff *= falloff;
	}
*/
	int	key;
//	if (g_pRefParams && g_pRefParams->maxclients == 1)//(cl.maxclients == 1)
	if (gHUD.m_iGameType == GT_SINGLE)
		key = gEngfuncs.GetLocalPlayer()->index;
	else
		key = pEnt->index;

	// Engine will (nullify and?) return existing light structure with this key
	dlight_t *dl = gEngfuncs.pEfxAPI->CL_AllocDlight(key);
	if (dl)
	{
//		byte br = clamp((byte)(255 * (1.0f-trace.fraction*1.5f)), 0, 255);
		byte br;
		if (trace.fraction == 1.0f)// XDM3036: try to lighen up some space
			br = 63;
		else
			br = 63+(byte)(192 * (1.0f - clamp(trace.fraction*1.5f-0.5f, 0.0, 1.0)));
			//br = (byte)(255 * (1.0f - clamp(trace.fraction*1.5f-0.5f, 0.0, 1.0)));

		VectorCopy(trace.endpos, dl->origin);
		dl->die = gEngfuncs.GetClientTime() + 0.01f; // die on next frame
//		dl->color.r = dl->color.g = dl->color.b = clamp((byte)(255 * falloff), 0, 255);
		dl->color.r = dl->color.g = dl->color.b = br;
		dl->radius = FLASHLIGHT_RADIUS1 + FLASHLIGHT_RADIUS2*trace.fraction;//72 + (1.0f-);
//		CON_PRINTF("light: fr = %g (x1.5=%g), br = %d, r = %g\n", trace.fraction, trace.fraction*1.5f, br, dl->radius);
	}
	return dl;
}


// this has different meaning than engine parameter
float FX_GetBubbleSpeed(void)//float height)
{
//	return CVAR_GET_FLOAT("test1");
	return (144.0f + (float)(DEFAULT_GRAVITY-g_cl_gravity));
}


particle_t *DrawParticle(const Vector &origin, short color, float life)
{
	particle_t *p = gEngfuncs.pEfxAPI->R_AllocParticle(NULL);
	if (p != NULL)
	{
		VectorCopy(origin, p->org);
		VectorClear(p->vel);
		p->color = color;
		gEngfuncs.pEfxAPI->R_GetPackedColor(&p->packedColor, p->color);
		p->die += life;
	}
	return p;
}

void ParticleCallback(struct particle_s *particle, float frametime)
{
	if (gHUD.m_iPaused <= 0)// fixes sparks in multiplayer
	{
		for (int i = 0; i < 3; ++i)
			particle->org[i] += particle->vel[i]*frametime;
	}
}

void ParticlesCustom(const Vector &origin, float rnd_vel, int color, int color_range, int number, float life, bool normalize)
{
	particle_t *p = NULL;
	for (int i=0; i<number; ++i)
	{
		p = gEngfuncs.pEfxAPI->R_AllocParticle(ParticleCallback);
		if (!p)
			continue;

		VectorCopy(origin, p->org);
		VectorRandom(p->vel);
		if (normalize)
		{
//			VectorAdd(VectorRandom(), VectorRandom(), p->vel); ???
			VectorNormalize(p->vel);
		}
		else
		{
//			VectorRandom(rnd);
//			VectorMultiply(p->vel, rnd, p->vel);
		}

		VectorScale(p->vel, rnd_vel, p->vel);
//		p->vel = (VectorRandom() + VectorRandom()).Normalize()*rnd_vel;
		p->color = color + (short)RANDOM_LONG(0, color_range-1);
		gEngfuncs.pEfxAPI->R_GetPackedColor(&p->packedColor, p->color);
		p->die += life;
	}
}

//-----------------------------------------------------------------------------
// Purpose: smoke trail spawner callback function
//-----------------------------------------------------------------------------
void TrailCallback(struct tempent_s *ent, float frametime, float currenttime)
{
	if (currenttime < ent->entity.baseline.fuser1)
		return;

	// FIX: temp entity disappears when it hits the sky
	if (ent->entity.origin == ent->entity.attachment[0])
		ent->die = currenttime;
	else
	{
		VectorCopy(ent->entity.origin, ent->entity.attachment[0]);
/*
looks kinda lame
		if (ent->entity.baseline.iuser1)
			gEngfuncs.pEfxAPI->R_SparkStreaks(ent->entity.origin, 24, -100, 100);
	//		gEngfuncs.pEfxAPI->R_RocketFlare(ent->entity.origin);
		ent->entity.baseline.iuser1 = !ent->entity.baseline.iuser1;// every second frame
		*/
	}
}

//-----------------------------------------------------------------------------
// Purpose: particle smoke trail
//			type:
// 0 rocket trail
// 1 smoke
// 2 blood
// 3 tracer
// 4 slight blood
// 5 tracer (same as 3)
// 6 voor trail
//-----------------------------------------------------------------------------
TEMPENTITY *FX_Trail(vec3_t origin, int entindex, unsigned short type, float life)
{
	TEMPENTITY *pTrailSpawner = NULL;
	pTrailSpawner = gEngfuncs.pEfxAPI->CL_TempEntAllocNoModel(origin);
	if (pTrailSpawner != NULL)
	{
		pTrailSpawner->flags |= (FTENT_PLYRATTACHMENT | FTENT_NOMODEL | FTENT_CLIENTCUSTOM | FTENT_SMOKETRAIL/* | FTENT_COLLIDEWORLD*/);
		pTrailSpawner->callback = TrailCallback;
		pTrailSpawner->clientIndex = entindex;
		pTrailSpawner->entity.trivial_accept = type;
		pTrailSpawner->die = gEngfuncs.GetClientTime() + life;
		pTrailSpawner->entity.baseline.fuser1 = gEngfuncs.GetClientTime() + 0.1f;
	}
	return pTrailSpawner;
}

//-----------------------------------------------------------------------------
// Purpose: Spark effect spawner per-frame callback function
// Input  : *ent - spark shower emitter
//			frametime - 
//			currenttime - 
//-----------------------------------------------------------------------------
void SparkShowerCallback(struct tempent_s *ent, float frametime, float currenttime)
{
	if (ent->entity.curstate.renderamt > 1)
	{
		if (ent->entity.curstate.renderamt > 48 && frametime > 0.0f)
		{
			if (!UTIL_PointIsFar(ent->entity.origin, 1.0))// XDM3035c
				gEngfuncs.pEfxAPI->R_SparkEffect(ent->entity.origin, 2, -128, 128);
		}
		ent->entity.curstate.renderamt -= 2;
	}
	else
	{
		ent->die = currenttime;
		gEngfuncs.pEfxAPI->R_BeamKill(ent->entity.index);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Spark effect spawner per-collision callback function
// curstate.health prevents early collisions (inside the explosion)
// curstate.fuser1 prevents often collisions
// Input  : *ent - spark shower emitter
//			*ptr - impact trace
//-----------------------------------------------------------------------------
void SparkShowerHitCallback(struct tempent_s *ent, struct pmtrace_s *ptr)
{
	ent->entity.curstate.health++;
	if (ent->entity.curstate.health > 1 && (gHUD.m_flTime - ent->entity.curstate.fuser1) > 0.25f)
	{
		if (!UTIL_PointIsFar(ent->entity.origin, 1.0))
		{
			gEngfuncs.pEfxAPI->R_BulletImpactParticles(ent->entity.origin);
			DecalTrace(RANDOM_LONG(DECAL_SMALLSCORCH1, DECAL_SMALLSCORCH3), ptr);
		}
	}
	ent->entity.curstate.fuser1 = gHUD.m_flTime;// last impact time
}

//-----------------------------------------------------------------------------
// Purpose: Client-side spark_shower entity replacement
// Input  : mdl_idx - sprite index
//			velocity - somewhere around 640 - 800
//-----------------------------------------------------------------------------
void FX_SparkShower(const Vector &origin, int mdl_idx, int count, const Vector &velocity, bool random)
{
	float fv = velocity.Length();
	for (int i = 0; i < count; ++i)
	{
		Vector vel;
		Vector dir;// = VectorRandom() + VectorRandom();
		VectorRandom(dir);

		if (random)
		{
			VectorScale(dir, fv, vel);// RANDOM_FLOAT(640, 800)
		}
		else
		{
			//VectorAdd(velocity, dir, vel);
			vel = velocity+dir;
		}
		TEMPENTITY *pEnt = gEngfuncs.pEfxAPI->R_TempModel(origin, vel, (float *)&g_vecZero, 1.0f, mdl_idx, TE_BOUNCE_NULL);
        if (pEnt != NULL)
        {
			pEnt->flags |= (/*FTENT_NOMODEL | */FTENT_ROTATE | FTENT_SLOWGRAVITY | FTENT_CLIENTCUSTOM);
			pEnt->hitcallback = SparkShowerHitCallback;// XDM3035c
			pEnt->callback = SparkShowerCallback;
			pEnt->entity.baseline.scale			= pEnt->entity.curstate.scale = 0.1;
			pEnt->entity.baseline.rendermode	= pEnt->entity.curstate.rendermode = kRenderTransAdd;
			pEnt->entity.baseline.renderamt		= pEnt->entity.curstate.renderamt = 255;
			pEnt->entity.curstate.health = 1;// XDM3035c: bounce counter
			pEnt->priority = TENTPRIORITY_LOW;
		}
	}
}

//-----------------------------------------------------------------------------
// Fixed smoke effect
//-----------------------------------------------------------------------------
TEMPENTITY *FX_Smoke(const Vector &origin, int spriteindex, float scale, float framerate)
{
//	origin[2] -= 12.0;
	TEMPENTITY *pTempEnt = gEngfuncs.pEfxAPI->R_DefaultSprite(origin, spriteindex, framerate);
	if (pTempEnt)
	{
		gEngfuncs.pEfxAPI->R_Sprite_Smoke(pTempEnt, scale);
		pTempEnt->entity.origin.z -= 12.0f;
//		pTempEnt->z -= 12.0f;
		pTempEnt->entity.baseline.origin[2] = 4.0;// HACK for velocity
	}
	return pTempEnt;
}

//-----------------------------------------------------------------------------
// Purpose: Internal function
// Input  : decalindex - Engine decal index for Draw_DecalIndex
//			*pTrace - 
//-----------------------------------------------------------------------------
void FX_DecalTrace(int decalindex, struct pmtrace_s *pTrace)
{
	if (pTrace == NULL || pTrace->allsolid)
		return;

	int entindex = gEngfuncs.pEventAPI->EV_IndexFromTrace(pTrace);
/* this is be done in the engine. probably.
	int modelindex = 0;
	cl_entity_t *pEnt = gEngfuncs.GetEntityByIndex(entindex);
	if (pEnt)
		modelindex = pEnt->curstate.modelindex;// wtf is this for?
*/
	gEngfuncs.pEfxAPI->R_DecalShoot(gEngfuncs.pEfxAPI->Draw_DecalIndex(decalindex), entindex, 0, pTrace->endpos, 0);
}

//-----------------------------------------------------------------------------
// Purpose: Internal function
// Input  : decalindex - Engine decal index for Draw_DecalIndex
//			&start - 
//			&end - 
//-----------------------------------------------------------------------------
void FX_DecalTrace(int decalindex, const Vector &start, const Vector &end)
{
	if (decalindex <= 0)
		return;

	pmtrace_t tr;
	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);
	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetSolidPlayers(-1);
	gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	gEngfuncs.pEventAPI->EV_PlayerTrace(start, end, PM_STUDIO_IGNORE, -1, &tr);
	gEngfuncs.pEventAPI->EV_PopPMStates();
//	if (tr.allsolid)
//		return;

	FX_DecalTrace(decalindex, &tr);
//	gEngfuncs.pEfxAPI->R_DecalShoot(textureindex, gEngfuncs.pEventAPI->EV_IndexFromTrace(&tr), 0, tr.endpos, 0);
}

//-----------------------------------------------------------------------------
// Purpose: Simplified, uses existing trace line
// Input  : decal - Game DLL decal like DECAL_GUNSHOT1
//			*pTrace - trace to use
//-----------------------------------------------------------------------------
void DecalTrace(int decal, struct pmtrace_s *pTrace)
{
	if (decal < 0)
		return;
	FX_DecalTrace(g_Decals[decal].index, pTrace);
//	int texindex = gEngfuncs.pEfxAPI->Draw_DecalIndex(g_Decals[decal].index);
//	gEngfuncs.pEfxAPI->R_DecalShoot(gEngfuncs.pEfxAPI->Draw_DecalIndex(gEngfuncs.pEfxAPI->Draw_DecalIndexFromName(decalname)), gEngfuncs.pEventAPI->EV_IndexFromTrace(pTrace), 0, pTrace->endpos, 0);
//	gEngfuncs.pEfxAPI->R_DecalShoot(texindex, gEngfuncs.pEventAPI->EV_IndexFromTrace(pTrace), 0, pTrace->endpos, 0);
}

//-----------------------------------------------------------------------------
// Purpose: Simplified, uses existing trace line
// Input  : *decalname - find decal by name (SLOW!)
//			*pTrace - trace to use
//-----------------------------------------------------------------------------
void DecalTrace(char *decalname, struct pmtrace_s *pTrace)
{
	FX_DecalTrace(gEngfuncs.pEfxAPI->Draw_DecalIndexFromName(decalname), pTrace);
}

//-----------------------------------------------------------------------------
// Purpose: Draw decal on a surface 
// Input  : decal - Game DLL decal like DECAL_GUNSHOT1
//			&start - trace start
//			&end - trace end
//-----------------------------------------------------------------------------
void DecalTrace(int decal, const Vector &start, const Vector &end)
{
	FX_DecalTrace(g_Decals[decal].index, start, end);
}

//-----------------------------------------------------------------------------
// Purpose: Simplified, uses vectors to build a new trace line
// Input  : *decalname - find decal by name (SLOW!)
//			&start - trace start
//			&end - trace end
//-----------------------------------------------------------------------------
void DecalTrace(char *decalname, const Vector &start, const Vector &end)
{
	FX_DecalTrace(gEngfuncs.pEfxAPI->Draw_DecalIndexFromName(decalname), start, end);
//	gEngfuncs.pEfxAPI->R_DecalShoot(gEngfuncs.pEfxAPI->Draw_DecalIndex(gEngfuncs.pEfxAPI->Draw_DecalIndexFromName(decalname)), gEngfuncs.pEventAPI->EV_IndexFromTrace(&tr), 0, tr.endpos, 0);
}


// TODO: FX_Tracer

//-----------------------------------------------------------------------------
// R_StreakSplash replacement
//-----------------------------------------------------------------------------
int FX_StreakSplash(const Vector &pos, const Vector &dir, color24 color, int count, float velocity, bool gravity, bool clip, bool bounce)
{
	CParticleSystem *pSystem = NULL;
	pSystem = (CParticleSystem *)g_pRenderManager->AddSystem(new CPSSparks(count, pos, RANDOM_FLOAT(0.6, 0.9), RANDOM_FLOAT(0, 0.02), 0.0f, velocity, 1.0f, color.r,color.g,color.b, 1.0f, -1.5f, g_iModelIndexBeamsAll, kRenderTransAdd, RANDOM_FLOAT(1.0f, 1.5f)), RENDERSYSTEM_FLAG_SIMULTANEOUS|(clip?0:RENDERSYSTEM_FLAG_NOCLIP)|(bounce?RENDERSYSTEM_FLAG_ADDPHYSICS:RENDERSYSTEM_FLAG_CLIPREMOVE)|RENDERSYSTEM_FLAG_LOOPFRAMES|(gravity?RENDERSYSTEM_FLAG_ADDGRAVITY:0));

	if (pSystem)
		pSystem->m_fFrame = BLAST_SKIN_TRACER;

return 1;
}

/*
void PMFX_WaterSplash(playermove_t *pmove)
{
}

// footsteps, dust, etc.
void PMFX_Land(playermove_t *pmove)
{
}
*/

//-----------------------------------------------------------------------------
// Purpose: Draw muzzle flash sprite by normal sprite index
// Input  : &pos - 
//			entindex - entity to follow
//			sprite_index - 
//			scale - 
//-----------------------------------------------------------------------------
void FX_MuzzleFlashSprite(const Vector &pos, int entindex, short attachment, int sprite_index, int frame, float framerate, float scale, bool rotation)
{
	if (g_pRenderManager == NULL)
		return;

	CRSSprite *pSys = new CRSSprite(pos, g_vecZero, sprite_index, kRenderTransAdd, 255,255,255, 1.0f,0.0f, scale, 0.25f, framerate, 0.01f);
	if (pSys)
	{
		if (rotation)
			pSys->m_vecAngles.z = RANDOM_FLOAT(0.0f, 359.9f);
		pSys->m_iFrame = frame;
		pSys->m_iFollowAttachment = attachment;
		g_pRenderManager->AddSystem(pSys, (framerate == 0.0f?0:RENDERSYSTEM_FLAG_RANDOMFRAME)|RENDERSYSTEM_FLAG_NOCLIP, entindex, RENDERSYSTEM_FFLAG_ICNF_REMOVE|RENDERSYSTEM_FFLAG_ICNF_NODRAW);
	}
}

//================
//Ghoul [BB]
//Gun Barrel smoke
//================
void FX_GunSmoke(const Vector &pos, int entindex, short attachment, bool black, float scale)
{
	if (g_pRenderManager == NULL)
		return;

	int contents = gEngfuncs.PM_PointContents(pos, NULL);
	if (contents == CONTENTS_WATER || contents == CONTENTS_SLIME || contents == CONTENTS_LAVA)
	{
		FX_BubblesPoint(pos, VECTOR_CONE_15DEGREES, g_iModelIndexBubble, RANDOM_LONG(4,6), 25);
		return;
	}

	Vector vecForward;
	AngleVectors(pos, vecForward, NULL, NULL);

	if (black)
	{
		CPSBlastCone *pSysSmoke = new CPSBlastCone( RANDOM_LONG(4,6), RANDOM_FLOAT(40,50), pos, vecForward, Vector(0.2,0.2,0.5), scale, scale*3, 1,1,1, 0.15, -0.2, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_8, kRenderTransAlpha, 0.3);
		if (pSysSmoke)
		{
//			pSysSmoke->m_iFollowAttachment = attachment;
			g_pRenderManager->AddSystem(pSysSmoke, RENDERSYSTEM_FLAG_CLIPREMOVE|RENDERSYSTEM_FLAG_SIMULTANEOUS, -1, 0);
		}
	}
	else
	{
		CPSBlastCone *pSysSmoke = new CPSBlastCone( RANDOM_LONG(4,6), RANDOM_FLOAT(40,50), pos, vecForward, Vector(0.2,0.2,0.5), scale, scale*3, 128,128,128, 0.25, -0.3, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_7, kRenderTransAdd, 0.3);
		if (pSysSmoke)
		{
//			pSysSmoke->m_iFollowAttachment = attachment;
			g_pRenderManager->AddSystem(pSysSmoke, RENDERSYSTEM_FLAG_CLIPREMOVE|RENDERSYSTEM_FLAG_SIMULTANEOUS, -1, 0);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Called by CParticleSystem::InitializeParticle in the last place
// Input  : *pSystem - 
//			*pParticle - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool FX_Bubbles_OnInitializeParticle(CParticleSystem *pSystem, CParticle *pParticle, void *pData)
{
	Vector v;
	VectorRandom(v, *(Vector *)pData);// appear in a specified volume
	pParticle->m_vPos += v;

	pParticle->m_vVel.z += FX_GetBubbleSpeed();
	pParticle->m_vAccel = Vector(0.0f,0.0f,1.0f);
	pParticle->m_fSizeX *= RANDOM_FLOAT(0.1f, 2.0f);
	pParticle->m_fSizeY = pParticle->m_fSizeX;
	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Real bubbles
// Input  : &mins &maxs - initial volume where particles will be created
//			direction - (0,0,0) means omnidirectional
//			modelIndex - 
//			count - 
//			speed - 
//-----------------------------------------------------------------------------
void FX_Bubbles(const Vector &mins, const Vector &maxs, const Vector &direction, int modelIndex, int count, float speed)
{
	if (g_pRenderManager == NULL)
		return;
	
	static Vector BubblesHalfVolume = (maxs-mins) * 0.5f;

	Vector center = (mins+maxs)/2;
	CPSFlameCone *pBubbles = new CPSFlameCone(12, center, direction, VECTOR_CONE_5DEGREES, speed, modelIndex, kRenderTransAlpha, 1.0f, 0.0f, 0.75f, 0.0f, 0.0f);
	if (pBubbles)
	{
		if (direction.IsZero())//
			pBubbles->m_flRandomDir = true;

		pBubbles->DrawContentsAdd(CONTENTS_WATER);
		pBubbles->DrawContentsAdd(CONTENTS_SLIME);
//		pBubbles->DrawContentsAdd(CONTENTS_LAVA);
		pBubbles->m_OnInitializeParticle = FX_Bubbles_OnInitializeParticle;
		pBubbles->m_pOnInitializeParticleData = &BubblesHalfVolume;
		g_pRenderManager->AddSystem(pBubbles, RENDERSYSTEM_FLAG_SIMULTANEOUS|RENDERSYSTEM_FLAG_CLIPREMOVE|RENDERSYSTEM_FLAG_INCONTENTSONLY, -1);
	}
}


void FX_BubblesPoint(const Vector &center, const Vector &spread, int modelIndex, int count, float speed)
{
	if (g_pRenderManager == NULL)
		return;

//			CPSBubbles(maxParticles, type, Vector &vector1, Vector &vector2, velocity, int sprindex, int r_mode, a, adelta, scale, scaledelta, timetolive)
	g_pRenderManager->AddSystem(
		new CPSBubbles(count, BUBBLES_TYPE_POINT, center, spread, speed, modelIndex, kRenderTransAlpha, 1.0f, 0.0f, BUBBLE_SCALE, 0.0f, BUBBLE_LIFE),
		RENDERSYSTEM_FLAG_SIMULTANEOUS|RENDERSYSTEM_FLAG_CLIPREMOVE|RENDERSYSTEM_FLAG_INCONTENTSONLY, -1);
}

void FX_BubblesSphere(const Vector &center, float radius, int modelIndex, int count, float speed)
{
	if (g_pRenderManager == NULL)
		return;

//			CPSBubbles(maxParticles, type, Vector &vector1, Vector &vector2, velocity, int sprindex, int r_mode, a, adelta, scale, scaledelta, timetolive)
	g_pRenderManager->AddSystem(
		new CPSBubbles(count, BUBBLES_TYPE_SPHERE, center, Vector(0.0f,0.0f,radius), speed, modelIndex, kRenderTransAlpha, 1.0f, 0.0f, BUBBLE_SCALE, 0.0f, BUBBLE_LIFE),
		RENDERSYSTEM_FLAG_SIMULTANEOUS|RENDERSYSTEM_FLAG_CLIPREMOVE|RENDERSYSTEM_FLAG_INCONTENTSONLY, -1);
}

void FX_BubblesBox(const Vector &center, const Vector &halfbox, int modelIndex, int count, float speed)
{
	if (g_pRenderManager == NULL)
		return;

//			CPSBubbles(maxParticles, type, Vector &vector1, Vector &vector2, velocity, int sprindex, int r_mode, a, adelta, scale, scaledelta, timetolive)
	g_pRenderManager->AddSystem(
		new CPSBubbles(count, BUBBLES_TYPE_BOX, center, halfbox, speed, modelIndex, kRenderTransAlpha, 1.0f, 0.0f, BUBBLE_SCALE, 0.0f, BUBBLE_LIFE),
		RENDERSYSTEM_FLAG_SIMULTANEOUS|RENDERSYSTEM_FLAG_CLIPREMOVE|RENDERSYSTEM_FLAG_INCONTENTSONLY, -1);
}

void FX_BubblesLine(const Vector &start, const Vector &end, int modelIndex, int count, float speed)
{
	if (g_pRenderManager == NULL)
		return;

//			CPSBubbles(maxParticles, type, Vector &vector1, Vector &vector2, velocity, int sprindex, int r_mode, a, adelta, scale, scaledelta, timetolive)
	g_pRenderManager->AddSystem(
		new CPSBubbles(count, BUBBLES_TYPE_LINE, start, end, speed, modelIndex, kRenderTransAlpha, 1.0f, 0.0f, BUBBLE_SCALE, 0.0f, BUBBLE_LIFE),
		RENDERSYSTEM_FLAG_SIMULTANEOUS|RENDERSYSTEM_FLAG_CLIPREMOVE|RENDERSYSTEM_FLAG_INCONTENTSONLY, -1);
}
