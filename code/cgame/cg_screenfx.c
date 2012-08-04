#include "cg_local.h"
#include "cg_screenfx.h"

// this is the list of currently drawing fx and their start times and end times
screenFX_t theScreenFX;

int CG_GetScreenEffectEndTime(int event)
{
	switch (event)
	{
	case SCREENFX_TRANSPORTER:
		return cg.time + 1000;
	case SCREENFX_SP_TRANSPORTER_IN:
		return cg.time + 4000;
	case SCREENFX_SP_TRANSPORTER_OUT:
		return cg.time + 8000;
	default:
		return 0;
	}
}

// maybe play a sound or something?
void CG_BeginScreenEffect(int event)
{
	switch (event)
	{
	case SCREENFX_TRANSPORTER:
		break;
	default:
		break;
	}
}


// when adding a new effect, we'll either take an empty slot in theScreenFX or
//overwrite the oldest effect
void CG_AddFullScreenEffect(int screenfx, int clientNum)
{
	int i = 0, oldestTime = cg.time, oldestEffect = 0;

	if (clientNum != cg.predictedPlayerState.clientNum)
	{	// only add screen effects for our client
		return;
	}
	for (i = 0; i < MAX_SCREENFX; i++)
	{
		// if we already have one of these effects going, just add to the duration of 
		//the existing one...don't create a new instance of the same effect
		if (theScreenFX.events[i] == screenfx)
		{
			theScreenFX.cgStartTimes[i] = cg.time;
			theScreenFX.cgEndTimes[i] = CG_GetScreenEffectEndTime(screenfx);
			return;
		}
		else if (theScreenFX.cgStartTimes[i])
		{
			if (theScreenFX.cgStartTimes[i] < oldestTime)
			{
				oldestTime = theScreenFX.cgStartTimes[i];
				oldestEffect = i;
			}
		}
		//Hack-didily-ack - TiM: If were already one powerup, switch to the next
		else if ( screenfx == SCREENFX_SP_TRANSPORTER_OUT ) {
			if ( theScreenFX.events[i] == SCREENFX_SP_TRANSPORTER_IN ) {
				theScreenFX.events[i] = 0;
				theScreenFX.cgStartTimes[i] = 0;
				theScreenFX.cgEndTimes[i] = 0;
			}
		}
		else if ( screenfx == SCREENFX_SP_TRANSPORTER_IN ) {
			if ( theScreenFX.events[i] == SCREENFX_SP_TRANSPORTER_OUT ) {
				theScreenFX.events[i] = 0;
				theScreenFX.cgStartTimes[i] = 0;
				theScreenFX.cgEndTimes[i] = 0;
			}
		}
		else
		{
			oldestTime = theScreenFX.cgStartTimes[i];
			oldestEffect = i;
		}
	}
	theScreenFX.events[oldestEffect] = screenfx;
	theScreenFX.cgStartTimes[oldestEffect] = cg.time;
	theScreenFX.cgEndTimes[oldestEffect] = CG_GetScreenEffectEndTime(screenfx);
	CG_BeginScreenEffect(screenfx);
}



/*
===============
CG_DrawScreenQuad

===============
*/


static void CG_DrawScreenQuad(float alpha, qhandle_t screenshader) 
{
	refEntity_t		ent;
	float radius;

	// ragePro systems can't fade blends, so don't obscure the screen
	if ( cgs.glconfig.hardwareType == GLHW_RAGEPRO ) {
		return;
	}

	if (cg.refdef.fov_x > 120)
	{
		return;		// Too wide to show this.
	}
	else if (cg.refdef.fov_x > 80)
	{
		radius = 8.0 + (cg.refdef.fov_x - 80)*0.2;
	}
	else
	{
		radius = 8.0;
	}

	memset( &ent, 0, sizeof( ent ) );
	ent.reType = RT_SPRITE;
	ent.renderfx = RF_FIRST_PERSON;

	VectorMA( cg.refdef.vieworg, 8, cg.refdef.viewaxis[0], ent.origin );

	ent.data.sprite.radius = radius;		
	ent.customShader = screenshader;
	ent.shaderRGBA[0] = alpha * 255;
	ent.shaderRGBA[1] = alpha * 255;
	ent.shaderRGBA[2] = alpha * 255;
	ent.shaderRGBA[3] = 255;
	trap_R_AddRefEntityToScene( &ent );
}

