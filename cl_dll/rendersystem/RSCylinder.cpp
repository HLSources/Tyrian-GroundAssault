#include "hud.h"
#include "cl_util.h"
#include "RenderManager.h"
#include "RenderSystem.h"
#include "RSCylinder.h"
#include "triangleapi.h"
#include "weapondef.h"

CRSCylinder::CRSCylinder(void)
{
	// Calling constructors directly is forbidden!
	ResetParameters();
}

CRSCylinder::~CRSCylinder(void)
{
	if (m_pv2dPoints)
	{
		delete [] m_pv2dPoints;
		m_pv2dPoints = NULL;
	}
	KillSystem();
}

//-----------------------------------------------------------------------------
// Purpose: Constructor.
// Input  : origin - 
//			radius - 
//			radiusdelta - 
//			width - 
//			segments - 
//			sprindex - 
//			r_mode - 
//			r,g,b -
//			a - 
//			adelta - 
//			timetolive - 
//-----------------------------------------------------------------------------
CRSCylinder::CRSCylinder(const Vector &origin, float radius, float radiusdelta, float width, unsigned short segments, int sprindex, int skin, int r_mode, byte r, byte g, byte b, float a, float adelta, float timetolive)
{
	index = 0;// the only good place for this
	removenow = false;
	ResetParameters();
	m_pTexture = NULL;// MUST be before InitTexture()
	if (!InitTexture(sprindex))
	{
		removenow = true;
		return;
	}
	m_vecOrigin = origin;
	m_fScale = radius;
	m_fScaleDelta = radiusdelta;
	m_fWidth = width;
	m_usSegments = segments;
	m_color.r = r;
	m_color.g = g;
	m_color.b = b;
	m_fBrightness = a;
	m_fBrightnessDelta = adelta;
	m_iFrame = skin;

	if (m_fScale < 2.0f)
		m_fScale = 2.0f;

	if (m_usSegments < 6)
		m_usSegments = 6;

	m_iFollowEntity = -1;
	m_iRenderMode = r_mode;

	if (timetolive < 0)
		m_fDieTime = -1;
	else
		m_fDieTime = gEngfuncs.GetClientTime() + timetolive;

	InitializeSystem();
}

//-----------------------------------------------------------------------------
// Purpose: Set default (external, public, non-system) values for all class variables.
//   Each derived class MUST call its ParentClass::ResetParameters()!
// DO NOT call any functions from here.
//-----------------------------------------------------------------------------
void CRSCylinder::ResetParameters(void)
{
	CRSCylinder::BaseClass::ResetParameters();
	m_fWidth = 0.0f;
	m_usSegments = 0;
	m_pv2dPoints = NULL;// dangerous!
}

//-----------------------------------------------------------------------------
// Purpose: Initialize SYSTEM (non-user) startup variables.
// Must be called from class constructor.
//-----------------------------------------------------------------------------
void CRSCylinder::InitializeSystem(void)
{
	CRSCylinder::BaseClass::InitializeSystem();

	float angle = 0.0f;
	float step = (M_PI_F*2)/(float)m_usSegments;
	m_pv2dPoints = new Vector2D[m_usSegments];
	for (unsigned short i=0; i<m_usSegments; ++i)
	{
		SinCos(angle, &m_pv2dPoints[i].x, &m_pv2dPoints[i].y);
		angle += step;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Render
//-----------------------------------------------------------------------------
void CRSCylinder::Render(void)
{
	if (!InitTexture(texindex))
		return;

	if (!gEngfuncs.pTriAPI->SpriteTexture(m_pTexture, (int)m_fFrame))
		return;

	gEngfuncs.pTriAPI->SpriteTexture(m_pTexture, m_iFrame);
	gEngfuncs.pTriAPI->RenderMode(m_iRenderMode);
	gEngfuncs.pTriAPI->Color4ub(m_color.r, m_color.g, m_color.b, 255);//(unsigned char)(m_fBrightness*255.0f));
	gEngfuncs.pTriAPI->Brightness(m_fBrightness);
	gEngfuncs.pTriAPI->CullFace(TRI_NONE);
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);

	float h = m_fWidth/2.0f;
	float x = 0.0f, y = 0.0f;
	float v = 0.0f;
	float vs = 0.25f;
	unsigned short i2;
	for (unsigned short i=0; i<m_usSegments; ++i)
	{
		x = m_vecOrigin.x + m_pv2dPoints[i].x*m_fScale;
		y = m_vecOrigin.y + m_pv2dPoints[i].y*m_fScale;

		gEngfuncs.pTriAPI->TexCoord2f(0.0f, v);
		gEngfuncs.pTriAPI->Vertex3f(x, y, m_vecOrigin.z+h);
		gEngfuncs.pTriAPI->TexCoord2f(1.0f, v);// exchange these to rotate by 90
		gEngfuncs.pTriAPI->Vertex3f(x, y, m_vecOrigin.z-h);
//		gEngfuncs.pEfxAPI->R_ShowLine(Vector(x,y,m_vecOrigin.z+h), Vector(x,y,m_vecOrigin.z-h));

		i2 = (i<m_usSegments-1)?(i+1):0;// warp
		x = m_vecOrigin.x + m_pv2dPoints[i2].x*m_fScale;
		y = m_vecOrigin.y + m_pv2dPoints[i2].y*m_fScale;

		gEngfuncs.pTriAPI->TexCoord2f(1.0f, v+vs);
		gEngfuncs.pTriAPI->Vertex3f(x, y, m_vecOrigin.z-h);
		gEngfuncs.pTriAPI->TexCoord2f(0.0f, v+vs);// exchange these to rotate by 90
		gEngfuncs.pTriAPI->Vertex3f(x, y, m_vecOrigin.z+h);
//		gEngfuncs.pEfxAPI->R_ShowLine(Vector(x,y,m_vecOrigin.z+h), Vector(x,y,m_vecOrigin.z-h));

		v += vs;
//		CON_PRINTF("i=%d\n", i);
	}

/*
	float h = m_fWidth/2.0f;
	float step = ((float)M_PI*2.0f)/m_usSegments;
	float x1 = 0.0f, y1 = 0.0f, x2 = 0.0f, y2 = 0.0f;
	float v = 0.0f;
	float vs = 0.25f;
	for (float a = 0.0f; a < M_PI*2.0f; a += step)
	{
		SinCos(a, &x1, &y1);
		x1 = x1*m_fScale + m_vecOrigin[0];
		y1 = y1*m_fScale + m_vecOrigin[1];

		SinCos(a + step, &x2, &y2);
		x2 = x2*m_fScale + m_vecOrigin[0];
		y2 = y2*m_fScale + m_vecOrigin[1];

		gEngfuncs.pTriAPI->TexCoord2f(0.0f, v);
		gEngfuncs.pTriAPI->Vertex3f(x1, y1, m_vecOrigin[2]+h);

		gEngfuncs.pTriAPI->TexCoord2f(1.0f, v);// exchange these to rotate by 90
		gEngfuncs.pTriAPI->Vertex3f(x1, y1, m_vecOrigin[2]-h);

		gEngfuncs.pTriAPI->TexCoord2f(1.0f, v+vs);
		gEngfuncs.pTriAPI->Vertex3f(x2, y2, m_vecOrigin[2]-h);

		gEngfuncs.pTriAPI->TexCoord2f(0.0f, v+vs);// exchange these to rotate by 90
		gEngfuncs.pTriAPI->Vertex3f(x2, y2, m_vecOrigin[2]+h);

		v += vs;
	}
*/
	gEngfuncs.pTriAPI->End();
//	gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
}
