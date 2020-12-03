#include "extdll.h"
#include "util.h"
#include "mapcycle.h"


static char com_token[1500];

// for external use
char *COM_Token(void)
{
	return com_token;
}

/*
==============
COM_Parse

Parse a token out of a string
==============
*/
char *COM_Parse(char *data)
{
	int		c;
	int		len;
	
	len = 0;
	com_token[0] = 0;

	if (!data)
		return NULL;

// skip whitespace
skipwhite:
	while ((c = *data) <= ' ')
	{
		if (c == 0)
			return NULL;			// end of file;
		data++;
	}

// skip // comments
	if (c=='/' && data[1] == '/')
	{
		while (*data && *data != '\n')
			data++;
		goto skipwhite;
	}

// XDM3035c: skip /* */ comments
/* TESTME: NEED TO BE TESTED!
	if (c=='/' && data[1] == '*')
	{
		while (*data)
		{
			if (*data == '*')
			{
				data++;
				if (*data == 0 || *data == '/')
					break;
			}
			data++;
		}
		goto skipwhite;
	}*/

// handle quoted strings specially
	if (c == '\"')
	{
		data++;
		while (1)
		{
			c = *data++;
			if (c=='\"' || !c)
			{
				com_token[len] = 0;
				return data;
			}
			com_token[len] = c;
			len++;
		}
	}

// parse single characters
	if (c=='{' || c=='}'|| c==')'|| c=='(' || c=='\'' || c == ',' )
	{
		com_token[len] = c;
		len++;
		com_token[len] = 0;
		return data+1;
	}

// parse a regular word
	do
	{
		com_token[len] = c;
		data++;
		len++;
		c = *data;
	if (c=='{' || c=='}'|| c==')'|| c=='(' || c=='\'' || c == ',' )
			break;
	} while (c>32);

	com_token[len] = 0;
	return data;
}

/*
==============
COM_TokenWaiting

Returns 1 if additional data is waiting to be processed on this line
==============
*/
int COM_TokenWaiting(char *buffer)
{
	char *p = buffer;
	while (*p && *p!='\n')
	{
		if (!isspace(*p) || isalnum(*p))
			return 1;

		p++;
	}
	return 0;
}








//-----------------------------------------------------------------------------
// MAP CYCLE FUNCTIONS
//-----------------------------------------------------------------------------


