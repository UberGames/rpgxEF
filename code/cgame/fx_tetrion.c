#include "cg_local.h"
#include "fx_local.h"

/*
-------------------------
FX_TetrionShot
-------------------------
*/
#define MAXRANGE_TETRION		5000000 //RPG-X: J2J OLD: 8192
void FX_TetrionShot( vec3_t start, vec3_t forward ) // TODO check whether I'm needed
{
	trace_t	trace;
	vec3_t	end, dir, new_start, new_end, radial, start2, spreadFwd;
	float	off, len, i, numBullets = 3;
	float	firingRadius = 6, minDeviation = 0.95, maxDeviation = 1.1;
	qboolean	render_impact = qtrue;

	for (i = 0; i < numBullets; i++)
	{
		render_impact = qtrue;
		// determine new firing position 
		fxRandCircumferencePos(start, forward, firingRadius, new_start);
		VectorSubtract(new_start, start, radial);
		VectorMA(start, 10, forward, start2);
		VectorMA(start2, flrandom(minDeviation, maxDeviation), radial, start2);
		VectorSubtract(start2, new_start, spreadFwd);
		VectorNormalize(spreadFwd);
		// determine new end position for this bullet. give the endpoint some spread, too.
		VectorMA(new_start, MAXRANGE_TETRION, spreadFwd, end);
		CG_Trace( &trace, new_start, NULL, NULL, end, cg_entities[cg.predictedPlayerState.clientNum].currentState.number, MASK_SHOT );
		// Get the length of the whole shot
		VectorSubtract( trace.endpos, new_start, dir ); 
		len = VectorNormalize( dir );
		// Don't do tracers when it gets really short
		if ( len >= 64 )
		{
			// Move the end_point in a bit so the tracer doesn't always trace the full line length--this isn't strictly necessary, but it does
			//		add a bit of variance
			off = flrandom(0.7, 1.0);
			VectorMA( new_start, len * off, dir, new_end );

			// Draw the tracer
			FX_AddLine( new_end, new_start, 1.0f, 1.5f + random(), 0.0f, flrandom(0.3,0.6), 0.0,
				flrandom(300,500), cgs.media.borgFlareShader );
		}
		// put the impact effect where this tracer hits
		if (len >= 32)
		{
			// Rendering things like impacts when hitting a sky box would look bad, but you still want to see the tracer
			if ( trace.surfaceFlags & SURF_NOIMPACT ) 
			{
				render_impact = qfalse;
			}
		}
	}
}
