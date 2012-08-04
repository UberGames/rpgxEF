// Copyright (C) 1999-2000 Id Software, Inc.
//
// cg_view.c -- setup all the parameters (position, angle, etc)
// for a 3D rendering
#include "cg_local.h"
#include "cg_screenfx.h"

#define MAX_SHAKE_INTENSITY			16.0f

#define FRAMES_DOOR		16

/*
=============================================================================

  MODEL TESTING

The viewthing and gun positioning tools from Q2 have been integrated and
enhanced into a single model testing facility.

Model viewing can begin with either "testmodel <modelname>" or "testgun <modelname>".

The names must be the full pathname after the basedir, like 
"models/weapons/v_launch/tris.md3" or "players/male/tris.md3"

Testmodel will create a fake entity 100 units in front of the current view
position, directly facing the viewer.  It will remain immobile, so you can
move around it to view it from different angles.

Testgun will cause the model to follow the player around and supress the real
view weapon model.  The default frame 0 of most guns is completely off screen,
so you will probably have to cycle a couple frames to see it.

"nextframe", "prevframe", "nextskin", and "prevskin" commands will change the
frame or skin of the testmodel.  These are bound to F5, F6, F7, and F8 in
q3default.cfg.

If a gun is being tested, the "gun_x", "gun_y", and "gun_z" variables will let
you adjust the positioning.

Note that none of the model testing features update while the game is paused, so
it may be convenient to test with deathmatch set to 1 so that bringing down the
console doesn't pause the game.

=============================================================================
*/

/*
=================
CG_TestModel_f

Creates an entity in front of the current position, which
can then be moved around
=================
*/
void CG_TestModel_f (void) {
	vec3_t		angles;

	memset( &cg.testModelEntity, 0, sizeof(cg.testModelEntity) );
	if ( trap_Argc() < 2 ) {
		return;
	}

	Q_strncpyz (cg.testModelName, CG_Argv( 1 ), MAX_QPATH );
	cg.testModelEntity.hModel = trap_R_RegisterModel( cg.testModelName );

	if ( trap_Argc() == 3 ) {
		cg.testModelEntity.backlerp = atof( CG_Argv( 2 ) );
		cg.testModelEntity.frame = 1;
		cg.testModelEntity.oldframe = 0;
	}
	if (! cg.testModelEntity.hModel ) {
		CG_Printf( "Can't register model\n" );
		return;
	}

	VectorMA( cg.refdef.vieworg, 100, cg.refdef.viewaxis[0], cg.testModelEntity.origin );

	angles[PITCH] = 0;
	angles[YAW] = 180 + cg.refdefViewAngles[1];
	angles[ROLL] = 0;

	AnglesToAxis( angles, cg.testModelEntity.axis );
	cg.testGun = qfalse;
}

/*
=================
CG_TestGun_f

Replaces the current view weapon with the given model
=================
*/
void CG_TestGun_f (void) {
	CG_TestModel_f();
	cg.testGun = qtrue;
	cg.testModelEntity.renderfx = RF_DEPTHHACK | RF_FIRST_PERSON;
}


void CG_TestModelNextFrame_f (void) {
	cg.testModelEntity.frame++;
	CG_Printf( "frame %i\n", cg.testModelEntity.frame );
}

void CG_TestModelPrevFrame_f (void) {
	cg.testModelEntity.frame--;
	if ( cg.testModelEntity.frame < 0 ) {
		cg.testModelEntity.frame = 0;
	}
	CG_Printf( "frame %i\n", cg.testModelEntity.frame );
}

void CG_TestModelNextSkin_f (void) {
	cg.testModelEntity.skinNum++;
	CG_Printf( "skin %i\n", cg.testModelEntity.skinNum );
}

void CG_TestModelPrevSkin_f (void) {
	cg.testModelEntity.skinNum--;
	if ( cg.testModelEntity.skinNum < 0 ) {
		cg.testModelEntity.skinNum = 0;
	}
	CG_Printf( "skin %i\n", cg.testModelEntity.skinNum );
}

static void CG_AddTestModel (void) {
	int		i;

	// re-register the model, because the level may have changed
	cg.testModelEntity.hModel = trap_R_RegisterModel( cg.testModelName );
	if (! cg.testModelEntity.hModel ) {
		CG_Printf ("Can't register model\n");
		return;
	}

	// if testing a gun, set the origin reletive to the view origin
	if ( cg.testGun ) {
		VectorCopy( cg.refdef.vieworg, cg.testModelEntity.origin );
		VectorCopy( cg.refdef.viewaxis[0], cg.testModelEntity.axis[0] );
		VectorCopy( cg.refdef.viewaxis[1], cg.testModelEntity.axis[1] );
		VectorCopy( cg.refdef.viewaxis[2], cg.testModelEntity.axis[2] );

		// allow the position to be adjusted
		for (i=0 ; i<3 ; i++) {
			cg.testModelEntity.origin[i] += cg.refdef.viewaxis[0][i] * cg_gun_x.value;
			cg.testModelEntity.origin[i] += cg.refdef.viewaxis[1][i] * cg_gun_y.value;
			cg.testModelEntity.origin[i] += cg.refdef.viewaxis[2][i] * cg_gun_z.value;
		}
	}

	trap_R_AddRefEntityToScene( &cg.testModelEntity );
}

//============================================================================

/*
=================
CG_CalcVrect

Sets the coordinates of the rendered window
=================
*/
static void CG_CalcVrect (void) {
	int		size;

	// the intermission should allways be full screen
	if ( cg.snap->ps.pm_type == PM_INTERMISSION ) {
		size = 100;
	} else {
		// bound normal viewsize
		if (cg_viewsize.integer < 30) {
			trap_Cvar_Set ("cg_viewsize","30");
			size = 30;
		} else if (cg_viewsize.integer > 100) {
			trap_Cvar_Set ("cg_viewsize","100");
			size = 100;
		} else {
			size = cg_viewsize.integer;
		}

	}
	cg.refdef.width = cgs.glconfig.vidWidth*size * 0.01;
	cg.refdef.width &= ~1;

	cg.refdef.height = cgs.glconfig.vidHeight*size * 0.01;
	cg.refdef.height &= ~1;

	cg.refdef.x = (cgs.glconfig.vidWidth - cg.refdef.width) * 0.5;
	cg.refdef.y = (cgs.glconfig.vidHeight - cg.refdef.height) * 0.5;
}

//==============================================================================

/*==============================================================================
New Third Person Camera Code
TiM: Based off of the logic of the camera code in Raven's Jedi Knight series,
however written by me, and tuned down since EF may not be that fast to handle 
all of it.

Although I consider copying someone else's logic to be somewhat lame, my programming
skills are not yet at the point I could do this by myself.  I understand the concept behind
how the camera is offset and how the relevant angles are calculated, but am unsure how this
is coupled with a non-linear interpolation algorithm.
I am hoping that being able to trace how the JK code works will enlighten me to some 
furthur graphical programming methodology

Meanings of CG variables from JKA:

Notes on the camera viewpoint in and out...

cg.refdef.vieworg
--at the start of the function holds the player actor's origin (center of player model).
--it is set to the final view location of the camera at the end of the camera code.
cg.refdef.viewangles
--at the start holds the client's view angles
--it is set to the final view angle of the camera at the end of the camera code.
*/

//TiM: Static Global Variables
#define CAMERA_DAMP_INTERVAL	50
#define CAMERA_SIZE				4
#define MASK_CAMERACLIP			(MASK_SOLID|CONTENTS_PLAYERCLIP)

//Bounding Boxes for volume traces
static vec3_t cameraMins = { -CAMERA_SIZE, -CAMERA_SIZE, -CAMERA_SIZE };
static vec3_t cameraMaxs = { CAMERA_SIZE, CAMERA_SIZE, CAMERA_SIZE };

//Directional Vectors
vec3_t cameraForward, cameraUp, cameraRight;

vec3_t	cameraFocusAngles,			cameraFocusLoc; //location and view angles of the player's head
vec3_t	cameraIdealTarget,			cameraIdealLoc; //location and view angles of where the camera should be
vec3_t	cameraCurTarget={0,0,0},	cameraCurLoc={0,0,0}; //Current view and location of camera
vec3_t	CameraOldLoc={0,0,0},		cameraNewLoc={0,0,0}; //Backup data for the lerp func

int cameraLastFrame=0;

