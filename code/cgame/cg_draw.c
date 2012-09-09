/*
 * Copyright (C) 1999-2000 Id Software, Inc.
 *
 * cg_draw.c -- draw all of the graphical elements during
 * active (after loading) gameplay
 */

#include "cg_local.h"
#include "cg_text.h"
#include "cg_screenfx.h"

/* set in CG_ParseTeamInfo */
int sortedTeamPlayers[TEAM_MAXOVERLAY];
int	numSortedTeamPlayers;
int drawTeamOverlayModificationCount = -1;

/*
 * TiM: dCross
 * qboolean CG_WorldCoordToScreenCoord(vec3_t worldCoord, float *x, float *y, qboolean clamp);
 * end dCross
 */

/* TiM: Tricorder Parameters */
vec3_t	vfwd;
vec3_t	vright;
vec3_t	vup;
vec3_t	vfwd_n;
vec3_t	vright_n;
vec3_t	vup_n;
int		infoStringCount;

static qboolean drawCrosshairName=qfalse;

extern void InitPostGameMenuStruct();

static void CG_InterfaceStartup();

char *ingame_text[IGT_MAX];		/*	Holds pointers to ingame text */

int zoomFlashTime=0;

/*typedef enum {
	RADAR_UP,
	RADAR_MIDDLE,
	RADAR_DOWN
} radarType_t;*/

interfacegraphics_s interface_graphics[IG_MAX] = 
{
/*	type		timer	x		y		width	height	file/text										graphic,	min		max		color			style			ptr */
	{ SG_VAR,		0.0,	0,		0,		0,		0,		NULL,										0,		0,		0,		CT_NONE,		0 },					/* IG_GROW */

	{ SG_VAR,		0.0,	0,		0,		0,		0,		NULL,										0,		0,		0,		CT_NONE,		0 },				 	/* IG_HEALTH_START */
	{ SG_GRAPHIC,	0.0,	5,		429,	32,		64,		"gfx/interface/rpgx_healthbar_leftcorner",	0,		0,		0,		CT_DKBROWN1,	0 },				 	/* IG_HEALTH_BEGINCAP */
	{ SG_GRAPHIC,	0.0,	64,		429,	6,		25,		"gfx/interface/ammobar",					0,		0,		0,		CT_DKBROWN1,	0 },				 	/* IG_HEALTH_BOX1 */
	{ SG_GRAPHIC,	0.0,	72,		429,	0,		25,		"gfx/interface/ammobar",					0,		0,		0,		CT_LTBROWN1,	0 },				 	/* IG_HEALTH_SLIDERFULL */
	{ SG_GRAPHIC,	0.0,	0,		429,	0,		25,		"gfx/interface/ammobar",					0,		0,		0,		CT_DKBROWN1,	0 },				 	/* IG_HEALTH_SLIDEREMPTY */
	{ SG_GRAPHIC,	0.0,	72,		429,	16,		32,		"gfx/interface/rpgx_healthbar_endcap",		0,		0,		147,	CT_DKBROWN1,	0 },				 	/* IG_HEALTH_ENDCAP */
	{ SG_NUMBER,	0.0,	23,		425,	16,		32,		NULL,										0,		0,		0,		CT_LTBROWN1,	NUM_FONT_BIG },	 		/* IG_HEALTH_COUNT */
	{ SG_VAR,		0.0,	0,		0,		0,		0,		NULL,										0,		0,		0,		CT_NONE,		0 },				 	/* IG_HEALTH_END */

	{ SG_VAR,		0.0,	0,		0,		0,		0,		NULL,										0,		0,		0,		CT_NONE,		0 },				 	/* IG_ARMOR_START */
	{ SG_GRAPHIC,	0.0,	20,		458,	32,		16,		"gfx/interface/armorcap1",					0,		0,		0,		CT_DKPURPLE1,	0 },			 		/* IG_ARMOR_BEGINCAP */
	{ SG_GRAPHIC,	0.0,	64,		458,	6,		12,		"gfx/interface/ammobar",					0,		0,		0,		CT_DKPURPLE1,	0 },			 		/* IG_ARMOR_BOX1 */
	{ SG_GRAPHIC,	0.0,	72,		458,	0,		12,		"gfx/interface/ammobar",					0,		0,		0,		CT_LTPURPLE1,	0 },			 		/* IG_ARMOR_SLIDERFULL */
	{ SG_GRAPHIC,	0.0,	0,		458,	0,		12,		"gfx/interface/ammobar",					0,		0,		0,		CT_DKPURPLE1,	0 },			 		/* IG_ARMOR_SLIDEREMPTY */
	{ SG_GRAPHIC,	0.0,	72,		458,	16,		16,		"gfx/interface/armorcap2",					0,		0,		147,	CT_DKPURPLE1,	0 },			 		/* IG_ARMOR_ENDCAP */
	{ SG_NUMBER,	0.0,	44,		458,	16,		16,		NULL,										0,		0,		0,		CT_LTPURPLE1,	NUM_FONT_SMALL }, 		/* IG_ARMOR_COUNT */
	{ SG_VAR,		0.0,	0,		0,		0,		0,		NULL,										0,		0,		0,		CT_NONE,		0 },			 		/* IG_ARMOR_END */

	{ SG_VAR,		0.0,	0,		0,		0,		0,		NULL,										0,		0,		0,		CT_NONE,		0 },			 		/* IG_AMMO_START */
	{ SG_GRAPHIC,	0.0,	613,	429,	32,		64,		"gfx/interface/ammouppercap1",				0,		0,		0,		CT_LTPURPLE2,	0 },			 		/* IG_AMMO_UPPER_BEGINCAP */
	{ SG_GRAPHIC,	0.0,	607,	429,	16,		32,		"gfx/interface/ammouppercap2",				0,		0,		572,	CT_LTPURPLE2,	0 },			 		/* IG_AMMO_UPPER_ENDCAP */
	{ SG_GRAPHIC,	0.0,	613,	458,	16,		16,		"gfx/interface/ammolowercap1",				0,		0,		0,		CT_LTPURPLE2,	0 },			 		/* IG_AMMO_LOWER_BEGINCAP */
	{ SG_GRAPHIC,	0.0,	578,	458,	0,		12,		"gfx/interface/ammobar",					0,		0,		0,		CT_LTPURPLE1,	0 },			 		/* IG_AMMO_SLIDERFULL */
	{ SG_GRAPHIC,	0.0,	0,		458,	0,		12,		"gfx/interface/ammobar",					0,		0,		0,		CT_DKPURPLE1,	0 },			 		/* IG_AMMO_SLIDEREMPTY */
	{ SG_GRAPHIC,	0.0,	607,	458,	16,		16,		"gfx/interface/ammolowercap2",				0,		0,		572,	CT_LTPURPLE2,	0 },			 		/* IG_AMMO_LOWER_ENDCAP */
	{ SG_NUMBER,	0.0,	573,	425,	16,		32,		NULL,										0,		0,		0,		CT_LTPURPLE1,	NUM_FONT_BIG }, 		/* IG_AMMO_COUNT */
	{ SG_VAR,		0.0,	0,		0,		0,		0,		NULL,										0,		0,		0,		CT_NONE,		0 },			 		/* IG_AMMO_END */

};

#define LOWEROVERLAY_Y (SCREEN_HEIGHT - ICON_SIZE - 15)

/*------------------------------------------------------*/

lensFlare_t lensFlare[MAX_LENS_FLARES];

lensReflec_s lensReflec[10] = 
{
/*   width, height, offset, positive, 	color, 					shadername, 								shaders placeholder */
	{ 23,	23,		0.192,	qtrue,		{ 0.73, 0.50, 0.23 },	"gfx/effects/flares/flare_straight",		0 }, /* Brown1 5.2 */
	{ 9,	9,		0.37,	qtrue,		{ 0.37, 0.58, 0.55 },	"gfx/effects/flares/flare_straight",		0 }, /* Aqua1 2.7 */
	{ 14,	14,		0.25,	qfalse,		{ 0.37, 0.79, 0.76 },	"gfx/effects/flares/flare_radial",			0 }, /* Turquoise1 4.0 */
	{ 86,	86,		0.556,	qfalse,		{ 0.73, 0.50, 0.23 },	"gfx/effects/flares/flare_inverseradial",	0 }, /* BigBrownInverseRad 1.8 */
	{ 49,	49,		0.476,	qfalse,		{ 0.73, 0.50, 0.23 },	"gfx/effects/flares/flare_straight",		0 }, /* StraightBrown2 2.1 */
	{ 35,	35,		0.667,	qfalse,		{ 0.34,	0.40, 0.44 },	"gfx/effects/flares/flare_straight",		0 }, /* Grey1 1.5 */
	{ 32,	32,		0.769,	qfalse,		{ 0.20,	0.38, 0.62 },	"gfx/effects/flares/flare_radial",			0 }, /* BlueRad 1.3 */
	{ 122,	122,	1.1,	qfalse,		{ 0.31, 0.65, 0.36 },	"gfx/effects/flares/flare_inverseradial",	0 }, /* BigInverseGreen 0.9 */
	{ 254,	254,	1.429,	qfalse,		{ 1.00,	1.00, 1.00 },	"gfx/effects/flares/flare_chromadisc",		0 }, /* ChromaHoop 0.7 */
	{ 52,	52,		1.429,	qtrue,		{ 0.40, 0.56, 0.42 },	"gfx/effects/flares/flare_inverseradial",	0 }, /* Green offset 0.7 */
};

#define HALF_SCREEN_WIDTH (SCREEN_WIDTH*0.5)
#define HALF_SCREEN_HEIGHT (SCREEN_HEIGHT*0.5)

void CG_InitLensFlare( vec3_t worldCoord, 
						int w1, int h1,
						vec3_t glowColor, float glowOffset, float hazeOffset, int minDist, int maxDist,
						vec3_t streakColor, int streakDistMin, int streakDistMax, int streakW, int streakH,  qboolean whiteStreaks, 
						int reflecDistMin, int reflecDistMax, qboolean reflecAnamorphic, qboolean defReflecs, 
						qboolean clamp, float maxAlpha, int startTime, int upTime, int holdTime, int downTime ) 
{
	int i;

	/* First thing's first.... I understand if you hate flares :'( */
	if (!cg_dynamiclensflares.value)
		return; 

	for (i = 0; i < MAX_LENS_FLARES; i++) { /* find the next free slot */
		if ( !lensFlare[i].qfull ) {
			/* VectorCopy(worldCoord, lensFlare[i].worldCoord); */
			lensFlare[i].worldCoord[0] = worldCoord[0];
			lensFlare[i].worldCoord[1] = worldCoord[1];
			lensFlare[i].worldCoord[2] = worldCoord[2];
			lensFlare[i].w1 = w1;
			lensFlare[i].h1 = h1;
			/* VectorCopy(glowColor, lensFlare[i].glowColor); */
			lensFlare[i].glowColor[0] = glowColor[0];
			lensFlare[i].glowColor[1] = glowColor[1];
			lensFlare[i].glowColor[2] = glowColor[2];
			lensFlare[i].glowOffset = glowOffset;
			lensFlare[i].hazeOffset = hazeOffset;
			lensFlare[i].minDist = minDist;
			lensFlare[i].maxDist = maxDist;
			/* VectorCopy(streakColor, lensFlare[i].streakColor); */
			lensFlare[i].streakColor[0] = streakColor[0];
			lensFlare[i].streakColor[1] = streakColor[1];
			lensFlare[i].streakColor[2] = streakColor[2];
			lensFlare[i].streakDistMin = streakDistMin;
			lensFlare[i].streakDistMax = streakDistMax;
			lensFlare[i].streakW = streakW;
			lensFlare[i].streakH = streakH;
			lensFlare[i].whiteStreaks = whiteStreaks;
			lensFlare[i].reflecDistMin = reflecDistMin;
			lensFlare[i].reflecDistMax = reflecDistMax;
			lensFlare[i].reflecAnamorphic = reflecAnamorphic;
			lensFlare[i].defReflecs = defReflecs;
			lensFlare[i].clamp = clamp;
			lensFlare[i].maxAlpha = maxAlpha;
			lensFlare[i].startTime = startTime;
			lensFlare[i].upTime = upTime;
			lensFlare[i].holdTime = holdTime;
			lensFlare[i].downTime = downTime;
			lensFlare[i].qfull = qtrue;
			
			break;
		}
	}	

}


/*
=================
CG_WorldCoordToScreenCoord
**Blatently plagiarised from EF SP**

OMFG this is some damn whacky maths!
It basically takes a vector variable and somehow
correlates that to an XY value on your screen!! O_o
=================
*/

static qboolean CG_WorldCoordToScreenCoord(vec3_t worldCoord, float *x, float *y, qboolean clamp)
{
	int	xcenter, ycenter;
	vec3_t	local, transformed;
	vec3_t	fwd;
	vec3_t	right;
	vec3_t	up;
	float xzi;
	float yzi;

/*	xcenter = cg.refdef.width / 2;*//*gives screen coords adjusted for resolution*/
/*	ycenter = cg.refdef.height / 2;*//*gives screen coords adjusted for resolution*/
	
	/*
	 * NOTE: did it this way because most draw functions expect virtual 640x480 coords
	 *	and adjust them for current resolution
	 */
	/*xcenter = 640 * 0.5;*//*gives screen coords in virtual 640x480, to be adjusted when drawn*/
	/*ycenter = 480 * 0.5;*//*gives screen coords in virtual 640x480, to be adjusted when drawn*/
	xcenter = 640 >> 1;
	ycenter = 480 >> 1;

	AngleVectors (cg.refdefViewAngles, fwd, right, up);
	VectorSubtract (worldCoord, cg.refdef.vieworg, local);

	transformed[0] = DotProduct(local,right);
	transformed[1] = DotProduct(local,up);
	transformed[2] = DotProduct(local,fwd);		

	/* Make sure Z is not negative. */
	if(transformed[2] < 0.01)
	{
		if ( clamp )
		{
			transformed[2] = 0.01f;
		}
		else
		{
			return qfalse;
		}
	}
	/* Simple convert to screen coords. */
	xzi = xcenter / transformed[2] * (96.0/cg.refdef.fov_x);/*90*/ /*95*/
	yzi = ycenter / transformed[2] * (102.0/cg.refdef.fov_y);/*90*/ /*105*/

	*x = (float)(xcenter + xzi * transformed[0]);
	*y = (float)(ycenter - yzi * transformed[1]);

	return qtrue;
}


/*************************************
CG_FlareScreenTrans - TiM

Used to return an alpha value
based on how far the xy value is
from two boundaries (Used mainly
for when the flare exits the screen
and fades out)

The function works by drawing an imaginary
line from the minimum point to the maximum
point.  If a point is above that line,
the Y value is used to calculate the alpha,
else, the X value does.  
There is a slight bit
of jerkiness if the point crosses this line,
but much less worse than what was before. :)
*************************************/

static float CG_FlareScreenTrans(int x, int y, int xmin, int ymin, int xmax, int ymax ) 
{
	/*
	 * Think about it, when the XY points are in separate quadrants of the screen,
	 * they're all the same values anyway, but just either negative or positive.
	 * Making them all positive, and working on just that set kills about 8 birds with a fricken' huge stone. >:)
	 */
	int lx = abs(x);
	int ly = abs(y);
	int lxmin = abs(xmin);
	int lymin = abs(ymin);
	int lxmax = abs(xmax);
	int lymax = abs(ymax);
	int xDif = lxmax - lxmin;
	int yDif = lymax - lymin;
	float grad = ( (float)lymax/(float)lxmax ); /* calc the grad as if (xmin, ymin) were the origin */

	float alpha = 1.0;

	/* if xy is under minimums, just make it 1 :P */
	if (lx < lxmin && ly < lymin ) {
		return alpha;
	}

	if ( ly < (lx * grad) ) {/* point is running along the side bar */
		alpha = (float)( 1.0 - ( (float)lx - (float)lxmin ) / (float)xDif );
		/* CG_Printf("SIDE BAR!!!! alpha = %f, ly = %i, lymin = %i, yDif = %i\n", alpha, ly, lymin, yDif); */
	}

	if ( ly > ( lx * grad) ) {/* point is running along the top bar */
		alpha = (float)( 1.0 - ( (float)ly - (float)lymin ) / (float)yDif );
		/* CG_Printf("TOP BAR!!!! alpha = %f, lx = %i, lxmin = %i, xDif = %i, xEq = %f\n", alpha, lx, lxmin, xDif, ((float)lx * grad) ); */
	}

	/* if xy has exceeded maxes, just make it 0 :P */
	if ( lx >= lxmax || ly >= lymax )
		alpha = 0.0;

	/* Lock it just in case something weird happened. :S */
	if ( alpha > 1.0 )
		alpha = 1.0;
	if ( alpha < 0.0 )
		alpha = 0.0;

	return alpha;

}


/*
================
CG_CorrelateMaxMinDist

Calcuates an alpha value
between a min and a max point
so elements can fade in or out 
depending on relative distance :) 
================
*/
static float CG_CorrelateMaxMinDist( float len, int min, int max ) {

	float alpha = 1.0;

	if ( min == max && max == 0 ) /* This means it will always be off */
		return 0.0;

	if ( min <= 0 ) /* this means that the parameter wants it to always be on */
		return alpha;

	alpha = /*1.0 -*/ ( len - (float)min ) / ((float)max - (float)min); /* calculate the alpha */

	if (alpha > 1.0 ) /* Clamp it.... again */
		alpha = 1.0;
	if (alpha < 0.0 )
		alpha = 0.0;

	return alpha;

}

/*
================
CG_FadeAlpha

Modified version of
CG_FadeColor. Only
covers alpha values now,
and also has an option
to fade in as well as out
================
*/
float CG_FadeAlpha( int startMsec, int totalMsec, qboolean fade_in ) {
	static float		alpha;
	int			t;

	if ( startMsec == 0 ) {
		return (fade_in ? 0.0 : 1.0);
	}

	t = cg.time - startMsec;

	if ( t >= totalMsec ) {
		return (fade_in ? 1.0 : 0.0);
	}

	// fade out
	if ( totalMsec - t < FADE_TIME ) {
		if (!fade_in)
			alpha = ( totalMsec - t ) * 1.0/FADE_TIME;
		else
			alpha = 1.0 - (( totalMsec - t ) * 1.0/FADE_TIME);
	} else {
		alpha = fade_in ? 0.0 : 1.0;
	}

	return alpha;
}

/*
================
CG_FlareTraceTrans

Performs a trace between player
and origin, and if anything gets in
the way, an alpha value is generated 
to make the flare fade out
================
*/

static float prevFrac = 0.0;
static int fadeTime, fadeInTime;

static qboolean CG_FlareTraceTrans ( vec3_t origin, float* alpha )
{
	trace_t trace;

	CG_Trace( &trace, origin, NULL, NULL, cg.refdef.vieworg, -1, CONTENTS_SOLID|CONTENTS_BODY ); //Do a trace // switched start and end

	if ( fadeTime > 0 && fadeInTime == 0 ) {
		*alpha = CG_FadeAlpha( fadeTime, 199, qfalse );
		if (*alpha == 0.0)
			fadeTime = 0.0f;
	}

	if ( fadeInTime > 0 && fadeTime == 0 ) {
		*alpha = CG_FadeAlpha( fadeInTime, 199, qtrue );
		if (*alpha == 1.0)
			fadeInTime = 0.0f;
	}
	
	//fade out the flare 
	if (trace.fraction < 1.0 && prevFrac == 1.0 ) {
		fadeTime = cg.time;
		prevFrac = trace.fraction;
	}

	//fade in the flare
	if (trace.fraction == 1.0 && prevFrac < 1.0 ) {
		fadeInTime = cg.time;
		prevFrac = trace.fraction;
	}

	if (fadeTime > 0 && fadeInTime > 0) { //Whoa, how did this happen???
		fadeTime = 0; //reset them both and all is good :)
		fadeInTime = 0;
	}

	if ( (fadeTime == 0.0 && fadeInTime == 0.0 ) && ( *alpha > 0.0 && *alpha < 1.0 ) ) //Now THIS effect was weird O_o
		*alpha = 1.0;

	if (trace.fraction < 1.0 && prevFrac < 1.0 && fadeTime == 0 && fadeInTime == 0) {
		prevFrac = trace.fraction;
		return qfalse;
	}

	return qtrue;

}

/*************************************************************
CG_DrawLensFlare - RPG-X : TiM
OMFG LENSFLARES R COOL!!!!! ^_^!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Yes, I know I'm over-doing it now, coding this uber-huge
processor-intensive, totally un-necessary lensflare engine ;P

Parameters Key:

vec3_t worldCoord			:			Position in world to draw the flare
int w1, h1					:			Initial (Maximum) w + h of the flare core
vec3_t glowColor			:			Color of the flare's glow
float glowOffset			:			Multiplier how much bigger the glow is than the core
float hazeOffset			:			Multiplier how much bigger the surrounding haze is to the core
int minDist					:			Minimum distance before the flare loses all brightness (Set to 0 if always normal size)
int maxDist					:			Maximum distance for flare's brightness
vec3_t streakColor			:			Color of the flare's lens reflections (if 0,0,0, then a default blue is used)
int	streakDistMin			:			Distance at where the flare is totally transparent (Set to 0 if always on)
int streakDistMax			:			Distance at where the flare is totally opaque (Set to same as above to turn it always off)
int streakW					:			Length of the anamorphic lens streak 
int streakH					:			Height of the anamorphic lens streak 
qboolean whiteStreaks		:			Adds white streaks to the center of normal streaks ;P
int reflecDistMin			:			Distance at where the reflections are totally transparent (Set to NULL if always on)
int reflecDistMax			:			Distance at where the reflections are totally opaque (Set to same value as above if wanted off)
qboolean reflecAnamorphic	:			Enables anamorphic lens reflections
qboolean defReflecs			:			Makes the Lens Reflections default colors
qboolean clamp				:			If qtrue, the lensflare will not resize as the distance changes
float maxAlpha				:			All alpha values of the elements in the flare will not exceed this number
int upTime					:			How long it takes for the flare to go from 0 intense to maximum intense
int holdTime				:			How long the flare stays at max intensity for
int downTime				:			How long it takes for the flare to go from max intensity to 0.

**************************************************************/

