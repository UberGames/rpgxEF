// Copyright (C) 1999-2000 Id Software, Inc.
//
// cg_ents.c -- present snapshot entities, happens every single frame

#include "cg_local.h"
#include "fx_local.h"

static void CG_LaserSight( centity_t *cent ); // Laser
static void CG_Turbolift( centity_t *cent );

/*
======================
CG_PositionEntityOnTag

Modifies the entities position and axis by the given
tag location
======================
*/
void CG_PositionEntityOnTag( refEntity_t *entity, const refEntity_t *parent, 
							qhandle_t parentModel, char *tagName ) {
	int				i;
	orientation_t	lerped;
	
	// lerp the tag
	trap_R_LerpTag( &lerped, parentModel, parent->oldframe, parent->frame,
		1.0 - parent->backlerp, tagName );

	// FIXME: allow origin offsets along tag?
	VectorCopy( parent->origin, entity->origin );
	for ( i = 0 ; i < 3 ; i++ ) {
		VectorMA( entity->origin, lerped.origin[i], parent->axis[i], entity->origin );
	}

	// had to cast away the const to avoid compiler problems...
	MatrixMultiply( lerped.axis, ((refEntity_t *)parent)->axis, entity->axis );
	entity->backlerp = parent->backlerp;

}


/*
======================
CG_PositionRotatedEntityOnTag

Modifies the entities position and axis by the given
tag location
======================
*/
void CG_PositionRotatedEntityOnTag( refEntity_t *entity, const refEntity_t *parent, 
							qhandle_t parentModel, char *tagName ) {
	int				i;
	orientation_t	lerped;
	vec3_t			tempAxis[3];

//AxisClear( entity->axis );
	// lerp the tag
	trap_R_LerpTag( &lerped, parentModel, parent->oldframe, parent->frame,
		1.0 - parent->backlerp, tagName );

	// FIXME: allow origin offsets along tag?
	VectorCopy( parent->origin, entity->origin );
	for ( i = 0 ; i < 3 ; i++ ) {
		VectorMA( entity->origin, lerped.origin[i], parent->axis[i], entity->origin );
	}

	// had to cast away the const to avoid compiler problems...
	MatrixMultiply( entity->axis, lerped.axis, tempAxis );
	MatrixMultiply( tempAxis, ((refEntity_t *)parent)->axis, entity->axis );
}



/*
==========================================================================

FUNCTIONS CALLED EACH FRAME

==========================================================================
*/

/*
======================
CG_SetEntitySoundPosition

Also called by event processing code
======================
*/
void CG_SetEntitySoundPosition( centity_t *cent ) {
	if ( cent->currentState.solid == SOLID_BMODEL ) {
		vec3_t	origin;
		float	*v;

		v = cgs.inlineModelMidpoints[ cent->currentState.modelindex ];
		VectorAdd( cent->lerpOrigin, v, origin );
		trap_S_UpdateEntityPosition( cent->currentState.number, origin );
	} else {
		trap_S_UpdateEntityPosition( cent->currentState.number, cent->lerpOrigin );
	}
}

/*
==================
CG_EntityEffects

Add continuous entity effects, like local entity emission and lighting
==================
*/
static void CG_EntityEffects( centity_t *cent ) {

	// update sound origins
	CG_SetEntitySoundPosition( cent );

	// add loop sound
	if ( cent->currentState.loopSound && cent->currentState.loopSound < 256 ) {
		trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin, 
			cgs.gameSounds[ cent->currentState.loopSound ] );
	}


	// constant light glow
	if ( cent->currentState.constantLight ) {
		int		cl;
		int		i, r, g, b;

		cl = cent->currentState.constantLight;
		r = cl & 255;
		g = ( cl >> 8 ) & 255;
		b = ( cl >> 16 ) & 255;
		i = ( ( cl >> 24 ) & 255 ) * 4;
		trap_R_AddLightToScene( cent->lerpOrigin, i, r, g, b );
	}

}


/*
==================
CG_Useable
==================
*/
static void CG_Useable( centity_t *cent ) {
	refEntity_t			ent;
	entityState_t		*s1;

	s1 = &cent->currentState;

	// if set to invisible, skip
	if (!s1->modelindex) {
		return;
	}

	if (s1->modelindex == HI_SHIELD) 
	{	// The portable shield should go through a different rendering function.
		FX_DrawPortableShield(cent);
		return;
	}


	memset (&ent, 0, sizeof(ent));

	// set frame

	if (s1->eFlags & EF_ANIM_ALLFAST)
	{
		//ent.frame = (cg.time / 100);
		ent.frame = (cg.time * 0.01);
		ent.renderfx|=RF_WRAP_FRAMES;
	}
	else
	{
		ent.frame = s1->frame;
	}
	ent.oldframe = ent.frame;
	ent.backlerp = 0;

	VectorCopy( cent->lerpOrigin, ent.origin);
	VectorCopy( cent->lerpOrigin, ent.oldorigin);

	ent.hModel = cg_items[s1->modelindex2].model;//cgs.useableModels[s1->modelindex];

	// player model
	if (s1->number == cg.snap->ps.clientNum) {
		ent.renderfx |= RF_THIRD_PERSON;	// only draw from mirrors
	}

	// convert angles to axis
//	AnglesToAxis( cent->lerpAngles, ent.axis );
	{
		// hack to keep dropped detpacks from rotating 
		vec3_t vecs[3];
		AngleVectors(s1->angles, vecs[0], vecs[1], vecs[2]);
		VectorNegate(vecs[1], vecs[1]);
		if (s1->modelindex == HI_DETPACK) // as stated, HACK for detpack
		{
			VectorScale(vecs[0], .5, vecs[0]);
			VectorScale(vecs[1], .5, vecs[1]);
			VectorScale(vecs[2], .5, vecs[2]);
		}
		AxisCopy( vecs, ent.axis );
	}

	// add to refresh list
	trap_R_AddRefEntityToScene (&ent);
}

