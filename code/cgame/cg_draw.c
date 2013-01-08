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

extern void InitPostGameMenuStruct(void);

static void CG_InterfaceStartup(void);

char *ingame_text[IGT_MAX];		/*	Holds pointers to ingame text */

int zoomFlashTime=0;

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
			lensFlare[i].worldCoord[0] = worldCoord[0];
			lensFlare[i].worldCoord[1] = worldCoord[1];
			lensFlare[i].worldCoord[2] = worldCoord[2];
			lensFlare[i].w1 = w1;
			lensFlare[i].h1 = h1;
			lensFlare[i].glowColor[0] = glowColor[0];
			lensFlare[i].glowColor[1] = glowColor[1];
			lensFlare[i].glowColor[2] = glowColor[2];
			lensFlare[i].glowOffset = glowOffset;
			lensFlare[i].hazeOffset = hazeOffset;
			lensFlare[i].minDist = minDist;
			lensFlare[i].maxDist = maxDist;
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
	
	/*
	 * NOTE: did it this way because most draw functions expect virtual 640x480 coords
	 *	and adjust them for current resolution
	 */
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
	}

	if ( ly > ( lx * grad) ) {/* point is running along the top bar */
		alpha = (float)( 1.0 - ( (float)ly - (float)lymin ) / (float)yDif );
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

	alpha = ( len - (float)min ) / ((float)max - (float)min); /* calculate the alpha */

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
}

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
void CG_DrawHead( float x, float y, float w, float h, int clientNum, vec3_t headAngles ) {
	clipHandle_t	cm;
	clientInfo_t	*ci;
	playerState_t	*ps;
	float		value;
	float			len;
	vec3_t			origin;
	vec3_t			mins, maxs;

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
	// unused function ... lol might be removed
	return;
}

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
		UI_DrawProportionalString( x +40-3, y + 23, ingame_text[IGT_SB_HEALTHBARLCARS], UI_TINYFONT|UI_RIGHT, colorTable[CT_BLACK]); 
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
	vec3_t		angles;
	int y=0;
	vec4_t	whiteA;
	int		x, z, i, h, yZ;
	vec3_t	tmpVec, eAngle, forward, dAngle;
	int healthBarWidth;

	whiteA[0] = whiteA[1] = whiteA[2] = 1.0f;	whiteA[3] = 0.3f;
	
	cent = &cg_entities[cg.snap->ps.clientNum];

	if ( cg_drawStatus.integer == 0 ) {
		return;
	}

	// draw the team background
	CG_DrawTeamBackground( 0, 420, 640, 60, 0.33, cg.snap->ps.persistant[PERS_TEAM], qfalse );

	VectorClear( angles );

	// draw any 3D icons first, so the changes back to 2D are minimized
	y = (SCREEN_HEIGHT - (4*ICON_SIZE) - 20);

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

				//RPG-X: RedTechie - If Dead show them as a medical symbol
				//.number
				if (h/32 < 100 && h/32 > 0) { // Limit Radar Range
					if ( cg_entities[cg.snap->entities[i].number].currentState.eFlags & EF_DEAD )
					{
						if (z > 64)
						{
							CG_DrawStretchPic( 86 - forward[1], 146 - forward[0], 16, 8, 0, 0, 1, 0.5, cgs.media.rd_injured_level );
						}
						else if (z < -64)
						{
							CG_DrawStretchPic( 86 - forward[1], 146 - forward[0], 16, 8, 0, 0.5, 1, 1, cgs.media.rd_injured_level );
						}							
						else
						{
							CG_DrawPic(86 - forward[1], 146 - forward[0], 16, 16, cgs.media.rd_injured_level);
						}
					}
					else
					{
						if ( cgs.clientinfo[cg.snap->entities[i].number].pClass >= 0 )
						{
							radColor[0] = (float)cgs.classData[cgs.clientinfo[cg.snap->entities[i].number].pClass].radarColor[0] / 255.0f;
							radColor[1] = (float)cgs.classData[cgs.clientinfo[cg.snap->entities[i].number].pClass].radarColor[1] / 255.0f;
							radColor[2] = (float)cgs.classData[cgs.clientinfo[cg.snap->entities[i].number].pClass].radarColor[2] / 255.0f;
							radColor[3] = 1.0f;
						}
						else
						{
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
==================
CG_DrawSnapshot
==================
*/
static float CG_DrawSnapshot( float y ) {
	char		*s;
	int			w;

	s = va( "time:%i frametime:%i snap:%i cmd:%i", cg.snap->serverTime, cg.frametime, 
		cg.latestSnapshotNum, cgs.serverCommandSequence );	

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
=====================
CG_DrawUpperRight

=====================
*/
static void CG_DrawUpperRight( void ) {
	float	y;

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

	cgs.widescreen.state = WIDESCREEN_NONE;
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
	float		y1;

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
			item = BG_FindItemForPowerup( (powerup_t)sorted[i] );

		if (NULL == item)
		{
			continue;
		}
		color = 1;

		y -= ICON_SIZE;

		trap_R_SetColor( colors[color] );

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

	y = CG_DrawPickupItem( y );

	cgs.widescreen.state = WIDESCREEN_NONE;
}



//===========================================================================================

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
			cmd.serverTime > cg.time					
	   ) {	
		return;
	}

	// also add text in center of screen
	s = ingame_text[IGT_CONNECTIONINTERRUPTED];
	w = UI_ProportionalStringWidth(s,UI_BIGFONT);
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

	if ( !cg_lagometer.integer ) {
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
=================
CG_DrawSelfdestructTimer
=================
*/
static float CG_DrawSelfdestructTimer( void ) {
	char		*s;
	int			w;
	int			mins, tens, seconds, remainder;
	int			msec;

	msec = cg.selfdestructTime - cg.time;

	if (msec > 0){

		mins = msec / 60000;
		tens = (msec - (mins * 60000)) / 10000;
		seconds = (msec - (mins * 60000) - (tens * 10000)) / 1000;
		remainder = msec - (mins * 60000) - (tens * 10000) - (seconds * 1000);

		s = va( "%i:%i%i.%i", mins, tens, seconds, remainder );
	
		w = UI_ProportionalStringWidth("SELF-DESTRTUCT IN",UI_SMALLFONT);
		UI_DrawProportionalString(320 - (w / 2), 10, "SELF-DESTRTUCT IN", UI_SMALLFONT, colorTable[CT_RED]);
	
		w = UI_ProportionalStringWidth(s,UI_SMALLFONT);
		UI_DrawProportionalString(320 - (w / 2), 30, s, UI_SMALLFONT, colorTable[CT_RED]);
		
	}

	return 0;
}

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

		w = UI_ProportionalStringWidth(linebuffer,UI_BIGFONT);

		x = ( SCREEN_WIDTH - w ) / 2;

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
=================
CG_DrawCrosshair
=================
*/
static void CG_DrawCrosshair(void) {
	float		w, h;
	float		f;
	float		x = 0;
	float		y = 0; //float
	int			weaponCrosshairNum;
	
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

	if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR ) {
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

		VectorCopy( cg.predictedPlayerState.viewangles, pitchConstraint); //cg.predictedPlayerState.viewangles //cg.refdefViewAngles //vieworg

		AngleVectors( pitchConstraint, d_f, NULL, NULL );

		VectorCopy( cg.predictedPlayerState.origin, start);
		if ( !(cg.predictedPlayerState.eFlags & EF_FULL_ROTATE) && Q_fabs( cg.predictedPlayerState.viewangles[PITCH] ) > 89.9f )
			start[2] -= 20;
		else
			start[2] += (float)cg.predictedPlayerState.viewheight * cgs.clientinfo[cg.predictedPlayerState.clientNum].height;

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

			x -= 320;
			y -= 240;
		}
	} else {
		x = cg_crosshairX.integer;
		y = cg_crosshairY.integer;
	}

	//end dCross

	//If admins scan non-players
	if ( cg.predictedPlayerState.weapon == WP_2 && cg.predictedPlayerState.eFlags & EF_FIRING ) {
		if (cg_showEntityNums.integer && cgs.clientinfo[cg.snap->ps.clientNum].isAdmin && cg.crosshairClientNum < ENTITYNUM_WORLD ) {
			vec4_t ccolor;
			ccolor[0] = 0.694f;	//0.9F;//R
			ccolor[1] = 0.816f;	//0.7F;//G
			ccolor[2] = 1.0f;	//0.0F;//B
			ccolor[3] = 0.8f;

			//TiM
			cgs.widescreen.state = WIDESCREEN_CENTER;
			
			UI_DrawProportionalString(x + 320,
										y + 270, 
										va("Entity: %i", cg.crosshairClientNum), 
										UI_CENTER|UI_SMALLFONT, 
										ccolor); //170

		}
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
	vec3_t			center, maxs, mins, top, bottom, topLeft, topRight, bottomLeft, bottomRight;
	vec3_t			worldEast = {1.0f, 0, 0}, worldNorth = {0, 1.0f, 0}, worldUp = {0, 0, 1.0f};
	float			x = 0, y = 0; 
	float			topLeftx, topLefty, topRightx, topRighty, bottomLeftx, bottomLefty, bottomRightx, bottomRighty;
	int				corner, topSize, bottomSize, leftSize, rightSize;
	int				charIndex, classCharIndex, rankCharIndex, ageCharIndex, raceCharIndex, htCharIndex, wtCharIndex, weapCharIndex, healthCharIndex;
	float			lineHorzLength = 8.0f, lineVertLength = 8.0f, lineWidth = 2.0f;
	float			fUpDot, fEastDot, fNorthDot, uNorthDot, uEastDot;
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

	infoStringCount += cg.frametime;
	rankCharIndex = raceCharIndex = classCharIndex = ageCharIndex = htCharIndex = wtCharIndex = weapCharIndex = charIndex = healthCharIndex = floor(infoStringCount/33);
	//TODO: have box scale in from corners of screen?  Or out from center?

	//IDEA:  We COULD actually rotate a wire-mesh version of the crossEnt until it
	//			matches the crossEnt's angles then flash it and pop up this info...
	//			but that would be way too much work for something like this.
	//			Alternately, could rotate a scaled-down fully-skinned version
	//			next to it, but that, too, might be overkill... (plus, model would
	//			need back faces)

	//FIXME: can be optimized...

	//Draw frame around ent's bbox
	//FIXME: make global, do once
	fUpDot = 1.0f - fabs( DotProduct( vfwd_n, worldUp ) );	//1.0 if looking up or down, so use mins and maxs more
	fEastDot = fabs( DotProduct( vfwd_n, worldEast ) );		//1.0 if looking east or west, so use mins[1] and maxs[1] more
	fNorthDot = fabs( DotProduct( vfwd_n, worldNorth ) );	//1.0 if looking north or south, so use mins[0] and maxs[0] more
	uEastDot = fabs( DotProduct( vup_n, worldEast ) );		//1.0 if looking up or down, head towards east or west, so use mins[0] and maxs[0] more
	uNorthDot = fabs( DotProduct( vup_n, worldNorth ) );	//1.0 if looking up or down, head towards north or south, so use mins[1] and maxs[1] more

	VectorCopy( origin, center ); //crossEnt->currentOrigin//cent->lerpOrigin
	VectorCopy( entMaxs, maxs ); //crossEnt->maxs //playerMaxs
	VectorCopy( entMins, mins ); //crossEnt->mins //playerMins

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
				//try to draw at top as to not obscure the tricorder
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
					w = CG_DrawStrlen( showHealth ) * SMALLCHAR_WIDTH;
					Q_strncpyz( showHealth, showHealth, healthCharIndex );
					CG_DrawSmallStringColor( x - w / 2, y + lineWidth, showHealth, color );
					y += SMALLCHAR_HEIGHT;
				}
			}
			break;
		case 5:
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

	VectorCopy( cg.predictedPlayerState.viewangles, pitchConstraint );
	AngleVectors( pitchConstraint, df_f, NULL, NULL );

	VectorMA( start, 8912, df_f, end);

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

		if ( trace.entityNum >= MAX_CLIENTS && !cgs.clientinfo[cg.snap->ps.clientNum].isAdmin/*cg.predictedPlayerState.persistant[PERS_CLASS] != PC_ADMIN*/ ) {
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
	centity_t	*cent;
	int			x, y;
	qboolean		tinyFont;
	int		drawFlags;

	if ( !cg_drawCrosshair.integer ) 
	{
		return;
	}

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
				float	ht = 0;
				float	wt = 0;
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

				irank = score;	//Q_log2( score );

				ci = &cgs.clientinfo[cg.crosshairClientNum];
				//over-ride the color, since we can't get teams in this case
				//use that good old LCARS yellow
				color[0] = 0.694f;	//0.9F;//R
				color[1] = 0.816f;	//0.7F;//G
				color[2] = 1.0f;	//0.0F;//B
				color[3] *= 0.5;

				ht = ci->height * (float)BASE_HEIGHT;
				wt = ci->weight * ci->height * (float)BASE_WEIGHT;
				if ( ci->gender == GENDER_FEMALE ) {
					wt *= (float)FEMALE_OFFSET;	//magic number, women are lighter than men
				}
				
				if ( ci->race && ci->race[0] ) {
					race = ci->race;
					Com_sprintf( racestr, sizeof( racestr ), "%s: %s", "Race", race );
				}

				if ( ci->age && ci->age[0] ) {
					age = ci->age;
					Com_sprintf( agestr, sizeof( agestr ), "%s: %s", "Age", age );
				}

				pClass = cgs.classData[ci->pClass].formalName;

				if ( pClass ) {
					Com_sprintf( classstr, sizeof(classstr), "%s: %s", "Class", pClass );
				}

				if ( cgs.classData[ci->pClass].showRanks ) {
					if ( cgs.ranksData[irank].formalName[0] ) {
						rank = cgs.ranksData[irank].formalName;
						Com_sprintf( rankstr, sizeof( rankstr ), "%s: %s", "Rank", rank );
					}
				}
					
				if ( ci->name && ci->name[0] ) {
					name = ci->name;
				}
				else {
					name = "Data Not Available";	//crossEnt->targetname;
				}
					
				Com_sprintf( namestr, sizeof( namestr), "%s: %s", "Name", name );		

				if ( cent->currentState.weapon != WP_1 )
				{
					if ( cg_weapons[ cent->currentState.weapon ].item->pickup_name ) {
						weap = cg_weapons[ cent->currentState.weapon ].item->pickup_name;
						Com_sprintf( weapstr, sizeof( weapstr), "%s: %s", "Weapon", weap );
					}
				}

				Com_sprintf( htstr, sizeof(htstr), "%s: %4.2f %s","Height", ht, HEIGHT_UNIT );
				Com_sprintf( wtstr, sizeof(wtstr), "%s: %4.2f %s","Weight", wt, WEIGHT_UNIT );

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

				color[0] = 0.694f;	//0.9F;//R
				color[1] = 0.816f;	//0.7F;//G
				color[2] = 1.0f;	//0.0F;//B
				color[3] *= 0.5;

				//TiM: Since dynamic brush ents seem to have no freaking origin in them, let's
				// calc our own using the bounding box dimensions (At least we have those lol )
				VectorAverage( eState->origin2, eState->angles2, origin );

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
					mins, maxs,	renderString,
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

	color[0] = colorTable[CT_YELLOW][0];
	color[1] = colorTable[CT_YELLOW][1];
	color[2] = colorTable[CT_YELLOW][2];

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
	if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR )
	{
		UI_DrawProportionalString(SCREEN_WIDTH/2, SCREEN_HEIGHT - ((BIGCHAR_HEIGHT * 1.50) * 2) , ingame_text[IGT_SPECTATOR], UI_BIGFONT|UI_CENTER, colorTable[CT_LTGOLD1]);
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
	
	UI_DrawProportionalString( 0,  58, s, UI_SMALLFONT, colorTable[CT_YELLOW]);
}

/*
=================
CG_DrawIntermission
=================
*/
static void CG_DrawIntermission( void ) {
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
	const char	*s;

	sec = cg.warmup;
	if ( !sec ) {
		return;
	}

	if ( sec < 0 ) {
		s = ingame_text[IGT_WAITINGFORPLAYERS];		
		w = UI_ProportionalStringWidth(s,UI_BIGFONT);
		UI_DrawProportionalString(320 - w / 2, 40, s, UI_BIGFONT, colorTable[CT_LTGOLD1]);

		cg.warmupCount = 0;
		return;
	}

	char	gamename[1024];

	s = ingame_text[IGT_GAME_FREEFORALL];

	Q_strncpyz( gamename, s, sizeof(gamename) );

	CG_AddGameModNameToGameName( gamename );

	w = UI_ProportionalStringWidth(s,UI_BIGFONT);

	UI_DrawProportionalString((SCREEN_WIDTH/2) , 20,gamename, UI_BIGFONT|UI_CENTER, colorTable[CT_LTGOLD1]);

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
	Boxcolor[3] = color[3];
	
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
		CG_DrawCrosshair();
		CG_DrawCrosshairNames();
	} else {
		cgs.widescreen.state = WIDESCREEN_LEFT;
			CG_DrawStatusBar(); //RPG-X: RedTechie - We want health displayed when dead
			// don't draw any status if dead
			if ( cg.snap->ps.stats[STAT_HEALTH] > 1 ) { //RPG-X: RedTechie - No weapons at health 1 (you die at health 1 now)
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

	CG_DrawSelfdestructTimer();

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
	if ( (cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR )&&
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