void CG_DrawLensFlare( lensFlare_t *flare )
{ 
	int		w = flare->w1;
	int		h = flare->h1;
	float	x, y, streakX, streakY; 
	int		xCart, yCart;
	int		i;
	vec4_t	color, reflecColor, strkColor;
	int		xMax, yMax;
	vec3_t	distDif, black = {0.0, 0.0, 0.0};
	int maxTime = flare->upTime + flare->holdTime + flare->downTime;
	int tMaxTime = maxTime + flare->startTime;
	int tUpTime	= flare->upTime + flare->startTime;
	int tHoldTime = flare->upTime + flare->holdTime + flare->startTime;
	int tDownTime = flare->upTime + flare->holdTime + flare->downTime + flare->startTime;
	float	length;

	float	reflecAlpha = 1.0; //alpha channel of reflections
	float	streakAlpha = 1.0; //alpha channel of streaks 
	float	boundAlpha	= 1.0; //alpha if flare leaves screen		
	float	commonAlpha = 1.0; //alpha variables common too all elements
	float	hazeAlpha	= 1.0;

	static float	fadeAlpha;	//This can't have a default value otherwise it screws up the flare fade transition
	static float	timeAlpha; //Alpha/w/h over the specified time

	//First thing's first.... I understand if you hate flares :'(
	if (!cg_dynamiclensflares.value)
		return; 

	//if we can't get an XY value, screw it :P
	if ( !CG_WorldCoordToScreenCoord( flare->worldCoord, &x, &y, qfalse) )
		return;

	//if we can't actually see the flare in line of sight, screw it again. :P
	if( !CG_FlareTraceTrans( flare->worldCoord, &fadeAlpha) )
		return;

	if (maxTime > 0 && cg.time <= tMaxTime) {
			
		if ( cg.time <= tUpTime )
			timeAlpha = (float)(cg.time - flare->startTime) * (float)(1.0/(float)flare->upTime);

		if (cg.time <= tHoldTime && cg.time > tUpTime )
			timeAlpha = 1.0;

		if (cg.time <= tDownTime && cg.time > tHoldTime )
			timeAlpha = 1.0 - ( (float)(cg.time - flare->startTime) * (float)(1.0/(float)flare->downTime) );
	}

	if (maxTime == 0 )
		timeAlpha = 1.0;

	w = w * timeAlpha;
	h = h * timeAlpha;

	//calc the distance between the player and the flare
	VectorSubtract( flare->worldCoord, cg.refdef.vieworg, distDif );
	length = VectorNormalize( distDif );

	//if the clamp boolean is false, resize the flare over player distance from it
	if ( !flare->clamp ) {
		w = w * CG_CorrelateMaxMinDist(length, flare->minDist, flare->maxDist ); //Change size/height in relation to distance
		h = h * CG_CorrelateMaxMinDist(length, flare->minDist, flare->maxDist );
	}

	xCart = (int)(x - HALF_SCREEN_WIDTH ); //Re-orient the EF drawing engine so co-ord (0,0) is in the middle of the screen)
	yCart = (int)(y - HALF_SCREEN_HEIGHT );

	streakX = (xCart - (flare->streakW*0.5)) + HALF_SCREEN_WIDTH; //Calculate X value of lens streak based on flare position
	streakY = (yCart - (flare->streakH*0.5)) + HALF_SCREEN_HEIGHT; //Calculate Y value of lens streak based on flare position

	xMax = (w*0.5) + HALF_SCREEN_WIDTH; //define the point the flare should fully fade out
	yMax = (h*0.5) + HALF_SCREEN_HEIGHT;

	if ( boundAlpha > 0.0 ) { //Calculate the reflections' opacity in contrast to the edge of the screen
		boundAlpha = CG_FlareScreenTrans( xCart, yCart, HALF_SCREEN_WIDTH, HALF_SCREEN_HEIGHT, xMax, yMax);
	}

	//set up all of the elements with their various alphas :P
	commonAlpha = commonAlpha * fadeAlpha * boundAlpha * flare->maxAlpha;

	if (commonAlpha * timeAlpha < 0.01 ) //no point in drawing if it's really really faint
		return;

	reflecAlpha = reflecAlpha * commonAlpha * timeAlpha * CG_CorrelateMaxMinDist(length, flare->reflecDistMin, flare->reflecDistMax );
	streakAlpha = streakAlpha * commonAlpha * timeAlpha * CG_CorrelateMaxMinDist(length, flare->streakDistMin, flare->streakDistMax );
	hazeAlpha	= hazeAlpha * commonAlpha;

	//Copy in the color the user wants, but we need control of the alpha.
	VectorCopy( flare->glowColor, color );
	color[3] = hazeAlpha;

	if ( VectorCompare( flare->streakColor, black) ) { //If they specified no streakcolor, use this awesome default blue one :)
		strkColor[0] = 0.31;
		strkColor[1] = 0.45;
		strkColor[2] = 1.0;
		strkColor[3] = streakAlpha;
	}
	else { //else, use the color they wanted
		VectorCopy( flare->streakColor, strkColor );
		strkColor[3] = streakAlpha;
	}

	//Lens Reflections - those cool circly bits that go in the opposite direction of the flare
	if ( reflecAlpha != 0.0 ) {//Sheez, only do this if we really WANT it O_o
		for( i = 0; i < 10; i++ ) {
		
			//if they wanted the cool photoshoppy style reflections
			if ( flare->defReflecs ) {
				VectorCopy( lensReflec[i].color, reflecColor );
				reflecColor[3] = reflecAlpha;
			}
			else { //otherwise, just use the color they picked
				VectorCopy( color, reflecColor );
				reflecColor[3] = reflecAlpha;
			}	

			trap_R_SetColor( reflecColor );

			CG_DrawPic( 
				( ( ( lensReflec[i].positive ? xCart : -xCart ) * lensReflec[i].offset ) + HALF_SCREEN_WIDTH ) - ( flare->reflecAnamorphic ? lensReflec[i].width : lensReflec[i].width*0.5 ), //X
				( ( ( lensReflec[i].positive ? yCart : -yCart ) * lensReflec[i].offset ) + HALF_SCREEN_HEIGHT ) - ( lensReflec[i].height*0.5 ), //Y
				flare->reflecAnamorphic ? lensReflec[i].width * 2 : lensReflec[i].width, //W
				lensReflec[i].height, //H
				lensReflec[i].graphic //pic
			);
		}
	}

	//Colored Middle + Streaks
	trap_R_SetColor( color ); 
	if ( color[3] > 0.0 ) {
		x = ( xCart - ( (w*flare->hazeOffset) *0.5) + HALF_SCREEN_WIDTH );
		y = ( yCart - ( (h*flare->hazeOffset) *0.5) + HALF_SCREEN_HEIGHT ); 
		CG_DrawPic( x, y, w*flare->hazeOffset, h*flare->hazeOffset, cgs.media.flareHaze ); //Surrounding ambient haze
	}

	trap_R_SetColor( strkColor );
	if ( strkColor[3] > 0.0f )
		CG_DrawPic( streakX , streakY , flare->streakW, flare->streakH, cgs.media.flareStreak ); //Colored portion of the anamorphic streaks
	
	trap_R_SetColor( color ); 
	if ( color[3] > 0.0f ) {
		x = ( xCart - ( (w*flare->glowOffset) *0.5) + HALF_SCREEN_WIDTH );
		y = ( yCart - ( (h*flare->glowOffset) *0.5) + HALF_SCREEN_HEIGHT ); 
		CG_DrawPic( x, y, w*flare->glowOffset, h*flare->glowOffset, cgs.media.flareCore ); //Main colored glow bit of the main flare
	}

	if ( flare->whiteStreaks ) { //if player wanted white streaks in their streaks
		strkColor[0] = strkColor[1] = strkColor[2] = 1.0;

		trap_R_SetColor( strkColor ); //White
		if ( strkColor[3] > 0.0 )
			CG_DrawPic( streakX + (flare->streakW*0.2), streakY + (flare->streakH*0.2), flare->streakW*0.6, flare->streakH*0.6, cgs.media.flareStreak ); //White Core of streak is ALWAYS 20% smaller.
	}

	color[0] = color[1] = color [2] = 1.0f;
	color[3] = hazeAlpha;

	trap_R_SetColor( color );
	if ( color[3] > 0.0 ) {
		x = ( xCart - (w *0.5) + HALF_SCREEN_WIDTH );
		y = ( yCart - (h *0.5) + HALF_SCREEN_HEIGHT );
		CG_DrawPic( x, y, w, h, cgs.media.flareCore ); //Draw teh main fl4r3 :)
	} 
	//CG_Printf("worldCoord = %f, colorAlpha = %f, streakAlpha = %f, streakColor = %f \n", flare->worldCoord[0], color[3], strkColor[3], strkColor[2]);
}

/*
==============
CG_DrawField

Draws large numbers for status bar and powerups
==============
*/
/*
static void CG_DrawField (int x, int y, int width, int value) 
{
	char	num[16], *ptr;
	int		l;
	int		frame;

	if ( width < 1 ) 
	{
		return;
	}

	// draw number string
	if ( width > 5 ) 
	{
		width = 5;
	}

	switch ( width ) 
	{
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
	x += 2 + CHAR_WIDTH*(width - l);

	ptr = num;
	while (*ptr && l)
	{
		if (*ptr == '-')
			frame = STAT_MINUS;
		else
			frame = *ptr -'0';

		CG_DrawPic( x,y, CHAR_WIDTH, CHAR_HEIGHT, cgs.media.numberShaders[frame] );
		x += CHAR_WIDTH;
		ptr++;
		l--;
	}
}
*/

/*
================
CG_Draw3DModel

================
*/
static void CG_Draw3DModel( float x, float y, float w, float h, qhandle_t model, qhandle_t skin, qhandle_t shader, vec3_t origin, vec3_t angles ) {
	refdef_t		refdef;
	refEntity_t		ent;

	if ( !cg_draw3dIcons.integer || !cg_drawIcons.integer ) {
		return;
	}

	CG_AdjustFrom640( &x, &y, &w, &h );

	memset( &refdef, 0, sizeof( refdef ) );

	memset( &ent, 0, sizeof( ent ) );
	AnglesToAxis( angles, ent.axis );
	VectorCopy( origin, ent.origin );
	ent.hModel = model;
	ent.customSkin = skin;
	ent.customShader = shader;
	ent.renderfx = RF_NOSHADOW;		// no stencil shadows

	refdef.rdflags = RDF_NOWORLDMODEL;

	AxisClear( refdef.viewaxis );

	refdef.fov_x = 30;
	refdef.fov_y = 30;

	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;

	refdef.time = cg.time;

	trap_R_ClearScene();
	trap_R_AddRefEntityToScene( &ent );
	trap_R_RenderScene( &refdef );
}

/*
================
CG_DrawHead

Used for both the status bar and the scoreboard
================
*/

//extern qhandle_t CG_CurrentHeadSkin( centity_t* cent, clientInfo_t* ci );

void CG_DrawHead( float x, float y, float w, float h, int clientNum, vec3_t headAngles ) {
	clipHandle_t	cm;
	centity_t		*cent;
	clientInfo_t	*ci;
	playerState_t	*ps;
	float		value;
	float			len;
	vec3_t			origin;
	vec3_t			mins, maxs;

	cent = &cg_entities[ clientNum ];
	ci = &cgs.clientinfo[ clientNum ];

	ps = &cg.snap->ps;

	value = ps->stats[STAT_HEALTH];

	if ( cg_draw3dIcons.integer && (ci->headOffset[0] != 404) ) {
		cm = ci->headModel;
		if ( !cm ) {
			return;
		}

		// offset the origin y and z to center the head
		trap_R_ModelBounds( cm, mins, maxs );

		origin[2] = -0.5 * ( mins[2] + maxs[2] );
		origin[1] = 0.5 * ( mins[1] + maxs[1] );

		// calculate distance so the head nearly fills the box
		// assume heads are taller than wide
		len = 0.7 * ( maxs[2] - mins[2] );		
		origin[0] = len / 0.268;	// len / tan( fov/2 )

		// allow per-model tweaking
		VectorAdd( origin, ci->headOffset, origin );

		CG_Draw3DModel( x, y, w, h, ci->headModel, ci->headSkin, 0, origin, headAngles );

		if ((value < 82.000000) && (value >= 65.000000)){
			CG_Draw3DModel( x, y, w, h, ci->headModel, ci->headSkin, 0, origin, headAngles );
		}else if(value >= 49.000000){
			CG_Draw3DModel( x, y, w, h, ci->headModel, ci->headSkin, 0, origin, headAngles );
		}else if(value >= 32.000000){
			CG_Draw3DModel( x, y, w, h, ci->headModel, ci->headSkin, 0, origin, headAngles );
		}else if(value >= 2.000000){
			CG_Draw3DModel( x, y, w, h, ci->headModel, ci->headSkin, 0, origin, headAngles );
		}else if(value <= 1.000000){
			CG_Draw3DModel( x, y, w, h, ci->headModel, ci->headSkin, 0, origin, headAngles );
		}
	} else if ( cg_drawIcons.integer ) {
		CG_DrawPic( x, y, w, h, ci->modelIcon );
	}
	
	//if ( cgs.clientinfo[clientNum].health <= 1 ) {//if eliminated, draw the cross-out
	//	CG_DrawPic( x, y, w, h, cgs.media.eliminatedShader );
	//} else if ( ci->deferred ) {// if they are deferred, draw a cross out
//		CG_DrawPic( x, y, w, h, cgs.media.deferShader );
	//}
}

/*
================
CG_DrawFlagModel

Used for both the status bar and the scoreboard
================
*/
void CG_DrawFlagModel( float x, float y, float w, float h, int team ) {
	qhandle_t		cm;
	float			len;
	vec3_t			origin, angles;
	vec3_t			mins, maxs;

	if ( cg_draw3dIcons.integer ) {

		VectorClear( angles );

		cm = cgs.media.redFlagModel;

		// offset the origin y and z to center the flag
		trap_R_ModelBounds( cm, mins, maxs );

		origin[2] = -0.5 * ( mins[2] + maxs[2] );
		origin[1] = 0.5 * ( mins[1] + maxs[1] );

		// calculate distance so the flag nearly fills the box
		// assume heads are taller than wide
		len = 0.5 * ( maxs[2] - mins[2] );		
		origin[0] = len / 0.268;	// len / tan( fov/2 )

		angles[YAW] = 60 * sin( cg.time / 2000.0 );;

		CG_Draw3DModel( x, y, w, h, 
			team == TEAM_RED ? cgs.media.redFlagModel : cgs.media.blueFlagModel, 0, 
			team == TEAM_RED ? cgs.media.redFlagShader[3] : cgs.media.blueFlagShader[3], origin, angles );
	} else if ( cg_drawIcons.integer ) {
		//gitem_t *item = BG_FindItemForPowerup( team == TEAM_RED ? PW_REDFLAG : PW_BORG_ADAPT );

		/*if (item)
		{
			CG_DrawPic( x, y, w, h, cg_items[ ITEM_INDEX(item) ].icon );
		}*/
	}
}

/*
================
CG_DrawStatusBarHead
RPG-X | Phenix | 09/06/2005
I dont know who commented this out but it's going back in ;)
================
*/
static int CG_DrawStatusBarHead( float x ) {
	vec3_t		angles;
	float		size, stretch;
	float		frac;

	VectorClear( angles );

	if ( cg.damageTime && cg.time - cg.damageTime < DAMAGE_TIME ) {
		frac = (float)(cg.time - cg.damageTime ) / DAMAGE_TIME;
		size = ICON_SIZE * 1.25 * ( 1.5 - frac * 0.5 );

		stretch = size - ICON_SIZE * 1.25;
		// kick in the direction of damage
		x -= stretch * 0.5 + cg.damageX * stretch * 0.5;

		cg.headStartYaw = 180 + cg.damageX * 45;

		cg.headEndYaw = 180 + 20 * cos( crandom()*M_PI );
		cg.headEndPitch = 5 * cos( crandom()*M_PI );

		cg.headStartTime = cg.time;
		cg.headEndTime = cg.time + 100 + random() * 2000;
	} else {
		if ( cg.time >= cg.headEndTime ) {
			// select a new head angle
			cg.headStartYaw = cg.headEndYaw;
			cg.headStartPitch = cg.headEndPitch;
			cg.headStartTime = cg.headEndTime;
			cg.headEndTime = cg.time + 100 + random() * 2000;

			cg.headEndYaw = 180 + 20 * cos( crandom()*M_PI );
			cg.headEndPitch = 5 * cos( crandom()*M_PI );
		}

		size = ICON_SIZE * 1.25;
	}

	size = size * 3;

	// if the server was frozen for a while we may have a bad head start time
	if ( cg.headStartTime > cg.time ) {
		cg.headStartTime = cg.time;
	}

	frac = ( cg.time - cg.headStartTime ) / (float)( cg.headEndTime - cg.headStartTime );
	frac = frac * frac * ( 3 - 2 * frac );
	angles[YAW] = cg.headStartYaw + ( cg.headEndYaw - cg.headStartYaw ) * frac;
	angles[PITCH] = cg.headStartPitch + ( cg.headEndPitch - cg.headStartPitch ) * frac;

	CG_DrawHead( x, 480 - (size + BIGCHAR_HEIGHT + 5), size, size, 
				cg.snap->ps.clientNum, angles );

	return size;
}

/*
================
CG_DrawTeamBackground

================
*/
void CG_DrawTeamBackground( int x, int y, int w, int h, float alpha, int team, qboolean scoreboard )
{
	vec4_t		hcolor;

	hcolor[3] = alpha;
	if ( team == TEAM_RED ) 
	{
		hcolor[0] = 1;
		hcolor[1] = 0;
		hcolor[2] = 0;
	} 
	else if ( team == TEAM_BLUE ) 
	{
		hcolor[0] = 0;
		hcolor[1] = 0;
		hcolor[2] = 1;
	} 
	else 
	{
		return; // no team
	}

	trap_R_SetColor( hcolor );
	CG_DrawPic( x, y, w, h, cgs.media.teamStatusBar );
	trap_R_SetColor( NULL );
}

/*
================
CG_DrawAmmo

================
*/
static void CG_DrawAmmo(centity_t	*cent)
{
	float		value;
//	float		xLength;
	playerState_t	*ps;
//	int			max,brightColor_i,darkColor_i,numColor_i;

	ps = &cg.snap->ps;
	
	value = ps->ammo[cent->currentState.weapon];
	
	return;
}

//RPG-X: - RedTechie NO ARMOR! how many times do i have to say it!
/*
================
CG_DrawArmor

================
*/
/*
static void CG_DrawArmor(centity_t	*cent)
{
	int			max;
	float		value,xLength;
	playerState_t	*ps;
	int			lengthMax;

	ps = &cg.snap->ps;

	value = ps->stats[STAT_ARMOR];

	interface_graphics[IG_ARMOR_COUNT].max = value;

	if (interface_graphics[IG_ARMOR_COUNT].max <= ps->stats[STAT_MAX_HEALTH])
	{
		interface_graphics[IG_ARMOR_COUNT].color = CT_LTPURPLE1;		// 
		interface_graphics[IG_ARMOR_SLIDERFULL].color = CT_LTPURPLE1;	// 
		interface_graphics[IG_ARMOR_COUNT].style &= ~UI_PULSE;			// Numbers
	}
	else
	{
		interface_graphics[IG_ARMOR_COUNT].color = CT_LTGREY;			// Numbers
		interface_graphics[IG_ARMOR_SLIDERFULL].color = CT_LTGREY;		// 
		interface_graphics[IG_ARMOR_COUNT].style |= UI_PULSE;			// Numbers
	}



//	if (cg.oldarmor < value)
//	{
//		cg.oldArmorTime = cg.time + 100;
//	}

//	cg.oldarmor = value;

//	if (cg.oldArmorTime < cg.time)
//	{ 
//		interface_graphics[IG_ARMOR_COUNT].color = CT_LTPURPLE1;	// Numbers
//	}
//	else
//	{
//		interface_graphics[IG_ARMOR_COUNT].color = CT_YELLOW;	// Numbers
//	}


	max = ps->stats[STAT_MAX_HEALTH];
	lengthMax = 73;
	if (max > 0)
	{
		if (value > max)
		{
			xLength = lengthMax;
		}
		else
		{
			xLength = lengthMax * (value/max);
		}

	}
	else
	{
		max = 0;
		xLength = 0;
	}

	// Armor empty section
	interface_graphics[IG_ARMOR_SLIDEREMPTY].x = 72 + xLength;
	interface_graphics[IG_ARMOR_SLIDEREMPTY].width = lengthMax - xLength;

	// Armor full section
	interface_graphics[IG_ARMOR_SLIDERFULL].width = xLength;

	CG_PrintInterfaceGraphics(IG_ARMOR_START + 1,IG_ARMOR_END);

}
*/

//RPG-X: - RedTechie Close but no cigar we need 3 stage health not a bar
/*
================
CG_DrawHealth

================
*/

/*static void CG_DrawHealth(centity_t	*cent)
{
	int			max;
	float		value,xLength;
	playerState_t	*ps;
	int			lengthMax;

	ps = &cg.snap->ps;

	value = ps->stats[STAT_HEALTH];


	// Changing colors on numbers
//	if (cg.oldhealth < value)
//	{
//		cg.oldHealthTime = cg.time + 100;
//	}
//	cg.oldhealth = value;

	// Is health changing?
//	if (cg.oldHealthTime < cg.time)
//	{
//		interface_graphics[IG_HEALTH_COUNT].color = CT_LTBROWN1;	// Numbers
//	}
//	else
//	{
//	}
	interface_graphics[IG_HEALTH_COUNT].max = value;

	if (interface_graphics[IG_HEALTH_COUNT].max <= ps->stats[STAT_MAX_HEALTH])
	{
		interface_graphics[IG_HEALTH_COUNT].color = CT_LTBROWN1;	// 
		interface_graphics[IG_HEALTH_SLIDERFULL].color = CT_LTBROWN1;	// 
		interface_graphics[IG_HEALTH_SLIDEREMPTY].color = CT_DKBROWN1;	// 
		interface_graphics[IG_HEALTH_COUNT].style &= ~UI_PULSE;			// Numbers
	}
	else
	{
		interface_graphics[IG_HEALTH_COUNT].color = CT_LTGREY;			// Numbers
		interface_graphics[IG_HEALTH_SLIDERFULL].color = CT_LTGREY;		// 
		interface_graphics[IG_HEALTH_COUNT].style |= UI_PULSE;			// Numbers
	}

	// Calculating size of health bar
	max = ps->stats[STAT_MAX_HEALTH];
	lengthMax = 73;
	if (max > 0)
	{
		if (value < max)
		{
			xLength = lengthMax * (value/max);
		}
		else	// So the graphic doesn't extend past the cap
		{
			xLength = lengthMax;
		}
	}
	else
	{
		max = 0;
		xLength = 0;
	}

	// Health empty section
	interface_graphics[IG_HEALTH_SLIDEREMPTY].x = 72 + xLength;
	interface_graphics[IG_HEALTH_SLIDEREMPTY].width = lengthMax - xLength;

	// Health full section
	interface_graphics[IG_HEALTH_SLIDERFULL].width = xLength;

	// Print it
	CG_PrintInterfaceGraphics(IG_HEALTH_START + 1,IG_HEALTH_END);
}*/

