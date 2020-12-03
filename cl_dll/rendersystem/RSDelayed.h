#ifndef RSDELAYED_H
#define RSDELAYED_H
#ifdef _WIN32
#pragma once
#endif


//-----------------------------------------------------------------------------
// A RenderSystem delayer
//-----------------------------------------------------------------------------
class CRSDelayed : public CRenderSystem
{
	typedef CRenderSystem BaseClass;
public:
	CRSDelayed(void);
	CRSDelayed(CRenderSystem *pSystem, float delay, int flags = 0, int followentindex = -1, int followflags = 0);
	virtual ~CRSDelayed(void);

	virtual void ResetParameters(void);
	virtual void KillSystem(void);
	virtual bool Update(const float &time, const double &elapsedTime);
	virtual void Render(void);

private:
	CRenderSystem *m_pSystem;// RenderSystem to be activated
//	float m_fStartTime;// used to delay appearance of child system
};

#endif // RSDELAYED_H