float	cameraLastYaw=0;
float	cameraStiffFactor=0.0f;

qboolean	freeRotate;

/*
===============
CG_CalcIdealThirdPersonViewTarget
TiM:
First Function : Calculate the point we should be looking at
as long as nothing is in the way
===============
*/
static void CG_CalcIdealThirdPersonViewTarget ( void ) {
	//ATM, vieworg is the base of the feet
	VectorCopy ( cg.refdef.vieworg, cameraFocusLoc );

	//offset the Z value so it lines up with the eyeheight of the player
	if ( freeRotate )
		VectorMA( cameraFocusLoc, (float)cg.predictedPlayerState.viewheight * cgs.clientinfo[cg.predictedPlayerState.clientNum].height, cameraUp, cameraFocusLoc );
	else
		cameraFocusLoc[2] += (float)cg.predictedPlayerState.viewheight * cgs.clientinfo[cg.predictedPlayerState.clientNum].height; //cg.snap->ps.viewheight
	
	// emote based model offset
	if ( cg.predictedPlayerState.stats[EMOTES] & EMOTE_LOWER ) 
	{
		vec3_t	yawForward, forward;
		VectorSet( yawForward, 0, cg.predictedPlayerEntity.pe.legs.yawAngle, 0 );
		AngleVectors( yawForward, forward, NULL, NULL );

		VectorMA( cameraFocusLoc, cgs.clientinfo[cg.predictedPlayerState.clientNum].modelOffset, forward, cameraFocusLoc );
	}

	//Transfer FocusLoc to CamTarget and use that from there
	VectorCopy( cameraFocusLoc, cameraIdealTarget );

	//Add in Horz offset
	if ( cg.zoomedLeft ) {
		float	ratio = cg_thirdPersonRange.value * 0.02f;
		if ( ratio > 1.0f ) ratio = 1.0f;

		cg_thirdPersonHorzOffset.value += ( cg_thirdPersonZoomRate.value * ratio * 0.075 );
	}
	if ( cg.zoomedRight ) {
		float	ratio = cg_thirdPersonRange.value * 0.02f;
		if ( ratio > 1.0f ) ratio = 1.0f;

		cg_thirdPersonHorzOffset.value -= ( cg_thirdPersonZoomRate.value * ratio * 0.075 );
	}

	if ( cg_thirdPersonHorzOffset.value ) {
		VectorMA( cameraIdealTarget, -cg_thirdPersonHorzOffset.value, cameraRight, cameraIdealTarget );
	}

	//Add in the vertOffset
	if ( cg.zoomedUp ) {
		float	ratio = cg_thirdPersonRange.value * 0.02f;
		if ( ratio > 1.0f ) ratio = 1.0f;

		cg_thirdPersonVertOffset.value += ( cg_thirdPersonZoomRate.value * ratio * 0.075 );
	}
	if ( cg.zoomedDown ) {
		float	ratio = cg_thirdPersonRange.value * 0.02f;
		if ( ratio > 1.0f ) ratio = 1.0f;

		cg_thirdPersonVertOffset.value -= ( cg_thirdPersonZoomRate.value * ratio * 0.075 );
	}
	if ( cg_thirdPersonVertOffset.value ) {
		if ( freeRotate ) 
			VectorMA( cameraFocusLoc, cg_thirdPersonVertOffset.value, cameraUp, cameraFocusLoc );
		else
			cameraIdealTarget[2] += cg_thirdPersonVertOffset.value;
	}
}

/*
===============
CG_CalcIdealThirdPersonViewLocation
TiM:
Second Function : Calculate the point we should be looking out
from given all is good :)
===============
*/
static void CG_CalcIdealThirdPersonViewLocation ( void ) {
	//float offset;

	if ( cg.zoomedForward ) {
		cg_thirdPersonRange.value -= ( cg_thirdPersonZoomRate.value * 0.1 );
	}
	else if ( cg.zoomedBackward ) {
		cg_thirdPersonRange.value += ( cg_thirdPersonZoomRate.value * 0.1 );
	}

	VectorMA( cameraIdealTarget, -cg_thirdPersonRange.value, cameraForward, cameraIdealLoc );
}

/*
===============
CG_ResetThirdPersonViewDamp
TiM:
Third Function : Reset all of the lerp and
set it back to normal
===============
*/

void CG_ResetThirdPersonViewDamp ( void ) {
	trace_t tr;
	
	//Clamp the pitch, so it won't cause bugs
	if ( !freeRotate ) 
		cameraFocusAngles[PITCH] = Com_Clamp( -89.0f, 89.0f, cameraFocusAngles[PITCH] );

	//Take our look directions and calculate vector angles
	AngleVectors( cameraFocusAngles, cameraForward, cameraRight, cameraUp );

	//Calc ideal cam target now
	CG_CalcIdealThirdPersonViewTarget();

	//Calc ideal cam view loaction now
	CG_CalcIdealThirdPersonViewLocation();

	//Take our ideal locations, and then set them to our active variables
	VectorCopy( cameraIdealLoc, cameraCurLoc );
	VectorCopy( cameraIdealTarget, cameraCurTarget );

	//Do a trace from the player's head out to the main location, in case something may be in the way
	//This is mainly for stopping things like the camera going thru ceilings n stuff
	CG_Trace( &tr, cameraFocusLoc, cameraMins, cameraMaxs, cameraCurTarget, cg.snap->ps.clientNum, MASK_CAMERACLIP );
	VectorCopy( tr.endpos, cameraCurTarget );

	//Do a trace from the target to our current location to see if there's anything potentially
	//blocking our view
	CG_Trace( &tr, cameraCurTarget, cameraMins, cameraMaxs, cameraCurLoc, cg.snap->ps.clientNum, MASK_CAMERACLIP );
	VectorCopy( tr.endpos, cameraCurLoc );

	//Initialise the lerp data
	cameraLastFrame = cg.time;
	cameraLastYaw = cameraFocusAngles[YAW];
	cameraStiffFactor = 0.0f;
}

/*
======================
CG_UpdateThirdPersonTargetDamp

TiM: From the looks of this, target damp lags
the position of the camera behind a certain amount
when the player moves.  The end result being a more
fluid movement. :)
Still trying to figure out how lerp actually works.
======================
*/
static void CG_UpdateThirdPersonTargetDamp ( void ) {
	trace_t tr;
	vec3_t	targetDiff; //difference between our aimed target and current target
	float dampFactor, dampTime, dampRatio;

	//Just to be on the safe side, let's set the current ideal data again
	CG_CalcIdealThirdPersonViewTarget();

	//if the CVAR says no delay, or if we're currently teleporting, don't do the lerp (Or we could make the player sick lol)
	if ( cg_thirdPersonTargetDamp.value >= 1.0 || cg.thisFrameTeleport || cg.nextFrameTeleport || freeRotate || cg.thirdPersonNoLerp ) {
		VectorCopy( cameraIdealTarget, cameraCurTarget );
	}
	else if ( cg_thirdPersonTargetDamp.value >= 0.0 ) { //Okay, all's good, so let's get lerping lol
		//First, let's get the difference between where we're at, and where we should be
		VectorSubtract( cameraIdealTarget, cameraCurTarget, targetDiff );

		//Ugh.... maaaaath >.<
		//The JKA code says the equation is "(Damp)^(time)", so I'm guessing it's inverse exponential to 
		//get that cool slowy down effect :)
		if ( !freeRotate )
			dampFactor = 1.0 - cg_thirdPersonTargetDamp.value; //yeh, I guess this is the inverse exponential bit.
		else
			dampFactor = 1.0 - Q_fabs( cameraFocusAngles[PITCH] ) / 90.0f;

		dampTime = (float)(cg.time - cameraLastFrame) * (1.0f/(float)CAMERA_DAMP_INTERVAL); //chikushou! I don't know how this time factor is caluclated O_o
		
		//Square this number for each unit of dampTime
		dampRatio = Q_powf( dampFactor, dampTime);

		//Okay, so our current position is calulated as the difference * -ratio + ideal... O_o
		VectorMA( cameraIdealTarget, -dampRatio, targetDiff, cameraCurTarget );
	}

	//Now, let's make sure we didn't lerp our way into a wall or summin
	CG_Trace( &tr, cameraFocusLoc, cameraMins, cameraMaxs, cameraCurTarget, cg.snap->ps.clientNum, MASK_CAMERACLIP );
	if ( tr.fraction < 1.0 ) {
		VectorCopy( tr.endpos, cameraCurTarget );
	}
}