/*
==================
CG_General
==================
*/
#define ITEM_SCALEUP_DIV 1.0/(float)ITEM_SCALEUP_TIME
static void CG_General( centity_t *cent ) {
	refEntity_t			ent;
	entityState_t		*s1;

	s1 = &cent->currentState;

	// if set to invisible, skip
	if (!s1->modelindex) {
		return;
	}

	memset (&ent, 0, sizeof(ent));

	// set frame

	if ( s1->eFlags & EF_ANIM_ONCE )
	{
		ent.frame = s1->frame;
		ent.renderfx|=RF_CAP_FRAMES;
	}
	else if (s1->eFlags & EF_ANIM_ALLFAST)
	{
		//ent.frame = (cg.time / 100);
		ent.frame = (cg.time * 0.01);
		ent.renderfx|=RF_WRAP_FRAMES;
	}
	else
	{
		ent.frame = s1->frame;
		ent.renderfx|=RF_CAP_FRAMES;
	}

	ent.oldframe = ent.frame;
	ent.backlerp = 0;

	VectorCopy( cent->lerpOrigin, ent.origin);
	VectorCopy( cent->lerpOrigin, ent.oldorigin);

	ent.hModel = cgs.gameModels[s1->modelindex];

	// player model
	if (s1->number == cg.snap->ps.clientNum) {
		ent.renderfx |= RF_THIRD_PERSON;	// only draw from mirrors
	}

	// convert angles to axis
	AnglesToAxis( cent->lerpAngles, ent.axis );

	// add to refresh list
	if ( s1->eFlags & EF_ITEMPLACEHOLDER )		// object is "spawning" in
	{
		int	msec;

		if ( !cent->miscTime )
		{
			cent->miscTime = cg.time;
		}

		msec = cg.time - cent->miscTime;
		if ( msec < ITEM_SCALEUP_TIME )
		{
			float alpha;
			int a;
			alpha = (float)msec * ITEM_SCALEUP_DIV;
			if ( s1->eventParm == 255 )
			{
				alpha = 1.0f-alpha;
			}
			a = alpha * 255.0;
			if (a <= 0)
				a=1;
			ent.shaderRGBA[0] =
			ent.shaderRGBA[1] =
			ent.shaderRGBA[2] = 255;
			ent.shaderRGBA[3] = a;
			ent.renderfx |= RF_FORCE_ENT_ALPHA;
			trap_R_AddRefEntityToScene(&ent);
			ent.renderfx &= ~RF_FORCE_ENT_ALPHA;
		
			// Now draw the static shader over it.
			// Alpha in over half the time, out over half.
			alpha = sin(M_PI*alpha);
			a = alpha * 255.0;
			if (a <= 0)
				a=1;
			ent.customShader = cgs.media.rezOutShader;
			ent.shaderRGBA[0] =
			ent.shaderRGBA[1] =
			ent.shaderRGBA[2] = a;
			trap_R_AddRefEntityToScene( &ent );
		}
		else
		{
			trap_R_AddRefEntityToScene (&ent);
		}
	}
	else
	{
		cent->miscTime = 0;
		trap_R_AddRefEntityToScene (&ent);
	}
}


/*
==================
CG_Speaker

Speaker entities can automatically play sounds
==================
*/
static void CG_Speaker( centity_t *cent ) {
	if ( ! cent->currentState.clientNum ) {	// FIXME: use something other than clientNum...
		return;		// not auto triggering
	}

	if ( cg.time < cent->miscTime ) {
		return;
	}

	trap_S_StartSound (NULL, cent->currentState.number, CHAN_ITEM, cgs.gameSounds[cent->currentState.eventParm] );

	//	ent->s.frame = ent->wait * 10;
	//	ent->s.clientNum = ent->random * 10;
	cent->miscTime = cg.time + cent->currentState.frame * 100 + cent->currentState.clientNum * 100 * crandom();
}

