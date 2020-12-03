#include "hud.h"
#include "cl_util.h"
#include "Particle.h"
#include "RenderManager.h"
#include "RenderSystem.h"
#include "ParticleSystem.h"
#include "PSDrips.h"
#include "pm_defs.h"
#include "event_api.h"
#include "triangleapi.h"
#include "studio_util.h"
#include "bsputil.h"

CPSDrips::CPSDrips(void)
{
	// Calling constructors directly is forbidden!
	ResetParameters();
}

CPSDrips::~CPSDrips(void)
{
	KillSystem();
}

//-----------------------------------------------------------------------------
// Purpose: Spawn drips from specified volume and send them in specified direction
// Input  : maxParticles - 
//			&origin - center of system (mins/maxs will be applied)
//			&mins &maxs - volume in which the particles will be created (relative)
//			&dir - direction AND speed as a vector length
//			sprindex - main particle texture
//			sprindex_splash - (optional) impact particle texture
//			r_mode - render mode
//			sizex sizey - particle size
//			scaledelta - 
//			timetolive - 
//-----------------------------------------------------------------------------
CPSDrips::CPSDrips(int maxParticles, const Vector &origin, const Vector &mins, const Vector &maxs, const Vector &dir, int sprindex, int sprhitwater, int sprhitground, int r_mode, float sizex, float sizey, float scaledelta, float sizehitground_delta, float timetolive)
{
	index = 0;// the only good place for this
	removenow = false;
	ResetParameters();
//	m_pTexture = NULL;
	if (!InitTexture(sprindex))
	{
		removenow = true;
		return;
	}
//	m_iSplashTexture = sprindex_splash;
	if (sprhitwater > 0)
	{
		m_pTextureHitWater = IEngineStudio.GetModelByIndex(sprhitwater);
		if (!m_pTextureHitWater || m_pTextureHitWater->type != mod_sprite)
			m_pTextureHitWater = NULL;
	}
	else
		m_pTextureHitWater = NULL;

	if (sprhitground > 0)
	{
		m_pTextureHitGround = IEngineStudio.GetModelByIndex(sprhitground);
		if (!m_pTextureHitGround || m_pTextureHitGround->type != mod_sprite)
			m_pTextureHitGround = NULL;
	}
	else
		m_pTextureHitGround = NULL;

	m_iMaxParticles = maxParticles;
	m_vecMinS = mins;
	m_vecMaxS = maxs;
	m_vecMinS[2] += 1.0f;

//	m_vecDirection = dir.Normalize();
//	m_fSpeed = dir.Length();
	m_vecOrigin = origin;
	m_vecDirection = dir;
	m_fSpeed = VectorNormalize(m_vecDirection);

	m_fScale = -0.1f;// XDM3035: this prevents CRenderSystem::InitializeSystem() from modifying sizes
	m_fScaleDelta = scaledelta;
	m_fScaleHitGroundDelta = sizehitground_delta;

	SetParticleSize(sizex, sizey);

//	m_pParticleList = NULL;
//	m_iFollowEntity = -1;
	m_iRenderMode = r_mode;

	if (timetolive <= 0.0f)
		m_fDieTime = -1;
	else
		m_fDieTime = gEngfuncs.GetClientTime() + timetolive;

//	CON_PRINTF("CPSDrips: size: %f %f\n", m_fSizeX, m_fSizeY);
	InitializeSystem();
}

//-----------------------------------------------------------------------------
// Purpose: Set default (external, public, non-system) values for all class variables.
//   Each derived class MUST call its ParentClass::ResetParameters()!
// DO NOT call any functions from here.
//-----------------------------------------------------------------------------
void CPSDrips::ResetParameters(void)
{
	CPSDrips::BaseClass::ResetParameters();
	m_fSpeed = 0.0f;
//	m_fSizeX = 1.0f;
//	m_fSizeY = 1.0f;
//	m_iSplashTexture = 0;
	m_pTextureHitWater = NULL;
	m_pTextureHitGround = NULL;
	VectorClear(m_vecMinS);
	VectorClear(m_vecMaxS);
}

