#include "extdll.h"
#include "util.h"
#include "const.h"
#include "cbase.h"
#include "entconfig.h"
#include "game.h"
#include "gamerules.h"

bool g_MapConfigCommands = false;// hack
bool g_MultiplayerOnlyCommands = false;
int g_iGamerulesSpecificCommands = -1;
char g_iGamerulesSpecificCommandsOp[4];

int g_iNumEntitiesAddedExternally = 0;


//-----------------------------------------------------------------------------
// Purpose: Parses an entity block and creates an entity. Same as in the engine.
// WARNING: does not check for duplicate entries or already existing entities!
//
// Input  : *pData - start of the buffer
// Output : char * - position where it stopped reading
//-----------------------------------------------------------------------------
char *SV_ParseEntity(char *pData)
{
	if (pData)
	{
		char *classname = NULL;
		char *pToken = NULL;
//		std::vector<KeyValueData> KeyValuePairs;
		KeyValueData kvd[MAX_ENTITY_KEYVALUES];
// we cannot use pointer to file data, COM_Parse doesn't allow us to, so just allocate some static buffers
		char names[MAX_ENTITY_KEYVALUES][64];
		char values[MAX_ENTITY_KEYVALUES][256];
		int i = 0, num = 0;

		for (num = 0; num < MAX_ENTITY_KEYVALUES; ++num)// TODO: make this code more reliable
		{	
			pData = COM_Parse(pData);// parse key
			pToken = COM_Token();
			if (pToken == NULL || *pToken == '}')// end (this is normal after last value was parsed)
				break;
			strcpy(names[num], pToken);
			kvd[num].szKeyName = names[num];//pToken; temporary buffer

			pData = COM_Parse(pData);// parse value
			pToken = COM_Token();
			if (pToken == NULL || *pToken == '}')// end (this is not normal)
				break;
			strcpy(values[num], pToken);
			kvd[num].szValue = values[num];//pToken;

			kvd[num].szClassName = NULL;// unknown
			kvd[num].fHandled = false;		

			if (strcmp(kvd[num].szKeyName, "classname") == 0)
				classname = kvd[num].szValue;
			else if (num == 0)// 1st key, but not a classname!
			{
				SERVER_PRINT("ERROR: ENT file: entity key/value pair list must start with classname!\n");
				ALERT(at_error, "ERROR: ENT file: entity key/value pair list must start with classname!\n");
				return NULL;// stop parsing the file
			}
		}

		if (num == 0)// no values
			return NULL;
		if (classname == NULL)// no classname - no entity
			return NULL;

		if (strcmp(classname, "worldspawn") == 0)// NEVER add a new world!
		{
			SERVER_PRINT("ERROR: ENT file: worldspawn detected!\n");
			ALERT(at_error, "ERROR: ENT file: worldspawn detected!\nThis is probably an exported ENT file.\nENT files in maps directory must contain only additional entities that should be added into the game!\n");
			return NULL;// looks like someone put a wrong .ent file here
		}

		edict_t *ent = CREATE_NAMED_ENTITY(ALLOC_STRING(classname));// we can't CREATE_ENTITY(); and then pass classname as value

		if (!UTIL_IsValidEntity(ent))//if (ent == NULL)// possible!
		{
			SERVER_PRINT(UTIL_VarArgs("SV_ParseEntity: unable to create entity: %s\n", classname));
			return NULL;
		}
//		if (ent->v.flags & FL_KILLME)// just in case
//			return NULL;

		for (i = 0; i < num; ++i)
		{
			kvd[i].szClassName = classname;
			DispatchKeyValue(ent, &kvd[i]);
		}

		if (DispatchSpawn(ent) == -1)
		{
			REMOVE_ENTITY(ent);
			ent = NULL;
		}
		else
		{
			g_iNumEntitiesAddedExternally++;
			char str[256];
			sprintf(str, "+added '%s'\n", classname);
			SERVER_PRINT(str);
		}
//		return ent;
	}
	return pData;
}

