#include "hud.h"
#include "cl_util.h"
#include "cl_entity.h"
#include "bspfile.h"
#include "event_api.h"
#include "pm_defs.h"
#include "pmtrace.h"
#include "bsputil.h"

mleaf_t *g_pViewLeaf = NULL;// leaf where current view origin resides, CAN BE NULL!!!

// Most of this code is a mixture of Half-Life, Quake and Xash3D code
// XHL does not hold any responsibility for whatever harm it may cause
// (^_^)


//-----------------------------------------------------------------------------
// Purpose: Update VIS-related globals for this frame
// Input  : time - 
// Output : void DLLEXPORT
//-----------------------------------------------------------------------------
void VIS_Frame(const double time)
{
	if (g_pWorld && g_pWorld->model)
		g_pViewLeaf = Mod_LeafForPoint(g_vecViewOrigin, (mnode_t *)g_pWorld->model->nodes);
	else
		g_pViewLeaf = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Find entity by classname in the loaded BSP data and get its parameters
// Input  : *name - classname
//			&origin &angles - output
// Output : int
//-----------------------------------------------------------------------------
bool UTIL_FindEntityInMap(const char *classname, Vector &origin, Vector &angles)//float *origin, float *angle)
{
//	cl_entity_t *pEnt = gEngfuncs.GetEntityByIndex(0);	// get world model
	cl_entity_t *pEnt = g_pWorld;
	if (pEnt == NULL)
		return 0;
	if (pEnt->model == NULL)
		return 0;

	char *data = pEnt->model->entities;
	int n;
	char keyname[256];
	char token[1024];
	bool found = false;

	while (data)
	{
		data = gEngfuncs.COM_ParseFile(data, token);
		
		if ( (token[0] == '}') ||  (token[0]==0) )
			break;

		if (!data)
		{
			CON_DPRINTF("UTIL_FindEntityInMap: EOF without closing brace\n");
			return 0;
		}

		if (token[0] != '{')
		{
			CON_DPRINTF("UTIL_FindEntityInMap: expected {\n");
			return 0;
		}

		// we parse the first { now parse entities properties
		while (1)
		{	
			// parse key
			data = gEngfuncs.COM_ParseFile(data, token);
			if (token[0] == '}')
				break; // finish parsing this entity

			if (!data)
			{	
				CON_DPRINTF("UTIL_FindEntityInMap: EOF without closing brace\n");
				return 0;
			}

			strcpy(keyname, token);

			// another hack to fix keynames with trailing spaces
			n = strlen(keyname);
			while (n && keyname[n-1] == ' ')
			{
				keyname[n-1] = 0;
				n--;
			}
			
			// parse value	
			data = gEngfuncs.COM_ParseFile(data, token);
			if (!data)
			{	
				CON_DPRINTF("UTIL_FindEntityInMap: EOF without closing brace!\n");
				return 0;
			}
	
			if (token[0] == '}')
			{
				CON_DPRINTF("UTIL_FindEntityInMap: closing brace without data\n");
				return 0;
			}

			if (!strcmp(keyname,"classname"))
			{
				if (!strcmp(token, classname))
					found = 1;	// thats our entity
			}
			else if (!strcmp(keyname, "angle"))
			{
				float y = (float)atof(token);
				if (y >= 0)
				{
					angles[0] = 0.0f;
					angles[1] = y;
				}
				else if ((int)y == -1)
				{
					angles[0] = -90.0f;
					angles[1] =   0.0f;;
				}
				else
				{
					angles[0] = 90.0f;
					angles[1] =  0.0f;
				}
				angles[2] =  0.0f;
			}
			else if (!strcmp(keyname, "angles"))
			{
				StringToVec(token, angles);
			}
			else if (!strcmp(keyname,"origin"))
			{
				StringToVec(token, origin);
			}
		}// while (1)

		if (found)
			return 1;
	}
	return 0;	// we search all entities, but didn't found the correct
}




// Xash code?
/*
qboolean CL_HeadnodeVisible(mnode_t *node, byte *visbits)
{
	if (g_pWorld == NULL)
		return false;

	mleaf_t	*leaf;
	int	leafnum;

	if (node->contents < 0)
	{
		if (node->contents != CONTENTS_SOLID)
		{
			leaf = (mleaf_t *)node;
			leafnum = leaf - g_pWorld->model->leafs - 1;

			if (visbits[leafnum >> 3] & (1<<(leafnum & 7)))
				return true;
		}
		return false;
	}

	if (CL_HeadnodeVisible(node->children[0], visbits))
		return true;

	return CL_HeadnodeVisible(node->children[1], visbits);
}

// Xash code?
int CL_CheckVisibility(const edict_t *ent, byte *pset)
{
	int	result = 0;

	if (ent == NULL)
		return 0;

	if (pset == NULL)// vis not set - fullvis enabled
		return 1;

//	g_pWorld = gEngfuncs.GetEntityByIndex(0);

	if (ent->headnode == -1)
	{
		int	i;

		// check individual leafs
		for (i = 0; i < ent->num_leafs; ++i)
		{
			if( pset[ent->leafnums[i] >> 3] & (1 << (ent->leafnums[i] & 7)))
				break;
		}

		if (i == ent->num_leafs)
			return 0;	// not visible

		result = 1;	// visible passed by leafs
	}
	else
	{
		mnode_t	*node;

		if (ent->headnode < 0)
			node = (mnode_t *)(g_pWorld->model->leafs + (-1 - ent->headnode));			
		else
			node = g_pWorld->model->nodes + ent->headnode;

		// too many leafs for individual check, go by headnode
		if (!CL_HeadnodeVisible(node, pset))
			return 0;

		result = 2;	// visible passed by headnode
	}

#if 0
	// NOTE: uncomment this if you want to get more accuracy culling on large brushes
	if (Mod_GetType(ent->v.modelindex) == mod_brush)
	{
		if (!Mod_BoxVisible(ent->v.absmin, ent->v.absmax, pset))
			return 0;

		result = 3; // visible passed by BoxVisible
	}
#endif
	return result;
}
*/


//-----------------------------------------------------------------------------
// Purpose: BoxOnPlaneSide
// Input  : &emins &emaxs - the box
//			*p - the plane
// Output : int 1, 2, or 1 + 2
//-----------------------------------------------------------------------------
int BoxOnPlaneSide(const Vector &emins, const Vector &emaxs, const mplane_t *p)
{
	float dist1, dist2;
	int	sides = 0;
	// general case
	switch(p->signbits)
	{
	case 0:
		dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
		dist2 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
		break;
	case 1:
		dist1 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
		dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
		break;
	case 2:
		dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
		dist2 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
		break;
	case 3:
		dist1 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
		dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
		break;
	case 4:
		dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
		dist2 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
		break;
	case 5:
		dist1 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
		dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
		break;
	case 6:
		dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
		dist2 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
		break;
	case 7:
		dist1 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
		dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
		break;
	default:
		// shut up compiler
		dist1 = dist2 = 0;
		break;
	}

	if (dist1 >= p->dist)
		sides = 1;
	if (dist2 < p->dist)
		sides |= 2;

	return sides;
}

//-----------------------------------------------------------------------------
// Purpose: BOX_ON_PLANE_SIDE top-level wrapper
// Input  : &emins &emaxs - the box
//			*p - the plane
// Output : int
//-----------------------------------------------------------------------------
int BOX_ON_PLANE_SIDE(const Vector &emins, const Vector &emaxs, const mplane_t *p)
{
	if (p->type < 3)
	{
		if (p->dist <= emins[(int)p->type])
			return 1;
		else if (p->dist >= emaxs[(int)p->type])
			return 2;
		else
			return 3;
	}
	else
		return BoxOnPlaneSide(emins, emaxs, p);
}

//-----------------------------------------------------------------------------
// Purpose: EXPERIMENTAL: find specified leaf in world's leafs marked for drawing
// Input  : *pSearchLeaf - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool Mod_FindLeafInVisible(mleaf_t *pSearchLeaf)
{
	if (pSearchLeaf == NULL || g_pWorld == NULL)
		return false;

	for (int i = 0; i < g_pWorld->model->numleafs; ++i)
	{
		mleaf_t *pLeaf = &g_pWorld->model->leafs[i];
		if (pLeaf)//pLeaf->visframe == framecount)
		{
			msurface_t **pMarkSurfaces = pLeaf->firstmarksurface;
			if ((pMarkSurfaces != NULL) && (pLeaf->nummarksurfaces > 0))// this world leaf is visible (has marked surfaces)
			{
				if (pSearchLeaf == pLeaf)
					return true;
			}
		}
	}
	return false;// not found in visible leaf list
}


//-----------------------------------------------------------------------------
// Purpose: Fills leaflist_t
// from Xash3D
// Input  : *ll - input and output data
//			*node - 
//			*worldmodel - 
//-----------------------------------------------------------------------------
void Mod_BoxLeafnums_r(leaflist_t *ll, mnode_t *node, model_t *worldmodel)
{
	mplane_t *plane;
	int	s;
	while (node)// XDM
	{
		if (node->contents == CONTENTS_SOLID)
			return;

		if (node->contents < 0)
		{
			mleaf_t	*leaf = (mleaf_t *)node;
			// it's a leaf!
			if (ll->count >= ll->maxcount)
			{
				ll->overflowed = true;
				return;
			}

			ll->list[ll->count++] = leaf - worldmodel->leafs - 1;
			return;
		}

		plane = node->plane;
		s = BOX_ON_PLANE_SIDE(ll->mins, ll->maxs, plane);

		if (s == 1)
		{
			node = node->children[0];
		}
		else if (s == 2)
		{
			node = node->children[1];
		}
		else// go down both
		{
			if (ll->topnode == -1)
				ll->topnode = node - worldmodel->nodes;

			Mod_BoxLeafnums_r(ll, node->children[0], worldmodel);
			node = node->children[1];
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Get leafs as bits
// Input  : mins maxs - box
//			*list - output
//			listsize - 
//			*topnode - output
// Output : int count
//-----------------------------------------------------------------------------
int Mod_BoxLeafnums(const Vector mins, const Vector maxs, short *list, int listsize, int *topnode)
{
	if (g_pWorld->model == NULL)
		return 0;

	leaflist_t ll;
	ll.mins = mins;
	ll.maxs = maxs;
	ll.count = 0;
	ll.maxcount = listsize;
	ll.list = list;
	ll.topnode = -1;
	ll.overflowed = false;
	Mod_BoxLeafnums_r(&ll, g_pWorld->model->nodes, g_pWorld->model);
	if (topnode) *topnode = ll.topnode;
	return ll.count;
}

//-----------------------------------------------------------------------------
// Purpose: Returns true if any leaf in boxspace is potentially visible
// Input  : &mins &maxs - the box
//			*visbits - PVS
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool Mod_BoxVisible(const Vector &mins, const Vector &maxs, const byte *visbits)
{
	if (visbits == NULL || mins.IsZero() || maxs.IsZero())
		return true;

	short	leafList[256];
	int	i, leafnum, count;
	count = Mod_BoxLeafnums(mins, maxs, leafList, 256, NULL);
	for (i = 0; i < count; ++i)
	{
		leafnum = leafList[i];
		if (leafnum != -1 && visbits[leafnum>>3] & (1<<(leafnum & 7)))
			return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: RLE decompression of VIS data
// Input  : *in - vis info
//			*model - BSP model
// Output : byte 
//-----------------------------------------------------------------------------
byte *Mod_DecompressVis(byte *in, model_t *model)
{
	static byte	decompressed[MAX_MAP_LEAFS/8];
	int row = (model->numleafs+7)>>3;	
	byte *out = decompressed;

	if (in == NULL)
	{	// no vis info, so make all visible
		while(row)
		{
			*out++ = 0xff;
			row--;
		}
		return decompressed;
	}
	int c;
	do
	{
		if (*in)
		{
			*out++ = *in++;
			continue;
		}
		c = in[1];
		in += 2;
		while(c)
		{
			*out++ = 0;
			c--;
		}
	} while (out - decompressed < row);

	return decompressed;
}

byte *Mod_LeafPVS(mleaf_t *leaf, model_t *model)
{
	if (leaf == NULL || model == NULL || leaf == model->leafs || !model->visdata)
		return Mod_DecompressVis(NULL, model);

	return Mod_DecompressVis(leaf->compressed_vis, model);
}

mleaf_t *Mod_LeafForPoint(const Vector &p, mnode_t *startnode)
{
//	try
//	{
	mnode_t *node = startnode;
	while (node)// XDM
	{
		if (node->contents < 0)
			return (mleaf_t *)node;

		if (node->children)
			node = node->children[(PlaneDiff(p, node->plane) < 0)?1:0];
		else
			break;

		if (node == startnode)
			break;
	}
/*	}
	catch(...)
	{
		CON_PRINTF("Mod_LeafForPoint exception!\n");
	}*/
	return NULL;
}

byte *Mod_GetCurrentVis(void)
{
//?	mleaf_t *pViewLeaf = Mod_LeafForPoint(g_vecViewOrigin, (mnode_t *)g_pWorld->topnode);
//	mleaf_t *pViewLeaf = Mod_LeafForPoint(g_vecViewOrigin, (mnode_t *)g_pWorld->model->nodes);
	if (g_pViewLeaf)
		return Mod_LeafPVS(g_pViewLeaf, g_pWorld->model);

	return NULL;
}





//-----------------------------------------------------------------------------
// Purpose: Usable: check if a point is in PVS
// Input  : &origin - any point, absolute world coordinates
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool Mod_CheckPointInPVS(const Vector &point)
{
	if (g_pViewLeaf == NULL)
		return false;

	mleaf_t *pPointLeaf = Mod_LeafForPoint(point, (mnode_t *)g_pWorld->model->nodes);
	if (pPointLeaf)
	{
		if (pPointLeaf->visframe == g_pViewLeaf->visframe)// what a neat trick
		{
// debug			gEngfuncs.pEfxAPI->R_SparkEffect(point, 8, -10, 10);
			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Usable: check if a box is in PVS
// Input  : &absmins &absmaxs - absolute world coordinates
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool Mod_CheckBoxInPVS(const Vector &absmins, const Vector &absmaxs)
{
	try
	{
		return Mod_BoxVisible(absmins, absmaxs, Mod_GetCurrentVis());// corrupted memory often occurs there
	}
	catch(...)
	{
		CON_PRINTF("Mod_CheckBoxInPVS() exception!\n");
		return true;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Usable: check if an entity is in PVS (by its mins/maxs box)
// Input  : *ent - entity
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool Mod_CheckEntityPVS(cl_entity_t *ent)
{
	if (ent == NULL || ent->index == 0)
		return false;

//	if (ent->curstate.messagenum != gHUD.m_LocalPlayerState.messagenum)
//		return false;// already culled by server

	Vector mins = ent->curstate.origin + ent->curstate.mins;
	Vector maxs = ent->curstate.origin + ent->curstate.maxs;
	return Mod_CheckBoxInPVS(mins, maxs);
}


//-----------------------------------------------------------------------------
// Purpose: Usable and safe for all render modes: check point visibility
// Input  : &origin - any point, absolute world coordinates
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CL_CheckVisibility(const Vector &point)
{
	if (gHUD.m_iHardwareMode == 0)// structures are different in software mode
	{
		pmtrace_t pmtrace;
		gEngfuncs.pEventAPI->EV_SetTraceHull(2);
		gEngfuncs.pEventAPI->EV_PlayerTrace((float *)point, g_vecViewOrigin, PM_STUDIO_IGNORE|PM_GLASS_IGNORE|PM_WORLD_ONLY, -1, &pmtrace);
		// since we can't check BSP leafs and nodes (all NULL), we use this simple method
		if (pmtrace.fraction >= 1.0f || (pmtrace.startsolid && pmtrace.fraction == 0.0f))
			return true;
	}
	else
	{
//		if (!UTIL_PointIsFar(point))// HACK: override nearby objects. Keep until Mod_CheckPointInPVS() glitches are fixed (DOM_Forest)
//			return true;

		return Mod_CheckPointInPVS(point);
	}
	return false;
}
