#ifndef PSDRIPS_H
#define PSDRIPS_H

#define SPLASH_SIZE_DELTA 96.0

//-----------------------------------------------------------------------------
// Spawns rain particles at specified origin and in volume of mins and maxs
//-----------------------------------------------------------------------------
class CPSDrips : public CParticleSystem
{
	typedef CParticleSystem BaseClass;
public:
	CPSDrips(void);
	CPSDrips(int maxParticles, const Vector &origin, const Vector &mins, const Vector &maxs, const Vector &dir, int sprindex, int sprhitwater, int sprhitground, int r_mode, float sizex, float sizey, float scaledelta, float sizehitground_delta, float timetolive);
	virtual ~CPSDrips(void);

	virtual void ResetParameters(void);
	virtual bool Update(const float &time, const double &elapsedTime);
	virtual void InitializeParticle(CParticle *pParticle);
//	virtual void ApplyForce(const Vector &origin, const Vector &force, float radius, bool point);
	virtual void Render(void);

	void SetParticleSize(const float &sizex, const float &sizey);

private:
	Vector m_vecMinS;
	Vector m_vecMaxS;
	float m_fSpeed;
//	float m_fSizeX;
//	float m_fSizeY;
//	int m_iSplashTexture;
	float m_fScaleHitGroundDelta;
	model_t *m_pTextureHitWater;
	model_t *m_pTextureHitGround;
};

#endif // PSDRIPS_H
