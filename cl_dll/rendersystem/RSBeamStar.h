#ifndef RSBEAMSTAR_H
#define RSBEAMSTAR_H

//-----------------------------------------------------------------------------
// A very nice "sunburst" effect
//-----------------------------------------------------------------------------
class CRSBeamStar : public CRenderSystem
{
	typedef CRenderSystem BaseClass;
public:
	CRSBeamStar(void);
	CRSBeamStar::CRSBeamStar(const Vector &origin, int sprindex, int frame, int type, unsigned short number, int r_mode, byte r, byte g, byte b, float a, float adelta, float scale, float scaledelta, float timetolive);
	virtual ~CRSBeamStar(void);

	virtual void ResetParameters(void);
	virtual bool Update(const float &time, const double &elapsedTime);
	virtual void Render(void);

protected:
	float *m_ang1;
	float *m_ang2;
	vec3_t *m_Coords;
	unsigned short m_iCount;
	int m_iFrame;
	int lines;
};

#endif // RSBEAMSTAR_H