//-----------------------------------------------------------------------------
// Purpose: Update system parameters along with time
//			DO NOT PERFORM ANY DRAWING HERE!
// Input  : &time - current client time
//			&elapsedTime - time elapsed since last frame
// Output : Returns true if needs to be removed
//-----------------------------------------------------------------------------
bool CPSDrips::Update(const float &time, const double &elapsedTime)
{
	if (m_fDieTime > 0 && m_fDieTime <= time)
		dying = true;

	if (dying && m_iNumParticles <= 0)
		return 1;

//	if (FollowEntity() == NULL)// entity not visible
//		return 0;

//	CON_DPRINTF(" .. CPSDrips::Update() (e %d)\n", m_iFollowEntity);
//	Emit(ceil((float)m_iMaxParticles*0.5f));
	Emit(max(1, m_iMaxParticles>>2));// 1/4

	int c = 0;
	pmtrace_t pmtrace;
	CParticle *curPart = NULL;

	if (!(m_iFlags & RENDERSYSTEM_FLAG_NOCLIP) && gHUD.m_iIntermission == 0)// XDM3035b: prevent exceptions during level change
	{
		gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);
		gEngfuncs.pEventAPI->EV_PushPMStates();// BUGBUG: do not call this during map loading process
		gEngfuncs.pEventAPI->EV_SetSolidPlayers(-1);
		gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	}
	for (int i = 0; i < m_iNumParticles; ++i)
	{
		curPart = &m_pParticleList[i];

		if (curPart->m_fEnergy <= 0.0f)
			m_pParticleList[i] = m_pParticleList[--m_iNumParticles];

		if (curPart->m_iFlags != 0)// & (PARTICLE_FLAG1|PARTICLE_FLAG2))// splash/water circle
		{
			curPart->m_fEnergy -= (float)(1.5 * elapsedTime);
			if (m_iFlags & RENDERSYSTEM_FLAG_RANDOMFRAME)
			{
				curPart->FrameRandomize();
			}
			else
			{
				curPart->FrameIncrease();
				if (curPart->m_iFlags == PARTICLE_FLAG2)// splash sprites play only once
				{
					if (curPart->m_iFrame == 0)// the frame is zero AFTER it should've been increased, it means loop
						curPart->m_fEnergy = -1.0f;// remove it
				}
			}
		}
		else// falling drips
		{
			curPart->m_vPosPrev = curPart->m_vPos;

			// BUGBUG TODO FIXME T_T  do something with this!!!
			curPart->m_vVel += elapsedTime*curPart->m_vAccel;
			curPart->m_vPos += elapsedTime*curPart->m_vVel;
//			VectorMA(curPart->m_vVel, elapsedTime, curPart->m_vAccel, curPart->m_vVel);
//			VectorMA(curPart->m_vPos, elapsedTime, curPart->m_vVel, curPart->m_vPos);
//works LAGLAG			VectorMA(curPart->m_vPos, elapsedTime, curPart->m_vVelAdd, curPart->m_vPos);
//works			VectorScale(curPart->m_vVelAdd, 1.0f-elapsedTime*2.0f, curPart->m_vVelAdd);

//			VectorMA(curPart->m_vAccel, elapsedTime*-0.1, curPart->m_vAccel, curPart->m_vAccel);

			gEngfuncs.pEventAPI->EV_PlayerTrace(curPart->m_vPosPrev, curPart->m_vPos, ((m_iFlags & RENDERSYSTEM_FLAG_ADDPHYSICS)?PM_STUDIO_BOX:PM_STUDIO_IGNORE), -1, &pmtrace);
//			if (!pmtrace.inwater)
//			Vector vNext = curPart->m_vPos + curPart->m_vVel*elapsedTime;
				c = gEngfuncs.PM_PointContents(curPart->m_vPos, NULL);

			if (/*pmtrace.inwater || */(c < CONTENTS_SOLID && c > CONTENTS_SKY))
			{
//				CON_DPRINTF("in water\n");
				VectorClear(curPart->m_vVel);
				if (m_pTextureHitWater && PointIsVisible(curPart->m_vPos))// slow?
				{
					curPart->m_pTexture = m_pTextureHitWater;//IEngineStudio.GetModelByIndex(m_iSplashTexture);
					curPart->m_fEnergy = 1.0f;
					curPart->SetSizeFromTexture(m_fScale, m_fScale);// slow?
					curPart->m_fSizeDelta = SPLASH_SIZE_DELTA;
					curPart->m_iFlags = PARTICLE_FLAG1;
					curPart->m_iFrame = 0;
#ifdef DRIPSPARALLELTEST// testing: circles parallel to surface
					Vector normal;//, angles;
					VectorCopy(pmtrace.plane.normal, normal);
					normal[0] *= -1.0f;
					normal[1] *= -1.0f;
					VectorAngles(normal, curPart->m_vVel);// angles
#endif// unused since there is only horizontal water in Half-Life
				}
				else// just remove
				{
					curPart->m_fEnergy = -1.0f;
//					curPart->m_fSizeDelta = 0.0f;
					continue;
				}
			}
			else if (pmtrace.fraction != 1.0f || c == CONTENTS_SOLID)// && !pmtrace.startsolid)// XDM3035c: allow to start in solid area?.. XDM3037: don't.
			{
				VectorClear(curPart->m_vVel);
				curPart->m_fSizeDelta = 0.0f;
				if (!pmtrace.allsolid && !pmtrace.inwater && c != CONTENTS_EMPTY && c != CONTENTS_SKY && m_pTextureHitGround)// bad && PointIsVisible(curPart->m_vPos))// slow?
				{
					curPart->m_pTexture = m_pTextureHitGround;//IEngineStudio.GetModelByIndex(m_iSplashTexture);
					curPart->m_fEnergy = 1.0f;
					curPart->SetSizeFromTexture(m_fScale, m_fScale);
					curPart->m_fSizeDelta = m_fScaleHitGroundDelta;
					curPart->m_iFlags = PARTICLE_FLAG2;
					curPart->m_iFrame = 0;
					curPart->m_vPos = curPart->m_vPosPrev;// revert back a little
				}
				else// just remove
				{
//				if (m_iFlags & RENDERSYSTEM_FLAG_CLIPREMOVE || c == CONTENTS_SOLID || !PointIsVisible(curPart->m_vPos))
					curPart->m_fEnergy = -1.0f;
				}
				continue;
			}

			if (m_iFlags & RENDERSYSTEM_FLAG_RANDOMFRAME)
				curPart->FrameRandomize();
			else
				curPart->FrameIncrease();
		}
		curPart->UpdateColor(elapsedTime);
		curPart->UpdateSize(elapsedTime);
		curPart->m_fColor[3] = curPart->m_fEnergy;
	}
	if (!(m_iFlags & RENDERSYSTEM_FLAG_NOCLIP) && gHUD.m_iIntermission == 0)
	{
		gEngfuncs.pEventAPI->EV_PopPMStates();
	}
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: A new particle has been created, initialize system-specific start
//			values for it.
// Input  : pParticle - particle
//-----------------------------------------------------------------------------
void CPSDrips::InitializeParticle(CParticle *pParticle)
{
	pParticle->m_pTexture = m_pTexture;
	pParticle->m_vPos[0] = m_vecOrigin[0] + RANDOM_FLOAT(m_vecMinS[0], m_vecMaxS[0]);
	pParticle->m_vPos[1] = m_vecOrigin[1] + RANDOM_FLOAT(m_vecMinS[1], m_vecMaxS[1]);
	pParticle->m_vPos[2] = m_vecOrigin[2] + RANDOM_FLOAT(m_vecMinS[2], m_vecMaxS[2]);
	pParticle->m_vPosPrev = pParticle->m_vPos;// m_vecOrigin;?
	pParticle->m_vAccel = m_vecDirection;
	pParticle->m_vVel = m_vecDirection * (m_fSpeed*RANDOM_FLOAT(0.9, 1.1));
//LAGLAG	VectorClear(pParticle->m_vVelAdd);

	pParticle->m_fSizeX = m_fSizeX*0.1f;//2.0f;
	pParticle->m_fSizeY = m_fSizeY*0.1f;//32.0f;
	pParticle->m_fSizeDelta = m_fScaleDelta;// XDM3035: 20110504
	pParticle->m_iFlags = 0;
	pParticle->m_iFrame = 0;
	pParticle->SetDefaultColor();

	if (m_iFlags & RENDERSYSTEM_FLAG_RANDOMFRAME)
		pParticle->FrameRandomize();
//	pParticle->m_weight = 0.0;
//	pParticle->m_weightDelta = 0.0;
}
/*
void CPSDrips::ApplyForce(const Vector &origin, const Vector &force, float radius, bool point)
{
	Vector delta;
	float d, f;
	int i;
	// particles closer to origin gets more velocity (radius-l)
	// partivles outside radius are not affected
	if (point)// random direction from origin
	{
		f = Length(force)*0.001;//*CVAR_GET_FLOAT("test1");
//		CON_PRINTF("CPSDrips: f = %f\n", f);
		for (i = 0; i < m_iNumParticles; ++i)
		{
			VectorSubtract(m_pParticleList[i].m_vPos, origin, delta);
			d = Length(delta);
			if (d <= radius)
			{
				// k = radius/d; this should normalize delta-vectors up to radius (I could use Normalize(), but it is SLOWER
// ADD to previous				VectorScale(delta, (radius/d)*(radius-d)*f, m_pParticleList[i].m_vVelAdd);
				VectorMA(m_pParticleList[i].m_vVelAdd, (radius/d)*(radius-d)*f, delta, m_pParticleList[i].m_vVelAdd);
//		debug		m_pParticleList[i].m_fColor[0]=1.0;
//				m_pParticleList[i].m_fColor[1]=0.1;
//				m_pParticleList[i].m_fColor[2]=0.1;
//				m_pParticleList[i].m_fColor[3]=1.0;
			}
		}
	}
	else
	{
		for (i = 0; i < m_iNumParticles; ++i)
		{
			VectorSubtract(m_pParticleList[i].m_vPos, origin, delta);
			d = Length(delta);
			if (d <= radius)
			{
				VectorMA(m_pParticleList[i].m_vVelAdd, (radius/d)*(radius-d), force, m_pParticleList[i].m_vVelAdd);
	//			VectorMA(m_pParticleList[i].m_vVelAdd, (radius-l)/radius, force, m_pParticleList[i].m_vVelAdd);
	//			VectorAdd(m_pParticleList[i].m_vVelAdd, force, m_pParticleList[i].m_vVelAdd);
	//			VectorCopy(m_pParticleList[i].m_vVelAdd, m_pParticleList[i].m_vAccel);
			}
		}
	}
}
*/

