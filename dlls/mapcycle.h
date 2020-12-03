#ifndef MAPCYCLE_H
#define MAPCYCLE_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif

#define MAX_RULE_BUFFER 1024

typedef struct mapcycle_item_s
{
	struct mapcycle_item_s *next;

	char mapname[32];
	int  minplayers, maxplayers;
	char rulebuffer[MAX_RULE_BUFFER];
} mapcycle_item_t;

typedef struct mapcycle_s
{
	struct mapcycle_item_s *items;
	struct mapcycle_item_s *next_item;
} mapcycle_t;




void DestroyMapCycle(mapcycle_t *cycle);
int ReloadMapCycleFile(char *filename, mapcycle_t *cycle);
mapcycle_item_t *Mapcycle_Find(mapcycle_t *cycle, const char *map);

#endif // MAPCYCLE_H