/*
===============
CG_UpdateThirdPersonCameraDamp

TiM: Okay, since the above function lagged
the camera's position, logic stands to reason
this one lags the camera's actual angles.
With the dynamic crosshair enabled, this should look pretty damn sweet. :)
Looks somewhat similar to Target Damp
================
*/
static void CG_UpdateThirdPersonCameraDamp ( void ) {
	trace_t	tr;
	vec3_t	locationDiff;
	float dampFactor=0.0, dampTime, dampRatio;

	//Initialise our goal angle
	CG_CalcIdealThirdPersonViewLocation();

	//If we need to do any damping at all
	if ( cg_thirdPersonCameraDamp.value != 0.0 ) {
		float pitch;

		//get pitch, and make it all positive.  Direction don't matter here
		pitch = Q_fabs( cameraFocusAngles[PITCH] );

		//If we're floating and rotate all around, perform this so the damping isn't so extreme
		/*if ( pitch > 89.0f ) {
			pitch = 90.0f - ( pitch - 90.0f );
		}*/

		//The JKA code says these statments are to get it to damp less the more u look up.
		//Makes sense. Still looking how tho lol
		pitch /= 115.0; //magic number I guess lol.
		dampFactor = (1.0-cg_thirdPersonCameraDamp.value) * ( pitch * pitch );
		
		dampFactor += cg_thirdPersonCameraDamp.value;		

		//the stiff factor is based off speed, so faster yaw changes seem stiffer
		if ( cameraStiffFactor > 0.0f ) {
			dampFactor += ( 1.0 - dampFactor) * cameraStiffFactor;
		}
	}

	//if our result meant no damping, or we're actively teleporting
	//sigh I guess we'll need to disable dampin upon rotation.  it causes absolute hell at the model's polar angles
	if ( dampFactor >= 1.0 || cg.thisFrameTeleport || cg.nextFrameTeleport || freeRotate || cg.thirdPersonNoLerp ) {
		VectorCopy( cameraIdealLoc, cameraCurLoc );
	}
	else if ( dampFactor >= 0.0 ) {
		//First, let's get the difference between where we're at, and where we should be
		VectorSubtract( cameraIdealLoc, cameraCurLoc, locationDiff );

		//Ugh.... maaaaath >.<
		//The JKA code says the equation is "(Damp)^(time)", so I'm guessing it's inverse exponential to 
		//get that cool slowy down effect :)
		dampFactor = 1.0 - dampFactor; //yeh, I guess this is the inverse exponential bit.
		dampTime = (float)(cg.time - cameraLastFrame) * (1.0/(float)CAMERA_DAMP_INTERVAL); //chikushou! I don't know how this time factor is caluclated O_o
		
		//Square this number for each unit of dampTime
		dampRatio = Q_powf( dampFactor, dampTime);

		//Okay, so our current position is calulated as the difference * -ratio + ideal... O_o
		VectorMA( cameraIdealLoc, -dampRatio, locationDiff, cameraCurLoc );		
	}

	//Now do a trace to see if we're all good for this loc
	CG_Trace( &tr, cameraCurTarget, cameraMins, cameraMaxs, cameraCurLoc, cg.snap->ps.clientNum, MASK_CAMERACLIP );

	//Now Raven added a huge hacky code tidbit at this stage regarding being on moving entities
	//I'll see if I can get away without instituting it...
	if ( tr.fraction < 1.0 ) {
		VectorCopy( tr.endpos, cameraCurLoc );
	}
}

/*
===============
CG_OffsetThirdPersonView

TiM: The end is nigh!
So, all of the funky code above is 
finally consolidated into this main function.

Let's see if I can understand this in any way lol
===============
*/
static void CG_OffsetThirdPersonView( void ) {
	vec3_t	diff;
	float deltaYaw;
	qboolean neg=qfalse;

	cameraStiffFactor = 0.0f;

	//TiM: change the math a tad if we're in free rotate mode
	if ( cg.predictedPlayerEntity.currentState.eFlags & EF_FULL_ROTATE )
		freeRotate = qtrue;
	else
		freeRotate = qfalse;

	//copy in our raw data values
	VectorCopy( cg.refdefViewAngles, cameraFocusAngles );

	//Add a rotation offset for viewAngle
	if ( cg.zoomAngleRight ) {
		cg_thirdPersonAngle.value -= ( cg_thirdPersonZoomRate.value * 0.1 );
	}
	if ( cg.zoomAngleLeft ) {
		cg_thirdPersonAngle.value += ( cg_thirdPersonZoomRate.value * 0.1 );
	}
	cameraFocusAngles[YAW] -= cg_thirdPersonAngle.value; //TiM - offset so it swings the right way lol

	//Add in pitch
	if ( cg.zoomPitchUp ) {
		cg_thirdPersonPitchOffset.value += ( cg_thirdPersonZoomRate.value * 0.075 );
		cg_thirdPersonPitchOffset.value = Com_Clamp( -89.0f, 89.0f, cg_thirdPersonPitchOffset.value );
	}
	if ( cg.zoomPitchDown ) {
		cg_thirdPersonPitchOffset.value -= ( cg_thirdPersonZoomRate.value * 0.075 );
		cg_thirdPersonPitchOffset.value = Com_Clamp( -89.0f, 89.0f, cg_thirdPersonPitchOffset.value );
	}
	cameraFocusAngles[PITCH] += cg_thirdPersonPitchOffset.value;

	//if something messed up, or we're just starting, initiliaze sample
	if ( cameraLastFrame == 0 || cameraLastFrame > cg.time ) {
		CG_ResetThirdPersonViewDamp();
	}
	else {
		//Cap the final angles :)
		if ( !freeRotate ) {
			cameraFocusAngles[PITCH] = Com_Clamp( -80.0, 89.0, cameraFocusAngles[PITCH] );
		}

		AngleVectors( cameraFocusAngles, cameraForward, cameraRight, cameraUp );

		deltaYaw = fabs( cameraFocusAngles[YAW] - cameraLastYaw );
		//if we exceeded our norms, stick it back
		if (deltaYaw > 180.0f ) {
			deltaYaw = fabs( deltaYaw - 360.0f );
		}

		cameraStiffFactor = deltaYaw / (float)(cg.time-cameraLastFrame);
		if ( cameraStiffFactor < 1.0 ) {
			cameraStiffFactor = 0.0;
		}
		else if ( cameraStiffFactor > 2.5 ) {
			cameraStiffFactor = 0.75;
		}
		else {
			cameraStiffFactor = (cameraStiffFactor-1.0f)*0.5f;
		}
		cameraLastYaw = cameraFocusAngles[YAW];

		CG_UpdateThirdPersonTargetDamp();
		CG_UpdateThirdPersonCameraDamp();
	}

	VectorSubtract( cameraCurTarget, cameraCurLoc, diff );

	//if we're hitting something, use cameraForward to calc new angles
	if ( VectorNormalize(diff) == 0 || diff[0] == 0 || diff[1] == 0 ) {
		VectorCopy( cameraForward, diff );
	}

	//Hack-a-dood-do.  vectoangles cannot comprehend if a player is upside-down.
	//It assumes it's just an opposite direction vector, so everything is rendered the right way up. >.<
	//To fix this, I'll hackily copy the viewangle pitch data, and then reset the angles afterwards

	if ( freeRotate && Q_fabs( cg.refdefViewAngles[PITCH] ) > 90.0f )
		neg = qtrue;

	vectoangles( diff, cg.refdefViewAngles );

	//Also if rotating, provide an offset when players turn fully upside down
	if ( freeRotate && neg ) {
		cg.refdefViewAngles[ROLL] -= 180; //AngleNormalize360( cg.refdefViewAngles[YAW] - 180);
	}

	/*if ( cg_thirdPersonHorzOffset.value != 0.0f ) {
		AnglesToAxis( cg.refdefViewAngles, cg.refdef.viewaxis );
		VectorMA( cameraCurLoc, cg_thirdPersonHorzOffset.value, cg.refdef.viewaxis[1], cameraCurLoc );
	}*/	

	//And update our origin lol
	VectorCopy( cameraCurLoc, cg.refdef.vieworg );

	cameraLastFrame = cg.time;
}

