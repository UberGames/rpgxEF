
#include "cg_local.h"
#include "fx_local.h"

/*
-------------------------
SPTransporterLensFlares

TiM: Judging from the title,
you just KNOW it's mine ;)

Anyway, the point of this 
function is to render 4 
sprites and then scale + move
them in a way reminisicant
of ST: Voyager's transporter FX.

I wrote this instead of using the
already made FX spawning functions
because they don't let u track an origin.
To stop the particle
from rendering within the character, I'm going to
use the DEPTHHACK renderflag, the flag
originally used for the weapon models
in first-person mode. :)

Planned timing:

0 - 500 : Flare spawns, and scales up to it's main size (width parameter)
500 - 1500 : Flare moves up the height/2 of model (height parameter)
1500 - 2000 : Flare scales down and disappears ( width * 1.0 - (timeScale) )
-------------------------
*/

void FX_SPTransporterLensFlares( centity_t* cent, vec3_t headVector, int startTime ) {
	refEntity_t flare;
	trace_t	tr;
	int i;
	int direction = 1;
	int timeOffset = 0; //250 - time between first and second flares appearing;
	float ratio;
	float dlightRatio;

	vec3_t origin, tempOrigin;
	int width;
	int height;

	//Hrmm... we have a glitch lol. Since DEPTHHACK is on, the flare will be drawn
	//from ANYWHERE IN THE LEVEL! O_o
	//So.... uh, we'll do a trace between ourselves and the entity this is attached
	//to, and if they can't see each other, screw it. :P
	if ( cg.predictedPlayerState.clientNum != cent->currentState.clientNum ) {
		CG_Trace( &tr, cg.refdef.vieworg, NULL, NULL, 
				cent->lerpOrigin, cg.predictedPlayerState.clientNum, CONTENTS_SOLID );
		if ( tr.fraction != 1 ) {
			return;
		}
	}
	/*else {
		Com_Printf( "Origin: { %f, %f, %f }\n", cent->lerpOrigin[0], cent->lerpOrigin[1], cent->lerpOrigin[2] );
		Com_Printf( "HeadVector: { %f, %f, %f }\n", headVector[0], headVector[1], headVector[2] );
		return;
	}*/

	//calculate the necessary data we need to place the origin in the direct center of the model

	memset( &flare, 0, sizeof( flare ) );

	//Bah.  I thought lerpOrigin was at the base of the feet.  Turns out it's at the knees O_o
	//This little hack should help that
	VectorCopy( cent->lerpOrigin, tempOrigin );
	tempOrigin[2] -= 24;

	//If the distance means we're not lying down
	if ( ( headVector[2] - tempOrigin[2] ) > 8 ) {
		//find the average between our lerp origin and headVector to find the center
		//VectorAdd( headVector, tempOrigin, origin );
		//VectorScale( origin, 0.5, origin );
		VectorAverage( headVector, tempOrigin, origin );

		width = 30;
		height = (headVector[2] - tempOrigin[2]) / 2;
	}
	else {
		width = 30;
		height = 4;

		VectorCopy( cent->lerpOrigin, origin);
	}

	flare.reType = RT_SPRITE;
	flare.shaderRGBA[0] = 0xff;
	flare.shaderRGBA[1] = 0xff;
	flare.shaderRGBA[2] = 0xff;
	flare.shaderRGBA[3] = 0xff;

	flare.data.sprite.rotation = 0;
	flare.nonNormalizedAxes = qtrue; //needed for effective scaling

	flare.renderfx |= RF_DEPTHHACK; //DEPTHHACK renders the element over everything else. Useful in this lens flare simulation case :)

	//loop 4 times = 4 flares. :)
	for (i = 0; i < 4; i++ ) {
		VectorClear( flare.origin );
		VectorCopy( origin, flare.origin);

		//the first two flares are the main ones
		if ( i < 2 ) {
			flare.customShader = cgs.media.transport1Shader; //1
			timeOffset = startTime;
		}
		else { // the second two spawn a little later
			flare.customShader = cgs.media.transport2Shader;
			timeOffset = startTime + 650; //750
		}

		//the second flare each round goes down instead of up
		if ( i % 2 == 0) 
			direction = 1;
		else	
			direction = -1;

		//===========================
		
		if ( cg.time > timeOffset + 2000 ) {
			continue;
		}

		//Phase 1: flares get bigger
		if ( cg.time < timeOffset + 500 ) {
			ratio = ((float)(cg.time - timeOffset) * 0.002);
			if (ratio < 0 )
				ratio = 0.0f;
			else if (ratio > 1 )
				ratio = 1.0f;
			
			flare.data.sprite.radius = (float)width * ratio;
			/*if ( i ==0 )
				Com_Printf( "Phase 1 Radius: %f\n", flare.data.sprite.radius );*/
		}
		//Phase 2: flares move up/down character
		if ( ( cg.time < timeOffset + 1500 ) && ( cg.time >= timeOffset + 500 ) ) {
			ratio = ( (float)(cg.time - (timeOffset + 500) ) * 0.001 );
			if (ratio < 0 )
				ratio = 0.0f;
			else if (ratio > 1 )
				ratio = 1.0f;

			flare.data.sprite.radius = (float)width;
			flare.origin[2] += (float)direction * (float)height * ratio;
			/*if (i == 0 )
				Com_Printf( "Phase 2 Location: %f\n", flare.origin[2] );*/
		}
		//Phase 3: flares get smaller
		if ( ( cg.time < timeOffset + 2000 ) && ( cg.time >= timeOffset + 1500 ) ) {
			ratio = 1.0f - (float)(cg.time - ( timeOffset + 1500 ) ) * 0,002;
			if (ratio < 0 )
				ratio = 0.0f;
			else if (ratio > 1 )
				ratio = 1.0f;

			flare.origin[2] += ((float)height * (float)direction);
			flare.data.sprite.radius = (float)width * ratio;
			/*if ( i == 0 )
				Com_Printf( "Phase 3 Radius: %f\n", flare.data.sprite.radius );*/
		}

		trap_R_AddRefEntityToScene( &flare );
	}
	
	//dynamic light calculation
	if ( cg.time < ( startTime + 2000 ) ) {
		dlightRatio = (float)( cg.time - startTime ) * 0,0005;
	}
	else {
		dlightRatio = 1.0f - (float)( cg.time - ( startTime + 2000 ) ) * 0,0005;
	}

	//dynamic light FX
	trap_R_AddLightToScene( origin, 80.0f * dlightRatio, 0.345, 0.624, 0.835 );
}