/*
==================
CG_Item
==================
*/
static void CG_Item( centity_t *cent ) {
	refEntity_t			ent;
	entityState_t		*es;
	gitem_t				*item;
	int					msec;
//	float				scale;
    // RPG-X: Marcin: Custom angles for each weapon so they lie on the ground correctly. - 06/12/2008
    const vec3_t weaponangles[WP_NUM_WEAPONS] = {
        { 0,   0,   0   },  // WP_0
        { 0,   0,   0   },  // WP_1
        { 52,  280, 18  },  // WP_2
        { 48,  26,  33  },  // WP_3
        { 335, 210, 347 },  // WP_4
        { 15,  160, 65  },  // WP_5
        { 5,   10,  70  },  // WP_6
        { 5,   6,   70  },  // WP_7
        { 5,   17,  70  },  // WP_8
        { 350, 23,  70  },  // WP_9
        { 15,  187, 80  },  // WP_10
        { 0,   270, 86  },  // WP_11
        { 0,   247, 90  },  // WP_12
        { 36,  190, 40  },  // WP_13
        { 0,   0,   105 },  // WP_14
        { 0,   210, 90  }   // WP_15
    };

	es = &cent->currentState;
	if ( es->modelindex >= bg_numItems ) {
		CG_Error( "Bad item index %i on entity", es->modelindex );
	}

	// if set to invisible, skip
	if ( !es->modelindex || ( es->eFlags & EF_NODRAW ) ) {
		return;
	}

	item = &bg_itemlist[ es->modelindex ];
	if ( cg_simpleItems.integer && item->giType != IT_TEAM ) {
		memset( &ent, 0, sizeof( ent ) );
		ent.reType = RT_SPRITE;
		VectorCopy( cent->lerpOrigin, ent.origin );
		ent.data.sprite.radius = 14;
	
		ent.customShader = cg_items[es->modelindex].icon;
		ent.shaderRGBA[0] = 255;
		ent.shaderRGBA[1] = 255;
		ent.shaderRGBA[2] = 255;

		if ( es->eFlags & EF_ITEMPLACEHOLDER )
		{
			ent.renderfx |= RF_FORCE_ENT_ALPHA;
			ent.shaderRGBA[3] = 50 + sin(cg.time*0.01)*30;
		}
		else
		{
			ent.shaderRGBA[3] = 255;
		}

		trap_R_AddRefEntityToScene(&ent);

		return;
	}

	// items bob up and down continuously
//	scale = 0.005 + cent->currentState.number * 0.00001;
//	cent->lerpOrigin[2] += 4 + cos( ( cg.time + 1000 ) *  scale ) * 4;

	memset (&ent, 0, sizeof(ent));


	// autorotate at one of two speeds
	if ( item->giType == IT_HEALTH ) 
	{
		VectorCopy( cg.autoAnglesFast, cent->lerpAngles );
		AxisCopy( cg.autoAxisFast, ent.axis );
	} 
	else if (item->giType != IT_TEAM) // RPG-X | Marcin | 05/12/2008
    {
		//VectorCopy( cg.autoAngles, cent->lerpAngles );
        //AxisCopy( cg.autoAxis, ent.axis );
        VectorCopy( weaponangles[item->giTag], cent->lerpAngles );
		AnglesToAxis( weaponangles[item->giTag], ent.axis);
	}
	else
	{	// Flags don't rotate at all...
		float frame;
		vec3_t vecs[3];


		// ...but they do animate.
		//frame = (cg.time / 100.0);
		frame = (cg.time * 0.01);
		ent.renderfx|=RF_WRAP_FRAMES;

		ent.oldframe = (int)frame;
		ent.frame = (int)frame+1;
		ent.backlerp = (float)(ent.frame) - frame;

		// and they are scaled too
		if (1)
		{
			AngleVectors(es->angles, vecs[0], vecs[1], vecs[2]);
			VectorScale( vecs[0], 1.6, vecs[0] );
			VectorScale( vecs[1], 1.6, vecs[1] );
			VectorScale( vecs[2], 1.6, vecs[2] );
			AxisCopy( vecs, ent.axis );
		}
		else
		{
			AnglesToAxis(cent->lerpAngles, ent.axis);
			VectorScale( ent.axis[0], 1.6, ent.axis[0] );
			VectorScale( ent.axis[1], 1.6, ent.axis[1] );
			VectorScale( ent.axis[2], 1.6, ent.axis[2] );
			ent.nonNormalizedAxes = qtrue;
		}
		if (item->giTag == PW_BORG_ADAPT)
		{ 
			//ent.customShader = cgs.media.blueFlagShader[3];
		}
		else
		{
			ent.customShader = cgs.media.redFlagShader[3];
		}
	}

	// the weapons have their origin where they attatch to player
	// models, so we need to offset them or they will rotate
	// eccentricly
	if ( item->giType == IT_WEAPON ) {
		weaponInfo_t	*wi;

		wi = &cg_weapons[item->giTag];
		cent->lerpOrigin[0] -= 
			wi->weaponMidpoint[0] * ent.axis[0][0] +
			wi->weaponMidpoint[1] * ent.axis[1][0] +
			wi->weaponMidpoint[2] * ent.axis[2][0];
		cent->lerpOrigin[1] -= 
			wi->weaponMidpoint[0] * ent.axis[0][1] +
			wi->weaponMidpoint[1] * ent.axis[1][1] +
			wi->weaponMidpoint[2] * ent.axis[2][1];
		cent->lerpOrigin[2] -= 
			wi->weaponMidpoint[0] * ent.axis[0][2] +
			wi->weaponMidpoint[1] * ent.axis[1][2] +
			wi->weaponMidpoint[2] * ent.axis[2][2];

		cent->lerpOrigin[2] -= 14;	// an extra height boost
                                    // RPG-X | Marcin | 03/12/2008
                                    // (Was += 8)
	}

	ent.hModel = cg_items[es->modelindex].model;

	VectorCopy( cent->lerpOrigin, ent.origin);
	VectorCopy( cent->lerpOrigin, ent.oldorigin);

	ent.nonNormalizedAxes = qfalse;

	if ( es->eFlags & EF_ITEMPLACEHOLDER )		// item has been picked up
	{
		if ( es->eFlags & EF_DEAD )				// if item had been droped, don't show at all
			return;

		ent.customShader = cgs.media.weaponPlaceholderShader;
	}

    // RPG-X | Marcin | 06/12/2008
	// increase the size of the weapons when they are presented as items - DON'T!
	/*if ( item->giType == IT_WEAPON ) {
		VectorScale( ent.axis[0], 1.5, ent.axis[0] );
		VectorScale( ent.axis[1], 1.5, ent.axis[1] );
		VectorScale( ent.axis[2], 1.5, ent.axis[2] );
		ent.nonNormalizedAxes = qtrue;
	}*/

	msec = cg.time - cent->miscTime;	// Count from last respawn.
	/*if (cg.predictedPlayerState.introTime > cg.time)
	{	// The stuff is "holodecking in".
		int dtime;

		dtime = cg.predictedPlayerState.introTime - cg.time;
		if (dtime < TIME_FADE_DUR)
		{	// "rez" in.
			float alpha;
			int a;
			
			alpha = 1.0 - ((float)dtime / (float)TIME_FADE_DUR);
			a = alpha * 255.0;
			if (a <= 0)
				a=1;
			ent.shaderRGBA[3] = a;
			ent.renderfx |= RF_FORCE_ENT_ALPHA;
			trap_R_AddRefEntityToScene(&ent);
			ent.renderfx &= ~RF_FORCE_ENT_ALPHA;
		
			// Now draw the static shader over it.
			// Alpha in over half the time, out over half.
			alpha = sin(M_PI*alpha);
			a = alpha * 255.0;
			if (a <= 0)
				a=1;
			ent.customShader = cgs.media.rezOutShader;
			ent.shaderRGBA[0] =
			ent.shaderRGBA[1] =
			ent.shaderRGBA[2] = a;
			trap_R_AddRefEntityToScene( &ent );
			ent.shaderRGBA[0] =
			ent.shaderRGBA[1] =
			ent.shaderRGBA[2] = 255;
		}
	}
	else*/ if (item->giType != IT_TEAM && msec >= 0 && msec < ITEM_SCALEUP_TIME && !(es->eFlags & EF_ITEMPLACEHOLDER)) 
	{	// if just respawned, fade in, but don't do this for flags.
		float alpha;
		int a;
		
		alpha = (float)msec * ITEM_SCALEUP_DIV;
		a = alpha * 255.0;
		if (a <= 0)
			a=1;
		ent.shaderRGBA[3] = a;
		ent.renderfx |= RF_FORCE_ENT_ALPHA;
		trap_R_AddRefEntityToScene(&ent);
		ent.renderfx &= ~RF_FORCE_ENT_ALPHA;
	
		// Now draw the static shader over it.
		// Alpha in over half the time, out over half.
		alpha = sin(M_PI*alpha);
		a = alpha * 255.0;
		if (a <= 0)
			a=1;
		ent.customShader = cgs.media.rezOutShader;
		ent.shaderRGBA[0] =
		ent.shaderRGBA[1] =
		ent.shaderRGBA[2] = a;
		trap_R_AddRefEntityToScene( &ent );
		//what is the point of this next bit???
		ent.shaderRGBA[0] =
		ent.shaderRGBA[1] =
		ent.shaderRGBA[2] = 255;
	}
	else
	{	// add to refresh list  -- normal item
		trap_R_AddRefEntityToScene(&ent);
	}	
}

