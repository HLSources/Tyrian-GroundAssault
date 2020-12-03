#include "hud.h"
#include "cl_util.h"
#include "Particle.h"
#include "RenderManager.h"
#include "RenderSystem.h"
#include "ParticleSystem.h"
#include "PSFlameCone.h"
#include "pm_defs.h"
#include "event_api.h"
#include "weapondef.h"

CPSFlameCone::CPSFlameCone(void)
{
	// Calling constructors directly is forbidden!
	ResetParameters();
}

CPSFlameCone::~CPSFlameCone(void)
{
	KillSystem();
}

CPSFlameCone::CPSFlameCone(int maxParticles, const Vector &origin, const Vector &direction, const Vector &spread, float velocity, int sprindex, int r_mode, float a, float adelta, float scale, float scaledelta, float timetolive)
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
	m_iMaxParticles = maxParticles;
	m_vecOrigin = origin;
//	m_pParticleList = NULL;
//	if (origin == direction)//
//	if (VectorCompare(origin, direction))
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
//	m_iFollowEntity = -1;
	m_fParticleVelocity = velocity;
	m_iRenderMode = r_mode;
	m_fBrightness = a;// 1.0f
	m_fBrightnessDelta = adelta;// 0.0f
	m_fScale = scale;
	m_fScaleDelta = scaledelta;

	if (timetolive <= 0.0f)
		m_fDieTime = -1;
	else
		m_fDieTime = gEngfuncs.GetClientTime() + timetolive;

	InitializeSystem();
}

//-----------------------------------------------------------------------------
// Purpose: Set default (external, public, non-system) values for all class variables.
//   Each derived class MUST call its ParentClass::ResetParameters()!
// DO NOT call any functions from here.
//-----------------------------------------------------------------------------
void CPSFlameCone::ResetParameters(void)
{
	CPSFlameCone::BaseClass::ResetParameters();
	VectorClear(m_vecSpread);
	m_flRandomDir = true;
	m_fParticleVelocity = 300.0f;
	m_fBrightnessDelta = -1.5f;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &time - 
//			&elapsedTime - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CPSFlameCone::Update(const float &time, const double &elapsedTime)
{
	if (m_fDieTime > 0.0f && m_fDieTime <= time)
		dying = true;// when true, Emit() will stop producing particles, and remaining ones will (hopefully) disappear

	if (dying && m_iNumParticles <= 0)
		return 1;

	/*cl_entity_t *pFollow = */FollowEntity();
/*	if (pFollow)
	{
		if (!m_flRandomDir)// XDM3035: TESTME
		{
			Vector forward;
			Vector vecEnd;
			AngleVectors(pFollow->angles, forward, NULL, NULL);
			VectorMA(m_vecOrigin, 24.0f, forward, vecEnd);// 24 units forward
			gEngfuncs.pEfxAPI->R_ParticleLine(m_vecOrigin, vecEnd, 0,0,255, 1.0f);
			m_vecDirection = vecEnd;
		}
	}*/


//	Emit(864 * elapsedTime);
	if (m_iFlags & RENDERSYSTEM_FLAG_SIMULTANEOUS)
	{
		Emit(m_iMaxParticles);
//		CON_PRINTF("Emit(%d) returned %d\n", m_iMaxParticles, Emit(m_iMaxParticles));
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

	// start values!
//	m_fBrightness += m_fBrightnessDelta*(float)elapsedTime;
//NO! we use this ONLY to initialize particles!	m_fScale += m_fScaleDelta*(float)elapsedTime;

	for (int i = 0; i < m_iNumParticles; ++i)
	{
		curPart = &m_pParticleList[i];

		if (curPart->m_fEnergy <= 0.0f)// this decreases overall particle count and moves the last one's pointer to current position
			m_pParticleList[i] = m_pParticleList[--m_iNumParticles];// UNSAFE?

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

		if (!(m_iFlags & RENDERSYSTEM_FLAG_NOCLIP))
		{
			/* TODO: don't collide with owner
			if (m_iFollowEntity > 0)
			{
				physent_t *pe = gEngfuncs.pEventAPI->EV_GetPhysent(pTrace->ent);
				if (pe && pe->info == m_iFollowEntity)
					continue;
			}*/
			gEngfuncs.pEventAPI->EV_PlayerTrace(curPart->m_vPosPrev, curPart->m_vPos, PM_STUDIO_IGNORE/*PM_STUDIO_BOX*/, -1, &pmtrace);
			if (pmtrace.fraction != 1.0f)
			{
				if (m_iFlags & RENDERSYSTEM_FLAG_CLIPREMOVE)
				{
					curPart->m_fEnergy = -1.0f;
					//--m_iNumParticles;
					continue;
				}
				else if (m_iFlags & RENDERSYSTEM_FLAG_ADDPHYSICS)// this system does not reflect
				{
//					TESTME
/*					float p = DotProduct(curPart->m_vVel, pmtrace.plane.normal);
reflection					VectorMA(curPart->m_vVel, -2.0f*p, pmtrace.plane.normal, curPart->m_vVel);
*/
					// actually we need to zero-out velocity component vector that is blocked by the obstacle, e.g.slide
					curPart->m_vVel = curPart->m_vVel * 0.8f;
					curPart->m_fColor[3] *= 0.8f;// XDM3035b: loose some energy
				}
			}
		}
		
		if (m_iFlags & RENDERSYSTEM_FLAG_RANDOMFRAME)
			curPart->FrameRandomize();
		else
			curPart->FrameIncrease();

//		curPart->m_fEnergy = m_fBrightness;//-= (float)(1.5 * elapsedTime);
		curPart->UpdateColor(elapsedTime);
		curPart->UpdateEnergyByBrightness();// XDM3035c
		curPart->UpdateSize(elapsedTime);
//		curPart->m_fColor[3] = max(0.0f, curPart->m_fEnergy);

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

//-----------------------------------------------------------------------------
// Purpose: A new particle has been created, initialize system-specific start
//			values for it.
// Input  : index - particle index in array
//-----------------------------------------------------------------------------
void CPSFlameCone::InitializeParticle(CParticle *pParticle)
{
	pParticle->m_vPos = m_vecOrigin;
	pParticle->m_vPosPrev = m_vecOrigin;

	VectorClear(pParticle->m_vAccel);
//	VectorAdd(VectorRandom(), VectorRandom(), pParticle->m_vVel);
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

	pParticle->m_vVel = pParticle->m_vVel * m_fParticleVelocity;
	pParticle->m_fSizeX = m_fScale*7.0f;// XDM: we have to keep ugly constants for compatibility :(
	pParticle->m_fSizeY = m_fScale*7.0f;
	pParticle->m_fSizeDelta = m_fScaleDelta;
	pParticle->m_pTexture = m_pTexture;
//	pParticle->SetDefaultColor();
	pParticle->SetColor(m_color, m_fBrightness);
	pParticle->SetColorDelta(m_fColorDelta, m_fBrightnessDelta);

	if (m_iFlags & RENDERSYSTEM_FLAG_RANDOMFRAME)
		pParticle->FrameRandomize();

	if (m_OnInitializeParticle)
		m_OnInitializeParticle(this, pParticle, m_pOnInitializeParticleData);
}
