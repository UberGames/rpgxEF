// Copyright (C) 2000 Raven Software
//
// g_groups.h -- local definitions for game module group collections

typedef struct
{
	char	name[128];
	char	text[128];
} group_list_t;

#define MAX_GROUP_MEMBERS	1024

extern group_list_t	group_list[MAX_GROUP_MEMBERS];
extern int 	group_count;
extern char* BG_RegisterRace( const char *name );
extern char *G_searchGroupList(const char *name);

#define MAX_SKINS_FOR_RACE 128