//============================================================================

/*
===============
CG_Missile
===============
*/
static void CG_Missile( centity_t *cent, qboolean altfire ) {
	refEntity_t			ent;
	entityState_t		*s1;
	qhandle_t			missile = 0;
	const weaponInfo_t		*weapon;
	int     rpg_tripmines;
	const char	*info;
	
	s1 = &cent->currentState;
	if ( s1->weapon > WP_NUM_WEAPONS ) {
		s1->weapon = 0;
	}
	weapon = &cg_weapons[s1->weapon];

	// calculate the axis
	VectorCopy( s1->angles, cent->lerpAngles);

//	if (cent->currentState.eFlags & EF_ALT_FIRING)
	if (altfire)
	{
		// add trails
		if ( weapon->alt_missileTrailFunc ) 
		{
			weapon->alt_missileTrailFunc( cent, weapon );
		}

		// add dynamic light
		if ( weapon->alt_missileDlight ) {
			trap_R_AddLightToScene(cent->lerpOrigin, weapon->alt_missileDlight, 
				weapon->missileDlightColor[0], weapon->missileDlightColor[1], weapon->missileDlightColor[2] );
		}

		// add missile sound
		if ( weapon->alt_missileSound ) 
		{
			vec3_t	velocity;

			BG_EvaluateTrajectoryDelta( &cent->currentState.pos, cg.time, velocity );
			if (velocity[0] || velocity[1] || velocity[2])
			{
				trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, velocity, weapon->alt_missileSound );
			}
		}
		//RPG-X: RedTechie - non-admin see no tripmines! But first by popular demand check CVAR
		info = CG_ConfigString( CS_SERVERINFO );
		rpg_tripmines = atoi( Info_ValueForKey( info, "rpg_invisibletripmines" ) );
		if (!weapon->alt_missileModel || (!cgs.clientinfo[cg.snap->ps.clientNum].isAdmin/*cg.snap->ps.persistant[PERS_CLASS] != PC_ADMIN*/ && rpg_tripmines == 1)) {
			//if there is no missile then we're done
			return;
		}
		missile = weapon->alt_missileModel;
	}
	else
	{
		if (cent->thinkFlag)
		{	// we already grabbed info that was stored on the game side, so use what's already in cent->rawAngles
			//and cent->rawOrigin
		}
		else
		{
			// kef -- get out some info we stored in a very unfortunate manner on the game side
			VectorCopy(cent->currentState.angles2,cent->rawAngles);
			VectorCopy(cent->currentState.angles2,cent->rawOrigin);
			cent->thinkFlag = 1;
		}
		// add trails
		if ( weapon->missileTrailFunc ) 
		{
			weapon->missileTrailFunc( cent, weapon );
		}

		// add dynamic light
		if ( weapon->missileDlight ) {
			trap_R_AddLightToScene(cent->lerpOrigin, weapon->missileDlight, 
				weapon->missileDlightColor[0], weapon->missileDlightColor[1], weapon->missileDlightColor[2] );
		}

		// add missile sound
/*		if ( weapon->missileSound ) {
			vec3_t	velocity;

			BG_EvaluateTrajectoryDelta( &cent->currentState.pos, cg.time, velocity );

			trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, velocity, weapon->missileSound );
		}
*/
		if (!weapon->missileModel) {	//if ther is no missile then we're done
			return;
		}
		missile = weapon->missileModel;
	}

	// create the render entity
	memset (&ent, 0, sizeof(ent));
	VectorCopy( cent->lerpOrigin, ent.origin);
	VectorCopy( cent->lerpOrigin, ent.oldorigin);

	if ( cent->currentState.weapon == WP_9 ) {
		ent.reType = RT_SPRITE;
		ent.data.sprite.radius = 16;
		ent.data.sprite.rotation = 0;
		trap_R_AddRefEntityToScene( &ent );
		return;
	}

	// flicker between two skins
	ent.skinNum = cg.clientFrame & 1;
	ent.hModel = missile;
	ent.renderfx = RF_NOSHADOW;

	if ( s1->pos.trType == TR_STATIONARY )
	{
		AnglesToAxis( s1->angles, ent.axis );
	}
	else
	{
		// convert direction of travel into axis
		if ( VectorNormalize2( s1->pos.trDelta, ent.axis[0] ) == 0 ) {
			ent.axis[0][2] = 1;
		}

		// spin as it moves
		if ( s1->pos.trType != TR_STATIONARY && (cent->currentState.weapon != WP_10) ) { //RPG-X-TiM: Stop from spinning O_o  I got dizzy
			//RotateAroundDirection( ent.axis, cg.time / 4 );
			RotateAroundDirection( ent.axis, cg.time * 0.25);
		} else {
			RotateAroundDirection( ent.axis, s1->time );
		}
	}

	// add to refresh list, possibly with quad glow
	CG_AddRefEntityWithPowerups( &ent, s1->powerups, s1->eFlags, &cent->beamData, cent->cloakTime, cent->decloakTime, qfalse );
	if ( s1->eFlags & EF_FIRING )
	{//special code for adding the beam to the attached tripwire mine
		trace_t	trace;
		vec3_t	beamOrg, beamEnd, rgb;
		float	alpha;
		qhandle_t	flareShader;
		
		if ( s1->otherEntityNum2 == TEAM_BLUE )
		{
			VectorSet( rgb, 0.0f, 0.3f, 1.0f );
			alpha = 1.0f - (random() * 0.2);
			flareShader = cgs.media.blueParticleShader;
		}
		else
		{
			VectorSet( rgb, 1.0f, 0.0f, 0.0f );
			alpha = 1.0f - (random() * 0.5);
			flareShader = cgs.media.borgEyeFlareShader;
		}
		VectorCopy( ent.origin, beamOrg );
		VectorMA( beamOrg, -2, ent.axis[0], beamOrg );//forward
		VectorMA( beamOrg, -1022, ent.axis[0], beamEnd);//forward to end, have to reverse it because it actually faces other way

		trap_CM_BoxTrace( &trace, beamOrg, beamEnd, NULL, NULL, 0, MASK_SHOT );
		VectorCopy(trace.endpos, beamEnd);
		FX_AddLine2( beamOrg, beamEnd, 1.0f, 0.35f + ( crandom() * 0.1 ), 0.0f, 0.35f + ( crandom() * 0.1 ), 0.0f, alpha, alpha, rgb, rgb,1.0f, cgs.media.whiteLaserShader );
		//FX_AddSprite( beamOrg, NULL, qfalse, 1.0f + (random() * 2.0f), 0.0f, 0.9f, 0.9f, 0.0f, 0.0f, 0.0f, flareShader );
		FX_AddQuad( beamOrg, ent.axis[0], 1.0f, 1.0f, 2.0f + (crandom() * 1.0f), 0.0f, 0.0f, 1.0f, flareShader );
		FX_AddQuad( beamEnd, trace.plane.normal, 1.0f, 1.0f, 2.0f + (crandom() * 1.0f), 0.0f, 0.0f, 1.0f, flareShader );
	}
}

