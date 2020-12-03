#ifndef PSFLAMECONE_H
#define PSFLAMECONE_H

//-----------------------------------------------------------------------------
// Creates a fountain-like spray of particles, can be directed of spheric
//-----------------------------------------------------------------------------
class CPSFlameCone : public CParticleSystem
{
	typedef CParticleSystem BaseClass;
public:
	CPSFlameCone(void);
	CPSFlameCone(int maxParticles, const Vector &origin, const Vector &direction, const Vector &spread, float velocity, int sprindex, int r_mode, float a, float adelta, float scale, float scaledelta, float timetolive);
	virtual ~CPSFlameCone(void);

	virtual void ResetParameters(void);
	virtual void InitializeParticle(CParticle *pParticle);
	virtual bool Update(const float &time, const double &elapsedTime);

	Vector m_vecSpread;
	float m_fParticleVelocity;
	bool m_flRandomDir;
};

#endif // PSFLAMECONE_H
