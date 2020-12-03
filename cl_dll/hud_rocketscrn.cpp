#include "hud.h"
#include "cl_util.h"

int CHudRocketScreen::VidInit(void)
{
	m_hSprText = SPR_Load("sprites/rtext.spr");
	if (m_hSprText == NULL)
		return 0;

	m_iOffset = 0;
	m_iFrames = SPR_Frames(m_hSprText);
	return 1;
}

int CHudRocketScreen::Init(void)
{
	m_iFlags |= HUD_DRAW_ALWAYS;
	m_iOffset = 0;

	gHUD.AddHudElem(this);

	return 1;
}

int CHudRocketScreen::Draw(const float &flTime)
{
	m_iCurFrame = (int)(flTime * 8) % m_iFrames;// 8 frames per second

	int h = SPR_Height(m_hSprText, m_iCurFrame);

	m_iOffset = ((int)(flTime * 128) % h - h);// 128 pix/sec
	
	SPR_Set(m_hSprText, 255, 255, 255);

	for (; m_iOffset < ScreenHeight; m_iOffset += h)
		SPR_DrawAdditive(m_iCurFrame, 0, m_iOffset, NULL);

	return 1;
}