/*
===============
CG_Mover
===============
*/
static void CG_Mover( centity_t *cent ) {
	refEntity_t			ent;
	entityState_t		*s1;

	s1 = &cent->currentState;

	// create the render entity
	memset (&ent, 0, sizeof(ent));
	VectorCopy( cent->lerpOrigin, ent.origin);
	VectorCopy( cent->lerpOrigin, ent.oldorigin);
	AnglesToAxis( cent->lerpAngles, ent.axis );

	ent.renderfx = RF_NOSHADOW;

	// flicker between two skins (FIXME?)
	ent.skinNum = ( cg.time >> 6 ) & 1;

	// get the model, either as a bmodel or a modelindex
	if ( s1->solid == SOLID_BMODEL ) {
		ent.hModel = cgs.inlineDrawModel[s1->modelindex];
	} else {
		ent.hModel = cgs.gameModels[s1->modelindex];
	}

	// add to refresh list
	trap_R_AddRefEntityToScene(&ent);

	// add the secondary model
	if ( s1->modelindex2 ) {
		ent.skinNum = 0;
		ent.hModel = cgs.gameModels[s1->modelindex2];
		trap_R_AddRefEntityToScene(&ent);
	}

}

/*
===============
CG_Beam

Also called as an event
===============
*/
void CG_Beam( centity_t *cent ) {
	refEntity_t			ent;
	entityState_t		*s1;

	s1 = &cent->currentState;

	// create the render entity
	memset (&ent, 0, sizeof(ent));
	VectorCopy( s1->pos.trBase, ent.origin );
	VectorCopy( s1->origin2, ent.oldorigin );
	AxisClear( ent.axis );
	ent.reType = RT_BEAM;

	ent.renderfx = RF_NOSHADOW;

	// add to refresh list
	trap_R_AddRefEntityToScene(&ent);
}


