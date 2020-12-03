#include "hud.h"
#include "cl_util.h"
#include "RenderManager.h"
#include "RenderSystem.h"
#include "RSModel.h"
#include "entity_types.h"
#include "bsputil.h"

CRSModel::CRSModel(void)
{
	// Calling constructors directly is forbidden!
	ResetParameters();
}

CRSModel::~CRSModel(void)
{
	KillSystem();
}

//-----------------------------------------------------------------------------
// Purpose: Static model
// Input  : origin - 
//			framerate - 
//			timetolive - 0 means the system removes itself after the last frame
//-----------------------------------------------------------------------------
CRSModel::CRSModel(const Vector &origin, const Vector &angles, const Vector &velocity, int entindex, int modelindex, int body, int skin, int sequence, int r_mode, int r_fx, byte r, byte g, byte b, float a, float adelta, float scale, float scaledelta, float framerate, float timetolive)
{
	index = 0;// the only good place for this
	removenow = false;
	m_pTexture = NULL;// MUST be before InitTexture(), InitModel() and ResetParameters()
	m_pModel = NULL;
	m_pEntity = NULL;
	ResetParameters();

	if (!InitModel(modelindex))
	{
		removenow = true;
		return;
	}

//	InitTexture(sprindex);// can be NULL

	m_pEntity = new cl_entity_t;
	memset(m_pEntity, 0, sizeof(cl_entity_t));

	m_vecOrigin = origin;
	m_vecAngles = angles;
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
	m_iFollowEntity = entindex;

	m_pEntity->index = entindex;
	m_pEntity->model = m_pModel;
	m_pEntity->player = false;
	m_pEntity->current_position = 0;
	m_pEntity->efrag = NULL;
	m_pEntity->topnode = NULL;
//	m_pEntity->origin = m_vecOrigin;
//	m_pEntity->angles = m_vecAngles;
	VectorCopy(m_vecOrigin, m_pEntity->origin);
	VectorCopy(m_vecAngles, m_pEntity->angles);
	VectorClear(m_pEntity->baseline.startpos);
	VectorClear(m_pEntity->baseline.endpos);
	VectorClear(m_pEntity->baseline.velocity);
	m_pEntity->baseline.entityType = ET_NORMAL;
	m_pEntity->baseline.number = -1;
	m_pEntity->baseline.gravity = 0.0f;
	m_pEntity->baseline.solid = SOLID_NOT;
	m_pEntity->baseline.movetype = MOVETYPE_NONE;
	m_pEntity->baseline.frame = 0;
//	m_pEntity->curstate.effects		= m_pEntity->baseline.effects = EF_INVLIGHT;//effects;
	m_pEntity->curstate.eflags		= m_pEntity->baseline.eflags = EFLAG_DRAW_ALWAYS;
	VectorCopy(origin, m_pEntity->baseline.origin);
	VectorCopy(origin, m_pEntity->curstate.origin);
	VectorCopy(angles, m_pEntity->baseline.angles);
	VectorCopy(angles, m_pEntity->curstate.angles);
//	m_pEntity->curstate.origin		= m_pEntity->baseline.origin = m_vecOrigin;
//	m_pEntity->curstate.angles		= m_pEntity->baseline.angles = m_vecAngles;
	m_pEntity->curstate.rendermode	= m_pEntity->baseline.rendermode = m_iRenderMode;
	m_pEntity->curstate.renderfx	= m_pEntity->baseline.renderfx = r_fx;
	m_pEntity->curstate.renderamt	= m_pEntity->baseline.renderamt = (int)(m_fBrightness*255.0f);
	m_pEntity->curstate.rendercolor.r=m_pEntity->baseline.rendercolor.r = m_color.r;
	m_pEntity->curstate.rendercolor.g=m_pEntity->baseline.rendercolor.g = m_color.g;
	m_pEntity->curstate.rendercolor.b=m_pEntity->baseline.rendercolor.b = m_color.b;
	m_pEntity->curstate.framerate	= m_pEntity->baseline.framerate = m_fFrameRate;
	m_pEntity->curstate.body		= m_pEntity->baseline.body = body;
	m_pEntity->curstate.skin		= m_pEntity->baseline.skin = skin;
	m_pEntity->curstate.scale		= m_pEntity->baseline.scale = m_fScale;
	m_pEntity->curstate.sequence	= m_pEntity->baseline.sequence = sequence;

	if (timetolive <= 0.0f)// if 0, just display all frames
		m_fDieTime = 0.0f;
	else
		m_fDieTime = gEngfuncs.GetClientTime() + timetolive;

//	m_iFollowFlags |= RENDERSYSTEM_FFLAG_DONTFOLLOW;// use externally on static models
	InitializeSystem();
}

