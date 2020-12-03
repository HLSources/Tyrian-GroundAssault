#include "hud.h"
#include "cl_util.h"
#include "RenderManager.h"
#include "RenderSystem.h"
#include "RSLight.h"
#include "r_efx.h"
#include "msg_fx.h"
#include "event_api.h"
#include "shared_resources.h"
#include "weapondef.h"

CRSLight::CRSLight(void)
{
	// Calling constructors directly is forbidden!
	ResetParameters();
}

CRSLight::~CRSLight(void)
{
	if (m_pLight != NULL)
	{
		m_pLight->decay = m_fBrightness;
		m_pLight->die = gEngfuncs.GetClientTime();
		m_pLight = NULL;
	}
	RadiusCallback = NULL;
	KillSystem();
}

//-----------------------------------------------------------------------------
// Purpose: Advanced dynamic light
// Input  : origin - 
//			r,g,b - light color
//			radius - 
//			(*RadiusFn) - makes light radius time-dependant. May be NULL, or, for example, sinf
//-----------------------------------------------------------------------------
CRSLight::CRSLight(const Vector &origin, byte r, byte g, byte b, float radius, float (*RadiusFn)(float time), float decay, float timetolive, bool elight)
{
	index = 0;// the only good place for this
	removenow = false;
	ResetParameters();

	if (elight)
		m_pLight = gEngfuncs.pEfxAPI->CL_AllocElight(LIGHT_INDEX_TE_RSLIGHT);
	else
		m_pLight = gEngfuncs.pEfxAPI->CL_AllocDlight(LIGHT_INDEX_TE_RSLIGHT);// if key != 0, the engine will overwrite existing dlight with the same key

	if (m_pLight == NULL)
	{
		CON_DPRINTF("CRSLight failed to allocate dynamic light!\n");
		removenow = true;
		return;// light is vital to this system
	}

	m_vecOrigin = origin;
	m_color.r = r;
	m_color.g = g;
	m_color.b = b;
	m_fBrightness = decay;
	m_fScale = radius;
	RadiusCallback = RadiusFn;
	m_flEntityLight = elight;

	m_pTexture = NULL;
	m_iRenderMode = 0;

	VectorCopy(m_vecOrigin, m_pLight->origin);
	m_pLight->radius = radius;
	m_pLight->color.r = r;
	m_pLight->color.g = g;
	m_pLight->color.b = b;
	m_pLight->decay = 0.0f;

	if (timetolive <= 0)
	{
		m_pLight->die = gEngfuncs.GetClientTime() + 1000.0f;
		m_fDieTime = 0;
	}
	else
	{
		m_pLight->die = gEngfuncs.GetClientTime() + timetolive;
		m_fDieTime = m_pLight->die;
	}

	InitializeSystem();
}

//-----------------------------------------------------------------------------
// Purpose: Set default (external, public, non-system) values for all class variables.
//   Each derived class MUST call its ParentClass::ResetParameters()!
// DO NOT call any functions from here.
//-----------------------------------------------------------------------------
void CRSLight::ResetParameters(void)
{
	CRenderSystem::ResetParameters();
	RadiusCallback = NULL;
	m_pLight = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Update light position and parameters. NOTE: light needs to be
//   recreated every frame in software mode.
// Input  : &time - 
//			&elapsedTime - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CRSLight::Update(const float &time, const double &elapsedTime)
{
	if (m_fDieTime > 0.0f && m_fDieTime <= time)
		dying = true;

	if (dying)
	{
		if (m_pLight)
		{
			m_pLight->die = time;
			m_pLight->decay = m_fBrightness;// allow "last" light to fade out properly
			m_pLight = NULL;
		}
	}
	else
	{
		FollowEntity();

		if (m_iFlags & RENDERSYSTEM_FLAG_NODRAW || (g_pCvarTrailDLight->value < 1.0f))// no need to restore as it'll be updated automatically
		{
			if (m_pLight)
				m_pLight->radius = 1.0f;
		}// don't recreate if nodraw
		else
		{
			if (m_pLight == NULL || !IEngineStudio.IsHardware())// software mode fixed lights fix?
			{
				if (m_pLight)
				{
					m_pLight->decay = 0.0f;
					m_pLight->die = time;// remove previous frame light
				}
				if (m_flEntityLight)
					m_pLight = gEngfuncs.pEfxAPI->CL_AllocElight(LIGHT_INDEX_TE_RSLIGHT);// m_pLight->key?
				else
					m_pLight = gEngfuncs.pEfxAPI->CL_AllocDlight(LIGHT_INDEX_TE_RSLIGHT);

				if (m_pLight)
				{
//						m_pLight->decay = 0.0f;
					m_pLight->die = /*gEngfuncs.GetClientTime()*/time + 0.001f;
				}
				else// unable to allocate the light
				{
					CON_DPRINTF("CRSLight failed to allocate dynamic light!\n");
					dying = true;
					return 1;
				}
			}

			if (m_pLight)
			{
//WTF?				m_pLight->radius = m_fScale*2.0f;
				if (RadiusCallback)
					m_pLight->radius = m_fScale * RadiusCallback(time);
				else
					m_pLight->radius = m_fScale;

				m_pLight->color.r = m_color.r;
				m_pLight->color.g = m_color.g;
				m_pLight->color.b = m_color.b;
				VectorCopy(m_vecOrigin, m_pLight->origin);

				if (m_fDieTime <= 0.0f)// don't let the light die
					m_pLight->die = time + 1.0f;
			}
		}
	}

	if (dying)
		return 1;

//	m_fScale += m_fScaleDelta*elapsedTime;
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: nothing to render here
//-----------------------------------------------------------------------------
void CRSLight::Render(void)
{
}
