#ifndef BG_LEX_H
#define BG_LEX_H

#include "q_shared.h"

#define LEX_DEBUG
#ifdef LEX_DEBUG
extern char *yyget_text (void* yyscanner);
#endif

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

typedef enum {
	LSYM_OBRACE,		/* ( */
	LSYM_OBRACEC,		/* { */
	LSYM_OBRACESQ,	/* [ */
	LSYM_CBRACE,		/* ) */
	LSYM_CBRACEC,		/* } */
	LSYM_CBRACESQ,	/* ] */
	LSYM_TRUE,
	LSYM_FALSE,
	LSYM_SERVER_CHANGE_CONFIG,
	LSYM_SERVER,
	LSYM_HOLODECK_DATA,
	LSYM_PROGRAM,
	LSYM_MAP_CHANGE_CONFIG,
	LSYM_MAP,
	LSYM_LOCATIONS_LIST,
	LSYM_LOCATIONS_LIST_2,
	LSYM_MESSAGE,
	LSYM_CONSOLE_NAME,
	LSYM_FORMAL_NAME,
	LSYM_RADAR_COLOR,
	LSYM_ICON_COLOR,
	LSYM_NULL,
	LSYM_MODEL_SKIN,
	LSYM_WEAPONS,
	LSYM_HAS_RANKS,
	LSYM_TEAL,
	LSYM_MEDICAL,
	LSYM_GOLD,
	LSYM_GREEN,
	LSYM_MARINE,
	LSYM_RED,
	LSYM_WP_NULL,
	LSYM_WP_NULL_HANDS,
	LSYM_WP_TRICORDER,
	LSYM_WP_PADD,
	LSYM_WP_COFFEE,
	LSYM_WP_PHASER,
	LSYM_WP_COMPRESSION_RIFLE,
	LSYM_WP_TR116,
	LSYM_WP_GRENADE_LAUNCHER,
	LSYM_WP_QUANTUM_BURST,
	LSYM_WP_MEDKIT,
	LSYM_WP_DISRUPTOR,
	LSYM_WP_VOYAGER_HYPO,
	LSYM_WP_DERMAL_REGEN,
	LSYM_WP_TOOLKIT,
	LSYM_WP_HYPERSPANNER,
	LSYM_WP_0,
	LSYM_WP_1,
	LSYM_WP_2,
	LSYM_WP_3,
	LSYM_WP_4,
	LSYM_WP_5,
	LSYM_WP_6,
	LSYM_WP_7,
	LSYM_WP_8,
	LSYM_WP_9,
	LSYM_WP_10,
	LSYM_WP_11,
	LSYM_WP_12,
	LSYM_WP_13,
	LSYM_WP_14,
	LSYM_WP_15,
	LSYM_ADMIN,
	LSYM_BORG,
	LSYM_N00B,
	LSYM_NO_SHOW,
	LSYM_MENU_TEXTURE_DEF,
	LSYM_MENU_TEXTURE_RED,
	LSYM_MENU_TEXTURE_TEAL,
	LSYM_MENU_TEXTURE_GOLD,
	LSYM_MENU_TEXTURE_GREEN,
	LSYM_BOLT_MODEL,
	LSYM_BOLT_SHADER,
	LSYM_ADMIRAL_RANK,
	LSYM_SEMICOLON,

	LSYM_UNKNOWN
} bgLexSymbol;

typedef enum {
	LMT_STRING = 1,
	LMT_INT,
	LMT_DOUBLE,
	LMT_VECTOR3,
	LMT_VECTOR4,
	LMT_SYMBOL,
	LMT_IGNORE
} bgLexMorphemType;

typedef struct bgLexMorphemData_s bgLexMorphemData;
struct bgLexMorphemData_s {
	char* str;
	bgLexSymbol symbol;
	int numInteger;
	double numDouble;
	vec3_t vector3;
	vec4_t vector4;	
};

typedef struct bgLexMorphem_s bgLexMorphem;
struct bgLexMorphem_s {
	bgLexMorphemType type;
	bgLexMorphemData data;
	int line;
	int column;
};

typedef struct bgLex_s bgLex;
struct bgLex_s {
	yyscan_t lex;
	bgLexMorphem morphem;
	void* buf;
};

bgLex* bgLex_create(char* data);
void bgLex_destroy(/*@only@*/ /*@out@*/ /*@null@*/ bgLex* lex);
int bgLex_lex(bgLex* lex);
bgLexSymbol bgLex_textToSymbol(char* text);
void bgLexFatalError(const char* msg, void* lex); 

#endif /* BG_LEX_H */