/*
==============
ExtractCommandString

Parse commands/key value pairs to issue right after map xxx command is issued on server
 level transition
==============
*/
void ExtractCommandString(char *s, char *szCommand)
{
	// Now make rules happen
	char	pkey[512];
	char	value[512];	// use two buffers so compares
						// work without stomping on each other
	char	*o;

	if ( *s == '\\' )
		s++;

	while (1)
	{
		o = pkey;
		while ( *s != '\\' )
		{
			if ( !*s )
				return;
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;

		while (*s != '\\' && *s)
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;

		strcat( szCommand, pkey );
		if ( strlen( value ) > 0 )
		{
			strcat(szCommand, " ");
			strcat(szCommand, value);
		}
		strcat( szCommand, "\n" );

		if (!*s)
			return;
		s++;
	}
}


/*
==============
DestroyMapCycle

Clean up memory used by mapcycle when switching it
==============
*/
void DestroyMapCycle( mapcycle_t *cycle )
{
	mapcycle_item_t *p, *n, *start;
	p = cycle->items;
	if ( p )
	{
		start = p;
		p = p->next;
		while ( p != start )
		{
			n = p->next;
			delete p;
			p = n;
		}
		
		delete cycle->items;
	}
	cycle->items = NULL;
	cycle->next_item = NULL;
}


/*
==============
ReloadMapCycleFile

Parses mapcycle.txt file into mapcycle_t structure
==============
*/
int ReloadMapCycleFile( char *filename, mapcycle_t *cycle )
{
	char szBuffer[ MAX_RULE_BUFFER ];
	char szMap[ 32 ];
	int length;
	char *pFileList;
	char *aFileList = pFileList = (char*)LOAD_FILE_FOR_ME( filename, &length );
	int hasbuffer;
	mapcycle_item_s *item, *newlist = NULL, *next;
	cycle->next_item = NULL;// XDM
	int count = 0;

	if (pFileList && length)
	{
		// the first map name in the file becomes the default
		while (1)
		{
			hasbuffer = 0;
			memset( szBuffer, 0, MAX_RULE_BUFFER );

			pFileList = COM_Parse( pFileList );
			if ( strlen( com_token ) <= 0 )
				break;

			strcpy( szMap, com_token );

			// Any more tokens on this line?
			if ( COM_TokenWaiting( pFileList ) )
			{
				pFileList = COM_Parse( pFileList );
				if ( strlen( com_token ) > 0 )
				{
					hasbuffer = 1;
					strncpy(szBuffer, com_token, MAX_RULE_BUFFER);// XDM: buffer overrun protection
				}
			}

			// Check map
			if ( IS_MAP_VALID( szMap ) )
			{
				// Create entry
				char *s = NULL;

				item = new mapcycle_item_s;
				count++;

				strcpy( item->mapname, szMap );

				// XDM: a good place to find current map
				if (cycle->next_item == NULL)
				{
					if (stricmp(szMap, STRING(gpGlobals->mapname)) == 0)
					{
						ALERT(at_console, "Map cycle (%s): current map %s found in '%s' (%d)\n", filename, STRING(gpGlobals->mapname), filename, count);
						cycle->next_item = item;// just set this to this->next at the end
					}
				}

				item->minplayers = 0;
				item->maxplayers = 0;

				memset( item->rulebuffer, 0, MAX_RULE_BUFFER );

				if ( hasbuffer )
				{
					s = GET_INFO_KEY_VALUE( szBuffer, "minplayers" );
					if ( s && s[0] )
					{
						item->minplayers = atoi( s );
						item->minplayers = max( item->minplayers, 0 );
						item->minplayers = min( item->minplayers, gpGlobals->maxClients );
					}
					s = GET_INFO_KEY_VALUE( szBuffer, "maxplayers" );
					if ( s && s[0] )
					{
						item->maxplayers = atoi( s );
						item->maxplayers = max( item->maxplayers, 0 );
						item->maxplayers = min( item->maxplayers, gpGlobals->maxClients );
					}
					// Remove keys
					INFO_REMOVE_KEY(szBuffer, "minplayers");
					INFO_REMOVE_KEY(szBuffer, "maxplayers");
					strcpy( item->rulebuffer, szBuffer );
				}

				item->next = cycle->items;
				cycle->items = item;
			}
			else
				ALERT( at_console, "Skipping %s from mapcycle, not a valid map\n", szMap );

		}
		FREE_FILE(aFileList);
	}

	// Fixup circular list pointer
	item = cycle->items;

	// Reverse it to get original order
	while (item)
	{
		next = item->next;
		item->next = newlist;
		newlist = item;
		item = next;
	}
	cycle->items = newlist;
	item = cycle->items;

	// Didn't parse anything
	if (!item)
		return 0;

	while (item->next)
	{
		item = item->next;
	}
	item->next = cycle->items;

	if (cycle->next_item)// XDM: current map found, use it's next
		cycle->next_item = cycle->next_item->next;

	if (cycle->next_item == NULL)// supersafety
		cycle->next_item = item->next;

	return count;
}

// XDM3034
mapcycle_item_t *Mapcycle_Find(mapcycle_t *cycle, const char *map)
{
	mapcycle_item_t *start = cycle->items;
	if (start)
	{
		mapcycle_item_t *current = start;
		do
		{
			if (stricmp(map, current->mapname) == 0)
				return current;

			current = current->next;
		}
		while (current != start);
	}
	return NULL;
}