/*
static void CG_DrawDirectionalScreenQuad(float alpha, qhandle_t screenshader) 
{
	refEntity_t		ent;
	vec3_t			screencenter;
	byte			topleft, topright, lowleft, lowright, top, low, left, right;
	float			val;

	// ragePro systems can't fade blends, so don't obscure the screen
	if ( cgs.glconfig.hardwareType == GLHW_RAGEPRO ) {
		return;
	}

	// Set up all the basic info about this refentity
	VectorMA( cg.refdef.vieworg, 8, cg.refdef.viewaxis[0], screencenter);

	memset( &ent, 0, sizeof( ent ) );
	ent.reType = RT_ALPHAVERTPOLY;
	ent.renderfx = RF_FIRST_PERSON;

	ent.data.sprite.radius = 4;		
	ent.customShader = screenshader;
	* (unsigned int *) ent.shaderRGBA = 0xffffffff;
	* (unsigned int *) ent.data.sprite.vertRGBA[0] = 0xffffffff; 
	* (unsigned int *) ent.data.sprite.vertRGBA[1] = 0xffffffff;
	* (unsigned int *) ent.data.sprite.vertRGBA[2] = 0xffffffff;
	* (unsigned int *) ent.data.sprite.vertRGBA[3] = 0xffffffff;

	// left
	val = alpha*(0.5 + 0.5*(cg.damageX - fabs(cg.damageY)));
	if (val<0)
		left=0;
	else if (val>1.0)
		left=255;
	else 
		left=255.0*val;

	// upper left
	val = alpha*(0.5*(cg.damageX + cg.damageY));
	if (val<0)
		topleft=0;
	else if (val>1.0)
		topleft=255;
	else 
		topleft=255.0*val;

	// top
	val = alpha*(0.5 + 0.5*(-fabs(cg.damageX) + cg.damageY));
	if (val<0)
		top=0;
	else if (val>1.0)
		top=255;
	else 
		top=255.0*val;

	// upper right
	val = alpha*(0.5*(-cg.damageX + cg.damageY));
	if (val<0)
		topright=0;
	else if (val>1.0)
		topright=255;
	else 
		topright=255.0*val;

	// right
	val = alpha*(0.5 + 0.5*(-cg.damageX - fabs(cg.damageY)));
	if (val<0)
		right=0;
	else if (val>1.0)
		right=255;
	else 
		right=255.0*val;

	// lower right
	val = alpha*(0.5*(-cg.damageX - cg.damageY));
	if (val<0)
		lowright=0;
	else if (val>1.0)
		lowright=255;
	else 
		lowright=255.0*val;

	// bottom
	val = alpha*(0.5 + 0.5*(-fabs(cg.damageX) - cg.damageY));
	if (val<0)
		low=0;
	else if (val>1.0)
		low=255;
	else 
		low=255.0*val;

	// lower left
	val = alpha*(0.5*(cg.damageX - cg.damageY));
	if (val<0)
		lowleft=0;
	else if (val>1.0)
		lowleft=255;
	else 
		lowleft=255.0*val;


	// Draw the upper left corner
	VectorMA(screencenter, 4, cg.refdef.viewaxis[1], ent.origin);
	VectorMA(ent.origin, 4, cg.refdef.viewaxis[2], ent.origin);
	ent.data.sprite.vertRGBA[0][3] = topleft;
	ent.data.sprite.vertRGBA[1][3] = top;
	ent.data.sprite.vertRGBA[2][3] = 0;
	ent.data.sprite.vertRGBA[3][3] = left;
	trap_R_AddRefEntityToScene( &ent );


	// Draw topper right corner
	VectorMA(screencenter, -4, cg.refdef.viewaxis[1], ent.origin);
	VectorMA(ent.origin, 4, cg.refdef.viewaxis[2], ent.origin);
	ent.data.sprite.vertRGBA[0][3] = top;
	ent.data.sprite.vertRGBA[1][3] = topright;
	ent.data.sprite.vertRGBA[2][3] = right;
	ent.data.sprite.vertRGBA[3][3] = 0;
	trap_R_AddRefEntityToScene( &ent );


	// Draw lower right corner
	VectorMA(screencenter, -4, cg.refdef.viewaxis[1], ent.origin);
	VectorMA(ent.origin, -4, cg.refdef.viewaxis[2], ent.origin);
	ent.data.sprite.vertRGBA[0][3] = 0;
	ent.data.sprite.vertRGBA[1][3] = right;
	ent.data.sprite.vertRGBA[2][3] = lowright;
	ent.data.sprite.vertRGBA[3][3] = low;
	trap_R_AddRefEntityToScene( &ent );


	// Draw lower left corner
	VectorMA(screencenter, 4, cg.refdef.viewaxis[1], ent.origin);
	VectorMA(ent.origin, -4, cg.refdef.viewaxis[2], ent.origin);
	ent.data.sprite.vertRGBA[0][3] = left;
	ent.data.sprite.vertRGBA[1][3] = 0;
	ent.data.sprite.vertRGBA[2][3] = low;
	ent.data.sprite.vertRGBA[3][3] = lowleft;
	trap_R_AddRefEntityToScene( &ent );

}
*/