//RPG-X: - RedTechie This is more like it
/*
================
CG_DrawHealth
New Draw health function by yours truly RedTechie
New version by TiM lol
================
*/

//static int CG_DrawHealth( centity_t *cent )
//{
//	float value;
//	float offset;
//	float yOffset;
//
//	value = cg.snap->ps.stats[STAT_HEALTH];
//
//	//Draw static graphics first
//	CG_FillRect( 8, 428, 89, 1, colorTable[CT_LTPURPLE1] );
//	CG_FillRect( 8, 429, 1, 44, colorTable[CT_LTPURPLE1] );
//	CG_FillRect( 8, 473, 89, 1, colorTable[CT_LTPURPLE1] );
//	CG_FillRect( 96, 429, 1, 44, colorTable[CT_LTPURPLE1] );
//
//	//Okay... we'll need to work out some funky math here later...
//	//For now, let's just test
//	offset = (( (float)(cg.time % 2000) / 2000.0f ) * (1.0f+(1.0f-(value/100.0f)) * 0.25f));
//	yOffset = (value / 100.0f ) * 21.0f ;
//	//CG_Printf( "%f\n", offset );
//
//	trap_R_SetColor( NULL );
//	CG_DrawStretchPic( 9, 450 - yOffset, 87, yOffset * 2.0f, 0.0f + offset, 0.0f, (1.0f+(1.0f-(value/100.0f)) * 0.25f) + offset, 1.0f, cgs.media.healthSineWave );
//	//CG_DrawStretchPic( 16, 413, 123, 60, 1.0f, 1.0f, 2.0f, 2.0f, cgs.media.healthSineWave );
//
//	return 125;
//}

static int CG_DrawHealth(centity_t	*cent)
{
	float		value;
	playerState_t	*ps;
	char		*health_str = NULL;
	int         health_barwidth;
	vec_t       *health_txtcolor = NULL;
	int			health_txteffect = 0;
	int			x, y;

	ps = &cg.snap->ps;

	value = ps->stats[STAT_HEALTH];
	
	//RPG-X: RedTechie - The GROSS math part icky icky!
	if(value >= 82.000000){
		health_str = ingame_text[IGT_SB_HEALTHSTATUS1]; //RPG-X and SFEF
		health_txtcolor = colorTable[CT_DKPURPLE2];
		health_txteffect = UI_BIGFONT;
	}else if(value >= 65.000000){
		health_str = ingame_text[IGT_SB_HEALTHSTATUS2]; //Scott Carter, after being locked in a room with rpg-x team for 20 minuts..
		health_txtcolor = colorTable[CT_DKPURPLE2];
		health_txteffect = UI_BIGFONT;
	}else if(value >= 49.000000){
		health_str = ingame_text[IGT_SB_HEALTHSTATUS3]; //Results after 10 minutes
		health_txtcolor = colorTable[CT_LTBLUE2];
		health_txteffect = UI_BIGFONT;
	}else if(value >= 32.000000){
		health_str = ingame_text[IGT_SB_HEALTHSTATUS4]; //Results after 15 minutes
		health_txtcolor = colorTable[CT_LTBLUE2];
		health_txteffect = UI_BIGFONT;
	}else if(value >= 2.000000){
		health_str = ingame_text[IGT_SB_HEALTHSTATUS5]; //Results after 20 minutes
		health_txtcolor = colorTable[CT_VDKBLUE2];
		health_txteffect = UI_BIGFONT;
	}else if(value <= 1.000000){
		health_str = ingame_text[IGT_SB_HEALTHSTATUS6]; //Final result - post your comments here coders ;) -
														//What do you mean final result - this is like after 30 seconds in a room with the rpg-x team :P (Phenix)
		health_txtcolor = colorTable[CT_RED];			//More like 10 -TiM
		health_txteffect = UI_BIGFONT;
	}
	//Get a accurate width
	health_barwidth = UI_ProportionalStringWidth(health_str,UI_BIGFONT);
	
	//Doom Style Health!
	if (doomHead.integer == 1)
	{
		health_barwidth = CG_DrawStatusBarHead( 2 );
		health_barwidth = ((health_barwidth / 2) + 2) - (UI_ProportionalStringWidth(health_str,UI_BIGFONT) / 2);
		UI_DrawProportionalString(health_barwidth, 460 - BIGCHAR_HEIGHT, health_str, health_txteffect, health_txtcolor);

		return health_barwidth;
	} else {
		x = 3;
		y = 435;

		//Draw the text
		UI_DrawProportionalString(x + 46, y + 11, health_str, health_txteffect, health_txtcolor);

		//RPG-X: - RedTechie The Graphics :)
		
		trap_R_SetColor( colorTable[CT_DKBLUE1] );
		CG_DrawPic( x, y, 85, 22, cgs.media.healthbigcurve ); //RPG-X: Big Curve //x,y,w,h=32
		CG_DrawPic( x + 49 + health_barwidth, y, 8, 7, cgs.media.healthendcap ); //RPG-X: Top End Cap - 133 
		CG_DrawPic( x + 49 + health_barwidth, y + 37, 8, 7, cgs.media.healthendcap ); //RPG-X: Bottum End Cap - 133 //CG_DrawPic( x + 49 + health_barwidth, y + 49, 16, 16, cgs.media.healthendcap );
		CG_FillRect( x, y + 15, 40, 20, colorTable[CT_DKBLUE1]); //Extra bit to fill in the gap under the curve graphic

		CG_FillRect( x, y + 37, 40, 7, colorTable[CT_DKGOLD1]); //RPG-X: Middle bar //15
		CG_FillRect( x + 42, y + 37, 5+health_barwidth, 7, colorTable[CT_DKBLUE1]); //RPG-X: Bottum Horizontal bar - CG_FillRect( 45, 469, 86+health_barwidth, 15, colorTable[CT_DKBLUE1]);
		CG_FillRect( x + 47, y, health_barwidth, 7, colorTable[CT_DKBLUE1]); //RPG-X: Top Horizontal bar - CG_FillRect( 61, 420, 70+health_barwidth, 15, colorTable[CT_DKBLUE1]);

		//RPG-X: RedTechie - Some eye candy text
		UI_DrawProportionalString( x +40-3, y + 23, ingame_text[IGT_SB_HEALTHBARLCARS], UI_TINYFONT|UI_RIGHT, colorTable[CT_BLACK]);//456
//x + 12
		return health_barwidth + 82;
	}
}

/*
================
CG_DrawStatusBar

================
*/
static void CG_DrawStatusBar( void ) 
{
	centity_t	*cent;
	playerState_t	*ps;
	vec3_t		angles;
	int y=0;
	vec4_t	whiteA;
	int		x, z, i, h, yZ;
	vec3_t	tmpVec, eAngle, forward, dAngle;
	//RPG-X: Redtechie - for the HACK code below
	//int     rpg_shakemycamera;
	int healthBarWidth;
	//float	rpg_shakemycamera_intensity;
	//const char	*info;

	/*static float colors[4][4] = 
	{ 
		{ 1, 0.69, 0, 1.0 } ,		// normal
		{ 1.0, 0.2, 0.2, 1.0 },		// low health
		{0.5, 0.5, 0.5, 1},			// weapon firing
		{ 1, 1, 1, 1 } };			// health > 100*/

	whiteA[0] = whiteA[1] = whiteA[2] = 1.0f;	whiteA[3] = 0.3f;
	
	cent = &cg_entities[cg.snap->ps.clientNum];
	
	//RPG-X: RedTechie - HACK HACK HACK!!!! this needs to be called soon to check to shake the players cameras
	/*info = CG_ConfigString( CS_SERVERINFO );
	rpg_shakemycamera = atoi( Info_ValueForKey( info, "rpg_servershakeallclients" ) );
	rpg_shakemycamera_intensity = atof( Info_ValueForKey( info, "rpg_servershakeallclientsintensity" ) );
	if(rpg_shakemycamera == 1){
		CG_CameraShake(rpg_shakemycamera_intensity,300);
	}*/

	if ( cg_drawStatus.integer == 0 ) {
		return;
	}

	// draw the team background
	CG_DrawTeamBackground( 0, 420, 640, 60, 0.33, cg.snap->ps.persistant[PERS_TEAM], qfalse );

	ps = &cg.snap->ps;

	VectorClear( angles );

	// draw any 3D icons first, so the changes back to 2D are minimized
	y = (SCREEN_HEIGHT - (4*ICON_SIZE) - 20);
	/*if (cg.predictedPlayerState.powerups[PW_REDFLAG])
	{	//fixme: move to powerup renderer?  make it pulse?
	//	CG_FillRect(      5, y, ICON_SIZE*2, ICON_SIZE*2, whiteA);
		CG_DrawFlagModel( 5, y, ICON_SIZE*2, ICON_SIZE*2, TEAM_RED );
	}*/
	/*else if (cg.predictedPlayerState.powerups[PW_BORG_ADAPT])
	{
	//	CG_FillRect(      5, y, ICON_SIZE*2, ICON_SIZE*2, whiteA);
	//	CG_DrawFlagModel( 5, y, ICON_SIZE*2, ICON_SIZE*2, TEAM_BLUE ); 
	//RPG-X | GSIO01 | 08/05/2009: we have flag in rpg? haha
	}*/

	// Do start
	if (!cg.interfaceStartupDone)
	{
		CG_InterfaceStartup();
	}

	//
	// ammo
	//
	if ( cent->currentState.weapon )
	{
		CG_DrawAmmo(cent);
	}


	//
	// health
	//
	//RPG-X | Phenix | 09/06/2005
	// Added return of the width for the cloak etc messages
	healthBarWidth = CG_DrawHealth(cent);


	// RPG-X
	// Print RPG Flags
	//By: RedTechie & Phenix
	//
	if(cg.predictedPlayerState.powerups[PW_EVOSUIT] || cg.predictedPlayerState.powerups[PW_FLIGHT] || cg.predictedPlayerState.powerups[PW_INVIS]){
		//RPG-X | Phenix | 08/06/2005
		yZ = 478 - SMALLCHAR_HEIGHT;
		// UI_BIGFONT
		//DEBUG
		if(cg.predictedPlayerState.powerups[PW_EVOSUIT]) {
			UI_DrawProportionalString(healthBarWidth, yZ, ingame_text[IGT_SB_EVOSUITSTATUS], UI_SMALLFONT, colorTable[CT_CYAN]);
			yZ -= SMALLCHAR_HEIGHT + 2;
		}
		if(cg.predictedPlayerState.powerups[PW_INVIS]){
			UI_DrawProportionalString(healthBarWidth, yZ, ingame_text[IGT_SB_CLOAKSTATUS], UI_SMALLFONT, colorTable[CT_RED]);
			yZ -= SMALLCHAR_HEIGHT + 2;
		}
		if(cg.predictedPlayerState.powerups[PW_FLIGHT]){
			UI_DrawProportionalString(healthBarWidth, yZ, ingame_text[IGT_SB_FLIGHTSTATUS], UI_SMALLFONT, colorTable[CT_RED]);
			yZ -= SMALLCHAR_HEIGHT + 2;
		}		
	}

	//
	// armor
	//
	//RPG-X: - Redtechie IT A FRICKEN RP NOOOO ARMOR! OMG!
	//CG_DrawArmor(cent);

	// Radar
	// By Sam "-=Jazz=-"Dickinson
	// http://www.telefragged.com/jazz
	if ( ( cg.snap->ps.weapon == WP_2 || cg.snap->ps.weapon == WP_6 ) && cg_drawradar.integer != 0 && !cg.zoomed )
	{
		vec4_t	radColor;

		CG_DrawPic(40, 100, 100, 100, cgs.media.radarShader);
		for (i = 0; i < cg.snap->numEntities; i++) // Go through all entities in VIS range
		{
			if ( cg.snap->entities[i].eType == ET_PLAYER ) // If the Entity is a Player
			{
				// Calculate How Far Away They Are
				x = (cg.snap->entities[i].pos.trBase[0] - cg.predictedPlayerState.origin[0]);
				y = (cg.snap->entities[i].pos.trBase[1] - cg.predictedPlayerState.origin[1]);
				z = (cg.snap->entities[i].pos.trBase[2] - cg.predictedPlayerState.origin[2]);
				tmpVec[0] = x;
				tmpVec[1] = y;
				tmpVec[2] = 0.0;

				// Convert Vector to Angle
				vectoangles(tmpVec, eAngle);
				h = sqrt((x*x) + (y*y)); // Get Range

				// We only Want "YAW" value
				dAngle[0] = 0.0;
				dAngle[1] = AngleSubtract(eAngle[1] - 180, cg.predictedPlayerState.viewangles[1]) + 180;
				dAngle[0] = 0.0;

				// Convert Angle back to Vector
				AngleVectors(dAngle, forward, NULL, NULL);
				VectorScale(forward, h/32, forward);
//				if (h/32 < 100 && h/32 > 0) // Limit Radar Range
//				{
					// Draw up arrow if above, down if below, or an ordinary blip if level
					// With tolerance of +- 5 units
					//RPG-X: RedTechie - No teams in a RP
					/*if ( cgs.gametype >= GT_TEAM )
					{
						if ( cgs.clientinfo[cg.snap->entities[i].number].team == TEAM_BLUE )
						{
							if (z > 64)
							{
								CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_blue_up);
							}
							else if (z < -64)
							{
								CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_blue_down);
							}
							else
							{
								CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_blue_level);
							}
						}
						else if ( cgs.clientinfo[cg.snap->entities[i].number].team == TEAM_RED )
						{
							if (z > 64)
							{
								CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_red_up);
							}
							else if (z < -64)
							{
								CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_red_down);
							}
							else
							{
								CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_red_level);
							}
						}
					}*/
					//RPG-X: RedTechie - If Dead show them as a medical symbol
				//.number
				if (h/32 < 100 && h/32 > 0) { // Limit Radar Range
					if ( cg_entities[cg.snap->entities[i].number].currentState.eFlags & EF_DEAD )
					{
						if (z > 64)
						{
							CG_DrawStretchPic( 86 - forward[1], 146 - forward[0], 16, 8, 0, 0, 1, 0.5, cgs.media.rd_injured_level );
							//CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_injured_up);
						}
						else if (z < -64)
						{
							//CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_injured_down);
							CG_DrawStretchPic( 86 - forward[1], 146 - forward[0], 16, 8, 0, 0.5, 1, 1, cgs.media.rd_injured_level );
						}							
						else
						{
							CG_DrawPic(86 - forward[1], 146 - forward[0], 16, 16, cgs.media.rd_injured_level);
						}
					}
					else
					{
						//if ( cgs.clientinfo[cg.snap->entities[i].number].pClass == PC_COMMAND )
						//{
						//	/*if (z > 64)
						//	{
						//		CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_red_up);
						//	}
						//	else if (z < -64)
						//	{
						//		CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_red_down);
						//	}
						//	else
						//	{
						//		CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_red_level);
						//	}*/
						//	//trap_R_SetColor( colorTable[CT_RED] );
						//	VectorCopy( colorTable[CT_RED], radColor );
						//	radColor[3] = colorTable[CT_RED][3];
						//}
						//else if ( cgs.clientinfo[cg.snap->entities[i].number].pClass == PC_SCIENCE )
						//{
						//	/*if (z > 64)
						//	{
						//		CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_teal_up);
						//	}
						//	else if (z < -64)
						//	{
						//		CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_teal_down);
						//	}
						//	else
						//	{
						//		CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_teal_level);
						//	}*/
						//	//trap_R_SetColor( colorTable[CT_TEAL] );
						//	VectorCopy( colorTable[CT_TEAL], radColor );
						//	radColor[3] = colorTable[CT_TEAL][3];
						//}
						//else if ( cgs.clientinfo[cg.snap->entities[i].number].pClass == PC_SECURITY )
						//{
						//	/*if (z > 64)
						//	{
						//		CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_blue_up);
						//	}
						//	else if (z < -64)
						//	{
						//		CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_blue_down);
						//	}
						//	else
						//	{
						//		CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_blue_level);
						//	}*/
						//	//trap_R_SetColor( colorTable[CT_GOLD] );
						//	VectorCopy( colorTable[CT_GOLD], radColor );
						//	radColor[3] = colorTable[CT_GOLD][3];
						//}
						//else if ( cgs.clientinfo[cg.snap->entities[i].number].pClass == PC_MEDICAL )
						//{
						//	/*if (z > 64)
						//	{
						//		CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_white_up);
						//	}
						//	else if (z < -64)
						//	{
						//		CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_white_down);
						//	}
						//	else
						//	{
						//		CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_white_level);
						//	}*/
						//	//trap_R_SetColor( colorTable[CT_TEAL] );
						//	VectorCopy( colorTable[CT_TEAL], radColor );
						//	radColor[3] = colorTable[CT_TEAL][3];
						//}
						//else if ( cgs.clientinfo[cg.snap->entities[i].number].pClass == PC_ENGINEER )
						//{
						//	/*if (z > 64)
						//	{
						//		CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_up);
						//	}
						//	else if (z < -64)
						//	{
						//		CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_down);
						//	}
						//	else
						//	{
						//		CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_level);
						//	}*/
						//	//trap_R_SetColor( colorTable[CT_GOLD] );
						//	VectorCopy( colorTable[CT_GOLD], radColor );
						//	radColor[3] = colorTable[CT_GOLD][3];
						//}
						//else if ( cgs.clientinfo[cg.snap->entities[i].number].pClass == PC_ALPHAOMEGA22 )
						//{
						//	/*if (z > 64)
						//	{
						//		CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_teal_up);
						//	}
						//	else if (z < -64)
						//	{
						//		CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_teal_down);
						//	}
						//	else
						//	{
						//		CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_teal_level);
						//	}*/
						//	//trap_R_SetColor( colorTable[CT_GREEN] );
						//	VectorCopy( colorTable[CT_GREEN], radColor );
						//	radColor[3] = colorTable[CT_GREEN][3];
						//}
						//else if ( cgs.clientinfo[cg.snap->entities[i].number].pClass == PC_ADMIN && cg.snap->ps.persistant[PERS_CLASS] != PC_ADMIN )
						//{
						//	//RPG-X: RedTechie - Dont show admins on radar unless you are a admin
						//}
						if ( cgs.clientinfo[cg.snap->entities[i].number].pClass >= 0 )
						{
							radColor[0] = (float)cgs.classData[cgs.clientinfo[cg.snap->entities[i].number].pClass].radarColor[0] / 255.0f;
							radColor[1] = (float)cgs.classData[cgs.clientinfo[cg.snap->entities[i].number].pClass].radarColor[1] / 255.0f;
							radColor[2] = (float)cgs.classData[cgs.clientinfo[cg.snap->entities[i].number].pClass].radarColor[2] / 255.0f;
							radColor[3] = 1.0f;
						}
						else
						{

							/*if (z > 64)
							{
								CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_black_up);
							}
							else if (z < -64)
							{
								CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_black_down);
							}
							else
							{
								CG_DrawPic(86 - forward[1], 146 - forward[0], 7, 7, cgs.media.rd_black_level);
							}*/
							//trap_R_SetColor( colorTable[CT_BLACK] );
							VectorCopy( colorTable[CT_BLACK], radColor );
							radColor[3] = colorTable[CT_BLACK][3];
						}

						if ( cgs.clientinfo[cg.snap->entities[i].number].isAdmin && !cgs.clientinfo[cg.snap->ps.clientNum].isAdmin )
							continue;

						if ( z > 64 ) 
						{
							trap_R_SetColor( radColor );
							CG_DrawStretchPic( 86 - forward[1], 146 - forward[0], 8, 4, 0, 0, 1, 0.5, cgs.media.radarMain );
						}
						else if ( z < -64 )
						{
							trap_R_SetColor( radColor );
							CG_DrawStretchPic( 86 - forward[1], 146 - forward[0], 8, 4, 0, 0.5, 1, 1, cgs.media.radarMain );
						}
						else
						{
							trap_R_SetColor( radColor );
							CG_DrawPic( 86 - forward[1], 146 - forward[0], 8, 8, cgs.media.radarMain );
						}
						trap_R_SetColor( NULL );
					}
				}
			}
		}
	}
	// End Radar
}

