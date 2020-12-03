#include "hud.h"
#include "cl_util.h"
#include "Particle.h"
#include "RenderManager.h"
#include "RenderSystem.h"
#include "ParticleSystem.h"
#include "pm_defs.h"
#include "event_api.h"
#include "triangleapi.h"

//-----------------------------------------------------------------------------
// Purpose: Default constructor. Should never be used.
//-----------------------------------------------------------------------------
CParticleSystem::CParticleSystem(void)
{
	// Calling constructors directly is forbidden!
	ResetParameters();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor. Used for memory cleaning. Destroy all data here.
//-----------------------------------------------------------------------------
CParticleSystem::~CParticleSystem(void)
{
	KillSystem();
}

//-----------------------------------------------------------------------------
// Purpose: Default particle system
// Input  : maxParticles - 
//			origin - 
//			direction - 
//			sprindex - 
//			r_mode - 
//			timetolive - 
//-----------------------------------------------------------------------------
CParticleSystem::CParticleSystem(int maxParticles, const Vector &origin, const Vector &direction, int sprindex, int r_mode, float timetolive)
{
	index = 0;// the only good place for this
	removenow = false;
	// Calling constructors directly is forbidden!
	ResetParameters();
//	m_pTexture = NULL;
	if (!InitTexture(sprindex))
	{
		removenow = true;
		return;
	}
//	m_pParticleList = NULL;
	m_iMaxParticles = maxParticles;
	m_vecOrigin = origin;
	m_vecDirection = direction;
	m_iRenderMode = r_mode;
//	m_iFollowEntity = -1;
//	m_fBrightness = 1.0f;
//	m_fBrightnessDelta = 0.0f;
	m_fScale = 4.0f;
//	m_fScaleDelta = 0.0f;

	if (timetolive < 0)
		m_fDieTime = -1;
	else
		m_fDieTime = gEngfuncs.GetClientTime() + timetolive;

	InitializeSystem();
//DON'T! We need flags set BEFORE we call this	Emit(maxParticles);
//	dying = true;
}

//-----------------------------------------------------------------------------
// Purpose: Set default (external, non-system) values for all class variables.
//   Each derived class MUST call its ParentClass::ResetParameters()!
//-----------------------------------------------------------------------------
void CParticleSystem::ResetParameters(void)
{
	CRenderSystem::ResetParameters();
	m_pParticleList = NULL;// NEW UPD 2007.10.11 // dangerous!
	VectorClear(m_vecDirection);
	m_fNextEmitTime = 0.0f;
	m_iMaxParticles = 0;
	m_iNumParticles = 0;
	m_iAccumulatedEmit = 0;
	m_fEnergyStart = 1.0f;
	m_OnInitializeParticle = NULL;
	m_OnUpdateParticle = NULL;
	m_pOnInitializeParticleData = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Initialize SYSTEM (non-user) startup variables.
// Must be called from class constructor.
//-----------------------------------------------------------------------------
void CParticleSystem::InitializeSystem(void)
{
	if (m_pParticleList != NULL)// 0x00000001
	{
		delete [] m_pParticleList;// Access Violation.
		m_pParticleList = NULL;
	}

	ASSERT(m_iMaxParticles > 0);
	m_iNumParticles = 0;
	m_iAccumulatedEmit = 0;
	m_fNextEmitTime = 0.0f;

	try
	{
		m_pParticleList = new CParticle[m_iMaxParticles];
	}
	catch(...)
	{
		CON_DPRINTF("memory allocation error\n");
		removenow = true;
		return;
	}

	CRenderSystem::InitializeSystem();
}

//-----------------------------------------------------------------------------
// Purpose: Clear-out and free dynamically allocated memory
//-----------------------------------------------------------------------------
void CParticleSystem::KillSystem(void)
{
	if (m_pParticleList != NULL)
	{
		delete [] m_pParticleList;
		m_pParticleList = NULL;
	}
	m_iMaxParticles = 0;
	m_iNumParticles = 0;
	m_iAccumulatedEmit = 0;

	CRenderSystem::KillSystem();
}

//-----------------------------------------------------------------------------
// Purpose: Reset or recreate particles. It's the tricky part to not to release
//			all particles at the same time. Calls Allocate, InitializeParticle
// Warning: Must NOT be called from constructor because it depends on flags!
// Input  : numParticles - 
// Output : int
//-----------------------------------------------------------------------------
int CParticleSystem::Emit(const int &numParticles)
{
	if (dying)
		return 0;

/*	numParticles += m_iAccumulatedEmit;

	// TODO: revisit
	while ((numParticles >= 1) && (m_iNumParticles < m_iMaxParticles))
	{
		InitializeParticle(m_iNumParticles);
		++m_iNumParticles;
		--numParticles;
	}

	m_iAccumulatedEmit = numParticles;
	return numParticles;
*/
	m_iAccumulatedEmit += numParticles;// XDM3035: optimizations

	int iNumCreated = 0;
	while ((m_iAccumulatedEmit >= 1) && (m_iNumParticles < m_iMaxParticles))
	{
		CParticle *pParticle = AllocateParticle();
		if (pParticle)
		{
			pParticle->m_fEnergy = 1.0f;
			InitializeParticle(pParticle);
			--m_iAccumulatedEmit;
			++iNumCreated;
		}
		else// no more particles will be created anyway
		{
			CON_DPRINTF("CParticleSystem::Emit(%d) cannot allocate any particles! AccumulatedEmit = %d\n", numParticles, m_iAccumulatedEmit);
			break;
		}
	}
	// TESTME
	if (m_iFlags & RENDERSYSTEM_FLAG_SIMULTANEOUS)
	{
		if (iNumCreated == 0)//(allparticleswereused)
		{
			dying = 1;
//			return 0;
		}
	}

	return iNumCreated;
}

//-----------------------------------------------------------------------------
// Purpose: One more layer for advanced use in cases when Emit() won't do.
// Does not really allocate any memory, just uses another free particle slot.
// Output : CParticle
//-----------------------------------------------------------------------------
CParticle *CParticleSystem::AllocateParticle(void)
{
	CParticle *pParticle = NULL;
	if (m_iNumParticles < m_iMaxParticles)
	{
		pParticle = &m_pParticleList[m_iNumParticles];
		pParticle->index = m_iNumParticles;
//		InitializeParticle(m_iNumParticles);
		++m_iNumParticles;
//		return &m_pParticleList[m_iNumParticles-1];
	}
	return pParticle;
}

//-----------------------------------------------------------------------------
// Purpose: Update system parameters along with time
//			DO NOT PERFORM ANY DRAWING HERE!
// Input  : &time - current client time
//			&elapsedTime - time elapsed since last frame
// Output : Returns true if needs to be removed
//-----------------------------------------------------------------------------
bool CParticleSystem::Update(const float &time, const double &elapsedTime)
{
	if (m_fDieTime > 0.0f && m_fDieTime <= time)
		dying = true;

	if (dying && m_iNumParticles <= 0)
		return 1;

	FollowEntity();
	Emit(1);

	pmtrace_t pmtrace;
	if (m_iFlags & RENDERSYSTEM_FLAG_ADDPHYSICS)// XDM3035
	{
		gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);
		gEngfuncs.pEventAPI->EV_PushPMStates();
		gEngfuncs.pEventAPI->EV_SetSolidPlayers(-1);
		gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	}
	CParticle *curPart;
	for (int i = 0; i < m_iNumParticles; ++i)
	{
		curPart = &m_pParticleList[i];

		if (curPart->m_fEnergy <= 0.0f)
			continue;
//			m_pParticleList[i] = m_pParticleList[--m_iNumParticles];

		if (m_iDrawContents != 0 && (m_iFlags & RENDERSYSTEM_FLAG_INCONTENTSONLY))
		{
			if (!DrawContentsHas(gEngfuncs.PM_PointContents(curPart->m_vPos, NULL)))
			{
				curPart->m_fEnergy = -1.0f;
				continue;
			}
		}

//		curPart->m_texture = m_pTexture;
		curPart->m_vPosPrev = curPart->m_vPos;
		curPart->m_vVel += elapsedTime*curPart->m_vAccel;
		curPart->m_vPos += elapsedTime*curPart->m_vVel;

		if (m_iFlags & RENDERSYSTEM_FLAG_ADDPHYSICS)
		{
			gEngfuncs.pEventAPI->EV_PlayerTrace(curPart->m_vPosPrev, curPart->m_vPos, (m_iFlags & RENDERSYSTEM_FLAG_NOCLIP)?PM_WORLD_ONLY:PM_STUDIO_BOX, -1, &pmtrace);
			if (pmtrace.fraction != 1.0f)
			{
				if (m_iFlags & RENDERSYSTEM_FLAG_CLIPREMOVE)
				{
					curPart->m_fEnergy = -1.0f;
				}
				else
				{
					float p = DotProduct(curPart->m_vVel, pmtrace.plane.normal);
					curPart->m_vVel -= (2.0f*p)*pmtrace.plane.normal;
//					VectorMA(curPart->m_vVel, -2.0f*p, pmtrace.plane.normal, curPart->m_vVel);
					curPart->m_vVel *= 0.8f;
				}
			}
/*already included in acceleration			else
			{
				curPart->m_vPos.z -= elapsedTime * g_cl_gravity;
			}*/
		}

		if (m_iFlags & RENDERSYSTEM_FLAG_RANDOMFRAME)
			curPart->FrameRandomize();
		else
			curPart->FrameIncrease();

//		curPart->m_fEnergy -= 0.5 * elapsedTime;
		curPart->UpdateColor(elapsedTime);
		curPart->UpdateSize(elapsedTime);
//		curPart->m_fColor[3] = curPart->m_fEnergy;
		curPart->UpdateEnergyByBrightness();

		if (m_OnUpdateParticle)
			m_OnUpdateParticle(this, curPart, m_pOnUpdateParticleData, time, elapsedTime);
	}
	if (m_iFlags & RENDERSYSTEM_FLAG_ADDPHYSICS)
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
void CParticleSystem::InitializeParticle(CParticle *pParticle)
{
	// TODO: relative coordinates
	pParticle->m_vPos = m_vecOrigin;
	pParticle->m_vPosPrev = m_vecOrigin;
	pParticle->m_vVel = m_vecDirection;

	if (m_iFlags & RENDERSYSTEM_FLAG_ADDGRAVITY)//RENDERSYSTEM_FLAG_ADDPHYSICS)// XDM3035
	{
		pParticle->m_vAccel.x = 0.0f;
		pParticle->m_vAccel.y = 0.0f;
		pParticle->m_vAccel.z = -g_cl_gravity;
	}
	else
		VectorClear(pParticle->m_vAccel);

//	pParticle->m_vecAngles
	pParticle->m_fEnergy = m_fEnergyStart;
	pParticle->m_fSizeX = m_fScale;
	pParticle->m_fSizeY = m_fScale;
	pParticle->m_fSizeDelta = m_fScaleDelta;
	pParticle->m_pTexture = m_pTexture;

	pParticle->SetColor(m_color, m_fBrightness);
	pParticle->SetColorDelta(m_fColorDelta, m_fBrightnessDelta);

/*	pParticle->SetDefaultColor();
	pParticle->m_fColor[3] = m_fBrightness;
	pParticle->m_fColorDelta[3] = m_fBrightnessDelta;
*/
//	pParticle->m_weight = 0.0;
//	pParticle->m_weightDelta = 0.0;
	pParticle->m_iFlags = 0;

	if (m_iFlags & RENDERSYSTEM_FLAG_RANDOMFRAME)
		pParticle->FrameRandomize();
	else
		pParticle->m_iFrame = 0;

	if (m_OnInitializeParticle)
		m_OnInitializeParticle(this, pParticle, m_pOnInitializeParticleData);
}

//-----------------------------------------------------------------------------
// Purpose: extremely useful for explosion and wind effects
// Input  : origin - 
//			force - 
//			radius - 
//			point - 
//-----------------------------------------------------------------------------
void CParticleSystem::ApplyForce(const Vector &origin, const Vector &force, float radius, bool point)
{
/*UNDONE	for (int i = 0; i < m_iNumParticles; ++i)
	{
		float dist = Length(m_pParticleList[i].m_vPos - origin);
		if (dist < radius)
		{
			if (point)
				m_pParticleList[i].m_vAccel += force*((radius-dist)/radius);
			else
				m_pParticleList[i].m_vAccel += force;
		}
	}*/
}

//-----------------------------------------------------------------------------
// Purpose: Draw system to screen. May get called in various situations, so
// DON'T change any RS variables here (do it in Update() instead).
//-----------------------------------------------------------------------------
void CParticleSystem::Render(void)
{
// handled globally by manager	if (m_iFlags & RENDERSYSTEM_FLAG_NODRAW)
//		return;

	if (gHUD.m_iPaused <= 0)
	{
//		gEngfuncs.GetViewAngles(m_vecAngles);
//		VectorAdd(m_vecAngles, ev_punchangle, m_vecAngles);
		m_vecAngles = g_vecViewAngles;
	}

// particles may be far away from initial origin	if (!PointIsVisible(m_vecOrigin))
//		return;

	if (m_iFlags & RENDERSYSTEM_FLAG_ZROTATION)
	{
		m_vecAngles[0] = 0.0f;
		m_vecAngles[2] = 0.0f;
	}

	Vector v_up, v_right;
	AngleVectors(m_vecAngles, NULL, v_right, v_up);

	CParticle *p = NULL;
	for (int i = 0; i < m_iNumParticles; ++i)
	{
		p = &m_pParticleList[i];
		if (p->m_fEnergy <= 0.0f)
			continue;

		if (!PointIsVisible(p->m_vPos))// faster?
			continue;

		if (m_iDrawContents != 0)
		{
			if (!DrawContentsHas(gEngfuncs.PM_PointContents(p->m_vPos, NULL)))
				continue;
		}
		p->Render(v_right, v_up, m_iRenderMode);
	}
	gEngfuncs.pTriAPI->RenderMode(kRenderNormal);// ?
}
