// Copyright (C) 1999-2000 Id Software, Inc.
//
/**********************************************************************
	UI_ATOMS.C

	User interface building blocks and support functions.
**********************************************************************/
#include "ui_local.h"
#include "../qcommon/stv_version.h"

uiStatic_t		uis;
qboolean		m_entersound;		/* after a frame, so caching won't disrupt the sound */
void UI_LanguageFilename(char *baseName,char *baseExtension,char *finalName);
void BG_LoadItemNames(void);

extern qboolean BG_ParseRankNames ( char* fileName, rankNames_t rankNames[] );
void UI_SecurityCodeSetup ( void );

/* these are here so the functions in q_shared.c can link */
#ifndef UI_HARD_LINKED

void QDECL Com_Error( int32_t level, const char *error, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, error);
	vsprintf (text, error, argptr);
	va_end (argptr);

	trap_Error( va("%s", text) );
}

void QDECL Com_Printf( const char *msg, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, msg);
	vsprintf (text, msg, argptr);
	va_end (argptr);

	trap_Print( va("%s", text) );
}

#endif


typedef struct 
{
	int32_t			initialized;	/* Has this structure been initialized */
	qhandle_t		cornerUpper;
	qhandle_t		cornerUpper2;
	qhandle_t		cornerLower;
} menuframe_t;

menuframe_t s_menuframe;

static qboolean UI_IsWidescreen( void )
{
	if ( ui_handleWidescreen.integer && uis.widescreen.ratio && uis.widescreen.state != WIDESCREEN_NONE )
		return qtrue;

	return qfalse;
}

const char menuEmptyLine[] = " ";
/*
=================
UI_ClampCvar
=================
*/
float UI_ClampCvar( float min, float max, float value )
{
	if ( value < min ) return min;
	if ( value > max ) return max;
	return value;
}


/*
=================
UI_PushMenu
=================
*/
void UI_PushMenu( menuframework_s *menu )
{
	int32_t				i;
	menucommon_s*	item;

	/* avoid stacking menus invoked by hotkeys */
	for (i=0 ; i<uis.menusp ; i++)
	{
		if (uis.stack[i] == menu)
		{
			uis.menusp = i;
			break;
		}
	}

	if (i == uis.menusp)
	{
		if (uis.menusp >= MAX_MENUDEPTH)
			trap_Error("UI_PushMenu: menu stack overflow");

		uis.stack[uis.menusp++] = menu;
	}

	uis.activemenu = menu;

	/* default cursor position */
	menu->cursor      = 0;
	menu->cursor_prev = 0;

	m_entersound = qtrue;

	trap_Key_SetCatcher( KEYCATCH_UI );

	/* force first available item to have focus */
	for (i=0; i<menu->nitems; i++)
	{
		item = (menucommon_s *)menu->items[i];
		if (!(item->flags & (QMF_GRAYED|QMF_MOUSEONLY|QMF_INACTIVE)))
		{
			menu->cursor_prev = -1;
			Menu_SetCursor( menu, i );
			break;
		}
	}

	uis.firstdraw = qtrue;
}

/*
=================
UI_PopMenu
=================
*/
void UI_PopMenu (void)
{
	trap_S_StartLocalSound( menu_out_sound, CHAN_LOCAL_SOUND );

	uis.menusp--;

	if (uis.menusp < 0)
		trap_Error ("UI_PopMenu: menu stack underflow");

	if (uis.menusp) {
		uis.activemenu = uis.stack[uis.menusp-1];
		uis.firstdraw = qtrue;
	}
	else {
		UI_ForceMenuOff ();
	}
}

void UI_ForceMenuOff (void)
{
	uis.menusp     = 0;
	uis.activemenu = NULL;

	trap_Key_SetCatcher( trap_Key_GetCatcher() & ~KEYCATCH_UI );
	trap_Key_ClearStates();
	trap_Cvar_Set( "cl_paused", "0" );
}

/*
=================
UI_LerpColor
=================
*/
static void UI_LerpColor(vec4_t a, vec4_t b, vec4_t c, float t)
{
	int32_t i;

	/* lerp and clamp each component */
	for (i=0; i<4; i++)
	{
		c[i] = a[i] + t*(b[i]-a[i]);
		if (c[i] < 0)
			c[i] = 0;
		else if (c[i] > 1.0)
			c[i] = 1.0;
	}
}


/*
=================
UI_DrawProportionalString2
=================
*/
#define CHARMAX 256
#define PROPB_GAP_WIDTH		4
#define PROPB_SPACE_WIDTH	12
#define PROPB_HEIGHT		36

static int32_t	propMapBig[CHARMAX][3];
static int32_t	propMap[CHARMAX][3];
static int32_t	propMapTiny[CHARMAX][3];

static int32_t const propMapB[26][3] = {
{11, 12, 33},
{49, 12, 31},
{85, 12, 31},
{120, 12, 30},
{156, 12, 21},
{183, 12, 21},
{207, 12, 32},

{13, 55, 30},
{49, 55, 13},
{66, 55, 29},
{101, 55, 31},
{135, 55, 21},
{158, 55, 40},
{204, 55, 32},

{12, 97, 31},
{48, 97, 31},
{82, 97, 30},
{118, 97, 30},
{153, 97, 30},
{185, 97, 25},
{213, 97, 30},

{11, 139, 32},
{42, 139, 51},
{93, 139, 32},
{126, 139, 31},
{158, 139, 25},
};

/*
=================
UI_DrawBannerString
=================
*/
static void UI_DrawBannerString2( int32_t x, int32_t y, const char* str, vec4_t color )
{
	const char* s;
	char	ch;
	float	ax;
	float	ay;
	float	aw;
	float	ah;
	float	frow;
	float	fcol;
	float	fwidth;
	float	fheight;

	/* draw the colored text */
	trap_R_SetColor( color );
	
	/*ax = x * uis.scale + uis.bias;*/
	ax = x * uis.scalex;
	ay = y * uis.scaley;

	s = str;
	while ( *s )
	{
		ch = *s & 255;
		if ( ch == ' ' ) {
			ax += ((float)PROPB_SPACE_WIDTH + (float)PROPB_GAP_WIDTH)* uis.scalex;
		}
		else if ( ch >= 'A' && ch <= 'Z' ) {
			ch -= 'A';
			fcol = (float)propMapB[(int32_t)ch][0] / 256.0f; /*256.0f*/
			frow = (float)propMapB[(int32_t)ch][1] / 256.0f;
			fwidth = (float)propMapB[(int32_t)ch][2] / 256.0f;
			fheight = (float)PROPB_HEIGHT / 256.0f;
			aw = (float)propMapB[(int32_t)ch][2] * uis.scalex;
			ah = (float)PROPB_HEIGHT * uis.scaley;
			trap_R_DrawStretchPic( ax, ay, aw, ah, fcol, frow, fcol+fwidth, frow+fheight, uis.charsetPropB );
			ax += (aw + (float)PROPB_GAP_WIDTH * uis.scalex);
		}
		s++;
	}

	trap_R_SetColor( NULL );
}

/*
=================
UI_DrawBannerString
=================
*/
void UI_DrawBannerString( int32_t x, int32_t y, const char* str, int32_t style, vec4_t color ) {
	const char *	s;
	int32_t				ch;
	int32_t				width;
	vec4_t			drawcolor;

	/* find the width of the drawn text */
	s = str;
	width = 0;
	while ( *s ) {
		ch = *s;
		if ( ch == ' ' ) {
			width += PROPB_SPACE_WIDTH;
		}
		else if ( ch >= 'A' && ch <= 'Z' ) {
			width += propMapB[ch - 'A'][2] + PROPB_GAP_WIDTH;
		}
		s++;
	}
	width -= PROPB_GAP_WIDTH;

	switch( style & UI_FORMATMASK ) {
		case UI_CENTER:
			x -= width / 2;
			break;

		case UI_RIGHT:
			x -= width;
			break;

		case UI_LEFT:
		default:
			break;
	}

	if ( style & UI_DROPSHADOW ) {
		drawcolor[0] = drawcolor[1] = drawcolor[2] = 0;
		drawcolor[3] = color[3];
		UI_DrawBannerString2( x+2, y+2, str, drawcolor );
	}

	UI_DrawBannerString2( x, y, str, color );
}


/*
=================
UI_ProportionalStringWidth
=================
*/
int32_t UI_ProportionalStringWidth( const char* str,int32_t style ) {
	const char *	s;
	int32_t				ch;
	int32_t				charWidth;
	int32_t				width;		

	if (style == UI_TINYFONT)
	{
		s = str;
		width = 0;
		while ( *s ) {
			ch = *s & 255;
			charWidth = propMapTiny[ch][2];
			if ( charWidth != -1 ) {
				width += charWidth;
				width += PROP_GAP_TINY_WIDTH;
			}
			s++;
		}

		width -= PROP_GAP_TINY_WIDTH;
	}
	else if (style == UI_BIGFONT)
	{
		s = str;
		width = 0;
		while ( *s ) {
			ch = *s & 255;
			charWidth = propMapBig[ch][2];
			if ( charWidth != -1 ) {
				width += charWidth;
				width += PROP_GAP_BIG_WIDTH;
			}
			s++;
		}

		width -= PROP_GAP_BIG_WIDTH;
	}
	else 
	{
		s = str;
		width = 0;
		while ( *s ) {
			ch = *s & 255;
			charWidth = propMap[ch][2];
			if ( charWidth != -1 ) {
				width += charWidth;
				width += PROP_GAP_WIDTH;
			}
			s++;
		}
		width -= PROP_GAP_WIDTH;
	}

	return width;
}