/*
================
CG_InterfaceStartup
================
*/
static void CG_InterfaceStartup()
{

	// Turn on Health Graphics
	if ((interface_graphics[IG_HEALTH_START].timer < cg.time) && (interface_graphics[IG_HEALTH_BEGINCAP].type == SG_OFF))
	{
		trap_S_StartLocalSound( cgs.media.interfaceSnd1, CHAN_LOCAL_SOUND );

		interface_graphics[IG_HEALTH_BEGINCAP].type = SG_GRAPHIC;
		interface_graphics[IG_HEALTH_BOX1].type = SG_GRAPHIC;
		interface_graphics[IG_HEALTH_ENDCAP].type = SG_GRAPHIC;
	}

	// Turn on Armor Graphics
	//RPG-X: - RedTechie how many times do i have to say NO ARMOR IN RP's!
	/*if ((interface_graphics[IG_ARMOR_START].timer <	cg.time) && (interface_graphics[IG_ARMOR_BEGINCAP].type == SG_OFF))
	{
		if (interface_graphics[IG_ARMOR_BEGINCAP].type == SG_OFF)
		{
			trap_S_StartLocalSound( cgs.media.interfaceSnd1, CHAN_LOCAL_SOUND );
		}

		interface_graphics[IG_ARMOR_BEGINCAP].type = SG_GRAPHIC;
		interface_graphics[IG_ARMOR_BOX1].type = SG_GRAPHIC;
		interface_graphics[IG_ARMOR_ENDCAP].type = SG_GRAPHIC;

	}*/

	// Turn on Ammo Graphics
	if (interface_graphics[IG_AMMO_START].timer <	cg.time)
	{
		if (interface_graphics[IG_AMMO_UPPER_BEGINCAP].type == SG_OFF)
		{
			trap_S_StartLocalSound( cgs.media.interfaceSnd1, CHAN_LOCAL_SOUND );
			interface_graphics[IG_GROW].type = SG_VAR;
			interface_graphics[IG_GROW].timer = cg.time;
		}

		interface_graphics[IG_AMMO_UPPER_BEGINCAP].type = SG_GRAPHIC;
		interface_graphics[IG_AMMO_UPPER_ENDCAP].type = SG_GRAPHIC;
		interface_graphics[IG_AMMO_LOWER_BEGINCAP].type = SG_GRAPHIC;
		interface_graphics[IG_AMMO_LOWER_ENDCAP].type = SG_GRAPHIC;
	}

	if (interface_graphics[IG_GROW].type == SG_VAR)
	{
		interface_graphics[IG_HEALTH_ENDCAP].x += 2; 
		interface_graphics[IG_ARMOR_ENDCAP].x += 2; 
		interface_graphics[IG_AMMO_UPPER_ENDCAP].x -= 1; 
		interface_graphics[IG_AMMO_LOWER_ENDCAP].x -= 1; 

		if (interface_graphics[IG_HEALTH_ENDCAP].x >= interface_graphics[IG_HEALTH_ENDCAP].max)
		{
			interface_graphics[IG_HEALTH_ENDCAP].x = interface_graphics[IG_HEALTH_ENDCAP].max;
			interface_graphics[IG_ARMOR_ENDCAP].x = interface_graphics[IG_ARMOR_ENDCAP].max;

			interface_graphics[IG_AMMO_UPPER_ENDCAP].x = interface_graphics[IG_AMMO_UPPER_ENDCAP].max;
			interface_graphics[IG_AMMO_LOWER_ENDCAP].x = interface_graphics[IG_AMMO_LOWER_ENDCAP].max;
			interface_graphics[IG_GROW].type = SG_OFF;

			interface_graphics[IG_HEALTH_SLIDERFULL].type = SG_GRAPHIC;
			interface_graphics[IG_HEALTH_SLIDEREMPTY].type = SG_GRAPHIC;
			interface_graphics[IG_HEALTH_COUNT].type = SG_NUMBER;

			interface_graphics[IG_ARMOR_SLIDERFULL].type = SG_GRAPHIC;
			interface_graphics[IG_ARMOR_SLIDEREMPTY].type = SG_GRAPHIC;
			interface_graphics[IG_ARMOR_COUNT].type = SG_NUMBER;

			interface_graphics[IG_AMMO_SLIDERFULL].type = SG_GRAPHIC;
			interface_graphics[IG_AMMO_SLIDEREMPTY].type = SG_GRAPHIC;
			interface_graphics[IG_AMMO_COUNT].type = SG_NUMBER;

			trap_S_StartLocalSound( cgs.media.interfaceSnd1, CHAN_LOCAL_SOUND );
			cg.interfaceStartupDone = 1;	// All done
		}

		interface_graphics[IG_GROW].timer = cg.time + 10;
	}

	cg.interfaceStartupTime = cg.time;

	// kef -- init struct for post game awards
	InitPostGameMenuStruct();
}

/*
===========================================================================================

  UPPER RIGHT CORNER

===========================================================================================
*/

/*
================
CG_DrawAttacker

================
*/
/*static float CG_DrawAttacker( float y ) {
	int			t;
	float		size;
	vec3_t		angles;
	const char	*info;
	const char	*name;
	int			clientNum;

	if ( cg.predictedPlayerState.stats[STAT_HEALTH] <= 0 ) {
		return y;
	}

	if ( !cg.attackerTime ) {
		return y;
	}

	clientNum = cg.predictedPlayerState.persistant[PERS_ATTACKER];
	if ( clientNum < 0 || clientNum >= MAX_CLIENTS || clientNum == cg.snap->ps.clientNum ) {
		return y;
	}

	t = cg.time - cg.attackerTime;
	if ( t > ATTACKER_HEAD_TIME ) {
		cg.attackerTime = 0;
		return y;
	}

	size = ICON_SIZE * 1.25;

	angles[PITCH] = 0;
	angles[YAW] = 180;
	angles[ROLL] = 0;
	CG_DrawHead( 640 - size, y, size, size, clientNum, angles );

	info = CG_ConfigString( CS_PLAYERS + clientNum );
	name = Info_ValueForKey(  info, "n" );
	y += size;
//	CG_DrawBigString( 640 - ( Q_PrintStrlen( name ) * BIGCHAR_WIDTH), y, name, 0.5 );
	UI_DrawProportionalString( 635, y, name, UI_RIGHT | UI_SMALLFONT, colorTable[CT_LTGOLD1] );

	return y + BIGCHAR_HEIGHT + 2;
}*/

/*
==================
CG_DrawSnapshot
==================
*/
static float CG_DrawSnapshot( float y ) {
	char		*s;
	int			w;

	s = va( "time:%i frametime:%i snap:%i cmd:%i", cg.snap->serverTime, cg.frametime, 
		cg.latestSnapshotNum, cgs.serverCommandSequence );

	//y = (BIGCHAR_HEIGHT * 2) + 20;
	

	w = UI_ProportionalStringWidth(s,UI_BIGFONT);
	
	if ( cg_lagometer.integer && ( y < (BIGCHAR_HEIGHT * 2) + 20) ) {
			w = w + 52;
	}

	UI_DrawProportionalString(635 - (w - 2), y + 2, s, UI_BIGFONT, colorTable[CT_LTGOLD1]);

	return y + BIGCHAR_HEIGHT + 10;
}

/*
==================
CG_DrawFPS
==================
*/
#define	FPS_FRAMES	4
static float CG_DrawFPS( float y ) {
	char		*s;
	int			w;
	static int	previousTimes[FPS_FRAMES];
	static int	index;
	int		i, total;
	int		fps;
	static	int	previous;
	int		t, frameTime;

	// don't use serverTime, because that will be drifting to
	// correct for internet lag changes, timescales, timedemos, etc
	t = trap_Milliseconds();
	frameTime = t - previous;
	previous = t;

	previousTimes[index % FPS_FRAMES] = frameTime;
	index++;
	if ( index > FPS_FRAMES ) {
		// average multiple frames together to smooth changes out a bit
		total = 0;
		for ( i = 0 ; i < FPS_FRAMES ; i++ ) {
			total += previousTimes[i];
		}
		if ( !total ) {
			total = 1;
		}
		fps = 1000 * FPS_FRAMES / total;

		s = va( "%ifps", fps );
		w = UI_ProportionalStringWidth(s,UI_BIGFONT);
		//RPG-X | Phenix | 08/06/2005
		// Changed "- w" to "- (w + 50)" to account for lagometer
		if ( !cg_lagometer.integer ) {
			w = w - 52;
		}
		UI_DrawProportionalString(635 - (w + 52), y + 2, s, UI_BIGFONT, colorTable[CT_LTGOLD1]);
	}

	return y + BIGCHAR_HEIGHT + 10;
}

/*
=================
CG_DrawTimer
=================
*/
static float CG_DrawTimer( float y ) {
	char		*s;
	int			w;
	int			mins, seconds, tens;
	int			msec;

	msec = cg.time - cgs.levelStartTime;

	seconds = msec / 1000;
	mins = seconds / 60;
	seconds -= mins * 60;
	tens = seconds / 10;
	seconds -= tens * 10;

	s = va( "%i:%i%i", mins, tens, seconds );

	w = UI_ProportionalStringWidth(s,UI_BIGFONT);
	// RPG-X | Phenix | 08/06/2005
	// Changed "- w" to "- (w + 50)" to account for lagometer
	if ( !cg_lagometer.integer ) {
		w = w - 52;
	}
	UI_DrawProportionalString(635 - (w + 52), y + 2, s, UI_BIGFONT, colorTable[CT_LTGOLD1]);

	return y + BIGCHAR_HEIGHT + 10;
}
#define TINYPAD 1.25

/*
=================
CG_DrawTeamOverlay
=================
*/

#define TEAM_OVERLAY_MAXNAME_WIDTH	12
#define TEAM_OVERLAY_MAXLOCATION_WIDTH	16

static float CG_DrawTeamOverlay( float y, qboolean right, qboolean upper ) {
	int x, w, h, xx;
	int i, j, len;
	const char *p;
	vec4_t		hcolor;
	int pwidth, lwidth;
	int plyrs;
	char st[16];
	clientInfo_t *ci;
	int ret_y;

	if ( !cg_drawTeamOverlay.integer )
	{
		return y;
	}

	if ( cg.snap->ps.persistant[PERS_TEAM] != TEAM_RED && cg.snap->ps.persistant[PERS_TEAM] != TEAM_BLUE )
	{
		return y; // Not on any team
	}

	if ( cg.snap->ps.pm_type == PM_INTERMISSION )
	{
		return y;
	}

	plyrs = 0;
	w = 0;

	// max player name width
	pwidth = 0;
	for (i = 0; i < numSortedTeamPlayers; i++) {
		ci = cgs.clientinfo + sortedTeamPlayers[i];
		if ( ci->infoValid && ci->team == cg.snap->ps.persistant[PERS_TEAM]) {
			plyrs++;
			len = CG_DrawStrlen(ci->name);

			if (len > pwidth)
				pwidth = len;
			if ( ci->pClass >= 0 /*PC_NOCLASS*/ )//if any one of them has a class, then we alloc space for the icon
				w = 1;
		}
	}

	if (!plyrs)
		return y;

	if (pwidth > TEAM_OVERLAY_MAXNAME_WIDTH)
		pwidth = TEAM_OVERLAY_MAXNAME_WIDTH;

	// max location name width
	lwidth = 0;
	for (i = 1; i < MAX_LOCATIONS; i++) {
		p = CG_ConfigString(CS_LOCATIONS + i);
		if (p && *p) {
			len = CG_DrawStrlen(p);
			if (len > lwidth)
				lwidth = len;
		}
	}

	if (lwidth > TEAM_OVERLAY_MAXLOCATION_WIDTH)
		lwidth = TEAM_OVERLAY_MAXLOCATION_WIDTH;

	w += (pwidth + lwidth + 4);
	w *= (TINYCHAR_WIDTH * TINYPAD);

	if ( right )
		x = 640 - w;
	else
		x = 0;

	h = plyrs * (TINYCHAR_HEIGHT * TINYPAD);

	if ( upper ) {
		ret_y = y + h;
	} else {
		y -= h;
		ret_y = y;
	}

	if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED ) {
		hcolor[0] = 1;
		hcolor[1] = 0;
		hcolor[2] = 0;
		hcolor[3] = 0.33;
	} else { // if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE )
		hcolor[0] = 0;
		hcolor[1] = 0;
		hcolor[2] = 1;
		hcolor[3] = 0.33;
	}
	trap_R_SetColor( hcolor );
	CG_DrawPic( x, y, w, h, cgs.media.teamStatusBar );
	trap_R_SetColor( NULL );

	for (i = 0; i < numSortedTeamPlayers; i++) {
		ci = cgs.clientinfo + sortedTeamPlayers[i];
		if ( ci->infoValid && ci->team == cg.snap->ps.persistant[PERS_TEAM]) {

			xx = x + TINYCHAR_WIDTH;
//Draw class icon if appropriate
			if ( ci->pClass >= 0/*PC_NOCLASS*/ )
			{
				//qhandle_t	icon;

				//Special hack: if it's Borg who has regen going, must be Borg queen
				/*if ( ci->pClass == PC_BORG && (ci->powerups&(1<<PW_LASER)) )
				{
					icon = cgs.media.borgQueenIconShader;
				}
				else
				{
					icon = cgs.media.pClassShaders[ci->pClass];
				}
				CG_DrawPic( xx, y, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, icon );*/

				xx += (TINYCHAR_WIDTH * TINYPAD);
			}
//draw name
//			CG_DrawStringExt( xx, y,
//				ci->name, hcolor, qfalse, qfalse,
//				TINYCHAR_WIDTH, TINYCHAR_HEIGHT, TEAM_OVERLAY_MAXNAME_WIDTH);
			hcolor[0] = hcolor[1] = hcolor[2] = hcolor[3] = 1.0;
			UI_DrawProportionalString( xx, y, ci->name, UI_TINYFONT, hcolor);

			if (lwidth) {
				p = CG_ConfigString(CS_LOCATIONS + ci->location);
				if (!p || !*p)
					p = "unknown";
				len = CG_DrawStrlen(p);
				if (len > lwidth)
					len = lwidth;

//				xx = x + TINYCHAR_WIDTH * 2 + TINYCHAR_WIDTH * pwidth + 
//					((lwidth/2 - len/2) * TINYCHAR_WIDTH);
				xx = x + TINYCHAR_WIDTH * 2 + TINYCHAR_WIDTH * pwidth;
//				CG_DrawStringExt( xx, y,
//					p, hcolor, qfalse, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT,
//					TEAM_OVERLAY_MAXLOCATION_WIDTH);
				UI_DrawProportionalString( xx, y, p, UI_TINYFONT, hcolor);

			}

			CG_GetColorForHealth( ci->health, ci->armor, hcolor );

			Com_sprintf (st, sizeof(st), "%3i %3i", ci->health,	ci->armor);

			xx = x + TINYCHAR_WIDTH * 3 + 
				TINYCHAR_WIDTH * pwidth + TINYCHAR_WIDTH * lwidth;

//			CG_DrawStringExt( xx, y,
//				st, hcolor, qfalse, qfalse,
//				TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0 );
			UI_DrawProportionalString( xx, y, st, UI_TINYFONT, hcolor);

			// draw weapon icon
			xx += (TINYCHAR_WIDTH * TINYPAD) * 3;

			if ( cg_weapons[ci->curWeapon].weaponIcon ) {
				CG_DrawPic( xx, y, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 
					cg_weapons[ci->curWeapon].weaponIcon );
			} else {
				CG_DrawPic( xx, y, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 
					cgs.media.deferShader );
			}

			// Draw powerup icons
			if (right) {
				xx = x;
			} else {
				xx = x + w - TINYCHAR_WIDTH;
			}
			for (j = 0; j < PW_NUM_POWERUPS; j++) {
				if (ci->powerups & (1 << j)) {
					gitem_t	*item = BG_FindItemForPowerup( j );

					if (item)
					{
						CG_DrawPic( xx, y, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 
							trap_R_RegisterShader( item->icon ) );
					}
					if (right) {
						xx -= (TINYCHAR_WIDTH * TINYPAD);
					} else {
						xx += (TINYCHAR_WIDTH * TINYPAD);
					}
				}
			}

			y += (TINYCHAR_HEIGHT * TINYPAD);
		}
	}

	return ret_y;
}


/*
=====================
CG_DrawUpperRight

=====================
*/
static void CG_DrawUpperRight( void ) {
	float	y;

	//vec3_t origin = {960, -1214, 242 };
	//vec3_t color = { 0.6, 0.6, 1.0 };

	cgs.widescreen.state = WIDESCREEN_RIGHT;

	y = 0;
	if ( cg_drawFPS.integer ) {
		y = CG_DrawFPS( y );
	}
	if ( cg_drawTimer.integer ) {
		y = CG_DrawTimer( y );
	}

	if ( cg_drawSnapshot.integer ) {
		y = CG_DrawSnapshot( y );
	}

	if ( cgs.gametype >= GT_TEAM && cg_drawTeamOverlay.integer == 1 ) {
		y = CG_DrawTeamOverlay( y, qtrue, qtrue );
	} 

	cgs.widescreen.state = WIDESCREEN_NONE;

/*	if ( cg_drawAttacker.integer ) { //RPG-X - TiM: We don't really need this in an RP
		y = CG_DrawAttacker( y );
	}*/

}

/*
===========================================================================================

  LOWER RIGHT CORNER

===========================================================================================
*/



/*
=================
CG_DrawScores

Draw the small two score display
=================
*/
static float CG_DrawScores( float y ) 
{
//	const char	*s;
	int			s1, s2; //, score;
//	int			x, w;
//	int			v;
//	vec4_t		color;
	float		y1;
//	gitem_t		*item;

	s1 = cgs.scores1;
	s2 = cgs.scores2;

	y -=  BIGCHAR_HEIGHT + 8;
	y1 = y;

	return y1 - 8;
}

/*
================
CG_DrawPowerups
================
*/
static float CG_DrawPowerups( float y ) {
	int		sorted[MAX_POWERUPS];
	int		sortedTime[MAX_POWERUPS];
	int		i, j, k;
	int		active;
	playerState_t	*ps;
	int		t;
	gitem_t	*item;
	int		x;
	int		color;
	float	size;
	float	f;
	static float colors[2][4] = { 
		{ 0.2, 1.0, 0.2, 1.0 } , { 1.0, 0.2, 0.2, 1.0 } };
	int		hasHoldable;

	hasHoldable = cg.snap->ps.stats[STAT_HOLDABLE_ITEM];

	ps = &cg.snap->ps;

	if ( ps->stats[STAT_HEALTH] <= 0 ) {
		return y;
	}

	// sort the list by time remaining
	active = 0;
	for ( i = 0 ; i < MAX_POWERUPS ; i++ ) {
		if ( !ps->powerups[ i ] ) {
			continue;
		}
		t = ps->powerups[ i ] - cg.time;
		// ZOID--don't draw if the power up has unlimited time (999 seconds)
		// This is true of the CTF flags
		if ( t < 0 || t > 999000) {
			continue;
		}

		// insert into the list
		for ( j = 0 ; j < active ; j++ ) {
			if ( sortedTime[j] >= t ) {
				for ( k = active - 1 ; k >= j ; k-- ) {
					sorted[k+1] = sorted[k];
					sortedTime[k+1] = sortedTime[k];
				}
				break;
			}
		}
		sorted[j] = i;
		sortedTime[j] = t;
		active++;
	}

	// draw the icons and timers
	x = 648;
	for ( i = 0 ; i < active ; i++ ) {

		// Don't draw almost timed out powerups if we have more than 3 and a holdable item
		if (!(hasHoldable && i<active-2))
		{
		item = BG_FindItemForPowerup( sorted[i] );

		if (NULL == item)
		{
			continue;
		}
		color = 1;

		y -= ICON_SIZE;

		trap_R_SetColor( colors[color] );
//		CG_DrawField( x, y, 2, sortedTime[ i ] / 1000 );
//		CG_DrawNumField (x,y,2,sortedTime[ i ] / 1000,16,32,NUM_FONT_BIG);

		t = ps->powerups[ sorted[i] ];
		if ( t - cg.time >= POWERUP_BLINKS * POWERUP_BLINK_TIME ) {
			trap_R_SetColor( NULL );
		} else {
			vec4_t	modulate;

			f = (float)( t - cg.time ) / POWERUP_BLINK_TIME;
			f -= (int)f;
			modulate[0] = modulate[1] = modulate[2] = modulate[3] = f;
			trap_R_SetColor( modulate );
		}

		if ( cg.powerupActive == sorted[i] && 
			cg.time - cg.powerupTime < PULSE_TIME ) {
			f = 1.0 - ( ( (float)cg.time - cg.powerupTime ) / PULSE_TIME );
			size = ICON_SIZE * ( 1.0 + ( PULSE_SCALE - 1.0 ) * f );
		} else {
			size = ICON_SIZE;
		}

		//CG_DrawPic( 640 - size, y + ICON_SIZE / 2 - size / 2, 
		//	size, size, trap_R_RegisterShader( item->icon ) );
		x -= size + 10;

		CG_DrawPic( x, 478 - size, 
			size, size, trap_R_RegisterShader( item->icon ) );
		}
	}
	trap_R_SetColor( NULL );

	return y;
	
}


/*
=====================
CG_DrawLowerRight

=====================
*/
static void CG_DrawLowerRight( void ) {
	float	y;

	y = LOWEROVERLAY_Y;

	cgs.widescreen.state = WIDESCREEN_RIGHT;

	if ( cgs.gametype >= GT_TEAM && cg_drawTeamOverlay.integer == 2 ) {
		y = CG_DrawTeamOverlay( y, qtrue, qfalse );
	} 

	y = CG_DrawScores( y );
	y = CG_DrawPowerups( y );

	cgs.widescreen.state = WIDESCREEN_NONE;
}

/*
===================
CG_DrawPickupItem
===================
*/
static int CG_DrawPickupItem( int y ) {
	int		value;
	float	*fadeColor;

	if ( cg.snap->ps.stats[STAT_HEALTH] <= 0 ) {
		return y;
	}

	y -= ICON_SIZE;

	value = cg.itemPickup;
	if ( value ) {
		fadeColor = CG_FadeColor( cg.itemPickupTime, 3000 );
		if ( fadeColor ) {
			CG_RegisterItemVisuals( value );
			trap_R_SetColor( fadeColor );
			CG_DrawPic( 8, y, ICON_SIZE, ICON_SIZE, cg_items[ value ].icon );
			UI_DrawProportionalString( ICON_SIZE + 16, y + (ICON_SIZE/2 - BIGCHAR_HEIGHT/2), bg_itemlist[ value ].pickup_name, UI_SMALLFONT, fadeColor);

//			CG_DrawBigString( ICON_SIZE + 16, y + (ICON_SIZE/2 - BIGCHAR_HEIGHT/2), bg_itemlist[ value ].pickup_name, fadeColor[0] );
			trap_R_SetColor( NULL );
		}
	}
	
	return y;
}

/*
=====================
CG_DrawLowerLeft

=====================
*/
static void CG_DrawLowerLeft( void ) {
	float	y;

	y = LOWEROVERLAY_Y;

	cgs.widescreen.state = WIDESCREEN_LEFT;

	if ( cgs.gametype >= GT_TEAM && cg_drawTeamOverlay.integer == 3 ) {
		y = CG_DrawTeamOverlay( y, qfalse, qfalse );
	} 


	y = CG_DrawPickupItem( y );

	cgs.widescreen.state = WIDESCREEN_NONE;
}



//===========================================================================================

/*
=================
CG_DrawTeamInfo
=================
*/
static void CG_DrawTeamInfo( void ) {
	int w, h;
	int i, len;
	vec4_t		hcolor;
	int		chatHeight;

#define CHATLOC_Y 420 // bottom end
#define CHATLOC_X 0

	if (cg_teamChatHeight.integer < TEAMCHAT_HEIGHT)
		chatHeight = cg_teamChatHeight.integer;
	else
		chatHeight = TEAMCHAT_HEIGHT;
	if (chatHeight <= 0)
		return; // disabled

	if (cgs.teamLastChatPos != cgs.teamChatPos) {
		if (cg.time - cgs.teamChatMsgTimes[cgs.teamLastChatPos % chatHeight] > cg_teamChatTime.integer) {
			cgs.teamLastChatPos++;
		}

		h = (cgs.teamChatPos - cgs.teamLastChatPos) * TINYCHAR_HEIGHT;

		w = 0;

		for (i = cgs.teamLastChatPos; i < cgs.teamChatPos; i++) {
			len = CG_DrawStrlen(cgs.teamChatMsgs[i % chatHeight]);
			if (len > w)
				w = len;
		}
		w *= TINYCHAR_WIDTH;
		w += TINYCHAR_WIDTH * 2;

		if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED ) {
			hcolor[0] = 1;
			hcolor[1] = 0;
			hcolor[2] = 0;
			hcolor[3] = 0.33;
		} else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE ) {
			hcolor[0] = 0;
			hcolor[1] = 0;
			hcolor[2] = 1;
			hcolor[3] = 0.33;
		} else {
			hcolor[0] = 0;
			hcolor[1] = 1;
			hcolor[2] = 0;
			hcolor[3] = 0.33;
		}

		trap_R_SetColor( hcolor );
		CG_DrawPic( CHATLOC_X, CHATLOC_Y - h, 640, h, cgs.media.teamStatusBar );
		trap_R_SetColor( NULL );

		hcolor[0] = hcolor[1] = hcolor[2] = 1.0;
		hcolor[3] = 1.0;

		for (i = cgs.teamChatPos - 1; i >= cgs.teamLastChatPos; i--) {
//			CG_DrawStringExt( CHATLOC_X + TINYCHAR_WIDTH, 
//				CHATLOC_Y - (cgs.teamChatPos - i)*TINYCHAR_HEIGHT, 
//				cgs.teamChatMsgs[i % chatHeight], hcolor, qfalse, qfalse,
//				TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0 );
			UI_DrawProportionalString( CHATLOC_X + TINYCHAR_WIDTH, 
				CHATLOC_Y - (cgs.teamChatPos - i)*TINYCHAR_HEIGHT, 
				cgs.teamChatMsgs[i % chatHeight], UI_TINYFONT, hcolor);

		}
	}
}

