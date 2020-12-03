#include "hud.h"
#include "cl_util.h"
#include "Particle.h"
#include "RenderManager.h"
#include "RenderSystem.h"
#include "ParticleSystem.h"
#include "PSBlastCone.h"
#include "pm_defs.h"
#include "event_api.h"
#include "weapondef.h"

CPSBlastCone::CPSBlastCone(void)
{
	ResetParameters();
}

CPSBlastCone::~CPSBlastCone(void)
{
	KillSystem();
}

CPSBlastCone::CPSBlastCone(int maxParticles, float velocity, const Vector &origin, const Vector &direction, const Vector &spread, float scale, float scaledelta, byte r, byte g, byte b, float a, float adelta, int sprindex, bool animate, int frame, int r_mode, float timetolive)
{
	index = 0;
	removenow = false;
	ResetParameters();
	if (!InitTexture(sprindex))
	{
		removenow = true;
		return;
	}
	m_iMaxParticles = maxParticles;
	m_vecOrigin = origin;
	if (origin == direction)
	{
		VectorClear(m_vecDirection);
		m_flRandomDir = true;
	}
	else
	{
		m_vecDirection = direction;
		m_flRandomDir = false;
	}
	m_vecSpread = spread;
	m_fParticleVelocity = velocity;
	m_iRenderMode = r_mode;
	m_fBrightness = a;
	m_fBrightnessDelta = adelta;
	m_fScale = scale;
	m_fScaleDelta = scaledelta;
	m_color.r = r;
	m_color.g = g;
	m_color.b = b;
	m_fAnimatedSpr = animate;
	m_iFrame = frame;

	if (timetolive <= 0.0f)
		m_fDieTime = -1;
	else
		m_fDieTime = gEngfuncs.GetClientTime() + timetolive;

	InitializeSystem();
}

void CPSBlastCone::ResetParameters(void)
{
	CPSBlastCone::BaseClass::ResetParameters();
	VectorClear(m_vecSpread);
	m_flRandomDir = true;
	m_fParticleVelocity = 300.0f;
}