static void CG_DrawScreenBlob(float redalpha, float greenalpha)
{
	refEntity_t		ent;
	float			alphascale;
	float			bluealpha = 0;

	// ragePro systems can't fade blends, so don't obscure the screen
	if ( cgs.glconfig.hardwareType == GLHW_RAGEPRO ) {
		return;
	}

	memset( &ent, 0, sizeof( ent ) );
	ent.reType = RT_SPRITE;
	ent.renderfx = RF_FIRST_PERSON;

	// Available input:
	// cg.damageValue:  Range from 0 to 1, indicating the amount of damage.
	// cg.damageX and cg_damageY:  Range from -1 to 1, indicating the location of the damage.

	VectorMA( cg.refdef.vieworg, 8, cg.refdef.viewaxis[0], ent.origin );
	VectorMA( ent.origin, cg.damageX * -8, cg.refdef.viewaxis[1], ent.origin );
	VectorMA( ent.origin, cg.damageY * 8, cg.refdef.viewaxis[2], ent.origin );

	// Here's the scoop:  The closer we are to the center, the more transparent this blob is.
	alphascale = (2*fabs(cg.damageX)+fabs(cg.damageY))/3.0;
	redalpha *= alphascale;
	greenalpha *= alphascale;

	if (redalpha > greenalpha)
	{
		ent.data.sprite.radius = cg.damageValue * 15 * redalpha;
	}
	else
	{
		ent.data.sprite.radius = cg.damageShieldValue * 15 * greenalpha;
	}

/*	if (redalpha < 0.01)
	{	// Just shield damage
		ent.customShader = cgs.media.shieldBlobShader;
		// Set all colors to the same as green, since the shader is green
		redalpha = bluealpha = greenalpha;
	}
	else*/ if (greenalpha < 0.01)
	{	// Just pain damage
		ent.customShader = cgs.media.painBlobShader;
		// Set all colors to the same as red, since the shader is red
		greenalpha = bluealpha = redalpha;
	}
	else
	{	// Both
		ent.customShader = cgs.media.painShieldBlobShader;
	}

	ent.shaderRGBA[0] = 0xff * redalpha;
	ent.shaderRGBA[1] = 0xff * greenalpha;
	ent.shaderRGBA[2] = 0xff * bluealpha;
	ent.shaderRGBA[3] = 0xff;
	trap_R_AddRefEntityToScene( &ent );
}



void CG_DrawFullScreenEffect(int screenfx, int start, int end)
{
	float	alpha, alpha2 = 0.0; //TiM - second alpha
	int end2, start2;

	alpha = (float)(end-cg.time)/(float)(end-start);

	switch (screenfx)
	{
	case SCREENFX_TRANSPORTER:
		CG_DrawScreenQuad(alpha, cgs.media.teleportEffectShader);
		break;
	case SCREENFX_SP_TRANSPORTER_IN:
		end2=end - 2500;

		CG_DrawScreenQuad( alpha, cgs.media.teleportEffectShader);
		//Fade in a white quad a little later
		if (cg.time <= end2 ) {
			alpha2 = (float)(end2-cg.time)/(float)(end2-start);
			CG_DrawScreenQuad( alpha2, cgs.media.white2Shader);
		}
		break;
	case SCREENFX_SP_TRANSPORTER_OUT:
		start2=start+2500;
		end2=end - 4000;
		
		alpha = (float)(end2-cg.time)/(float)(end2-start);
		CG_DrawScreenQuad(( 1.0f - alpha), cgs.media.teleportEffectShader);
		if ( cg.time >= start2 ) {
			alpha2 = (float)(end2-cg.time)/(float)(end2-start2);
			if ( cg.time >= end2 )
				alpha2=0.0f;

			CG_DrawScreenQuad( ( 1.0f - alpha2), cgs.media.white2Shader);
		}
		break;
	default:
		break;
	}
}


void CG_DrawFullScreenFX( void ) 
{
	int i = 0, t;
	float alpha, redalpha, greenalpha;

	if ( (cg.snap->ps.clientNum != cg.predictedPlayerState.clientNum) || cg.renderingThirdPerson )
	{
		return;
	}

	t = cg.time - cg.damageTime;
	if ( t > 0 && t < DAMAGE_TIME) 
	{	// Draw the blobs.
		alpha = 1.0 - ((float)t / (float)DAMAGE_TIME);

		redalpha = alpha*cg.damageValue*1.5;
		if (redalpha > 1.0)
		{
			redalpha = 1.0;
		}

		greenalpha = alpha*cg.damageShieldValue*1.5;
		if (greenalpha > 1.0)
		{
			greenalpha = 1.0;
		}

		CG_DrawScreenBlob(redalpha, greenalpha);
	}

	for (i = 0; i < MAX_SCREENFX; i++)
	{
		if (theScreenFX.cgEndTimes[i])
		{
			if (theScreenFX.cgEndTimes[i] <= cg.time)
			{
				// remove this effect
				theScreenFX.events[i] = 0;
				theScreenFX.cgStartTimes[i] = 0;
				theScreenFX.cgEndTimes[i] = 0;
			}
			else
			{
				// still drawing this effect
				CG_DrawFullScreenEffect(theScreenFX.events[i],theScreenFX.cgStartTimes[i],theScreenFX.cgEndTimes[i]);
			}
		}
	}
}