/*
===================
CG_DrawHoldableItem
===================
*/
static void CG_DrawHoldableItem( void ) { 
	int		value;

	value = cg.snap->ps.stats[STAT_HOLDABLE_ITEM];
	if ( value )
	{
		CG_RegisterItemVisuals( value );
		if ( cg.snap->ps.stats[STAT_USEABLE_PLACED] && cg.snap->ps.stats[STAT_USEABLE_PLACED] != 2 )
		{//draw detpack... Borg 2-part teleporter will just draw the same until done
			CG_DrawPic( 640-ICON_SIZE, 480-ICON_SIZE, ICON_SIZE, ICON_SIZE, cgs.media.detpackPlacedIcon );
		}
		else
		{
			CG_DrawPic( 640-ICON_SIZE, 480-ICON_SIZE, ICON_SIZE, ICON_SIZE, cg_items[ value ].icon );
		}
	}
	else
	{//holding nothing...
		if ( cg.snap->ps.stats[STAT_USEABLE_PLACED] > 0 )
		{//it's a timed countdown to getting a holdable, display the number in seconds
			int		sec;
			char	*s;
			int		w;

			sec = cg.snap->ps.stats[STAT_USEABLE_PLACED];

			if ( sec < 0 )
			{
				sec = 0;
			}

			s = va( "%i", sec );

			w = UI_ProportionalStringWidth(s,UI_BIGFONT);
			UI_DrawProportionalString(640-(ICON_SIZE/2)-(w/2), (SCREEN_HEIGHT-ICON_SIZE)/2+(BIGCHAR_HEIGHT/2), s, UI_BIGFONT, colorTable[CT_WHITE]);
		}
	}
}


/*
===================
CG_DrawReward
===================
*/
static void CG_DrawReward( void ) { 
	float	*color;
	int		i;
	float	x, y;

	if ( !cg_drawRewards.integer ) {
		return;
	}
	color = CG_FadeColor( cg.rewardTime, REWARD_TIME );
	if ( !color ) {
		return;
	}

	trap_R_SetColor( color );
	y = 56;
	x = 320 - cg.rewardCount * ICON_SIZE/2;
	for ( i = 0 ; i < cg.rewardCount ; i++ ) {
		CG_DrawPic( x, y, ICON_SIZE-4, ICON_SIZE-4, cg.rewardShader );
		x += ICON_SIZE;
	}
	trap_R_SetColor( NULL );
}


/*
===============================================================================

LAGOMETER

===============================================================================
*/

#define	LAG_SAMPLES		128


typedef struct {
	int		frameSamples[LAG_SAMPLES];
	int		frameCount;
	int		snapshotFlags[LAG_SAMPLES];
	int		snapshotSamples[LAG_SAMPLES];
	int		snapshotCount;
} lagometer_t;

lagometer_t		lagometer;

/*
==============
CG_AddLagometerFrameInfo

Adds the current interpolate / extrapolate bar for this frame
==============
*/
void CG_AddLagometerFrameInfo( void ) {
	int			offset;

	offset = cg.time - cg.latestSnapshotTime;
	lagometer.frameSamples[ lagometer.frameCount & ( LAG_SAMPLES - 1) ] = offset;
	lagometer.frameCount++;
}

/*
==============
CG_AddLagometerSnapshotInfo

Each time a snapshot is received, log its ping time and
the number of snapshots that were dropped before it.

Pass NULL for a dropped packet.
==============
*/
void CG_AddLagometerSnapshotInfo( snapshot_t *snap ) {
	// dropped packet
	if ( !snap ) {
		lagometer.snapshotSamples[ lagometer.snapshotCount & ( LAG_SAMPLES - 1) ] = -1;
		lagometer.snapshotCount++;
		return;
	}

	// add this snapshot's info
	lagometer.snapshotSamples[ lagometer.snapshotCount & ( LAG_SAMPLES - 1) ] = snap->ping;
	lagometer.snapshotFlags[ lagometer.snapshotCount & ( LAG_SAMPLES - 1) ] = snap->snapFlags;
	lagometer.snapshotCount++;
}

/*
==============
CG_DrawDisconnect

Should we draw something differnet for long lag vs no packets?
==============
*/
static void CG_DrawDisconnect( void ) {
	float		x, y;
	int			cmdNum;
	usercmd_t	cmd;
	const char		*s;
	int			w;

	// draw the phone jack if we are completely past our buffers
	cmdNum = trap_GetCurrentCmdNumber() - CMD_BACKUP + 1;
	trap_GetUserCmd( cmdNum, &cmd );
	if (	cmd.serverTime <= cg.snap->ps.commandTime	||
			cmd.serverTime > cg.time					/*||	// special check for map_restart
			cmd.serverTime < cg.snap->ps.introTime*/)			// special check for holointro
	{	
		return;
	}

	// also add text in center of screen
	s = ingame_text[IGT_CONNECTIONINTERRUPTED];
//	w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
	w = UI_ProportionalStringWidth(s,UI_BIGFONT);
//	CG_DrawBigString( 320 - w/2, 100, s, 1.0F);
	// Used to be (Height) 100
	UI_DrawProportionalString(320 - w/2, 240, s, UI_BIGFONT, colorTable[CT_LTGOLD1]);

	// blink the icon
	if ( ( cg.time >> 9 ) & 1 ) {
		return;
	}

	// RPG-X | Phenix | 08/06/2005
	x = 296; //640 - 50;
	y = 182;

	CG_DrawPic( x, y, 48, 48, trap_R_RegisterShader("gfx/2d/net.tga" ) );
}


#define	MAX_LAGOMETER_PING	900
#define	MAX_LAGOMETER_RANGE	300