bool CPSBlastCone::Update(const float &time, const double &elapsedTime)
{
	if (m_fDieTime > 0.0f && m_fDieTime <= time)
		dying = true;

	if( m_fBrightness <= 0.0f)
		m_fBrightness = 0.0f;

	if (dying && m_iNumParticles <= 0)
		return 1;

	FollowEntity();

	if (m_iFlags & RENDERSYSTEM_FLAG_SIMULTANEOUS)
	{
		Emit(m_iMaxParticles);
	}
	else if (m_fNextEmitTime <= time)
	{
		Emit(1);
		m_fNextEmitTime = time + 1.0f/(float)m_iMaxParticles;
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

	for (int i = 0; i < m_iNumParticles; ++i)
	{
		curPart = &m_pParticleList[i];

		if (curPart->m_fEnergy <= 0.0f)
			m_pParticleList[i] = m_pParticleList[--m_iNumParticles];

		curPart->m_vPosPrev = curPart->m_vPos;
		curPart->m_vVel += elapsedTime*curPart->m_vAccel;
		curPart->m_vPos += elapsedTime*curPart->m_vVel;

		if (m_iDrawContents != 0 && (m_iFlags & RENDERSYSTEM_FLAG_INCONTENTSONLY))
		{
			if (!DrawContentsHas(gEngfuncs.PM_PointContents(curPart->m_vPos, NULL)))
			{
				curPart->m_fEnergy = -1.0f;
				continue;
			}
		}
/*
		if (!(m_iFlags & RENDERSYSTEM_FLAG_NOCLIP))
		{
			gEngfuncs.pEventAPI->EV_PlayerTrace(curPart->m_vPosPrev, curPart->m_vPos, PM_WORLD_ONLY, -1, &pmtrace);
			if (pmtrace.fraction != 1.0f)
			{
				if (m_iFlags & RENDERSYSTEM_FLAG_CLIPREMOVE)
				{
					curPart->m_fEnergy = -1.0f;
					//--m_iNumParticles;
					continue;
				}
			}
		}
*/
		if (!(m_iFlags & RENDERSYSTEM_FLAG_NOCLIP))
		{
			gEngfuncs.pEventAPI->EV_PlayerTrace(curPart->m_vPosPrev, curPart->m_vPos, PM_WORLD_ONLY, -1, &pmtrace);

			if (pmtrace.fraction != 1.0f)
			{
				if (m_iFlags & RENDERSYSTEM_FLAG_CLIPREMOVE)
				{
					curPart->m_fEnergy = -1.0f;
					continue;
				}
				else if (m_iFlags & RENDERSYSTEM_FLAG_ADDPHYSICS)
				{
					float p = DotProduct(curPart->m_vVel, pmtrace.plane.normal);
					VectorMA(curPart->m_vVel, -2.0f*p, pmtrace.plane.normal, curPart->m_vVel);
					curPart->m_vVel = curPart->m_vVel * 0.8f;
				}
			}	
		}

		if (m_fAnimatedSpr == FALSE)
		{
			curPart->m_iFrame = m_iFrame;
		}
		else if (m_fAnimatedSpr == TRUE)
		{
			if (m_iFlags & RENDERSYSTEM_FLAG_RANDOMFRAME)
				curPart->FrameRandomize();
			else
				curPart->FrameIncrease();
		}

		curPart->UpdateColor(elapsedTime);
		curPart->UpdateEnergyByBrightness();// XDM3035c
		curPart->UpdateSize(elapsedTime);

		if (m_OnUpdateParticle)
			m_OnUpdateParticle(this, curPart, m_pOnUpdateParticleData, time, elapsedTime);
	}
	if (!(m_iFlags & RENDERSYSTEM_FLAG_NOCLIP))
		gEngfuncs.pEventAPI->EV_PopPMStates();

	if (m_iFlags & RENDERSYSTEM_FLAG_SIMULTANEOUS)// XDM3035c: TESTME
		if (m_iNumParticles <= 0)
			return 1;

	return 0;
}

void CPSBlastCone::InitializeParticle(CParticle *pParticle)
{
	pParticle->m_vPos = m_vecOrigin;
	pParticle->m_vPosPrev = m_vecOrigin;

	VectorClear(pParticle->m_vAccel);
	VectorRandom(pParticle->m_vVel);
	Vector rnd2;
	VectorRandom(rnd2);
	VectorAdd(pParticle->m_vVel, rnd2, pParticle->m_vVel);

	if (m_flRandomDir)
	{
		VectorNormalize(pParticle->m_vVel);
	}
	else
	{
		pParticle->m_vVel[0] *= m_vecSpread[0];
		pParticle->m_vVel[1] *= m_vecSpread[1];
		pParticle->m_vVel[2] *= m_vecSpread[2];
		VectorAdd(pParticle->m_vVel, m_vecDirection, pParticle->m_vVel);
	}

	if (m_iFlags & RENDERSYSTEM_FLAG_ADDGRAVITY)
		pParticle->m_vAccel.z = -g_cl_gravity/2;

	pParticle->m_vVel = pParticle->m_vVel * m_fParticleVelocity;
	pParticle->m_fSizeY = pParticle->m_fSizeX = m_fScale*RANDOM_FLOAT(0.85,1.15);
	pParticle->m_fSizeDelta = m_fScaleDelta;
	pParticle->m_pTexture = m_pTexture;
	pParticle->SetColor(m_color, m_fBrightness);
	pParticle->SetColorDelta(m_fColorDelta, m_fBrightnessDelta);

	if (m_iFlags & RENDERSYSTEM_FLAG_RANDOMFRAME)
		pParticle->FrameRandomize();

	if (m_OnInitializeParticle)
		m_OnInitializeParticle(this, pParticle, m_pOnInitializeParticleData);
}
