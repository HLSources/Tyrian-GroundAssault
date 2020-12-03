#include "hud.h"
#include "cl_util.h"
#include "Particle.h"
#include "RenderManager.h"
#include "RenderSystem.h"
#include "ParticleSystem.h"
#include "PSSparkShower.h"
#include "pm_defs.h"
#include "event_api.h"
#include "msg_fx.h"
#include "r_efx.h"
#include "PSBlastCone.h"
#include "PSSparks.h"
#include "RSBeam.h"
#include "shared_resources.h"
#include "weapondef.h"
#include "decals.h"
#include "cl_fx.h"

CPSSparkShower::CPSSparkShower(void)
{
	ResetParameters();
}

CPSSparkShower::~CPSSparkShower(void)
{
	KillSystem();
}

CPSSparkShower::CPSSparkShower(int maxParticles, float life, float velocity, int type, const Vector &origin, const Vector &direction, const Vector &spread, int sprindex, float timetolive)
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

	m_iRenderMode = kRenderTransAdd;
	m_fLife = life;
	FX_Type = type;
	m_fDecalTime = gHUD.m_flTime;

	if (timetolive <= 0.0f)
		m_fDieTime = -1;
	else
		m_fDieTime = gEngfuncs.GetClientTime() + timetolive;

	InitializeSystem();
}

void CPSSparkShower::ResetParameters(void)
{
	CPSSparkShower::BaseClass::ResetParameters();
	VectorClear(m_vecSpread);
	m_flRandomDir = true;
	m_fParticleVelocity = 300.0f;
}