/*
-------------------------
TransporterParticle
-------------------------
*/

qboolean TransporterParticle( localEntity_t *le)
{
	vec3_t		org, velocity	=	{ 0, 0, 68 };
	vec3_t		accel = { 0, 0, -12 };
	float		scale, dscale;
	qhandle_t	shader;

	VectorCopy( le->refEntity.origin, org );
	org[2] += 0;//38;

	shader = ( le->data.spawner.dir[0] == 0 ) ? cgs.media.trans1Shader : cgs.media.trans2Shader;
	scale  = ( le->data.spawner.dir[0] == 0 ) ? 2.0 : 4.0;
	dscale  = ( le->data.spawner.dir[0] == 0 ) ? 4.0 : 24.0;

	le->data.spawner.dir[0]++;

	FX_AddSprite(	org,
					velocity,
					qfalse,
					scale,
					dscale,
					1.0f,
					0.0f,
					0,
					0.0f,
					450.0f,
					shader );
	
	VectorScale( velocity, -1, velocity );
	VectorScale( accel, -1, accel );

	FX_AddSprite(	org,
					velocity,
					qfalse,
					scale,
					dscale,
					1.0f,
					0.0f,
					0,
					0.0f,
					450.0f,
					shader );

	return qtrue;
}

/*
-------------------------
TransporterPad
-------------------------
*/

qboolean TransporterPad( localEntity_t *le)
{
	vec3_t		org;
	vec3_t		up = {0,0,1};
	float		scale, dscale;
	qhandle_t	shader;

	VectorCopy( le->refEntity.origin, org );
	org[2] -= 3;

	shader = cgs.media.trans1Shader;
	scale  = 20.0;
	dscale  = 2.0;

	FX_AddQuad(		org,
					up,
					scale,
					dscale,
					1.0f,
					0.0f,
					0,
					950.0f,
					shader );
	return qtrue;
}

/*
-------------------------
FX_Transporter
-------------------------
*/

void FX_Transporter( vec3_t origin )
{
	vec3_t up = {0,0,1};

	FX_AddSpawner( origin, up, NULL, NULL, qfalse, 0, 0, 200, TransporterParticle, 0 );
//	trap_S_StartSound( origin, NULL, CHAN_AUTO, cgs.media.teleInSound );
}

/*
-------------------------
FX_TransporterPad
-------------------------
*/

void FX_TransporterPad( vec3_t origin )
{
	vec3_t up = {0,0,1};

	FX_AddSpawner( origin, up, NULL, NULL, qfalse, 1000, 0, 0, TransporterPad, 0 );
}

