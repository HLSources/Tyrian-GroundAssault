#include "hud.h"
#include "cl_util.h"
#include "RenderManager.h"
#include "RenderSystem.h"
#include "RSSprite.h"
#include "weapondef.h"

//-----------------------------------------------------------------------------
// Purpose: Default constructor. Should never be used.
//-----------------------------------------------------------------------------
CRSSprite::CRSSprite(void)
{
	// Calling constructors directly is forbidden!
	ResetParameters();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor. Destroy data by calling KillSystem(), not directly!
//-----------------------------------------------------------------------------
CRSSprite::~CRSSprite(void)
{
	KillSystem();
}

//-----------------------------------------------------------------------------
// Purpose: Sprite. 
// Input  : origin - 
//			velocity - 
//			sprindex - 
//			r_mode - kRenderGlow does not work (impossible)
//			r g b - 
//			a - this value has no effect with kRenderTransAlpha (GoldSource)
//			adelta - 
//			scale - 
//			scaledelta - 
//			framerate - 
//			timetolive - 0 means the system removes itself after the last frame
// Accepts flags: RENDERSYSTEM_FLAG_RANDOMFRAME | LOOPFRAMES | etc.
//-----------------------------------------------------------------------------
CRSSprite::CRSSprite(const Vector &origin, const Vector &velocity, int sprindex, int r_mode, byte r, byte g, byte b, float a, float adelta, float scale, float scaledelta, float framerate, float timetolive)
{
	index = 0;// the only good place for this
	removenow = false;
	ResetParameters();
	m_pTexture = NULL;// MUST be before InitTexture()

	if (!InitTexture(sprindex))
	{
		dying = true;
		removenow = true;
		return;
	}
	m_vecOrigin = origin;
	m_vecVelocity = velocity;
	m_color.r = r;
	m_color.g = g;
	m_color.b = b;
	m_fBrightness = a;
	m_fBrightnessDelta = adelta;
	m_fScale = scale;
	m_fScaleDelta = scaledelta;
	m_iRenderMode = r_mode;
	m_fFrameRate = framerate;

	m_vecAngles.x = g_vecViewAngles.x + ev_punchangle.x;
	m_vecAngles.y = g_vecViewAngles.y + ev_punchangle.y;// keep Z
//	m_vecAngles.z = 0.0f;
//	gEngfuncs.GetViewAngles(m_vecAngles);// sprite differs from CRenderSystem in only one way: it is parallel to viewport
//	VectorAdd(m_vecAngles, ev_punchangle, m_vecAngles);

	if (m_iRenderMode == kRenderGlow)// XDM3035a: HACK: TriAPI does not support glow mode
		m_iRenderMode = kRenderTransAdd;

	if (timetolive <= 0.0f)// if 0, just display all frames
		m_fDieTime = 0.0f;
	else
		m_fDieTime = gEngfuncs.GetClientTime() + timetolive;

	InitializeSystem();
}

//-----------------------------------------------------------------------------
// Purpose: Draw system to screen. May get called in various situations, so
// DON'T change any RS variables here (do it in Update() instead).
//-----------------------------------------------------------------------------
void CRSSprite::Render(void)
{
	m_vecAngles.x = g_vecViewAngles.x + ev_punchangle.x;
	m_vecAngles.y = g_vecViewAngles.y + ev_punchangle.y;
//	m_vecAngles.z = g_vecViewAngles.y + ev_punchangle.z;// keep Z
	CRenderSystem::Render();
}