//-----------------------------------------------------------------------------
// Purpose: Set default (external, non-system) values for all class variables.
//   Each derived class MUST call its ParentClass::ResetParameters()!
//-----------------------------------------------------------------------------
void CRSModel::ResetParameters(void)
{
	m_iBSPLeaf = 0;// -1 ??
	CRenderSystem::ResetParameters();
	if (m_pEntity)
	{
		VectorClear(m_pEntity->baseline.startpos);
		VectorClear(m_pEntity->baseline.endpos);
		m_pEntity->baseline.gravity = 0.0f;
		m_pEntity->baseline.solid = SOLID_NOT;
		m_pEntity->baseline.movetype = MOVETYPE_NONE;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Clear-out and free dynamically allocated memory
//-----------------------------------------------------------------------------
void CRSModel::KillSystem(void)
{
	if (m_pEntity)
	{
		delete m_pEntity;
		m_pEntity = NULL;
	}
	CRenderSystem::KillSystem();
}

//-----------------------------------------------------------------------------
// Purpose: Update system parameters along with time
//			DO NOT PERFORM ANY DRAWING HERE!
// Input  : &time - current client time
//			&elapsedTime - time elapsed since last frame
// Output : Returns true if needs to be removed
//-----------------------------------------------------------------------------
bool CRSModel::Update(const float &time, const double &elapsedTime)
{
	CRenderSystem::Update(time, elapsedTime);

	if (!dying)
	{
		if (m_pEntity)
		{
//			UpdateFrame(time, elapsedTime);// for texture
//			m_fFrame += m_fFrameRate * elapsedTime;
			m_color.r += (int)(m_fColorDelta[0] * elapsedTime);
			m_color.g += (int)(m_fColorDelta[1] * elapsedTime);
			m_color.b += (int)(m_fColorDelta[2] * elapsedTime);
			m_fScale += m_fScaleDelta * (float)elapsedTime;
			VectorCopy(m_vecOrigin, m_pEntity->curstate.origin);
			VectorCopy(m_vecAngles, m_pEntity->curstate.angles);
//			m_pEntity->curstate.origin = m_vecOrigin;
//			m_pEntity->curstate.angles = m_vecAngles;
			m_pEntity->curstate.rendermode = m_iRenderMode;
			m_pEntity->curstate.renderamt = (int)(m_fBrightness*255.0f);
			m_pEntity->curstate.rendercolor.r = m_color.r;
			m_pEntity->curstate.rendercolor.g = m_color.g;
			m_pEntity->curstate.rendercolor.b = m_color.b;
			m_pEntity->curstate.scale = m_fScale;
//somehow models work without it			m_pEntity->curstate.frame = m_fFrame;
			m_pEntity->curstate.framerate = m_fFrameRate;
			m_pEntity->curstate.body = m_pEntity->baseline.body;
			m_pEntity->curstate.skin = m_pEntity->baseline.skin;
			m_pEntity->curstate.sequence = m_pEntity->baseline.sequence;
		}
	}
	return dying;
}

//-----------------------------------------------------------------------------
// Purpose: Draw system to screen. May get called in various situations, so
// DON'T change any RS variables here (do it in Update() instead).
//-----------------------------------------------------------------------------
void CRSModel::Render(void)
{
/*	if (m_pEntity && m_pModel)
	{
no		gEngfuncs.CL_CreateVisibleEntity(ET_NORMAL, m_pEntity);
		IEngineStudio.SetRenderModel(m_pModel);
	}*/
}

//-----------------------------------------------------------------------------
// Purpose: Called by the engine, allows to add user entities to render list
//-----------------------------------------------------------------------------
void CRSModel::CreateEntities(void)
{
	if (m_pEntity)
	{
		vec3_t dir;
		float l = 2048.0f;

		VectorSubtract(m_vecOrigin, g_vecViewOrigin, dir);
		l = VectorNormalize(dir);

		if (g_pCvarServerZMax && l >= g_pCvarServerZMax->value*0.9f)// clipped by sv_zmax
			return;

		if ((gHUD.m_iFogMode > 0) && (gHUD.m_flFogEnd > 32.0f) && (l >= gHUD.m_flFogEnd))// clipped by fog
			return;

// UNDONE: TODO: don't draw what shouldn't be drawn
// Don't simply check origin because it may be underground, in walls, etc. or the model may be large enough to show even when player is not facing it.
//		if (PointIsVisible(m_vecOrigin + (m_pModel->maxs - m_pModel->mins)*0.5f))// somehow all mins/maxs are 0 :(
// LAME! origin is not enough! Need to check BSP!		if (CL_CheckVisibility(m_pEntity->curstate.origin))
//		if (CL_CheckLeafVisibility(m_iBSPLeaf))

//		if (!Mod_CheckEntityPVS(m_pEntity))// XDM3035c: fast way to check visiblility
//			return;// TODO: still does not work as desired (especially on stuck-in-ground objects)

//0 0		if (!Mod_CheckBoxInPVS(m_pModel->mins, m_pModel->maxs))
//			return;

		gEngfuncs.CL_CreateVisibleEntity(ET_NORMAL, m_pEntity);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Load model by index
// Input  : texture_index - precached model index
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CRSModel::InitModel(int model_index)
{
	if (model_index <= 0)
		return false;

	if (m_pModel == NULL || m_iModelIndex != model_index)
	{
		model_s *pModel = IEngineStudio.GetModelByIndex(model_index);
		if (pModel == NULL || pModel->type != mod_studio)
		{
			CON_PRINTF("CRSModel::InitModel(%d) failed!\n", model_index);
			return false;
		}

		m_pModel = pModel;
		m_iModelIndex = model_index;

// Works, but still all zeroes
/*		studiohdr_t *m_pModelData = (studiohdr_t *)IEngineStudio.Mod_Extradata(pModel);
		if (m_pModelData)
		{
			m_pModel->mins = m_pModelData->bbmin;
			m_pModel->maxs = m_pModelData->bbmax;
		}*/
	}
	return true;
}
