#include "hud.h"
#include "cl_util.h"
#include "Particle.h"
#include "RenderManager.h"
#include "RenderSystem.h"
#include "ParticleSystem.h"
#include "PSAurora.h"
#include "triangleapi.h"
#include "pm_defs.h"
#include "event_api.h"
#include "shared_resources.h"

// UNDONE
// TODO: emission, updating

//-----------------------------------------------------------------------------
// Purpose: Custom particle class
//-----------------------------------------------------------------------------
CParticleAur::CParticleAur():CParticle()
{
	m_pOverlay = NULL;
	m_pType = NULL;
	age = 0.0f;
	// don't bother resetting other data - it will only slow down the system
}

//-----------------------------------------------------------------------------
// Purpose: Custom particle render procedure. Includes overlays.
// Input  : &rt - AngleVectors() outputs - right and
//			&up - up
//			rendermode - kRenderTransAdd
//			doubleside - draw both front and back sides (useful for faces not
//						aligned parallel to the screen
//-----------------------------------------------------------------------------
void CParticleAur::Render(const Vector &rt, const Vector &up, const int &rendermode, const bool &doubleside)
{
/*
	float fCosSize = CosLookup(part->m_fAngle)*fSize;
	float fSinSize = SinLookup(part->m_fAngle)*fSize;

	// calculate the four corners of the sprite
	VectorMA(origin, fSinSize, up, point1);
	VectorMA(point1, -fCosSize, right, point1);

	VectorMA(origin, fCosSize, up, point2);
	VectorMA(point2, fSinSize, right, point2);

	VectorMA(origin, -fSinSize, up, point3);
	VectorMA(point3, fCosSize, right, point3);

	VectorMA(origin, -fCosSize, up, point4);
	VectorMA(point4, -fSinSize, right, point4);
*/
//	float sr,cr;
//	SinCos(DEG2RAD(part->m_fAngle), &sr, &cr);
	Vector local_rt, local_up;
	AngleVectors(Vector(0.0f,0.0f,m_fAngle), NULL, local_rt, local_up);
	local_rt += rt;
	local_up += up;// TESTME!

	int iContents = 0;
	for (CParticleAur *pDraw = this; pDraw; pDraw = pDraw->m_pOverlay)
	{
// ERROR TODO UNDONE TESTME FIXME BUGBUG		m_pTexture is null
		if (pDraw->m_pType && pDraw->m_pType->m_iDrawContents)
		{
			if (iContents == 0)
				iContents = gEngfuncs.PM_PointContents(m_vPos, NULL);

			if (iContents != pDraw->m_pType->m_iDrawContents)
				continue;
		}
		pDraw->CParticle::Render(local_rt, local_up, rendermode, doubleside);// great if your compiler can handle this. otherwise do something to prevent recursion!
	}
}



