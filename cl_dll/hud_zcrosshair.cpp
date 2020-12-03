#include "hud.h"
#include "cl_util.h"
#include "r_studioint.h"
#include "com_model.h"
#include "triangleapi.h"
#include "studio_util.h"
#include "event_api.h"
#include "eventscripts.h"
#include "weapondef.h"

#define SPR_TOP_LEFT		0
#define SPR_MIDDLE_LEFT		1
#define SPR_MIDDLE_RIGHT	2
#define SPR_TOP_RIGHT		3


extern float g_lastFOV;

// !!! DON'T FORGET TO Begin(TRI_QUADS) !!!
void DrawQuad(float xmin, float ymin, float xmax, float ymax)
{
	//top left
	gEngfuncs.pTriAPI->TexCoord2f(0,0);
	gEngfuncs.pTriAPI->Vertex3f(xmin, ymin, 0.0f); 
	//bottom left
	gEngfuncs.pTriAPI->TexCoord2f(0,1);
	gEngfuncs.pTriAPI->Vertex3f(xmin, ymax, 0.0f);
	//bottom right
	gEngfuncs.pTriAPI->TexCoord2f(1,1);
	gEngfuncs.pTriAPI->Vertex3f(xmax, ymax, 0.0f);
	//top right
	gEngfuncs.pTriAPI->TexCoord2f(1,0);
	gEngfuncs.pTriAPI->Vertex3f(xmax, ymin, 0.0f);
}

// this is called just as launcher loads in steam versions
int CHudZoomCrosshair::Init(void)
{
	m_fFinalFOV = 0.0f;
	m_iRenderMode = -1;
	m_iTextureIndex1 = -1;
	m_pTexture1 = NULL;

	m_iFlags = 0;

	gHUD.AddHudElem(this);

	return 1;
}

void CHudZoomCrosshair::Reset(void)
{
	m_pTexture1 = gEngfuncs.CL_LoadModel("sprites/hud_zoom01.spr", &m_iTextureIndex1);

	// XDM3035b: test
	m_fFinalFOV = 0.0f;// gHUD.GetUpdatedDefaultFOV();
	m_iFlags &= ~HUD_ACTIVE;
	g_lastFOV = gHUD.m_iFOV = gHUD.GetUpdatedDefaultFOV();
	gHUD.m_Ammo.UpdateCrosshair(1, 0);
}

int CHudZoomCrosshair::VidInit(void)
{
//	CON_DPRINTF("CHudZoomCrosshair::VidInit()\n");

	if (m_iRenderMode >= 0)
	{
		if (m_iTextureIndex1 > 0 && m_pTexture1 == NULL)
		{
			m_pTexture1 = IEngineStudio.GetModelByIndex(m_iTextureIndex1);
			if (!m_pTexture1 || m_pTexture1->type != mod_sprite)
			{
				CON_DPRINTF("CHudZoomCrosshair::VidInit() unable to load texture 1 %d!\n", m_iTextureIndex1);
				return 0;
			}
		}
	}

	return 1;
}

int CHudZoomCrosshair::Draw(const float &flTime)
{
	if (m_iRenderMode < 0)
		return 0;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT))))
		return 0;

	if (m_pTexture1 == NULL)
	{
/* do not disable, we still need FOV updates!
		int result = VidInit();
		if (result == 0 || m_pTexture1 == NULL || m_pTexture2 == NULL)
		{
//			m_iRenderMode = -1;
			m_iFlags &= ~HUD_ACTIVE;// XDM3035
			CON_DPRINTF("CHudZoomCrosshair::Draw() failed completely! Disabling.\n");
			*/
			return 0;
//		}
	}