/*
RPG-X: RedTechie - FIXME: STILL FRICKEN SWAYS its not a sailing ship its a fricken star ship!
===============
CG_Portal
===============
*/
static void CG_Portal( centity_t *cent ) {
	refEntity_t			ent;
	entityState_t		*s1;

	s1 = &cent->currentState;

	// create the render entity
	memset (&ent, 0, sizeof(ent));
	VectorCopy( cent->lerpOrigin, ent.origin );
	VectorCopy( s1->origin2, ent.oldorigin );
	ByteToDir( s1->eventParm, ent.axis[0] );
	PerpendicularVector( ent.axis[1], ent.axis[0] );

	// negating this tends to get the directions like they want
	// we really should have a camera roll value
	VectorSubtract( vec3_origin, ent.axis[1], ent.axis[1] );

	CrossProduct( ent.axis[0], ent.axis[1], ent.axis[2] );
	ent.reType = RT_PORTALSURFACE;
	
	ent.frame = s1->frame;		// rotation speed - s1->frame
	ent.skinNum = s1->clientNum / 256 * 360;	// roll offset //RPG-X: RedTechie - ent.skinNum = s1->clientNum/256.0 * 360;
	//ent.oldframe = 0;

	// add to refresh list
	trap_R_AddRefEntityToScene(&ent);
}


