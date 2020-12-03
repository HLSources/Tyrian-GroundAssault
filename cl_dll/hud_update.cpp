/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
//  hud_update.cpp
//

#include "hud.h"
#include "cl_util.h"
#include "in_defs.h"

//-----------------------------------------------------------------------------
// Purpose: called every time shared client dll/engine data gets changed,
// Input  : *cdata - 
//			time - 
// Output : return 1 if in anything in the client_data struct has been changed, 0 otherwise
//-----------------------------------------------------------------------------
int CHud::UpdateClientData(client_data_t *cdata, const float &time)
{
	VectorCopy(cdata->origin, m_vecOrigin);// XDM: ?
	VectorCopy(cdata->viewangles, m_vecAngles);

	m_iKeyBits = CL_ButtonBits(0);
	m_iWeaponBits = cdata->iWeaponBits;

//	in_fov = cdata->fov;

	Think();

	cdata->fov = m_iFOV;
//	v_idlescale = m_iConcussionEffect;

	CL_ResetButtonBits(m_iKeyBits);
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: update data, no drawing
//-----------------------------------------------------------------------------
void CHud::Think(void)
{
	HUDLIST *pList = m_pHudList;
	while (pList)
	{
		if (pList->p->IsActive())
			pList->p->Think();
		pList = pList->pNext;
	}

	float def_fov = GetUpdatedDefaultFOV();// XDM
	float newfov = HUD_GetFOV();

	if (newfov == 0)
	{
		m_iFOV = def_fov;
	}
	else
	{
		if (m_iFOV != newfov)
		{
			m_iFOV = newfov;
			m_flMouseSensitivity = GetSensitivityByFOV(m_iFOV);// XDM
		}
	}

	if (gEngfuncs.GetMaxClients() > 1)// XDM3037: multiplayer
		CheckRemainingTimeAnnouncements();
}