//	if (IEngineStudio.IsHardware() <= 0)
	if (gHUD.m_iHardwareMode <= 0)// XDM3035: software mode does not support this code
		return 0;

	float left = (ScreenWidth - ScreenHeight)/2.0f;
	float right = left + ScreenHeight;
	float centerx = ScreenWidth/2.0f;

	gEngfuncs.pTriAPI->RenderMode(m_iRenderMode);//kRenderTransColor for indexalpha
	gEngfuncs.pTriAPI->Brightness(1.0f);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
	gEngfuncs.pTriAPI->CullFace(TRI_NONE);

	if (gEngfuncs.pTriAPI->SpriteTexture(m_pTexture1, SPR_TOP_LEFT))
	{
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
			DrawQuad(0, 0, left+1.5, ScreenHeight);
		gEngfuncs.pTriAPI->End();
	}

	if (gEngfuncs.pTriAPI->SpriteTexture(m_pTexture1, SPR_MIDDLE_LEFT))
	{
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);
	DrawQuad(left-1,	0,	centerx+2,	ScreenHeight);
	gEngfuncs.pTriAPI->End();
	}
	if (gEngfuncs.pTriAPI->SpriteTexture(m_pTexture1, SPR_MIDDLE_RIGHT))
	{
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);
	DrawQuad(centerx-1,	0,	right+2,	ScreenHeight);
	gEngfuncs.pTriAPI->End();
	}
	if (gEngfuncs.pTriAPI->SpriteTexture(m_pTexture1, SPR_TOP_RIGHT))
	{
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
			DrawQuad(right-1, 0, ScreenWidth, ScreenHeight);
		gEngfuncs.pTriAPI->End();
	}
	else
	{
//		CON_DPRINTF("CHudZoomCrosshair::Draw() failed to use texture %s!\n", m_pTexture2->name);
//		gEngfuncs.pTriAPI->End();

		FillRGBA(0, 0, left, ScreenHeight, 0,0,0,255);
		FillRGBA(right, 0, ScreenWidth, ScreenHeight, 0,0,0,255);
//		return 1;
	}

	gEngfuncs.pTriAPI->RenderMode(kRenderNormal);

	return 1;
}

void CHudZoomCrosshair::SetParams(int spr_idx1, int rendermode, float finalfov)
{
//	CON_DPRINTF("SetParams: %d %d rm %d ff %f\n", spr_idx1, spr_idx2, rendermode, finalfov);
	if (rendermode >= 0)
	{
		m_fFinalFOV = finalfov;
		m_iFlags |= HUD_ACTIVE;
		g_lastFOV = gHUD.GetUpdatedDefaultFOV();
		gHUD.m_Ammo.UpdateCrosshair(1, 1);
	}
	else
	{
		m_fFinalFOV = 0;// gHUD.GetUpdatedDefaultFOV(); TODO: restore FOV gradually
		m_iFlags &= ~HUD_ACTIVE;
		g_lastFOV = gHUD.m_iFOV = gHUD.GetUpdatedDefaultFOV();
		gHUD.m_Ammo.UpdateCrosshair(1, 0);
	}

#ifdef XDMOLDCODE3035
	// XDM3035a: somehow server may send invalid sprites :(
	if (m_iTextureIndex1 != spr_idx1)
	{
		m_iTextureIndex1 = spr_idx1;
		VidInit();
	}
#endif
	m_iRenderMode = rendermode;
}

void CHudZoomCrosshair::InitHUDData(void)
{
	m_iRenderMode = -1;
	m_iFlags = 0;
}

void CHudZoomCrosshair::Think(void)
{
	if ((m_iFlags & HUD_ACTIVE) && (gHUD.m_iPaused <= 0) && (gHUD.m_iKeyBits & IN_ATTACK2))
	{
		float currentfov = HUD_GetFOV();
		if (currentfov > m_fFinalFOV)// zooming in
		{
			if (m_fPlaySoundTime <= gEngfuncs.GetClientTime())
			{
				float df = gHUD.GetUpdatedDefaultFOV();
				float k = (df-g_lastFOV)/(df-m_fFinalFOV);// fov will go from 90 to ~30, k = 0...1
//				CON_DPRINTF("CHudZoomCrosshair::Think() k = %f!\n", k);
				EMIT_SOUND(gEngfuncs.GetLocalPlayer()->index, gHUD.m_LocalPlayerState.origin, CHAN_WEAPON, "weapons/zoom.wav", VOL_NORM, ATTN_IDLE, 0, 90+(int)(20.0f*k));
				m_fPlaySoundTime = gEngfuncs.GetClientTime() + 0.04f;
			}
			g_lastFOV -= 3.0f;
			gHUD.m_iFOV = g_lastFOV;
		}
		else
			m_fFinalFOV = currentfov;
	}
}