bool SV_WriteEdict(char *pData, edict_t *ent)
{
	if (pData && ent)
	{
//		sprintf(pData, "{");
//		SaveWriteFields(
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------
// Purpose: read additional entities from map.ent
//-----------------------------------------------------------------------------
void ENTCONFIG_Init(void)// START DEBUG IN SOFTWARE!!!
{
	char str[128];

	if (sv_loadentfile.value > 0.0f)
	{
		_snprintf(str, 127, "maps/%s.ent", STRING(gpGlobals->mapname));
		int len = 0;
		g_iNumEntitiesAddedExternally = 0;
		char *pFileStart = (char*)LOAD_FILE_FOR_ME(str, &len);
		if (pFileStart)
		{
			SERVER_PRINT("Initializing external map entity list\n");
			char *pFileData = pFileStart;
			char *token;
//			edict_t *pent = NULL;

			while ((pFileData = COM_Parse(pFileData)) != NULL)
			{
				token = COM_Token();
				if (token && token[0] == '{')// new entity block
				{
					pFileData = SV_ParseEntity(pFileData);// keep the pFileData pointer updated!
				}
			}
			FREE_FILE(pFileStart);
			pFileStart = NULL;
			_snprintf(str, 127, "Created %d additional entities\n", g_iNumEntitiesAddedExternally);
			SERVER_PRINT(str);
		}
	}
	else
		SERVER_PRINT("External map entity list loading is disasbled in the game DLL.\n");

/*	FILE *pFile = LoadFile(str, "r");
	if (pFile)
	{
		char line[256];
		SERVER_PRINT("Initializing external map entity list\n");

		entities = COM

		bool bInsideEntity = false;
		while (fgets(line, 0, pFile))
		{
			if (line[0] == '{')
			{
				if (bInsideEntity)
				{
					SERVER_PRINT("Error: unexpected '{' in map entity file\n");
					goto entconf_finish;
				}
				else
				{
					bInsideEntity = true;
				}
			}

  I hate this wheel
		}

		fclose(pFile);
		pFile = NULL;
	}*/
//entconf_finish:

//	sprintf(szCommand, "maps/%s.ent", STRING(gpGlobals->mapname));
//	ParseFileKV(szCommand, EntCallback);
}

//-----------------------------------------------------------------------------
// Purpose: execute map_pre.cfg before installing game rules
//-----------------------------------------------------------------------------
void ENTCONFIG_ExecMapPreConfig(void)
{
	char str[127];
	_snprintf(str, 127, "exec maps/%s_pre.cfg\n", STRING(gpGlobals->mapname));
	g_MapConfigCommands = true;
	SERVER_COMMAND(str);
	SERVER_EXECUTE();
	g_MapConfigCommands = false;
	g_MultiplayerOnlyCommands = false;
	g_iGamerulesSpecificCommands = -1;
	g_iGamerulesSpecificCommandsOp[0] = 0;
}

//-----------------------------------------------------------------------------
// Purpose: execute map.cfg after installing game rules
//-----------------------------------------------------------------------------
void ENTCONFIG_ExecMapConfig(void)
{
	char str[127];
	_snprintf(str, 127, "exec maps/%s.cfg\n", STRING(gpGlobals->mapname));
	g_MapConfigCommands = true;
	SERVER_COMMAND(str);
	SERVER_EXECUTE();
	g_MapConfigCommands = false;
	g_MultiplayerOnlyCommands = false;
	g_iGamerulesSpecificCommands = -1;
	g_iGamerulesSpecificCommandsOp[0] = 0;
}

//-----------------------------------------------------------------------------
// Purpose: Write an entity to map config for future loading
// Input  : *pev - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool ENTCONFIG_WriteEntity(entvars_t *pev)
{
	if (pev == NULL)
		return false;

	char mapentname[127];
	sprintf(mapentname, "maps/%s.ent", STRING(gpGlobals->mapname));
	FILE *pMapConfig = LoadFile(mapentname, "a+");// Opens an empty file for both reading and writing. If the given file exists, its contents are destroyed.
	if (pMapConfig)
	{
		fseek(pMapConfig, 0L, SEEK_END);// APPEND!
/* this is needed if ALL entities in the file were enclosed in {}
		fpos_t fpos;
		fpos_t offset = 1;
		  fgetpos(pMapConfig, &fpos);
		while (offset < 64)// search from the end
		{
			fseek(pMapConfig, -1, SEEK_CUR);
			fsetpos(pMapConfig, fpos-offset);
			if (fgetc(pMapConfig) == '}')
				break;
			++offset;
		}*/

		// XDM: wanted to use SaveWriteFields so much...
		fprintf(pMapConfig, "{\n\"classname\" \"%s\"\n\"origin\" \"%g %g %g\"\n\"angles\" \"%g %g %g\"\n", STRING(pev->classname), pev->origin.x, pev->origin.y, pev->origin.z, pev->angles.x, pev->angles.y, pev->angles.z);
		if (!FStringNull(pev->targetname))
			fprintf(pMapConfig, "\"targetname\" \"%s\"\n", STRING(pev->targetname));
		if (!FStringNull(pev->target))
			fprintf(pMapConfig, "\"target\" \"%s\"\n", STRING(pev->target));
		if (pev->scale > 0.0f && pev->scale != 1.0f)
			fprintf(pMapConfig, "\"scale\" \"%g\"\n", pev->scale);

		// UNDONE: all entvars
		fprintf(pMapConfig, "}\n");
		fclose(pMapConfig);
	}
	else
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: enable if-sections in map config files. "gamerules_only == 1"
// Input  : &left - left operand
//			*cmp_operator - "=="
//			&right - right operand
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool ENTCONFIG_Evaluate(const int &left, char *cmp_operator, const int &right)
{
	if (cmp_operator == NULL)
		return false;

	if (strcmp(cmp_operator, "==") == 0)
		return (left == right);
	else if (strcmp(cmp_operator, "!=") == 0)
		return (left != right);
	else if (strcmp(cmp_operator, ">=") == 0)
		return (left >= right);
	else if (strcmp(cmp_operator, ">") == 0)
		return (left > right);
	else if (strcmp(cmp_operator, "<") == 0)
		return (left < right);
	else if (strcmp(cmp_operator, "<=") == 0)
		return (left <= right);
	else if (strcmp(cmp_operator, "&") == 0)
		return (left & right) != 0;
	else if (strcmp(cmp_operator, "^") == 0)
		return (left ^ right) != 0;
	else if (strcmp(cmp_operator, "|") == 0)
		return (left | right) !=0;
	else
		ALERT(at_aiconsole, "ENTCONFIG_Evaluate: unknown operator: '%s'\n", cmp_operator);

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Check global conditions
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool ENTCONFIG_ValidateCommand(void)
{
	if (g_MapConfigCommands)
	{
		if (g_MultiplayerOnlyCommands && gpGlobals->deathmatch == 0)
			return false;
		if (g_iGamerulesSpecificCommands != -1 && g_pGameRules && ENTCONFIG_Evaluate(g_pGameRules->GetGameType(), g_iGamerulesSpecificCommandsOp, g_iGamerulesSpecificCommands) == false)
			return false;
	}
	return true;
}
