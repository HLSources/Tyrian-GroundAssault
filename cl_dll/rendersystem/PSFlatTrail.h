#ifndef PSFLATTRAIL_H
#define PSFLATTRAIL_H

//-----------------------------------------------------------------------------
// Trail made of world-oriented (non-rotating) particles
//-----------------------------------------------------------------------------
class CPSFlatTrail : public CParticleSystem
{
	typedef CParticleSystem BaseClass;
public:
	CPSFlatTrail(void);
	CPSFlatTrail(const Vector &start, const Vector &end, int sprindex, int frame, int r_mode, byte r, byte g, byte b, float a, float adelta, float scale, float scaledelta, float dist_delta, float timetolive);
	virtual ~CPSFlatTrail(void);

	virtual void ResetParameters(void);
	virtual void InitializeParticle(CParticle *pParticle);
	virtual bool Update(const float &time, const double &elapsedTime);
	virtual void Render(void);

protected:

	Vector m_vecDelta;
	int m_iFrame;
};

#endif // PSFLATTRAIL_H
