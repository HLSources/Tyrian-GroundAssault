/*----------------------------------------------------------------
*	Render System
*
*	Copyright © 2001-2012, Xawari. All rights reserved.
*	Created for X-Half-Life: Deathmatch, a Half-Life modification.
*	http://x.netheaven.ru
*
*	This code partially depends on software technology
*	created by Valve LLC.
*
*	Author of this code allows redistribution and use of it only
*	in non-commercial (non-profit) and open-source projects.
*
*	If this code to be used along with Valve Gold Source(tm) Endine,
*	the developer must also accept and follow its license agreement.
*
*
*  USAGE NOTES:
* - please do not use vectors other than class Vector
* - please try to reuse as much code as possible and keep OOP model
* - optimize your code at the time you write it, consider every operator
* - please standartize commentaries as you see it is done here
*
*
* This source code contains no secret or confidential information.
*---------------------------------------------------------------*/
#include "hud.h"
/* or
#include "vector.h"
#include "const.h"
#include "cl_entity.h"
#include "hud_iface.h"
*/
#include "cl_util.h"
#include "RenderManager.h"
#include "RenderSystem.h"
#include "triangleapi.h"
#include "studio.h"
#include "bsputil.h"
#include "weapondef.h"

//-----------------------------------------------------------------------------
// Purpose: Default constructor. Should never be used.
//-----------------------------------------------------------------------------
CRenderSystem::CRenderSystem(void)
{
	ResetParameters();
/* should never be used anyway
	texindex = 0;
//?	m_pTexture = NULL;
	m_pNext = NULL;*/
	removenow = false;
	index = 0;
//	UpdateCallback = NULL;// !!!
}

//-----------------------------------------------------------------------------
// Purpose: Destructor. Destroy data by calling KillSystem(), not directly!
//-----------------------------------------------------------------------------
CRenderSystem::~CRenderSystem(void)
{
	// do not clear anything HERE, do it in KillSystem() because it's a base class
	KillSystem();
}

//-----------------------------------------------------------------------------
// Purpose: Main constructor for external use (with all nescessary parameters)
//			e,g, g_pRenderManager->AddSystem(new CRenderSystem(a,b,...), 0, -1);
// Input  : origin - absolute position
//			velocity - 
//			angles - 
//			sprindex - precached sprite index (used as texture)
//			r_mode - kRenderTransAdd
//			r,g,b - RGB (0...255 each)
//			a - alpha (0...1)
//			adelta - alpha velocity, any value is acceptable
//			scale - positive values base on texture size, negative are absolute
//			scaledelta - scale velocity, any value is acceptable
//			framerate - texture frame rate (if animated), == FPS if negative
//			timetolive - 0 means the system removes itself after the last frame
// Accepts flags: RENDERSYSTEM_FLAG_RANDOMFRAME | LOOPFRAMES | etc.
//-----------------------------------------------------------------------------
CRenderSystem::CRenderSystem(const Vector &origin, const Vector &velocity, const Vector &angles, int sprindex, int r_mode, byte r, byte g, byte b, float a, float adelta, float scale, float scaledelta, float frame, float framerate, float timetolive)
{
	index = 0;// the only good place for this
	removenow = false;
	// Calling constructors directly is forbidden!
	ResetParameters();// should be called in all constructors so no parameters will left uninitialized
	if (!InitTexture(sprindex))
	{
		dying = true;
		removenow = true;// tell render manager to delete this system
		return;// no texture - no system
	}

	m_vecOrigin = origin;
	m_vecVelocity = velocity;
	m_vecAngles = angles;
	NormalizeAngles(m_vecAngles);
	m_color.r = r;
	m_color.g = g;
	m_color.b = b;
	m_fBrightness = a;
	m_fBrightnessDelta = adelta;
	m_fScale = scale;
	m_fScaleDelta = scaledelta;
	m_iRenderMode = r_mode;
	m_fFrameRate = framerate;
	m_fFrame = frame;
	m_iFrame = frame;

	if (timetolive <= 0.0f)
		m_fDieTime = 0.0f;// persist forever OR cycle through all texture frames and die (depends on Update function)
	else
		m_fDieTime = gEngfuncs.GetClientTime() + timetolive;

	InitializeSystem();
}

