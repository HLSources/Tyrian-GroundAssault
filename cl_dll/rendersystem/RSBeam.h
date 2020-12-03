#ifndef RSBEAM_H
#define RSBEAM_H


//-----------------------------------------------------------------------------
// Simple beam made of two crossed quads (like those in Unreal)
//-----------------------------------------------------------------------------
class CRSBeam : public CRenderSystem
{
	typedef CRenderSystem BaseClass;
public:
	CRSBeam(void);
	CRSBeam(const Vector &start, const Vector &end, int sprindex, int frame, int r_mode, byte r, byte g, byte b, float a, float adelta, float scale, float scaledelta, float timetolive);
	virtual ~CRSBeam(void);

	virtual void ResetParameters(void);
	virtual void Render(void);

	Vector	m_vecEnd;
	float	m_fTextureTile;
	int m_iFrame;
};

#endif // RSBEAM_H
