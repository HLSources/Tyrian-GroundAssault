#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

class CParticle;

//-----------------------------------------------------------------------------
// Base particle system, with spawn point and direction (*velocity)
//-----------------------------------------------------------------------------
class CParticleSystem : public CRenderSystem
{
	typedef CRenderSystem BaseClass;
public:
	CParticleSystem(void);
	CParticleSystem(int maxParticles, const Vector &origin, const Vector &direction, int sprindex, int r_mode, float timetolive);
	virtual ~CParticleSystem(void);

	virtual void ResetParameters(void);
	virtual void InitializeSystem(void);
	virtual void KillSystem(void);
	virtual void InitializeParticle(CParticle *pParticle);
	virtual void ApplyForce(const Vector &origin, const Vector &force, float radius, bool point);
	virtual int Emit(const int &numParticles);
	virtual bool Update(const float &time, const double &elapsedTime);
	virtual void Render(void);

	CParticle *AllocateParticle(void);

	Vector m_vecDirection;

	void *m_pOnInitializeParticleData;// will be passed as pData argument into the callback function
	bool (*m_OnInitializeParticle)(CParticleSystem *pSystem, CParticle *pParticle, void *pData);
	void *m_pOnUpdateParticleData;// will be passed as pData argument into the callback function
	bool (*m_OnUpdateParticle)(CParticleSystem *pSystem, CParticle *pParticle, void *pData, const float &time, const double &elapsedTime);

protected:
	float m_fEnergyStart;// start value for particles
	float m_fNextEmitTime;
	int m_iMaxParticles;// maximum number of particles
	int m_iNumParticles;// number of currently active particles
	int m_iAccumulatedEmit;// number of particles left uninitialized

	CParticle *m_pParticleList;
};

#endif // PARTICLESYSTEM_H