static int32_t specialTinyPropChars[CHARMAX][2] = {
{0, 0},
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 10 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 20 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 30 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 40 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 50 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 60 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 70 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 80 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 90 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 100 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 110 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 120 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 130 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 140 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 150 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{2,-3},{0, 0},	/* 160 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 170 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 180 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 190 */
{0,-1},{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{0, 0},{2, 0},{2,-3},	/* 200 */
{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{0,-1},{2,-3},{2,-3},	/* 210 */
{2,-3},{3,-3},{2,-3},{2,-3},{0, 0},{0,-1},{2,-3},{2,-3},{2,-3},{2,-3},	/* 220 */
{2,-3},{0,-1},{0,-1},{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{0, 0},	/* 230 */
{2, 0},{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{0, 0},	/* 240 */
{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{0, 0},{0,-1},{2,-3},{2,-3},	/* 250 */
{2,-3},{2,-3},{2,-3},{0,-1},{2,-3}					/* 255 */
}; 


static int32_t specialPropChars[CHARMAX][2] = {
{0, 0},
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 10 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 20 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 30 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 40 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 50 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 60 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 70 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 80 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 90 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 100 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 110 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 120 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 130 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 140 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 150 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 160 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 170 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 180 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 190 */
{2,-2},{2,-2},{2,-2},{2,-2},{2,-2},{2,-2},{2,-2},{0, 0},{1, 1},{2,-2},	/* 200 */
{2,-2},{2,-2},{2,-2},{2,-2},{2,-2},{2,-2},{2,-2},{0, 0},{2,-2},{2,-2},	/* 210 */
{2,-2},{2,-2},{2,-2},{2,-2},{0, 0},{0, 0},{2,-2},{2,-2},{2,-2},{2,-2},	/* 220 */
{2,-2},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 230 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 240 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 250 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0}					/* 255 */
}; 


static int32_t specialBigPropChars[CHARMAX][2] = {
{0, 0},
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 10 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 20 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 30 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 40 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 50 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 60 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 70 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 80 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 90 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 100 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 110 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 120 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 130 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 140 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 150 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 160 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 170 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 180 */
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	/* 190 */
{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{0, 0},{3, 1},{3,-3},	/* 200 */
{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{0, 0},{3,-3},{3,-3},	/* 210 */
{3,-3},{3,-3},{3,-3},{3,-3},{0, 0},{0, 0},{3,-3},{3,-3},{3,-3},{3,-3},	/* 220 */
{3,-3},{0, 0},{0, 0},{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{0, 0},	/* 230 */
{3, 1},{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{0, 0},	/* 240 */
{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{0, 0},{0, 0},{3,-3},{3,-3},	/* 250 */
{3,-3},{3,-3},{3,-3},{0, 0},{3,-3}					/* 255 */
}; 

static int32_t showColorChars;

/*
=================
UI_DrawProportionalString2
=================
*/
static void UI_DrawProportionalString2( int32_t x, int32_t y, const char* str, vec4_t color, int32_t style, qhandle_t charset )
{
	const char* s;
	unsigned char ch;
	float	ax;
	float	ay,holdY;
	float	aw;
	float	ah;
	float	frow;
	float	fcol;
	float	fwidth;
	float	fheight;
	float	sizeScale;
	int32_t		colorI;
	int32_t		special;

	/* draw the colored text */
	trap_R_SetColor( color );
	
	/*ax = x * uis.scale + uis.bias;*/
	ax = x * uis.scalex;
	ay = y * uis.scaley;
	holdY = ay;

	/* TiM - adjust for widescreen monitors */
	if ( UI_IsWidescreen() )
	{
		ax *= uis.widescreen.ratio;

		/* center the elements into the middle of the screen */
		if ( uis.widescreen.state == WIDESCREEN_CENTER )
			ax += uis.widescreen.bias;
	}	
	/*else
		Com_Printf( S_COLOR_RED "CVAR: %i, Ratio: %f, State: %i\n", ui_handleWidescreen.integer, uis.widescreen.ratio, uis.widescreen.state );*/

	sizeScale = UI_ProportionalSizeScale( style );

	if (style & UI_TINYFONT)
	{
		s = str;
		while ( *s )
		{
			/* Is this a color???? */
			if ( Q_IsColorString( s ) && !( style & UI_SHOWCOLOR ) ) 
			{
				colorI = ColorIndex( *(s+1) );
				trap_R_SetColor( g_color_table[colorI] );
				s += 2;
				continue;
			}

			ch = *s & 255;
			if ( ch == ' ' ) {
				aw = (float)PROP_SPACE_TINY_WIDTH;
			}
			else if ( propMap[ch][2] != -1 ) {
				/* Because some foreign characters were a little different */
				special = specialTinyPropChars[ch][0];
				ay = holdY + (specialTinyPropChars[ch][1] * uis.scaley);

				fcol = (float ) propMapTiny[ch][0] / 256.0f; /*256.0f*/
				frow = (float)propMapTiny[ch][1] / 256.0f;
				fwidth = (float)propMapTiny[ch][2] / 256.0f;
				fheight = (float)(PROP_TINY_HEIGHT + special) / 256.0f;
				aw = (float)propMapTiny[ch][2] * uis.scalex * sizeScale;
				ah = (float)(PROP_TINY_HEIGHT+ special) * uis.scaley * sizeScale;

				/* TiM - adjust for widescreen */
				if ( UI_IsWidescreen() )
				{
					aw *= uis.widescreen.ratio;
				}

				trap_R_DrawStretchPic( ax, ay, aw, ah, fcol, frow, fcol + fwidth, frow + fheight, charset );

			}
			else
			{
				aw = 0;
			}

			
			ax += (aw + (float)PROP_GAP_TINY_WIDTH * uis.scalex * sizeScale);
			
			/* again adjust for widescreen */
			if ( UI_IsWidescreen() )
				ax -= ((float)PROP_GAP_TINY_WIDTH * uis.scalex * sizeScale)*(1.0f-uis.widescreen.ratio);

			s++;
		}
	}
	else if (style & UI_BIGFONT)
	{
		s = str;
		while ( *s )
		{
			/* Is this a color???? */
			if ( Q_IsColorString( s ) && !( style & UI_SHOWCOLOR ) ) 
			{
				colorI = ColorIndex( *(s+1) );
				trap_R_SetColor( g_color_table[colorI] );
				s += 2;
				continue;
			}

			ch = *s & 255;
			if ( ch == ' ' ) {
				aw = (float)PROP_SPACE_BIG_WIDTH * uis.scalex;
			}
			else if ( propMap[ch][2] != -1 ) {
				/* Because some foreign characters were a little different */
				special = specialBigPropChars[ch][0];
				ay = holdY + (specialBigPropChars[ch][1] * uis.scaley);

				fcol = (float ) propMapBig[ch][0] / 256.0f; /* 256.0f */
				frow = (float)propMapBig[ch][1] / 256.0f;
				fwidth = (float)propMapBig[ch][2] / 256.0f;
				fheight = (float)(PROP_BIG_HEIGHT+ special) / 256.0f;
				aw = (float)propMapBig[ch][2] * uis.scalex * sizeScale;
				ah = (float)(PROP_BIG_HEIGHT+ special) * uis.scaley * sizeScale;

				/* TiM - adjust for widescreen */
				if ( UI_IsWidescreen() )
				{
					aw *= uis.widescreen.ratio;
				}

				trap_R_DrawStretchPic( ax, ay, aw, ah, fcol, frow, fcol + fwidth, frow + fheight, charset );
			}
			else
			{
				aw = 0;
			}

			
			ax += (aw + (float)PROP_GAP_BIG_WIDTH * uis.scalex * sizeScale);
			
			/* again adjust for widescreen */
			if ( UI_IsWidescreen() )
				ax -= ((float)PROP_GAP_BIG_WIDTH * uis.scalex * sizeScale)*(1.0f-uis.widescreen.ratio);

			s++;
		}
	} 
	else
	{
		s = str;
		while ( *s )
		{
			/* Is this a color???? */
			if ( Q_IsColorString( s ) && !( style & UI_SHOWCOLOR ) ) 
			{
				colorI = ColorIndex( *(s+1) );
				trap_R_SetColor( g_color_table[colorI] );
				s += 2;
				continue;
			}

			ch = *s & 255;
			if ( ch == ' ' ) {
				aw = (float)PROP_SPACE_WIDTH * uis.scalex * sizeScale;
			}
			else if ( propMap[ch][2] != -1 ) {
				/* Because some foreign characters were a little different */
				special = specialPropChars[ch][0];
				ay = holdY + (specialPropChars[ch][1] * uis.scaley);

				fcol = (float)propMap[ch][0] / 256.0f;
				frow = (float)propMap[ch][1] / 256.0f;
				fwidth = (float)propMap[ch][2] / 256.0f;
				fheight = (float)(PROP_HEIGHT+ special) / 256.0f;
				aw = (float)propMap[ch][2] * uis.scalex * sizeScale;
				ah = (float)(PROP_HEIGHT+ special) * uis.scaley * sizeScale;
				
				/* TiM - adjust for widescreen */
				if ( ui_handleWidescreen.integer && uis.widescreen.ratio )
				{
					aw *= uis.widescreen.ratio;
				}				
				
				trap_R_DrawStretchPic( ax, ay, aw, ah, fcol, frow, fcol+fwidth, frow+fheight, charset );
			}
			else
			{
				aw = 0;
			}
			
			
			ax += (aw + (float)PROP_GAP_WIDTH * uis.scalex * sizeScale);
			
			/* again adjust for widescreen */
			if ( UI_IsWidescreen() )
				ax -= ((float)PROP_GAP_WIDTH * uis.scalex * sizeScale)*(1.0f-uis.widescreen.ratio);

			s++;
		}
	}

	trap_R_SetColor( NULL );
}

/*
=================
UI_ProportionalSizeScale
=================
*/
float UI_ProportionalSizeScale( int32_t style ) {
	if(  style & UI_SMALLFONT ) {
		return PROP_SMALL_SIZE_SCALE;
	}

	return 1.00;
}


/*
=================
UI_DrawProportionalString
=================
*/
void UI_DrawProportionalString( int32_t x, int32_t y, const char* str, int32_t style, vec4_t color ) {
	vec4_t	drawcolor;
	int32_t		width;
	float	sizeScale;
	int32_t		charstyle=0;

	if ((style & UI_BLINK) && ((uis.realtime/BLINK_DIVISOR) & 1))
		return;

	/* Get char style */
	if (style & UI_TINYFONT)
	{
		charstyle = UI_TINYFONT;
	}
	else if (style & UI_SMALLFONT)
	{
		charstyle = UI_SMALLFONT;
	}
	else if (style & UI_BIGFONT)
	{
		charstyle = UI_BIGFONT;
	}
	else if (style & UI_GIANTFONT)
	{
		charstyle = UI_GIANTFONT;
	}
	else	/* Just in case */
	{
		charstyle = UI_SMALLFONT;
	}

	if ( style & UI_SHOWCOLOR )
		charstyle |= UI_SHOWCOLOR;

	sizeScale = UI_ProportionalSizeScale( style );

	switch( style & UI_FORMATMASK ) {
		case UI_CENTER:
			width = UI_ProportionalStringWidth( str,charstyle) * sizeScale;
			x -= width / 2;
			break;

		case UI_RIGHT:
			width = UI_ProportionalStringWidth( str,charstyle) * sizeScale;
			x -= width;
			break;

		case UI_LEFT:
		default:
			break;
	}

	if ( style & UI_DROPSHADOW ) {
		drawcolor[0] = drawcolor[1] = drawcolor[2] = 0;
		drawcolor[3] = color[3];
		UI_DrawProportionalString2( x+2, y+2, str, drawcolor, sizeScale, uis.charsetProp );
	}

	if ( style & UI_INVERSE ) {
		drawcolor[0] = color[0] * 0.7;
		drawcolor[1] = color[1] * 0.7;
		drawcolor[2] = color[2] * 0.7;
		drawcolor[3] = color[3];
		UI_DrawProportionalString2( x, y, str, drawcolor, sizeScale, uis.charsetProp );
		return;
	}

	if ( style & UI_PULSE ) {
		drawcolor[0] = color[0] * 0.7;
		drawcolor[1] = color[1] * 0.7;
		drawcolor[2] = color[2] * 0.7;
		drawcolor[3] = color[3];
		UI_DrawProportionalString2( x, y, str, color, sizeScale, uis.charsetProp );

		drawcolor[0] = color[0];
		drawcolor[1] = color[1];
		drawcolor[2] = color[2];
		drawcolor[3] = 0.5 + 0.5 * sin( uis.realtime / PULSE_DIVISOR );
		UI_DrawProportionalString2( x, y, str, drawcolor, sizeScale, uis.charsetProp );
		return;
	}


	if (style & UI_TINYFONT)
	{
		UI_DrawProportionalString2( x, y, str, color, charstyle, uis.charsetPropTiny );
	}
	else if (style & UI_BIGFONT)
	{
		UI_DrawProportionalString2( x, y, str, color, charstyle, uis.charsetPropBig );
	}
	else
	{
		UI_DrawProportionalString2( x, y, str, color, charstyle, uis.charsetProp );
	}
}

/*
=================
UI_DrawString2
=================
*/
static void UI_DrawString2( int32_t x, int32_t y, const char* str, vec4_t color, int32_t charw, int32_t charh )
{
	const char* s;
	char	ch;
	int32_t forceColor = qfalse; /* APSFIXME; */
	vec4_t	tempcolor;
	float	ax;
	float	ay;
	float	aw;
	float	ah;
	float	frow;
	float	fcol;

	if (y < -charh)
		/* offscreen */
		return;

	/* draw the colored text */
	trap_R_SetColor( color );
	
	/* ax = x * uis.scale + uis.bias; */
	ax = x * uis.scalex;
	ay = y * uis.scaley;
	aw = charw * uis.scalex;
	ah = charh * uis.scaley;

	if ( UI_IsWidescreen() )
	{
		ax *= uis.widescreen.ratio;
		aw *= uis.widescreen.ratio;

		if ( uis.widescreen.state == WIDESCREEN_CENTER )
			ax += uis.widescreen.bias;
	}

	s = str;
	while ( *s )
	{
		if (!showColorChars)
		{
			if ( Q_IsColorString( s ) )
			{
				if ( !forceColor )
				{
					memcpy( tempcolor, g_color_table[ColorIndex(s[1])], sizeof( tempcolor ) );
					tempcolor[3] = color[3];
					trap_R_SetColor( tempcolor );
				}
				s += 2;
				continue;
			}
		}

		ch = *s & 255;

		if (ch != ' ')
		{
			/*frow = (ch>>4)*0.0625;
			fcol = (ch&15)*0.0625;
			trap_R_DrawStretchPic( ax, ay, aw, ah, fcol, frow, fcol + 0.0625, frow + 0.0625, uis.charset );*/

			frow = (ch>>4)*0.0625;
			fcol = (ch&15)*0.0625;

			trap_R_DrawStretchPic( ax, ay, aw, ah, fcol, frow, fcol + 0.03125, frow + 0.0625, uis.charset );

		}

		ax += aw;
		s++;
	}

	trap_R_SetColor( NULL );
}


/*
=================
UI_DrawString
=================
*/
void UI_DrawString( int32_t x, int32_t y, const char* str, int32_t style, vec4_t color, qboolean highRes )
{
	int32_t		len;
	int32_t		charw;
	int32_t		charh;
	vec4_t	newcolor;
	vec4_t	lowlight;
	float	*drawcolor;
	vec4_t	dropcolor;

	if( !str ) {
		return;
	}

	if ((style & UI_BLINK) && ((uis.realtime/BLINK_DIVISOR) & 1))
		return;

	if (style & UI_TINYFONT)
	{
		charw =	TINYCHAR_WIDTH;
		charh =	TINYCHAR_HEIGHT;
	}
	else if (style & UI_BIGFONT)
	{
		charw =	BIGCHAR_WIDTH;
		charh =	BIGCHAR_HEIGHT;
	}
	else if (style & UI_GIANTFONT)
	{
		charw =	GIANTCHAR_WIDTH;
		charh =	GIANTCHAR_HEIGHT;
	}
	else
	{
		charw =	SMALLCHAR_WIDTH;
		charh =	SMALLCHAR_HEIGHT;
	}

	if (style & UI_PULSE)
	{
		lowlight[0] = 0.8*color[0]; 
		lowlight[1] = 0.8*color[1];
		lowlight[2] = 0.8*color[2];
		lowlight[3] = 0.8*color[3];
		UI_LerpColor(color,lowlight,newcolor,0.5+0.5*sin(uis.realtime/PULSE_DIVISOR));
		drawcolor = newcolor;
	}	
	else
		drawcolor = color;

	switch (style & UI_FORMATMASK)
	{
		case UI_CENTER:
			/* center justify at x */
			len = strlen(str);
			x   = x - len*charw/2;
			break;

		case UI_RIGHT:
			/* right justify at x */
			len = strlen(str);
			x   = x - len*charw;
			break;

		default:
			/* left justify at x */
			break;
	}

	if (style & UI_SHOWCOLOR)
	{
		showColorChars = qtrue;
	}
	else
	{
		showColorChars = qfalse;
	}

	if ( style & UI_DROPSHADOW )
	{
		dropcolor[0] = dropcolor[1] = dropcolor[2] = 0;
		dropcolor[3] = drawcolor[3];

		if (highRes)
			UI_DrawProportionalString( x+2, y+2, str, style, dropcolor );
		else
			UI_DrawString2(x+2,y+2,str,dropcolor,charw,charh);
	}

	/* TiM - Using a different char set now... */
	if (!highRes) /* keep the low res version for specific instances */
		UI_DrawString2(x,y,str,drawcolor,charw,charh);
	else
		UI_DrawProportionalString( x, y, str, style, drawcolor );

}

/*
=================
UI_DrawChar
=================
*/
void UI_DrawChar( int32_t x, int32_t y, int32_t ch, int32_t style, vec4_t color )
{
	char	buff[2];

	buff[0] = ch;
	buff[1] = '\0';

	UI_DrawString( x, y, buff, style, color, qfalse );
}

qboolean UI_IsFullscreen( void ) {
	if ( uis.activemenu && ( trap_Key_GetCatcher() & KEYCATCH_UI ) ) {
		return uis.activemenu->fullscreen;
	}

	return qfalse;
}

static void NeedCDAction( qboolean result ) {
	if ( !result ) {
		/*trap_Cvar_Set ("rpg_playIntro", "1");*/
		trap_Cmd_ExecuteText( EXEC_APPEND, "quit\n" );
	}
}

void UI_SetActiveMenu( uiMenuCommand_t menu ) {
	/* 
 	 * this should be the ONLY way the menu system is brought up, except for UI_ConsoleCommand below
	 * enusure minumum menu data is cached 
	 */
	Menu_Cache();

	switch ( menu ) {
	case UIMENU_NONE:
		UI_ForceMenuOff();
		return;
	case UIMENU_MAIN:
		UI_MainMenu();
		return;
	case UIMENU_NEED_CD:
		UI_ConfirmMenu( menu_normal_text[MNT_INSERTCD], 0, NeedCDAction );
		return;
	case UIMENU_INGAME:
		trap_Cvar_Set( "cl_paused", "1" );
		UI_InGameMenu();
		return;
	}
}

/*
=================
UI_KeyEvent
=================
*/
void UI_KeyEvent( int32_t key ) {
	sfxHandle_t		s;

	if (!uis.activemenu) {
		return;
	}

	if (uis.activemenu->key)
		s = uis.activemenu->key( key );
	else
		s = Menu_DefaultKey( uis.activemenu, key );

	if ((s > 0) && (s != menu_null_sound))
		trap_S_StartLocalSound( s, CHAN_LOCAL_SOUND );
}

/*
=================
UI_MouseEvent
=================
*/
void UI_MouseEvent( int32_t dx, int32_t dy )
{
	int32_t				i;
	menucommon_s*	m;

	if (!uis.activemenu)
		return;

	/* update mouse screen position */
	uis.cursorx += dx;
	/* kinda pointless, but looks nice. allow negative offsets for widescreen setups (we must maintain the ratio or buttons will stop working) */
	if ( UI_IsWidescreen() && uis.widescreen.state == WIDESCREEN_CENTER )
	{
		if (uis.cursorx < ( 0 - uis.widescreen.bias))
			uis.cursorx = 0 - uis.widescreen.bias;
		else if (uis.cursorx > ( SCREEN_WIDTH + uis.widescreen.bias) )
			uis.cursorx = SCREEN_WIDTH + uis.widescreen.bias;
	}
	else
	{
		if (uis.cursorx < 0)
			uis.cursorx = 0;
		else if (uis.cursorx > SCREEN_WIDTH)
			uis.cursorx = SCREEN_WIDTH;
	}

	uis.cursory += dy;
	if (uis.cursory < 0)
		uis.cursory = 0;
	else if (uis.cursory > SCREEN_HEIGHT)
		uis.cursory = SCREEN_HEIGHT;

	/* RPG-X: TiM - Skip new selections if a spin control window is open */
	if ( uis.activemenu->noNewSelecting )
		return;

	/* region test the active menu items */
	for (i=0; i<uis.activemenu->nitems; i++)
	{
		m = (menucommon_s*)uis.activemenu->items[i];

		if (m->flags & (QMF_GRAYED|QMF_INACTIVE))
			continue;

		if ((uis.cursorx < m->left) ||
			(uis.cursorx > m->right) ||
			(uis.cursory < m->top) ||
			(uis.cursory > m->bottom))
		{
			/* cursor out of item bounds */
			continue;
		}

		/* set focus to item at cursor */
		if (uis.activemenu->cursor != i)
		{
			Menu_SetCursor( uis.activemenu, i );
			((menucommon_s*)(uis.activemenu->items[uis.activemenu->cursor_prev]))->flags &= ~QMF_HASMOUSEFOCUS;

			if ( !(((menucommon_s*)(uis.activemenu->items[uis.activemenu->cursor]))->flags & QMF_SILENT ) ) {
				trap_S_StartLocalSound( menu_move_sound, CHAN_LOCAL_SOUND );
			}
		}

		((menucommon_s*)(uis.activemenu->items[uis.activemenu->cursor]))->flags |= QMF_HASMOUSEFOCUS;
		return;
	}  

	if (uis.activemenu->nitems > 0) {
		/* out of any region */
		((menucommon_s*)(uis.activemenu->items[uis.activemenu->cursor]))->flags &= ~QMF_HASMOUSEFOCUS;
	}
}

char *UI_Argv( int32_t arg ) {
	static char	buffer[MAX_STRING_CHARS];

	trap_Argv( arg, buffer, sizeof( buffer ) );

	return buffer;
}


char *UI_Cvar_VariableString( const char *var_name ) {
	static char	buffer[MAX_STRING_CHARS];

	trap_Cvar_VariableStringBuffer( var_name, buffer, sizeof( buffer ) );

	return buffer;
}


/*
=================
UI_Cache
=================
*/
static void UI_Cache_f( void ) {
	MainMenu_Cache();
	InGame_Cache();
	ConfirmMenu_Cache();
	PlayerModel_Cache();
	PlayerSettings_Cache();
	/*Preferences_Cache();*/
	ServerInfo_Cache();
	SpecifyServer_Cache();
	ArenaServers_Cache();
	StartServer_Cache();
	ServerOptions_Cache();
	DriverInfo_Cache();
	/*GraphicsOptions_Cache();*/
	/*UI_DisplayOptionsMenu_Cache();*/
	/*UI_SoundOptionsMenu_Cache();*/
	UI_NetworkOptionsMenu_Cache();
	/*UI_SPLevelMenu_Cache();*/
	UI_SPSkillMenu_Cache();
	UI_SPPostgameMenu_Cache();
	/*TeamMain_Cache();*/
	UI_AddBots_Cache();
	UI_RemoveBots_Cache();
	/*UI_LoadConfig_Cache();*/
	/*UI_SaveConfigMenu_Cache();*/
	UI_BotSelectMenu_Cache();
	UI_CDKeyMenu_Cache();
	UI_ModsMenu_Cache();
	UI_SoundMenu_Cache(); 
	UI_QuitMenu_Cache();
	UI_DemosMenu_Cache();
	UI_VideoDataMenu_Cache();
	UI_GameOptionsMenu_Cache();
	UI_ControlsMouseJoyStickMenu_Cache();
	/*UI_ResetGameMenu_Cache();*/
	UI_VideoData2Menu_Cache();
	UI_VideoDriverMenu_Cache();
	UI_HolomatchInMenu_Cache();
	UI_ChooseServerTypeMenu_Cache();
	UI_AdminMenu_Cache();
	UI_CreditsMenu_Cache();
	/*UI_AccessingMenu_Cache();*/
	/*UI_LibraryMenu_Cache();*/
	UI_PlayerEmotes_Cache();
    UI_MotdMenu_Cache();
}

/*
=================
UI_ConsoleCommand
=================
*/
qboolean UI_ConsoleCommand( void ) {
	char	*cmd;
	/*int32_t	i;*/

	cmd = UI_Argv( 0 );

	/* ensure minimum menu data is available*/
	Menu_Cache();

	/*if ( Q_stricmp (cmd, "levelselect") == 0 ) {
		UI_SPLevelMenu_f();
		return qtrue;
	}*/

	if ( Q_stricmp (cmd, "postgame") == 0 ) {
		UI_SPPostgameMenu_f();
		return qtrue;
	}

	if ( Q_stricmp (cmd, "ui_cache") == 0 ) {
		UI_Cache_f();
		return qtrue;
	}

	if ( Q_stricmp (cmd, "ui_teamOrders") == 0 ) {
		UI_TeamOrdersMenu_f();
		return qtrue;
	}

	if ( Q_stricmp (cmd, "ui_cdkey") == 0 ) {
		UI_CDKeyMenu_f();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "ui_emotes" ) == 0 ) {
		UI_EmotesMenu( qtrue );
		return qtrue;
	}

	/*if ( Q_stricmp( cmd, "ui_library" ) == 0 ) {
		UI_AccessingMenu();
		return qtrue;
	}*/

	if ( Q_stricmp( cmd, "ui_admin" ) == 0 ) {
		UI_AdminMenu(qtrue);
		return qtrue;
	}

	if ( Q_stricmp( cmd, "ui_turbolift" ) == 0 ) {
		UI_TurboliftMenu(atoi(UI_Argv( 1 )));
		return qtrue;
	}

	if ( Q_stricmp (cmd, "err_dialog") == 0 ) {
		UI_ConfirmMenu( UI_Argv( 1 ), 0, 0);
		return qtrue;
	}

    	/* RPG-X | Marcin | 15/12/2008 */
	if ( Q_stricmp( cmd, "ui_motd" ) == 0 ) {
		UI_MotdMenu();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "ui_motd_reset" ) == 0 ) {
		MotdReset();
		return qtrue;
	}

    /*
     *  RPG-X | Marcin | 18/12/2008
     * I hope this is the right way to do it...
     */
    if ( Q_stricmp( cmd, "ui_motd_line" ) == 0 ) {
		MotdReceiveLine( UI_Argv( 1 ) );
		return qtrue;
	}

	if ( Q_stricmp( cmd, "ui_msd" ) == 0 ) {
		static char	ui_msd1[MAX_STRING_CHARS], ui_msd2[MAX_STRING_CHARS], ui_msd3[MAX_STRING_CHARS], ui_msd4[MAX_STRING_CHARS], ui_msd5[MAX_STRING_CHARS], ui_msd6[MAX_STRING_CHARS], ui_msd7[MAX_STRING_CHARS], ui_msd8[MAX_STRING_CHARS], ui_msd9[MAX_STRING_CHARS], ui_msd10[MAX_STRING_CHARS];
		trap_Argv( 1, ui_msd1, sizeof( ui_msd1 ) );
		trap_Argv( 2, ui_msd2, sizeof( ui_msd2 ) );
		trap_Argv( 3, ui_msd3, sizeof( ui_msd3 ) );
		trap_Argv( 4, ui_msd4, sizeof( ui_msd4 ) );
		trap_Argv( 5, ui_msd5, sizeof( ui_msd5 ) );
		trap_Argv( 6, ui_msd6, sizeof( ui_msd6 ) );
		trap_Argv( 7, ui_msd7, sizeof( ui_msd7 ) );
		trap_Argv( 8, ui_msd8, sizeof( ui_msd8 ) );
		trap_Argv( 9, ui_msd9, sizeof( ui_msd9 ) );
		trap_Argv( 10, ui_msd10, sizeof( ui_msd10 ) );
		UI_msdMenu( atoi(ui_msd1), atoi(ui_msd2), atoi(ui_msd3), atoi(ui_msd4), atoi(ui_msd5), atoi(ui_msd6), atoi(ui_msd7), atoi(ui_msd8), atoi(ui_msd9), ui_msd10);
		return qtrue;
	}
	
	if ( Q_stricmp( cmd, "ui_transporter" ) == 0 ) {
		UI_TransporterMenu( atoi(UI_Argv( 1 )) );
		return qtrue;
	}

	if ( Q_stricmp( cmd, "holo_data" ) == 0 ) {
		/*HoloDataReceived(UI_Argv(1));*/
		return qtrue;
	}

	if ( Q_stricmp( cmd, "ui_trdata" ) == 0 ) {
		TransDataReceived(UI_Argv(1));
		return qtrue;
	}

	if ( Q_stricmp( cmd, "ui_holodeck" ) == 0) {
		/*UI_HolodeckMenu( atoi(UI_Argv(1)) );*/
		return qtrue;
	}

	if ( Q_stricmp( cmd, "ui_sqlmenu") == 0) {
		UI_sqlMenu();
		return qtrue;
	}

	return qfalse;
}

/*
=================
UI_Shutdown
=================
*/
void UI_Shutdown( void ) {
	/*trap_Cvar_Set ("rpg_playIntro", "1");*/
}



//--------------------------------------------
static char *UI_ParseFontParms(char *buffer,int32_t	propArray[CHARMAX][3])
{
	char	*token;
	int32_t		i,i2;
	
	while ( buffer ) 
	{
		token = COM_ParseExt( &buffer, qtrue );

		/* Start with open braket */
		if ( !Q_stricmp( token, "{" ) ) 
		{
			for (i=0;i<CHARMAX;++i)
			{
				/* Brackets for the numbers */
				token = COM_ParseExt( &buffer, qtrue );
				if ( !Q_stricmp( token, "{" ) ) 
				{
					;
				}
				else
				{
					trap_Print( va( S_COLOR_RED "UI_ParseFontParms : Invalid FONTS.DAT data, near character %d!\n",i));
					return(NULL);
				}

				for (i2=0;i2<3;++i2)
				{	
					token = COM_ParseExt( &buffer, qtrue );
					propArray[i][i2] = atoi(token);
				}

				token = COM_ParseExt( &buffer, qtrue );
				if ( !Q_stricmp( token, "}" ) ) 
				{
					;
				}
				else
				{
					trap_Print( va( S_COLOR_RED "UI_ParseFontParms : Invalid FONTS.DAT data, near character %d!\n",i));
					return(NULL);
				}
			}
		}

		token = COM_ParseExt( &buffer, qtrue );	/* Grab closing bracket */
		if ( !Q_stricmp( token, "}" ) ) 
		{
			break;
		}
	}

	return(buffer);
}

#define FONT_BUFF_LENGTH 20000

/*
=================
UI_LoadFonts
=================
*/
void UI_LoadFonts( void ) 
{
	char buffer[FONT_BUFF_LENGTH];
	int32_t len;
	fileHandle_t	f;
	char *holdBuf;

	len = trap_FS_FOpenFile( "ext_data/fonts.dat", &f, FS_READ );

	if ( !f ) 
	{
		trap_Print( va( S_COLOR_RED "UI_LoadFonts : FONTS.DAT file not found!\n"));
		return;
	}

	if (len > FONT_BUFF_LENGTH)
	{
		trap_Print( va( S_COLOR_RED "UI_LoadFonts : FONTS.DAT file bigger than %d!\n",FONT_BUFF_LENGTH));
		return;
	}

	/* initialise the data area */
	memset(buffer, 0, sizeof(buffer));	

	trap_FS_Read( buffer, len, f );

	trap_FS_FCloseFile( f );

	COM_BeginParseSession();

	holdBuf = (char *) buffer;
	holdBuf = UI_ParseFontParms( holdBuf,propMapTiny);
	holdBuf = UI_ParseFontParms( holdBuf,propMap);
	holdBuf = UI_ParseFontParms( holdBuf,propMapBig);
}

/*************************
UI_LoadClassString

TiM: We're in a server now,
load the class data string
and parse it up
*************************/

/*int32_t UI_LoadClassString( void ) {
	char	buffer[MAX_TOKEN_CHARS];
	int32_t		i, j;
	char	*val;
	char	*lineChar;
	char	*lineCharEnd;
	int32_t		classIndex=0;
	char*	className;

	if ( !ingameFlag ) {
		return qfalse;
	}

	//get config string
	trap_GetConfigString( CS_CLASS_DATA, buffer, sizeof( buffer ) );

	if ( !buffer[0] ) {
		Com_Printf( S_COLOR_RED "ERROR: Cannot load class data from config string.\n" );
		return qfalse;
	}

	className = Info_ValueForKey( buffer, "n");

	//already cached this class set
	if ( !Q_stricmp( className, uis.classSetName ) ) {
		return qtrue;
	}

	//purge present data in the class struct
	memset( &uis.classData, 0, sizeof ( uis.classData ) );

	for ( i = 0; i < MAX_CLASSES; i++ ) {
		//load string
		val = Info_ValueForKey( buffer, va( "c%i", i ) );

		if (!val[0])
			break;

		//First slash = consoleName
		lineChar = strstr( val, "|" );
		lineChar++;

		Q_strncpyz( uis.classData[classIndex].classNameConsole, val, ( strlen( val ) - strlen( lineChar ) ) );

		//Com_Printf( S_COLOR_RED "%s\n", uis.classData[i].classNameConsole );

		//next line should be formal name
		lineCharEnd = strstr( lineChar, "|" );

		val = lineChar;
		val[ strlen(lineChar) - strlen(lineCharEnd) ] = '\0';

		Q_strncpyz( uis.classData[classIndex].classNameFull, val, sizeof( uis.classData[classIndex].classNameFull ) );

		//isolate the bits at the end and find out if 
		//we're to show this rank or not in the menu
		val = Info_ValueForKey( buffer, va( "c%i", i ) );
		j = strlen( val );
		
		while ( j >= 0 && val[j] != '|' ) {
			j--;
		}

		val = val + j + 1;

		//Com_Printf( S_COLOR_RED "Val: %s\n", val );

		//check if this is a noShow class. 
		//skip it if it is
		if ( atoi( val ) & 1 ) {
			memset( uis.classData[classIndex].classNameConsole, 0, sizeof( uis.classData[classIndex].classNameConsole ) );
			memset( uis.classData[classIndex].classNameFull, 0, sizeof( uis.classData[classIndex].classNameFull ) );
		}
		else
			classIndex++;
	}
	
	Q_strncpyz( uis.classSetName, className, sizeof( uis.classSetName ) );

	return qtrue;
}*/

/*************************
UI_LoadClassData

TiM: Scopes out a class
file on the player's local
system and loads it, parsing 
the data we need to show a list
in the UI
*************************/

int32_t UI_InitClassData( char* fileName ) {
	char			buffer[32000];
	fileHandle_t	f;
	int32_t				fileLen;
	char			*textPtr;
	char			filePath[MAX_QPATH];
	int32_t				classIndex=0;
	char			*token;

	/* TiM - check if we've already loaded this file */
	if ( !Q_stricmp( uis.classSetName, fileName ) ) {
		return qtrue;
	}

	/* TiM - build the file name */
	Com_sprintf( filePath, sizeof( filePath ), "ext_data/classes/%s.classes", fileName );

	fileLen = trap_FS_FOpenFile( filePath, &f, FS_READ );

	if ( !fileLen ) {
		Com_Printf( S_COLOR_RED "ERROR: File not found: %s\n", fileName );
		return qfalse;
	}

	/*if ( fileLen > sizeof(buffer)-1) {
		Com_Printf( S_COLOR_RED "ERROR: File too large for buffer: %s\n", fileName );
		return qfalse;		
	}*/

	/* init file buffer */
	memset( buffer, 0, sizeof( buffer ) );

	trap_FS_Read( buffer, fileLen, f );

	if ( !buffer[0] ) {
		Com_Printf( S_COLOR_RED "ERROR: File could not be read: %s\n", fileName );
		return qfalse;		
	}

	trap_FS_FCloseFile( f );

	/* Re-init class list */
	memset( uis.classData, 0, sizeof( uis.classData ) );

	textPtr = buffer;

	COM_BeginParseSession();

	token = COM_Parse( &textPtr );

	if ( !token[0] ) {
		Com_Printf( S_COLOR_RED "ERROR: File was loaded, but no data could be read: %s\n", fileName );
		return qfalse;
	}

	if ( Q_stricmpn( token, "{", 1 ) ) {
		Com_Printf( S_COLOR_RED "ERROR: No opening brace { found in: %s\n", fileName );
		return qfalse;
	}

	while ( 1 ) {
		if ( classIndex >= MAX_CLASSES )
			break;

		if ( !Q_stricmpn( token, "{", 1 ) ) {
			while ( 1 ) {

				/* formal Name */
				if ( !Q_stricmpn( token, "formalName", 10 ) ) {
					if ( COM_ParseString( &textPtr, &token ) ) {
						Com_Printf( S_COLOR_RED "ERROR: Error parsing formalName parameter in file: %s.\n", fileName );
						continue;
					}

					Q_strncpyz( uis.classData[classIndex].classNameFull, token, sizeof( uis.classData[classIndex].classNameFull ) );
					continue;
				}

				/* console Name */
				if ( !Q_stricmpn( token, "consoleName", 11 ) ) {
					if ( COM_ParseString( &textPtr, &token ) ) {
						Com_Printf( S_COLOR_RED "ERROR: Error parsing consoleName parameter in file: %s.\n", fileName );
						continue;
					}

					Q_strncpyz( uis.classData[classIndex].classNameConsole, token, sizeof( uis.classData[classIndex].classNameConsole ) );
					continue;
				}
				
				/* TiM : Disregard noShow Classes */
				if ( !Q_stricmp( token, "noShow" ) ) {
					token = COM_Parse( &textPtr );

					if ( atoi( token ) >=1 ) {
						memset( uis.classData[classIndex].classNameConsole, 0, sizeof( uis.classData[classIndex].classNameConsole ) );
						memset( uis.classData[classIndex].classNameFull, 0, sizeof( uis.classData[classIndex].classNameFull ) );
						break;
					}
				}

				token = COM_Parse( &textPtr );
				if ( !token[0] )
					break;

				/* skip any more braces. They're obviously color vals */
				if ( !Q_stricmpn( token, "{", 1 ) ) {
					SkipBracedSection( &textPtr );
					continue;
				}

				if ( !Q_strncmp( token, "}", 1 ) ) {
					classIndex++;
					break;
				}
			}
		}

		token = COM_Parse( &textPtr );
		if ( !token[0] )
			break;
	}

	Q_strncpyz( uis.classSetName, fileName, sizeof( uis.classSetName ) );

	return qtrue;
}

/*************************
UI_PopulateRanksArray

TiM: Fills up a local array with the 
formal names of the currently selected rankset.
Used for choosing ranks in spin control menu items.
*************************/
int32_t UI_PopulateRanksArray( char* ranks[] ) {
	int32_t				i;
	rankNames_t		*rank;

	for( i = 0; i < MAX_RANKS; i++ ) {
		rank = &uis.rankSet.rankNames[i];
		
		if ( !rank->formalName[0] )
			break;

		ranks[i] = rank->formalName;
	}

	ranks[i] = "Other";
	/*ranks[i+1] = 0; *//*IMPORTANT: Spin controls need these or else values bleed into different controls*/

	return i;
}

/*************************
UI_InitRanksData

TiM: Upon call, it'll locate
which rankset the UI is set to,
load it up, and then reset the global
rankset to that new data
*************************/

void UI_InitRanksData( char* ranksName ) {
	char	filePath[MAX_QPATH];

	if ( !Q_stricmp( uis.rankSet.rankSetName, ranksName ) )
		goto refreshRank;

	/* Init the transfer space */
	memset( &uis.rankSet.rankNames, 0, sizeof( uis.rankSet.rankNames ) );

	/* Create the file route */
	Com_sprintf( filePath, sizeof( filePath ), "ext_data/ranksets/%s.ranks", ranksName );

	/* attempt to parse */
	if ( !BG_ParseRankNames( filePath, uis.rankSet.rankNames ) ) {
		
		/* Rank attempt failed.  Try loading the defaults.  If we end up with no loaded ranks... many menu elements will start crapping. bad */
		if ( !BG_ParseRankNames( va( "ext_data/ranksets/%s.ranks", RANKSET_DEFAULT), uis.rankSet.rankNames ) )
			trap_Error( va( "UI_InitRanksData: Was unable to load default rankset: %s", RANKSET_DEFAULT ) ); 
	}
	else {  
		int32_t i;
		 /*
		 * nvm, all loaded good :)
		 * set the current rank CVAR so it'll use this rankset next time they start the game
		 */
		trap_Cvar_Set( "ui_currentRankSet", ranksName );

		/* eh... to be on the safe side, save the name of the ranks in a local string */
		Q_strncpyz( uis.rankSet.rankSetName, ranksName, sizeof( uis.rankSet.rankSetName ) );

refreshRank:
		/* using our current cvar'd rank, do a compare.  if we find a match, set our player to that rank in the menu */
		for ( i=0, uis.currentRank=0; i < MAX_RANKS; i++ ) {
			if ( !Q_stricmp( uis.rankSet.rankNames[i].consoleName, UI_Cvar_VariableString( "ui_playerRank" ) ) ) {
				uis.currentRank = i;
				break;
			}
		}
	}
}

int32_t	UI_PopulateRankSetArray( char *rankSets[] ) {
	int32_t	i;

	for( i = 0; i < MAX_RANKSETS; i++ ) {
		if ( !uis.rankSet.rankSetNames[i][0] )
			break;

		rankSets[i] = uis.rankSet.rankSetNames[i];
	}

	rankSets[i] = 0;

	return i;
}

/*
=========================
UI_GetRankSets
TiM : Loads and stores a list of the
current ranksets we have.  Good for the player
settings menu, and the server settings menu.
=========================
*/

int32_t UI_GetRankSets( void )
{
	int32_t		numFiles, i;
	char		fileBuffer[2048];
	char		*filePtr;
	/*char		filePath[128];*/
	int32_t		fileLen;

	numFiles = trap_FS_GetFileList("ext_data/ranksets", ".ranks", fileBuffer, sizeof(fileBuffer) );
	/*Com_Printf( S_COLOR_RED "%s\n", filePtr);*/
	
	if ( numFiles == 1 )
		return 1;

	memset( &uis.rankSet.rankSetNames, 0, sizeof( uis.rankSet.rankSetNames ) );

	if ( numFiles > MAX_RANKSETS )
		numFiles = MAX_RANKSETS;

	filePtr = fileBuffer;
	i = 0;
	while ( i < numFiles ) {
		if ( !filePtr )
			break;
		
		fileLen = strlen(filePtr);

		/*Remove the extension*/
		if ( fileLen>6 && !Q_stricmp(filePtr +  fileLen - 6, ".ranks" ))
		{
			filePtr[fileLen-6] = '\0';
		}

		/*Com_Printf( S_COLOR_RED "%s\n", filePtr );*/

		Q_strncpyz( uis.rankSet.rankSetNames[i], filePtr, sizeof( uis.rankSet.rankSetNames[i] ) );
		
		filePtr += fileLen+1;
		i++;
	}

	trap_Print( va("%i ranksets detected\n", i ) );

	return i;
}

/**********************
UI_PopulateClassArray

TiM: Populate a char* 
array with these here
hard stored char vals
**********************/

int32_t	UI_PopulateClassArray( char *classes[] ) {
	int32_t	i;

	for( i = 0; i < MAX_CLASSES; i++ ) {
		if ( !uis.classData[i].classNameFull[0] )
			break;

		classes[i] = uis.classData[i].classNameFull;
	}

	classes[i] = "Other";
	/*classes[i+1] = 0;*/

	return i;
}

/*********************
UI_GetClassSets

Load a list of class
files and store them
locally.
*********************/

int32_t UI_GetClassSets( void )
{
	int32_t		numFiles, i;
	char		fileBuffer[2048];
	char		*filePtr;
	/*char		filePath[128];*/
	int32_t		fileLen;

	numFiles = trap_FS_GetFileList("ext_data/classes", ".classes", fileBuffer, sizeof(fileBuffer) );
	/*Com_Printf( S_COLOR_RED "%s\n", filePtr);*/
	
	if ( numFiles == 1 )
		return 1;

	memset( &uis.classList, 0, sizeof( uis.classList ) );

	if ( numFiles > MAX_CLASSSETS )
		numFiles = MAX_CLASSSETS;

	filePtr = fileBuffer;
	i = 0;
	while ( i < numFiles ) {
		if ( !filePtr )
			break;
		
		fileLen = strlen(filePtr);

		/* Remove the extension */
		if ( fileLen>8 && !Q_stricmp(filePtr +  fileLen - 8, ".classes" ))
		{
			filePtr[fileLen-8] = '\0';
		}

		/* Com_Printf( S_COLOR_RED "%s\n", filePtr ); */

		Q_strncpyz( uis.classList[i], filePtr, sizeof( uis.classList[i] ) );
		
		filePtr += fileLen+1;
		i++;
	}

	trap_Print( va("%i class sets detected\n", i ) );

	return i;
}

/*******************************
UI_PopulateClassSetArray

TiM: Populate a character pointer
array with class set names
*******************************/

int32_t	UI_PopulateClassSetArray( char *classSets[] ) {
	int32_t	i;

	for( i = 0; i < MAX_CLASSSETS; i++ ) {
		if ( !uis.classList[i][0] )
			break;

		classSets[i] = uis.classList[i];
	}

	return i;
}

/*
 * TiM Hold onto this... it could prove useful later
 * Plus I don't want to delete it as I tore several layers of hair out coding it rofl
 */
/*void	UI_InitRanksData( void ) {
	int32_t		numFiles, i=0, j=0;
	char	fileBuffer[2048];
	char*	filePtr;
	char	filePath[128];
	int32_t		fileLen;

	numFiles = trap_FS_GetFileList("ext_data/ranksets", ".ranks", fileBuffer, sizeof(fileBuffer) );
	filePtr = fileBuffer;
	//Com_Printf( S_COLOR_RED "%s\n", filePtr);

	for ( i = 0, j = 0; i < numFiles && j < MAX_RANKSETS; i++, filePtr+=(int32_t)fileLen+1 ) {
		fileLen = strlen(filePtr);

		if ( !fileLen )
			continue;
		
		memset( &uis.rankSets[j], 0, sizeof( uis.rankSets[j] ) );

		strcpy( uis.rankSets[j].rankSetName, filePtr );
		
		Com_sprintf( filePath, sizeof ( filePath ), "ext_data/ranksets/%s", filePtr );

		if ( BG_ParseRankNames( filePath, uis.rankSets[j].rankNames ) )
		{
			j++;
		}
	}

	trap_Print( va("%i ranksets parsed\n", j ) );
}*/

/*
=================
UI_Init
=================
*/
void UI_Init( void ) {

	Com_Printf ("This is RPG-X version %s compiled by %s on %s.\n", RPGX_VERSION, RPGX_COMPILEDBY, RPGX_COMPILEDATE);

	memset( &uis, 0, sizeof ( uis ) );

	/*if ( !uis.playCinematic ) {
		trap_Cmd_ExecuteText( EXEC_APPEND, "wait 5; wait 5; cinematic rpgx_intro.roq \n" );
		uis.playCinematic = qtrue;
	}*/

	init_tonextint(qfalse);

	UI_RegisterCvars();

	UI_LoadMenuText();

	UI_LoadButtonText();

	UI_LoadFonts();

	BG_LoadItemNames();

	UI_InitGameinfo();

	/* Initialize the ranks data */
	UI_InitRanksData( UI_Cvar_VariableString( "ui_currentRankSet" ) );
	UI_GetRankSets();

	UI_InitClassData( UI_Cvar_VariableString( "ui_currentClassSet" ) );
	UI_GetClassSets();

	/* cache redundant calulations */
	trap_GetGlconfig( &uis.glconfig );

	/* for 640x480 virtualized screen */
	uis.scaley = uis.glconfig.vidHeight * (1.0/480.0);
	uis.scalex = uis.glconfig.vidWidth * (1.0/640.0);
/*	uis.scale = uis.glconfig.vidHeight * (1.0/480.0);
	if ( uis.glconfig.vidWidth * 480 > uis.glconfig.vidHeight * 640 ) {
		// wide screen
		uis.bias = 0.5 * ( uis.glconfig.vidWidth - ( uis.glconfig.vidHeight * (640.0/480.0) ) );
	}
	else {
		// no wide screen
		uis.bias = 0;
	}
*/
	/* TiM - handle wide screens */
	if ( uis.glconfig.vidWidth * 480 > uis.glconfig.vidHeight * 640 ) 
	{
		uis.widescreen.ratio	= 640.0f*uis.scaley  * (1.0f/uis.glconfig.vidWidth);
		uis.widescreen.bias		= 0.5 * ( uis.glconfig.vidWidth - ( uis.glconfig.vidHeight * (640.0/480.0) ) );
	}
	else
	{
		uis.widescreen.ratio	= 0;
		uis.widescreen.bias		= 0;
	}

	/* initialize the menu system */
	Menu_Cache();

	uis.activemenu = NULL;
	uis.menusp     = 0;
	trap_Cvar_Create ("ui_initialsetup", "0", CVAR_ARCHIVE );

	/* TiM - initiate the client side portion of the security code */
	UI_SecurityCodeSetup();

	/* trap_Cvar_Create ("rpg_playIntro", "1", CVAR_ARCHIVE ); *//*RPG-X | Phenix | 25/02/2005 */
}

/*
================
UI_AdjustFrom640

Adjusted for resolution and screen aspect ratio
================
*/
void UI_AdjustFrom640( float *x, float *y, float *w, float *h ) {
	/* expect valid pointers */
	/* *x = *x * uis.scale + uis.bias;*/
	*x *= uis.scalex;
	*y *= uis.scaley;
	*w *= uis.scalex;
	*h *= uis.scaley;

	/* handle widescreen projections */
	if ( UI_IsWidescreen() )
	{
		*x *= uis.widescreen.ratio;
		*w *= uis.widescreen.ratio;
	
		/* center the elements into the middle of the screen */
		if ( uis.widescreen.state  == WIDESCREEN_CENTER )
			*x += uis.widescreen.bias;
	}
}

void UI_DrawNamedPic( float x, float y, float width, float height, const char *picname ) {
	qhandle_t	hShader;

	hShader = trap_R_RegisterShaderNoMip( picname );
	UI_AdjustFrom640( &x, &y, &width, &height );
	trap_R_DrawStretchPic( x, y, width, height, 0, 0, 1, 1, hShader );
}

void UI_DrawHandleStretchPic( float x, float y, float w, float h, float s0, float t0, float s1, float t1, qhandle_t hShader ) {
	UI_AdjustFrom640( &x, &y, &w, &h );
	trap_R_DrawStretchPic( x, y, w, h, s0, t0, s1, t1, hShader );
}

void UI_DrawHandlePic( float x, float y, float w, float h, qhandle_t hShader ) {
	float	s0;
	float	s1;
	float	t0;
	float	t1;

	/* TiM - security check */
	if ( w == 0.0f || h == 0.0f )
		return;

	if( w < 0 ) {	/* flip about vertical */
		w  = -w;
		s0 = 1;
		s1 = 0;
	}
	else {
		s0 = 0;
		s1 = 1;
	}

	if( h < 0 ) {	/* flip about horizontal */
		h  = -h;
		t0 = 1;
		t1 = 0;
	}
	else {
		t0 = 0;
		t1 = 1;
	}
	
	UI_AdjustFrom640( &x, &y, &w, &h );
	trap_R_DrawStretchPic( x, y, w, h, s0, t0, s1, t1, hShader );
}

/*
================
UI_FillRect

Coordinates are 640*480 virtual values
=================
*/
void UI_FillRect( float x, float y, float width, float height, const float *color ) {
	trap_R_SetColor( color );

	UI_AdjustFrom640( &x, &y, &width, &height );
	trap_R_DrawStretchPic( x, y, width, height, 0, 0, 0, 0, uis.whiteShader );

	trap_R_SetColor( NULL );
}

/*
================
UI_DrawRect

Coordinates are 640*480 virtual values
=================
*/
void UI_DrawRect( float x, float y, float width, float height, const float *color ) {
	trap_R_SetColor( color );

	UI_AdjustFrom640( &x, &y, &width, &height );

	trap_R_DrawStretchPic( x, y, width, 1, 0, 0, 0, 0, uis.whiteShader );
	trap_R_DrawStretchPic( x, y, 1, height, 0, 0, 0, 0, uis.whiteShader );
	trap_R_DrawStretchPic( x, y + height - 1, width, 1, 0, 0, 0, 0, uis.whiteShader );
	trap_R_DrawStretchPic( x + width - 1, y, 1, height, 0, 0, 0, 0, uis.whiteShader );

	trap_R_SetColor( NULL );
}

/*
=================
UI_Refresh
=================
*/
void UI_Refresh( int32_t realtime )
{
	vec4_t color;

	uis.frametime = realtime - uis.realtime;
	uis.realtime  = realtime;

	/*trap_Cvar_Set( "sys_lastactive", uis.realtime );*/
	
	if ( !( trap_Key_GetCatcher() & KEYCATCH_UI ) ) {
		return;
	}

	UI_UpdateCvars();

	if ( uis.activemenu )
	{
		uis.widescreen.state = WIDESCREEN_NONE;

		if (uis.activemenu->fullscreen)
		{
			/* draw the background */
			trap_R_SetColor( colorTable[CT_BLACK]);
			UI_DrawHandlePic(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, uis.whiteShader );
		}
		else if (!uis.activemenu->nobackground)
		{
			/* draw the background */
			color[0] = colorTable[CT_BLACK][0];
			color[1] = colorTable[CT_BLACK][1];
			color[2] = colorTable[CT_BLACK][1];
			color[3] = .75;

			trap_R_SetColor( color);
			UI_DrawHandlePic(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, uis.whiteShader );
		}

		uis.widescreen.state = WIDESCREEN_CENTER;

		if (uis.activemenu->draw)
			uis.activemenu->draw();
		else
			Menu_Draw( uis.activemenu );

		if( uis.firstdraw ) {
			UI_MouseEvent( 0, 0 );
			uis.firstdraw = qfalse;
		}

	}

	/* draw cursor */
	trap_R_SetColor( NULL );
	if (uis.cursorDraw)
	{
		UI_DrawHandlePic( uis.cursorx, uis.cursory, 16, 16, uis.cursor);
	}

#ifndef NDEBUG
	if (uis.debug)
	{
		/* cursor coordinates */
		uis.widescreen.state = WIDESCREEN_LEFT;
		UI_DrawString( 0, 0, va("(%d,%d)",uis.cursorx,uis.cursory), UI_LEFT|UI_SMALLFONT, colorRed, qtrue );
		uis.widescreen.state = WIDESCREEN_CENTER;
	}
#endif

	/*
 	 * delay playing the enter sound until after the
	 * menu has been drawn, to avoid delay while
	 * caching images
	 */
	if (m_entersound)
	{
		trap_S_StartLocalSound( menu_in_sound, CHAN_LOCAL_SOUND );
		m_entersound = qfalse;
	}
}

qboolean UI_CursorInRect (int32_t x, int32_t y, int32_t width, int32_t height)
{
	if (uis.cursorx < x ||
		uis.cursory < y ||
		uis.cursorx > x+width ||
		uis.cursory > y+height)
		return qfalse;

	return qtrue;
}

/*
==============
UI_DrawNumField

Take x,y positions as if 640 x 480 and scales them to the proper resolution

==============
*/
static void UI_DrawNumField (int32_t x, int32_t y, int32_t width, int32_t value,int32_t charWidth,int32_t charHeight)
{
	char	num[16], *ptr;
	int32_t		l;
	int32_t		frame;
	int32_t		xWidth;

	if (width < 1)
		return;

	/* draw number string */
	if (width > 15)
		width = 15;

	switch ( width ) {
	case 1:
		value = value > 9 ? 9 : value;
		value = value < 0 ? 0 : value;
		break;
	case 2:
		value = value > 99 ? 99 : value;
		value = value < -9 ? -9 : value;
		break;
	case 3:
		value = value > 999 ? 999 : value;
		value = value < -99 ? -99 : value;
		break;
	case 4:
		value = value > 9999 ? 9999 : value;
		value = value < -999 ? -999 : value;
		break;
	}

	Com_sprintf (num, sizeof(num), "%i", value);
	l = strlen(num);
	if (l > width)
		l = width;

	xWidth = (charWidth/3);

	x += (xWidth)*(width - l);

	ptr = num;
	while (*ptr && l)
	{
/*if (*ptr == '-')
	frame = STAT_MINUS;
  else */
			frame = *ptr -'0';

		UI_DrawHandlePic( x,y, 16, 16, uis.smallNumbers[frame] );

		x += (xWidth);
		ptr++;
		l--;
	}

}


/*
==============
UI_PrintMenuGraphics
==============
*/
void UI_PrintMenuGraphics(menugraphics_s *menuGraphics,int32_t maxI)
{
	int32_t i;
	const char *text;

	/* Now that all the changes are made, print up the graphics */
	for (i=0;i<maxI;++i)
	{
		if (menuGraphics[i].type == MG_GRAPHIC)
		{	
			trap_R_SetColor( colorTable[menuGraphics[i].color]);
			UI_DrawHandlePic( menuGraphics[i].x, 
				menuGraphics[i].y,
				menuGraphics[i].width,
				menuGraphics[i].height,
				menuGraphics[i].graphic);
			trap_R_SetColor( colorTable[CT_NONE]);
		}
		else if (menuGraphics[i].type == MG_STRING)
		{
			if (menuGraphics[i].file)
			{
				text = menuGraphics[i].file;
			}
			else if (menuGraphics[i].normaltextEnum)
			{
				text= menu_normal_text[menuGraphics[i].normaltextEnum];
			}
			else
			{
				return;
			}

			UI_DrawProportionalString( menuGraphics[i].x, 
				menuGraphics[i].y, 
				text,
				menuGraphics[i].style,
				colorTable[menuGraphics[i].color]);
		}
		else if (menuGraphics[i].type == MG_NUMBER)
		{
			trap_R_SetColor( colorTable[menuGraphics[i].color]);
			UI_DrawNumField (menuGraphics[i].x,
				menuGraphics[i].y, 
				menuGraphics[i].max, 
				menuGraphics[i].target,
				menuGraphics[i].width,	
				menuGraphics[i].height);
			trap_R_SetColor( colorTable[CT_NONE]);
		}
		else if (menuGraphics[i].type == MG_NONE)
		{
			;	/* Don't print anything */
		}
	}
}


/*
==============
UI_PrecacheMenuGraphics
==============
*/
void UI_PrecacheMenuGraphics(menugraphics_s *menuGraphics,int32_t maxI)
{
	int32_t i;

	for (i=0;i<maxI;++i)
	{
		if (menuGraphics[i].type == MG_GRAPHIC)
		{
			menuGraphics[i].graphic = trap_R_RegisterShaderNoMip(menuGraphics[i].file);
		}
	}

}

/*
===============
MenuFrame_Cache
===============
*/
static void MenuFrame_Cache(void)
{
	s_menuframe.cornerUpper = trap_R_RegisterShaderNoMip("menu/common/corner_ll_47_7.tga");
	s_menuframe.cornerUpper2= trap_R_RegisterShaderNoMip("menu/common/corner_ul_47_7.tga");
	s_menuframe.cornerLower = trap_R_RegisterShaderNoMip("menu/common/corner_ll_47_18.tga");
}

/*
===============
UI_FrameTop_Graphics
===============
*/
static void UI_FrameTop_Graphics(menuframework_s *menu)
{

	trap_R_SetColor( colorTable[CT_DKPURPLE2] );
	UI_DrawHandlePic(  30,  24,  47,  54, uis.whiteShader);	/* Top left hand column */

	trap_R_SetColor( colorTable[CT_DKPURPLE3]);
	UI_DrawHandlePic(  30,  81,  47,  34, uis.whiteShader);	/* Middle left hand column */
	UI_DrawHandlePic(  30,  115,  128,  64, s_menuframe.cornerUpper);	/* Corner */
	UI_DrawHandlePic(  100, 136,  49,  6.5, uis.whiteShader);	/* Start of line across bottom of top third section */
	/*ABOVE HAS LINE BUG 111 - 38 - 7 */

	trap_R_SetColor( colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic( 152, 136, 135,   7, uis.whiteShader);	/* 2nd line across bottom of top third section */

	trap_R_SetColor( colorTable[CT_LTPURPLE2]);
	UI_DrawHandlePic( 290, 136,  12,   7, uis.whiteShader);	/* 3rd line across bottom of top third section */

	trap_R_SetColor( colorTable[CT_LTBROWN1]); /* RPG-X ADDITION */
	UI_DrawHandlePic( 305, 139,  60,   4, uis.whiteShader);	/* 4th line across bottom of top third section */

	/*RPG-X REMOVE trap_R_SetColor( colorTable[CT_LTBROWN1]);*/
	UI_DrawHandlePic( 368, 136,  111,   7, uis.whiteShader); /* 5th line across bottom of top third section */

	if (menu->titleI)
	{
		UI_DrawProportionalString( menu->titleX, menu->titleY ,menu_normal_text[menu->titleI],
			 UI_RIGHT|UI_BIGFONT, colorTable[CT_LTORANGE]);
	}
}

/*
===============
UI_FrameBottom_Graphics
===============
*/
static void UI_FrameBottom_Graphics(void)
{
	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(  30, 147, 128,  64, s_menuframe.cornerUpper2); /* Top corner */
	UI_DrawHandlePic(  50, 147,  99,   7, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_LTBROWN1]); /* DKBROWN1 */
	UI_DrawHandlePic( 152, 147, 135,   7, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKRED1]); /* DKBROWN1 */
	UI_DrawHandlePic( 290, 147,  12,   7, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic( 305, 147,  60,   4, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_LTGOLD1]); /* DKBROWN1 */
	UI_DrawHandlePic( 368, 147, 111,   7, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(  30, 173,  47,  27, uis.whiteShader);	/* Top left column (81453) */

	/* Start of piece (431108) */
	UI_DrawHandlePic(  30, 392,  47,  33, uis.whiteShader);	/* Bottom left column */
	UI_DrawHandlePic(  30, 425, 128,  64, s_menuframe.cornerLower); /* Bottom Left Corner */

	trap_R_SetColor( colorTable[CT_LTBLUE1]); /* LTBROWN1]); */
	UI_DrawHandlePic( 96,  438, 268,  18, uis.whiteShader);	/* Bottom front Line */

	trap_R_SetColor(NULL);
}

/*
=================
UI_MenuBottomLineEnd_Graphics
=================
*/
void UI_MenuBottomLineEnd_Graphics (const char *string,int32_t color, qboolean *space )
{
	int32_t holdX,holdLength;

	trap_R_SetColor( colorTable[color]);
	holdX = MENU_TITLE_X - (UI_ProportionalStringWidth( string,UI_SMALLFONT));
	holdLength = (367 + 6) - holdX;
	UI_DrawHandlePic( 367,  438,  holdLength,  18, uis.whiteShader); /* Bottom end line */

	/* TiM - stop the text leaving the box in some instances */
	if ( space != NULL )
	{
		if ( (holdLength < 0 ? -holdLength : holdLength ) < UI_ProportionalStringWidth( Q3_VERSION,UI_TINYFONT) )
			*space = qfalse;
		else
			*space = qtrue;
	}

	/* Com_Printf( S_COLOR_RED "Length: %i, Width: %i\n", holdLength, (UI_ProportionalStringWidth( Q3_VERSION,UI_TINYFONT)) ); */
}

/*
=================
UI_MenuFrame
=================
*/
void UI_MenuFrame(menuframework_s *menu)
{
	qboolean space=qtrue;

	if (!s_menuframe.initialized)
	{
		MenuFrame_Cache();
	}

	if (!ingameFlag)
	{
		menu->fullscreen			= qtrue;
	}
	else	/* In game menu */
	{
		menu->fullscreen			= qfalse;
	}

	/* Graphic frame */
	UI_FrameTop_Graphics(menu);	/* Top third */
	UI_FrameBottom_Graphics();	/* Bottom two thirds */

	/* Add foot note */
	if (menu->footNoteEnum)
	{
		UI_DrawProportionalString(  MENU_TITLE_X, 440, menu_normal_text[menu->footNoteEnum],UI_RIGHT | UI_SMALLFONT, colorTable[CT_LTORANGE]);
		UI_MenuBottomLineEnd_Graphics (menu_normal_text[menu->footNoteEnum],CT_LTBROWN1, &space);
	}

	/* Print version */
	if ( space )
		UI_DrawProportionalString(  371, 445, Q3_VERSION,UI_TINYFONT, colorTable[CT_BLACK]);
}

/*
=================
UI_MenuFrame2
=================
*/
void UI_MenuFrame2(menuframework_s *menu)
{
	qboolean space=qtrue;

	if (!s_menuframe.initialized)
	{
		MenuFrame_Cache();
	}

	if (!ingameFlag)
	{
		menu->fullscreen			= qtrue;
	}
	else	/* In game menu */
	{
		menu->fullscreen			= qfalse;
	}

	if (menu->titleI)
	{
		UI_DrawProportionalString( menu->titleX, menu->titleY ,menu_normal_text[menu->titleI],
			 UI_RIGHT|UI_BIGFONT, colorTable[CT_LTORANGE]);
	}

	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(  30, 25,  47,  119, uis.whiteShader);	/* Top left column */
	UI_DrawHandlePic(  30, 147,  47,  53, uis.whiteShader);	/* left column */

	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	/*UI_DrawHandlePic(  30, 175,  47,  25, uis.whiteShader);*/ /* Mid left column*/
	UI_DrawHandlePic(  30, 392,  47,  33, uis.whiteShader);	/* Bottom left column */
	UI_DrawHandlePic(  30, 425, 128,  64, s_menuframe.cornerLower); /* Bottom Left Corner */

	trap_R_SetColor( colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic( 96,  438, 268,  18, uis.whiteShader);	/* Bottom front Line */

	/* Add foot note */
	if (menu->footNoteEnum)
	{
		UI_DrawProportionalString(  MENU_TITLE_X, 440, menu_normal_text[menu->footNoteEnum],UI_RIGHT | UI_SMALLFONT, colorTable[CT_LTORANGE]);
		UI_MenuBottomLineEnd_Graphics (menu_normal_text[menu->footNoteEnum],CT_LTBROWN1, &space);
	}
	trap_R_SetColor(NULL);

	/* Print version */
	if ( space )
		UI_DrawProportionalString(  371, 445, Q3_VERSION,UI_TINYFONT, colorTable[CT_BLACK]);
}

/*
=================
UI_AntiPlagiarise
RPG-X: TiM- After I noticed how Scott and his buddies
were having a grand time renaming RPG-X to D4, I thought of this
little function to make that a little harder from them.
The function works in 2 phases:
firstly, it scans each string that is parsed into the game
and if either D4 or RPG-Y is seen, it quits the game right away, 
giving them absolutely no reason why.  Hopefully if they're not too smart, 
they'll think it's some other kind of error :D
Secondly, (if the arg is 1) if the string 'RPG-X' is NOT present (Like in the main menu title),
it quits as well. :)
=================
*/

const char* illegalStrings[] = { "D4", "d4", "RPG-Y", "rpg-y", "AORP", "aorp" };

/*static void UI_AntiPlagiarise ( const char *textLine, int32_t arg )
{
	int32_t i;
	char language[32];

	trap_Cvar_VariableStringBuffer( "g_language", language, 32 );

	// If it's English then no extension
	if (language[0]=='\0' || Q_stricmp ("ENGLISH",language)==0)
	{
		if (arg == 0 ) {
			for (i = 0; i < 6; i++ ) {
				if ( strstr( textLine, illegalStrings[i] ) != 0 ) 
					trap_Cmd_ExecuteText( EXEC_APPEND, "quit\n" );
			}
		}

		else if (arg == 1 ) {
			if ( !strstr( textLine, "RPG-X" ) ) {
				trap_Cmd_ExecuteText( EXEC_APPEND, "quit\n" );
			}
		}
	}
}*/

#define MAXMENUTEXT 15000
char	MenuText[MAXMENUTEXT];

/*
=================
UI_ParseMenuText
=================
*/
static void UI_ParseMenuText(void)
{
	char *token;
	char *buffer;
	int32_t  i;
	int32_t  len;
	/*int32_t j;*/

	COM_BeginParseSession();

	buffer = MenuText;

 	 /* RPG-X: TiM - Stop any D3 menu hacks in their tracks :P */
	/*UI_AntiPlagiarise( buffer, 0 );*/

	i = 1;	/* Zero is null string */
	while ( buffer ) 
	{
		token = COM_ParseExt( &buffer, qtrue );

		len = strlen(token);
		if (len)
		{
			menu_normal_text[i] = (buffer - (len + 1));
			*(buffer - 1) = '\0';		/* Place an string end where is belongs. */
			i++;

		}

		if (uis.debug)
			Com_Printf( S_COLOR_RED "UI_ParseMenuText - Line: %i, String: %s\n", i-1, menu_normal_text[i-1] );

		if (i> MNT_MAX)
		{
			Com_Printf( S_COLOR_RED "UI_ParseMenuText : too many values! Needed %d but got %d.\n",MNT_MAX,i);
			return;
		}
	}
	if (i != MNT_MAX)
	{
		Com_Printf( S_COLOR_RED "UI_ParseMenuText : not enough lines. Read %d of %d!\n",i,MNT_MAX);
		for(;i<MNT_MAX;i++) {
			menu_normal_text[i] = "?";
		}
	}

	/* RPG-X: TiM- If 'RPG-X' is missing from any of the main menu titles, kick it ;) */
	/*for(j=1; j < 12; j++) {
		UI_AntiPlagiarise( menu_normal_text[j], 1 );
	}*/
}

/*
=================
UI_LoadMenuText
=================
*/
void UI_LoadMenuText()
{
	int32_t len;/*,i;*/
	fileHandle_t	f;
	char	filename[MAX_QPATH];

	UI_LanguageFilename("ext_data/mp_normaltext","dat",filename);

	len = trap_FS_FOpenFile( filename, &f, FS_READ );

	if ( !f ) 
	{
		Com_Error( ERR_FATAL, "UI_LoadMenuText : MP_NORMALTEXT.DAT file not found!\n");
		return;
	}

	if ( len > MAXMENUTEXT ) 
	{
		Com_Error( ERR_FATAL, "UI_LoadMenuText : MP_NORMALTEXT.DAT size (%d) > max (%d)!\n", len, MAXMENUTEXT );
		return;
	}

	/* initialise the data area */
	memset(MenuText, 0, sizeof(MenuText));	

	trap_FS_Read( MenuText, len, f );

	trap_FS_FCloseFile( f );

	UI_ParseMenuText();

}

#define MAXBUTTONTEXT 50000 /*15000 - Might need optimizing*/
char	ButtonText[MAXBUTTONTEXT];

/*
=================
UI_ParseButtonText
=================
*/
static void UI_ParseButtonText(void)
{
	char	*token;
	char *buffer;
	int32_t i;
	int32_t len;

	COM_BeginParseSession();

	buffer = ButtonText;

	/* RPG-X: TiM - Stop any D3 menu hacks in their tracks :P */
	/*UI_AntiPlagiarise( buffer, 0 );*/

	i = 1;	/* Zero is null string */
	while ( buffer ) 
	{
		/*G_ParseString( &buffer, &token);*/
		token = COM_ParseExt( &buffer, qtrue );

		len = strlen(token);
		if (len)
		{

			if ((len == 1) && (token[0] == '/'))	/* A NULL? */
			{
				menu_button_text[i][0] = menuEmptyLine;
				menu_button_text[i][1] = menuEmptyLine;
			}
			else
			{
				menu_button_text[i][0] = (buffer - (len + 1));	/* The +1 is to get rid of the " at the beginning of the string.*/
			}

			*(buffer - 1) = '\0';		/* Place an string end where is belongs. */

			token = COM_ParseExt( &buffer, qtrue );
			len = strlen(token);
			if (len)
			{
				menu_button_text[i][1] = (buffer - (len+1));	/* The +1 is to get rid of the " at the beginning of the string. */
				*(buffer-1) = '\0';		/* Place an string end where is belongs. */
			}
			++i;
		}

		if (uis.debug)
			Com_Printf( S_COLOR_RED "UI_ParseButtonText - Line: %i, String1: %s, String2: %s\n", i-1, menu_button_text[i-1][0], menu_button_text[i-1][1] );

		if (i> MBT_MAX)
		{
			Com_Printf( S_COLOR_RED "UI_ParseButtonText : too many values! Needed %d but got %d.\n",MBT_MAX,i);
			return;
		}
	}
	if (i != MBT_MAX)
	{
		Com_Printf( S_COLOR_RED "UI_ParseButtonText : not enough lines. Read %d of %d!\n",i,MBT_MAX);
		for(;i<MBT_MAX;i++) {
			menu_button_text[i][0] = "?";
			menu_button_text[i][1] = "?";
		}
	}
}

/*
=================
UI_LoadButtonText
=================
*/
void UI_LoadButtonText()
{
	char	filename[MAX_QPATH];
	int32_t len,i;
	fileHandle_t	f;

	UI_LanguageFilename("ext_data/mp_buttontext","dat",filename);

	len = trap_FS_FOpenFile( filename, &f, FS_READ );

	if ( !f ) 
	{
		Com_Printf( S_COLOR_RED "UI_LoadButtonText : MP_BUTTONTEXT.DAT file not found!\n");
		return;
	}

	if ( len > MAXBUTTONTEXT ) 
	{
		Com_Printf( S_COLOR_RED "UI_LoadButtonText : MP_BUTTONTEXT.DAT too big!\n");
		return;
	}

	for (i=0;i<MBT_MAX;++i)
	{
		menu_button_text[i][0] = menuEmptyLine;
		menu_button_text[i][1] = menuEmptyLine;
	}

	/* initialise the data area */
	memset(ButtonText, 0, sizeof(ButtonText));	

	trap_FS_Read( ButtonText, len, f );

	trap_FS_FCloseFile( f );

	UI_ParseButtonText();

}

/*
=================
UI_InitSpinControl
=================
*/
void UI_InitSpinControl(menulist_s *spincontrol)
{
	spincontrol->generic.type		= MTYPE_SPINCONTROL;
	spincontrol->generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	spincontrol->textcolor			= CT_BLACK;
	spincontrol->textcolor2			= CT_WHITE;
	spincontrol->color				= CT_DKPURPLE1;
	spincontrol->color2				= CT_LTPURPLE1;
	spincontrol->textX				= MENU_BUTTON_TEXT_X;
	spincontrol->textY				= MENU_BUTTON_TEXT_Y;
}

/*
UI_LanguageFilename - create a filename with an extension based on the value in g_language
*/
void UI_LanguageFilename(char *baseName,char *baseExtension,char *finalName)
{
	char	language[32];
	fileHandle_t	file;

	trap_Cvar_VariableStringBuffer( "g_language", language, 32 );

	/* If it's English then no extension */
	if (language[0]=='\0' || Q_stricmp ("ENGLISH",language)==0)
	{
		Com_sprintf(finalName,MAX_QPATH,"%s.%s",baseName,baseExtension);
	}
	else
	{
		Com_sprintf(finalName,MAX_QPATH,"%s_%s.%s",baseName,language,baseExtension);

		/* Attempt to load the file */
		trap_FS_FOpenFile( finalName, &file, FS_READ );

		if ( file == 0 ) /* This extension doesn't exist, go English. */
		{
			Com_sprintf(finalName,MAX_QPATH,"%s.%s",baseName,baseExtension); /* the caller will give the error if this isn't there */
		}
		else
		{
			trap_FS_FCloseFile( file );
		}
	}
}

/*=========================
UI_SecurityCodeSetup

Upon connecting to a server,
set up the client-side security
system.

This involves:
a)validating the idkey is there
b)checking its hash against the
UI, and spitting an error if the
UI one is different.
c)if the UI one is default,
generate a new key now
==========================*/

static void SecurityFeedback( qboolean result )
{
	trap_Cmd_ExecuteText( EXEC_APPEND, "quit\n" );
}

void UI_SecurityCodeSetup ( void )
{
	fileHandle_t			f;
	byte					buffer[SECURITY_SIZE];
	int32_t						fileLen;
	rpgxSecurityFile_t		*code;
	rpgxSecurityFile_t		wCode;
	static qboolean			ui_SecuritySetup=qfalse;

	/* QVM Hack */
	if ( !ui_SecuritySetup )
		ui_SecuritySetup = qtrue;
	else
		return;

	fileLen = trap_FS_FOpenFile( SECURITY_FILE, &f, FS_READ );

	if ( !f )
	{
		UI_ConfirmMenu( menu_normal_text[MNT_ID_NOTTHERE], 0, SecurityFeedback );
		return;
	}

	if ( fileLen != SECURITY_SIZE )
	{
		Com_Printf( S_COLOR_RED "rpgxid.dat is wrong size. %i, should be %i\n", fileLen, SECURITY_SIZE );
		UI_ConfirmMenu( menu_normal_text[MNT_ID_WRONGSIZE], 0, SecurityFeedback );
		return;		
	}
	
	trap_FS_Read( buffer, SECURITY_SIZE, f );
	trap_FS_FCloseFile( f );

	code = (rpgxSecurityFile_t *)((byte *)buffer);

	#if defined(__linux__)
	unsigned long ID = (buffer[3]<<24|buffer[2]<<16|buffer[1]<<8|buffer[0]);
	unsigned long SECID = ('7'<<24|'X'<<16|'G'<<8|'R');
	if ( !code || ID != SECID)
	#else
	if ( !code || code->ID != SECURITY_ID )
	#endif
	{
		if ( !code )
			Com_Printf( S_COLOR_RED "No data was able to be loaded\n" );
		else
			Com_Printf( S_COLOR_RED "ID was %lu, should be %u\n", code->ID, SECURITY_ID );

		UI_ConfirmMenu( menu_normal_text[MNT_ID_INVALID], 0, SecurityFeedback );
		return;
	}

	/*Com_Printf( S_COLOR_RED "FileID: %u, RealID: %u. FileHash: %u, ConsoleHash: %u, DefHash: %u. FileCode: %u, ConsoleCode: %u, DefCode: %u.\n",
			code->ID, SECURITY_ID, code->hash, atoul( sv_securityHash.string ), SECURITY_HASH, code->playerID, atoul( sv_securityCode.string ), SECURITY_PID );*/

	/* if hash is identical to console to default, then generate a new one */
	if ( code->hash == atoul(sv_securityHash.string)
			&&
		code->hash == SECURITY_HASH )
	{
		fileHandle_t		wf;
		int32_t					i;

		/*Com_Printf( S_COLOR_RED "Building new key!\n" );*/

		memset( &wCode, 0, sizeof( rpgxSecurityFile_t ) );

		code = &wCode;

		code->ID = SECURITY_ID;

		/* generate our player hash */
		while ( code->hash == 0 || code->hash == SECURITY_HASH )
		{
			unsigned long			bit=0;
			/* set a pretty good random seed */
			srand( trap_Milliseconds() );
			/*code->hash = (int32_t)(rand() / (((double)RAND_MAX + 1)/ SECURITY_HASH));*/
			for ( i = 0; i < 32; i++ )
			{
				if ( (irandom(1, 2)-1 ) )
				{
					bit += 1 << i;
				}
				
				/*bit = bit | (irandom(1, 2)-1);*/
				/*bit <<= 1;*/
			}

			code->hash = bit;
		}

		/* generate our player id */
		/* TiM - shifted to client to be built off of IP */
		while ( code->playerID == 0 || code->playerID == SECURITY_PID )
		{
			srand( (int32_t)(trap_Milliseconds() * irandom( 0, 0x7FFF )) );
			for ( i = 0; i < 32; i++ )
			{
				code->playerID |= irandom(1, 2)-1;
				code->playerID <<= 1;
			}
		}
		/*code->playerID = SECURITY_PID;*/

		/*generate random padding to make viewing this in a hex editor harder*/
		/*code->padding	= irandom( 0, 0xffff );*/
		/*code->padding2	= irandom( 0, 0xffff );*/
		/*code->padding3	= irandom( 0, 0xffff );*/
		
		/* overwrite the file */
		trap_FS_FOpenFile( SECURITY_FILE, &wf, FS_WRITE );
		trap_FS_Write( code, SECURITY_SIZE, wf );
		trap_FS_FCloseFile( wf );

		trap_Cvar_Set( "sv_securityHash", va( "%lu", code->hash ) );
	}
	
	/* TiM - NYARRR!!!!! ioEF has a weird config system which appears to trip this system. */
	/*  *sigh* I guess for now we'll have to isolate this for now */

	/*else if ( code->hash != SECURITY_HASH 
				&& 
			code->hash != atoul(sv_securityHash.string) )
	{
		//Com_Printf( S_COLOR_RED "code->hash: %u, sv: %u, Default: %u\n", code->hash, atoul(sv_securityHash.string), SECURITY_HASH );
		UI_ConfirmMenu( menu_normal_text[MNT_ID_INVALID], 0, SecurityFeedback );
		return;
	}
	else if ( code->playerID != SECURITY_PID
				&&
				code->playerID != atoul(sv_securityCode.string) )
	{
		//Com_Printf( S_COLOR_RED "code->PID: %u, sv: %u, Default: %u\n", code->playerID, atoul(sv_securityCode.string), SECURITY_PID );
		UI_ConfirmMenu( menu_normal_text[MNT_ID_INVALID], 0, SecurityFeedback );
		return;
	}*/

	/*
 	 * update the security code value and lock it each time
	 * from here, it is subsequently sent to the server on player connect
	 */
	trap_Cvar_Set( "sv_securityCode", va( "%lu", code->playerID ) );

}