//-----------------------------------------------------------------------------
// Purpose: Particle type, this class holds initial values for particles
// Input  : CPSAurora	*pMySystem - 
//			*pNext - 
//-----------------------------------------------------------------------------
CParticleType::CParticleType(CPSAurora *pMySystem)
{
	m_bDefined = false;
	m_iRenderMode = kRenderTransAdd;
	m_iDrawContents = 0;
//	m_Bounce
//	m_BounceFriction;
	m_bBouncing = false;
	m_iSprite = 0;
	m_pSprite = NULL;
	m_Life = RandomRange(1);
	m_StartRed = m_StartGreen = m_StartBlue = m_StartAlpha = m_EndRed = m_EndGreen = m_EndBlue = m_EndAlpha = RandomRange(1);
	m_StartSize = RandomRange(8);
//	m_SizeDelta = RandomRange(0);
	m_EndSize = RandomRange(16);
//	m_StartFrame;
//	m_EndFrame;
	m_FrameRate = RandomRange(16);// incompatible with EndFrame
	m_bEndFrame = false;
	m_StartAngle = RandomRange(45);
//	m_AngleDelta = RandomRange(0);
/*	m_SprayRate;
	m_SprayForce;
	m_SprayPitch;
	m_SprayYaw;
	m_SprayRoll;
	m_Gravity;
	m_WindStrength;
	m_WindYaw;
	m_Drag;*/
	m_pSprayType = m_pOverlayType = NULL;
	m_szName[0] = 0;
	m_pMySystem = pMySystem;
	m_pNext = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Load data from ini-file section
// Input  : *szFile - 
// Output : char * - points to where the parsing ended
//-----------------------------------------------------------------------------
char *CParticleType::ParseData(char *szFile)
{
	if (szFile == NULL)
		return NULL;

	char *szStart = szFile;
	char szToken[1024];
	szFile = gEngfuncs.COM_ParseFile(szFile, szToken);
	while (*szToken != '}')
	{
		if (szFile == NULL)
			break;

		if (!strcmp(szToken, "name"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			strncpy(m_szName, szToken, sizeof(m_szName));

			CParticleType *pTemp = m_pMySystem->FindParticleType(szToken, NULL);
			if (pTemp)
			{
				// there's already a type with this name
				if (pTemp->m_bDefined)
					CON_PRINTF("Warning: CParticleType %s is defined more than once!\n", szToken);

				m_bDefined = false;
				return szStart;// tell parent object we did not parse a thing
			}
		}
		else if (!strcmp(szToken, "sprite"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_pSprite = gEngfuncs.CL_LoadModel(szToken, &m_iSprite);
			if (m_pSprite == NULL)
			{
				CON_PRINTF("ERROR! CParticleType %s has invalid sprite %s!\n", m_szName, szToken);
				m_bDefined = false;
				return szStart;// tell parent object we did not parse a thing
			}
		}
		else if (!strcmp(szToken, "startalpha"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_StartAlpha = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "endalpha"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_EndAlpha = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "startred"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_StartRed = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "endred"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_EndRed = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "startgreen"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_StartGreen = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "endgreen"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_EndGreen = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "startblue"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_StartBlue = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "endblue"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_EndBlue = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "startsize"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_StartSize = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "sizedelta"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_SizeDelta = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "endsize"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_EndSize = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "startangle"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_StartAngle = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "angledelta"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_AngleDelta = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "startframe"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_StartFrame = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "endframe"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_EndFrame = RandomRange(szToken);
			m_bEndFrame = true;
		}
		else if (!strcmp(szToken, "framerate"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_FrameRate = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "lifetime"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_Life = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "gravity"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_Gravity = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "windyaw"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_WindYaw = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "windstrength"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_WindStrength = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "spraytype"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			CParticleType *pTemp = m_pMySystem->FindParticleType(szToken, NULL);
			if (pTemp)
				m_pSprayType = pTemp;
			else
				CON_PRINTF("CParticleType %s: undefined spraytype: %s\n", m_szName, szToken);
//TODO				m_pSprayType = new CParticleType(this, szToken);
//				m_pMySystem->AddParticleType(m_pSprayType);
		}
		else if (!strcmp(szToken, "overlaytype"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			CParticleType *pTemp = m_pMySystem->FindParticleType(szToken, NULL);

			if (pTemp)
				m_pOverlayType = pTemp;
			else
				CON_PRINTF("CParticleType %s: undefined overlaytype: %s\n", m_szName, szToken);
//TODO				m_pOverlayType = new CParticleType(this, szToken)
//				m_pMySystem->AddParticleType(m_pOverlayType);
		}
		else if (!strcmp(szToken, "sprayrate"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_SprayRate = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "sprayforce"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_SprayForce = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "spraypitch"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_SprayPitch = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "sprayyaw"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_SprayYaw = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "sprayroll"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_SprayRoll = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "drag"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_Drag = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "bounce"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_Bounce = RandomRange(szToken);
			if (m_Bounce.m_flMin != 0 || m_Bounce.m_flMax != 0)
				m_bBouncing = true;
		}
		else if (!strcmp(szToken, "bouncefriction"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			m_BounceFriction = RandomRange(szToken);
		}
		else if (!strcmp(szToken, "rendermode"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			if (!strcmp(szToken, "additive"))
				m_iRenderMode = kRenderTransAdd;
			else if (!strcmp(szToken, "solid"))
				m_iRenderMode = kRenderTransAlpha;
			else if (!strcmp(szToken, "texture"))
				m_iRenderMode = kRenderTransTexture;
			else if (!strcmp(szToken, "color"))
				m_iRenderMode = kRenderTransColor;
		}
		else if (!strcmp(szToken, "drawcondition"))
		{
			szFile = gEngfuncs.COM_ParseFile(szFile,szToken);
			if (!strcmp(szToken, "empty"))
				m_iDrawContents = CONTENTS_EMPTY;
			else if (!strcmp(szToken, "water"))
				m_iDrawContents = CONTENTS_WATER;
			else if (!strcmp(szToken, "solid"))
				m_iDrawContents = CONTENTS_SOLID;
			else if (!strcmp(szToken, "special1"))
				m_iDrawContents = CONTENTS_SPECIAL1;
			else if (!strcmp(szToken, "special2"))
				m_iDrawContents = CONTENTS_SPECIAL2;
			else if (!strcmp(szToken, "special3"))
				m_iDrawContents = CONTENTS_SPECIAL3;
		}
		else
			CON_PRINTF("CParticleType %s: unknown value: %s\n", m_szName, szToken);

		// get the next token
		szFile = gEngfuncs.COM_ParseFile(szFile, szToken);
	}

//	ASSERT(m_pSprite != NULL);
	m_bDefined = true;
	return szFile;
//	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Load data into a particle
// Input  : *pParticle - 
//-----------------------------------------------------------------------------
void CParticleType::InitParticle(CParticleAur *pParticle)
{
	float fLifeRecip = 1/pParticle->age_death;
	pParticle->m_pType = this;
//	pParticle->m_vVel[0] = pParticle->m_vVel[1] = pParticle->m_vVel[2] = 0;
//	pParticle->m_vAccel[0] = pParticle->m_vAccel[1] = 0;
	pParticle->m_vAccel[2] = m_Gravity.Get();
//	pParticle->m_iEntIndex = 0;

	if (m_pOverlayType)
	{
		// create an overlay for this particle
		CParticleAur *pOverlayParticle = (CParticleAur *)m_pMySystem->AllocateParticle();
		if (pOverlayParticle)
		{
			pOverlayParticle->m_pType = m_pOverlayType;
			m_pMySystem->InitializeParticle(pOverlayParticle);
			pOverlayParticle->age = pParticle->age;
			pOverlayParticle->age_death = pParticle->age_death;
		}
		pParticle->m_pOverlay = pOverlayParticle;
	}

	if (m_pSprayType)
		pParticle->age_spray = 1.0f/m_SprayRate.Get();
	else
		pParticle->age_spray = 0.0f;

	pParticle->m_fSizeX = pParticle->m_fSizeY = m_StartSize.Get();// cannot separate

	if (m_EndSize.IsDefined())
		pParticle->m_fSizeDelta = m_EndSize.GetOffset(pParticle->m_fSizeX) * fLifeRecip;
	else
		pParticle->m_fSizeDelta = m_SizeDelta.Get();

	pParticle->m_iFrame = (int)m_StartFrame.Get();
	if (m_EndFrame.IsDefined())
		pParticle->m_fFrameDelta = m_EndFrame.GetOffset((float)pParticle->m_iFrame) * fLifeRecip;
	else
		pParticle->m_fFrameDelta = m_FrameRate.Get();

	pParticle->m_fColor[0] = m_StartRed.Get();
	pParticle->m_fColorDelta[0] = m_EndRed.GetOffset(pParticle->m_fColor[0]) * fLifeRecip;
	pParticle->m_fColor[1] = m_StartGreen.Get();
	pParticle->m_fColorDelta[1] = m_EndGreen.GetOffset(pParticle->m_fColor[1]) * fLifeRecip;
	pParticle->m_fColor[2] = m_StartBlue.Get();
	pParticle->m_fColorDelta[2] = m_EndBlue.GetOffset(pParticle->m_fColor[2]) * fLifeRecip;
	pParticle->m_fColor[3] = m_StartAlpha.Get();
	pParticle->m_fColorDelta[3] = m_EndAlpha.GetOffset(pParticle->m_fColor[3]) * fLifeRecip;

	pParticle->m_pTexture = m_pSprite;

	pParticle->m_fAngle = m_StartAngle.Get();
	pParticle->m_fAngleDelta = m_AngleDelta.Get();

	pParticle->m_fDrag = m_Drag.Get();

	float fWindStrength = m_WindStrength.Get();
	float fWindYaw = m_WindYaw.Get();
	float s,c;
	SinCos(fWindYaw, &s, &c);
	pParticle->m_vecWind.x = fWindStrength*c;
	pParticle->m_vecWind.y = fWindStrength*s;
	pParticle->m_vecWind.z = 0;

	pParticle->age = 0.0f;
	pParticle->age_death = m_Life.Get();
}

//-----------------------------------------------------------------------------
// Purpose: GetName
// Output : const char
//-----------------------------------------------------------------------------
const char *CParticleType::GetName(void)
{
	return m_szName;
}




//-----------------------------------------------------------------------------
// Purpose: Default constructor
//-----------------------------------------------------------------------------
CPSAurora::CPSAurora(void)
{
	m_pParticleTypes = NULL;
	ResetParameters();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CPSAurora::~CPSAurora(void)
{
	KillSystem();
}

//-----------------------------------------------------------------------------
// Purpose: Usable constructor
// Input  : *filename - 
//			attachment - entity attachment to use
//-----------------------------------------------------------------------------
CPSAurora::CPSAurora(const char *filename, int attachment)
{
	index = 0;// the only good place for this
	removenow = false;
	InitTexture(g_iModelIndexAnimglow01);// error-proof (should never be used)
	m_pParticleTypes = NULL;
	ResetParameters();
	m_iEntAttachment = attachment;

	char relpath[MAX_PATH];// = "scripts/";// XDM: keep root directory clean! Put your stuff into /scripts/
	//strcat(relpath, filename);
	sprintf(relpath, "scripts/aurora/%s.aur", filename);
	char *pFileStart = (char *)gEngfuncs.COM_LoadFile((char *)relpath, 5, NULL);
	char *pFile = pFileStart;
	char szToken[1024];
	char szMainPartTypeName[PARTICLETYPE_NAME_MAX];
	int parttypes = 0;

	if (pFile == NULL)
	{
		CON_PRINTF("CPSAurora: couldn't load %s.\n", relpath);
		removenow = true;
		return;
	}
	else
	{
		pFile = gEngfuncs.COM_ParseFile(pFile, szToken);
		while (pFile)
		{
			if (strcmp(szToken, "particles") == 0)
			{
				pFile = gEngfuncs.COM_ParseFile(pFile, szToken);
				if (pFile)
					m_iMaxParticles = atoi(szToken);
			}
			else if (strcmp(szToken, "maintype") == 0)
			{
				pFile = gEngfuncs.COM_ParseFile(pFile, szToken);
				//m_pMainType = new CParticleType(this, szToken);
				if (pFile)
					strncpy(szMainPartTypeName, szToken, PARTICLETYPE_NAME_MAX);
			}
			else if (strcmp(szToken, "attachment") == 0)
			{
				pFile = gEngfuncs.COM_ParseFile(pFile, szToken);
				if (pFile)
					m_iEntAttachment = atoi(szToken);
			}
			else if (strcmp(szToken, "lightmodel") == 0)
			{
				pFile = gEngfuncs.COM_ParseFile(pFile, szToken);
				if (pFile)
					m_iLightingModel = atoi(szToken);
			}
			else if (strcmp(szToken, "killcondition") == 0)
			{
				pFile = gEngfuncs.COM_ParseFile(pFile, szToken);
				if (strcmp(szToken, "empty") == 0)
					m_iKillCondition = CONTENTS_EMPTY;
				else if (strcmp(szToken, "water") == 0)
					m_iKillCondition = CONTENTS_WATER;
				else if (strcmp(szToken, "solid") == 0)
					m_iKillCondition = CONTENTS_SOLID;
			}
			else if (*szToken == '{')//(strcmp(szToken, "{") == 0)
			{
				CParticleType *pType = new CParticleType(this);
				pType->ParseData(pFile);
				if (pType->m_bDefined)
				{
					if (AddParticleType(pType))
						parttypes++;
					else
						delete pType;
				}
				else
					delete pType;
			}
			pFile = gEngfuncs.COM_ParseFile(pFile, szToken);
		}
	}
	gEngfuncs.COM_FreeFile(pFileStart);
//	pFileStart = NULL;
	strcpy(m_szName, filename);

	m_pMainType = FindParticleType(szMainPartTypeName, NULL);// estabilish link by name
	if (m_pMainType)
		CON_PRINTF("CPSAurora(%s) initialized, %d particle types (main: %s)\n", m_szName, parttypes, m_pMainType->GetName());
	else
		CON_PRINTF("CPSAurora(%s) ERROR: no main type! (defined %d)\n", m_szName, parttypes);

	m_fDieTime = -1.0f;
	InitializeSystem();

	// CHECKME
// NO! Emit() will fail to InitializeParticle()!	m_iNumParticles = m_iMaxParticles;// all particles are present at start
}

//-----------------------------------------------------------------------------
// Purpose: Set default (external, public, non-system) values for all class variables.
//   Each derived class MUST call its ParentClass::ResetParameters()!
// DO NOT call any functions from here.
//-----------------------------------------------------------------------------
void CPSAurora::ResetParameters(void)
{
	CPSAurora::BaseClass::ResetParameters();
	m_iEntAttachment = 0;
	m_iKillCondition = 0;
	m_iLightingModel = 0;
//	entityMatrix.clear();
	m_pMainType = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Initialize SYSTEM (non-user) startup variables.
// Must be called from class constructor.
// Overrides particle class
//-----------------------------------------------------------------------------
void CPSAurora::InitializeSystem(void)
{
	CRenderSystem::InitializeSystem();// skip CParticleSystem!

	if (m_pParticleList != NULL)
	{
		delete [] m_pParticleList;
		m_pParticleList = NULL;
	}

	ASSERT(m_iMaxParticles > 0);
	m_pParticleList = new CParticleAur[m_iMaxParticles];// allocate custom class

	m_iNumParticles = 0;
	m_iAccumulatedEmit = 0;
	m_fNextEmitTime = 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: Clear-out and free dynamically allocated memory
//-----------------------------------------------------------------------------
void CPSAurora::KillSystem(void)
{
	DeleteAllParticleTypes();
	CPSAurora::BaseClass::KillSystem();
}

//-----------------------------------------------------------------------------
// Purpose: A new particle has been created, initialize system-specific start
//			values for it.
// Input  : pParticle - particle
//-----------------------------------------------------------------------------
void CPSAurora::InitializeParticle(CParticle *pParticle)
{
	CParticleAur *pParticleEx = (CParticleAur *)pParticle;
	// Assign values from CParticleType to CParticleAur
/*	pParticleEx->m_vPos[0] = m_vecOrigin[0];
	pParticleEx->m_vPos[1] = m_vecOrigin[1];
	pParticleEx->m_vPos[2] = m_vecOrigin[2];*/
	VectorCopy(m_vecOrigin, pParticleEx->m_vPos);
	VectorCopy(pParticleEx->m_vPos, pParticleEx->m_vPosPrev);

	VectorClear(pParticleEx->m_vAccel);
	VectorClear(pParticleEx->m_vVel);
	pParticleEx->m_vAccel[2] = 4.0f;
	pParticleEx->m_vVel[2] = 16.0f;

	pParticleEx->m_fEnergy = 1.0f;
	pParticleEx->m_fSizeX = 1.0f;
	pParticleEx->m_fSizeY = 1.0f;
//test	pParticleEx->m_fSizeDelta = (float)(pParticle->index+1)*0.25f;

// done in constructor	pParticleEx->m_frame = 0;
/*	pParticleEx->SetColor(m_color, m_fBrightness);
	pParticleEx->m_fColorDelta[0] = 0.1f;
	pParticleEx->m_fColorDelta[1] = 0.1f;
	pParticleEx->m_fColorDelta[2] = 0.1f;
	pParticleEx->m_fColorDelta[3] = m_fBrightnessDelta;
*/
	pParticleEx->m_pType = m_pMainType;
	if (pParticleEx->m_pType)
		pParticleEx->m_pType->InitParticle(pParticleEx);
	else
		pParticleEx->m_pTexture = m_pTexture;
//	else
//		DBG_FORCEBREAK
}

//-----------------------------------------------------------------------------
// Purpose: Update time-dependent properties
// Input  : &time - 
//			&elapsedTime - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CPSAurora::Update(const float &time, const double &elapsedTime)
{
	if (m_fDieTime > 0.0f && m_fDieTime <= time)
		dying = true;

	if (dying && m_iNumParticles <= 0)
		return 1;

	cl_entity_t *pEntity = FollowEntity();
	if (pEntity)
		m_vecAngles = pEntity->curstate.angles;

//	if (m_fNextEmitTime <= time)
	{
		Emit(1);
//		m_fNextEmitTime = time + 1.0f/(float)m_iMaxParticles;
	}

	CParticleAur *curPart = NULL;
	pmtrace_t pmtrace;
	for (int i = 0; i < m_iNumParticles; ++i)
	{
		curPart = (CParticleAur *)&m_pParticleList[i];
		// special erase procedure
		if (curPart->m_fEnergy <= 0.0f || (curPart->age_death >= 0 && curPart->age > curPart->age_death))
		{
			if (curPart->m_pOverlay)
			{
				curPart->m_pOverlay->m_fEnergy = 0.0f;// will be removed
				curPart->m_pOverlay = NULL;// unlink
			}
			curPart->m_pType = NULL;
			m_pParticleList[i] = m_pParticleList[--m_iNumParticles];
		}
		curPart->m_vPosPrev = curPart->m_vPos;

		if (curPart->m_fDrag != 0.0f)
			curPart->m_vVel -= (curPart->m_fDrag*elapsedTime)*(curPart->m_vVel - curPart->m_vecWind);

		curPart->m_vVel += elapsedTime*curPart->m_vAccel;
		curPart->m_vPos += elapsedTime*curPart->m_vVel;

		if (!(m_iFlags & RENDERSYSTEM_FLAG_NOCLIP))
		{
//			pmtrace_t *tr = gEngfuncs.PM_TraceLine(part->origin, vecTarget, PM_TRACELINE_PHYSENTSONLY, 2, -1);
			gEngfuncs.pEventAPI->EV_PlayerTrace(curPart->m_vPosPrev, curPart->m_vPos, PM_STUDIO_IGNORE/*PM_STUDIO_BOX*/, -1, &pmtrace);// TODO: don't collide with owner ?
			if (pmtrace.fraction != 1.0f)
			{
				if (m_iFlags & RENDERSYSTEM_FLAG_CLIPREMOVE)// remove particle
				{
					curPart->m_fEnergy = -1.0f;
					//--m_iNumParticles;
					continue;
				}
				else if (curPart->m_pType->m_bBouncing || (m_iFlags & RENDERSYSTEM_FLAG_ADDPHYSICS))// reflect particle velocity
				{
					float bounceforce = DotProduct(curPart->m_vVel, pmtrace.plane.normal);
					float newspeed = (1.0f - curPart->m_pType->m_BounceFriction.Get());
					curPart->m_vPos = pmtrace.endpos;
					curPart->m_vVel *= newspeed;
					curPart->m_vVel -= (bounceforce*(newspeed+curPart->m_pType->m_Bounce.Get()))*pmtrace.plane.normal;
				}
			}
		}

		// spray children
		if (curPart->m_pType && curPart->age_spray > 0.0f && curPart->age > curPart->age_spray)
		{
//			ASSERT(curPart->m_pType != NULL);
			float v = curPart->m_pType->m_SprayRate.Get();
			if (v != 0.0f)
				curPart->age_spray = curPart->age + 1.0f/v;// Divide by zero! DBZ!

			//particle *pChild = ActivateParticle();
			if (curPart->m_pType->m_pSprayType)
			{
				CParticleAur *pChild = (CParticleAur *)AllocateParticle();
				if (pChild)
				{
					pChild->m_pType = curPart->m_pType->m_pSprayType;
					InitializeParticle(pChild);
					pChild->m_vPos = curPart->m_vPos;
					float fSprayForce = curPart->m_pType->m_SprayForce.Get();
					pChild->m_vVel = curPart->m_vVel;
					if (fSprayForce)
					{
						float fSprayPitch = curPart->m_pType->m_SprayPitch.Get() - m_vecAngles.x;	//AJH For rotating paticles.
						float fSprayYaw = curPart->m_pType->m_SprayYaw.Get() - m_vecAngles.y;		//AJH
						float fForceCosPitch = fSprayForce*cosf(DEG2RAD(fSprayPitch));//- m_vecAngles.z;	//AJH
						pChild->m_vVel.x += cosf(DEG2RAD(fSprayYaw)) * fForceCosPitch;
						pChild->m_vVel.y += sinf(DEG2RAD(fSprayYaw)) * fForceCosPitch;
						pChild->m_vVel.z -= sinf(DEG2RAD(fSprayPitch)) * fSprayForce;
					}
				}
			}
		}

		if (m_iFlags & RENDERSYSTEM_FLAG_RANDOMFRAME)
			curPart->FrameRandomize();
		else
			curPart->FrameIncrease();

		curPart->UpdateColor(elapsedTime);
		curPart->UpdateSize(elapsedTime);
		curPart->UpdateEnergyByBrightness();
		if (curPart->m_fAngleDelta != 0.0f)
		{
			curPart->m_fAngle += curPart->m_fAngleDelta * elapsedTime;
			NormalizeAngle360(&curPart->m_fAngle);
		}
		curPart->age += elapsedTime;
	}
	m_fScale += m_fScaleDelta*elapsedTime;// effect radius is increasing
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: Render
//-----------------------------------------------------------------------------
void CPSAurora::Render(void)
{
//	if (PointIsVisible(m_vecOrigin))
	{
//		BaseClass::Render();
		if (gHUD.m_iPaused <= 0)
			m_vecAngles = g_vecViewAngles;

		Vector v_up, v_right;
		AngleVectors(m_vecAngles, NULL, v_right, v_up);
		CParticleAur *p = NULL;
		for (int i = 0; i < m_iNumParticles; ++i)
		{
			p = (CParticleAur *)&m_pParticleList[i];
			if (p->m_fEnergy <= 0.0f || p->m_fSizeX <= 0.0f)
				continue;

			if (UTIL_PointIsFar(p->m_vPos, 1.0))// faster?
				continue;
//			if (!PointIsVisible(p->m_vPos))// faster?
//				continue;

//			AngleVectors(p->m_fAngle, NULL, v_right, v_up);
			p->Render(v_right, v_up, m_iRenderMode);
		}
		gEngfuncs.pTriAPI->RenderMode(kRenderNormal);// ?
	}
}

//-----------------------------------------------------------------------------
// Purpose: Add new type into system, "undefined" types are allowed too.
// Input  : *pType - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CPSAurora::AddParticleType(CParticleType *pType)
{
	if (pType == NULL)
		return false;

	pType->m_pNext = m_pParticleTypes;
	m_pParticleTypes = pType;
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Delete all Particle Types
//-----------------------------------------------------------------------------
void CPSAurora::DeleteAllParticleTypes(void)
{
	CParticleType *pType = m_pParticleTypes;
	CParticleType *pNext = NULL;
	while (pType)
	{
		pNext = pType->m_pNext;
		delete pType;
		pType = pNext;
	}
	m_pParticleTypes = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Find Particle Type by name
// Input  : *szName - 
// Output : CParticleType
//-----------------------------------------------------------------------------
CParticleType *CPSAurora::FindParticleType(const char *szName, CParticleType *pStartType)
{
	if (szName && *szName)
	{
		if (pStartType == NULL)
			pStartType = m_pParticleTypes;

		for (CParticleType *pType = pStartType; pType; pType = pType->m_pNext)
		{
			if (strcmp(pType->m_szName, szName) == 0)
				return pType;
		}
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: GetName
// Output : const char
//-----------------------------------------------------------------------------
const char *CPSAurora::GetName(void)
{
	return m_szName;
}
