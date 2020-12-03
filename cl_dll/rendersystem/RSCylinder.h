#ifndef RSCYLINDER_H
#define RSCYLINDER_H

//-----------------------------------------------------------------------------
// Renders cylinder, replacement for TE_BEAMCYLINDER
//-----------------------------------------------------------------------------
class CRSCylinder : public CRenderSystem
{
	typedef CRenderSystem BaseClass;
public:
	CRSCylinder(void);
	CRSCylinder(const Vector &origin, float radius, float radiusdelta, float width, unsigned short segments, int sprindex, int skin, int r_mode, byte r, byte g, byte b, float a, float adelta, float timetolive);
	virtual ~CRSCylinder(void);

	virtual void ResetParameters(void);
	virtual void InitializeSystem(void);
	virtual void Render(void);

private:
	float m_fWidth;
	unsigned short m_usSegments;
	Vector2D *m_pv2dPoints;
	int m_iFrame;
};

#endif // RSCYLINDER_H