bool CPSSparkShower::Update(const float &time, const double &elapsedTime)
{
	if (m_fDieTime > 0.0f && m_fDieTime <= time)
		dying = true;

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

		if (gEngfuncs.PM_PointContents(curPart->m_vPos, NULL ) == CONTENTS_WATER)
			curPart->m_fEnergy -= 1.0f;

		if (!(m_iFlags & RENDERSYSTEM_FLAG_NOCLIP))
		{
			gEngfuncs.pEventAPI->EV_PlayerTrace(curPart->m_vPosPrev, curPart->m_vPos, PM_WORLD_ONLY, -1, &pmtrace);

			if (pmtrace.fraction != 1.0f)
			{
				if (m_iFlags & RENDERSYSTEM_FLAG_DRAWDECAL)
				{
					if (!UTIL_PointIsFar(curPart->m_vPos, 0.75) && (gHUD.m_flTime - m_fDecalTime > 0.25f))
					{
						DecalTrace(RANDOM_LONG(DECAL_SMALLSCORCH1,DECAL_SMALLSCORCH3), &pmtrace);
						m_fDecalTime = gHUD.m_flTime;
					}
				}
				if (m_iFlags & RENDERSYSTEM_FLAG_CLIPREMOVE)
				{
					curPart->m_fEnergy = -1.0f;
					//--m_iNumParticles;
					continue;
				}
				else
				{
					float p = DotProduct(curPart->m_vVel, pmtrace.plane.normal);
					VectorMA(curPart->m_vVel, -2.0f*p, pmtrace.plane.normal, curPart->m_vVel);
					curPart->m_vVel = curPart->m_vVel * 0.7f;
				}
			}
			switch (FX_Type)
			{
				case SPARKSHOWER_SPARKS:
					gEngfuncs.pEfxAPI->R_SparkEffect(curPart->m_vPos, 1, -128, 128);
				break;

				case SPARKSHOWER_SPARKS2:
					g_pRenderManager->AddSystem(new CPSBlastCone(1, 0, curPart->m_vPos, curPart->m_vPos, Vector(0,0,0), 5, 0, 255,167,17, 0.7, -0.8, g_iModelIndexExplosion5, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_RANDOMFRAME, -1);
					gEngfuncs.pEfxAPI->R_StreakSplash ( curPart->m_vPos, Vector(0,0,0), 5, RANDOM_FLOAT(2,3), 120, -128, 128);
				break;

				case SPARKSHOWER_EXP:
					g_pRenderManager->AddSystem(new CPSBlastCone(1, 20, curPart->m_vPos, curPart->m_vPos, Vector(1,1,1), 15, 40, 255,255,255, 0.5, -0.5, g_iModelIndexPartRed, FALSE, PARTICLE_RED_6, kRenderTransAdd, 0.1), 0, -1);
				break;

				case SPARKSHOWER_STREAKS:
					gEngfuncs.pEfxAPI->R_StreakSplash ( curPart->m_vPos, Vector(0,0,0), 5, RANDOM_FLOAT(3,7), 400, -150, 150);
				break;

				case SPARKSHOWER_FLICKER:
					gEngfuncs.pEfxAPI->R_FlickerParticles(curPart->m_vPos);
				break;

				case SPARKSHOWER_SPARKSMOKE:
					gEngfuncs.pEfxAPI->R_BulletImpactParticles(curPart->m_vPos);
				break;

				case SPARKSHOWER_SMOKE:
					gEngfuncs.pEfxAPI->R_RocketTrail ( curPart->m_vPosPrev, curPart->m_vPos, 1 );
				break;

				case SPARKSHOWER_FIRESMOKE:
					gEngfuncs.pEfxAPI->R_RocketTrail ( curPart->m_vPosPrev, curPart->m_vPos, 0 );
				break;

				case SPARKSHOWER_BLOODDRIPS:
					gEngfuncs.pEfxAPI->R_RocketTrail ( curPart->m_vPosPrev, curPart->m_vPos, 2 );
				break;

				case SPARKSHOWER_FIREEXP:
					g_pRenderManager->AddSystem(new CPSBlastCone(1, 30, curPart->m_vPos, curPart->m_vPos, Vector(1,1,1), 10, 25, 255,255,255, 0.8, -0.95, g_iModelIndexFlame, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_RANDOMFRAME, -1);
				break;
	
				case SPARKSHOWER_ENERGY:
					g_pRenderManager->AddSystem(new CPSBlastCone(1, 30, curPart->m_vPos, curPart->m_vPos, Vector(1,1,1), 10, 15, 255,255,255, 0.8, -0.95, g_iModelIndexPartRed, FALSE, PARTICLE_RED_3, kRenderTransAdd, 0.1), 0, -1);
				break;

				case SPARKSHOWER_BLUEENERGY:
					g_pRenderManager->AddSystem(new CPSBlastCone(1, 30, curPart->m_vPos, curPart->m_vPos, Vector(1,1,1), 10, 15, 255,255,255, 0.8, -0.95, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.1), 0, -1);
				break;

				case SPARKSHOWER_GREENENERGY:
					g_pRenderManager->AddSystem(new CPSBlastCone(1, 30, curPart->m_vPos, curPart->m_vPos, Vector(1,1,1), 10, 15, 0,200,0, 0.8, -0.95, g_iModelIndexAnimSpr11, TRUE, 0, kRenderTransAdd, 0.1), 0, -1);
				break;

				case SPARKSHOWER_BLACKSMOKE:
					g_pRenderManager->AddSystem(new CPSBlastCone(1, 30, curPart->m_vPos, curPart->m_vPos, Vector(1,1,1), 10, 15, 0,0,0, 0.5, -0.75, g_iModelIndexPartBlack, FALSE, 0, kRenderTransAlpha, 0.1), 0, -1);
				break;

				case SPARKSHOWER_LAVA_FLAME:
					g_pRenderManager->AddSystem(new CPSBlastCone(1, 15, curPart->m_vPos, curPart->m_vPos, Vector(1,1,1), 5, 15, 128,128,128, 1, -0.9, g_iModelIndexAnimSpr9, TRUE, 0, kRenderTransAdd, 0.1), 0, -1);
				break;

				case SPARKSHOWER_GREENSMOKE:
					g_pRenderManager->AddSystem(new CPSBlastCone(1, 30, curPart->m_vPos, curPart->m_vPos, Vector(1,1,1), 5, 20, 0,128,0, 0.75, -0.9, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_2, kRenderTransAdd, 0.1), 0, -1);
				break;

				case SPARKSHOWER_LIGHTNING_STRIKE: //lightning ball
					dir.x=RANDOM_FLOAT(-1,1);
					dir.y=RANDOM_FLOAT(-1,1);
					dir.z=RANDOM_FLOAT(-1,1);
					dir=dir.Normalize();

					switch (RANDOM_LONG(0,1))
					{
						case 0:gEngfuncs.pEfxAPI->R_BeamPoints(curPart->m_vPos, curPart->m_vPos+(dir*150), g_iModelIndexBeamsAll, 0.1, 1.2, 1.5, 250, 20, BLAST_SKIN_LIGHTNING, 0, 255, 255, 255);break;
						case 1:gEngfuncs.pEfxAPI->R_BeamPoints(curPart->m_vPos, curPart->m_vPos+(dir*150), g_iModelIndexBeamsAll, 0.1, 1.0, 1.5, 250, 20, BLAST_SKIN_FROSTGRENADE, 0, 255, 255, 255);break;
					}
				break;	
				
				case SPARKSHOWER_GREEN_LIGHTNING_STRIKE: //displacer
					dir.x=RANDOM_FLOAT(-1,1);
					dir.y=RANDOM_FLOAT(-1,1);
					dir.z=RANDOM_FLOAT(-1,1);
					dir=dir.Normalize();

					switch (RANDOM_LONG(0,1))
					{
						   case 0:gEngfuncs.pEfxAPI->R_BeamPoints(curPart->m_vPos, curPart->m_vPos+(dir*300), g_iModelIndexBeamsAll, 0.2, 2, 4, 255, 20, BLAST_SKIN_PLASMA, 0, 0, 255, 0);break;
						   case 1:gEngfuncs.pEfxAPI->R_BeamPoints(curPart->m_vPos, curPart->m_vPos+(dir*400), g_iModelIndexBeamsAll, 0.2, 1, 3, 255, 20, BLAST_SKIN_PLASMA, 0, 255, 200, 0);break;
					}
				break;

				case SPARKSHOWER_LIGHTNING_CHAOS: //Lightning gun hit
					dir.x=RANDOM_FLOAT(-1,1);
					dir.y=RANDOM_FLOAT(-1,1);
					dir.z=RANDOM_FLOAT(-1,1);
					dir=dir.Normalize();
					gEngfuncs.pEfxAPI->R_BeamPoints(curPart->m_vPos, curPart->m_vPos+(dir*RANDOM_FLOAT(50,100)), g_iModelIndexBeamsAll, 0.3, 0.9, 2.5, 40, 20, BLAST_SKIN_PULSE, 0, 255, 255, 255);
				break;
			}
		}

		curPart->m_fEnergy -= (float)(1.5 * elapsedTime);

		if (m_OnUpdateParticle)
			m_OnUpdateParticle(this, curPart, m_pOnUpdateParticleData, time, elapsedTime);
	}
	if (!(m_iFlags & RENDERSYSTEM_FLAG_NOCLIP))
		gEngfuncs.pEventAPI->EV_PopPMStates();

	if (m_iFlags & RENDERSYSTEM_FLAG_SIMULTANEOUS)
		if (m_iNumParticles <= 0)
			return 1;

	return 0;
}

void CPSSparkShower::InitializeParticle(CParticle *pParticle)
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
	{
		pParticle->m_vAccel.z = -g_cl_gravity/2;
		pParticle->m_vAccel.x = 0.0f;
		pParticle->m_vAccel.y = 0.0f;
	}
	pParticle->m_vVel = pParticle->m_vVel * m_fParticleVelocity;
	pParticle->m_pTexture = m_pTexture;
	pParticle->m_fEnergy = m_fLife;
	pParticle->SetColor(m_color, 0.01);

	if (m_OnInitializeParticle)
		m_OnInitializeParticle(this, pParticle, m_pOnInitializeParticleData);
}