//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef SPECTATOR_H
#define SPECTATOR_H
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */

#include "cl_entity.h"

#define INSET_OFF				0
#define	INSET_CHASE_FREE		1
#define	INSET_IN_EYE			2
#define	INSET_MAP_FREE			3
#define	INSET_MAP_CHASE			4

#define MAX_SPEC_HUD_MESSAGES	8


#define OVERVIEW_TILE_SIZE		128		// don't change this
#define OVERVIEW_MAX_LAYERS		6		// XDM
#define OVERVIEW_MAX_ENTITIES	128


typedef struct overviewInfo_s
{
	char		map[MAX_MAPNAME];	// cl.levelname or empty
	vec3_t		origin;		// center of map
	float		zoom;		// zoom of map images
	int			layers;		// how may layers do we have
	float		layersHeights[OVERVIEW_MAX_LAYERS];
	char		layersImages[OVERVIEW_MAX_LAYERS][255];
//	int			layersRenderModes[OVERVIEW_MAX_LAYERS];// XDM
	qboolean	rotated;	// are map images rotated (90 degrees) ?
	int			insetWindowX;
	int			insetWindowY;
	int			insetWindowHeight;
	int			insetWindowWidth;
} overviewInfo_t;

typedef struct overviewEntity_s
{
//	HSPRITE				hSprite;
	struct cl_entity_s	*entity;
	float/*double*/		killTime;// XDM3035a: GetClientTime does not offer double precision
	struct model_s		*sprite;
//	float				scale;
//	bool				viewport_parallel;// XDM3035c: parallel to viewport
} overviewEntity_t;


//-----------------------------------------------------------------------------
// Purpose: Handles the drawing of the spectator stuff (camera & top-down map and all the things on it )
//-----------------------------------------------------------------------------
class CHudSpectator : public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	virtual void InitHUDData(void);
	virtual void Reset(void);

	int ToggleInset(int newInsetMode, bool allowOff);
	void CheckSettings(void);
	bool AddOverviewEntity(struct cl_entity_s *ent, struct model_s *icon, float lifetime);
	bool AddOverviewEntityToList(cl_entity_t *ent, const struct model_s *sprite, float killTime);
	void DeathMessage(int victim);
	void CheckOverviewEntities(void);
	int DrawOverview(void);
	void DrawOverviewEntities(void);
	void GetMapPosition(float *returnvec);
	void DrawOverviewLayer(void);
	void LoadMapSprites(void);
	bool ParseOverviewFile(void);
	void SetModes(int iMainMode, int iInsetMode);
	void HandleButtonsDown(int ButtonPressed);
	void HandleButtonsUp(int ButtonPressed);
	void FindNextPlayer(bool bReverse);
	void SetSpectatorStartPosition(void);
//	void DumpOverviewEnts(void);// XDM
	bool ShouldDrawOverview(void);// XDM

	void DirectorMessage(int iSize, void *pbuf);

	int m_iDrawCycle;
	client_textmessage_t m_HUDMessages[MAX_SPEC_HUD_MESSAGES];
	char				m_HUDMessageText[MAX_SPEC_HUD_MESSAGES][128];
	int					m_lastHudMessage;
	overviewInfo_t		m_OverviewData;
	overviewEntity_t	m_OverviewEntities[OVERVIEW_MAX_ENTITIES];
	int					m_iObserverFlags;
	int					m_iSpectatorNumber;
	float				m_mapZoom;		// zoom the user currently uses
	vec3_t				m_mapOrigin;	// origin where user rotates around
	cvar_t *			m_drawnames;
	cvar_t *			m_drawcone;
	cvar_t *			m_drawstatus;
	cvar_t *			m_autoDirector;
//	cvar_t *			m_pip;
	int m_iInsetMode;// XDM
	qboolean			m_chatEnabled;
	vec3_t				m_cameraOrigin;	// a help camera
	vec3_t				m_cameraAngles;	// and it's angles
//	Vector				m_vecViewPortAngles;

private:
	vec3_t		m_vPlayerPos[MAX_PLAYERS];// XDM3035: TESTME
	HSPRITE		m_hsprPlayer;
	HSPRITE		m_hsprCamera;
	HSPRITE		m_hsprPlayerDead;
	HSPRITE		m_hsprViewcone;
	HSPRITE		m_hsprUnkownMap;
	HSPRITE		m_hsprBeam;
	HSPRITE		m_hCrosshair;
	wrect_t		m_crosshairRect;
	struct model_s *m_DefaultSprite;// XDM
	struct model_s *m_MapSprite[OVERVIEW_MAX_LAYERS];// each layer image is saved in one sprite, where each tile is a sprite frame
	float		m_flNextObserverInput;
	float		m_zoomDelta;
	float		m_moveDelta;
	int			m_lastPrimaryObject;
	int			m_lastSecondaryObject;
};

#endif // SPECTATOR_H
