#ifndef PSSPAWNEFFECT_H
#define PSSPAWNEFFECT_H

#include "matrix.h"
#include "randomrange.h"

class CParticle;
class CParticleAur;
class CPSAurora;

#define PARTICLETYPE_NAME_MAX	32
//-----------------------------------------------------------------------------
// CParticleType - particle group parameters description holder
// Since Aurora allows many different particle systems at once, we need this.
//
// XDM: What a weird system. Did the author fully understand what he was doing?
// XDM: Right now it's a pretty plain conversion, TODO:
// class CPSAurora : public CParticleSystem - contains particle parameters
// class CRSAurora : public CRenderSystem - contains instances of CPSAuroraInternalSystem
//-----------------------------------------------------------------------------
class CParticleType
{
	friend class CPSAurora;// allow system to access protected data
public:
	CParticleType(CPSAurora	*pMySystem);
	char *ParseData(char *szFile);
	void InitParticle(CParticleAur *pParticle);
	const char *GetName(void);

	bool		m_bDefined; // is this CParticleType just a placeholder?
	byte		m_iRenderMode;
	short		m_iDrawContents;
	RandomRange	m_Bounce;
	RandomRange	m_BounceFriction;
	bool		m_bBouncing;

	int			m_iSprite;
	struct model_s *m_pSprite;

	RandomRange	m_Life;

	RandomRange	m_StartAlpha;
	RandomRange	m_EndAlpha;
	RandomRange	m_StartRed;
	RandomRange	m_EndRed;
	RandomRange	m_StartGreen;
	RandomRange	m_EndGreen;
	RandomRange	m_StartBlue;
	RandomRange	m_EndBlue;

	RandomRange	m_StartSize;
	RandomRange	m_SizeDelta;
	RandomRange	m_EndSize;

	RandomRange	m_StartFrame;
	RandomRange	m_EndFrame;
	RandomRange	m_FrameRate; // incompatible with EndFrame
	bool		m_bEndFrame;

	RandomRange	m_StartAngle;
	RandomRange	m_AngleDelta;

	RandomRange	m_SprayRate;
	RandomRange	m_SprayForce;
	RandomRange	m_SprayPitch;
	RandomRange	m_SprayYaw;
	RandomRange	m_SprayRoll;

	RandomRange	m_Gravity;
	RandomRange	m_WindStrength;
	RandomRange	m_WindYaw;

	RandomRange	m_Drag;

	CParticleType	*m_pSprayType;
	CParticleType	*m_pOverlayType;
	char		m_szName[PARTICLETYPE_NAME_MAX];

protected:
	CPSAurora		*m_pMySystem;
	CParticleType	*m_pNext;
};

//-----------------------------------------------------------------------------
// Aurora-compatible particle class
// WARNING: hold particles in CParticleSystem::m_pParticleList!
//-----------------------------------------------------------------------------
class CParticleAur : public CParticle
{
public:
	CParticleAur();
	virtual void Render(const Vector &rt, const Vector &up, const int &rendermode, const bool &doubleside = false);

	CParticleAur	*m_pOverlay;// for making multi-layered CParticles
	CParticleType	*m_pType;// a pointer to its type
	Vector		m_vecWind;
	float		m_fAngle;
	float		m_fAngleDelta;
	float		m_fFrameDelta;
	float		m_fDrag;
	float		age;
	float		age_death;
	float		age_spray;
};

//-----------------------------------------------------------------------------
// Aurora system compatibility
//-----------------------------------------------------------------------------
class CPSAurora : public CParticleSystem
{
	typedef CParticleSystem BaseClass;
public:
	CPSAurora(void);
	CPSAurora(const char *filename, int attachment);
	virtual ~CPSAurora(void);

	virtual void ResetParameters(void);
	virtual void InitializeSystem(void);
	virtual void KillSystem(void);
	virtual void InitializeParticle(CParticle *pParticle);
	virtual bool Update(const float &time, const double &elapsedTime);
	virtual void Render(void);

	bool AddParticleType(CParticleType *pType);
	void DeleteAllParticleTypes(void);
	CParticleType *FindParticleType(const char *szName, CParticleType *pStartType);
	const char *GetName(void);

protected:
	int m_iEntAttachment;
	int m_iKillCondition;
	int m_iLightingModel;
	matrix3x3 entityMatrix;
	char m_szName[32];

//	std::vector<CParticleType *>	m_ParticleTypes;
	CParticleType	*m_pParticleTypes;
	CParticleType	*m_pMainType;
};

#endif // PSSPAWNEFFECT_H
