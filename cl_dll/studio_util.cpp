//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include <memory.h>
#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "com_model.h"
#include "r_studioint.h"
#include "studio_util.h"


//-----------------------------------------------------------------------------
// Purpose: Easier and safer version of model_t.type
// Input  : modelIndex - 
// Output : int modtype_t
//-----------------------------------------------------------------------------
int Mod_GetType(int modelIndex)
{
	model_t *m_pModel = IEngineStudio.GetModelByIndex(modelIndex);
	if (m_pModel == NULL)
		return 255;

	return m_pModel->type;
}
