#ifndef RSLIGHT_H
#define RSLIGHT_H

// Half-Life specific
#include "dlight.h"

#define LIGHT_INDEX_TE_RSLIGHT 0// 0 allows multiple dlight instances

//-----------------------------------------------------------------------------
// Controls a dynamic light, makes it possible to move, change color, etc.
//-----------------------------------------------------------------------------
class CRSLight : public CRenderSystem
{
	typedef CRenderSystem BaseClass;
public:
	CRSLight(void);
	CRSLight(const Vector &origin, byte r, byte g, byte b, float radius, float (*RadiusFn)(float time), float decay, float timetolive, bool elight = false );
	virtual ~CRSLight(void);

	virtual void ResetParameters(void);
	virtual bool Update(const float &time, const double &elapsedTime);
	virtual void Render(void);

	float (*RadiusCallback)(float time);// radius = f(time);

private:
	dlight_t *m_pLight;
	bool m_flEntityLight;// affect studio models only
};

#endif // RSLIGHT_H
