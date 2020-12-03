#ifndef PSSPARKS_H
#define PSSPARKS_H

//-----------------------------------------------------------------------------
// Set of particles, emitting from a point
//-----------------------------------------------------------------------------
class CPSSparks : public CParticleSystem
{
	typedef CParticleSystem BaseClass;
public:
	CPSSparks(void);
	CPSSparks(int maxParticles, const Vector &origin, float scalex, float scaley, float scaledelta, float velocity, float startenergy, byte r, byte g, byte b, float a, float adelta, int sprindex, int r_mode, float timetolive);
	virtual ~CPSSparks(void);

	virtual void ResetParameters(void);
	virtual bool Update(const float &time, const double &elapsedTime);
	virtual void Render(void);
	virtual void InitializeParticle(CParticle *pParticle);

protected:
//	float m_fScaleX;
//	float m_fScaleY;
	float m_fVelocity;
	bool m_bReversed;
};

#endif // PSSPARKS_H