//-----------------------------------------------------------------------------
// Purpose: Render
//-----------------------------------------------------------------------------
void CPSDrips::Render(void)
{
// handled globally by manager	if (m_iFlags & RENDERSYSTEM_FLAG_NODRAW)
//		return;

	if (!Mod_CheckBoxInPVS(m_vecOrigin+m_vecMinS, m_vecOrigin+m_vecMaxS))// XDM3035c: fast way to check visiblility
		return;

	if (gHUD.m_iPaused <= 0)
	{
		m_vecAngles = g_vecViewAngles;
	}

	Vector v_up, v_right;
/*	if (m_iFlags & RENDERSYSTEM_FLAG_ZROTATION)// UNDONE: TODO: rotate around direction vector, not just Z axis
	{
//		m_vecAngles[0] = 0.0f;
//		m_vecAngles[2] = 0.0f;
		v_up = -m_vecDirection;// FIXME: reversed rain particles drawn upside down and facing wrong direction (temporarily fixed with TRI_NONE
	}
	else
		v_up = g_vecViewUp;// FIXME: reversed rain particles drawn upside down and facing wrong direction (temporarily fixed with TRI_NONE
*/
	v_up = -m_vecDirection;// FIXME: reversed rain particles drawn upside down and facing wrong direction (temporarily fixed with TRI_NONE
	v_right = g_vecViewRight;
//	AngleVectors(m_vecAngles, NULL, v_right, NULL);// TODO: this is common angles for all particles which is fast but not as nice as individual sprite-like rotation

	Vector v1, v2;// up, right for circle
	// horizontal
	v1[0] = 1.0f;
	v1[1] = 0.0f;
	v1[2] = 0.0f;
	v2[0] = 0.0f;
	v2[1] = 1.0f;
	v2[2] = 0.0f;

	Vector rx;// tmp for faster code
	Vector uy;
	Vector delta;
	CParticle *p = NULL;

	// We should draw rain as a single mesh (which is faster) but we can't do that because of different texture frames

	if (m_pTextureHitWater)// water splash: parallel to water surface
	{
		for (int i = 0; i < m_iNumParticles; ++i)
		{
			p = &m_pParticleList[i];
			if (p->m_iFlags == PARTICLE_FLAG1)// water circle
			{
				if (p->m_fEnergy <= 0.0f)
					continue;

//				if (!PointIsVisible(p->m_vPos))// faster? Can't perform check on system origin because it's a large brush entity
				if (UTIL_PointIsFar(p->m_vPos, 1.0))
					continue;

//#ifdef DRIPSPARALLELTEST// testing: circles parallel to surface
//				AngleVectors(p->m_vVel, NULL, v1, v2);
//#endif
				p->Render(v1, v2, m_iRenderMode, true);
			}
		}
	}

	if (m_pTextureHitGround)// ground splash: normal sprite
	{
		for (int i = 0; i < m_iNumParticles; ++i)
		{
			p = &m_pParticleList[i];
			if (p->m_iFlags == PARTICLE_FLAG2)// water circle
			{
				if (p->m_fEnergy <= 0.0f)
					continue;

//				if (!PointIsVisible(p->m_vPos))// faster? Can't perform check on system origin because it's a large brush entity
				if (UTIL_PointIsFar(p->m_vPos, 1.0))
					continue;

				p->Render(g_vecViewRight, g_vecViewUp, m_iRenderMode, false);
			}
		}
	}

	if (m_pTexture)// drips
	{
		for (int i = 0; i < m_iNumParticles; ++i)
		{
			p = &m_pParticleList[i];
			if (p->m_iFlags == 0)
			{
				if (p->m_fEnergy <= 0.0f)
					continue;

				if (!PointIsVisible(p->m_vPos))// faster? Can't perform check on system origin because it's a large brush entity
					continue;

				if (gEngfuncs.pTriAPI->SpriteTexture(p->m_pTexture, p->m_iFrame))
				{
					// We draw drips the way they are always rotated perpendicular to vector between particle and camera
					delta = (p->m_vPos - g_vecViewOrigin).Normalize();
					v_right = CrossProduct(delta, m_vecDirection).Normalize();
	//				float adiff = AngleBetweenVectors(g_vecViewForward, delta);
	//				AngleVectors(m_vecAngles + Vector(0.0f,0.0f,adiff), NULL, v_right, v_up);
					if (!(m_iFlags & RENDERSYSTEM_FLAG_ZROTATION))// rotate
					{
						v_up = CrossProduct(delta, v_right).Normalize();
					}
					rx = v_right * p->m_fSizeX;
					uy = v_up * p->m_fSizeY;
/* TEST
					gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
					gEngfuncs.pTriAPI->CullFace(TRI_NONE);
					gEngfuncs.pTriAPI->Begin(TRI_LINES);
					gEngfuncs.pTriAPI->Color4f(1.0f, 1.0f, 1.0f, 1.0f);
					gEngfuncs.pTriAPI->Brightness(1.0f);
					gEngfuncs.pTriAPI->TexCoord2f(0.0f, 0.0f);
					gEngfuncs.pTriAPI->Vertex3fv(p->m_vPosPrev);
					gEngfuncs.pTriAPI->TexCoord2f(0.0f, 1.0f);
					gEngfuncs.pTriAPI->Vertex3fv(p->m_vPos);
*/
					gEngfuncs.pTriAPI->RenderMode(m_iRenderMode);
					gEngfuncs.pTriAPI->CullFace(TRI_NONE);
					gEngfuncs.pTriAPI->Begin(TRI_QUADS);
					gEngfuncs.pTriAPI->Color4f(p->m_fColor[0], p->m_fColor[1], p->m_fColor[2], 255);//p->m_fColor[3]);
					gEngfuncs.pTriAPI->Brightness(p->m_fColor[3]);

					gEngfuncs.pTriAPI->TexCoord2f(1.0f, 0.0f);
					gEngfuncs.pTriAPI->Vertex3fv(p->m_vPos + rx + uy);// XDM3037: was m_vPosPrev
					gEngfuncs.pTriAPI->TexCoord2f(1.0f, 1.0f);
					gEngfuncs.pTriAPI->Vertex3fv(p->m_vPos + rx - uy);
					gEngfuncs.pTriAPI->TexCoord2f(0.0f, 1.0f);
					gEngfuncs.pTriAPI->Vertex3fv(p->m_vPos - rx - uy);
					gEngfuncs.pTriAPI->TexCoord2f(0.0f, 0.0f);
					gEngfuncs.pTriAPI->Vertex3fv(p->m_vPos - rx + uy);// was m_vPosPrev

					gEngfuncs.pTriAPI->End();
				}
//				p->Render(v_right, v_up, rendermode, true);
			}
		}
	}
	gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
}

//-----------------------------------------------------------------------------
// Purpose: Set relative particle size (texture must be set!)
// Input  : &sizex sizey - X Y
//-----------------------------------------------------------------------------
void CPSDrips::SetParticleSize(const float &sizex, const float &sizey)
{
	if (m_pTexture)
	{
		m_fSizeX = m_pTexture->maxs[1] - m_pTexture->mins[1];
		m_fSizeY = m_pTexture->maxs[2] - m_pTexture->mins[2];
		if (sizex > 0.0f)
			m_fSizeX *= sizex;
		if (sizey > 0.0f)
			m_fSizeY *= sizey;
	}
	else
	{
		m_fSizeX = sizex;
		m_fSizeY = sizey;
	}
}
