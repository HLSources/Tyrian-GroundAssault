//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "hud.h"
#include "cl_util.h"
#include "com_model.h"
#include "studio.h"
#include "r_studioint.h"
#include <memory.h>
#include "studio_util.h"
#include "StudioModelRenderer.h"
#include "GameStudioModelRenderer.h"
#include "pm_shared.h"
#include "triangleapi.h"

//
// Override the StudioModelRender virtual member functions here to implement custom bone
// setup, blending, etc.
//

// The renderer object, created on the stack.
CGameStudioModelRenderer g_StudioRenderer;
/*
====================
CGameStudioModelRenderer

====================
*/
CGameStudioModelRenderer::CGameStudioModelRenderer(void) : CStudioModelRenderer()
{
//	m_pIndicatorSprite = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Overridden in XDM
//-----------------------------------------------------------------------------
/*void CGameStudioModelRenderer::Init(void)
{
	CStudioModelRenderer::Init();
	m_pIndicatorSprite = gEngfuncs.CL_LoadModel("sprites/spec_player.spr", NULL);//&i);
}*/

//-----------------------------------------------------------------------------
// Purpose: Overridden in XDM
//-----------------------------------------------------------------------------
#if 0
#error This code messes up draw order!
void CGameStudioModelRenderer::StudioRenderFinal(void)
{
	CStudioModelRenderer::StudioRenderFinal();

	// XDM3035c: draw team indicators above players
	if (m_pChromeSprite && IsActivePlayer(m_pCurrentEntity) && m_pCurrentEntity->curstate.framerate > 0.0f && !UTIL_PointIsFar(m_pCurrentEntity->origin, 1.0))//always 0 m_pCurrentEntity->curstate.health > 0.0f)
	{
		IEngineStudio.SetupRenderer(kRenderTransAdd);
		if (gEngfuncs.pTriAPI->SpriteTexture(m_pChromeSprite, 0))
		{
			byte r,g,b;
			Vector point = m_pCurrentEntity->origin + Vector(0,0,VEC_VIEW+16);
			Vector rx = (m_vRight)*6.0f;
			Vector uy = (m_vUp)*6.0f;
			GetTeamColor(m_pCurrentEntity->curstate.team, r,g,b);
			gEngfuncs.pTriAPI->RenderMode(kRenderTransAdd);
			gEngfuncs.pTriAPI->Color4ub(r, g, b, 200);
			gEngfuncs.pTriAPI->CullFace(TRI_NONE);
			gEngfuncs.pTriAPI->Begin(TRI_QUADS);
			gEngfuncs.pTriAPI->TexCoord2f(0,0);
			gEngfuncs.pTriAPI->Vertex3fv(point - rx + uy);
			gEngfuncs.pTriAPI->TexCoord2f(0,1);
			gEngfuncs.pTriAPI->Vertex3fv(point + rx + uy);
			gEngfuncs.pTriAPI->TexCoord2f(1,1);
			gEngfuncs.pTriAPI->Vertex3fv(point + rx - uy);
			gEngfuncs.pTriAPI->TexCoord2f(1,0);
			gEngfuncs.pTriAPI->Vertex3fv(point - rx - uy);
//triangle
/*			gEngfuncs.pTriAPI->Begin(TRI_TRIANGLES);
			gEngfuncs.pTriAPI->TexCoord2f(0,0);
			gEngfuncs.pTriAPI->Vertex3fv(point - rx + uy);
			gEngfuncs.pTriAPI->TexCoord2f(0.5,1);
			gEngfuncs.pTriAPI->Vertex3fv(point);
			gEngfuncs.pTriAPI->TexCoord2f(1,0);
			gEngfuncs.pTriAPI->Vertex3fv(point + rx + uy);
*/
			gEngfuncs.pTriAPI->End();
		}
		IEngineStudio.RestoreRenderer();
	}
}
#endif

////////////////////////////////////
// Hooks to class implementation
////////////////////////////////////

/*
====================
R_StudioDrawPlayer

====================
*/
int R_StudioDrawPlayer( int flags, entity_state_t *pplayer )
{
	return g_StudioRenderer.StudioDrawPlayer( flags, pplayer );
}

/*
====================
R_StudioDrawModel

====================
*/
int R_StudioDrawModel( int flags )
{
	return g_StudioRenderer.StudioDrawModel( flags );
}

/*
====================
R_StudioInit

====================
*/
void R_StudioInit( void )
{
	g_StudioRenderer.Init();
}

// The simple drawing interface we'll pass back to the engine
r_studio_interface_t studio =
{
	STUDIO_INTERFACE_VERSION,
	R_StudioDrawModel,
	R_StudioDrawPlayer,
};

/*
====================
HUD_GetStudioModelInterface

Export this function for the engine to use the studio renderer class to render objects.
====================
*/
extern "C" int EXPORT HUD_GetStudioModelInterface( int version, struct r_studio_interface_s **ppinterface, struct engine_studio_api_s *pstudio )
{
	if ( version != STUDIO_INTERFACE_VERSION )
		return 0;

	// Point the engine to our callbacks
	*ppinterface = &studio;

	// Copy in engine helper functions
	memcpy( &IEngineStudio, pstudio, sizeof( IEngineStudio ) );

	// Initialize local variables, etc.
	R_StudioInit();

	// Success
	return 1;
}
