#ifndef PSBlastCone_H
#define PSBlastCone_H

//-----------------------------------------------------------------------------
// Creates a fountain-like spray of particles, can be directed of spheric
//-----------------------------------------------------------------------------
class CPSBlastCone : public CParticleSystem
{
	typedef CParticleSystem BaseClass;
public:
	CPSBlastCone(void);
	CPSBlastCone(int maxParticles, float velocity, const Vector &origin, const Vector &direction, const Vector &spread, float scale, float scaledelta, byte r, byte g, byte b, float a, float adelta, int sprindex, bool animate, int frame, int r_mode, float timetolive);
	virtual ~CPSBlastCone(void);

	virtual void ResetParameters(void);
	virtual void InitializeParticle(CParticle *pParticle);
	virtual bool Update(const float &time, const double &elapsedTime);

	Vector m_vecSpread;
	float m_fParticleVelocity;
	bool m_flRandomDir, m_fAnimatedSpr;
	int m_fLife, m_iFrame;
};

#endif // PSBlastCone_H