#define	FOCUS_DISTANCE	512 //512
/*static void CG_OffsetThirdPersonView( void ) {
	vec3_t		forward, right, up;
	vec3_t		view;
	vec3_t		focusAngles;
	trace_t		trace;
	static	vec3_t	mins = { -4, -4, -4 };
	static	vec3_t	maxs = { 4, 4, 4 };
	vec3_t		focusPoint;
	float		focusDist;
	float		forwardScale, sideScale;
	char		medicrevive[32];
	int		medicrevive_int;
	vec3_t		camPlayerPos; //TiM
	
	//cg.refdef.vieworg[2] += cg.predictedPlayerState.viewheight;
	cg.refdef.vieworg[2] += cg.predictedPlayerState.viewheight * cgs.clientinfo[cg.predictedPlayerState.clientNum].height;

	VectorCopy( cg.refdefViewAngles, focusAngles );
	VectorCopy( cg.refdef.vieworg, camPlayerPos); //Copy the values b4 we offset;

	//RPG-X: TiM - Incorporated offsets so third person can be more dynamic
	//Woo! I figured out how AngleVectors and VectorMA work!! ^_^
	AngleVectors( cg.refdefViewAngles, NULL, right, NULL);
	VectorMA( cg.refdef.vieworg, cg_thirdPersonHorzOffset.value, right, cg.refdef.vieworg );
	//cg.refdef.vieworg[0] += cg_thirdPersonHorzOffset.value;
	cg.refdef.vieworg[2] += cg_thirdPersonVertOffset.value;

	
	// if dead, look at killer
	//RPG-X: Fix camera movment when play dies with medics revive turned on
	trap_Cvar_VariableStringBuffer( "rpg_medicsrevive", medicrevive, 32 );
	medicrevive_int = atoi(medicrevive);
	
	//TiM: Meh, you don't spin around to look at your killer in real life. O_o
	//Plus, this screws up the model system :(
	if(medicrevive_int == 1){
		if ( cg.predictedPlayerState.stats[STAT_HEALTH] <= 1 ) {

			focusAngles[YAW] = cg_entities[cg.predictedPlayerState.clientNum].pe.legs.yawAngle;
			cg.refdefViewAngles[YAW] = cg_entities[cg.predictedPlayerState.clientNum].pe.legs.yawAngle;
			//focusAngles[YAW] = cg.predictedPlayerState.stats[STAT_DEAD_YAW];
			//cg.refdefViewAngles[YAW] = cg.predictedPlayerState.stats[STAT_DEAD_YAW];
		}
	}else{
		if ( cg.predictedPlayerState.stats[STAT_HEALTH] <= 0 ) {
			
			focusAngles[YAW] = cg_entities[cg.predictedPlayerState.clientNum].pe.legs.yawAngle;
			cg.refdefViewAngles[YAW] = cg_entities[cg.predictedPlayerState.clientNum].pe.legs.yawAngle;
			//focusAngles[YAW] = cg.predictedPlayerState.stats[STAT_DEAD_YAW];
			//cg.refdefViewAngles[YAW] = cg.predictedPlayerState.stats[STAT_DEAD_YAW];
		}
	}

	if ( focusAngles[PITCH] > 89.9 )
	{
		focusAngles[PITCH] = 89.9f; // don't go too far overhead - has to be under 90 or bad things happen
	}
	else if ( focusAngles[PITCH] < -89.9 ) //89 - Stop from going through legs
	{
		focusAngles[PITCH] = -89.9f;
	}

	if ( cg.refdefViewAngles[PITCH] > 89.9 )
	{
		cg.refdefViewAngles[PITCH] = 89.9f; // don't go too far overhead - has to be under 90 or bad things happen
	}
	else if ( cg.refdefViewAngles[PITCH] < -79.9 ) //89 - Stop from going through legs
	{
		cg.refdefViewAngles[PITCH] = -79.9f;
	}

	AngleVectors( focusAngles, forward, NULL, NULL );

	VectorMA( cg.refdef.vieworg, FOCUS_DISTANCE, forward, focusPoint );

	VectorCopy( cg.refdef.vieworg, view );

	view[2] += 16;

	//cg.refdefViewAngles[PITCH] *= 0.5;

	AngleVectors( cg.refdefViewAngles, forward, right, up );

	//VectorScale( forward, cg_thirdPersonAngle.value, normalize );
	forwardScale = VectorNormalize( forward ); //cos( cg_thirdPersonAngle.value / 180 * M_PI );

	///VectorScale( right, cg_thirdPersonAngle.value, normalize );
	sideScale = VectorNormalize( right );//sin( cg_thirdPersonAngle.value / 180 * M_PI );
	VectorMA( view, -cg_thirdPersonRange.value * forwardScale, forward, view );
	VectorMA( view, -cg_thirdPersonRange.value * sideScale, right, view );

	// trace a ray from the origin to the viewpoint to make sure the view isn't
	// in a solid block.  Use an 8 by 8 block to prevent the view from near clipping anything

	//TiM : Sometimes if the value of these variables is set to extreme numbers, they'll go thru walls. O_o
	//This trace function is to fix that.
	//If player is using these CVARs...
	if ( cg_thirdPersonVertOffset.value != 0 || cg_thirdPersonHorzOffset.value != 0) {
		//Do a trace from playermodel's head to our view location
		CG_Trace( &trace, camPlayerPos, mins, maxs, view, cg.predictedPlayerState.clientNum, MASK_SOLID );

		//Okay, the trace hit something... O_o
		if ( trace.fraction != 1.0 ) {
			//copy where it hit to our view origin. :)
			VectorCopy( trace.endpos, cg.refdef.vieworg );
		}
	}

	CG_Trace( &trace, cg.refdef.vieworg, mins, maxs, view, cg.predictedPlayerState.clientNum, MASK_SOLID );

	if ( trace.fraction != 1.0 ) {
		VectorCopy( trace.endpos, view );
		view[2] += (1.0 - trace.fraction) * 32;
		// try another trace to this position, because a tunnel may have the ceiling
		// close enogh that this is poking out

		CG_Trace( &trace, cg.refdef.vieworg, mins, maxs, view, cg.predictedPlayerState.clientNum, MASK_SOLID );
		VectorCopy( trace.endpos, view );
	}

	VectorCopy( view, cg.refdef.vieworg );

	// select pitch to look at focus point from vieword
	VectorSubtract( focusPoint, cg.refdef.vieworg, focusPoint );
	focusDist = sqrt( focusPoint[0] * focusPoint[0] + focusPoint[1] * focusPoint[1] );
	if ( focusDist < 1 ) {
		focusDist = 1;	// should never happen
	}
	//cg.refdefViewAngles[PITCH] = -180 / M_PI * atan2( focusPoint[2], focusDist );
	cg.refdefViewAngles[YAW] -= cg_thirdPersonAngle.value;
}*/

// this causes a compiler bug on mac MrC compiler
static void CG_StepOffset( void ) {
	int		timeDelta;
	
	// smooth out stair climbing
	timeDelta = cg.time - cg.stepTime;
	if ( timeDelta < STEP_TIME ) {
		cg.refdef.vieworg[2] -= cg.stepChange 
			* (STEP_TIME - timeDelta) / STEP_TIME;
	}
}

