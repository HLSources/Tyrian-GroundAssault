#include "hud.h"
//#include "cl_util.h"
#include "RenderManager.h"
#include "RenderSystem.h"
#include "RSDelayed.h"

//-----------------------------------------------------------------------------
// Purpose: Default constructor. Should never be used.
//-----------------------------------------------------------------------------
CRSDelayed::CRSDelayed(void)
{
	// Calling constructors directly is forbidden!
	ResetParameters();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor. Used for memory cleaning. Destroy all data here.
//-----------------------------------------------------------------------------
CRSDelayed::~CRSDelayed(void)
{
	KillSystem();
}

//-----------------------------------------------------------------------------
// Purpose: This system is used to delay creation of another render system
// Input  : *pSystem - a newly created, NOT ADDED to the render manager list RS
//			delay - time period in which pSystem will be activated
//-----------------------------------------------------------------------------
CRSDelayed::CRSDelayed(CRenderSystem *pSystem, float delay, int flags, int followentindex, int followflags)
{
	if (pSystem)
	{
		index = 0;// the only good place for this
		removenow = false;
		ResetParameters();
		m_pTexture = NULL;
		m_pSystem = pSystem;
		m_pSystem->m_iFlags = flags;
		m_pSystem->m_iFollowEntity = followentindex;
		m_pSystem->m_iFollowFlags = followflags;
		m_fDieTime = gEngfuncs.GetClientTime() + delay;
		InitializeSystem();
	}
	else
		removenow = true;
}

//-----------------------------------------------------------------------------
// Purpose: Set default (external, non-system) values for all class variables.
//   Each derived class MUST call its ParentClass::ResetParameters()!
//-----------------------------------------------------------------------------
void CRSDelayed::ResetParameters(void)
{
	CRenderSystem::ResetParameters();
	m_pSystem = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Clear-out and free dynamically allocated memory
//-----------------------------------------------------------------------------
void CRSDelayed::KillSystem(void)
{
	if (m_pSystem)// somehow we didn't activate this system, so just delete it
	{
		delete m_pSystem;// it was not added to the manager's list
		m_pSystem = NULL;
	}
	CRenderSystem::KillSystem();
}

//-----------------------------------------------------------------------------
// Purpose: Check if it is time to activate our system
// Input  : &time - 
//			&elapsedTime - 
// Output : Returns true if needs to be removed
//-----------------------------------------------------------------------------
bool CRSDelayed::Update(const float &time, const double &elapsedTime)
{
	if (m_fDieTime <= time)
	{
		if (m_pSystem)// WARNING! Very tricky code here!
		{
			g_pRenderManager->AddSystem(m_pSystem, m_iFlags, m_iFollowEntity);
			m_pSystem = NULL;// the new system MODIFIED the render manager's order! DO NOT remove ourself right now!
		}
		else// Remove self only on NEXT update! This is probably a hack...
			return 1;// ready for removal
	}
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: Do nothing
//-----------------------------------------------------------------------------
void CRSDelayed::Render(void)
{
/*#ifdef _DEBUG
	// draw some debugging things here =)
#endif*/
}
