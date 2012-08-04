#include "cg_local.h"

#define MAX_MOTIONBLURDOTS 20

typedef struct motionblurDot_s {
	qboolean	active;
	refEntity_t	refEnt;
	int			startTime;
	int			lifeTime;
} motionblurDot_t;

//static motionblurDot_t cg_motionblurDots[MAX_MOTIONBLURDOTS];



void CG_MotionBlur(void) {
	//motionblurDot_t	*dot;
	//vec3_t			pos, axis[3];
	//int i;	

		
	/*if ( !cg.snap->ps.powerups[PW_BOOST] && cg.snap->ps.timers[tmZanzoken] < 1 && !cg.snap->ps.timers[tmTransform]) {
		cg.refdef.rdflags &= ~RDF_MOTIONBLUR;

		
		//for ( i = 0; i < MAX_MOTIONBLURDOTS; i++ ) {
		//	cg_motionblurDots[i].active = qfalse;
		//}
		

		return;
	}*/


	cg.refdef.rdflags |= RDF_MOTIONBLUR;

	/*
	// Destroy dots over lifetime
	for ( i = 0; i < MAX_MOTIONBLURDOTS; i++ ) {
		dot = &cg_motionblurDots[i];

		if ( dot->lifeTime + dot->startTime < cg.time ) {
			dot->active = qfalse;
		}
	}

	// Create new dots
	for ( i = 0; i < MAX_MOTIONBLURDOTS; i++ ) {
		dot = &cg_motionblurDots[i];

		if ( dot->active )
			continue;

		VectorCopy( cg.predictedPlayerEntity.lerpOrigin, pos );
		VectorNormalize2( cg.predictedPlayerState.velocity, axis[0] );
		VectorMA( pos, 300, axis[0], pos );
		RotateAroundDirection( axis, crandom() * 360 );
		VectorMA( pos, 120, axis[2], pos );
		
		memset( &(dot->refEnt), 0, sizeof(refEntity_t));
		dot->refEnt.reType = RT_SPRITE;
		dot->refEnt.radius = 2;
		dot->refEnt.customShader = cgs.media.whiteShader;
		dot->refEnt.shaderRGBA[0] = 255;
		dot->refEnt.shaderRGBA[1] = 255;
		dot->refEnt.shaderRGBA[2] = 255;
		dot->refEnt.shaderRGBA[3] = 128;
		VectorCopy( pos, dot->refEnt.origin );
		
		dot->lifeTime = 250 + crandom() * 100;
		dot->startTime = cg.time + crandom() * 150;
		dot->active = qtrue;
	}

	// Render dots
	for ( i = 0; i < MAX_MOTIONBLURDOTS; i++ ) {
		dot = &cg_motionblurDots[i];
		if ( dot->startTime > cg.time )
			continue;

		trap_R_AddRefEntityToScene( &(dot->refEnt));
	}
	*/
}
