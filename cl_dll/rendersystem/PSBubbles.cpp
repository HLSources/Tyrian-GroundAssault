#include "hud.h"
#include "cl_util.h"
#include "cl_fx.h"
#include "Particle.h"
#include "RenderManager.h"
#include "RenderSystem.h"
#include "ParticleSystem.h"
#include "PSFlameCone.h"
#include "PSBubbles.h"
#include "msg_fx.h"

CPSBubbles::CPSBubbles(void)
{
	// Calling constructors directly is forbidden!
	ResetParameters();
}

CPSBubbles::~CPSBubbles(void)
{
	KillSystem();
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
// Input  : maxParticles - 
//			type - BUBBLES_TYPE_POINT, BUBBLES_TYPE_SPHERE, BUBBLES_TYPE_BOX, BUBBLES_TYPE_LINE
//			&vector1 - 
//			&vector2 - 
//			velocity - scalar velocity for particles
//-----------------------------------------------------------------------------
CPSBubbles::CPSBubbles(int maxParticles, byte type, const Vector &vector1, const Vector &vector2, float velocity, int sprindex, int r_mode, float a, float adelta, float scale, float scaledelta, float timetolive)
{
	index = 0;// the only good place for this
	removenow = false;
	ResetParameters();
	if (maxParticles <= 0 || !InitTexture(sprindex))
	{
		removenow = true;
		return;
	}
	m_iMaxParticles = maxParticles;
	m_bType = type;
	m_vecOrigin = vector1;
	m_vecSpread = vector2;

	m_fParticleVelocity = velocity;
	m_iRenderMode = r_mode;
	m_fBrightness = a;
	m_fBrightnessDelta = adelta;
	m_fScale = scale;
	m_fScaleDelta = scaledelta;
	m_iFlags |= RENDERSYSTEM_FLAG_INCONTENTSONLY;// probably useless here, but you get the idea

	DrawContentsAdd(CONTENTS_WATER);
	DrawContentsAdd(CONTENTS_SLIME);

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
void CPSBubbles::ResetParameters(void)
{
	CPSBubbles::BaseClass::ResetParameters();
//	VectorClear(m_vecSpread);
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &time - 
//			&elapsedTime - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CPSBubbles::Update(const float &time, const double &elapsedTime)
{
	if (CPSBubbles::BaseClass::Update(time, elapsedTime))
		return 1;

/*	CParticle *curPart = NULL;
	for (int i = 0; i < m_iNumParticles; ++i)
	{
		curPart = &m_pParticleList[i];

		if (curPart->m_fEnergy <= 0.0f)
			continue;

		curPart->m_vAccel.x = sin(m_vecOrigin.x + elapsedTime*m_fFrameRate);
		curPart->m_vAccel.x = cos(m_vecOrigin.x + elapsedTime*m_fFrameRate);
	}*/

	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: A new particle has been created, initialize system-specific start
//			values for it.
// Input  : index - particle index in array
//-----------------------------------------------------------------------------
void CPSBubbles::InitializeParticle(CParticle *pParticle)
{
	if (m_bType == BUBBLES_TYPE_POINT)// v1 = org, v2 = spread
	{
		pParticle->m_vPos = m_vecOrigin;
		pParticle->m_vVel = VectorRandom() + VectorRandom();
		pParticle->m_vVel[0] *= m_vecSpread[0];
		pParticle->m_vVel[1] *= m_vecSpread[1];
		pParticle->m_vVel[2] *= m_vecSpread[2];
		pParticle->m_vPos += pParticle->m_vVel*2.0f;
		pParticle->m_vPos.z += pParticle->index*0.1f;
	}
	else if (m_bType == BUBBLES_TYPE_SPHERE)// v1 = org, v2.z = rad
	{
		pParticle->m_vVel = (VectorRandom() + VectorRandom()).Normalize();
		pParticle->m_vPos = m_vecOrigin + pParticle->m_vVel*m_fParticleVelocity;
	}
	else if (m_bType == BUBBLES_TYPE_BOX)// v1 = origin, v2 = halfbox
	{
		VectorRandom(pParticle->m_vPos, m_vecSpread);
		pParticle->m_vPos += m_vecOrigin;
	}
	else if (m_bType == BUBBLES_TYPE_LINE)// v1 = start, v2 = end
	{
		Vector d = m_vecSpread - m_vecOrigin;
		pParticle->m_vPos = m_vecOrigin + RANDOM_FLOAT(0,1)*d;
	}
	else
	{
		pParticle->m_vPos = m_vecOrigin;
	}

	pParticle->m_vPosPrev = pParticle->m_vPos;
	pParticle->m_vVel += m_vecDirection;
	pParticle->m_vVel *= m_fParticleVelocity;
	pParticle->m_vVel.z += FX_GetBubbleSpeed();
	pParticle->m_vAccel.x = 0.0f;
	pParticle->m_vAccel.y = 0.0f;
	pParticle->m_vAccel.z = 1.0;
	pParticle->m_fSizeX = m_fScale*RANDOM_FLOAT(0.5f, 2.0f);
	pParticle->m_fSizeY = pParticle->m_fSizeX;
	pParticle->m_fSizeDelta = m_fScaleDelta;
	pParticle->m_pTexture = m_pTexture;
	pParticle->m_iFrame = PARTICLE_WHITE_11; //DOESNT WORK. WHY???????
	pParticle->SetColor(m_color, m_fBrightness);
	pParticle->SetColorDelta(m_fColorDelta, m_fBrightnessDelta);

	if (m_iFlags & RENDERSYSTEM_FLAG_RANDOMFRAME)
		pParticle->FrameRandomize();

	if (m_OnInitializeParticle)
		m_OnInitializeParticle(this, pParticle, m_pOnInitializeParticleData);
}