/*
==============
CG_DrawLagometer
==============
*/
static void CG_DrawLagometer( void ) {
	int		a, x, y, i;
	float	v;
	float	ax, ay, aw, ah, mid, range;
	int		color;
	float	vscale;

	if ( !cg_lagometer.integer /* || cgs.localServer */) {
		CG_DrawDisconnect();
		return;
	}

	//
	// draw the graph
	//
	// 640, 480 (-48)
	x = 640 - 50;	//move it left of the ammo numbers
	y = 2;

	trap_R_SetColor( NULL );
	CG_DrawPic( x, y, 48, 48, cgs.media.lagometerShader );

	ax = x;
	ay = y;
	aw = 48;
	ah = 48;
	CG_AdjustFrom640( &ax, &ay, &aw, &ah );

	color = -1;
	range = ah / 3;
	mid = ay + range;

	vscale = range / MAX_LAGOMETER_RANGE;

	// draw the frame interpoalte / extrapolate graph
	for ( a = 0 ; a < aw ; a++ ) {
		i = ( lagometer.frameCount - 1 - a ) & (LAG_SAMPLES - 1);
		v = lagometer.frameSamples[i];
		v *= vscale;
		if ( v > 0 ) {
			if ( color != 1 ) {
				color = 1;
				trap_R_SetColor( g_color_table[ColorIndex(COLOR_YELLOW)] );
			}
			if ( v > range ) {
				v = range;
			}
			trap_R_DrawStretchPic ( ax + aw - a, mid - v, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
		} else if ( v < 0 ) {
			if ( color != 2 ) {
				color = 2;
				trap_R_SetColor( g_color_table[ColorIndex(COLOR_BLUE)] );
			}
			v = -v;
			if ( v > range ) {
				v = range;
			}
			trap_R_DrawStretchPic( ax + aw - a, mid, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
		}
	}

	// draw the snapshot latency / drop graph
	range = ah / 2;
	vscale = range / MAX_LAGOMETER_PING;

	for ( a = 0 ; a < aw ; a++ ) {
		i = ( lagometer.snapshotCount - 1 - a ) & (LAG_SAMPLES - 1);
		v = lagometer.snapshotSamples[i];
		if ( v > 0 ) {
			if ( lagometer.snapshotFlags[i] & SNAPFLAG_RATE_DELAYED ) {
				if ( color != 5 ) {
					color = 5;	// YELLOW for rate delay
					trap_R_SetColor( g_color_table[ColorIndex(COLOR_YELLOW)] );
				}
			} else {
				if ( color != 3 ) {
					color = 3;
					trap_R_SetColor( g_color_table[ColorIndex(COLOR_GREEN)] );
				}
			}
			v = v * vscale;
			if ( v > range ) {
				v = range;
			}
			trap_R_DrawStretchPic( ax + aw - a, ay + ah - v, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
		} else if ( v < 0 ) {
			if ( color != 4 ) {
				color = 4;		// RED for dropped snapshots
				trap_R_SetColor( g_color_table[ColorIndex(COLOR_RED)] );
			}
			trap_R_DrawStretchPic( ax + aw - a, ay + ah - range, 1, range, 0, 0, 0, 0, cgs.media.whiteShader );
		}
	}

	trap_R_SetColor( NULL );

	if ( cg_nopredict.integer || cg_synchronousClients.integer ) {
//		CG_DrawBigString( ax, ay, "snc", 1.0 );
		UI_DrawProportionalString(ax, ay, "snc", UI_BIGFONT, colorTable[CT_LTGOLD1]);
	}

	CG_DrawDisconnect();
}



/*
===============================================================================

CENTER PRINTING

===============================================================================
*/


/*
==============
CG_CenterPrint

Called for important messages that should stay in the center of the screen
for a few moments
==============
*/
void CG_CenterPrint( const char *str, int y, int charWidth ) {
	char	*s;

	Q_strncpyz( cg.centerPrint, str, sizeof(cg.centerPrint) );

	cg.centerPrintTime = cg.time;
	cg.centerPrintY = y;
	cg.centerPrintCharWidth = charWidth;

	// count the number of lines for centering
	cg.centerPrintLines = 1;
	s = cg.centerPrint;
	while( *s ) {
		if (*s == '\n')
			cg.centerPrintLines++;
		s++;
	}
}


/*
===================
CG_DrawCenterString
===================
*/
static void CG_DrawCenterString( void ) {
	char	*start;
	int		l;
	int		x, y, w;
	float	*color;

	if ( !cg.centerPrintTime ) {
		return;
	}

	color = CG_FadeColor( cg.centerPrintTime, 1000 * cg_centertime.value );
	if ( !color ) {
		return;
	}

	trap_R_SetColor( color );

	start = cg.centerPrint;

	y = cg.centerPrintY - cg.centerPrintLines * BIGCHAR_HEIGHT / 2;

	while ( 1 ) {
		char linebuffer[1024];

		for ( l = 0; l < 60; l++ ) {
			if ( !start[l] || start[l] == '\n' ) {
				break;
			}
			linebuffer[l] = start[l];
		}
		linebuffer[l] = 0;

//		w = cg.centerPrintCharWidth * CG_DrawStrlen( linebuffer );
		w = UI_ProportionalStringWidth(linebuffer,UI_BIGFONT);

		x = ( SCREEN_WIDTH - w ) / 2;

//		CG_DrawStringExt( x, y, linebuffer, color, qfalse, qtrue,
//			cg.centerPrintCharWidth, (int)(cg.centerPrintCharWidth * 1.5), 0 );

		UI_DrawProportionalString( x, y, linebuffer, UI_BIGFONT|UI_DROPSHADOW, color);

		y += cg.centerPrintCharWidth * 1.5;

		while ( *start && ( *start != '\n' ) ) {
			start++;
		}
		if ( !*start ) {
			break;
		}
		start++;
	}

	trap_R_SetColor( NULL );
}



/*
================================================================================

CROSSHAIR

================================================================================
*/

/*qboolean CG_WorldCoordToScreenCoordFloat(vec3_t worldCoord, float *x, float *y)
{
	float	xcenter, ycenter;
	vec3_t	local, transformed;
	vec3_t	vfwd;
	vec3_t	vright;
	vec3_t	vup;
	float xzi;
	float yzi;

//	xcenter = cg.refdef.width / 2;//gives screen coords adjusted for resolution
//	ycenter = cg.refdef.height / 2;//gives screen coords adjusted for resolution
	
	//NOTE: did it this way because most draw functions expect virtual 640x480 coords
	//	and adjust them for current resolution
	xcenter = 640.0f / 2.0f;//gives screen coords in virtual 640x480, to be adjusted when drawn
	ycenter = 480.0f / 2.0f;//gives screen coords in virtual 640x480, to be adjusted when drawn

	AngleVectors (cg.refdefViewAngles, vfwd, vright, vup);

	VectorSubtract (worldCoord, cg.refdef.vieworg, local);

	transformed[0] = DotProduct(local,vright);
	transformed[1] = DotProduct(local,vup);
	transformed[2] = DotProduct(local,vfwd);		

	// Make sure Z is not negative.
	if(transformed[2] < 0.01f)
	{
		return qfalse;
	}

	xzi = xcenter / transformed[2] * (96.0f/cg.refdef.fov_x);
	yzi = ycenter / transformed[2] * (102.0f/cg.refdef.fov_y);

	*x = xcenter + xzi * transformed[0];
	*y = ycenter - yzi * transformed[1];

	return qtrue;
}*/

/*float cg_crosshairPrevPosX = 0;
float cg_crosshairPrevPosY = 0;
#define CRAZY_CROSSHAIR_MAX_ERROR_X	(100.0f*640.0f/480.0f)
#define CRAZY_CROSSHAIR_MAX_ERROR_Y	(100.0f)
void CG_LerpCrosshairPos( float *x, float *y )
{
	if ( cg_crosshairPrevPosX )
	{//blend from old pos
		float maxMove = 100.0f * ((float)cg.frametime/500.0f) * 640.0f/480.0f; //30
		float xDiff = (*x - cg_crosshairPrevPosX);
		if ( fabs(xDiff) > CRAZY_CROSSHAIR_MAX_ERROR_X )
		{
			maxMove = CRAZY_CROSSHAIR_MAX_ERROR_X;
		}
		if ( xDiff > maxMove )
		{
			*x = cg_crosshairPrevPosX + maxMove;
		}
		else if ( xDiff < -maxMove )
		{
			*x = cg_crosshairPrevPosX - maxMove;
		}
	}
	cg_crosshairPrevPosX = *x;

	if ( cg_crosshairPrevPosY )
	{//blend from old pos
		float maxMove = 100.0f * ((float)cg.frametime/500.0f);
		float yDiff = (*y - cg_crosshairPrevPosY);
		if ( fabs(yDiff) > CRAZY_CROSSHAIR_MAX_ERROR_Y )
		{
			maxMove = CRAZY_CROSSHAIR_MAX_ERROR_X;
		}
		if ( yDiff > maxMove )
		{
			*y = cg_crosshairPrevPosY + maxMove;
		}
		else if ( yDiff < -maxMove )
		{
			*y = cg_crosshairPrevPosY - maxMove;
		}
	}
	cg_crosshairPrevPosY = *y;
}*/

/*
=================
CG_CalcMuzzlePoint
**Blatently plagiarised from JKA**

Um, I guess this calculates the approximate vector
of where your gun is at ingame. :P 
=================*/


//static qboolean	CG_CalcMuzzlePoint( int entityNum, vec3_t muzzle ) {
//	vec3_t		forward, right;
//	vec3_t		gunpoint;
//	centity_t	*cent;
//	int			anim;
//
//	if ( entityNum == cg.snap->ps.clientNum )
//	{ //I'm not exactly sure why we'd be rendering someone else's crosshair, but hey.
//		int weapontype = cg.snap->ps.weapon;
//		vec3_t weaponMuzzle = {13, 6, -6};
//		centity_t *pEnt = &cg_entities[cg.predictedPlayerState.clientNum];
//
//		if (cg.renderingThirdPerson)
//		{
//			VectorCopy( pEnt->lerpOrigin, gunpoint ); //lerp
//			AngleVectors( pEnt->lerpAngles, forward, right, NULL );
//		}
//		/*else
//		{
//			VectorCopy( cg.refdef.vieworg, gunpoint );
//			AngleVectors( cg.refdefViewAngles, forward, right, NULL );
//		}*/
//
//		VectorCopy(gunpoint, muzzle);
//
//		VectorMA(muzzle, weaponMuzzle[0], forward, muzzle);
//		VectorMA(muzzle, weaponMuzzle[1], right, muzzle);
//
//		if (cg.renderingThirdPerson)
//		{
//			muzzle[2] += cg.snap->ps.viewheight + weaponMuzzle[2];
//		}
//		/*else
//		{
//			muzzle[2] += weaponMuzzle[2];
//		}*/
//
//		return qtrue;
//	}
//
//	cent = &cg_entities[entityNum];
//	if ( !cent->currentValid ) {
//		return qfalse;
//	}
//
//	VectorCopy( cent->currentState.pos.trBase, muzzle );
//
//	AngleVectors( cent->currentState.apos.trBase, forward, NULL, NULL );
//	anim = cent->currentState.legsAnim;
//	if ( anim == BOTH_CROUCH1IDLE || anim == BOTH_CROUCH1WALK ) {
//		muzzle[2] += CROUCH_VIEWHEIGHT;
//	} else {
//		muzzle[2] += DEFAULT_VIEWHEIGHT;
//	}
//
//	VectorMA( muzzle, 14, forward, muzzle );
//
//	return qtrue;
//
//
//}

//end dCross

/*
=================
CG_DrawCrosshair
=================
*/
static void CG_DrawCrosshair(void) {
	float		w, h;
	//qhandle_t	hShader;
	float		f;
	float		x = 0;
	float		y = 0; //float
	int			weaponCrosshairNum;
	
	//dCross
	trace_t		trace;
	vec3_t		start, end;
	int			ignore;
	vec3_t		d_f;
	vec3_t		pitchConstraint;
	vec3_t		worldPoint;

	crosshairsData_t	*cd;

	if( cg.zoomed ) { //RPG-X - TiM: We dun need crosshairs when zoomed anymore :P
		return;
	}

	if ( !cg_drawCrosshair.integer ) {
		return;
	}

	if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR /*|| (cg.snap->ps.eFlags&EF_ELIMINATED)*/ ) {
		return;
	}

	//clamp crosshair num
	if ( (weaponCrosshairNum = cg.predictedPlayerState.weapon - 1) < 0 ){
		weaponCrosshairNum = 0;
	}
	else if ( weaponCrosshairNum >= MAX_CROSSHAIRS ) {
		weaponCrosshairNum = 14;
	}

	cd = &cgs.crosshairsData[weaponCrosshairNum];

	ignore = cg.predictedPlayerState.clientNum;

	//if noDraw was specified in the crosshair script
	if ( cd->noDraw ) {
		return;
	}

	//TiM: With the new crosshair rendering system, this should be no problem
	/*if ( cg.snap->ps.weapon == WP_1 ) { //Teh hand has no crosshair
		return;
	}*/

//	if ( cg.renderingThirdPerson ) {
//		return;
//	}

//We don't need this anymore (RPG-X: J2J)
/*
	// set color based on health
	if ( cg_crosshairHealth.integer ) {
		vec4_t		hcolor;

		CG_ColorForHealth( hcolor );
		trap_R_SetColor( hcolor );
	} else {
		trap_R_SetColor( NULL );
//	}*/

	w = h = cg_crosshairSize.value;

	// pulse the size of the crosshair when picking up items
	f = cg.time - cg.itemPickupBlendTime;
	if ( f > 0 && f < ITEM_BLOB_TIME ) {
		f /= ITEM_BLOB_TIME;
		w *= ( 1 + f );
		h *= ( 1 + f );
	}

	//dCross

	if( cg_dynamicCrosshair.value == 1 && cg.renderingThirdPerson) {

		//if ( cg.renderingThirdPerson ) {
			VectorCopy( cg.predictedPlayerState.viewangles, pitchConstraint); //cg.predictedPlayerState.viewangles //cg.refdefViewAngles //vieworg
		//}
		/*else
		{
			VectorCopy(cg.refdefViewAngles, pitchConstraint);
		}*/

		AngleVectors( pitchConstraint, d_f, NULL, NULL );

		//CG_CalcMuzzlePoint(cg.snap->ps.clientNum, start);
		//if ( cg.renderingThirdPerson ) {
			VectorCopy( cg.predictedPlayerState.origin, start);
			if ( !(cg.predictedPlayerState.eFlags & EF_FULL_ROTATE) && Q_fabs( cg.predictedPlayerState.viewangles[PITCH] ) > 89.9f )
				start[2] -= 20;
			else
				start[2] += (float)cg.predictedPlayerState.viewheight * cgs.clientinfo[cg.predictedPlayerState.clientNum].height;
		//}
	//	else {
		//	VectorCopy( cg.refdef.vieworg, start);
	//	}

		VectorMA( start, 6000.0f, d_f, end ); //cg.distanceCull

		CG_Trace( &trace, start, vec3_origin, vec3_origin, end, ignore, CONTENTS_SOLID|CONTENTS_BODY );

		//TiM - if we hit a cloaked admin, bypass them so the crosshair doesn't jump randomly
		//NOTE: Possibly could cause errors
		while ( cg_entities[trace.entityNum].currentState.powerups & ( 1 <<PW_INVIS ) ) {
			VectorMA( trace.endpos, 6000.0f, d_f, end ); //cg.distanceCull		
			CG_Trace( &trace, trace.endpos, vec3_origin, vec3_origin, end, trace.entityNum, CONTENTS_SOLID|CONTENTS_BODY );
		}

		VectorCopy( trace.endpos, worldPoint ); 
		
		if ( VectorLength( worldPoint ) ) {
			if ( !CG_WorldCoordToScreenCoord( worldPoint, &x, &y, qfalse) )
			{//off screen, don't draw it
				return;
			}

			//CG_LerpCrosshairPos( &x, &y );
			x -= 320;
			y -= 240;
		}
	} else {
		x = cg_crosshairX.integer;
		y = cg_crosshairY.integer;
	}

	//end dCross

	//RPG-X: J2J - This picks which crosshair to draw depending on the current weapon.
	//TiM: Re-optimized so it'll only use one texture slot for the whole lot. :)
	/*switch(cg.snap->ps.weapon)
	{
		default:
		case WP_5: hShader = cgs.media.crosshair[0]; break;							
		case WP_6: hShader = cgs.media.crosshair[1]; break;
		case WP_1: hShader = cgs.media.crosshair[4]; break;								
		case WP_4: hShader = cgs.media.crosshair[2]; break;		
		case WP_10: hShader = cgs.media.crosshair[3]; break;							
		case WP_8: hShader = cgs.media.crosshair[6]; break;
		case WP_7: hShader = cgs.media.crosshair[5]; break;
		case WP_9: hShader = cgs.media.crosshair[7]; break;			
		case WP_13: hShader = cgs.media.crosshair[8]; break;				
		case WP_12: hShader = cgs.media.crosshair[9]; break;			
		case WP_14: hShader = cgs.media.crosshair[11]; break;		
		case WP_11: hShader = cgs.media.crosshair[10]; break;				
		case WP_2: hShader = cgs.media.crosshair[14]; break;					
		case WP_3: hShader = cgs.media.crosshair[13]; break;								
		case WP_NEUTRINO_PROBE: hShader = cgs.media.crosshair[12]; break;
	}*/

	//If admins scan non-players
	if ( cg.predictedPlayerState.weapon == WP_2 && cg.predictedPlayerState.eFlags & EF_FIRING ) {
		if (/*cg.predictedPlayerState.persistant[PERS_CLASS] == PC_ADMIN*/cg_showEntityNums.integer && cgs.clientinfo[cg.snap->ps.clientNum].isAdmin && cg.crosshairClientNum < ENTITYNUM_WORLD ) {
			vec4_t ccolor;
			/*color[0] = colorTable[CT_YELLOW][0];
			color[1] = colorTable[CT_YELLOW][1];
			color[2] = colorTable[CT_YELLOW][2];*/
			/*color[0] = 0.9F;//R
			color[1] = 0.7F;//G
			color[2] = 0.0F;//B
			color[3] = 0.8;*/
			ccolor[0] = 0.694f;//0.9F;//R
			ccolor[1] = 0.816f;//0.7F;//G
			ccolor[2] = 1.0f;//0.0F;//B
			ccolor[3] = 0.8f;

			//TiM
			cgs.widescreen.state = WIDESCREEN_CENTER;
			
			UI_DrawProportionalString(x + 320,
										y + 270, 
										va("Entity: %i", cg.crosshairClientNum), 
										UI_CENTER|UI_SMALLFONT, 
										ccolor); //170

			//CG_Printf( "x= %i, y = %i, w = %i, h = %i\n", cg.refdef.x, cg.refdef.y, cg.refdef.width, cg.refdef.height );
		}
		/*if(cg_entities[cg.crosshairClientNum].currentState.modelindex == HI_SHIELD && cg_entities[cg.crosshairClientNum].currentState.apos.trBase[0] != 0) {
			vec4_t	ccolor;
			ccolor[0] = 0.694f;
			ccolor[1] = 0.816f;
			ccolor[2] = 1.0f;
			ccolor[3] = 0.8f;
			UI_DrawProportionalString(x + 320,
										y + 285,
										va("Frequency: %f", cg_entities[cg.crosshairClientNum].currentState.apos.trBase[0]),
										UI_CENTER|UI_SMALLFONT,
										ccolor);
		}*/
	}

	cgs.widescreen.state = WIDESCREEN_LEFT;
	CG_AdjustFrom640( &x, &y, &w, &h );

	trap_R_SetColor( cd->color );
	
	//TiM: Huh... we have a problem cap'n.
	//Even though we have absolutely perfect alignment, the ingame drawing (regardless of mipmapping)
	//appears to be blurring the icons to the point where they overlap, leaving little smudges at the corners
	//of certain crosshairs ingame :'(
	//So I'm attempting to fix this by creating a very very subtle offset to scale the scan region inwards a bit.
	//Addendum: FRAK! Okay... offsetting will not work.  It clips any of the hairs that are in their full boundary. Which looks crap :P
	//Com_Printf("s1 = %f, t1 = %f, s2 = %f, t2 = %f\n", ((float)cd->s1/128.0f), ((float)cd->t1/128.0f), ((float)cd->s2/128.0f), ((float)cd->t2/128.0f));

	//Magic number! 0.0078125 = 1 pixel in a 128x128 bitmap - Edited out.  1 pixel = WAY TOO MUCH!
	trap_R_DrawStretchPic( x + cg.refdef.x + 0.5 * (cg.refdef.width - w),	//X
							y + cg.refdef.y + 0.5 * (cg.refdef.height - h), //Y
							w, h,											//W+H
							((float)cd->s1/128.0f), ((float)cd->t1/128.0f), //s1 + t1
							((float)cd->s2/128.0f), ((float)cd->t2/128.0f), //s2 + t2
							cgs.media.crosshairSheet );

	trap_R_SetColor( NULL );
}

/*
=================
CG_LabelCrosshairEntity
=================
*/

static void CG_LabelViewEntity( int clientNum, vec3_t origin, vec3_t entMins, vec3_t entMaxs, char *name, qboolean scanAll, vec4_t color, qboolean drawHealth, int health, char *pClass, char *rank, char *race, char* age, char *height, char *weight, char *weapon ) 
{//ID teammates, ID enemies, ID objectives, etc.
	centity_t		*cent;
	//clientInfo_t	*ci;
	vec3_t			center, maxs, mins, top, bottom, topLeft, topRight, bottomLeft, bottomRight;
	vec3_t			worldEast = {1.0f, 0, 0}, worldNorth = {0, 1.0f, 0}, worldUp = {0, 0, 1.0f};
	//vec4_t			hcolor;
	float			x = 0, y = 0; 
	float			topLeftx, topLefty, topRightx, topRighty, bottomLeftx, bottomLefty, bottomRightx, bottomRighty;
	int				corner, topSize, bottomSize, leftSize, rightSize;
	int				charIndex, classCharIndex, rankCharIndex, ageCharIndex, raceCharIndex, htCharIndex, wtCharIndex, weapCharIndex, healthCharIndex;
	float			lineHorzLength = 8.0f, lineVertLength = 8.0f, lineWidth = 2.0f;
	float			fUpDot, fEastDot, fNorthDot, uNorthDot, uEastDot;//, hwidth;//, timedScale = 1.0f;
	qboolean		doTopLeft = qfalse;
	qboolean		doTopRight = qfalse;
	qboolean		doBottomLeft = qfalse;
	qboolean		doBottomRight = qfalse;
	qboolean		doSizes = qtrue;
	float			w;
	char			showName[1024];
	char			showRank[1024];
	char			showRace[1024];
	char			showHt[1024];
	char			showWt[1024];
	char			showWeap[1024];
	char			showHealth[1024];
	char			showAge[1024];
	char			showClass[1024];
	//char			*health = "100";

	cent = &cg_entities[clientNum];
	
	/*if ( clientNum < MAX_CLIENTS ) {  
		ci = &cgs.clientinfo[clientNum];
	}*/

	infoStringCount += cg.frametime;
	rankCharIndex = raceCharIndex = classCharIndex = ageCharIndex = htCharIndex = wtCharIndex = weapCharIndex = charIndex = healthCharIndex = floor(infoStringCount/33);
	//TODO: have box scale in from corners of screen?  Or out from center?
	/*
	if(infoStringCount < 1000)
	{
		timedScale = (float)infoStringCount/100.0f;
		timedScale = 10.0f - timedScale;
		if(timedScale < 1.0f)
		{
			timedScale = 1.0f;
		}
	}
	*/
	//IDEA:  We COULD actually rotate a wire-mesh version of the crossEnt until it
	//			matches the crossEnt's angles then flash it and pop up this info...
	//			but that would be way too much work for something like this.
	//			Alternately, could rotate a scaled-down fully-skinned version
	//			next to it, but that, too, might be overkill... (plus, model would
	//			need back faces)

	//FIXME: can be optimized...

	//Draw frame around ent's bbox
	//FIXME: make global, do once
	fUpDot = 1.0f - fabs( DotProduct( vfwd_n, worldUp ) );//1.0 if looking up or down, so use mins and maxs more
	fEastDot = fabs( DotProduct( vfwd_n, worldEast ) );//1.0 if looking east or west, so use mins[1] and maxs[1] more
	fNorthDot = fabs( DotProduct( vfwd_n, worldNorth ) );//1.0 if looking north or south, so use mins[0] and maxs[0] more
	uEastDot = fabs( DotProduct( vup_n, worldEast ) );//1.0 if looking up or down, head towards east or west, so use mins[0] and maxs[0] more
	uNorthDot = fabs( DotProduct( vup_n, worldNorth ) );//1.0 if looking up or down, head towards north or south, so use mins[1] and maxs[1] more

	/*if ( crossEnt->s.solid == SOLID_BMODEL )
	{//brush model, no origin, so use the center
		VectorAdd( crossEnt->absmin, crossEnt->absmax, center );
		VectorScale( center, 0.5, center );
		VectorSubtract( crossEnt->absmax, center, maxs );
		VectorSubtract( crossEnt->absmin, center, mins );
	}
	else
	{*/
		VectorCopy( origin, center ); //crossEnt->currentOrigin//cent->lerpOrigin
		VectorCopy( entMaxs, maxs ); //crossEnt->maxs //playerMaxs
		VectorCopy( entMins, mins ); //crossEnt->mins //playerMins
	//}

	//NOTE: this presumes that mins[0] and maxs[0] are symmetrical and mins[1] and maxs[1] as well
	topSize = (maxs[2]*fUpDot + maxs[1]*uNorthDot + maxs[0]*uEastDot);//* timedScale
	bottomSize = (mins[2]*fUpDot + mins[1]*uNorthDot + mins[0]*uEastDot);//* timedScale
	leftSize = (fUpDot*(mins[0]*fNorthDot + mins[1]*fEastDot) + mins[0]*uNorthDot + mins[1]*uEastDot);//* timedScale
	rightSize = (fUpDot*(maxs[0]*fNorthDot + maxs[1]*fEastDot) + maxs[0]*uNorthDot + maxs[1]*uEastDot);//* timedScale

	//Find corners
	//top
	VectorMA( center, topSize, vup_n, top );
	//bottom
	VectorMA( center, bottomSize, vup_n, bottom );
	//Top-left frame
	VectorMA( top, leftSize, vright_n, topLeft );
	//Top-right frame
	VectorMA( top, rightSize, vright_n, topRight );
	//bottom-left frame
	VectorMA( bottom, leftSize, vright_n, bottomLeft );
	//bottom-right frame
	VectorMA( bottom, rightSize, vright_n, bottomRight );

	if ( CG_WorldCoordToScreenCoord( topLeft, &topLeftx, &topLefty, qfalse ) )
	{
		doTopLeft = qtrue;
	}
	else
	{
		doSizes = qfalse;
	}

	if ( CG_WorldCoordToScreenCoord( topRight, &topRightx, &topRighty, qfalse ) )
	{
		doTopRight = qtrue;
	}
	else
	{
		doSizes = qfalse;
	}

	if ( CG_WorldCoordToScreenCoord( bottomLeft, &bottomLeftx, &bottomLefty, qfalse ) )
	{
		doBottomLeft = qtrue;
	}
	else
	{
		doSizes = qfalse;
	}

	if ( CG_WorldCoordToScreenCoord( bottomRight, &bottomRightx, &bottomRighty, qfalse ) )
	{
		doBottomRight = qtrue;
	}
	else
	{
		doSizes = qfalse;
	}

	//NOTE: maybe print color-coded "Primary/Secondary Objective" on top if an objective?
	for ( corner = 0; corner < 13; corner++ ) //11
	{//FIXME: make sure line length of 8 isn't greater than width of object
		switch ( corner )
		{
		case 0://top-left
			if ( doTopLeft )
			{
				if ( doSizes )
				{
				//Line lengths
					lineVertLength = (bottomLefty-topLefty)*0.25f;
					lineHorzLength = (topRightx-topLeftx)*0.25f;
				}
				CG_FillRect( topLeftx + 2, topLefty, lineHorzLength, lineWidth, color );
				CG_FillRect( topLeftx, topLefty, lineWidth, lineVertLength, color );
			}
			break;
		case 1://top-right
			if ( doTopRight )
			{
				if ( doSizes )
				{
				//Line lengths
					lineVertLength = (bottomRighty-topRighty)*0.25f;
					lineHorzLength = (topRightx-topLeftx)*0.25f;
				}
				CG_FillRect( topRightx-lineHorzLength, topRighty, lineHorzLength, lineWidth, color );
				CG_FillRect( topRightx, topRighty, lineWidth, lineVertLength, color );
			}
			break;
		case 2://bottom-left
			if ( doBottomLeft )
			{
				if ( doSizes )
				{
				//Line lengths
					lineVertLength = (bottomLefty-topLefty)*0.25f;
					lineHorzLength = (bottomRightx-bottomLeftx)*0.25f;
				}
				CG_FillRect( bottomLeftx, bottomLefty, lineHorzLength, lineWidth, color );
				CG_FillRect( bottomLeftx, bottomLefty-lineVertLength, lineWidth, lineVertLength, color );
			}
			break;
		case 3://bottom-right
			if ( doBottomRight )
			{
				if ( doSizes )
				{
				//Line lengths
					lineVertLength = (bottomRighty-topRighty)*0.25f;
					lineHorzLength = (bottomRightx-bottomLeftx)*0.25f;
				}
				CG_FillRect( bottomRightx-lineHorzLength, bottomRighty, lineHorzLength, lineWidth, color );
				CG_FillRect( bottomRightx, bottomRighty-lineVertLength, lineWidth, lineVertLength + 2, color );
			}
			break;
		case 4://healthBar
			if ( charIndex > 0 )
			{
				/*
				//tried to keep original functionality, but it would pop from top to bottom
				//when you let go of the button and had no way to tell then (during the
				//fade-out) whether it should be on top or bottom.  So now it is always on top.
				if ( !scanAll )
				{
					if ( !CG_WorldCoordToScreenCoord( bottom, &x, &y, qfalse ) )
					{//Can't draw bottom
						return;
					}
				}
				else
				*/
				{//try to draw at top as to not obscure the tricorder
					CG_WorldCoordToScreenCoord( top, &x, &y, qtrue );
					if ( y > 0.01 )
					{
						y -= SMALLCHAR_HEIGHT;
						if ( y > 0.01 )
						{
							if ( charIndex > 0 && name )
							{
								if ( y >= SMALLCHAR_HEIGHT )
								{
									y -= SMALLCHAR_HEIGHT;
								}
								else
								{
									y = 0.01;
								}
							}
							if ( y > 0.01 )
							{
								if ( rankCharIndex > 0 && rank )
								{
									if ( y >= SMALLCHAR_HEIGHT )
									{
										y -= SMALLCHAR_HEIGHT;
									}
								}
								if ( y > 0.01 )
								{
									if ( ageCharIndex > 0 && age )
									{
										if ( y >= SMALLCHAR_HEIGHT )
										{
											y -= SMALLCHAR_HEIGHT;
										}
									}
									if ( y > 0.01 )
									{
										if ( classCharIndex > 0 && pClass )
										{
											if ( y >= SMALLCHAR_HEIGHT )
											{
												y -= SMALLCHAR_HEIGHT;
											}
										}
										if ( y > 0.01 )
										{
											if ( raceCharIndex > 0 && race )
											{
												if ( y >= SMALLCHAR_HEIGHT )
												{
													y -= SMALLCHAR_HEIGHT;
												}
											}
											if ( y > 0.01 )
											{
												if ( htCharIndex > 0 && height )
												{
													if ( y >= SMALLCHAR_HEIGHT )
													{
														y -= SMALLCHAR_HEIGHT;
													}
												}
												if ( y > 0.01 )
												{
													if ( wtCharIndex > 0 && weight )
													{
														if ( y >= SMALLCHAR_HEIGHT )
														{
															y -= SMALLCHAR_HEIGHT;
														}
													}
													if ( y > 0.01 )
													{
														if ( weapCharIndex > 0 && weapon )
														{
															if ( y >= SMALLCHAR_HEIGHT )
															{
																y -= SMALLCHAR_HEIGHT;
															}
														}
													}	
												}
											}
										}
									}
								}
							}
						}
					}
				}
				
				if ( !color[0] && !color[1] && !color[2] )
				{
					// We really don't want black, so set it to yellow
					color[0] = 0.9F;//R
					color[1] = 0.7F;//G
					color[2] = 0.0F;//B
				}
				color[3] = 0.75;

				if ( !drawHealth || !health )
				{
					continue;
				}

				//health = ci->health; //health, max_health //ceil( (float)ci->health/(float)100.0f*100.0f );
				//CG_ColorForGivenHealth( hcolor, health );
				//hwidth = (float)health*0.5f;

				//y += lineWidth + 2;

				//CG_FillRect( x - hwidth/2, y + lineWidth, hwidth, lineWidth*2, hcolor );

				//y += lineWidth*2;

				Com_sprintf( showHealth, sizeof( showHealth ), "%s: %i", "Health", health );

				if ( healthCharIndex > 0 && showHealth[0] ) {
					int len = strlen( showHealth );

					if ( healthCharIndex > len+1 )
					{
						healthCharIndex = len+1;
					}
					else
					{
						trap_S_StartSound( NULL, 0, CHAN_ITEM, cgs.media.tedTextSound );
					}
					//Q_strncpyz( showHealth, showHealth, healthCharIndex );
					w = CG_DrawStrlen( showHealth ) * SMALLCHAR_WIDTH;
					Q_strncpyz( showHealth, showHealth, healthCharIndex );
					CG_DrawSmallStringColor( x - w / 2, y + lineWidth, showHealth, color );
					y += SMALLCHAR_HEIGHT;
				}
			}
			break;
		case 5://infoString (name/description)
			//Bright yellow
			//VectorCopy( crossEnt->startRGBA, color );
			
			/*if ( !color[0] && !color[1] && !color[2] )
			{
				// We really don't want black, so set it to yellow
				color[0] = 0.9F;//R
				color[1] = 0.7F;//G
				color[2] = 0.0F;//B
			}
			color[3] = 0.75;*/
			if ( charIndex > 0 && name )
			{
				int	len = strlen(name);
				if ( charIndex > len+1 )
				{
					charIndex = len+1;
				}
				else
				{
					trap_S_StartSound( NULL, 0, CHAN_ITEM, cgs.media.tedTextSound );
				}
				Q_strncpyz( showName, name, charIndex );
				w = CG_DrawStrlen( name ) * SMALLCHAR_WIDTH;
				CG_DrawSmallStringColor( x - w / 2, y + lineWidth, showName, color );
				y += SMALLCHAR_HEIGHT;
			}
			break;
		case 6://class
			if ( classCharIndex > 0 && pClass )
			{
				int	len = strlen(pClass);
				if ( classCharIndex > len+1 )
				{
					classCharIndex = len+1;
				}
				else
				{
					trap_S_StartSound( NULL, 0, CHAN_ITEM, cgs.media.tedTextSound );
				}
				Q_strncpyz( showClass, pClass, classCharIndex );
				w = CG_DrawStrlen( pClass ) * SMALLCHAR_WIDTH;
				CG_DrawSmallStringColor( x - w / 2, y + lineWidth, showClass, color );
				y += SMALLCHAR_HEIGHT;
			}
			break;
		case 7://rank
			if ( rankCharIndex > 0 && rank )
			{
				int	len = strlen(rank);
				if ( rankCharIndex > len+1 )
				{
					rankCharIndex = len+1;
				}
				else
				{
					trap_S_StartSound( NULL, 0, CHAN_ITEM, cgs.media.tedTextSound );
				}
				Q_strncpyz( showRank, rank, rankCharIndex );
				w = CG_DrawStrlen( rank ) * SMALLCHAR_WIDTH;
				CG_DrawSmallStringColor( x - w / 2, y + lineWidth, showRank, color );
				y += SMALLCHAR_HEIGHT;
			}
			break;
		case 8://age
			if ( ageCharIndex > 0 && age )
			{
				int	len = strlen(age);
				if ( ageCharIndex > len+1 )
				{
					ageCharIndex = len+1;
				}
				else
				{
					trap_S_StartSound( NULL, 0, CHAN_ITEM, cgs.media.tedTextSound );
				}
				Q_strncpyz( showAge, age, ageCharIndex );
				w = CG_DrawStrlen( age ) * SMALLCHAR_WIDTH;
				CG_DrawSmallStringColor( x - w / 2, y + lineWidth, showAge, color );
				y += SMALLCHAR_HEIGHT;
			}
			break;
		case 9://race
			if ( raceCharIndex > 0 && race )
			{
				int	len = strlen(race);
				if ( raceCharIndex > len+1 )
				{
					raceCharIndex = len+1;
				}
				else
				{
					trap_S_StartSound( NULL, 0, CHAN_ITEM, cgs.media.tedTextSound );
				}
				Q_strncpyz( showRace, race, raceCharIndex );
				w = CG_DrawStrlen( race ) * SMALLCHAR_WIDTH;
				CG_DrawSmallStringColor( x - w / 2, y + lineWidth, showRace, color );
				y += SMALLCHAR_HEIGHT;
			}
			break;
		case 10://height
			if ( htCharIndex > 0 && height )
			{
				int	len = strlen(height);
				if ( htCharIndex > len+1 )
				{
					htCharIndex = len+1;
				}
				else
				{
					trap_S_StartSound( NULL, 0, CHAN_ITEM, cgs.media.tedTextSound );
				}
				Q_strncpyz( showHt, height, htCharIndex );
				w = CG_DrawStrlen( height ) * SMALLCHAR_WIDTH;
				CG_DrawSmallStringColor( x - w / 2, y + lineWidth, showHt, color );
				y += SMALLCHAR_HEIGHT;
			}
			break;
		case 11://weight
			if ( wtCharIndex > 0 && weight )
			{
				int	len = strlen(weight);
				if ( wtCharIndex > len+1 )
				{
					wtCharIndex = len+1;
				}
				else
				{
					trap_S_StartSound( NULL, 0, CHAN_ITEM, cgs.media.tedTextSound );
				}
				Q_strncpyz( showWt, weight, wtCharIndex );
				w = CG_DrawStrlen( weight ) * SMALLCHAR_WIDTH;
				CG_DrawSmallStringColor( x - w / 2, y + lineWidth, showWt, color );
				y += SMALLCHAR_HEIGHT;
			}
			break;
		case 12://weapon
			if ( weapCharIndex > 0 && weapon )
			{
				int	len = strlen(weapon);
				if ( weapCharIndex > len+1 )
				{
					weapCharIndex = len+1;
				}
				else
				{
					trap_S_StartSound( NULL, 0, CHAN_ITEM, cgs.media.tedTextSound );
				}
				Q_strncpyz( showWeap, weapon, weapCharIndex );
				w = CG_DrawStrlen( weapon ) * SMALLCHAR_WIDTH;
				CG_DrawSmallStringColor( x - w / 2, y + lineWidth, showWeap, color );
				y += SMALLCHAR_HEIGHT;
			}
			break;
		}
	}
}

/*
=================
CG_ScanForCrosshairEntity
=================
*/
static void CG_ScanForCrosshairEntity( void ) {
	trace_t		trace;
	vec3_t		start, end;
	int			content;
	vec3_t		pitchConstraint, df_f;

	VectorCopy( cg.predictedPlayerState.origin, start ); //cg.refdef.vieworg
	start[2] += (float)cg.predictedPlayerState.viewheight * cgs.clientinfo[cg.predictedPlayerState.clientNum].height;
	//VectorCopy( cg.predictedPlayerState.origin, start);
	//start[2] += cg.predictedPlayerState.viewheight;

	VectorCopy( cg.predictedPlayerState.viewangles, pitchConstraint );
	AngleVectors( pitchConstraint, df_f, NULL, NULL );

	VectorMA( start, 8912, df_f, end);

	//VectorMA( start, 8192, cg.refdef.viewaxis[0], end );

	if ( cg.snap->ps.weapon == WP_7 && cg.zoomed ) {
		CG_Trace( &trace, start, vec3_origin, vec3_origin, end, 
			cg.snap->ps.clientNum, CONTENTS_BODY );
		
		// if the player is invisible, don't show it
		if ( cg_entities[ trace.entityNum ].currentState.powerups & ( 1 << PW_INVIS ) && !cgs.clientinfo[cg.snap->ps.clientNum].isAdmin/*cg.snap->ps.persistant[PERS_CLASS] != PC_ADMIN*/ ) {
			return;
		}
	}
	else {
		CG_Trace( &trace, start, vec3_origin, vec3_origin, end, 
			cg.snap->ps.clientNum, MASK_SHOT ); //CONTENTS_SOLID|CONTENTS_BODY

		if ( cg.predictedPlayerState.weapon == WP_2 && cg.predictedPlayerState.eFlags & EF_FIRING 
			&& (cg_entities[trace.entityNum].currentState.eType == ET_TRIC_STRING || cg_entities[trace.entityNum].currentState.eType == ET_MOVER_STR) ) 
		{
			//Never mind if it's a valid useable ent
		} //else, return
		else if ( trace.entityNum >= MAX_CLIENTS && !cgs.clientinfo[cg.snap->ps.clientNum].isAdmin/*cg.predictedPlayerState.persistant[PERS_CLASS] != PC_ADMIN*/ ) {
			return;
		}

		// if the player is in fog, don't show it
		content = trap_CM_PointContents( trace.endpos, 0 );
		if ( content & CONTENTS_FOG ) {
			return;
		}

		// if the player is invisible, don't show it
		if ( cg_entities[ trace.entityNum ].currentState.powerups & ( 1 << PW_INVIS ) && !cgs.clientinfo[cg.snap->ps.clientNum].isAdmin/*cg.snap->ps.persistant[PERS_CLASS] != PC_ADMIN*/ ) {
			return;
		}

		if ( cg.crosshairClientNum != trace.entityNum) {
			infoStringCount = 0;
		}

	}

	// update the fade timer
	cg.crosshairClientNum = trace.entityNum;
	cg.crosshairClientTime = cg.time;

	//CG_Printf( "Current ent num: %i\n", cg.crosshairClientNum );
}


/*
=====================
CG_DrawCrosshairNames
=====================
*/

extern qboolean PM_PlayerCrouching ( int legsAnim );

static vec3_t	playerMins = {-12, -12, -24}; //RPG-X : TiM - {-15, -15, -24}
static vec3_t	playerMaxs = {12, 12, 32}; // {15, 15, 32}
static void CG_DrawCrosshairNames( void ) {
	float		*color;
	char		name[MAX_QPATH];
	int			team;
	centity_t	*cent;
	//vec4_t		vecColor = { 0.0, 1.0, 0.0, 1.0 };
	int			x, y;
	qboolean		tinyFont;
	int		drawFlags;

	if ( !cg_drawCrosshair.integer ) 
	{
		return;
	}

	//if ( cg.renderingThirdPerson )
	//{
	//	return;
	//}

	// scan the known entities to see if the crosshair is sighted on one
	CG_ScanForCrosshairEntity();
		
	// draw the name of the player being looked at
	color = CG_FadeColor( cg.crosshairClientTime, 1000 );
	if ( !color ) 
	{
		trap_R_SetColor( NULL );
		infoStringCount = 0;
		return;
	}

	color[3] *= 0.9;

	//If they're actively firing the tricorder
	if( ( (cg.snap->ps.eFlags & EF_FIRING) && !(cg.snap->ps.eFlags & EF_ALT_FIRING) ) 
		&& cg.snap->ps.weapon == WP_2 ) {
		if(cg.crosshairClientNum != cg.predictedPlayerState.clientNum && cg.crosshairClientNum < MAX_CLIENTS ) { //ENTITYNUM_WORLD
			
			drawCrosshairName = qfalse;

			cent = &cg_entities[cg.crosshairClientNum];

			if ( cent ) {
				char	*name = NULL;
				char	*rank = NULL;
				char	*race = NULL;
				char	*age = NULL;
				char	*pClass = NULL;
				//vec3_t	size;
				float	ht = 0;
				float	wt = 0;
				//int		health = 0;
				char	*weap = NULL;
				char	namestr[128];
				char	rankstr[128];
				char	racestr[128];
				char	htstr[128];
				char	wtstr[128];
				char	weapstr[128];
				char	agestr[128];
				char	classstr[128];
				int		i, irank;
				int		score = 0;
				clientInfo_t *ci;

				for ( i = 0; i < cgs.maxclients; i++ ) {
					if ( cg.scores[i].client == cg.crosshairClientNum ) {
						score = cg.scores[i].score;
						break;
					}
				}

				irank = score;//Q_log2( score );

				ci = &cgs.clientinfo[cg.crosshairClientNum];
				//over-ride the color, since we can't get teams in this case
				//use that good old LCARS yellow
				color[0] = 0.694f;//0.9F;//R
				color[1] = 0.816f;//0.7F;//G
				color[2] = 1.0f;//0.0F;//B
				color[3] *= 0.5;

				//vec3_t	maxs, mins;

				//VectorCopy( crossEnt->maxs, maxs );
				//VectorCopy( crossEnt->mins, mins );
				//if ( crossEnt->client && crossEnt->NPC )
				//{//only use the standing height of the NPCs because people can't understand the complex dynamics of height in weight in a ceiling-installed anti-gravitic plating environment
				//	maxs[2] = crossEnt->client->standheight;
				//}
				//VectorSubtract(maxs, mins, size);
				//ht = (maxs[2] - mins[2]) * 3.46875;//magic number
				ht = ci->height * (float)BASE_HEIGHT;
				//wt = VectorLength(size)*1.4;//magic number
				wt = ci->weight * ci->height * (float)BASE_WEIGHT;
				//if ( crossEnt->client && crossEnt->NPC )
				//{
				//if ( strstr( crossEnt->client->renderInfo.legsModelName, "female" ) ||
					//	strstr( crossEnt->client->renderInfo.legsModelName, "seven" ) )
					//{//crewfemale, hazardfemale or seven of nine
				if ( ci->gender == GENDER_FEMALE ) {
					wt *= (float)FEMALE_OFFSET;//magic number, women are lighter than men
				}
				
				if ( ci->race && ci->race[0] ) {
					race = ci->race;
					Com_sprintf( racestr, sizeof( racestr ), "%s: %s", "Race", race );
					//Q_strncpyz( race, racestr, sizeof( racestr) );
				}

				if ( ci->age && ci->age[0] ) {
					age = ci->age;
					Com_sprintf( agestr, sizeof( agestr ), "%s: %s", "Age", age );
					//Q_strncpyz( race, racestr, sizeof( racestr) );
				}

				//Com_Printf( "%i\n", ci->pClass );
				pClass = cgs.classData[ci->pClass].formalName;
				/*switch ( ci->pClass ) {
					case PC_ADMIN:
						pClass = "Admin";
						break;
					case PC_SECURITY:
						pClass = "Security";
						break;
					case PC_ALIEN:
						pClass = "Alien";
						break;
					case PC_COMMAND:
						pClass = "Command";
						break;
					case PC_SCIENCE:
						pClass = "Science";
						break;
					case PC_ENGINEER:
						pClass = "Engineer";
						break;
					case PC_ALPHAOMEGA22:
						pClass = "Marine";
						break;
					case PC_N00B:
						pClass = "n00b";
						break;
					case PC_NOCLASS:
					default:
						pClass = "Unknown";
						break;
				}*/

				if ( pClass ) {
					Com_sprintf( classstr, sizeof(classstr), "%s: %s", "Class", pClass );
				}

				if ( cgs.classData[ci->pClass].showRanks/*ci->pClass != PC_ALIEN && ci->pClass != PC_NOCLASS*/ ) {
					//rank = "Awesome"; //RankForNumber func needed
					if ( cgs.ranksData[irank].formalName[0] ) {
						rank = cgs.ranksData[irank].formalName;
						Com_sprintf( rankstr, sizeof( rankstr ), "%s: %s", "Rank", rank );
					}
					//Q_strncpyz( rank, rankstr, sizeof( rankstr ) );
				}
					
				if ( ci->name && ci->name[0] ) {
					name = ci->name;
				}
				else {
					name = "Data Not Available";//crossEnt->targetname;
				}
					
				Com_sprintf( namestr, sizeof( namestr), "%s: %s", "Name", name );		

				if ( cent->currentState.weapon != WP_1 /*&& cg_weapons[ cent->currentState.weapon ].item*/ )
				{
					if ( cg_weapons[ cent->currentState.weapon ].item->pickup_name ) {
						weap = cg_weapons[ cent->currentState.weapon ].item->pickup_name;
						Com_sprintf( weapstr, sizeof( weapstr), "%s: %s", "Weapon", weap );
					}
				}

				Com_sprintf( htstr, sizeof(htstr), "%s: %4.2f %s","Height", ht, HEIGHT_UNIT );
				Com_sprintf( wtstr, sizeof(wtstr), "%s: %4.2f %s","Weight", wt, WEIGHT_UNIT );

				//Com_Printf("Name: %s, Rank: %s, Race: %s, Height: %s, Weight: %s, Weap: %s\n", namestr, rankstr, racestr, htstr, wtstr, weapstr );

				CG_LabelViewEntity( cg.crosshairClientNum, cent->lerpOrigin, playerMins, playerMaxs,
					name ? namestr : NULL, qfalse, color, 
					(cgs.clientinfo[cg.snap->ps.clientNum].isAdmin || cgs.classData[cg.snap->ps.persistant[PERS_CLASS]].isMedic) ? qtrue : qfalse, ci->health,
					pClass ? classstr : NULL,
					rank ? rankstr : NULL, 
					race ? racestr : NULL, 
					age ? agestr : NULL,
					ht ? htstr : NULL,
					wt ? wtstr : NULL,
					weap ? weapstr : NULL);
			}
			else {
				infoStringCount = 0;
			}
		}
		else {
			if ( (cg_entities[cg.crosshairClientNum].currentState.eType == ET_TRIC_STRING || cg_entities[cg.crosshairClientNum].currentState.eType == ET_MOVER_STR) && cgs.scannablePanels ) 
			{
				entityState_t	*eState;
				vec3_t origin;
				vec3_t mins, maxs;
				char *renderString;

				eState = &cg_entities[cg.crosshairClientNum].currentState;

				color[0] = 0.694f;//0.9F;//R
				color[1] = 0.816f;//0.7F;//G
				color[2] = 1.0f;//0.0F;//B
				color[3] *= 0.5;

				//TiM: Since dynamic brush ents seem to have no freaking origin in them, let's
				// calc our own using the bounding box dimensions (At least we have those lol )
				VectorAverage( eState->origin2, eState->angles2, origin );
				//origin[2] = eState->origin2[2] - 24;

				//The algorithm needs the max and min dimensions to be symmetrical on either side
				//of the origin.  This set of random code does that. :)
				VectorSubtract( origin, eState->origin2, mins );

				VectorSet( maxs, Q_fabs( mins[0] ), Q_fabs( mins[1] ), Q_fabs( mins[2] ) );
				VectorScale( maxs, -1, mins );

				if ( eState->time2 > 0 )
					renderString = (char *)CG_ConfigString( CS_TRIC_STRINGS + eState->time2 );
				else if ( eState->weapon > 0 && cgs.scannableStrings[eState->weapon-1][0] )
					renderString = cgs.scannableStrings[eState->weapon-1]; //subtracted since '0' is a valid cell value
				else
					renderString = "<undefined>";


				CG_LabelViewEntity( cg.crosshairClientNum, origin,
					mins, maxs,	renderString, //cgs.tricStrings[eState->time2],
					qfalse, color, 
					qfalse, 0,
					NULL,
					NULL, 
					NULL,
					NULL, 
					NULL,
					NULL,
					NULL);
			}
		}
	}
	else
		drawCrosshairName = qtrue;
	
	if ( !cg_drawCrosshairNames.integer || cg.crosshairClientNum > MAX_CLIENTS || !drawCrosshairName ) 
	{
		return;
	}

	//Now only draw team names + health if specifically wanted
	Q_strncpyz (name, cgs.clientinfo[ cg.crosshairClientNum ].name, sizeof (name) );

	// Draw in red if red team, blue if blue team
	if (cgs.gametype >= GT_TEAM) 
	{
		Q_CleanStr(name);
		team = cgs.clientinfo[ cg.crosshairClientNum ].team;
		if (team==TEAM_RED)
		{
			color[0] = colorRed[0];
			color[1] = colorRed[1];
			color[2] = colorRed[2];
		}
		else
		{
			color[0] = colorBlue[0];
			color[1] = colorBlue[1];
			color[2] = colorBlue[2];
		}
	}
	else
	{
		color[0] = colorTable[CT_YELLOW][0];
		color[1] = colorTable[CT_YELLOW][1];
		color[2] = colorTable[CT_YELLOW][2];
	}

	if ( !cg_dynamicCrosshairNames.integer )
	{
		x = 320;
		y = 170;

		tinyFont = qfalse;

		drawFlags = UI_CENTER|UI_SMALLFONT;
	}
	else
	{
		vec3_t		org;
		centity_t	*cent;
		float		x2, y2;

		cent = &cg_entities[ cg.crosshairClientNum ];

		VectorCopy( cent->lerpOrigin, org );

		if ( PM_PlayerCrouching( cent->currentState.legsAnim ) )
			org[2] += CROUCH_VIEWHEIGHT + 7;
		else
			org[2] += DEFAULT_VIEWHEIGHT + 7;

		CG_WorldCoordToScreenCoord( org, &x2, &y2, qfalse);

		x = (int)x2;
		y = (int)y2;

		tinyFont = qtrue;
		drawFlags = UI_CENTER|UI_BOTTOM|UI_TINYFONT;
	}

		//FIXME: need health (&armor?) of teammates (if not TEAM_FREE) or everyone (if SPECTATOR) (or just crosshairEnt?) sent to me
	if (cgs.clientinfo[ cg.snap->ps.clientNum ].team == TEAM_SPECTATOR 
		|| cgs.classData[cgs.clientinfo[ cg.snap->ps.clientNum ].pClass].isMedic
		|| cgs.clientinfo[ cg.snap->ps.clientNum ].isAdmin  ) /*|| cgs.clientinfo[ cg.snap->ps.clientNum ].pClass == PC_MEDIC*/
	{//if I'm a spectator, draw colored health of target under crosshair
		CG_GetColorForHealth( cgs.clientinfo[ cg.crosshairClientNum ].health, cgs.clientinfo[ cg.crosshairClientNum ].armor, color );
		
		y -= ( tinyFont ? TINYCHAR_HEIGHT : SMALLCHAR_HEIGHT );

		UI_DrawProportionalString(x,y+(tinyFont ? TINYCHAR_HEIGHT+5 : SMALLCHAR_HEIGHT),va( "^7%i", cgs.clientinfo[ cg.crosshairClientNum ].health ), drawFlags,color);
	}

	UI_DrawProportionalString(x,y, va("^7%s ^7(%i)", name, cg.crosshairClientNum), drawFlags, color);
}



//==============================================================================

/*
=================
CG_DrawSpectator
=================
*/
static void CG_DrawSpectator(void) {
//	CG_DrawBigString(320 - 9 * 8, 440, ingame_text[IGT_SPECTATOR], 1.0F);
	if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR )
	{
		UI_DrawProportionalString(SCREEN_WIDTH/2, SCREEN_HEIGHT - ((BIGCHAR_HEIGHT * 1.50) * 2) , ingame_text[IGT_SPECTATOR], UI_BIGFONT|UI_CENTER, colorTable[CT_LTGOLD1]);
	}
	/*else if ( cg.snap->ps.eFlags&EF_ELIMINATED )
	{
		UI_DrawProportionalString(SCREEN_WIDTH/2, SCREEN_HEIGHT - ((BIGCHAR_HEIGHT * 1.50) * 2) , ingame_text[IGT_TITLEELIMINATED], UI_BIGFONT|UI_CENTER, colorTable[CT_LTGOLD1]);
	}*/
	if ( cgs.gametype == GT_TOURNAMENT ) {
//		CG_DrawBigString(320 - 15 * 8, 460, ingame_text[IGT_WAITINGTOPLAY], 1.0F);
		UI_DrawProportionalString(SCREEN_WIDTH/2,  SCREEN_HEIGHT - (BIGCHAR_HEIGHT * 1.5), ingame_text[IGT_WAITINGTOPLAY], UI_BIGFONT|UI_CENTER, colorTable[CT_LTGOLD1]);
	}
	if ( cgs.gametype == GT_TEAM || cgs.gametype == GT_CTF ) {
//		CG_DrawBigString(320 - 25 * 8, 460, ingame_text[IGT_USEDTEAMMENU], 1.0F);
		UI_DrawProportionalString(SCREEN_WIDTH/2,  SCREEN_HEIGHT - (BIGCHAR_HEIGHT * 1.5), ingame_text[IGT_USEDTEAMMENU], UI_BIGFONT|UI_CENTER, colorTable[CT_LTGOLD1]);
	}
}

/*
=================
CG_DrawVote
=================
*/
static void CG_DrawVote(void) {
	char	*s;
	int		sec;

	if ( !cgs.voteTime ) {
		return;
	}

	// play a talk beep whenever it is modified
	if ( cgs.voteModified ) {
		cgs.voteModified = qfalse;
		trap_S_StartLocalSound( cgs.media.talkSound, CHAN_LOCAL_SOUND );
	}

	sec = ( VOTE_TIME - ( cg.time - cgs.voteTime ) ) / 1000;
	if ( sec < 0 ) {
		sec = 0;
	}
	s = va("%s(%i):%s %s(F1):%i %s(F2):%i", ingame_text[IGT_VOTE],sec, cgs.voteString,ingame_text[IGT_YES], cgs.voteYes,ingame_text[IGT_NO]  ,cgs.voteNo);
	
//	CG_DrawSmallStringColor( 0, 58, s, colorTable[CT_YELLOW] );
	UI_DrawProportionalString( 0,  58, s, UI_SMALLFONT, colorTable[CT_YELLOW]);
}

/*
=================
CG_DrawIntermission
=================
*/
static void CG_DrawIntermission( void ) {
	if (0)// cgs.gametype == GT_SINGLE_PLAYER )
	{
		CG_DrawCenterString();
		return;
	}

	cg.scoreFadeTime = cg.time;
	CG_DrawScoreboard();
}

/*
=================
CG_DrawAbridgedObjective
=================
*/
static void CG_DrawAbridgedObjective(void)
{
	int i,pixelLen,x,y;

	for (i=0;i<MAX_OBJECTIVES;i++)
	{
		if (cgs.objectives[i].abridgedText[0])
		{
			if (!cgs.objectives[i].complete)
			{
				pixelLen = UI_ProportionalStringWidth( cgs.objectives[i].abridgedText,UI_TINYFONT);

				x = 364 - (pixelLen/2);
				y = SCREEN_HEIGHT - PROP_TINY_HEIGHT;
				UI_DrawProportionalString(x, y, cgs.objectives[i].abridgedText, UI_TINYFONT, colorTable[CT_GREEN] );
				break;
			}
		}
	}

}

/*
=================
CG_DrawFollow
=================
*/
static qboolean CG_DrawFollow( void ) {
	float		y;
	vec4_t		color;
	const char	*name;

	if ( !(cg.snap->ps.pm_flags & PMF_FOLLOW) ) {
		return qfalse;
	}
	color[0] = 1;
	color[1] = 1;
	color[2] = 1;
	color[3] = 1;

	y = 16;

	UI_DrawProportionalString((SCREEN_WIDTH/2), y, ingame_text[IGT_FOLLOWING], UI_BIGFONT|UI_CENTER, colorTable[CT_LTGOLD1]);

	name = cgs.clientinfo[ cg.snap->ps.clientNum ].name;

	y += (BIGCHAR_HEIGHT * 1.25);
	UI_DrawProportionalString(  (SCREEN_WIDTH/2), 40, name, UI_BIGFONT|UI_CENTER, color);

	return qtrue;
}



/*
=================
CG_DrawAmmoWarning
=================
RPG-X | Marcin | 30/12/2008
Don't!!!
*/
static void CG_DrawAmmoWarning( void ) 
{
	return;
/*
	const char	*s;

	if ( cg_drawAmmoWarning.integer == 0 ) 
	{
		return;
	}

	if ( !cg.lowAmmoWarning ) 
	{
		return;
	}

	if ( cg.lowAmmoWarning >= 2 ) 
	{
		s = ingame_text[IGT_OUTOFAMMO];
	} else 
	{
		s = ingame_text[IGT_LOWAMMO];
	}

	UI_DrawProportionalString(320, 64, s, UI_SMALLFONT | UI_CENTER, colorTable[CT_LTGOLD1]); */

}

/*
=================
CG_DrawWarmup
=================
*/
extern void CG_AddGameModNameToGameName( char *gamename );
static void CG_DrawWarmup( void ) {
	int			w;
	int			sec;
	int			i;
	clientInfo_t	*ci1, *ci2;
	int			cw;
	const char	*s;

	sec = cg.warmup;
	if ( !sec ) {
		return;
	}

	if ( sec < 0 ) {
		s = ingame_text[IGT_WAITINGFORPLAYERS];		
//		w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
		w = UI_ProportionalStringWidth(s,UI_BIGFONT);
//		CG_DrawBigString(320 - w / 2, 40, s, 1.0F);
		UI_DrawProportionalString(320 - w / 2, 40, s, UI_BIGFONT, colorTable[CT_LTGOLD1]);

		cg.warmupCount = 0;
		return;
	}

	if (cgs.gametype == GT_TOURNAMENT) {
		// find the two active players
		ci1 = NULL;
		ci2 = NULL;
		for ( i = 0 ; i < cgs.maxclients ; i++ ) {
			if ( cgs.clientinfo[i].infoValid && cgs.clientinfo[i].team == TEAM_FREE ) {
				if ( !ci1 ) {
					ci1 = &cgs.clientinfo[i];
				} else {
					ci2 = &cgs.clientinfo[i];
				}
			}
		}

		if ( ci1 && ci2 ) {
			s = va( "%s vs %s", ci1->name, ci2->name );
//			w = CG_DrawStrlen( s );
			w = UI_ProportionalStringWidth(s,UI_BIGFONT);

			if ( w > 640 / BIGCHAR_WIDTH ) {
				cw = 640 / w;
			} else {
				cw = BIGCHAR_WIDTH;
			}
//			CG_DrawStringExt( 320 - w * cw/2, 20,s, colorWhite, 
//					qfalse, qtrue, cw, (int)(cw * 1.5), 0 );
			UI_DrawProportionalString( (SCREEN_WIDTH/2), 20,s, UI_BIGFONT|UI_CENTER, colorTable[CT_LTGOLD1]);

		}
	} else {
		char	gamename[1024];

		if ( cgs.gametype == GT_FFA ) {
			s = ingame_text[IGT_GAME_FREEFORALL];
		} else if ( cgs.gametype == GT_TEAM ) {
			s =ingame_text[IGT_GAME_TEAMHOLOMATCH];
		} else if ( cgs.gametype == GT_CTF ) {
			s = ingame_text[IGT_GAME_CAPTUREFLAG];
		} else {
			s = "";
		}

		Q_strncpyz( gamename, s, sizeof(gamename) );

		CG_AddGameModNameToGameName( gamename );

		w = UI_ProportionalStringWidth(s,UI_BIGFONT);

		if ( w > 640 / BIGCHAR_WIDTH ) {
			cw = 640 / w;
		} else {
			cw = BIGCHAR_WIDTH;
		}

		UI_DrawProportionalString((SCREEN_WIDTH/2) , 20,gamename, UI_BIGFONT|UI_CENTER, colorTable[CT_LTGOLD1]);
	}

	sec = ( sec - cg.time ) / 1000;
	if ( sec < 0 ) {
		sec = 0;
	}
	s = va( "%s: %i",ingame_text[IGT_STARTSIN], sec + 1 );
	if ( sec != cg.warmupCount ) {
		cg.warmupCount = sec;
		switch ( sec ) {
		case 0:
			trap_S_StartLocalSound( cgs.media.count1Sound, CHAN_ANNOUNCER );
			break;
		case 1:
			trap_S_StartLocalSound( cgs.media.count2Sound, CHAN_ANNOUNCER );
			break;
		case 2:
			trap_S_StartLocalSound( cgs.media.count3Sound, CHAN_ANNOUNCER );
			break;
		default:
			break;
		}
	}
	switch ( cg.warmupCount ) {
	case 0:
		cw = 28;
		break;
	case 1:
		cw = 24;
		break;
	case 2:
		cw = 20;
		break;
	default:
		cw = 16;
		break;
	}

//	w = CG_DrawStrlen( s );
//	CG_DrawStringExt( 320 - w * cw/2, 70, s, colorWhite, 
//			qfalse, qtrue, cw, (int)(cw * 1.5), 0 );

	w = UI_ProportionalStringWidth(s,UI_BIGFONT);
	UI_DrawProportionalString(  (SCREEN_WIDTH/2), 70, s, UI_BIGFONT|UI_CENTER, colorTable[CT_LTGOLD1]);

}

/*
================
CG_DrawZoomMask

================
*/
static void CG_DrawZoomMask( void )
{
	float		amt = 1, size, /*val,*/ start_x, start_y;
	int			width, height, i;
	vec4_t		color1;
	int			x, y;

	/*if (	cg.snap->ps.persistant[PERS_CLASS] == PC_NOCLASS 
			|| cg.snap->ps.persistant[PERS_CLASS] != PC_SECURITY 
			&& cg.snap->ps.persistant[PERS_CLASS] != PC_ALPHAOMEGA22 
			&& cg.snap->ps.persistant[PERS_CLASS] != PC_ADMIN )
	{//in a class-based game, only the sniper can zoom
		cg.zoomed = qfalse;
		cg.zoomLocked = qfalse;
		return;
	}*/
	//TiM: New system. :)  Base zoom on current active weapon. :)
	if ( !(cg.snap->ps.weapon == WP_6 || cg.snap->ps.weapon == WP_7) ) 
	{
		cg.zoomed = qfalse;
		cg.zoomLocked = qfalse;
		return;
	}

	// Calc where to place the zoom mask...all calcs are based off of a virtual 640x480 screen
	size = cg_viewsize.integer;

	width = 640 * size * 0.01;
	width &= ~1;

	height = 480 * size * 0.01;
	height &= ~1;

	start_x = ( 640 - width ) * 0.5;
	start_y = ( 480 - height ) * 0.5;

	if ( cg.zoomed )
	{
		// Smoothly fade in..Turn this off for now since the zoom is set to snap to 30% or so...fade looks a bit weird when it does that
		if ( cg.time - cg.zoomTime <= ZOOM_OUT_TIME ) {
			amt = ( cg.time - cg.zoomTime ) / ZOOM_OUT_TIME;
		}

		// Fade mask in
		for ( i = 0; i < 4; i++ ) { 
			color1[i] = amt;
		}

		// Set fade color
		trap_R_SetColor( color1 );
		
		if ( cg.snap->ps.weapon == WP_7 ) {
			static int TR116LoopTime = 0;

			//Loop the whirring sight sound
			if ( TR116LoopTime < cg.time )
			{
				trap_S_StartSound( cg.refdef.vieworg, ENTITYNUM_WORLD, CHAN_LOCAL, cgs.media.tr116Whir );
				TR116LoopTime = cg.time + 900;
			}

			CG_DrawPic( start_x, start_y, width, height, cgs.media.zoomMask116Shader );

			//if we're zoomed over a potential target, start flashing the red crosshair
			if ( cg.crosshairClientNum != cg.snap->ps.clientNum 
				&& cg.crosshairClientNum < MAX_CLIENTS ) 
			{
				vec4_t	alphaColor;
				float	amt;

				if ( cg.time > zoomFlashTime ) {
					zoomFlashTime = cg.time + 800;
					trap_S_StartLocalSound( cgs.media.tr116Chirp, CHAN_LOCAL_SOUND );
				}

				amt = ( ( zoomFlashTime % cg.time ) / 500.0f );
				amt = Com_Clamp( 0.0, 1.0, amt );

				//Com_Printf( S_COLOR_RED "Ratio: %f\n", amt );

				VectorSet( alphaColor, 1, 1, 1 );
				alphaColor[3] = amt;

				trap_R_SetColor( alphaColor );
				//========================================

				CG_DrawPic( 256, 176, 128, 128, cgs.media.zoomGlow116Shader );
				trap_R_SetColor( color1 );
			}
			else
			{
				zoomFlashTime = 0;
			}
		} 
		else {
			CG_DrawPic( start_x, start_y, width, height, cgs.media.zoomMaskShader );
		}

		/* RPG-X - TiM : Since the rifle's view doesn't really account for these elements, toss 'em
		start_x = 210;
		start_y = 80;

		CG_DrawPic( 320 + start_x, 241, 35, -170, cgs.media.zoomBarShader);
		CG_DrawPic( 320 - start_x, 241, -35, -170, cgs.media.zoomBarShader);
		CG_DrawPic( 320 + start_x, 239, 35, 170, cgs.media.zoomBarShader);
		CG_DrawPic( 320 - start_x, 239, -35, 170, cgs.media.zoomBarShader);

		// Calculate a percent and clamp it
		val = 26 - ( cg_fov.value - cg_zoomFov.value ) / ( cg_fov.value - MAX_ZOOM_FOV ) * 26;

		if ( val > 17.0f )
			val = 17.0f;
		else if ( val < 0.0f )
			val = 0.0f;
	
		// pink
		color1[0] = 0.85f;
		color1[1] = 0.55f;
		color1[2] = 0.75f;
		color1[3] = 1.0f;

		CG_DrawPic( 320 + start_x + 12, 245, 10, 108, cgs.media.zoomInsertShader );
		CG_DrawPic( 320 + start_x + 12, 235, 10, -108, cgs.media.zoomInsertShader );
		CG_DrawPic( 320 - start_x - 12, 245, -10, 108, cgs.media.zoomInsertShader );
		CG_DrawPic( 320 - start_x - 12, 235, -10, -108, cgs.media.zoomInsertShader );

		trap_R_SetColor( color1 );
		i = ((int)val) * 6;

		CG_DrawPic( 320 + start_x + 10, 230 - i, 12, 5, cgs.media.ammoslider );
		CG_DrawPic( 320 + start_x + 10, 251 + i, 12, -5, cgs.media.ammoslider );
		CG_DrawPic( 320 - start_x - 10, 230 - i, -12, 5, cgs.media.ammoslider );
		CG_DrawPic( 320 - start_x - 10, 251 + i, -12, -5, cgs.media.ammoslider );
		*/

		//yellow
		if ( cg.snap->ps.weapon == WP_7 ) {
			color1[0] = 0.886f;
			color1[1] = 0.749f;
			color1[2] = 0.0f;
			color1[3] = 0.60f;
		}
		else { // red
			color1[0] = 1.0f;
			color1[1] = 0.0f;
			color1[2] = 0.0f;
			color1[3] = 0.60f;
		}

		// Convert zoom and view axis into some numbers to throw onto the screen
		if ( cg.snap->ps.weapon == WP_7 ) {
			x = 74;
			y = 340;
		}
		else {
			x = 468;
			y = 300;
		}

		trap_R_SetColor( color1 );
		CG_DrawNumField( x, y, 5, cg_zoomFov.value * 1000 + 9999, 18, 10 ,NUM_FONT_BIG ); //100
		CG_DrawNumField( x, y+20, 5, cg.refdef.viewaxis[0][0] * 9999 + 20000, 18, 10,NUM_FONT_BIG );
		CG_DrawNumField( x, y+40, 5, cg.refdef.viewaxis[0][1] * 9999 + 20000, 18, 10,NUM_FONT_BIG );
		CG_DrawNumField( x, y+60, 5, cg.refdef.viewaxis[0][2] * 9999 + 20000, 18, 10,NUM_FONT_BIG );
		
		/*
		// Is it time to draw the little max zoom arrows?
		if ( val < 0.2f )
		{
			amt = sin( cg.time * 0.03 ) * 0.5 + 0.5;
			color1[0] = 0.592156f * amt;
			color1[1] = 0.592156f * amt;
			color1[2] = 0.850980f * amt;
			color1[3] = 1.0f * amt;

			trap_R_SetColor( color1 );

			CG_DrawPic( 320 + start_x, 240 - 6, 16, 12, cgs.media.zoomArrowShader );
			CG_DrawPic( 320 - start_x, 240 - 6, -16, 12, cgs.media.zoomArrowShader );
		}*/
	}
	else
	{
		if ( cg.time - cg.zoomTime <= ZOOM_OUT_TIME )
		{
			amt = 1.0f - ( cg.time - cg.zoomTime ) / ZOOM_OUT_TIME;

			// Fade mask away
			for ( i = 0; i < 4; i++ ) {
				color1[i] = amt;
			}

			trap_R_SetColor( color1 );
			if ( cg.snap->ps.weapon == WP_7 ) {
				CG_DrawPic( start_x, start_y, width, height, cgs.media.zoomMask116Shader );
			} 
			else {
				CG_DrawPic( start_x, start_y, width, height, cgs.media.zoomMaskShader );
			}
		}
	}
}

//==================================================================================

/*static char *AfterSpace( char *p )
{
	while (*p && *p != ' ') {
		++p;
	}

	return p;
}*/

/*
=====================
CG_Drawcg.adminMsg
RPG-X | Phenix | 08/06/2005
RPG-X | Marcin | 30/12/2008

Now I'm going to kill you Phenix!!!!
=====================
*/
static void CG_DrawAdminMsg( void ) {
	float	y;
	int			t;
	int		i, msgRow, msgCol;
	int		biggestW, w;
	char	message[35][45];
	char	*thisMessage;
	char	*p, *currRow;
	static vec4_t		color;
	static vec4_t		Boxcolor;

	y = 460;

	//Nothing to display
	if ( cg.adminMsgTime < cg.time )
	{
		return;
	}

	//No message!
	if ( cg.adminMsgMsg[0] == '\0' )
	{
		return;
	}

	//Colour Fade.
	t = cg.adminMsgTime - cg.time;

	// fade out
	if (t < 500) {
		color[3] = t * 1.0/500;
	} else {
		color[3] = 1.0;
	}
	color[0] = color[1] = color[2] = 1;
	
	Boxcolor[0] = 0.016;
	Boxcolor[1] = 0.055;
	Boxcolor[2] = 0.170;
	/*
	CT_VDK_PURPLE
	(New values are halfed
	  Boxcolor[0] = 0.031;
	Boxcolor[1] = 0.110;
	Boxcolor[2] = 0.341;
	*/
	Boxcolor[3] = color[3];
	
	/*if ( !color ) {
		return;
	}*/

	trap_R_SetColor( color );

	i = 0;

	msgRow = 0;
	msgCol = 0;
	p = cg.adminMsgMsg;
	currRow = p;

	while ( qtrue ) {
		if ( !*p ) {
			break;
		}

		if ( NextWordEndsHere( p ) - currRow > 43 ) { //we need to wrap...
			message[msgRow][msgCol] = '\0';
			currRow = p++;
			++msgRow, msgCol = 0;
			continue;
		}

		message[msgRow][msgCol] = *p;
		++p, ++msgCol;
	}
	
	message[msgRow][msgCol] = '\0';
	++msgRow;

	/* Sorry but this code really didn't work Phenix ... memory errors.
	for (msgRow = 0; msgRow < 35; msgRow++)
	{
		if (cg.adminMsgMsg[i] == '\0')
		{
			break;
		}

		for (msgCol = 0; msgCol < 45; msgCol++)
		{
			if (cg.adminMsgMsg[i] == '\0')
			{
				break;
			}

			if (cg.adminMsgMsg[i] == '\\')
			{
				i++;
				break;
			}

			if ((msgCol >= 30) && (cg.adminMsgMsg[i] == ' '))
			{
				i++;
				break;
			}
			
			if (msgCol == 44) {
				message[msgRow][msgCol] = '-';
			} else {
				message[msgRow][msgCol] = cg.adminMsgMsg[i];
				i++;
			}
		}
	}
	*/

	biggestW = 0;
	for (i = 0; i < msgRow; i++)
	{
		thisMessage = va("%s", message[i]);
		w = UI_ProportionalStringWidth(thisMessage, UI_SMALLFONT);
		
		if (w > biggestW)
		{
			biggestW = w;
		}
	}

	CG_FillRect( 640 - (biggestW + 22), y - (((SMALLCHAR_HEIGHT + 2) * msgRow) + 2), biggestW + 4, ((SMALLCHAR_HEIGHT + 2) * msgRow) + 4, Boxcolor );

	for (i = (msgRow - 1); i >= 0; i--)
	{
		y -= (SMALLCHAR_HEIGHT + 2);
		
		//memset(&thisMessage, 0, sizeof(thisMessage));
		thisMessage = va("%s", message[i]);
		UI_DrawProportionalString(640 - (biggestW + 20), y, thisMessage, UI_SMALLFONT, color);
	}


	trap_R_SetColor( NULL );

}

/*
=================
CG_Draw2D
=================
*/
static void CG_Draw2D( void ) {
	int i;

	//TiM : Testing this API function...
	//trap_R_SetColor( colorTable[ CT_RED ] );
	//trap_R_DrawStretchPic( 100, 100, 800, 600, 0, 0, 0.5, 0.5, cgs.media.charsetPropB );

	// if we are taking a levelshot for the menu, don't draw anything
	if ( cg.levelShot ) {
		return;
	}

	if ( cg_draw2D.integer == 0 ) {
		return;
	}

	if ( cg.snap->ps.pm_type == PM_CCAM ) {
		return;
	}

	if ( cg.snap->ps.pm_type == PM_INTERMISSION ) {
#ifndef FINAL_BUILD
		CG_DrawUpperRight();
#endif
		CG_DrawIntermission();
		return;
	}

	if ( !cg.renderingThirdPerson )
	{
		CG_DrawZoomMask();
	}
	
	//RPG-X: RedTechie - Keep Lagometer on the botum always

	cgs.widescreen.state = WIDESCREEN_RIGHT;
	CG_DrawLagometer();
	cgs.widescreen.state = WIDESCREEN_NONE;

	if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR /*|| (cg.snap->ps.eFlags&EF_ELIMINATED)*/ ) {
//		CG_DrawSpectator();
		CG_DrawCrosshair();
		CG_DrawCrosshairNames();
	} else {
		cgs.widescreen.state = WIDESCREEN_LEFT;
			CG_DrawStatusBar(); //RPG-X: RedTechie - We want health displayed when dead
			// don't draw any status if dead
			if ( cg.snap->ps.stats[STAT_HEALTH] > 1 ) { //RPG-X: RedTechie - No weapons at health 1 (you die at health 1 now)
				//CG_DrawStatusBar();
				CG_DrawAmmoWarning();
				
				cgs.widescreen.state = WIDESCREEN_NONE;
				CG_DrawCrosshair();

				cgs.widescreen.state = WIDESCREEN_CENTER;
				CG_DrawCrosshairNames();

				cgs.widescreen.state = WIDESCREEN_LEFT;
				CG_DrawWeaponSelect();

				cgs.widescreen.state = WIDESCREEN_RIGHT;
				CG_DrawHoldableItem();
				CG_DrawReward();
				CG_DrawAbridgedObjective();

				cgs.widescreen.state = WIDESCREEN_NONE;
			}
			if ( cgs.gametype >= GT_TEAM ) {
				CG_DrawTeamInfo();
			}
		cgs.widescreen.state = WIDESCREEN_NONE;
	}

	if (cg.showObjectives)
	{
		CG_DrawObjectiveInformation();
	}

	CG_DrawVote();
	
	//RPG-X: RedTechie - Moved above others to keep on the bottum
	//CG_DrawLagometer();

	CG_DrawUpperRight();

	CG_DrawLowerRight();

	CG_DrawLowerLeft();

	//RPG-X | Phenix | 08/06/2005
	cgs.widescreen.state = WIDESCREEN_CENTER;
		CG_DrawAdminMsg();
	cgs.widescreen.state = WIDESCREEN_NONE;

	cgs.widescreen.state = WIDESCREEN_CENTER;
		if ( !CG_DrawFollow() ) {
			CG_DrawWarmup();
		}
	cgs.widescreen.state = WIDESCREEN_NONE;

	// don't draw center string if scoreboard is up
	cgs.widescreen.state = WIDESCREEN_CENTER;
		if ( !CG_DrawScoreboard() ) {
			CG_DrawCenterString();
		}
	cgs.widescreen.state = WIDESCREEN_NONE;

	// kef -- need the "use TEAM menu to play" message to draw on top of the bottom bar of scoreboard
	if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR /*|| (cg.snap->ps.eFlags&EF_ELIMINATED)*/ )
	{
		cgs.widescreen.state = WIDESCREEN_CENTER;
			CG_DrawSpectator();
		cgs.widescreen.state = WIDESCREEN_NONE;
	}

	//TiM - Draw teh fl4r3s

	for (i = 0; i < MAX_LENS_FLARES; i++) {
		if ( lensFlare[i].qfull )
			CG_DrawLensFlare( &lensFlare[i] );

		if ( lensFlare[i].upTime + lensFlare[i].holdTime + lensFlare[i].downTime > 0 &&
			cg.time > lensFlare[i].startTime + lensFlare[i].upTime + lensFlare[i].holdTime + lensFlare[i].downTime )
			lensFlare[i].qfull = qfalse;
	}

}