/*
===============
CG_OffsetFirstPersonView

===============
*/
static void CG_OffsetFirstPersonView( void ) {
	float			*origin;
	float			*angles;
	float			bob;
	float			ratio;
	float			delta;
	float			speed;
	float			f;
	vec3_t			predictedVelocity;
	int				timeDelta;
	char			medicrevive[32];
	int				medicrevive_int;
	
	if ( cg.snap->ps.pm_type == PM_INTERMISSION ) {
		return;
	}

	origin = cg.refdef.vieworg;
	angles = cg.refdefViewAngles;

	// emote based model offset
	if ( cg.predictedPlayerState.stats[EMOTES] & EMOTE_LOWER ) 
	{
		vec3_t	yawForward, forward;
		VectorSet( yawForward, 0, cg.predictedPlayerEntity.pe.legs.yawAngle, 0 );
		AngleVectors( yawForward, forward, NULL, NULL );

		VectorMA( origin, cgs.clientinfo[cg.predictedPlayerState.clientNum].modelOffset, forward, origin );
	
		//CG_Printf(S_COLOR_RED "%i\n", cgs.clientinfo[cg.predictedPlayerState.clientNum].modelOffset );
	}

	// if dead, fix the angle and don't add any kick
	//RPG-X: Fix camera movment when play dies with medics revive turned on
	trap_Cvar_VariableStringBuffer( "rpg_medicsrevive", medicrevive, 32 );
	medicrevive_int = atoi(medicrevive);
	if(medicrevive_int == 1){
	if ( cg.snap->ps.stats[STAT_HEALTH] <= 1 ) {
		angles[ROLL] = 40;
		angles[PITCH] = -15;
		//angles[YAW] = cg.snap->ps.stats[STAT_DEAD_YAW];
		angles[YAW] = cg_entities[cg.predictedPlayerState.clientNum].pe.legs.yawAngle;
		origin[2] += cg.predictedPlayerState.viewheight;
		return;
	}
	}else{
		if ( cg.snap->ps.stats[STAT_HEALTH] <= 0 ) {
		angles[ROLL] = 40;
		angles[PITCH] = -15;
		//angles[YAW] = cg.snap->ps.stats[STAT_DEAD_YAW];
		angles[YAW] = cg_entities[cg.predictedPlayerState.clientNum].pe.legs.yawAngle;
		origin[2] += cg.predictedPlayerState.viewheight;
		return;
		}
	}

	// add angles based on weapon kick
	VectorAdd (angles, cg.kick_angles, angles);

	// add angles based on damage kick
	if ( cg.damageTime ) {
		ratio = cg.time - cg.damageTime;
		if ( ratio < DAMAGE_DEFLECT_TIME ) {
			ratio /= DAMAGE_DEFLECT_TIME;
			angles[PITCH] += ratio * cg.v_dmg_pitch;
			angles[ROLL] += ratio * cg.v_dmg_roll;
		} else {
			ratio = 1.0 - ( ratio - DAMAGE_DEFLECT_TIME ) / DAMAGE_RETURN_TIME;
			if ( ratio > 0 ) {
				angles[PITCH] += ratio * cg.v_dmg_pitch;
				angles[ROLL] += ratio * cg.v_dmg_roll;
			}
		}
	}

	// add pitch based on fall kick
#if 0
	ratio = ( cg.time - cg.landTime) / FALL_TIME;
	if (ratio < 0)
		ratio = 0;
	angles[PITCH] += ratio * cg.fall_value;
#endif

	// add angles based on velocity
	VectorCopy( cg.predictedPlayerState.velocity, predictedVelocity );

	delta = DotProduct ( predictedVelocity, cg.refdef.viewaxis[0]);
	angles[PITCH] += delta * cg_runpitch.value;
	
	delta = DotProduct ( predictedVelocity, cg.refdef.viewaxis[1]);
	angles[ROLL] -= delta * cg_runroll.value;

	// add angles based on bob

	// make sure the bob is visible even at low speeds
	speed = cg.xyspeed > 200 ? cg.xyspeed : 200;

	delta = cg.bobfracsin * cg_bobpitch.value * speed;
	if (cg.predictedPlayerState.pm_flags & PMF_DUCKED)
		delta *= 3;		// crouching
	angles[PITCH] += delta;
	delta = cg.bobfracsin * cg_bobroll.value * speed;
	if (cg.predictedPlayerState.pm_flags & PMF_DUCKED)
		delta *= 3;		// crouching accentuates roll
	if (cg.bobcycle & 1)
		delta = -delta;
	angles[ROLL] += delta;

//===================================

	// add view height
	//origin[2] += cg.predictedPlayerState.viewheight;
	origin[2] += (float)cg.predictedPlayerState.viewheight * cgs.clientinfo[cg.predictedPlayerState.clientNum].height;
	//TiM: Model system enhancements

	// smooth out duck height changes
	timeDelta = cg.time - cg.duckTime;
	if ( timeDelta < DUCK_TIME) {
		cg.refdef.vieworg[2] -= cg.duckChange 
			* (DUCK_TIME - timeDelta) / DUCK_TIME;
	}

	// add bob height
	bob = cg.bobfracsin * cg.xyspeed * cg_bobup.value;
	if (bob > 6) {
		bob = 6;
	}

	origin[2] += bob;


	// add fall height
	delta = cg.time - cg.landTime;
	if ( delta < LAND_DEFLECT_TIME ) {
		f = delta / LAND_DEFLECT_TIME;
		cg.refdef.vieworg[2] += cg.landChange * f;
	} else if ( delta < LAND_DEFLECT_TIME + LAND_RETURN_TIME ) {
		delta -= LAND_DEFLECT_TIME;
		f = 1.0 - ( delta / LAND_RETURN_TIME );
		cg.refdef.vieworg[2] += cg.landChange * f;
	}

	// add step offset
	CG_StepOffset();

	// add kick offset

	VectorAdd (origin, cg.kick_origin, origin);

	//TiM : For rotated players
	//if ( (cg.predictedPlayerEntity.currentState.eFlags & EF_FULL_ROTATE) && Q_fabs( angles[PITCH] ) > 89 )
		//angles[ROLL] += 180;

	// pivot the eye based on a neck length
//#if 0
	{
#define	NECK_LENGTH		8//8
	vec3_t			forward, up;
 
	cg.refdef.vieworg[2] -= NECK_LENGTH;
	AngleVectors( cg.refdefViewAngles, forward, NULL, up );
	VectorMA( cg.refdef.vieworg, 3, forward, cg.refdef.vieworg );
	VectorMA( cg.refdef.vieworg, NECK_LENGTH, up, cg.refdef.vieworg );
	}
//#endif
}

/*
-------------------------
CGCam_Shake
-------------------------
*/

void CG_CameraShake( float intensity, int duration, qboolean addRumbleSound )
{
	if ( intensity > MAX_SHAKE_INTENSITY )
		intensity = MAX_SHAKE_INTENSITY;

	cg.shake_intensity = intensity;
	cg.shake_duration = duration;
	cg.shake_start = cg.time;
}




/*
-------------------------
CG_UpdateShake

This doesn't actually affect the camera's info, but passed information instead
-------------------------
*/

extern void CG_ClientShakeCamera( void );

