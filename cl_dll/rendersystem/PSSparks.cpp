#include "hud.h"
#include "cl_util.h"
#include "Particle.h"
#include "RenderManager.h"
#include "RenderSystem.h"
#include "ParticleSystem.h"
#include "PSSparks.h"
#include "pm_defs.h"
#include "event_api.h"
#include "triangleapi.h"
#include "studio_util.h"
#include "weapondef.h"

CPSSparks::CPSSparks(void)
{
	ResetParameters();
}

CPSSparks::~CPSSparks(void)
{
	KillSystem();
}

//-----------------------------------------------------------------------------
// Purpose: The one and only constructor
//  Use negative scale values to get real sprite dimensions multiplied by fabs(scale)
//  So many parameters here because this system ignores customization outside of constructor (all particles get initialized inside)
// Input  : maxParticles - 
//			origin - 
//			scalex, scaley - 
//			scaledelta - was always 0.5f
//			velocity - 
//			startenergy - constantly decreases (particles are removed when their energy reaches zero), was always 2.0f
//			r,g,b - rendercolor
//			a - alpha/brightness
//			adelta - alpha/brightness delta
//			sprindex - 
//			r_mode - render mode
//			timetolive - 
//-----------------------------------------------------------------------------
CPSSparks::CPSSparks(int maxParticles, const Vector &origin, float scalex, float scaley, float scaledelta, float velocity, float startenergy, byte r, byte g, byte b, float a, float adelta, int sprindex, int r_mode, float timetolive)
{
	index = 0;// the only good place for this
	removenow = false;
	ResetParameters();
//	m_pTexture = NULL;
	m_pParticleList = NULL;
	if (!InitTexture(sprindex))
	{
		removenow = true;
		return;
	}
	m_iMaxParticles = maxParticles;
	m_vecOrigin = origin;
	m_fEnergyStart = startenergy;// 2.0f

	m_color.r = r;
	m_color.g = g;
	m_color.b = b;
	m_fBrightness = a;// 1.0f
	m_fBrightnessDelta = adelta;// 0.0f
	m_fScale = 0.0f;
	m_fScaleDelta = scaledelta*0.01f;// HACK?
	m_fSizeX = scalex*0.1f;
	m_fSizeY = scaley*0.1f;

	if (m_fSizeX == 0.0f)
		m_fSizeX = 0.1f;

	if (m_fSizeY == 0.0f)
		m_fSizeY = 0.05f;

	if (velocity < 0.0f)
	{
		m_fVelocity = -velocity;
		m_bReversed = true;
	}
	else
	{
		m_fVelocity = velocity;
		m_bReversed = false;
	}

//	if (m_fVelocity == 0)
//		m_fVelocity = 56.0;

//	m_iFollowEntity = -1;

	m_iRenderMode = r_mode;

	if (timetolive < 0)// TESTME
		m_fDieTime = -1;
	else
		m_fDieTime = gEngfuncs.GetClientTime() + timetolive;

	InitializeSystem();
//DON'T! We need flags set BEFORE we call this	Emit(maxParticles);
//NO!	if (timetolive > 0)
//		dying = true;
}