/*
=====================
CG_DrawActive

Perform all drawing needed to completely fill the screen
=====================
*/
void CG_DrawActive( stereoFrame_t stereoView ) {
	float		separation;
	vec3_t		baseOrg;

	// optionally draw the info screen instead
	if ( !cg.snap ) {
		CG_DrawInformation();
		return;
	}

	//vectors needed for tricorder
	AngleVectors (cg.refdefViewAngles, vfwd, vright, vup);
	VectorCopy( vfwd, vfwd_n );
	VectorCopy( vright, vright_n );
	VectorCopy( vup, vup_n );
	VectorNormalize( vfwd_n );
	VectorNormalize( vright_n );
	VectorNormalize( vup_n );

	// optionally draw the tournement scoreboard instead
	if ( (cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR /*|| (cg.snap->ps.eFlags&EF_ELIMINATED)*/)&&
		( cg.snap->ps.pm_flags & PMF_SCOREBOARD ) ) {
		CG_DrawTourneyScoreboard();
		return;
	}

	switch ( stereoView ) {
	case STEREO_CENTER:
		separation = 0;
		break;
	case STEREO_LEFT:
		separation = -cg_stereoSeparation.value / 2;
		break;
	case STEREO_RIGHT:
		separation = cg_stereoSeparation.value / 2;
		break;
	default:
		separation = 0;
		CG_Error( "CG_DrawActive: Undefined stereoView" );
	}


	// clear around the rendered view if sized down
	CG_TileClear();

	// offset vieworg appropriately if we're doing stereo separation
	VectorCopy( cg.refdef.vieworg, baseOrg );
	if ( separation != 0 ) {
		VectorMA( cg.refdef.vieworg, -separation, cg.refdef.viewaxis[1], cg.refdef.vieworg );
	}

	// draw 3D view
	trap_R_RenderScene( &cg.refdef );

	// restore original viewpoint if running stereo
	if ( separation != 0 ) {
		VectorCopy( baseOrg, cg.refdef.vieworg );
	}

	// draw status bar and other floating elements
	CG_Draw2D();
}

