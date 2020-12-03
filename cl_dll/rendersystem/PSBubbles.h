#ifndef PSBUBBLES_H
#define PSBUBBLES_H

enum
{
	BUBBLES_TYPE_POINT = 0,
	BUBBLES_TYPE_SPHERE,
	BUBBLES_TYPE_BOX,
	BUBBLES_TYPE_LINE
};

//-----------------------------------------------------------------------------
// Bubbles
//-----------------------------------------------------------------------------
class CPSBubbles : public CPSFlameCone
{
	typedef CPSFlameCone BaseClass;
public:
	CPSBubbles(void);
	CPSBubbles(int maxParticles, byte type, const Vector &vector1, const Vector &vector2, float velocity, int sprindex, int r_mode, float a, float adelta, float scale, float scaledelta, float timetolive);
	virtual ~CPSBubbles(void);

	virtual void ResetParameters(void);
	virtual void InitializeParticle(CParticle *pParticle);
	virtual bool Update(const float &time, const double &elapsedTime);

	byte m_bType;
};

#endif // PSBUBBLES_H
