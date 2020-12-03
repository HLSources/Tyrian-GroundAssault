//-----------------------------------------------------------------------------
// Player spawn effect (spiral of particles)
//-----------------------------------------------------------------------------
class CPSSpawnEffect : public CParticleSystem
{
	typedef CParticleSystem BaseClass;
public:
	CPSSpawnEffect(void);
	CPSSpawnEffect::CPSSpawnEffect(int maxParticles, const Vector &origin, float scale, float scaledelta, float radius, float radiusdelta, int sprindex, int r_mode, byte r, byte g, byte b, float a, float adelta, float timetolive);
	virtual ~CPSSpawnEffect(void);

//no need to	virtual void ResetParameters(void);
	virtual void InitializeParticle(CParticle *pParticle);
	virtual bool Update(const float &time, const double &elapsedTime);
	virtual void Render(void);
	float m_fRadius;
	float m_fRadiusDelta;
};