void CG_UpdateCameraShake( vec3_t origin, vec3_t angles )
{
	vec3_t	curOrigin, curAngle; //moveDir,
	//vec3_t	zero = {0, 0, 0};
	float	intensity_scale, intensity;
	//float	ranIntensity;
	float	ratio;
	int		i;

	//TiM - restart a server loop shake
	if ( cg.shake_duration <= 0 && cg.shake_serverIndex > (cg.time - cgs.levelStartTime ) ) {
		CG_ClientShakeCamera();
	}

	if ( cg.shake_duration <= 0 ) {
		//VectorSet( cg.shake_LastOrigin, 0, 0, 0 );
		//VectorSet( cg.shake_LastAngle, 0, 0, 0 );

		//VectorSet( cg.shake_LerpOrigin, 0, 0, 0 );
		//VectorSet( cg.shake_LerpAngle, 0, 0, 0 );

		memset( &cg.shake_LastOrigin, 0, sizeof( cg.shake_LastOrigin ) );
		memset( &cg.shake_LastAngle, 0, sizeof( cg.shake_LastAngle ) );

		memset( &cg.shake_LerpOrigin, 0, sizeof( cg.shake_LerpOrigin ) );
		memset( &cg.shake_LerpAngle, 0, sizeof( cg.shake_LerpAngle ) );

		return;
	}

	//This is designed to try and make it lerp back to normal at the end
	if ( cg.time > ( cg.shake_start + cg.shake_duration ) )
	{
		cg.shake_intensity = 0;
		cg.shake_duration = 0;
		cg.shake_start = 0;

		return;
	}

	//intensity_scale now also takes into account FOV with 90.0 as normal
	intensity_scale = 1.0f - ( (float) ( cg.time - cg.shake_start ) / (float) cg.shake_duration ) * (cg.refdef.fov_x/90.0f);

	intensity = cg.shake_intensity * intensity_scale;

	//LerpCode
	if ( cg.time > cg.shake_nextLerp ) {

		VectorCopy( cg.shake_LerpOrigin, cg.shake_LastOrigin );
		VectorCopy( cg.shake_LerpAngle, cg.shake_LastAngle );
		
		//ranIntensity = flrandom( ( 10000.0f * ( 1.0f - Q_fabs( intensity ) ) ), ( 30000.0f * ( 1.0f - Q_fabs( intensity ) )) );

		cg.shake_lastLerp = cg.shake_nextLerp;
		//cg.shake_nextLerp = cg.time + (int)ranIntensity;
		cg.shake_nextLerp = cg.time + irandom( 30, 40 );

		if( cg.shake_nextLerp > ( cg.shake_start + cg.shake_duration ) ) {
			cg.shake_nextLerp = (cg.shake_start + cg.shake_duration);
			VectorSet( cg.shake_LerpOrigin, 0, 0, 0 );
			VectorSet( cg.shake_LerpAngle, 0, 0, 0 );
		}
		else {
			for (i=0; i < 3; i++ ) {
				cg.shake_LerpOrigin[i] = ( crandom() * intensity );
				cg.shake_LerpAngle[i] = ( crandom() * intensity );
			}

			//If we're moving out of our boundary, away from the player... >.<
			//clamp it bak in by inverting it
			for ( i = 0; i < 3; i++ ) {
				if ( ( cg.shake_LerpOrigin[i] + cg.shake_LastOrigin[i] ) > ( origin[i] * intensity ) ) {
					cg.shake_LerpOrigin[i] = -(cg.shake_LerpOrigin[i]);
				}

				if ( ( cg.shake_LerpAngle[i] + cg.shake_LastAngle[i] ) > ( angles[i] * intensity ) ) {
					cg.shake_LerpAngle[i] = -(cg.shake_LerpAngle[i]);
				}
			}

			/*i = 0;
			while ( 1 ) {
				if ( ( cg.shake_LerpOrigin[i] + cg.shake_LastOrigin[i] ) > ( origin[i] * intensity ) || ( cg.shake_LerpAngle[i] + cg.shake_LastAngle[i] ) > ( angles[i] * intensity ) ) {
					cg.shake_LerpOrigin[i] = crandom() * intensity;
					cg.shake_LerpAngle[i] = crandom() * intensity;
				}
				else {
					if ( i == 2 ) {
						break;
					}
				}
				i++;
				if ( i == 3 ) {
					i= 0;
				}
			}*/
		}
	}

	//Com_Printf( S_COLOR_RED "NextLerp: %i, origin = { %f, %f, %f }\n", cg.shake_nextLerp, origin[0], origin[1], origin[2] );

	//FIXME: Lerp
	//TiM : Doing that
	
	ratio = ((float)( cg.time - cg.shake_lastLerp ) / (float)( cg.shake_nextLerp - cg.shake_lastLerp ) );
	if (ratio < 0 )
		ratio = 0.0f;
	else if (ratio > 1 )
		ratio = 1.0f;

	for ( i = 0; i < 3; i++ ) {
		curOrigin[i] = ratio * (float)( (origin[i] + cg.shake_LerpOrigin[i] ) - ( origin[i] + cg.shake_LastOrigin[i] ) ); //origin
	}
	
	//Move the camera
	//VectorAdd( origin, curOrigin, origin );
	VectorAdd( origin, cg.shake_LastOrigin, origin );
	VectorAdd( origin, curOrigin, origin );

	/*for ( i=0; i < 3; i++ )
		moveDir[i] = ( crandom() * intensity );*/
	for ( i =0; i < 3; i++ ) {
		curAngle[i] = ratio * (float)( ( angles[i] + cg.shake_LerpAngle[i] ) - ( angles[i] + cg.shake_LastAngle[i] ) ); //angles
	}

	//FIXME: Lerp

	//Move the angles
	//VectorAdd( angles, curAngle, angles );
	VectorAdd( angles, cg.shake_LastAngle, angles );
	VectorAdd( angles, curAngle, angles );

	//Com_Printf( S_COLOR_RED "ratio: %f, origin = { %f, %f, %f }\n", ratio, origin[0], origin[1], origin[2] );
}


//======================================================================

void CG_ZoomDown_f( void )
{ 
	//if we're not holding a rifle or TR-116, don't draw
	if ( !( cg.snap->ps.weapon == WP_6 || cg.snap->ps.weapon == WP_7 ) ) {
		cg.zoomed = qfalse;
		cg.zoomLocked = qfalse;
		return;
	}

	/*if ( cg.snap->ps.persistant[PERS_CLASS] == PC_NOCLASS 
		||	cg.snap->ps.persistant[PERS_CLASS] != PC_SECURITY 
		&& cg.snap->ps.persistant[PERS_CLASS] != PC_ALPHAOMEGA22 
		&&  cg.snap->ps.persistant[PERS_CLASS] != PC_ADMIN )
	{//in a class-based game, only these can zoom
		cg.zoomed = qfalse;
		cg.zoomLocked = qfalse;
		return;
	}*/

	// The zoom hasn't been started yet, so do it now
	if ( !cg.zoomed )
	{
		cg.zoomLocked = qfalse;
		cg.zoomed = qtrue;
		cg_zoomFov.value = cg_fov.value;
		cg.zoomTime = cg.time;
		if ( cg.snap->ps.weapon == WP_7 ) {
			trap_S_StartSound( cg.refdef.vieworg, ENTITYNUM_WORLD, CHAN_AUTO, cgs.media.zoomStart116 );
		}
		else {
			trap_S_StartSound( cg.refdef.vieworg, ENTITYNUM_WORLD, CHAN_AUTO, cgs.media.zoomStart );
		}
		return;
	}

	// Can only snap out of the zoom mode if it has already been locked (CG_ZoomUp_f has been called)
	if ( cg.zoomLocked )
	{
		// Snap out of zoom mode
		cg.zoomed = qfalse;
		cg.zoomTime = cg.time;

		if ( cg.snap->ps.weapon == WP_7 ) {
			trap_S_StartSound( cg.refdef.vieworg, ENTITYNUM_WORLD, CHAN_AUTO, cgs.media.zoomEnd116 );
		}
		else {
			trap_S_StartSound( cg.refdef.vieworg, ENTITYNUM_WORLD, CHAN_AUTO, cgs.media.zoomEnd );
		}
	}
}
 
void CG_ZoomUp_f( void )
{ 

	if ( cg.zoomed ) {
		// Freeze the zoom mode
		cg.zoomLocked = qtrue;
	}
}

/*
====================
CG_CalcFov

Fixed fov at intermissions, otherwise account for fov variable and zooms.
====================
*/
#define	WAVE_AMPLITUDE	1
#define	WAVE_FREQUENCY	0.4

#define FOV_MAX			120

static int CG_CalcFov( void ) {
	float	x;
	float	phase;
	float	v;
	int		contents;
	float	fov_x, fov_y;
	float	zoomFov;
	float	f;
	int		inwater;
	qboolean	warpEffect=qfalse;

	if ( cg.predictedPlayerState.pm_type == PM_INTERMISSION ) {
		// if in intermission, use a fixed value
		fov_x = 90;
	} else {
		// user selectable
		if ( cgs.dmflags & DF_FIXED_FOV ) {
			// dmflag to prevent wide fov for all clients
			fov_x = 80;
		} else {
			fov_x = cg_fov.value;
			if ( fov_x < 1 ) {
				fov_x = 1;
			} else if ( fov_x > FOV_MAX ) {
				fov_x = FOV_MAX;
			}
		}

		// account for zooms
		zoomFov = cg_zoomFov.value;
		if ( zoomFov < 1 ) {
			zoomFov = 1;
		} else if ( zoomFov > FOV_MAX) {
			zoomFov = FOV_MAX;
		}

		// Disable zooming when in third person
		if ( cg.zoomed && !cg.renderingThirdPerson )
		{
			if ( !cg.zoomLocked )
			{
				// Interpolate current zoom level
				cg_zoomFov.value = cg_fov.value - ((float)( cg.time - cg.zoomTime ) / ZOOM_IN_TIME + ZOOM_START_PERCENT) 
									* ( cg_fov.value - MAX_ZOOM_FOV );

				// Clamp zoomFov
				if ( cg_zoomFov.value < MAX_ZOOM_FOV )
				{
					cg_zoomFov.value = MAX_ZOOM_FOV;
				}
				else if ( cg_zoomFov.value > cg_fov.value )
				{
					cg_zoomFov.value = cg_fov.value;
				}
				else
				{//still zooming
					static int zoomSoundTime = 0;

					if ( zoomSoundTime < cg.time )
					{
						trap_S_StartSound( cg.refdef.vieworg, ENTITYNUM_WORLD, CHAN_LOCAL, cgs.media.zoomLoop );
						zoomSoundTime = cg.time + 300;
					}
				}
			}

			fov_x = cg_zoomFov.value;
		} else {
			f = ( cg.time - cg.zoomTime ) / (float)ZOOM_OUT_TIME;
			if ( f > 1.0 ) {
				fov_x = fov_x;
			} else {
				fov_x = zoomFov + f * ( fov_x - zoomFov );
			}
		}
	}

	/*if (cg.predictedPlayerState.introTime > cg.time)
	{	// The stuff is "holodecking in".
		fov_x = 80;
	}*/


	x = cg.refdef.width / tan( fov_x / 360 * M_PI );
	fov_y = atan2( cg.refdef.height, x );
	fov_y = fov_y * 360 / M_PI;

	// warp if underwater //TiM Also do it if we're critically injured
	contents = CG_PointContents( cg.refdef.vieworg, -1 );
	
	warpEffect = ( contents & ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) )
				 || ( !cg.renderingThirdPerson && cg.predictedPlayerState.stats[STAT_HEALTH] <= INJURED_MODE_HEALTH && cg.predictedPlayerState.stats[STAT_HEALTH] > 1 );

	if ( warpEffect ){
		//phase = cg.time / 1000.0 * WAVE_FREQUENCY * M_PI * 2;
		phase = cg.time * 0.001 * WAVE_FREQUENCY * M_PI * 2;
		v = WAVE_AMPLITUDE * sin( phase );
		fov_x += v;
		fov_y -= v;
		inwater = qtrue;
	}
	else {
		inwater = qfalse;
	}


	// set it
	cg.refdef.fov_x = fov_x;
	cg.refdef.fov_y = fov_y;

	if ( !cg.zoomed ) {
		cg.zoomSensitivity = 1;
	} else {
		cg.zoomSensitivity = cg.refdef.fov_y / 75.0;
	}

	return inwater;
}




