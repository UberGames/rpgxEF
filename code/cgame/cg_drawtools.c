// Copyright (C) 1999-2000 Id Software, Inc.
//
// cg_drawtools.c -- helper functions called by cg_draw, cg_scoreboard, cg_info, etc
#include "cg_local.h"

#define CHARMAX 256

static qboolean CG_IsWidescreen( void )
{
	if ( cg_handleWidescreen.integer && cgs.widescreen.ratio && cgs.widescreen.state != WIDESCREEN_NONE )
		return qtrue;

	return qfalse;
}

/*
================
CG_AdjustFrom640

Adjusted for resolution and screen aspect ratio
================
*/
void CG_AdjustFrom640( float *x, float *y, float *w, float *h ) {
//TiM - huh.... looks like the were looking into widescreens.
//only offsetting the X value isn't good enough tho
#if 0
	// adjust for wide screens
	if ( cgs.glconfig.vidWidth * 480 > cgs.glconfig.vidHeight * 640 ) {
		*x += 0.5 * ( cgs.glconfig.vidWidth - ( cgs.glconfig.vidHeight * 640 / 480 ) );
	}
#endif
	// scale for screen sizes
	*x *= cgs.screenXScale;
	*y *= cgs.screenYScale;
	*w *= cgs.screenXScale;
	*h *= cgs.screenYScale;

	//TiM - handle widescreens
	if ( CG_IsWidescreen() )
	{
		*w *= cgs.widescreen.ratio;

		if ( cgs.widescreen.state == WIDESCREEN_RIGHT )
			*x = (*x * cgs.widescreen.ratio) + (cgs.widescreen.bias*2);//*x = (*x+*w) - (*w * cgs.widescreen.ratio);
		else if ( cgs.widescreen.state == WIDESCREEN_CENTER )
			*x = (*x * cgs.widescreen.ratio) + (cgs.widescreen.bias);  
		else
			*x *= cgs.widescreen.ratio;
	}
}

/*
================
CG_FillRect

Coordinates are 640*480 virtual values
=================
*/
void CG_FillRect( float x, float y, float width, float height, const float *color ) {
	trap_R_SetColor( color );

	CG_AdjustFrom640( &x, &y, &width, &height );
	trap_R_DrawStretchPic( x, y, width, height, 0, 0, 0, 0, cgs.media.whiteShader );

	trap_R_SetColor( NULL );
}



/*
================
CG_DrawPic

Coordinates are 640*480 virtual values
=================
*/
void CG_DrawPic( float x, float y, float width, float height, qhandle_t hShader ) {
	CG_AdjustFrom640( &x, &y, &width, &height );
	trap_R_DrawStretchPic( x, y, width, height, 0, 0, 1, 1, hShader );
}

/*
================
CG_DrawStretchPic

TiM: Made so we can use
s1 - t2 as parameters as well :)

Coordinates are 640*480 virtual values
=================
*/
void CG_DrawStretchPic( float x, float y, float width, float height, float s1, float t1, float s2, float t2, qhandle_t hShader ) {
	CG_AdjustFrom640( &x, &y, &width, &height );
	trap_R_DrawStretchPic( x, y, width, height, s1, t1, s2, t2, hShader );
}

/*
===============
CG_DrawChar

Coordinates and size in 640*480 virtual screen size
===============
*/
void CG_DrawChar( int x, int y, int width, int height, int ch ) {
	int row, col;
	float frow, fcol;
	float size,size2;
	float	ax, ay, aw, ah;

	ch &= 255;

	if ( ch == ' ' ) {
		return;
	}

	ax = x;
	ay = y;
	aw = width;
	ah = height;
	CG_AdjustFrom640( &ax, &ay, &aw, &ah );

	row = ch>>4;
	col = ch&15;


	frow = row*0.0625;
	fcol = col*0.0625;
	size = 0.03125;
	size2 = 0.0625;

	trap_R_DrawStretchPic( ax, ay, aw, ah,
					   fcol, frow, 
					   fcol + size, frow + size2, 
					   cgs.media.charsetShader );

}


/*
==================
CG_DrawStringExt

Draws a multi-colored string with a drop shadow, optionally forcing
to a fixed color.

Coordinates are at 640 by 480 virtual resolution
==================
*/
void CG_DrawStringExt( int x, int y, const char *string, const float *setColor, 
	qboolean forceColor, qboolean shadow, int charWidth, int charHeight, int maxChars ) {
	vec4_t		color;
	const char	*s;
	int			xx;
	int			cnt;

	if (maxChars <= 0)
		maxChars = 32767; // do them all!

	// draw the drop shadow
	if (shadow) {
		color[0] = color[1] = color[2] = 0;
		color[3] = setColor[3];
		trap_R_SetColor( color );
		s = string;
		xx = x;
		cnt = 0;
		while ( *s && cnt < maxChars) {
			if ( Q_IsColorString( s ) ) {
				s += 2;
				continue;
			}
			CG_DrawChar( xx + 2, y + 2, charWidth, charHeight, *s );
			cnt++;
			xx += charWidth;
			s++;
		}
	}

	// draw the colored text
	s = string;
	xx = x;
	cnt = 0;
	trap_R_SetColor( setColor );
	while ( *s && cnt < maxChars) {
		if ( Q_IsColorString( s ) ) {
			if ( !forceColor ) {
				memcpy( color, g_color_table[ColorIndex(*(s+1))], sizeof( color ) );
				color[3] = setColor[3];
				trap_R_SetColor( color );
			}
			s += 2;
			continue;
		}
		CG_DrawChar( xx, y, charWidth, charHeight, *s );
		xx += charWidth;
		cnt++;
		s++;
	}
	trap_R_SetColor( NULL );
}

void CG_DrawBigString( int x, int y, const char *s, float alpha ) {
	float	color[4];

	color[0] = color[1] = color[2] = 1.0;
	color[3] = alpha;
	CG_DrawStringExt( x, y, s, color, qfalse, qtrue, BIGCHAR_WIDTH, BIGCHAR_HEIGHT, 0 );
}

void CG_DrawBigStringColor( int x, int y, const char *s, vec4_t color ) {
	CG_DrawStringExt( x, y, s, color, qtrue, qtrue, BIGCHAR_WIDTH, BIGCHAR_HEIGHT, 0 );
}

void CG_DrawSmallString( int x, int y, const char *s, float alpha ) {
	float	color[4];

	color[0] = color[1] = color[2] = 1.0;
	color[3] = alpha;
	CG_DrawStringExt( x, y, s, color, qfalse, qfalse, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 0 );
}