//-----------------------------------------------------------------------------
// Purpose: Set default (external, public, non-system) values for all class variables.
//   Each derived class MUST call its ParentClass::ResetParameters()!
// DO NOT call any functions from here.
//-----------------------------------------------------------------------------
void CRenderSystem::ResetParameters(void)
{
//	CON_DPRINTF("CRenderSystem::ResetParameters()\n");
	m_fScale = 1.0f;
	m_fScaleDelta = 0.0f;
	m_fBrightness = 1.0f;
	m_fBrightnessDelta = 0.0f;
	m_color.r = 255;
	m_color.g = 255;
	m_color.b = 255;
/*	m_colordelta.r = 0;
	m_colordelta.g = 0;
	m_colordelta.b = 0;*/
	m_fColorDelta[0] = 0.0f;
	m_fColorDelta[1] = 0.0f;
	m_fColorDelta[2] = 0.0f;
	m_fSizeX = 1.0f;
	m_fSizeY = 1.0f;
	m_fStartTime = 0.0f;
	m_fDieTime = 0.0f;
	m_fFrameRate = 0.0f;
	m_iRenderMode = 0;
	m_iDrawContents = 0;
	m_fFrame = 0;
	m_iFlags = 0;
	m_iFollowFlags = 0;
	m_iFollowEntity = -1;// does not follow any entities by default
	m_iFollowAttachment = 32767;
	m_LastFollowedEntity = NULL;
	VectorClear(m_vecOrigin);
	VectorClear(m_vecVelocity);
	VectorClear(m_vecAngles);
// TEST	m_vecAngles.z = 90.0f;
	VectorClear(m_vecOffset);
	m_pTexture = NULL;
//	m_UpdateCallback = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Clear-out and free dynamically allocated memory
//-----------------------------------------------------------------------------
void CRenderSystem::KillSystem(void)
{
//	CON_DPRINTF("CRenderSystem::KillSystem()\n");
	m_iRenderMode = 0;
	m_fFrame = 0;
	m_iFlags = 0;
//	ResetParameters();
//	m_pTexture = NULL; // !! NEVER do this!
	texindex = 0;
//	UpdateCallback = NULL;// !!!
//	m_fStartTime = 0.0f;
/*
	NEVER RESET:
	m_pNext
	removenow
	index
*/
}

//-----------------------------------------------------------------------------
// Purpose: Initialize SYSTEM (non-user) startup variables.
// Warning: this function may only use variables set by constructor
// as other parameters may be set AFTER this instance has been created!
// Must be called from class constructor.
//-----------------------------------------------------------------------------
void CRenderSystem::InitializeSystem(void)
{
//	CON_DPRINTF("CRenderSystem::InitializeSystem()\n");
	m_fFrame = 0.0f;
	dying = false;
	removenow = false;
	m_fStartTime = gHUD.m_flTime;// XDM3035b: easy tracking of system lifetime

	// XDM3035: derived classes depend on this code
	if (m_fScale > 0.0f)// get texture sizes
	{
		if (m_pTexture)// some systems do not have texture
		{
			m_fSizeX = (m_pTexture->maxs[1] - m_pTexture->mins[1])*0.5f;
			m_fSizeY = (m_pTexture->maxs[2] - m_pTexture->mins[2])*0.5f;
		}
	}
	else
		m_fScale = -m_fScale;
}

//-----------------------------------------------------------------------------
// Purpose: Load texture by index. WARNING: sprite must be precached on server!
// Input  : texture_index - precached sprite index
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CRenderSystem::InitTexture(const int &texture_index)
{
	if (texture_index <= 0)
		return false;

	if (m_pTexture == NULL || texindex != texture_index)
	{
		model_s *pTexture = IEngineStudio.GetModelByIndex(texture_index);
		if (pTexture == NULL || pTexture->type != mod_sprite)
		{
			CON_DPRINTF("CRenderSystem::InitTexture(%d) failed!\n", texture_index);
			return false;
		}

		m_pTexture = pTexture;
		texindex = texture_index;
	}
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Update system parameters along with time
//			DO NOT PERFORM ANY DRAWING HERE!
//			This function may not be called every frame by the engine.
// Input  : &time - current client time
//			&elapsedTime - time elapsed since last frame
// Output : Returns true if needs to be removed
//-----------------------------------------------------------------------------
bool CRenderSystem::Update(const float &time, const double &elapsedTime)
{
	dying = false;

	if (m_fDieTime > 0.0f && m_fDieTime <= time)
		dying = true;

	m_fBrightness += m_fBrightnessDelta*(float)elapsedTime;// fix for software mode: don't allow to be negative

	if (dying == false)// TODO: revisit, this is common for all render systems
	{
		if (m_fBrightnessDelta < 0.0f && m_fBrightness <= 0.0f)// should we?
		{
			m_fBrightness = 0.0f;
			dying = true;
		}
		else if (m_fBrightnessDelta > 0.0f && m_fBrightness >= 1.0f)// overbrightening?!	
		{
			m_fBrightness = 1.0f;
//			dying = true;
		}
		//else dangerous condition! eternal system possible!
	}

// allow inversion?	if (!dying && m_fScale <= 0.000001 && m_fScaleDelta < 0.0)
//		dying = true;
// overscaling?	else if (m_fScale > 65536 && m_fScaleDelta > 0.0)
//		dying = true;

// UNDONE	if (!dying && UpdateCallback)
//		dying = UpdateCallback(this, time);

	if (m_fFrameRate > 0)
	UpdateFrame(time, elapsedTime);

	if (dying)// all vital calculations and checks should be made before this point
		return 1;

	FollowEntity();
//	if (!FollowEntity())// update only if not following???
	VectorMA(m_vecOrigin, elapsedTime, m_vecVelocity, m_vecOrigin);

	if (m_iDrawContents != 0 && (m_iFlags & RENDERSYSTEM_FLAG_INCONTENTSONLY))
	{
		if (!DrawContentsHas(gEngfuncs.PM_PointContents(m_vecOrigin, NULL)))
		{
			dying = true;
			return 1;
		}
	}
/*
	m_fColor[0] += m_fColorDelta[0] * elapsedTime;
	m_fColor[1] += m_fColorDelta[1] * elapsedTime;
	m_fColor[2] += m_fColorDelta[2] * elapsedTime;
	m_fColor[3] += m_fColorDelta[3] * elapsedTime;
*/
	m_color.r += (int)(m_fColorDelta[0] * elapsedTime);
	m_color.g += (int)(m_fColorDelta[1] * elapsedTime);
	m_color.b += (int)(m_fColorDelta[2] * elapsedTime);
	m_fScale += m_fScaleDelta*elapsedTime;

//	CON_DPRINTF("CRenderSystem(%d)::Update(): %d %d %d %g sc %g fm %g\n", index, m_color.r, m_color.g, m_color.b, m_fBrightness, m_fScale, m_fFrame);
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: Called by the engine, allows to add user entities to render list
// For HL-specific studio models only.
//-----------------------------------------------------------------------------
void CRenderSystem::CreateEntities(void)
{
}

//-----------------------------------------------------------------------------
// Purpose: Find and follow specified entity
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
cl_entity_t *CRenderSystem::FollowEntity(void)
{
	if (m_iFlags & RENDERSYSTEM_FLAG_DONTFOLLOW)
		return NULL;

	if (dying == false && m_iFollowEntity > 0)
	{
		cl_entity_t *ent = GetUpdatingEntity(m_iFollowEntity);// XDM3035c: TESTME: gEngfuncs.GetEntityByIndex(m_iFollowEntity);
		if (ent)// != NULL)
		{
			m_LastFollowedEntity = ent;
			FollowUpdatePosition();// Update system position now, some code may depend on it. Another call should be made from drawing function in some cases.

			if (m_iFollowFlags & RENDERSYSTEM_FFLAG_ICNF_NODRAW)
			{
				m_iFlags &= ~RENDERSYSTEM_FLAG_NODRAW;// search was successful, unhide
			}
		}
		else// Entity may got out of the PVS, or has been removed and this index will soon be occupied again!! Whad should we do?
		{
			m_LastFollowedEntity = NULL;
			if (m_iFollowFlags & RENDERSYSTEM_FFLAG_ICNF_STAYANDFORGET)
			{
				m_iFollowEntity = -1;// stop following, keep current location
			}
			if (m_iFollowFlags & RENDERSYSTEM_FFLAG_ICNF_REMOVE)
			{
				dying = true;// remove softly
			}
			if (m_iFollowFlags & RENDERSYSTEM_FFLAG_ICNF_NODRAW)
			{
				m_iFlags |= RENDERSYSTEM_FLAG_NODRAW;// hide
			}
		}
		return ent;// can be NULL
	}
	return NULL;
}


//-----------------------------------------------------------------------------
// Purpose: Fast and safe to call often.
// Some systems may require two or even three points, so leave it as virtual.
//-----------------------------------------------------------------------------
void CRenderSystem::FollowUpdatePosition(void)
{
	// XDM3035c: Update() is slow and we need position updated EVERY FRAME.
	if (m_iFollowEntity > 0 && m_LastFollowedEntity)
	{
		if (!(m_iFollowFlags & RENDERSYSTEM_FFLAG_DONTFOLLOW))
		{
//			studiohdr_t *pStudioHeader = (studiohdr_t *)IEngineStudio.Mod_Extradata(ent->model);// a little bit slow, but safe and reliable way to validate attachment
// Causes "Cache_UnlinkLRU: NULL link" error eventually			if (m_iFollowAttachment < MAXSTUDIOATTACHMENTS)//pStudioHeader->numattachments)
			if (m_LastFollowedEntity->model && (m_iFollowAttachment < MAXSTUDIOATTACHMENTS))//pStudioHeader->numattachments)
			{
				VectorCopy(m_LastFollowedEntity->attachment[m_iFollowAttachment], m_vecOrigin);
				m_vecAngles = m_LastFollowedEntity->curstate.angles;// XDM3035c: TESTME
			}
			else
			{
				if (m_iFollowFlags & RENDERSYSTEM_FFLAG_USEOFFSET)// XDM3035b: use user-specified offset in entity local coordinates (rotates according to entity angles)
				{
					Vector efw, ert, eup;
					AngleVectors(m_LastFollowedEntity->angles, efw, ert, eup);
					m_vecOrigin = m_LastFollowedEntity->origin + m_vecOffset[0]*ert + m_vecOffset[1]*efw + m_vecOffset[2]*eup;
				}
				else
					m_vecOrigin = m_LastFollowedEntity->origin;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Pick next texture frame if needed.
//			Internal frame counter must be a float to collect half-frames.
// Input  : &time - 
//			&elapsedTime - 
//-----------------------------------------------------------------------------
void CRenderSystem::UpdateFrame(const float &time, const double &elapsedTime)
{
	if (m_pTexture == NULL || m_pTexture->numframes <= 1)
		return;

	if (gHUD.m_iPaused > 0)
		return;

	if (m_iFlags & RENDERSYSTEM_FLAG_RANDOMFRAME)// UNDONE: display ALL frames but in RANDOM ORDER, then destroy if nescessary. Count displayed frames?
	{
		m_fFrame = (float)RANDOM_LONG(0, m_pTexture->numframes - 1);
		return;
	}

	if (m_fFrameRate < 0)// framerate == fps
	{
		m_fFrame += 1.0f;
	}
	else// custom framerate
	{
		m_fFrame += m_fFrameRate * elapsedTime;
	}
	// m_fFrameRate == 0.0 means single frame

	if ((int)m_fFrame >= m_pTexture->numframes)
	{
		// don't remove after last frame
		if (m_fDieTime == 0.0f)
		{
			if (!(m_iFlags & RENDERSYSTEM_FLAG_LOOPFRAMES))
			{
				dying = true;
				return;
			}
		}
		m_fFrame -= (int)m_fFrame;// = 0.0f; // leave fractional part
	}
}

//-----------------------------------------------------------------------------
// Purpose: An external phusical force must be applied. Wind, shockwave, etc.
// Input  : origin - 
//			force - 
//			radius - 
//			point - 
//-----------------------------------------------------------------------------
void CRenderSystem::ApplyForce(const Vector &origin, const Vector &force, float radius, bool point)
{
	// nothing here
}

//-----------------------------------------------------------------------------
// Purpose: Check specified 3D point for visibility. This function decides what is to be drawn.
// Be EXTREMELY careful with this when porting! This thing may screw up your whole work!
// Input  : &point - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CRenderSystem::PointIsVisible(const Vector &point)
{
	// TODO: make render systems render pass-dependant?
	//m_iRenderPass == gHUD.m_iRenderPass

	if (m_iFlags & RENDERSYSTEM_FLAG_DRAWALWAYS)// may be used by 3D skybox elements
		return true;

	return UTIL_PointIsVisible(point, true);
}

//-----------------------------------------------------------------------------
// Purpose: UNDONE It seems that the engine's HUD_Frame doesn't work as desired
//-----------------------------------------------------------------------------
/*void CRenderSystem::PreFrame(void)
{
	FollowUpdatePosition();
}*/

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
/*void CRenderSystem::RenderOpaque(void)
{
}*/

//-----------------------------------------------------------------------------
// Purpose: Draw system to screen. May get called in various situations, so
// DON'T change any RS variables here (do it in Update() instead).
//-----------------------------------------------------------------------------
void CRenderSystem::Render(void)
{
// handled globally by manager	if (m_iFlags & RENDERSYSTEM_FLAG_NODRAW)
//		return;

	if (!InitTexture(texindex))
		return;

	if (m_iDrawContents != 0)
	{
		if (!DrawContentsHas(gEngfuncs.PM_PointContents(m_vecOrigin, NULL)))
			return;
	}

	if (!gEngfuncs.pTriAPI->SpriteTexture(m_pTexture, (int)m_fFrame))
		return;

	FollowUpdatePosition();

	if (!CL_CheckVisibility(m_vecOrigin))// XDM3035a: TESTME! Should not allow systems to be rendered behind sky
		return;
/* we can check box too, but it's not needed
	Vector halfsize(m_fScale/2,m_fScale/2,m_fScale/2);
	if (!Mod_CheckBoxInPVS(m_vecOrigin-halfsize, m_vecOrigin+halfsize))// XDM3035c: fast way to check visiblility
		return;
*/
	Vector right, up;
	AngleVectors(m_vecAngles, NULL, right, up);

	if (m_fFrameRate > 0)
		gEngfuncs.pTriAPI->SpriteTexture(m_pTexture, m_fFrame);
	else
		gEngfuncs.pTriAPI->SpriteTexture(m_pTexture, m_iFrame);

	gEngfuncs.pTriAPI->RenderMode(m_iRenderMode);
//	if (CVAR_GET_FLOAT("test1") > 0)
		gEngfuncs.pTriAPI->Color4ub(m_color.r, m_color.g, m_color.b, 255);// ? (unsigned char)(m_fBrightness*255.0f));
//	else
//		gEngfuncs.pTriAPI->Color4ub(m_color.r, m_color.g, m_color.b, (unsigned char)(m_fBrightness*255.0f));

	gEngfuncs.pTriAPI->Brightness(m_fBrightness);
	gEngfuncs.pTriAPI->CullFace(TRI_NONE);
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);

	Vector rx = right*m_fSizeX*m_fScale;// tmp for faster code
	Vector uy = up*m_fSizeY*m_fScale;
	gEngfuncs.pTriAPI->TexCoord2f(0,0);
	gEngfuncs.pTriAPI->Vertex3fv(m_vecOrigin - rx + uy);
	gEngfuncs.pTriAPI->TexCoord2f(1,0);
	gEngfuncs.pTriAPI->Vertex3fv(m_vecOrigin + rx + uy);
	gEngfuncs.pTriAPI->TexCoord2f(1,1);
	gEngfuncs.pTriAPI->Vertex3fv(m_vecOrigin + rx - uy);
	gEngfuncs.pTriAPI->TexCoord2f(0,1);
	gEngfuncs.pTriAPI->Vertex3fv(m_vecOrigin - rx - uy);

	gEngfuncs.pTriAPI->End();
/* TEST
	gEngfuncs.pTriAPI->Begin(TRI_LINES);
	gEngfuncs.pTriAPI->TexCoord2f(0,0.5);
	gEngfuncs.pTriAPI->Vertex3fv(m_vecOrigin - rx);
	gEngfuncs.pTriAPI->TexCoord2f(1,0.5);
	gEngfuncs.pTriAPI->Vertex3fv(m_vecOrigin);
	gEngfuncs.pTriAPI->End();
*/
//	gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
}

void CRenderSystem::DrawContentsAdd(short contents)
{
	m_iDrawContents |= (1 << abs(contents));
}

void CRenderSystem::DrawContentsRemove(short contents)
{
	m_iDrawContents &= ~(1 << abs(contents));
}

bool CRenderSystem::DrawContentsHas(short contents)
{
	if (m_iDrawContents == 0)// no restrictions applied
		return true;// agree with everything

	return ((m_iDrawContents & (1 << abs(contents))) != 0);
}

void CRenderSystem::DrawContentsClear(void)
{
	m_iDrawContents = 0;
}

bool CRenderSystem::DrawContentsCheck(const Vector &origin)
{
	if (m_iDrawContents != 0)
	{
		if (!DrawContentsHas(gEngfuncs.PM_PointContents(origin, NULL)))
		{
			return false;
		}
	}
	return true;
}
