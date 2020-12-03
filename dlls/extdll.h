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
#ifndef EXTDLL_H
#define EXTDLL_H

//
// Global header file for extension DLLs
//

// Silence certain warnings (see platform.h)
#include "platform.h"
#include <stdio.h>
#include <stdlib.h>// itoa
#include <stdarg.h>
#include <string.h>
#include <ctype.h>//isspace

// Vector class
#include "vector.h"

// Shared engine/DLL constants
#include "const.h"
#include "progdefs.h"
#include "edict.h"

// Shared header describing protocol between engine and DLLs
#include "eiface.h"

// Shared header between the client DLL and the game DLLs
#include "cdll_dll.h"

enum XDM_EntityIs_e
{
	ENTIS_INVALID = 0,
	ENTIS_PLAYER,
	ENTIS_MONSTER,
	ENTIS_HUMAN,
	ENTIS_GAMEGOAL,
	ENTIS_PROJECTILE,
	ENTIS_PLAYERWEAPON,
	ENTIS_PLAYERITEM,
	ENTIS_PUSHABLE,
	ENTIS_BREAKABLE,
	ENTIS_TRIGGER,
	ENTIS_OTHER
};

#endif //EXTDLL_H