/*
=========================
CG_AdjustPositionForMover

Also called by client movement prediction code
=========================
*/
void CG_AdjustPositionForMover( const vec3_t in, int moverNum, int fromTime, int toTime, vec3_t out ) {
	centity_t	*cent;
	vec3_t	oldOrigin, origin, deltaOrigin;
	vec3_t	oldAngles, angles, deltaAngles;

	if ( moverNum <= 0 || moverNum >= ENTITYNUM_MAX_NORMAL ) {
		VectorCopy( in, out );
		return;
	}

	cent = &cg_entities[ moverNum ];
	if ( cent->currentState.eType != ET_MOVER && cent->currentState.eType != ET_MOVER_STR ) { //RPG-X | GSIO01 | 13/05/2009
		VectorCopy( in, out );
		return;
	}

	BG_EvaluateTrajectory( &cent->currentState.pos, fromTime, oldOrigin );
	BG_EvaluateTrajectory( &cent->currentState.apos, fromTime, oldAngles );

	BG_EvaluateTrajectory( &cent->currentState.pos, toTime, origin );
	BG_EvaluateTrajectory( &cent->currentState.apos, toTime, angles );

	VectorSubtract( origin, oldOrigin, deltaOrigin );
	VectorSubtract( angles, oldAngles, deltaAngles );

	VectorAdd( in, deltaOrigin, out );

	// FIXME: origin change when on a rotating object
}


/*
=============================
CG_InterpolateEntityPosition
=============================
*/
static void CG_InterpolateEntityPosition( centity_t *cent ) {
	vec3_t		current, next;
	float		f;

	// it would be an internal error to find an entity that interpolates without
	// a snapshot ahead of the current one
	if ( cg.nextSnap == NULL ) {
		CG_Error( "CG_InterpoateEntityPosition: cg.nextSnap == NULL" );
	}

	f = cg.frameInterpolation;

	// this will linearize a sine or parabolic curve, but it is important
	// to not extrapolate player positions if more recent data is available
	BG_EvaluateTrajectory( &cent->currentState.pos, cg.snap->serverTime, current );
	BG_EvaluateTrajectory( &cent->nextState.pos, cg.nextSnap->serverTime, next );

	cent->lerpOrigin[0] = current[0] + f * ( next[0] - current[0] );
	cent->lerpOrigin[1] = current[1] + f * ( next[1] - current[1] );
	cent->lerpOrigin[2] = current[2] + f * ( next[2] - current[2] );

	BG_EvaluateTrajectory( &cent->currentState.apos, cg.snap->serverTime, current );
	BG_EvaluateTrajectory( &cent->nextState.apos, cg.nextSnap->serverTime, next );

	cent->lerpAngles[0] = LerpAngle( current[0], next[0], f );
	cent->lerpAngles[1] = LerpAngle( current[1], next[1], f );
	cent->lerpAngles[2] = LerpAngle( current[2], next[2], f );

}

/*
===============
CG_CalcEntityLerpPositions

===============
*/
static void CG_CalcEntityLerpPositions( centity_t *cent ) {
	if ( cent->interpolate && cent->currentState.pos.trType == TR_INTERPOLATE ) {
		CG_InterpolateEntityPosition( cent );
		return;
	}
	
	// just use the current frame and evaluate as best we can
	BG_EvaluateTrajectory( &cent->currentState.pos, cg.time, cent->lerpOrigin );
	BG_EvaluateTrajectory( &cent->currentState.apos, cg.time, cent->lerpAngles );

	// adjust for riding a mover if it wasn't rolled into the predicted
	// player state
	if ( cent != &cg.predictedPlayerEntity ) {
		CG_AdjustPositionForMover( cent->lerpOrigin, cent->currentState.groundEntityNum, 
		cg.snap->serverTime, cg.time, cent->lerpOrigin );
	}
}

/*
===============
CG_AddCEntity

===============
*/
static void CG_AddCEntity( centity_t *cent ) {
	// event-only entities will have been dealt with already
	if ( cent->currentState.eType >= ET_EVENTS ) {
		return;
	}

	// calculate the current origin
	CG_CalcEntityLerpPositions( cent );

	// add automatic effects
	CG_EntityEffects( cent );

	switch ( cent->currentState.eType ) {
	default:
		CG_Error( "Bad entity type: %i\n", cent->currentState.eType );
		break;
	case ET_TRIC_STRING:
	case ET_INVISIBLE:
	case ET_PUSH_TRIGGER:
	case ET_TELEPORT_TRIGGER:
		break;
	case ET_USEABLE:		// e.g. detpacks
		CG_Useable( cent );
		break;
	case ET_GENERAL:
		CG_General( cent );
		break;
	case ET_PLAYER:
		CG_Player( cent );
		break;
	case ET_ITEM:
		CG_Item( cent );
		break;
	case ET_ALT_MISSILE:
		CG_Missile( cent, qtrue );
		break;
	case ET_MISSILE:
		CG_Missile( cent, qfalse );
		break;
	case ET_MOVER:
	case ET_MOVER_STR:
		CG_Mover( cent );
		break;
	case ET_BEAM:
		CG_Beam( cent );
		break;
	case ET_PORTAL:
		CG_Portal( cent );
		break;
	case ET_SPEAKER:
		CG_Speaker( cent );
		break;
	case ET_LASER:
		CG_LaserSight( cent );
		break;
	case ET_TURBOLIFT:
		CG_Turbolift( cent );
		break;
	}
}