//-----------------------------------------------------------------------------
// Purpose: Set default (external, public, non-system) values for all class variables.
//   Each derived class MUST call its ParentClass::ResetParameters()!
// DO NOT call any functions from here.
//-----------------------------------------------------------------------------
void CPSSparks::ResetParameters(void)
{
	CPSSparks::BaseClass::ResetParameters();
	m_fSizeX = 0.1f;
	m_fSizeY = 0.05f;
	m_fVelocity = 0.0f;
	m_bReversed = false;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &time - 
//			&elapsedTime - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CPSSparks::Update(const float &time, const double &elapsedTime)
{
	if (m_fDieTime > 0.0f && m_fDieTime <= time)// XDM3035b: how did I managed to miss this?!
		dying = true;// when true, Emit() will stop producing particles, and remaining ones will (hopefully) disappear

	if (dying && m_iNumParticles <= 0)
		return 1;

	FollowEntity();

	if (m_iFlags & RENDERSYSTEM_FLAG_SIMULTANEOUS)
	{
		Emit(m_iMaxParticles);
		dying = true;// XDM3035b: don't repeat after this shot
	}
	else if (m_fNextEmitTime <= time)
	{
		Emit(1);
		m_fNextEmitTime = time + 10.0f/(float)m_iMaxParticles;
	}

	CParticle *curPart = NULL;

	pmtrace_t pmtrace;
	if (!(m_iFlags & RENDERSYSTEM_FLAG_NOCLIP))
	{
		gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);
		gEngfuncs.pEventAPI->EV_PushPMStates();
		gEngfuncs.pEventAPI->EV_SetSolidPlayers(-1);
		gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	}
//	for (int i = 0; i < m_iNumParticles; ++i)
	m_iNumParticles = m_iMaxParticles;
	for (int i = 0; i < m_iMaxParticles; ++i)// TESTME
	{
		curPart = &m_pParticleList[i];

		if (curPart->m_fEnergy <= 0.0f)
		{
			--m_iNumParticles;
			continue;// TESTME
		}
//			m_pParticleList[i] = m_pParticleList[--m_iNumParticles];

//		VectorCopy(curPart->m_vPos, curPart->m_vPosPrev);
		curPart->m_vPosPrev = curPart->m_vPos;

//-1FPS		curPart->m_vVel = curPart->m_vVel + elapsedTime * curPart->m_vAccel;
		VectorMA(curPart->m_vVel, elapsedTime, curPart->m_vAccel, curPart->m_vVel);
//-1FPS		curPart->m_vPos = curPart->m_vPos + elapsedTime * curPart->m_vVel;
		VectorMA(curPart->m_vPos, elapsedTime, curPart->m_vVel, curPart->m_vPos);

		if (m_iDrawContents != 0 && (m_iFlags & RENDERSYSTEM_FLAG_INCONTENTSONLY))
		{
			if (!DrawContentsHas(gEngfuncs.PM_PointContents(curPart->m_vPos, NULL)))
			{
				curPart->m_fEnergy = -1.0f;
				continue;
			}
		}

		if (!(m_iFlags & RENDERSYSTEM_FLAG_NOCLIP))
		{
			// TODO: don't collide with owner
			gEngfuncs.pEventAPI->EV_PlayerTrace(curPart->m_vPosPrev, curPart->m_vPos, PM_STUDIO_IGNORE/*PM_STUDIO_BOX*/, -1, &pmtrace);
			if (pmtrace.fraction != 1.0f)
			{
				if (m_iFlags & RENDERSYSTEM_FLAG_CLIPREMOVE)// remove particle
				{
					curPart->m_fEnergy = -1.0f;
					//--m_iNumParticles;
					continue;
				}
				else if (m_iFlags & RENDERSYSTEM_FLAG_ADDPHYSICS)// reflect particle velocity
				{
					float p = DotProduct(curPart->m_vVel, pmtrace.plane.normal);
					VectorMA(curPart->m_vVel, -2.0f*p, pmtrace.plane.normal, curPart->m_vVel);
					curPart->m_vVel = curPart->m_vVel * 0.8f;
					curPart->m_fColor[3] *= 0.8f;// XDM3035b: loose some energy
				}
			}
		}
		if (m_iFlags & RENDERSYSTEM_FLAG_RANDOMFRAME)
			curPart->FrameRandomize();
		else
			curPart->FrameIncrease();

		curPart->m_fEnergy -= (float)(1.5 * elapsedTime);
		curPart->UpdateColor(elapsedTime);
		curPart->UpdateSize(elapsedTime);
		curPart->UpdateEnergyByBrightness();

		if (m_OnUpdateParticle)
			m_OnUpdateParticle(this, curPart, m_pOnUpdateParticleData, time, elapsedTime);
	}
	if (!(m_iFlags & RENDERSYSTEM_FLAG_NOCLIP))
		gEngfuncs.pEventAPI->EV_PopPMStates();
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: A new particle has been created, initialize system-specific start
//			values for it.
// Input  : pParticle - particle
//-----------------------------------------------------------------------------
void CPSSparks::InitializeParticle(CParticle *pParticle)
{
	Vector rnd;// = VectorRandom() + VectorRandom();
	Vector rnd2;
	VectorRandom(rnd);
	VectorRandom(rnd2);
//	VectorAdd(rnd, rnd2, rnd);
	rnd += rnd2;

//	VectorNormalize(rnd);// too ideal
	if (m_bReversed)
	{
		VectorMA(m_vecOrigin, m_fVelocity, rnd, pParticle->m_vPos);
		VectorInverse(rnd);
//		VectorScale(rnd, -1.0, rnd);
	}
	else
		VectorCopy(m_vecOrigin, pParticle->m_vPos);
//		pParticle->m_vPos = m_vecOrigin;

//	pParticle->m_vPosPrev = pParticle->m_vPos;
	VectorCopy(pParticle->m_vPos, pParticle->m_vPosPrev);
//	pParticle->m_vVel = rnd*m_fVelocity;
	VectorScale(rnd, m_fVelocity, pParticle->m_vVel);

//test	pParticle->m_vAccel = Vector(0.0f, 0.0f, CVAR_GET_FLOAT("test1"));//-1.0f/* *g_cl_gravity*/);
	if (m_iFlags & RENDERSYSTEM_FLAG_ADDGRAVITY)//RENDERSYSTEM_FLAG_ADDPHYSICS)// XDM3035
	{
		pParticle->m_vAccel.x = 0.0f;
		pParticle->m_vAccel.y = 0.0f;
		pParticle->m_vAccel.z = -g_cl_gravity;
	}
	else
		VectorClear(pParticle->m_vAccel);

	pParticle->m_fEnergy = m_fEnergyStart;
	pParticle->m_fSizeX = fabs(m_fSizeX);
	pParticle->m_fSizeY = fabs(m_fSizeY);

	if (m_pTexture)
	{
		if (m_fSizeX <= 0.0f)// use sprite dimensions. FIXME: for now, absolute scale is 1:32
		{
//			CON_DPRINTF(">>> m_fSizeX <= 0!!!\n");
			pParticle->m_fSizeX *= (m_pTexture->maxs[1] - m_pTexture->mins[1])/3.20f;
		}

		if (m_fSizeY <= 0.0f)
		{
//			CON_DPRINTF(">>> m_fSizeY <= 0!!!\n");
			pParticle->m_fSizeY *= (m_pTexture->maxs[1] - m_pTexture->mins[1])/3.20f;
		}
	}

	pParticle->m_fSizeDelta = m_fScaleDelta;// 0.5f
	pParticle->m_pTexture = m_pTexture;
	pParticle->SetColor(m_color, m_fBrightness);
	pParticle->SetColorDelta(m_fColorDelta, m_fBrightnessDelta);

	if (m_iFlags & RENDERSYSTEM_FLAG_RANDOMFRAME)
		pParticle->FrameRandomize();

	if (m_OnInitializeParticle)
		m_OnInitializeParticle(this, pParticle, m_pOnInitializeParticleData);
}

//-----------------------------------------------------------------------------
// Purpose: Draw system to screen. May get called in various situations, so
// DON'T change any RS variables here (do it in Update() instead).
//-----------------------------------------------------------------------------
void CPSSparks::Render(void)
{
//	if (!PointIsVisible(m_vecOrigin))// Don't! System is too large!
//		return;

	if (!InitTexture(texindex))
		return;

	if (!gEngfuncs.pTriAPI->SpriteTexture(m_pTexture, (int)m_fFrame))
		return;

	if (gHUD.m_iPaused <= 0)
		m_vecAngles = g_vecViewAngles;

	Vector v_fwd;
	AngleVectors(m_vecAngles, v_fwd, NULL, NULL);

	gEngfuncs.pTriAPI->RenderMode(m_iRenderMode);
	gEngfuncs.pTriAPI->CullFace(TRI_NONE);
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);

	CParticle *curPart = NULL;
	Vector velocity, crossvel, backpoint;
	for (int i = 0; i < m_iNumParticles; ++i)
	{
		curPart = &m_pParticleList[i];

		if (curPart->m_fEnergy <= 0.0f)
			continue;

		if (!PointIsVisible(curPart->m_vPos))// faster?
			continue;

		curPart->m_iFrame = (int)m_fFrame;
//		VectorCopy(curPart->m_vVel, velocity);
		velocity = curPart->m_vVel;
		CrossProduct(velocity, v_fwd, crossvel);

		Vector vx = velocity*curPart->m_fSizeX;
		Vector cy = crossvel*curPart->m_fSizeY;
		gEngfuncs.pTriAPI->Color4f(curPart->m_fColor[0], curPart->m_fColor[1], curPart->m_fColor[2], curPart->m_fColor[3]/* * curPart->m_fEnergy*/);
		gEngfuncs.pTriAPI->Brightness(curPart->m_fColor[3]);
//		gEngfuncs.pTriAPI->Brightness(curPart->m_fEnergy);
		gEngfuncs.pTriAPI->TexCoord2f(0.0f, 0.0f);
		gEngfuncs.pTriAPI->Vertex3fv(curPart->m_vPos - vx + cy);
		gEngfuncs.pTriAPI->TexCoord2f(0.0f, 1.0f);
		gEngfuncs.pTriAPI->Vertex3fv(curPart->m_vPos/*+ vx*/+ cy);
		gEngfuncs.pTriAPI->TexCoord2f(1.0f, 1.0f);
		gEngfuncs.pTriAPI->Vertex3fv(curPart->m_vPos/*+ vx*/- cy);
		gEngfuncs.pTriAPI->TexCoord2f(1.0f, 0.0f);
		gEngfuncs.pTriAPI->Vertex3fv(curPart->m_vPos - vx - cy);
	}
	gEngfuncs.pTriAPI->End();
	gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
}