void CG_DrawSmallStringColor( int x, int y, const char *s, vec4_t color ) {
	CG_DrawStringExt( x, y, s, color, qtrue, qfalse, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 0 );
}

/*
=================
CG_DrawStrlen

Returns character count, skiping color escape codes
=================
*/
int CG_DrawStrlen( const char *str ) {
	const char *s = str;
	int count = 0;

	while ( *s ) {
		if ( Q_IsColorString( s ) ) {
			s += 2;
		} else {
			count++;
			s++;
		}
	}

	return count;
}

/*
=============
CG_TileClearBox

This repeats a 64*64 tile graphic to fill the screen around a sized down
refresh window.
=============
*/
static void CG_TileClearBox( int x, int y, int w, int h, qhandle_t hShader ) {
	float	s1, t1, s2, t2;

	s1 = x / 64;
	t1 = y / 64;
	s2 = (x+w) / 64;
	t2 = (y+h) / 64;
	trap_R_DrawStretchPic( x, y, w, h, s1, t1, s2, t2, hShader );
}



/*
==============
CG_TileClear

Clear around a sized down screen
==============
*/
void CG_TileClear( void ) {
	int		top, bottom, left, right;
	int		w, h;

	w = cgs.glconfig.vidWidth;
	h = cgs.glconfig.vidHeight;

	if ( cg.refdef.x == 0 && cg.refdef.y == 0 && 
		cg.refdef.width == w && cg.refdef.height == h ) {
		return;		// full screen rendering
	}

	top = cg.refdef.y;
	bottom = top + cg.refdef.height-1;
	left = cg.refdef.x;
	right = left + cg.refdef.width-1;

	// clear above view screen
	CG_TileClearBox( 0, 0, w, top, cgs.media.backTileShader );

	// clear below view screen
	CG_TileClearBox( 0, bottom, w, h - bottom, cgs.media.backTileShader );

	// clear left of view screen
	CG_TileClearBox( 0, top, left, bottom - top + 1, cgs.media.backTileShader );

	// clear right of view screen
	CG_TileClearBox( right, top, w - right, bottom - top + 1, cgs.media.backTileShader );
}



/*
================
CG_FadeColor
================
*/
float *CG_FadeColor( int startMsec, int totalMsec ) {
	static vec4_t		color;
	int			t;

	if ( startMsec == 0 ) {
		return NULL;
	}

	t = cg.time - startMsec;

	if ( t >= totalMsec ) {
		return NULL;
	}

	// fade out
	if ( totalMsec - t < FADE_TIME ) {
		color[3] = ( totalMsec - t ) * 1.0/FADE_TIME;
	} else {
		color[3] = 1.0;
	}
	color[0] = color[1] = color[2] = 1;

	return color;
}


/*
================
CG_TeamColor
================
*/
float *CG_TeamColor( int team ) {
	static vec4_t	red = {1, 0.2, 0.2, 1};
	static vec4_t	blue = {0.2, 0.2, 1, 1};
	static vec4_t	other = {1, 1, 1, 1};
	static vec4_t	spectator = {0.7, 0.7, 0.7, 1};

	switch ( team ) {
	case TEAM_RED:
		return red;
	case TEAM_BLUE:
		return blue;
	case TEAM_SPECTATOR:
		return spectator;
	default:
		return other;
	}
}



/*
=================
CG_GetColorForHealth
=================
*/
void CG_GetColorForHealth( int health, int armor, vec4_t hcolor ) {
	int		count;
	int		max;

	// calculate the total points of damage that can
	// be sustained at the current health / armor level
	if ( health <= 0 ) {
		VectorClear( hcolor );	// black
		hcolor[3] = 1;
		return;
	}
	count = armor;
	// kef -- FIXME ...not sure what's happening here, but a divide-by-zero would be bad
	max = health;// * ARMOR_PROTECTION / ( 1.0 - ARMOR_PROTECTION );
	if ( max < count ) {
		count = max;
	}
	health += count;

	// set the color based on health
	hcolor[0] = 1.0;
	hcolor[3] = 1.0;
	if ( health >= 100 ) {
		hcolor[2] = 1.0;
	} else if ( health < 66 ) {
		hcolor[2] = 0;
	} else {
		hcolor[2] = ( health - 66 ) / 33.0;
	}

	if ( health > 60 ) {
		hcolor[1] = 1.0;
	} else if ( health < 30 ) {
		hcolor[1] = 0;
	} else {
		hcolor[1] = ( health - 30 ) / 30.0;
	}
}

/*
=================
CG_ColorForHealth
=================
*/
void CG_ColorForHealth( vec4_t hcolor ) {

	CG_GetColorForHealth( cg.snap->ps.stats[STAT_HEALTH], 
		cg.snap->ps.stats[STAT_ARMOR], hcolor );
}


static int	propMapTiny[CHARMAX][3];
static int	propMap[CHARMAX][3];
static int	propMapBig[CHARMAX][3];

static int propMapB[26][3] = {
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

#define PROPB_GAP_WIDTH		4
#define PROPB_SPACE_WIDTH	12
#define PROPB_HEIGHT		36

#define TEXTSHEET_SIZE		512 //TiM
#define ORIG_SCALE			TEXTSHEET_SIZE / 256

/*
=================
UI_DrawBannerString
=================
*/
static void UI_DrawBannerString2( int x, int y, const char* str, vec4_t color )
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

	// draw the colored text
	trap_R_SetColor( color );
	
//	ax = x * cgs.screenXScale + cgs.screenXBias;
	ax = x * cgs.screenXScale;
	ay = y * cgs.screenYScale;

	s = str;
	while ( *s )
	{
		ch = *s & 255;
		if ( ch == ' ' ) {
			ax += ((float)PROPB_SPACE_WIDTH + (float)PROPB_GAP_WIDTH)* cgs.screenXScale;
		}
		else if ( ch >= 'A' && ch <= 'Z' ) {
			ch -= 'A';
			fcol = (float)propMapB[(int)ch][0] / 256;
			frow = (float)propMapB[(int)ch][1] / 256;
			fwidth = (float)propMapB[(int)ch][2] / 256;
			fheight = (float)PROPB_HEIGHT / 256;
			aw = (float)(propMapB[(int)ch][2] * cgs.screenXScale);
			ah = (float)(PROPB_HEIGHT * cgs.screenYScale);
			trap_R_DrawStretchPic( ax, ay, aw, ah, fcol, frow, fcol+fwidth, frow+fheight, cgs.media.charsetPropB );
			ax += (aw + (float)PROPB_GAP_WIDTH * cgs.screenXScale);
		}
		s++;
	}

	trap_R_SetColor( NULL );
}