/*
===============
CG_CalcViewValues

Sets cg.refdef view values
===============
*/
static int CG_CalcViewValues( void ) {
	playerState_t	*ps;

	memset( &cg.refdef, 0, sizeof( cg.refdef ) );

	// strings for in game rendering
	// Q_strncpyz( cg.refdef.text[0], "Park Ranger", sizeof(cg.refdef.text[0]) );
	// Q_strncpyz( cg.refdef.text[1], "19", sizeof(cg.refdef.text[1]) );

	// calculate size of 3D view
	CG_CalcVrect();

	ps = &cg.predictedPlayerState;

	// intermission view
	if ( ps->pm_type == PM_INTERMISSION ) {
		VectorCopy( ps->origin, cg.refdef.vieworg );
		VectorCopy( ps->viewangles, cg.refdefViewAngles );
		AnglesToAxis( cg.refdefViewAngles, cg.refdef.viewaxis );
		return CG_CalcFov();
	}

	cg.bobcycle = ( ps->bobCycle & 128 ) >> 7;
	cg.bobfracsin = fabs( sin( ( ps->bobCycle & 127 ) / 127.0 * M_PI ) );
	cg.xyspeed = sqrt( ps->velocity[0] * ps->velocity[0] +
		ps->velocity[1] * ps->velocity[1] );


	VectorCopy( ps->origin, cg.refdef.vieworg );
	VectorCopy( ps->viewangles, cg.refdefViewAngles );

	// add error decay
	if ( cg_errorDecay.value > 0 ) {
		int		t;
		float	f;

		t = cg.time - cg.predictedErrorTime;
		f = ( cg_errorDecay.value - t ) / cg_errorDecay.value;
		if ( f > 0 && f < 1 ) {
			VectorMA( cg.refdef.vieworg, f, cg.predictedError, cg.refdef.vieworg );
		} else {
			cg.predictedErrorTime = 0;
		}
	}

	if ( cg.renderingThirdPerson && ps->pm_type != PM_CCAM ) {
		// back away from character
		CG_OffsetThirdPersonView();
	} else {
		// offset for local bobbing and kicks
		CG_OffsetFirstPersonView();
	}

	// shake the camera if necessary
	CG_UpdateCameraShake( cg.refdef.vieworg, cg.refdefViewAngles );
	// position eye reletive to origin
	AnglesToAxis( cg.refdefViewAngles, cg.refdef.viewaxis );

	//TiM - As far as I can see, all this does is cause flashy
	//effects on-screen when a player teleports the hide the delay.
	//This probably doesn't really apply to us now...
	/*if ( cg.hyperspace ) {
		cg.refdef.rdflags |= RDF_NOWORLDMODEL | RDF_HYPERSPACE;
	}*/

	// field of view
	return CG_CalcFov();
}


/*
=====================
CG_PowerupTimerSounds
=====================
*/
/*static void CG_PowerupTimerSounds( void ) {
	int		i;
	int		t;

	// powerup timers going away
	for ( i = 0 ; i < MAX_POWERUPS ; i++ ) {
		t = cg.snap->ps.powerups[i];

		// kef -- hack hack hack. additionally, hack.
		if ( (PW_OUCH == i) || (PW_GHOST == i) )
		{
			continue;
		}
		if ( t <= cg.time ) {
			continue;
		}
		if ( t - cg.time >= POWERUP_BLINKS * POWERUP_BLINK_TIME ) {
			continue;
		}
		if ( ( t - cg.time ) / POWERUP_BLINK_TIME != ( t - cg.oldTime ) / POWERUP_BLINK_TIME ) {
			//trap_S_StartSound( NULL, cg.snap->ps.clientNum, CHAN_ITEM, cgs.media.wearOffSound );
		}
	}
}*/



//=========================================================================

/*
=============
CG_IntroModel

This is when the player is starting the level.
=============
*/
/*void CG_AddIntroModel(playerState_t *ps, int time) 
{
	static int soundpoint=0, lasttime=999999;
	refEntity_t	doorbox;
	float	alpha;
	byte a;
	//char pClass[MAX_QPATH];
	//char pRank[MAX_QPATH];

	if (lasttime > time)
	{	// Restart everything.
		soundpoint=0;
	}

	lasttime=time;

	// add the model
	memset( &doorbox, 0, sizeof( doorbox ) );
	VectorCopy( cg.refdef.vieworg, doorbox.lightingOrigin );

	doorbox.shaderRGBA[0] = 255;
	doorbox.shaderRGBA[1] = 255;
	doorbox.shaderRGBA[2] = 255;
	doorbox.shaderRGBA[3] = 255;

	doorbox.hModel = cgs.media.doorbox;
	if (!doorbox.hModel) {
		return;
	}

	VectorMA(cg.refdef.vieworg, 25, cg.refdef.viewaxis[0], doorbox.origin);
	VectorMA(doorbox.origin, -35, cg.refdef.viewaxis[2], doorbox.origin);
	AnglesToAxis(cg.refdefViewAngles, doorbox.axis);

	VectorScale(doorbox.axis[0], -1.0, doorbox.axis[0]);
	VectorScale(doorbox.axis[1], -1.0, doorbox.axis[1]);

	if (soundpoint <= 0)
	{	// First part...  "Prepare to compete."
		if (time >= TIME_INIT)
		{
			soundpoint = 1;
			trap_S_StartSound( cg.refdef.vieworg, ENTITYNUM_WORLD, CHAN_AUTO, cgs.media.holoInitSound );
		}
		doorbox.frame = 0;
	}
	else if (soundpoint == 1)
	{	// Second part...  Open door after "prepare".
		if (time >= TIME_DOOR_START)
		{
			soundpoint = 2;
			trap_S_StartSound( cg.refdef.vieworg, ENTITYNUM_WORLD, CHAN_AUTO, cgs.media.holoDoorSound );
		}
		doorbox.frame = 0;
	}
	else if (soundpoint == 2)
	{	// Third part...  Fade in after opening door.
		if (time >= TIME_FADE_START)
		{
			soundpoint = 3;
			trap_S_StartSound( cg.refdef.vieworg, ENTITYNUM_WORLD, CHAN_AUTO, cgs.media.holoFadeSound );
			doorbox.frame = FRAMES_DOOR-1;
		}
		else
		{
			doorbox.frame = ((float)(time - TIME_DOOR_START) / 100.0) + 1;
			if (doorbox.frame >= FRAMES_DOOR)
			{
				doorbox.frame=FRAMES_DOOR-1;
			}
			else
			{
				doorbox.oldframe = doorbox.frame-1;
				doorbox.backlerp = (float)(doorbox.frame) - ((float)(time - TIME_DOOR_START) / 100.0);
			}
		}
	}
	else
	{	// Final part...  Fade out the model.

		alpha = 1.0 - ((float)(time - TIME_FADE_START) / (float)TIME_FADE_DUR);

		if (alpha<0.0)
		{
			alpha=0.0;
		}

		a=255.0*alpha;
		if (a<=0)
		{	// An alpha of zero defaults to opaque...  Makes sense, why even send something that is 100% transparent?
			a=1;
		}

		doorbox.shaderRGBA[0] = 255;
		doorbox.shaderRGBA[1] = 255;
		doorbox.shaderRGBA[2] = 255;
		doorbox.shaderRGBA[3] = a;
		doorbox.frame = FRAMES_DOOR-1;
	}

	doorbox.renderfx |= (RF_DEPTHHACK|RF_FORCE_ENT_ALPHA|RF_FULLBRIGHT);

	trap_R_AddRefEntityToScene(&doorbox);
}*/

