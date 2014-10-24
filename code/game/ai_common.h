#ifndef _AI_COMMON_H
#define _AI_COMMON_H

//some maxs
enum ai_commonLimits_e {
	MAX_NETNAME = 36,
	MAX_FILEPATH = 144
};

//bot settings
/** \struct bot_settings_t
*	Containing settings for bots.
*/
typedef struct bot_settings_s {
	char	characterfile[MAX_FILEPATH];	/*!< filename of character file */
	int		skill;							/*!< skill level */
	char	team[MAX_FILEPATH];				/*!< team */
	char	pclass[MAX_FILEPATH];			/*!< class */
} bot_settings_t;

#define AI_Assert(ptr, ret) if(ptr == NULL) { return ret; }

#endif /* _AI_COMMON_H */
