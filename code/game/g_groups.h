// Copyright (C) 2000 Raven Software
//
// g_groups.h -- local definitions for game module group collections

#ifndef G_GROUPS_H_
#define G_GROUPS_H_

typedef struct
{
	char	name[128];
	char	text[128];
} group_list_t;

enum g_groupsLimits_e {
	MAX_SKINS_FOR_RACE = 128,
	MAX_GROUP_MEMBERS = 1024
};

extern group_list_t	group_list[MAX_GROUP_MEMBERS];
extern int32_t group_count;
/*@shared@*/ /*@null@*/ extern char* G_searchGroupList(const char* name);
 


#endif /* G_GROUPS_H_ */