void CG_DrawEVAHelmet ( playerState_t *ps )
{
	refEntity_t	helmet;

	if ( !ps->powerups[PW_EVOSUIT] && !( cgs.clientinfo[ps->clientNum].isHazardModel && ps->powerups[PW_BOLTON] ) ) {
		return;
	}

	memset( &helmet, 0, sizeof(helmet) );
	VectorCopy( ps->origin, helmet.lightingOrigin );
	helmet.renderfx = RF_LIGHTING_ORIGIN | RF_DEPTHHACK | RF_FIRST_PERSON;
	helmet.hModel = cgs.media.evaInterior;

	if ( !helmet.hModel ) {
		CG_Printf("EVA Helmet Model not found\n");
		return;
	}

	VectorCopy( cg.refdef.vieworg, helmet.origin );
	AxisCopy( cg.refdef.viewaxis, helmet.axis );
	VectorMA( helmet.origin, 6, cg.refdef.viewaxis[0], helmet.origin );

	CG_AddRefEntityWithPowerups( &helmet, 
									cg.predictedPlayerEntity.currentState.powerups, 
									cg.predictedPlayerEntity.currentState.eFlags, 
									&cg.predictedPlayerEntity.beamData,
									cg.predictedPlayerEntity.cloakTime,
									cg.predictedPlayerEntity.decloakTime,
									qfalse );
	
}


//=========================================================================

/*
=================
CG_DrawActiveFrame

Generates and draws a game scene and status information at the given time.
=================
*/
void CG_DrawActiveFrame( int serverTime, stereoFrame_t stereoView, qboolean demoPlayback ) {
	int		inwater;
	char	cvarYaw[16]; //an uber long floating point value lol
	float	yaw;

	cg.time = serverTime;
	cg.demoPlayback = demoPlayback;

	//RPG-X: TiM - Set up for giant uber rant.
	//GARRRRRGGGGGGGGGGGGGGGGGGGGGGGGGHHHHHHHHHHHHHH!!!!!!!!!!!!!!!!!!!!!!!!! ->O_O<-
	//Here's me looking at the BFP mod, wondering how the gravynuggets they managed
	//to flip the yaw value when the players rotated beyond the normal view axis.
	//I developed a totally l33tzor rotational offset algorithm that came oh so close
	//to working, but after noticing a little inconsistancy in their rotation ingame, I realised
	//all they did was change the player's mouse yaw CVAR to invert!!!! O_O
	//Ohhhhh I just lost several years of my life and the use of my wrists.....
	//Probably just as well we disabled the RPG-X online webcam... that wasn't pretty. O_o
	
	//load our yaw value
	trap_Cvar_VariableStringBuffer( "m_yaw", cvarYaw, sizeof( cvarYaw ) );
	yaw = atof ( cvarYaw );

	if ( cg.predictedPlayerEntity.currentState.eFlags & EF_FULL_ROTATE 
		&& Q_fabs( cg.predictedPlayerEntity.lerpAngles[PITCH] ) > 89.0f ) 
	{
		if ( yaw > 0.0f ) 
		{
			//yaw = -yaw;
			//trap_Cvar_Set( "m_yaw", va( "%f", yaw ) );
			trap_Cvar_Set( "m_yaw", va( "-%s", cvarYaw ) );

			//CG_Printf( S_COLOR_RED "%f\n", yaw );
		}
	}
	else { //ugh... I hope no one plays with their yaws inverted. >.< This MAY need to be CVAR controlled
		if ( yaw < 0.0f )
		{
			char *tmp = cvarYaw;
			if ( tmp[0] == '-') tmp++; //erase the neg sign

			//trap_Cvar_Set( "m_yaw", va( "%f", Q_fabs( yaw ) ) );
			trap_Cvar_Set( "m_yaw", va( "%s", tmp ) );
		}
	}

	// update cvars
	CG_UpdateCvars();

	// if we are only updating the screen as a loading
	// pacifier, don't even try to read snapshots
	if ( cg.infoScreenText[0] != 0 ) {
		CG_DrawInformation();
		return;
	}

	// any looped sounds will be respecified as entities
	// are added to the render list
	trap_S_ClearLoopingSounds();

	// clear all the render lists
	trap_R_ClearScene();

	// set up cg.snap and possibly cg.nextSnap
	CG_ProcessSnapshots();

	// if we haven't received any snapshots yet, all
	// we can draw is the information screen
	if ( !cg.snap || ( cg.snap->snapFlags & SNAPFLAG_NOT_ACTIVE ) ) {
		CG_DrawInformation();
		return;
	}

	// let the client system know what our weapon and zoom settings are
	trap_SetUserCmdValue( cg.weaponSelect, cg.zoomSensitivity );

	// this counter will be bumped for every valid scene we generate
	cg.clientFrame++;

	// update cg.predictedPlayerState
	CG_PredictPlayerState();

	// decide on third person view
	cg.renderingThirdPerson = ( cg_thirdPerson.integer && !cg.zoomed && cg.predictedPlayerState.pm_type != PM_SPECTATOR ) || (cg.snap->ps.stats[STAT_HEALTH] <= 1 ) ; //TiM - So we'll always be first person in zooming //0

	// build cg.refdef
	inwater = CG_CalcViewValues();

	// first person blend blobs, done after AnglesToAxis
	if ( !cg.renderingThirdPerson ) 
	{
		CG_DrawFullScreenFX();
		CG_DrawEVAHelmet( &cg.predictedPlayerState );
	}


	// build the render lists
	if ( !cg.hyperspace ) {
		CG_AddPacketEntities();			// adter calcViewValues, so predicted player state is correct
		CG_AddMarks();
		CG_AddLocalEntities();
	}

	/*if (cg.predictedPlayerState.introTime > cg.time)
	{	// Render the holodeck doors
		CG_AddIntroModel(&cg.predictedPlayerState, TIME_INTRO - (cg.predictedPlayerState.introTime - cg.time));
	}*/

	CG_AddViewWeapon( &cg.predictedPlayerState );

	// finish up the rest of the refdef
	if ( cg.testModelEntity.hModel ) {
		CG_AddTestModel();
	}
	cg.refdef.time = cg.time;
	memcpy( cg.refdef.areamask, cg.snap->areamask, sizeof( cg.refdef.areamask ) );

	// update audio positions
	trap_S_Respatialize( cg.snap->ps.clientNum, cg.refdef.vieworg, cg.refdef.viewaxis, inwater );

	// warning sounds when powerup is wearing off
	//TiM - Not really needed :P
	//CG_PowerupTimerSounds();

	// make sure the lagometerSample and frame timing isn't done twice when in stereo
	if ( stereoView != STEREO_RIGHT ) {
		cg.frametime = cg.time - cg.oldTime;
		if ( cg.frametime < 0 ) {
			cg.frametime = 0;
		}
		cg.oldTime = cg.time;
		CG_AddLagometerFrameInfo();
	}

	// actually issue the rendering calls
	CG_DrawActive( stereoView );

	if ( cg_stats.integer ) {
		CG_Printf( "cg.clientFrame:%i\n", cg.clientFrame );
	}

	//TiM - Reset the lerp code at the end of this frame.
	if ( cg.thirdPersonNoLerp )
		cg.thirdPersonNoLerp = qfalse;
}

