#ifndef PSSparkShower_H
#define PSSparkShower_H

class CPSSparkShower : public CParticleSystem
{
	typedef CParticleSystem BaseClass;
public:
	CPSSparkShower(void);

	CPSSparkShower(int maxParticles, float life, float velocity, int type, const Vector &origin, const Vector &direction, const Vector &spread, int sprindex, float timetolive);
	virtual ~CPSSparkShower(void);

	virtual void ResetParameters(void);
	virtual void InitializeParticle(CParticle *pParticle);
	virtual bool Update(const float &time, const double &elapsedTime);

	Vector m_vecSpread;
	float m_fParticleVelocity;
	bool m_flRandomDir;
	int FX_Type;
	float m_fLife;
	float m_fDecalTime;
	vec3_t dir;
};

#endif // PSSparkShower_H