/*
===============
CG_AddPacketEntities

===============
*/
void CG_AddPacketEntities( void ) {
	int					num;
	centity_t			*cent;
	playerState_t		*ps;

	// set cg.frameInterpolation
	if ( cg.nextSnap ) {
		int		delta;

		delta = (cg.nextSnap->serverTime - cg.snap->serverTime);
		if ( delta == 0 ) {
			cg.frameInterpolation = 0;
		} else {
			cg.frameInterpolation = (float)( cg.time - cg.snap->serverTime ) / delta;
		}
	} else {
		cg.frameInterpolation = 0;	// actually, it should never be used, because 
									// no entities should be marked as interpolating
	}


	// the auto-rotating items will all have the same axis

    // RPG-X | Marcin | 03/12/2008
    // We don't want dropped weapons to rotate at all
    // -- NOT USED ANY MORE THOUGH --
    cg.autoAngles[0] = 0;
	cg.autoAngles[1] = 0;
	cg.autoAngles[2] = 68;

	cg.autoAnglesFast[0] = 0;
	cg.autoAnglesFast[1] = ( cg.time & 1023 ) * 360 / 1024;
	cg.autoAnglesFast[2] = 0;

	AnglesToAxis( cg.autoAngles, cg.autoAxis );
	AnglesToAxis( cg.autoAnglesFast, cg.autoAxisFast );

	// generate and add the entity from the playerstate
	ps = &cg.predictedPlayerState;
	BG_PlayerStateToEntityState( ps, &cg.predictedPlayerEntity.currentState, qfalse );
	CG_AddCEntity( &cg.predictedPlayerEntity );

	// lerp the non-predicted value for lightning gun origins
	CG_CalcEntityLerpPositions( &cg_entities[ cg.snap->ps.clientNum ] );

	// add each entity sent over by the server
	for ( num = 0 ; num < cg.snap->numEntities ; num++ ) {
		cent = &cg_entities[ cg.snap->entities[ num ].number ];
		CG_AddCEntity( cent );
	}
}

/*
==================
CG_LaserSight
  Creates the laser
==================
*/

static void CG_LaserSight( centity_t *cent )  {
	refEntity_t			ent;

	// create the render entity
	memset (&ent, 0, sizeof(ent));
	VectorCopy( cent->lerpOrigin, ent.origin);
	VectorCopy( cent->lerpOrigin, ent.oldorigin);

	if (cent->currentState.eventParm == 1)
	{
		ent.reType = RT_SPRITE;
		//ent.radius = 2;
		//ent.rotation = 0;
		ent.data.sprite.radius = 2;
		ent.customShader = cgs.media.laserShader;
		trap_R_AddRefEntityToScene( &ent );
	}
	else	{
		trap_R_AddLightToScene(ent.origin, 200, 1, 1, 1);
	}

	
}

/*
==================
CG_Turbolift
A client complement
to the turbolift ent,
this plays the sound
FX whilst it is in action
==================
*/

static void CG_Turbolift( centity_t* cent )
{
	int i;
	centity_t *player;
	//TiM - find all of the cents inside the lift, and make it so they orient to their view angles.
	//Otherwise, when they teleport, the entire body snaps around, looking weird and/or painful.
	
	if ( cent->currentState.time2 <= 0 )
		return;

	for ( i = 0; i < MAX_CLIENTS; i++ )
	{
		player = &cg_entities[i];

		if ( !player )
			continue;

		if ( ( player->lerpOrigin[0] > cent->currentState.angles2[0] && player->lerpOrigin[0] < cent->currentState.origin2[0] )
			 && ( player->lerpOrigin[1] > cent->currentState.angles2[1] && player->lerpOrigin[1] < cent->currentState.origin2[1] )
			  && ( player->lerpOrigin[2] > cent->currentState.angles2[2] && player->lerpOrigin[2] < cent->currentState.origin2[2] ) )
		{
			//time2 = startTime+waittime
			if ( cent->currentState.time2 > 0 ) 
			{
				cg_liftEnts[player->currentState.clientNum] = cent->currentState.time2;
			}
			else
			{
				cg_liftEnts[player->currentState.clientNum] = 0;
			}
		}
	}

	trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin, //cent->lerpOrigin
			cgs.gameSounds[ cent->currentState.loopSound ] );
}

/*static void CG_Turbolift( centity_t* cent )
{
	if ( cent->currentState.eventParm <= 0 )
	{
		cent->deathTime = 0;
		cent->miscTime = 0;
		return;
	}

	//Init sound
	if ( cent->miscTime == 0 )
	{
		cent->miscTime = cg.time + cent->currentState.modelindex2; //set the end of the wait time
	}

	if ( cg.time < cent->miscTime )
	{
		trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin, //cent->lerpOrigin
			cgs.gameSounds[ cent->currentState.loopSound ] );

		return;
	}
	
	if ( cent->deathTime == 0 )
	{
		trap_S_StartSound (NULL, cent->currentState.number, CHAN_ITEM, cgs.gameSounds[cent->currentState.otherEntityNum2] );
		cent->deathTime = 1;
	}
}*/