void UI_DrawBannerString( int x, int y, const char* str, int style, vec4_t color ) {
	const char *	s;
	int				ch;
	int				width;
	vec4_t			drawcolor;

	// find the width of the drawn text
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

#define MAX_STRINGWIDTH 2048

/*
=================
UI_ProportionalSizeScale
=================
*/
int UI_ProportionalStringWidth( const char* str,int style )
{
	const char *	s;
	int				ch;
	int				charWidth;
	int				width;
	char			holdStr[2048];

	Q_strncpyz( holdStr, str, MAX_STRINGWIDTH );

	Q_CleanStr(holdStr);

	if (style & UI_TINYFONT)
	{
		s = holdStr;
		width = 0;
		while ( *s ) 
		{
			ch = *s & 255;
			charWidth = propMapTiny[ch][2];
			if ( charWidth != -1 ) 
			{
				width += charWidth;
				width += PROP_GAP_TINY_WIDTH;
			}
			s++;
		}

		width -= PROP_GAP_TINY_WIDTH;
	}
	else if (style & UI_BIGFONT)
	{
		s = holdStr;
		width = 0;
		while ( *s ) 
		{
			ch = *s & 255;
			charWidth = propMapBig[ch][2];
			if ( charWidth != -1 ) 
			{
				width += charWidth;
				width += PROP_GAP_BIG_WIDTH;
			}
			s++;
		}

		width -= PROP_GAP_BIG_WIDTH;
	}
	else
	{
		s = holdStr;
		width = 0;
		while ( *s ) 
		{
			ch = *s & 255;
			charWidth = propMap[ch][2];
			if ( charWidth != -1 ) 
			{
				width += charWidth;
				width += PROP_GAP_WIDTH;
			}
			s++;
		}

		width -= PROP_GAP_WIDTH;
	}

	return width;
}

static int specialTinyPropChars[CHARMAX][2] = {
{0, 0},
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 10
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 20
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 30
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 40
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 50
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 60
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 70
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 80
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 90
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 100
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 110
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 120
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 130
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 140
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 150
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{2,-3},{0, 0},	// 160
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 170
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 180
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 190
{0,-1},{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{0, 0},{2, 0},{2,-3},	// 200
{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{0,-1},{2,-3},{2,-3},	// 210
{2,-3},{3,-3},{2,-3},{2,-3},{0, 0},{0,-1},{2,-3},{2,-3},{2,-3},{2,-3},	// 220
{2,-3},{0,-1},{0,-1},{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{0, 0},	// 230
{2, 0},{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{0, 0},	// 240
{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{2,-3},{0, 0},{0,-1},{2,-3},{2,-3},	// 250
{2,-3},{2,-3},{2,-3},{0,-1},{2,-3}										// 255
}; 


static int specialPropChars[CHARMAX][2] = {
{0, 0},
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 10
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 20
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 30
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 40
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 50
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 60
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 70
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 80
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 90
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 100
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 110
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 120
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 130
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 140
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 150
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 160
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 170
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 180
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 190
{2,-2},{2,-2},{2,-2},{2,-2},{2,-2},{2,-2},{2,-2},{0, 0},{1, 1},{2,-2},	// 200
{2,-2},{2,-2},{2,-2},{2,-2},{2,-2},{2,-2},{2,-2},{0, 0},{2,-2},{2,-2},	// 210
{2,-2},{2,-2},{2,-2},{2,-2},{0, 0},{0, 0},{2,-2},{2,-2},{2,-2},{2,-2},	// 220
{2,-2},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 230
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 240
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 250
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0}										// 255
}; 


static int specialBigPropChars[CHARMAX][2] = {
{0, 0},
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 10
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 20
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 30
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 40
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 50
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 60
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 70
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 80
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 90
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 100
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 110
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 120
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 130
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 140
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 150
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 160
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 170
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 180
{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},	// 190
{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{0, 0},{3, 1},{3,-3},	// 200
{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{0, 0},{3,-3},{3,-3},	// 210
{3,-3},{3,-3},{3,-3},{3,-3},{0, 0},{0, 0},{3,-3},{3,-3},{3,-3},{3,-3},	// 220
{3,-3},{0, 0},{0, 0},{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{0, 0},	// 230
{3, 1},{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{0, 0},	// 240
{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{3,-3},{0, 0},{0, 0},{3,-3},{3,-3},	// 250
{3,-3},{3,-3},{3,-3},{0, 0},{3,-3}										// 255
}; 

/*
=================
UI_AdjustForWideScreen
=================
*/
static void UI_AdjustForWidescreen( float *x, float *w )
{
	if ( CG_IsWidescreen() )
		*w *= cgs.widescreen.ratio;
}

/*
=================
UI_DrawProportionalString2
=================
*/
static void UI_DrawProportionalString2( int x, int y, const char* str, vec4_t color, float sizeScale,int style, qhandle_t charset,qboolean forceColor )
{
	const char* s;
	unsigned char	ch;
	float	ax;
	float	ay,holdY;
	float	aw = 0;	// stop compiler complaining about uninitialised vars
	float	ah;
	float	frow;
	float	fcol;
	float	fwidth;
	float	fheight;
	vec4_t	givenColor;
	int		colorI;
	int		special;

	givenColor[0] = color[0];
	givenColor[1] = color[1];
	givenColor[2] = color[2];
	givenColor[3] = color[3];

	// draw the colored text
	trap_R_SetColor( color );
	
//	ax = x * cgs.screenXScale + cgs.screenXBias;
	ax = x * cgs.screenXScale;
	ay = y * cgs.screenYScale;
	holdY = ay;

	if ( CG_IsWidescreen() )
	{
		ax *= cgs.widescreen.ratio;

		if ( cgs.widescreen.state == WIDESCREEN_CENTER )
			ax += cgs.widescreen.bias;
		else if ( cgs.widescreen.state == WIDESCREEN_RIGHT )
			ax += (cgs.widescreen.bias*2);
	}

	if (style & UI_TINYFONT)
	{
		s = str;
		while ( *s )
		{
			// Is there a color character
			if ( Q_IsColorString( s ) ) 
			{
				if (!forceColor)
				{
					colorI = ColorIndex( *(s+1) );
					trap_R_SetColor( g_color_table[colorI] );
				}
				s += 2;
				continue;
			}

			//CG_Printf("s = %s, ch = %s", s, ch);

			ch = *s & 255;
			if ( ch == ' ' ) 
			{
				aw = (float)PROP_SPACE_TINY_WIDTH;
			}
			else if ( propMapTiny[ch][2] != -1 ) 
			{
				// Because some foreign characters were a little different
				special = specialTinyPropChars[ch][0];
				ay = holdY + (specialTinyPropChars[ch][1] * cgs.screenYScale);

				fcol = (float ) propMapTiny[ch][0] / 256;
				frow = (float)propMapTiny[ch][1] / 256;
				fwidth = (float)propMapTiny[ch][2] / 256;
				fheight = (float)(PROP_TINY_HEIGHT + special) / 256;
				aw = (float)propMapTiny[ch][2] * cgs.screenXScale * sizeScale;
				ah = (float)(PROP_TINY_HEIGHT + special) * cgs.screenYScale * sizeScale;

				//TiM - adjust for widescreen
				UI_AdjustForWidescreen( &ax, &aw );

				trap_R_DrawStretchPic( ax, ay, aw, ah, fcol, frow, fcol + fwidth, frow + fheight, charset );

			}

			ax += (aw + (float)PROP_GAP_TINY_WIDTH * cgs.screenXScale * sizeScale);

			if ( CG_IsWidescreen() )
				ax -= ((float)PROP_GAP_TINY_WIDTH * cgs.screenXScale * sizeScale)*(1.0f-cgs.widescreen.ratio);

			s++;
		}
	}
	else if (style & UI_BIGFONT)
	{
		s = str;
		while ( *s )
		{
			// Is there a color character
			if ( Q_IsColorString( s ) ) 
			{
				if (!forceColor)
				{
					colorI = ColorIndex( *(s+1) );
					trap_R_SetColor( g_color_table[colorI] );
				}
				s += 2;
				continue;
			}

			ch = *s & 255;
			if ( ch == ' ' ) 
			{
				aw = (float)PROP_SPACE_BIG_WIDTH* cgs.screenXScale * sizeScale;
			}
			else if ( propMap[ch][2] != -1 ) 
			{
				// Because some foreign characters were a little different
				special = specialBigPropChars[ch][0];
				ay = holdY + (specialBigPropChars[ch][1] * cgs.screenYScale);

				fcol = (float ) propMapBig[ch][0] / 256; //256.0f
				frow = (float)propMapBig[ch][1] / 256;
				fwidth = (float)propMapBig[ch][2] / 256;
				fheight = (float)(PROP_BIG_HEIGHT+ special) / 256;
				aw = (float)propMapBig[ch][2] * cgs.screenXScale * sizeScale;
				ah = (float)(PROP_BIG_HEIGHT+ special) * cgs.screenYScale * sizeScale;

				//TiM - adjust for widescreen
				UI_AdjustForWidescreen( &ax, &aw );

				trap_R_DrawStretchPic( ax, ay, aw, ah, fcol, frow, fcol + fwidth, frow + fheight, 
					charset );

			}
			else
			{
				aw = 0;
			}

			ax += (aw + (float)PROP_GAP_BIG_WIDTH * cgs.screenXScale * sizeScale);

			if ( CG_IsWidescreen() )
				ax -= ((float)PROP_GAP_BIG_WIDTH * cgs.screenXScale * sizeScale)*(1.0f-cgs.widescreen.ratio);
				

			s++;
		}
	} 
	else
	{
		s = str;
		while ( *s )
		{
			// Is there a color character
			if ( Q_IsColorString( s ) ) 
			{
				if (!forceColor)
				{
					colorI = ColorIndex( *(s+1) );
					trap_R_SetColor( g_color_table[colorI] );
				}
				s += 2;
				continue;
			}

			ch = *s & 255;
			if ( ch == ' ' ) 
			{
				aw = (float)PROP_SPACE_WIDTH * cgs.screenXScale * sizeScale;
			} 
			else if ( propMap[ch][2] != -1 ) 
			{
				// Because some foreign characters were a little different
				special = specialPropChars[ch][0];
				ay = holdY + (specialPropChars[ch][1] * cgs.screenYScale);

				fcol = (float)propMap[ch][0] / 256;
				frow = (float)propMap[ch][1] / 256;
				fwidth = (float)propMap[ch][2] / 256;
				fheight = (float)(PROP_HEIGHT+ special) / 256;
				aw = (float)(propMap[ch][2] * cgs.screenXScale * sizeScale);
				ah = (float)((PROP_HEIGHT+ special) * cgs.screenYScale * sizeScale);

				//TiM - adjust for widescreen
				UI_AdjustForWidescreen( &ax, &aw );

				trap_R_DrawStretchPic( ax, ay, aw, ah, fcol, frow, fcol+fwidth, frow+fheight, charset );
			} 
			else 
			{
				aw = 0;
			}

			ax += (aw + (float)PROP_GAP_WIDTH * cgs.screenXScale * sizeScale);

			if ( CG_IsWidescreen() )
				ax -= ((float)PROP_GAP_WIDTH * cgs.screenXScale * sizeScale)*(1.0f-cgs.widescreen.ratio);

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
float UI_ProportionalSizeScale( int style ) 
{
	if(  style & UI_SMALLFONT ) 
	{
		return 1;
	}

	return 1.00;
}


/*
=================
UI_DrawProportionalString
=================
*/
void UI_DrawProportionalString( int x, int y, const char* str, int style, vec4_t color ) 
{
	vec4_t		drawcolor;
	int			width;
	float		sizeScale;
	qhandle_t	charset;

	sizeScale = UI_ProportionalSizeScale( style );

	switch( style & UI_FORMATMASK ) 
	{
		case UI_CENTER:
			width = UI_ProportionalStringWidth( str, style ) * sizeScale;
			x -= width / 2;
			break;

		case UI_RIGHT:
			width = UI_ProportionalStringWidth( str,style ) * sizeScale;
			x -= width;
			break;

		case UI_LEFT:
		default:
			break;
	}

	//TiM - vertical align
	if ( style & UI_BOTTOM )
	{
		if (style & UI_TINYFONT)
			y -= (PROP_TINY_HEIGHT+3);
		else if (style & UI_BIGFONT)
			y -= (PROP_BIG_HEIGHT+3);
		else
			y -= (PROP_HEIGHT+3);
	}

	if (style & UI_TINYFONT)
	{
		charset =  cgs.media.charsetPropTiny;
	}
	else if (style & UI_BIGFONT)
	{
		charset = cgs.media.charsetPropBig;
		sizeScale = 1;
	}
	else
	{
		charset = cgs.media.charsetProp;
		sizeScale = 1;
	}

	if ( style & UI_DROPSHADOW ) 
	{
		drawcolor[0] = drawcolor[1] = drawcolor[2] = 0;
		drawcolor[3] = color[3];

		UI_DrawProportionalString2( x+2, y+2, str, drawcolor, sizeScale,style, charset,qtrue );
	}

	if ( style & UI_INVERSE ) 
	{
		drawcolor[0] = color[0] * 0.8;
		drawcolor[1] = color[1] * 0.8;
		drawcolor[2] = color[2] * 0.8;
		drawcolor[3] = color[3];
		UI_DrawProportionalString2( x, y, str, drawcolor, sizeScale,style, charset,qtrue );
		return;
	}

	if ( style & UI_PULSE ) 
	{
		drawcolor[0] = color[0] * 0.8;
		drawcolor[1] = color[1] * 0.8;
		drawcolor[2] = color[2] * 0.8;
		drawcolor[3] = color[3];
		UI_DrawProportionalString2( x, y, str, color, sizeScale,style, charset,qtrue );

		drawcolor[0] = color[0];
		drawcolor[1] = color[1];
		drawcolor[2] = color[2];
		drawcolor[3] = 0.5 + 0.5 * sin( cg.time / PULSE_DIVISOR );
		UI_DrawProportionalString2( x, y, str, drawcolor, sizeScale,style,charset,qtrue );
		return;
	}

	if ( style & UI_FORCECOLOR ) 
	{	
		UI_DrawProportionalString2( x, y, str, color, sizeScale,style, charset,qtrue);
		return;
	}

	UI_DrawProportionalString2( x, y, str, color, sizeScale,style, charset,qfalse);
}

/*
==============
CG_DrawNumField

Take x,y positions as if 640 x 480 and scales them to the proper resolution

==============
*/
void CG_DrawNumField (int x, int y, int width, int value,int charWidth,int charHeight,int style) 
{
	char	num[16], *ptr;
	int		l;
	int		frame;
	int		xWidth;

	if (width < 1) {
		return;
	}

	// draw number string
	if (width > 5) {
		width = 5;
	}

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

	if (style & NUM_FONT_SMALL)
	{
		xWidth = 4 + 2;
	}
	else
	{
		xWidth = (charWidth/2) + 3;//(charWidth/6);
	}

	x += 2 + (xWidth)*(width - l);

	ptr = num;
	while (*ptr && l)
	{
		if (*ptr == '-')
			frame = STAT_MINUS;
		else
			frame = *ptr -'0';

		if (style & NUM_FONT_SMALL)
		{
			CG_DrawPic( x,y, charWidth, charHeight, cgs.media.smallnumberShaders[frame] );
		}
		else
		{
			CG_DrawPic( x,y, charWidth, charHeight, cgs.media.numberShaders[frame] );
		}

		x += (xWidth);
		ptr++;
		l--;
	}

}

/*
================
CG_PrintInterfaceGraphics
================
*/
void CG_PrintInterfaceGraphics(int min,int max)
{
	int i;
	vec4_t drawcolor;

	// Printing graphics
	for (i=min;i<max;++i)
	{
		drawcolor[0] = colorTable[interface_graphics[i].color][0];
		drawcolor[1] = colorTable[interface_graphics[i].color][1];
		drawcolor[2] = colorTable[interface_graphics[i].color][2];
		drawcolor[3] = colorTable[interface_graphics[i].color][3];
		
		if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED ) 
		{
			drawcolor[0] = 1;
		} 
		else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE ) 
		{
			drawcolor[2] = 1;
		} 

		if (interface_graphics[i].type == SG_GRAPHIC)
		{
			trap_R_SetColor(drawcolor);

			CG_DrawPic( interface_graphics[i].x, 
			interface_graphics[i].y,	
			interface_graphics[i].width, 
			interface_graphics[i].height,	
			interface_graphics[i].graphic);
		}
		else if (interface_graphics[i].type == SG_NUMBER)
		{
			if (interface_graphics[i].style & UI_PULSE)
			{
				drawcolor[3] = 0.5+0.5*sin(cg.time/PULSE_DIVISOR);
			}
			trap_R_SetColor( drawcolor);

			CG_DrawNumField (
				interface_graphics[i].x, 
				interface_graphics[i].y, 3, 
				interface_graphics[i].max, 
				interface_graphics[i].width, 
				interface_graphics[i].height,interface_graphics[i].style);
		}
	}

	trap_R_SetColor(NULL);
}

/*
=================
CG_ParseFontParms
=================
*/
static char *CG_ParseFontParms(char *buffer,int	propArray[CHARMAX][3])
{
	char	*token;
	int		i,i2;
	
	while ( buffer ) 
	{
		token = COM_ParseExt( &buffer, qtrue );

		// Start with open braket
		if ( !Q_stricmp( token, "{" ) ) 
		{
			for (i=0;i<CHARMAX;++i)
			{
				// Brackets for the numbers
				token = COM_ParseExt( &buffer, qtrue );
				if ( !Q_stricmpn( token, "{", 1 ) ) 
				{
					;
				}
				else
				{
					trap_Print( va( S_COLOR_RED "CG_ParseFontParms : Invalid FONTS.DAT data, near character %d!\n",i));
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
					trap_Print( va( S_COLOR_RED "CG_ParseFontParms : Invalid FONTS.DAT data, near character %d!\n",i));
					return(NULL);
				}
			}
		}

		token = COM_ParseExt( &buffer, qtrue );	// Grab closing bracket
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
CG_LoadFonts
=================
*/
void CG_LoadFonts(void)
{
	char buffer[FONT_BUFF_LENGTH];
	int len;
	fileHandle_t	f;
	char *holdBuf;

	len = trap_FS_FOpenFile( "ext_data/fonts.dat", &f, FS_READ );

	if ( !f ) 
	{
		trap_Print( va( S_COLOR_RED "CG_LoadFonts : FONTS.DAT file not found!\n"));
		return;
	}

	if (len > FONT_BUFF_LENGTH)
	{
		trap_Print( va( S_COLOR_RED "CG_LoadFonts : FONTS.DAT file bigger than %d!\n",FONT_BUFF_LENGTH));
		return;
	}

	// initialise the data area
	memset(buffer, 0, sizeof(buffer));	

	trap_FS_Read( buffer, len, f );

	trap_FS_FCloseFile( f );

	COM_BeginParseSession();

	holdBuf = (char *) buffer;
	holdBuf = CG_ParseFontParms( holdBuf,propMapTiny);
	holdBuf = CG_ParseFontParms( holdBuf,propMap);
	holdBuf = CG_ParseFontParms( holdBuf,propMapBig);

}

/*
=================
CG_ParseRankData

Takes '{ x, y, w, h, file_route } and parses it.
=================
*/

#define MAX_RANK_WIDTH 67
#define MAX_RANK_HEIGHT 15

qboolean CG_ParseRankData( char **data, rankMenuData_t* rankMenuData ) {
	const char* token;

	token = COM_ParseExt( data, qtrue );
	if ( token[0] == 0 ) {
		Com_Printf( S_COLOR_RED "Unexpected EOF! O_o\n" );
		return qtrue;
	}

	if ( !Q_stricmp( token, "{" ) ) {
		//parse w value
		token = COM_ParseExt( data, qtrue );
		if ( token[0] == 0 ) {
			Com_Printf( S_COLOR_RED "Unexpected EOF! O_o\n" );
			return qtrue;
		}
		rankMenuData->w = atoi( token );

		if ( rankMenuData->w > MAX_RANK_WIDTH ) {
			rankMenuData->w = MAX_RANK_WIDTH;
		}

		//parse h value
		token = COM_ParseExt( data, qtrue );
		if ( token[0] == 0 ) {
			Com_Printf( S_COLOR_RED "Unexpected EOF! O_o\n" );
			return qtrue;
		}
		rankMenuData->h = atoi( token );

		if ( rankMenuData->h > MAX_RANK_HEIGHT ) {
			rankMenuData->h = MAX_RANK_HEIGHT;
		}

		//parse s1 value
		token = COM_ParseExt( data, qtrue );
		if ( token[0] == 0 ) {
			Com_Printf( S_COLOR_RED "Unexpected EOF! O_o\n" );
			return qtrue;
		}
		rankMenuData->s1 = atoi( token );

		if ( !rankMenuData->s1 ) {
			Com_Printf( S_COLOR_RED "Invalid s1 value in rankset\n");
			return qtrue;
		}

		//parse t1 value
		token = COM_ParseExt( data, qtrue );
		if ( token[0] == 0 ) {
			Com_Printf( S_COLOR_RED "Unexpected EOF! O_o\n" );
			return qtrue;
		}
		rankMenuData->t1 = atoi( token );

		if ( !rankMenuData->t1 ) {
			Com_Printf( S_COLOR_RED "Invalid t1 value in rankset\n");
			return qtrue;
		}

		//parse s2 value
		token = COM_ParseExt( data, qtrue );
		if ( token[0] == 0 ) {
			Com_Printf( S_COLOR_RED "Unexpected EOF! O_o\n" );
			return qtrue;
		}
		rankMenuData->s2 = atoi( token );

		if ( !rankMenuData->s2 ) {
			Com_Printf( S_COLOR_RED "Invalid s2 value in rankset\n");
			return qtrue;
		}

		//parse t2 value
		token = COM_ParseExt( data, qtrue );
		if ( token[0] == 0 ) {
			Com_Printf( S_COLOR_RED "Unexpected EOF! O_o\n" );
			return qtrue;
		}
		rankMenuData->t2 = atoi( token );

		if ( !rankMenuData->t2 ) {
			Com_Printf( S_COLOR_RED "Invalid t2 value in rankset\n");
			return qtrue;
		}

		//parse gWidth value
		token = COM_ParseExt( data, qtrue );
		if ( token[0] == 0 ) {
			Com_Printf( S_COLOR_RED "Unexpected EOF! O_o\n" );
			return qtrue;
		}
		rankMenuData->gWidth = atoi( token );

		if ( !rankMenuData->gWidth ) {
			Com_Printf( S_COLOR_RED "Invalid graphic width value in rankset\n");
			return qtrue;
		}

		//parse gHeight value
		token = COM_ParseExt( data, qtrue );
		if ( token[0] == 0 ) {
			Com_Printf( S_COLOR_RED "Unexpected EOF! O_o\n" );
			return qtrue;
		}
		rankMenuData->gHeight = atoi( token );

		if ( !rankMenuData->gHeight ) {
			Com_Printf( S_COLOR_RED "Invalid graphic height value in rankset\n");
			return qtrue;
		}

		//parse graphics handle
		token = COM_ParseExt( data, qtrue );
		if ( token[0] == 0 ) {
			Com_Printf( S_COLOR_RED "Unexpected EOF! O_o\n" );
			return qtrue;
		}
		rankMenuData->graphic = trap_R_RegisterShaderNoMip( token );

		if ( !rankMenuData->graphic ) {
			Com_Printf( S_COLOR_RED "Unable to find shader in rankset: %s\n", token );
			return qtrue;
		}

		//error checking - we should see a '}' now :P
		token = COM_ParseExt( data, qtrue );
		if ( token[0] == 0 ) {
			Com_Printf( S_COLOR_RED "Unexpected EOF! O_o\n" );
			return qtrue;
		}

		//all good :) return the function good
		if ( !Q_stricmp( token, "}" ) ) {
			return qfalse;
		}
		else {
			return qtrue;
		}
	}
	else {
		Com_Printf( S_COLOR_RED, "Could not find { in current rankset file.\n" );
		return qtrue;
	}
}

/*
=================
CG_LoadRanks
By TiM
=================
*/
qboolean CG_LoadRanks( void ) {
	fileHandle_t	file;
	int				file_len;
	char			charText[32000];
	char			*textPtr, *prevValue;
	char			fileName[MAX_QPATH];
	int				i;
	int				rankCount=0;
	char			*token;

	qboolean		DefaultRankLoaded = qfalse;

	if ( cgs.rankSet ) {
		Com_sprintf( fileName, sizeof( fileName ), "ext_data/ranksets/%s.ranks", cgs.rankSet );
	}
	else {
		return qfalse;
	}

	file_len = trap_FS_FOpenFile( fileName, &file, FS_READ );

	if ( file_len <= 0 ) {
		return qfalse;
	}

	if ( file_len >= sizeof( charText ) - 1 ) {
		Com_Printf( S_COLOR_RED "Size of rankset file %s is too large.\n", fileName );
		return qfalse;
	}

	memset( &charText, 0, sizeof( charText ) );
	memset( &cgs.defaultRankData, 0, sizeof( cgs.defaultRankData ) );
	memset( &cgs.ranksData, 0, sizeof( cgs.ranksData ) );

	trap_FS_Read( charText, file_len, file );

	charText[file_len] = 0;

	trap_FS_FCloseFile( file );

	COM_BeginParseSession();

	textPtr = charText;

	token = COM_Parse( &textPtr );

	if ( !token[0] ) {
		Com_Printf( S_COLOR_RED "No data found in rankset: %s\n", fileName );
		return qfalse;
	}

	if ( Q_stricmp( token, "{" ) ) {
		Com_Printf( S_COLOR_RED "Missing starting { in rankset file: %s\n", fileName);
		return qfalse;
	}

	//Parse the first one only... the first one should be the DEFAULT RANK!!!
	//DEFAULT I SAY!  IT'S SPECIAL!
	//WE NEED A DEFAULT FOR ERRORS! NO DEFAULT NO RANK FOR YOU!
	while ( 1 ) {
		prevValue = textPtr;

		token = COM_Parse( &textPtr );
		if ( !token[0] ) {
			break;
		}

		if ( !Q_stricmpn( token, "MenuTextureDef", 14 ) ) {
			if ( CG_ParseRankData( &textPtr, &cgs.defaultRankData.rankMenuData ) ) {
				continue;
			}

			DefaultRankLoaded = qtrue;
			continue;
		}
		else if ( !Q_stricmpn( token, "BoltModel", 9 ) ) {
			if ( COM_ParseString( &textPtr, &token ) ) {
				continue;
			}

			Q_strncpyz( cgs.defaultRankData.rankModelData.boltModelPath, token,
						sizeof( cgs.defaultRankData.rankModelData.boltModelPath ) );

			/*if ( !cgs.defaultRankData.boltModel ) {
				Com_Printf(S_COLOR_RED "Unable to load model file: %s\n", token );
			}*/

			continue;

		}
		else if ( !Q_stricmpn( token, "boltShader", 8 ) ) {
			if ( COM_ParseString( &textPtr, &token ) ) {
				continue;
			}

			Q_strncpyz( cgs.defaultRankData.rankModelData.boltShaderPath, token,
						sizeof( cgs.defaultRankData.rankModelData.boltShaderPath ) );

			/*cgs.defaultRankData.boltShader = trap_R_RegisterSkin( token );
			if ( !cgs.defaultRankData.boltShader ) {
				Com_Printf(S_COLOR_RED "Unable to load skin file: %s\n", token );
			}*/

			continue;
		}
		else if ( !Q_stricmpn( token, "AdmiralRank", 11 ) ) {
			if ( COM_ParseInt( &textPtr, &i ) ) {
				continue;
			}

			cgs.defaultRankData.rankModelData.admiralRank = i;
			continue;
		}
		else if ( !Q_stricmp( token, "}" ) ) {
			break;
		}

	}

	//if we succeeded in loading a default value (^_^!), then begin parsing the main ranks
	if ( !DefaultRankLoaded ) {
		Com_Printf( S_COLOR_RED "No default rank located in %s\n", fileName );
		return qfalse;
	}
	else {
		//Parse.Rank.Data? :) *sigh* if only rofl 
		while (1) {
			prevValue = textPtr;
			token = COM_Parse( &textPtr );
			if ( !token[0] ) {
				break;
			}

			if ( rankCount >= MAX_RANKS ) {
				break;
			}

			if ( !Q_stricmpn( token, "{", 1 ) ) {
	
				while ( 1 ) {
					token = COM_Parse( &textPtr );
					if ( !token[0] ) {
						break;
					}

					if ( !Q_stricmpn( token, "ConsoleName", 11 ) ) {
						if ( COM_ParseString( &textPtr, &token ) ) {
							Com_Printf( S_COLOR_RED "Could not parse console rank name for rank number: %i\n", rankCount );
							return qfalse;
						}

						Q_strncpyz( cgs.ranksData[rankCount].consoleName, token, sizeof( cgs.ranksData[rankCount].consoleName ) );
						continue;
					}					

					if ( !Q_stricmpn( token, "FormalName", 10 ) ) {
						if ( COM_ParseString( &textPtr, &token ) ) {
							Com_Printf( S_COLOR_RED "Could not parse formal rank name for rank number: %i\n", rankCount );
							return qfalse;
						}

						Q_strncpyz( cgs.ranksData[rankCount].formalName, token, sizeof( cgs.ranksData[rankCount].formalName ) );
						continue;
					}

					if ( !Q_stricmpn( token, "MenuTextureRed", 14 ) ) {
						if ( CG_ParseRankData( &textPtr, &cgs.ranksData[rankCount].rankMenuData[CLR_RED] ) ) {
							Com_Printf( S_COLOR_RED "Could not parse red rank color for rank number: %i\n", rankCount );
							return qfalse;
						}

						continue;
					}
					else if ( !Q_stricmpn( token, "MenuTextureTeal", 15 ) ) {
						if ( CG_ParseRankData( &textPtr, &cgs.ranksData[rankCount].rankMenuData[CLR_TEAL] ) ) {
							Com_Printf( S_COLOR_RED "Could not parse teal rank color for rank number: %i\n", rankCount );
							return qfalse;
						}

						continue;						
					}
					else if ( !Q_stricmpn( token, "MenuTextureGold", 15 ) ) {
						if ( CG_ParseRankData( &textPtr, &cgs.ranksData[rankCount].rankMenuData[CLR_GOLD] ) ) {
							Com_Printf( S_COLOR_RED "Could not parse teal rank color for rank number: %i\n", rankCount );
							return qfalse;
						}

						continue;							
					}
					else if ( !Q_stricmpn( token, "MenuTextureGreen", 16 ) ) {
						if ( CG_ParseRankData( &textPtr, &cgs.ranksData[rankCount].rankMenuData[CLR_GREEN] ) ) {
							Com_Printf( S_COLOR_RED "Could not parse green rank color for rank number: %i\n", rankCount );
							return qfalse;
						}

						continue;							
					}
					else if ( !Q_stricmpn( token, "BoltModel", 9 ) ) {
						if ( COM_ParseString( &textPtr, &token ) ) {
							continue;
						}

						//cgs.ranksData[rankCount].boltModel = trap_R_RegisterModel( token );
						
						Q_strncpyz( cgs.ranksData[rankCount].rankModelData.boltModelPath,
									token,
									sizeof( cgs.ranksData[rankCount].rankModelData.boltModelPath) );

						/*if ( !cgs.ranksData[rankCount].boltModel ) {
							Com_Printf( S_COLOR_RED, "Could not load bolt model: %s\n", token );
						}*/
						continue;
					}
					else if ( !Q_stricmpn( token, "boltShader", 8 ) ) {
						if ( COM_ParseString( &textPtr, &token ) ) {
							continue;
						}

						Q_strncpyz( cgs.ranksData[rankCount].rankModelData.boltShaderPath,
									token,
									sizeof( cgs.ranksData[rankCount].rankModelData.boltShaderPath) );

						
						/*if ( !cgs.ranksData[rankCount].boltShader ) {
							Com_Printf( S_COLOR_RED, "Could not load bolt skin: %s\n", token );
						}*/
						continue;
					}
					else if ( !Q_stricmpn( token, "AdmiralRank", 11 ) ) {
						if ( COM_ParseInt( &textPtr, &i ) ) {
							continue;
						}

						cgs.ranksData[rankCount].rankModelData.admiralRank = i;
						
						continue;
					}
					else if ( !Q_stricmpn( token, "}", 1 ) ) {
						break;
					}
				}
				rankCount++;
			}
		}
	}
	return qtrue;
}

/*
=================
CG_LoadCrosshairs
By TiM

Parsing script to load
and display multiple 
crosshairs.
=================
*/
qboolean CG_LoadCrosshairs(void) {
	fileHandle_t	f;
	int				file_len;
	char			charText[20000];
	char			*token, *textPtr;
	char			*fileName = "ext_data/crosshairs.dat";
	int				cHairCount = 0;
	//int				i;

	//load file and get file length
	file_len = trap_FS_FOpenFile( fileName, &f, FS_READ );

	memset( &charText, 0, sizeof( charText ) );
	memset( &cgs.crosshairsData, 0, sizeof( &cgs.crosshairsData ) );

	//check to see if we got anything
	if ( file_len <= 0 ) {
		Com_Printf( S_COLOR_RED "Could not find file: %s\n", fileName );
		return qfalse;
	}

	//check to see if we got too much
	if ( file_len > sizeof( charText) - 1 ) {
		Com_Printf( S_COLOR_RED "File %s waaaaay too big.\n", fileName );
		return qfalse;
	}

	//read the data into the array
	trap_FS_Read( charText, file_len, f );

	//EOF?
	charText[file_len] = 0;

	trap_FS_FCloseFile( f );

	COM_BeginParseSession();

	textPtr = charText;

	token = COM_Parse( &textPtr );

	//wtf? Nothing here??
	if ( !token[0] ) {
		Com_Printf( S_COLOR_RED "File: %s terminated rather unexpectantly\n", fileName );
		return qfalse;
	}

	if ( Q_stricmp( token, "{" ) ) {
		Com_Printf( S_COLOR_RED "File: %s had no starting {\n", fileName );
		return qfalse;
	}

	//now for the natural hard-core loop parsing bit
	while ( 1 ) {

		if ( cHairCount >= MAX_CROSSHAIRS ) {
			break;
		}

		if ( !Q_stricmpn( token, "{", 1 ) ) {
			//Make color default to white
			VectorCopy( colorTable[CT_WHITE], cgs.crosshairsData[cHairCount].color );
			cgs.crosshairsData[cHairCount].color[3] = 1.0f;

			while ( 1 ) 
			{
				token = COM_Parse( &textPtr );
				if (!token[0]) {
					break;
				}

				if ( !Q_stricmpn( token, "noDraw", 6 ) ) {
					cgs.crosshairsData[cHairCount].noDraw = qtrue;
					break;
				}
				else if ( !Q_stricmpn( token, "CrosshairColor", 14 ) ) {
					if ( COM_ParseVec4( &textPtr, cgs.crosshairsData[cHairCount].color ) ) {
						Com_Printf( S_COLOR_RED "Invalid Color Value in %s\n", fileName );
						return qfalse;
					}

					VectorScale( cgs.crosshairsData[cHairCount].color, 1.0f/255.0f, cgs.crosshairsData[cHairCount].color );
					cgs.crosshairsData[cHairCount].color[3] *= 1.0f/255.0f;

					continue;
				}
				else if ( !Q_stricmpn( token, "BitmapCoords", 12 ) ) {
					if ( COM_ParseString( &textPtr, &token ) ) {
						Com_Printf( S_COLOR_RED "No bitmap co-ords found in %s\n", fileName );
						return qfalse;
					}

					if ( Q_stricmpn( token, "{", 1 ) ) {
						Com_Printf( S_COLOR_RED "No { found in bitmap co-ords in %s\n", fileName );
						return qfalse;
					}

					//parse s1 value
					if ( COM_ParseString( &textPtr, &token ) ) {
						Com_Printf( S_COLOR_RED "Bit co-ord value ended prematurely: %s\n", fileName );
						return qfalse;
					}

					cgs.crosshairsData[cHairCount].s1 = atoi( token );

					//parse t1 value
					if ( COM_ParseString( &textPtr, &token ) ) {
						Com_Printf( S_COLOR_RED "Bit co-ord value ended prematurely: %s\n", fileName );
						return qfalse;
					}

					cgs.crosshairsData[cHairCount].t1 = atoi( token );

					//parse s2 value
					if ( COM_ParseString( &textPtr, &token ) ) {
						Com_Printf( S_COLOR_RED "Bit co-ord value ended prematurely: %s\n", fileName );
						return qfalse;
					}

					cgs.crosshairsData[cHairCount].s2 = atoi( token );

					//parse t2 value
					if ( COM_ParseString( &textPtr, &token ) ) {
						Com_Printf( S_COLOR_RED "Bit co-ord value ended prematurely: %s\n", fileName );
						return qfalse;
					}

					cgs.crosshairsData[cHairCount].t2 = atoi( token );

					//make sure we have } at the end FFS
					if ( COM_ParseString( &textPtr, &token ) ) {
						Com_Printf( S_COLOR_RED "Bit co-ord value ended prematurely: %s\n", fileName );
						return qfalse;
					}

					//This is needed or else we can't tell between this and the main closing }
					if ( Q_stricmpn( token, "}", 1 ) ) {
						Com_Printf( S_COLOR_RED "No terminating } in bitmap co-ord: %s\n", fileName );
						return qfalse;						
					}
				}
				else if ( !Q_stricmpn ( token, "}", 1 ) ) {
					break;
				}
			}
			cHairCount++;
		}

		token = COM_Parse( &textPtr );
		if (!token[0]) {
			break;
		}
	}
	return qtrue;
}
