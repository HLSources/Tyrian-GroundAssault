//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

// Triangle rendering, if any
#include "hud.h"
#include "cl_util.h"
#include "triangleapi.h"
#include "RenderManager.h"// XDM
#include "gl_dynamic.h"
#include "cl_fx.h"

extern "C"
{
	void DLLEXPORT HUD_DrawNormalTriangles( void );
	void DLLEXPORT HUD_DrawTransparentTriangles( void );
};

/*
=================
HUD_DrawNormalTriangles

Non-transparent triangles-- add them here
=================
*/
void DLLEXPORT HUD_DrawNormalTriangles( void )
{
	if (gHUD.m_iActive > 0)
	{
		if (gHUD.m_Spectator.ShouldDrawOverview() == false)// don't duplicate in overview window
		{
			if (g_pRenderManager)
				g_pRenderManager->RenderOpaque();
		}
	}

	if (gHUD.m_iFogMode > 0)
		RenderFog(0,0,0,0,0,true);
}

/*
=================
HUD_DrawTransparentTriangles

Render any triangles with transparent rendermode needs here
=================
*/
void DLLEXPORT HUD_DrawTransparentTriangles( void )
{
	if (gHUD.m_iActive > 0)
	{
		if (gHUD.m_Spectator.DrawOverview() == 0)// don't duplicate in overview window
		{
			if (g_pRenderManager)
				g_pRenderManager->RenderTransparent();
		}
	}
//does not help	if (gHUD.m_iFogMode > 0)
//		RenderFog(0,0,0,0,0,true);
}

extern int g_iWaterLevel;
static float fog_color[3] = {127.0f, 127.0f, 127.0f};// 3 floats, but still 0-255
GLfloat fog_color_gl[4] = {0.5f, 0.5f, 0.5f, 1.0f};// 4 real floats, 0.0 - 1.0
//static float fog_startdist = 0.0f;
//static float fog_enddist = 1024.0f;


void RenderFog(byte r, byte g, byte b, float fStartDist, float fEndDist, bool updateonly)
{
	if (updateonly == false)
	{
		fog_color[0] = (float)r;
		fog_color[1] = (float)g;
		fog_color[2] = (float)b;
		fog_color_gl[0] = fog_color[0]/255.0f;
		fog_color_gl[1] = fog_color[1]/255.0f;
		fog_color_gl[2] = fog_color[2]/255.0f;
		gHUD.m_flFogStart = fStartDist;
		gHUD.m_flFogEnd = fEndDist;
//		CON_DPRINTF("cl: RenderFog(%f %f %f, %f, %f) updateonly == false\n", fog_color[0],fog_color[1],fog_color[2], gHUD.m_flFogStart, gHUD.m_flFogEnd);
	}
	// render fog only when NOT in water!
	bool bFog = g_iWaterLevel < 3 && fStartDist >= 0.0f && fEndDist >= 0.0f;
//	gEngfuncs.pTriAPI->Fog(fog_color, gHUD.m_flFogStart, gHUD.m_flFogEnd, bFog);

//	CON_DPRINTF("cl: RenderFog() %d\n", hw);
	if (gHUD.m_iHardwareMode == 1 && GL_glEnable)// OpenGL
	{
		if (bFog)
		{
			GL_glEnable(GL_FOG);
			GL_glFogi(GL_FOG_MODE, GL_LINEAR);// GL_EXP?
			GL_glFogfv(GL_FOG_COLOR, fog_color_gl);
			GL_glFogf(GL_FOG_DENSITY, 1.0f);
			GL_glHint(GL_FOG_HINT, GL_NICEST);
			GL_glFogf(GL_FOG_START, gHUD.m_flFogStart);
			GL_glFogf(GL_FOG_END, gHUD.m_flFogEnd);
		}
//		else
//			GL_glDisable(GL_FOG);
	}
	else//if (gHUD.m_iHardwareMode == 0 || gHUD.m_iHardwareMode == 2)
	{
		gEngfuncs.pTriAPI->Fog(fog_color, gHUD.m_flFogStart, gHUD.m_flFogEnd, bFog);
	}
}

void ResetFog(void)
{
	CON_DPRINTF("cl: ResetFog\n");
	fog_color[0] = 127.0f;
	fog_color[1] = 127.0f;
	fog_color[2] = 127.0f;
	fog_color_gl[0] = fog_color[0]/255.0f;
	fog_color_gl[1] = fog_color[1]/255.0f;
	fog_color_gl[2] = fog_color[2]/255.0f;
	gHUD.m_flFogStart = 0.0f;
	gHUD.m_flFogEnd = 0.0f;
	if (gHUD.m_iHardwareMode == 1 && GL_glEnable)// OpenGL
		GL_glDisable(GL_FOG);
	else
		gEngfuncs.pTriAPI->Fog(fog_color, 0.0f, 0.0f, 0);
}
