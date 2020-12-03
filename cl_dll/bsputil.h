//====================================================================
//
// Purpose: BSP-related utility functions
//
//====================================================================

#ifndef BSPUTIL_H
#define BSPUTIL_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* !__MINGW32__ */
#endif

#include "com_model.h"

extern cl_entity_t *g_pWorld;


// XDM3035c: from Xash3D project
typedef struct leaflist_s
{
	int		count;
	int		maxcount;
	bool	overflowed;
	short	*list;
	Vector	mins, maxs;
	int		topnode;		// for overflows where each leaf can't be stored individually
} leaflist_t;


bool UTIL_FindEntityInMap(const char *classname, Vector &origin, Vector &angles);//float *origin, float *angle);

void VIS_Frame(const double time);
// from Xash3D
void Mod_BoxLeafnums_r(leaflist_t *ll, mnode_t *node, model_t *worldmodel);
int Mod_BoxLeafnums(const Vector mins, const Vector maxs, short *list, int listsize, int *topnode);
bool Mod_PointVisible(const Vector &point, const byte *visbits);
bool Mod_BoxVisible(const Vector &mins, const Vector &maxs, const byte *visbits);
byte *Mod_DecompressVis(byte *in, model_t *model);
byte *Mod_LeafPVS(mleaf_t *leaf, model_t *model);
mleaf_t *Mod_LeafForPoint(const Vector &p, mnode_t *node);
byte *Mod_GetCurrentVis(void);

bool Mod_CheckPointInPVS(const Vector &point);
bool Mod_CheckBoxInPVS(const Vector &absmins, const Vector &absmaxs);
bool Mod_CheckEntityPVS(cl_entity_t *ent);
bool CL_CheckVisibility(const Vector &point);

#endif // BSPUTIL_H
