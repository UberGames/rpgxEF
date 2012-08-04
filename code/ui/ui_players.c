// Copyright (C) 1999-2000 Id Software, Inc.
//
// ui_players.c

#include "ui_local.h"


#define UI_TIMER_GESTURE		2300
#define UI_TIMER_JUMP			1000
#define UI_TIMER_LAND			130
#define UI_TIMER_WEAPON_SWITCH	300
#define UI_TIMER_ATTACK			500
#define	UI_TIMER_MUZZLE_FLASH	20
#define	UI_TIMER_WEAPON_DELAY	250

#define JUMP_HEIGHT				56

#define SWINGSPEED				0.2  //TiM - 0.3

#define SPIN_SPEED				0.9
#define COAST_TIME				1000


static int			dp_realtime;
static float		jumpHeight;

//TiM : Bolton Table
stringID_table_t BoltonTable[BOLTON_MAX + 1] =
{
	{ ENUM2STRING(BOLTON_HEAD) },
	{ ENUM2STRING(BOLTON_TORSO) },
	{ ENUM2STRING(BOLTON_LEGS) },
	{ NULL, -1 }
};

/*
===============
UI_PlayerInfo_SetWeapon
===============
*/
static void UI_PlayerInfo_SetWeapon( playerInfo_t *pi, weapon_t weaponNum ) {
	gitem_t *	item;
	char		path[MAX_QPATH];

	pi->currentWeapon = weaponNum;
tryagain:
	pi->realWeapon = weaponNum;
	pi->weaponModel = 0;
	pi->barrelModel = 0;
	pi->flashModel = 0;

	if ( weaponNum == WP_0 ) {
		return;
	}

	for ( item = bg_itemlist + 1; item->classname ; item++ ) {
		if ( item->giType != IT_WEAPON ) {
			continue;
		}
		if ( item->giTag == weaponNum ) {
			break;
		}
	}

	if ( item->classname ) {
		pi->weaponModel = trap_R_RegisterModel( item->world_model );
	}

	if( pi->weaponModel == 0 )
	{
		if( weaponNum == WP_5 )
		{

			weaponNum = WP_0;
			goto tryagain;
		}
		weaponNum = WP_5;
		goto tryagain;
	}

	strcpy( path, item->world_model );
	COM_StripExtension( path, path );
	strcat( path, "_flash.md3" );
	pi->flashModel = trap_R_RegisterModel( path );

	switch( weaponNum ) {
	case WP_8:
		MAKERGB( pi->flashDlightColor, 0.6, 0.6, 1 );
		break;

	case WP_10:
		MAKERGB( pi->flashDlightColor, 0.6, 0.6, 1 );
		break;

	case WP_5:
		MAKERGB( pi->flashDlightColor, 0, 0, 0 );
		break;

	case WP_13:
		MAKERGB( pi->flashDlightColor, 0.6, 0.6, 1 );
		break;

	case WP_1:
		//MAKERGB( pi->flashDlightColor, 0.6, 0.6, 1 );
		break;

	case WP_6:
		MAKERGB( pi->flashDlightColor, 0.16, 0.16, 1 );
		break;

	case WP_7:
		MAKERGB( pi->flashDlightColor, 0.6, 0.6, 1 );
		break;

	case WP_4:
		MAKERGB( pi->flashDlightColor, 1, 0.6, 0.6 );
		break;

	case WP_9:
		MAKERGB( pi->flashDlightColor, 0.6, 0.6, 1 );
		break;

	default:
		MAKERGB( pi->flashDlightColor, 1, 1, 1 );
		break;
	}
}


/*
===============
UI_ForceLegsAnim
===============
*/
static void UI_ForceLegsAnim( playerInfo_t *pi, int anim ) {
	pi->legsAnim = ( ( pi->legsAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;

	if ( anim == BOTH_JUMP1 ) {
		pi->legsAnimationTimer = UI_TIMER_JUMP;
	}
}


/*
===============
UI_SetLegsAnim
===============
*/
static void UI_SetLegsAnim( playerInfo_t *pi, int anim ) {
	if ( pi->pendingLegsAnim ) {
		anim = pi->pendingLegsAnim;
		pi->pendingLegsAnim = 0;
	}
	UI_ForceLegsAnim( pi, anim );
}


/*
===============
UI_ForceTorsoAnim
===============
*/
static void UI_ForceTorsoAnim( playerInfo_t *pi, int anim ) {
	pi->torsoAnim = ( ( pi->torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;

	/*.if ( anim == TORSO_GESTURE ) {
		pi->torsoAnimationTimer = UI_TIMER_GESTURE;
	}*/

	if ( anim == UI_GetAnim( ANIM_ATTACK, pi->currentWeapon, qtrue ) ) { //BOTH_ATTACK1 //Hack ROFL. Code can't see the ANIM defines from here. ANIM_ATTACK = 22
		pi->torsoAnimationTimer = UI_TIMER_ATTACK;
	}
}


/*
===============
UI_SetTorsoAnim
===============
*/
static void UI_SetTorsoAnim( playerInfo_t *pi, int anim ) {
	if ( pi->pendingTorsoAnim ) {
		anim = pi->pendingTorsoAnim;
		pi->pendingTorsoAnim = 0;
	}

	UI_ForceTorsoAnim( pi, anim );
}


/*
===============
UI_TorsoSequencing
===============
*/
static void UI_TorsoSequencing( playerInfo_t *pi ) {
	int		currentAnim;

	currentAnim = pi->torsoAnim & ~ANIM_TOGGLEBIT;

	if ( pi->weapon != pi->currentWeapon && !pi->upperEmoting ) {
		if ( currentAnim != TORSO_DROPWEAP1 ) {
			pi->torsoAnimationTimer = UI_TIMER_WEAPON_SWITCH;
			UI_ForceTorsoAnim( pi, TORSO_DROPWEAP1 );
		}
	}

	if ( pi->torsoAnimationTimer > 0 ) {
		return;
	}

	/*if( currentAnim == TORSO_GESTURE ) {
		UI_SetTorsoAnim( pi, TORSO_STAND );
		return;
	}*/

	if( currentAnim == UI_GetAnim( ANIM_ATTACK, pi->currentWeapon, qtrue ) ) {  //BOTH_ATTACK1 22 = ANIM_ATTACK
		//if ( pi->currentWeapon == WP_0 || pi->currentWeapon == WP_5  ) 
		if ( pi->currentWeapon != WP_6 
			&& pi->currentWeapon != WP_7
			&& pi->currentWeapon != WP_8
			&& pi->currentWeapon != WP_9 )
		{
			UI_SetTorsoAnim( pi, BOTH_STAND1 ); //TORSO_STAND
		}
		else 
		{
			UI_SetTorsoAnim( pi, BOTH_STAND4 ); //TORSO_STAND
		}

		return;
	}

	if ( currentAnim == TORSO_DROPWEAP1 ) {
		UI_PlayerInfo_SetWeapon( pi, pi->weapon );
		pi->torsoAnimationTimer = UI_TIMER_WEAPON_SWITCH;
		UI_ForceTorsoAnim( pi, TORSO_RAISEWEAP1 ); //TORSO_RAISE
		return;
	}

	if ( currentAnim == TORSO_RAISEWEAP1 ) {
		UI_SetTorsoAnim( pi, BOTH_STAND1 ); //STAND2
		return;
	}

	//TiM: Was playing a non-loop emote, so go back to default now
	if ( pi->upperEmoting ) {
		if ( !pi->upperLoopEmote )
		{
			UI_SetTorsoAnim( pi, BOTH_STAND1 );
			pi->upperEmoting = qfalse;
		}
		else
		{
			UI_SetTorsoAnim( pi, pi->upperLoopEmote );
			pi->upperEmoting = qfalse;
		}
		return;
	}
}


/*
===============
UI_LegsSequencing
===============
*/
static void UI_LegsSequencing( playerInfo_t *pi ) {
	int		currentAnim;

	currentAnim = pi->legsAnim & ~ANIM_TOGGLEBIT;

	if ( pi->legsAnimationTimer > 0 ) {
		if ( currentAnim == BOTH_JUMP1) {
			jumpHeight = JUMP_HEIGHT * sin( M_PI * ( UI_TIMER_JUMP - pi->legsAnimationTimer ) / UI_TIMER_JUMP );
		}
		return;
	}

	if ( currentAnim == BOTH_JUMP1) {
		UI_ForceLegsAnim( pi, BOTH_LAND1 );
		pi->legsAnimationTimer = UI_TIMER_LAND;
		jumpHeight = 0;
		return;
	}

	if ( currentAnim == BOTH_LAND1 ) {
		UI_SetLegsAnim( pi, BOTH_STAND1 );
		return;
	}

	//TiM: Was playing a non-loop emote, so go back to default now
	if ( pi->lowerEmoting ) {
		if ( !pi->lowerLoopEmote )
		{
			UI_SetLegsAnim( pi, BOTH_STAND1 );
			pi->lowerEmoting = qfalse;
		}
		else
		{
			UI_SetLegsAnim( pi, pi->lowerLoopEmote );
			pi->lowerEmoting = qfalse;
		}
		return;
	}
}


/*
======================
UI_PositionEntityOnTag
======================
*/
static void UI_PositionEntityOnTag( refEntity_t *entity, const refEntity_t *parent, 
							clipHandle_t parentModel, char *tagName ) {
	int				i;
	orientation_t	lerped;
	
	// lerp the tag
	trap_CM_LerpTag( &lerped, parentModel, parent->oldframe, parent->frame,
		1.0 - parent->backlerp, tagName );

	// FIXME: allow origin offsets along tag?
	VectorCopy( parent->origin, entity->origin );
	for ( i = 0 ; i < 3 ; i++ ) {
		VectorMA( entity->origin, lerped.origin[i], parent->axis[i], entity->origin );
	}

	// cast away const because of compiler problems
	MatrixMultiply( lerped.axis, ((refEntity_t*)parent)->axis, entity->axis );
	entity->backlerp = parent->backlerp;
}


/*
======================
UI_PositionRotatedEntityOnTag
======================
*/
static void UI_PositionRotatedEntityOnTag( refEntity_t *entity, const refEntity_t *parent, 
							clipHandle_t parentModel, char *tagName ) {
	int				i;
	orientation_t	lerped;
	vec3_t			tempAxis[3];

	// lerp the tag
	trap_CM_LerpTag( &lerped, parentModel, parent->oldframe, parent->frame,
		1.0 - parent->backlerp, tagName );

	// FIXME: allow origin offsets along tag?
	VectorCopy( parent->origin, entity->origin );
	for ( i = 0 ; i < 3 ; i++ ) {
		VectorMA( entity->origin, lerped.origin[i], parent->axis[i], entity->origin );
	}

	// cast away const because of compiler problems
	MatrixMultiply( entity->axis, ((refEntity_t *)parent)->axis, tempAxis );
	MatrixMultiply( lerped.axis, tempAxis, entity->axis );
}


/*
===============
UI_SetLerpFrameAnimation
===============
*/
static void UI_SetLerpFrameAnimation( playerInfo_t *ci, lerpFrame_t *lf, int newAnimation ) {
	animation_t	*anim;

	lf->animationNumber = newAnimation;
	newAnimation &= ~ANIM_TOGGLEBIT;

	if ( newAnimation < 0 || newAnimation >= MAX_ANIMATIONS ) {
		trap_Error( va("Bad animation number: %i", newAnimation) );
	}

	anim = &ci->animations[ newAnimation ];

	lf->animation = anim;
	lf->animationTime = lf->frameTime + anim->initialLerp;
}


/*
===============
UI_RunLerpFrame
===============
*/
static void UI_RunLerpFrame( playerInfo_t *ci, lerpFrame_t *lf, int newAnimation ) {
	int			f;
	animation_t	*anim;

	// see if the animation sequence is switching
	if ( newAnimation != lf->animationNumber || !lf->animation ) {
		UI_SetLerpFrameAnimation( ci, lf, newAnimation );
	}

	// if we have passed the current frame, move it to
	// oldFrame and calculate a new frame
	if ( dp_realtime >= lf->frameTime ) {
		lf->oldFrame = lf->frame;
		lf->oldFrameTime = lf->frameTime;

		// get the next frame based on the animation
		anim = lf->animation;

		/*if ( anim->numFrames < 0 ) {
			UI_SetLerpFrameAnimation( ci, lf, BOTH_STAND1 );
		}*/

		if ( dp_realtime < lf->animationTime ) {
			lf->frameTime = lf->animationTime;		// initial lerp
		} else {
			lf->frameTime = lf->oldFrameTime + anim->frameLerp;
		}
		f = ( lf->frameTime - lf->animationTime ) / anim->frameLerp;
		if ( f >= anim->numFrames ) {
			f -= anim->numFrames;
			if ( anim->loopFrames ) {
				f %= anim->loopFrames;
				f += anim->numFrames - anim->loopFrames;
			} else {
				f = anim->numFrames - 1;
				// the animation is stuck at the end, so it
				// can immediately transition to another sequence
				lf->frameTime = dp_realtime;
			}
		}
		lf->frame = anim->firstFrame + f;
		if ( dp_realtime > lf->frameTime ) {
			lf->frameTime = dp_realtime;
		}
	}

	if ( lf->frameTime > dp_realtime + 200 ) {
		lf->frameTime = dp_realtime;
	}

	if ( lf->oldFrameTime > dp_realtime ) {
		lf->oldFrameTime = dp_realtime;
	}
	// calculate current lerp value
	if ( lf->frameTime == lf->oldFrameTime ) {
		lf->backlerp = 0;
	} else {
		lf->backlerp = 1.0 - (float)( dp_realtime - lf->oldFrameTime ) / ( lf->frameTime - lf->oldFrameTime );
	}
}


/*
===============
UI_PlayerAnimation
===============
*/
static void UI_PlayerAnimation( playerInfo_t *pi, int *legsOld, int *legs, float *legsBackLerp,
						int *torsoOld, int *torso, float *torsoBackLerp ) {

	// legs animation
	pi->legsAnimationTimer -= uis.frametime;
	if ( pi->legsAnimationTimer < 0 ) {
		pi->legsAnimationTimer = 0;
	}

	UI_LegsSequencing( pi );

	if ( pi->legs.yawing && ( ( pi->legsAnim & ~ANIM_TOGGLEBIT ) == BOTH_STAND1 
		|| ( pi->legsAnim & ~ANIM_TOGGLEBIT ) == BOTH_STAND2
		|| ( pi->legsAnim & ~ANIM_TOGGLEBIT ) == BOTH_STAND4 ) ) 
	{
		UI_RunLerpFrame( pi, &pi->legs, LEGS_TURN1 );
	} else {
		UI_RunLerpFrame( pi, &pi->legs, pi->legsAnim );
	}
	*legsOld = pi->legs.oldFrame;
	*legs = pi->legs.frame;
	*legsBackLerp = pi->legs.backlerp;

	// torso animation
	pi->torsoAnimationTimer -= uis.frametime;
	if ( pi->torsoAnimationTimer < 0 ) {
		pi->torsoAnimationTimer = 0;
	}

	UI_TorsoSequencing( pi );

	UI_RunLerpFrame( pi, &pi->torso, pi->torsoAnim );
	*torsoOld = pi->torso.oldFrame;
	*torso = pi->torso.frame;
	*torsoBackLerp = pi->torso.backlerp;
}


/*
==================
UI_SwingAngles
==================
*/
static void UI_SwingAngles( float destination, float swingTolerance, float clampTolerance,
					float speed, float *angle, qboolean *swinging ) {
	float	swing;
	float	move;
	float	scale;

	if ( !*swinging ) {
		// see if a swing should be started
		swing = AngleSubtract( *angle, destination );
		if ( swing > swingTolerance || swing < -swingTolerance ) {
			*swinging = qtrue;
		}
	}

	if ( !*swinging ) {
		return;
	}
	
	// modify the speed depending on the delta
	// so it doesn't seem so linear
	swing = AngleSubtract( destination, *angle );
	scale = fabs( swing );
	if ( scale < swingTolerance * 0.5 ) {
		scale = 0.5;
	} else if ( scale < swingTolerance ) {
		scale = 1.0;
	} else {
		scale = 2.0;
	}

	// swing towards the destination angle
	if ( swing >= 0 ) {
		move = uis.frametime * scale * speed;
		if ( move >= swing ) {
			move = swing;
			*swinging = qfalse;
		}
		*angle = AngleMod( *angle + move );
	} else if ( swing < 0 ) {
		move = uis.frametime * scale * -speed;
		if ( move <= swing ) {
			move = swing;
			*swinging = qfalse;
		}
		*angle = AngleMod( *angle + move );
	}

	// clamp to no more than tolerance
	swing = AngleSubtract( destination, *angle );
	if ( swing > clampTolerance ) {
		*angle = AngleMod( destination - (clampTolerance - 1) );
	} else if ( swing < -clampTolerance ) {
		*angle = AngleMod( destination + (clampTolerance - 1) );
	}
}


/*
======================
UI_MovedirAdjustment
======================
*/
/*static float UI_MovedirAdjustment( playerInfo_t *pi ) {
	vec3_t		relativeAngles;
	vec3_t		moveVector;

	VectorSubtract( pi->viewAngles, pi->moveAngles, relativeAngles );
	AngleVectors( relativeAngles, moveVector, NULL, NULL );
	if ( Q_fabs( moveVector[0] ) < 0.01 ) {
		moveVector[0] = 0.0;
	}
	if ( Q_fabs( moveVector[1] ) < 0.01 ) {
		moveVector[1] = 0.0;
	}

	if ( moveVector[1] == 0 && moveVector[0] > 0 ) {
		return 0;
	}
	if ( moveVector[1] < 0 && moveVector[0] > 0 ) {
		return 22;
	}
	if ( moveVector[1] < 0 && moveVector[0] == 0 ) {
		return 45;
	}
	if ( moveVector[1] < 0 && moveVector[0] < 0 ) {
		return -22;
	}
	if ( moveVector[1] == 0 && moveVector[0] < 0 ) {
		return 0;
	}
	if ( moveVector[1] > 0 && moveVector[0] < 0 ) {
		return 22;
	}
	if ( moveVector[1] > 0 && moveVector[0] == 0 ) {
		return  -45;
	}

	return -22;
}*/


/*
===============
UI_PlayerAngles
===============
*/
static void UI_PlayerAngles( playerInfo_t *pi, vec3_t legs[3], vec3_t torso[3], vec3_t head[3] ) {
	vec3_t		legsAngles, torsoAngles, headAngles;
	float		dest;
	float		adjust;

	VectorCopy( pi->viewAngles, headAngles );
	headAngles[YAW] = AngleMod( headAngles[YAW] );
	VectorClear( legsAngles );
	VectorClear( torsoAngles );

	// --------- yaw -------------

	// allow yaw to drift a bit
	if ( ( pi->legsAnim & ~ANIM_TOGGLEBIT ) != UI_GetAnim( ANIM_IDLE, pi->currentWeapon, qfalse ) //TORSO_STAND2 
		|| ( pi->torsoAnim & ~ANIM_TOGGLEBIT ) != UI_GetAnim( ANIM_IDLE, pi->currentWeapon, qtrue ) ) {
		// if not standing still, always point all in the same direction
		pi->torso.yawing = qtrue;	// always center
		pi->torso.pitching = qtrue;	// always center
		pi->legs.yawing = qtrue;	// always center
	}

	// adjust legs for movement dir
	if ( !uis.spinView ) {
		//adjust = UI_MovedirAdjustment( pi ); //TiM: Do we really need this?
		adjust = 0;
		legsAngles[YAW] = headAngles[YAW] + adjust;
		torsoAngles[YAW] = headAngles[YAW] + /*0.25 **/ adjust;

		// torso
		UI_SwingAngles( torsoAngles[YAW], 25, 90, SWINGSPEED, &pi->torso.yawAngle, &pi->torso.yawing );
		UI_SwingAngles( legsAngles[YAW], 40, 90, SWINGSPEED, &pi->legs.yawAngle, &pi->legs.yawing );
	}
	else {
		pi->torso.yawAngle = headAngles[YAW];
		pi->legs.yawAngle = headAngles[YAW];
	}

	torsoAngles[YAW] = pi->torso.yawAngle;
	legsAngles[YAW] = pi->legs.yawAngle;

	// --------- pitch -------------

	// only show a fraction of the pitch angle in the torso
	if ( headAngles[PITCH] > 180 ) {
		dest = (-360 + headAngles[PITCH]) * 0.75;
	} else {
		dest = headAngles[PITCH] * 0.75;
	}
	UI_SwingAngles( dest, 15, 30, 0.1, &pi->torso.pitchAngle, &pi->torso.pitching );
	torsoAngles[PITCH] = pi->torso.pitchAngle;

	// pull the angles back out of the hierarchial chain
	AnglesSubtract( headAngles, torsoAngles, headAngles );
	AnglesSubtract( torsoAngles, legsAngles, torsoAngles );
	AnglesToAxis( legsAngles, legs );
	AnglesToAxis( torsoAngles, torso );
	AnglesToAxis( headAngles, head );
}


/*
===============
UI_PlayerFloatSprite
===============
*/
static void UI_PlayerFloatSprite( playerInfo_t *pi, vec3_t origin, qhandle_t shader ) {
	refEntity_t		ent;

	memset( &ent, 0, sizeof( ent ) );
	VectorCopy( origin, ent.origin );
	ent.origin[2] += 48;
	ent.reType = RT_SPRITE;
	ent.customShader = shader;
	ent.data.sprite.radius = 10;
	ent.renderfx = 0;
	trap_R_AddRefEntityToScene( &ent );
}


/*
======================
UI_MachinegunSpinAngle
======================
*/
/*float	UI_MachinegunSpinAngle( playerInfo_t *pi ) {
	int		delta;
	float	angle;
	float	speed;
	int		torsoAnim;

	delta = dp_realtime - pi->barrelTime;
	if ( pi->barrelSpinning ) {
		angle = pi->barrelAngle + delta * SPIN_SPEED;
	} else {
		if ( delta > COAST_TIME ) {
			delta = COAST_TIME;
		}

		speed = 0.5 * ( SPIN_SPEED + (float)( COAST_TIME - delta ) / COAST_TIME );
		angle = pi->barrelAngle + delta * speed;
	}

	torsoAnim = pi->torsoAnim  & ~ANIM_TOGGLEBIT;
	if( torsoAnim == TORSO_ATTACK2 ) {
		torsoAnim = TORSO_ATTACK;
	}
	if ( pi->barrelSpinning == !(torsoAnim == TORSO_ATTACK) ) {
		pi->barrelTime = dp_realtime;
		pi->barrelAngle = AngleMod( angle );
		pi->barrelSpinning = !!(torsoAnim == TORSO_ATTACK);
	}

	return angle;
}*/


/*
===============
UI_DrawPlayer
===============
*/
void UI_DrawPlayer( float x, float y, float w, float h, vec3_t pOrigin, playerInfo_t *pi, int time  ) { //RPG-X : TiM- Origin added
	refdef_t		refdef;
	refEntity_t		legs;
	refEntity_t		torso;
	refEntity_t		head;
	refEntity_t		gun;
	refEntity_t		flash;
	vec3_t			origin;
	int				renderfx;
	vec3_t			mins = {-16, -24, -24};
	vec3_t			maxs = {16, 16, 32};
	float			len;
	float			xx;
	int				anim;

	if ( !pi->legsModel || !pi->torsoModel || !pi->headModel || !pi->animations[0].numFrames ) {
		return;
	}

	dp_realtime = time;

	if ( pi->pendingWeapon != -1 && dp_realtime > pi->weaponTimer ) {
		pi->weapon = pi->pendingWeapon;
		pi->lastWeapon = pi->pendingWeapon;
		pi->pendingWeapon = -1;
		pi->weaponTimer = 0;
		/*if( pi->currentWeapon != pi->weapon ) {
			trap_S_StartLocalSound( trap_S_RegisterSound( "sound/weapons/change.wav" ), CHAN_LOCAL );
		}*/
	}

	UI_AdjustFrom640( &x, &y, &w, &h );

	y -= jumpHeight;

	memset( &refdef, 0, sizeof( refdef ) );
	memset( &legs, 0, sizeof(legs) );
	memset( &torso, 0, sizeof(torso) );
	memset( &head, 0, sizeof(head) );

	refdef.rdflags = RDF_NOWORLDMODEL;

	AxisClear( refdef.viewaxis );

	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;

	refdef.fov_x = (int)((float)refdef.width / 640.0f * 10.0f); //RPG-X : TiM- 90.0f //Anyone else noticed how the high FOV value distorted the model horribly in the menus? O_o
	xx = refdef.width / tan( refdef.fov_x / 360 * M_PI );
	refdef.fov_y = atan2( refdef.height, xx );
	refdef.fov_y *= ( 360 / M_PI );

	// calculate distance so the player nearly fills the box
	//len = 0.7f * ( maxs[2] - mins[2] );		
	//origin[0] = len / tan( DEG2RAD(refdef.fov_x) * 0.5 );
	//origin[1] = 0.5 * ( mins[1] + maxs[1] );
	//origin[2] = -0.5 * ( mins[2] + maxs[2] );

	len = 0.35f * ( maxs[2] - mins[2] );		//TiM: 0.35f
	origin[0] = (len / tan( DEG2RAD(refdef.fov_x) * 0.5 )) + pOrigin[0]; //0.5 //Z scale - Conventional 3d, not Q3 ;)
	origin[1] = 0.5 * ( mins[1] + maxs[1] ) + pOrigin[1]; //Xscale, adding numbers pushes the model to the left
	origin[2] = -0.5 * ( mins[2] + maxs[2] ) + pOrigin[2]; //yScale, adding numbers pushes up

	refdef.time = dp_realtime;

	trap_R_ClearScene();

	//spinView 
	if ( uis.spinView ) 
	{
		pi->viewAngles[YAW] = AngleNormalize360( ( uis.cursorx - uis.cursorpx) + uis.lastYaw );

		if ( !trap_Key_IsDown( K_MOUSE1 ) ) 
		{
			uis.spinView = qfalse;
			uis.lastYaw = pi->viewAngles[YAW];
		}
	}

	//TiM: random emote functionality :)
	//first init the timer so this will start a minute after loading the menu
	if ( pi->randomEmote && pi->nextEmoteTime == 0 ) {
		pi->nextEmoteTime = uis.realtime + ( irandom( 15, 20 ) * 1000 );
	}

	//whup, time to play a random emote
	if ( pi->randomEmote && uis.realtime > pi->nextEmoteTime ) {
		//randomly pick an anim
		anim = irandom( BOTH_STAND1_RANDOM2, BOTH_STAND1_RANDOM11 );

		//make sure we can play this emote
		if ( pi->animations[anim].numFrames > 0 ) {
			
			UI_ForceLegsAnim( pi, anim );
			UI_ForceTorsoAnim( pi, anim );			
			
			//play lower
			pi->legsAnimationTimer = pi->animations[ anim ].numFrames * pi->animations[ anim ].frameLerp * ((float)uis.realtime/(float)dp_realtime);
			pi->lowerEmoting = qtrue;

			pi->torsoAnimationTimer = pi->animations[ anim ].numFrames * pi->animations[ anim ].frameLerp * ((float)uis.realtime/(float)dp_realtime);
			pi->upperEmoting = qtrue;

			pi->nextEmoteTime = uis.realtime + ( irandom( 10, 20 ) * 1000 ) + pi->legsAnimationTimer; 
		}
		else {
			pi->nextEmoteTime = uis.realtime + ( irandom( 3, 8 ) * 1000 );
		}
	}

	// get the rotation information
	UI_PlayerAngles( pi, legs.axis, torso.axis, head.axis );
	
	// get the animation state (after rotation, to allow feet shuffle)
	UI_PlayerAnimation( pi, &legs.oldframe, &legs.frame, &legs.backlerp,
		 &torso.oldframe, &torso.frame, &torso.backlerp );

	renderfx = RF_LIGHTING_ORIGIN | RF_NOSHADOW;

	//
	// add the legs
	//
	legs.hModel = pi->legsModel;
	legs.customSkin = pi->legsSkin;

	VectorCopy( origin, legs.origin );

	VectorCopy( origin, legs.lightingOrigin );
	legs.renderfx = renderfx;
	VectorCopy (legs.origin, legs.oldorigin);

	VectorScale( legs.axis[0], pi->height, legs.axis[0]);
	VectorScale( legs.axis[1], (pi->height * pi->weight), legs.axis[1]); //weight... i think
	VectorScale( legs.axis[2], pi->height, legs.axis[2]);
	legs.origin[2] = legs.origin[2] - (24.0f * (1.0f - pi->height));

	trap_R_AddRefEntityToScene( &legs );

	if (!legs.hModel) {
		return;
	}

	//
	// add the torso
	//
	torso.hModel = pi->torsoModel;
	if (!torso.hModel) {
		return;
	}

	torso.customSkin = pi->torsoSkin;

	VectorCopy( origin, torso.lightingOrigin );

	UI_PositionRotatedEntityOnTag( &torso, &legs, pi->legsModel, "tag_torso");

	torso.renderfx = renderfx;

	trap_R_AddRefEntityToScene( &torso );

	//
	// add the head
	//
	head.hModel = pi->headModel;
	if (!head.hModel) {
		return;
	}
	head.customSkin = pi->headSkin;

	VectorCopy( origin, head.lightingOrigin );

	UI_PositionRotatedEntityOnTag( &head, &torso, pi->torsoModel, "tag_head");

	head.renderfx = renderfx;

	trap_R_AddRefEntityToScene( &head );

	//
	// add the gun
	//
	if ( pi->currentWeapon != WP_0 ) {
		memset( &gun, 0, sizeof(gun) );
		gun.hModel = pi->weaponModel;
		VectorCopy( origin, gun.lightingOrigin );
		UI_PositionEntityOnTag( &gun, &torso, pi->torsoModel, "tag_weapon");
		gun.renderfx = renderfx;
		trap_R_AddRefEntityToScene( &gun );
	}

	//
	// add muzzle flash
	//
	if ( dp_realtime <= pi->muzzleFlashTime ) {
		if ( pi->flashModel ) {
			memset( &flash, 0, sizeof(flash) );
			flash.hModel = pi->flashModel;
			VectorCopy( origin, flash.lightingOrigin );
			UI_PositionEntityOnTag( &flash, &gun, pi->weaponModel, "tag_flash");
			flash.renderfx = renderfx;
			trap_R_AddRefEntityToScene( &flash );
		}

		// make a dlight for the flash
		if ( pi->flashDlightColor[0] || pi->flashDlightColor[1] || pi->flashDlightColor[2] ) {
			trap_R_AddLightToScene( flash.origin, 200 + (rand()&31), pi->flashDlightColor[0],
				pi->flashDlightColor[1], pi->flashDlightColor[2] );
		}
	}

	//
	// add the chat icon
	//
	if ( pi->chat ) {
		UI_PlayerFloatSprite( pi, origin, trap_R_RegisterShaderNoMip( "sprites/chat" ) );
	}

	//
	// add an accent light
	// TiM: Holy Hell.  This explains why the models are washed out when overBrightBits is active. O_o
	// 500 is WAY too high
	//
	origin[0] -= 100;	// + = behind, - = in front
	origin[1] += 100;	// + = left, - = right
	origin[2] += 100;	// + = above, - = below
	trap_R_AddLightToScene( origin, 100, 1.0, 1.0, 1.0 ); //500

	origin[0] -= 100;
	origin[1] -= 100;
	origin[2] -= 100;
	trap_R_AddLightToScene( origin, 100, 1.0, 0.0, 0.0 );

	trap_R_RenderScene( &refdef );
}


/*
==========================
UI_RegisterClientSkin
==========================
*/
/*extern char* BG_RegisterRace( const char *name );
static qboolean UI_RegisterClientSkin( playerInfo_t *pi, const char *modelName, const char *skinName ) {
	char		filename[MAX_QPATH];

	Com_sprintf( filename, sizeof( filename ), "models/players2/%s/lower_%s.skin", modelName, skinName );
	pi->legsSkin = trap_R_RegisterSkin( filename );

	Com_sprintf( filename, sizeof( filename ), "models/players2/%s/upper_%s.skin", modelName, skinName );
	pi->torsoSkin = trap_R_RegisterSkin( filename );

	Com_sprintf( filename, sizeof( filename ), "models/players2/%s/head_%s.skin", modelName, skinName );
	pi->headSkin = trap_R_RegisterSkin( filename );

	Com_sprintf( filename, sizeof( filename ), "models/players2/%s/groups.cfg", modelName);
	strcpy(pi->race, BG_RegisterRace( filename ));

	if ( !pi->legsSkin || !pi->torsoSkin || !pi->headSkin ) {
		return qfalse;
	}

	return qtrue;
}*/


/*
======================
UI_ParseAnimationFile
======================
*/
static qboolean UI_ParseAnimationFile( const char *filename, animation_t *animations ) {
	char		*text_p, *prev;
	int			len;
	int			i;
	char		*token;
	float		fps;
	int			skip;
	char		text[20000];
	fileHandle_t	f;

	memset( animations, 0, sizeof( animation_t ) * MAX_ANIMATIONS );

	// load the file
	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( len <= 0 ) {
		return qfalse;
	}
	if ( len >= ( sizeof( text ) - 1 ) ) {
		Com_Printf( "File %s too long\n", filename );
		return qfalse;
	}
	trap_FS_Read( text, len, f );
	text[len] = 0;
	trap_FS_FCloseFile( f );

	// parse the text
	text_p = text;
	skip = 0;	// quite the compiler warning

	// read optional parameters
	while ( 1 ) {
		prev = text_p;	// so we can unget
		token = COM_Parse( &text_p );
		if ( !token[0] ) {
			break;
		}
		if ( !Q_stricmp( token, "footsteps" ) ) {
			token = COM_Parse( &text_p );
			if ( !token[0] ) {
				break;
			}
			continue;
		} else if ( !Q_stricmp( token, "headoffset" ) ) {
			for ( i = 0 ; i < 3 ; i++ ) {
				token = COM_Parse( &text_p );
				if ( !token[0] ) {
					break;
				}
			}
			continue;
		} else if ( !Q_stricmp( token, "sex" ) ) {
			token = COM_Parse( &text_p );
			if ( !token[0] ) {
				break;
			}
			continue;
		} else if ( !Q_stricmp( token, "soundpath" ) ) {
			token = COM_Parse( &text_p );
			if ( !token[0] ) {
				break;
			}
			continue;
		}

		// if it is a number, start parsing animations
		if ( token[0] >= '0' && token[0] <= '9' ) {
			text_p = prev;	// unget the token
			break;
		}

		Com_Printf( "unknown token '%s' is %s\n", token, filename );
	}

	// read information for each frame
	for ( i = 0 ; i < MAX_ANIMATIONS ; i++ ) {

		token = COM_Parse( &text_p );
		if ( !token[0] ) {
			break;
		}
		animations[i].firstFrame = atoi( token );
		// leg only frames are adjusted to not count the upper body only frames
		if ( i == LEGS_KNEEL1 ) {
			skip = animations[LEGS_KNEEL1].firstFrame - animations[TORSO_ACTIVATEMEDKIT1].firstFrame;
		}
		if ( i >= LEGS_KNEEL1) {
			animations[i].firstFrame -= skip;
		}

		token = COM_Parse( &text_p );
		if ( !token[0] ) {
			break;
		}
		animations[i].numFrames = atoi( token );

		token = COM_Parse( &text_p );
		if ( !token[0] ) {
			break;
		}
		animations[i].loopFrames = atoi( token );

		token = COM_Parse( &text_p );
		if ( !token[0] ) {
			break;
		}
		fps = atof( token );
		if ( fps == 0 ) {
			fps = 1;
		}
		animations[i].frameLerp = 1000 / fps;
		animations[i].initialLerp = 1000 / fps;
	}

	if ( i != MAX_ANIMATIONS ) {
		Com_Printf( "Error parsing animation file: %s", filename );
		return qfalse;
	}

	return qtrue;
}

/*
======================
UI_InitModelData
by TiM

Initialize default values 
in case the crazy modder 
left out some of the keys.

In most cases, the fields 
will just be left blank.
No point in using extra 
resources if they weren't 
specified.
======================
*/

static void UI_InitModelData( playerInfo_t *pi ) {
	pi->hasRanks = qfalse;

	//initialize all model + skin data as 0, so it can be told if they don't get
	//values assigned in the script parser, in which case we exit.
	pi->headModel = 0;
	pi->torsoModel = 0;
	pi->legsModel = 0;

	pi->headSkin = 0;
	pi->headSkinBlink = 0; //doesn't matter if left 0; won't end the parser
	pi->torsoSkin = 0;
	pi->legsSkin = 0;

	//doesn't matter if left 0
	pi->headBlinkTime.minSeconds = 0;
	pi->headBlinkTime.maxSeconds = 0;
	
	pi->nextTalkTime = 0;
	pi->currentTalkSkin = 0;

	pi->headSkinTalk[0] = 0;
	pi->headSkinTalk[1] = 0;
	pi->headSkinTalk[2] = 0;
	pi->headSkinTalk[3] = 0;

	memset( &pi->boltonTags, 0, sizeof(pi->boltonTags));
}

/*
=====================
UI_ParseSkinSetDataFile
by TiM

Parses a separate.skinset
file to get the skin data 
for this model.
======================
*/

static qboolean UI_ParseSkinSetDataFile( playerInfo_t *pi, const char *skinSetFrame, const char *charName, const char *skinName )
{
	char*			skinStar;
	char			skinSetName[MAX_QPATH];
	char			skinSetRoute[MAX_QPATH];
	char*			token;
	char*			textPtr;
	char			buffer[5000];
	int				len;
	fileHandle_t	f;
	int				n, i;
	int				noBlinking = trap_Cvar_VariableValue( "cg_noBlinkingHeads" );

	if ( ( skinStar = strstr( skinSetFrame, "*" ) ) == NULL )
	{
		Com_Printf( S_COLOR_RED "ERROR: No '*' specified in model skin set!\n" );
		return qfalse;
	}
	else
	{
		//star is at front
		if ( skinStar == skinSetFrame )
		{
			skinStar++;
			Com_sprintf( skinSetName, sizeof( skinSetName ), "%s%s", skinName, skinStar );
		}
		//star is at end
		else if ((int)(skinStar - skinSetFrame)+1 == (int)strlen(skinSetFrame) )
		{
			Q_strncpyz( skinSetName, skinSetFrame, strlen( skinSetFrame ) );
			Q_strcat( skinSetName, sizeof( skinSetName ), skinName );
		}
		else
		{
			Com_Printf( "ERROR: The '*' in %s must be on either the start or end, not the middle.\n", skinSetFrame );
			return qfalse;
		}
	}

	//Com_Printf( S_COLOR_RED "DEBUG: skinSetName = %s \n", skinSetName );

	Com_sprintf( skinSetRoute, sizeof( skinSetRoute ), "models/players_rpgx/%s/%s.skinset", charName, skinSetName );

	len = trap_FS_FOpenFile( skinSetRoute, &f, FS_READ );

	if ( len <= 0 )
	{
		Com_Printf( S_COLOR_RED "ERROR: Could not open file: %s\n", skinSetRoute );
		return qfalse;
	}

	if ( len > sizeof( buffer) - 1 )
	{
		Com_Printf( S_COLOR_RED "ERROR: Imported file is too big for buffer: %s. Len is %i\n", skinSetRoute, len );
		return qfalse;
	}

	trap_FS_Read( buffer, len, f );

	trap_FS_FCloseFile( f );

	if ( !buffer[0] )
	{
		Com_Printf( S_COLOR_RED "ERROR: Could not import data from %s\n", skinSetRoute );
		return qfalse;
	}

	buffer[len] = '\0';

	textPtr = buffer;

	token = COM_Parse( &textPtr );

	if ( Q_stricmp( token, "{" ) )
	{
		Com_Printf( S_COLOR_RED "ERROR: Skinset %s did not start with a '{'\n", skinSetRoute );
		return qfalse;
	}
	else
	{
		while ( 1 ) 
		{ //while we don't hit the closing brace
			
			token = COM_Parse( &textPtr ); //parse
			if ( !token[0] ) { //error check
				break;
			}

			//head skin when blinking
			//must be before headskin, or the damn thing will think the two are the same :P
			if ( !Q_stricmpn( token, "headSkinBlink", 13 ) ) {
				if ( COM_ParseString( &textPtr, &token ) ) {
					continue;
				}

				if ( !noBlinking ) {
					pi->headSkinBlink = trap_R_RegisterSkin( token );
				}

				if ( !noBlinking && !pi->headSkinBlink ) {
					//We'll alert them, but not cancel the loop
					Com_Printf( S_COLOR_RED "WARNING: Couldn't load headSkinBlink: %s\n", token);
				}
				continue;
			}

			//head blink time
			else if ( !Q_stricmpn( token, "headBlinkTime", 13 ) ) 
			{
				//Done this way so we know we got two valid args b4 proceeding
				if ( COM_ParseInt( &textPtr, &n ) ) { //first arg
					SkipRestOfLine( &textPtr );
					continue;
				}

				if ( COM_ParseInt( &textPtr, &i ) ) { //2nd arg
					SkipRestOfLine( &textPtr );
					continue;
				}				
				
				//Bug: if the stupid n00b of a modder made 
				//the minimum time larger than the max time >.<
				if ( n > i ) 
				{
					Com_Printf( S_COLOR_RED "ERROR: Minimum blink time was larger than maximum blink time.\n" );
					continue;
				}

				if ( !noBlinking ) {
					pi->headBlinkTime.minSeconds = n;
					pi->headBlinkTime.maxSeconds = i;
				}
				continue;
			}

			else if ( !Q_stricmpn( token, "torsoSkin", 9 ) ) {
				if (COM_ParseString( &textPtr, &token ) ) {
					continue;
				}

				pi->torsoSkin = trap_R_RegisterSkin( token );
				if (!pi->torsoSkin ) {
					Com_Printf( S_COLOR_RED "ERROR: Couldn't load torsoSkin: %s\n", token);
				}
				continue;
			}

			else if ( !Q_stricmpn( token, "legsSkin", 8 ) ) {
				if (COM_ParseString( &textPtr, &token ) ) {
					continue;
				}

				pi->legsSkin = trap_R_RegisterSkin( token );
				if (!pi->legsSkin ) {
					Com_Printf( S_COLOR_RED "ERROR: Couldn't load legsSkin: %s\n", token);
				}
				continue;
			}

			else if ( !Q_stricmpn( token, "headSkinTalk", 12 ) )
			{
				SkipBracedSection( &textPtr );
				continue;
			}

			//head skin
			else if ( !Q_stricmp( token, "headSkin" ) ) {
				if ( COM_ParseString( &textPtr, &token ) ) {
					continue;
				}

				pi->headSkin = trap_R_RegisterSkin( token );
				if ( !pi->headSkin ) {
					Com_Printf( S_COLOR_RED "ERROR: Couldn't load headSkin: %s\n", token );
					return qfalse;
				}
				continue;
			}

			if ( !Q_stricmpn( token, "}", 1) ) {
				break;
			}
		}
	}

	return qtrue;
}


/*
======================
UI_ParseModelDataFile
by TiM

Reads in the .model file 
needed to put together 
a character model.
======================
*/

qboolean UI_ParseModelDataFile( playerInfo_t *pi, const char *charName, 
										const char *modelName, const char *skinName ) {
	fileHandle_t	file;
	int				file_len;
	char			charText[20000];
	char			*textPtr, *prevValue;
	char			fileName[MAX_QPATH];
	//char			animPath[MAX_QPATH];
	int				i, n;
	char			*token;
	char			legsFileRoute[MAX_QPATH];
	qboolean		didAnims = qfalse;
	qboolean		skinSetFound=qfalse;
	int				noBlinking;

	noBlinking = trap_Cvar_VariableValue( "cg_noBlinkingHeads" );
	//size_t			strLen;

	//create the file route
	Com_sprintf( fileName, sizeof(fileName), "models/players_rpgx/%s/%s.model", charName, modelName);

	//Okay... gotta get the hang of ANSI C text parsing >.<
	//first... I guess load the file
	file_len = trap_FS_FOpenFile( fileName, &file, FS_READ );
	//Error handle
	//if length was 0, ie file not found or was empty
	if (file_len <= 0 ) {
		return qfalse;
	}
	//Another error... if text is WAY bigger than our available buffer O_O
	if ( file_len >= sizeof( charText ) - 1 ) {
		Com_Printf( S_COLOR_RED "Model Data File %s too long... WAY too long\n", fileName );
		return qfalse;
	}

	//initialize the buffer
	memset( charText, 0, sizeof( charText ) );

	//read data into char array
	//i guess we use a char array so we can actually specify size/width.
	trap_FS_Read( charText, file_len, file );
	//I guess this is needed to mark the EOF.
	charText[file_len] = 0;
	//Free memory. Close Files
	trap_FS_FCloseFile( file );

	//default values if needed
	UI_InitModelData( pi );
	
	//Used to just clear any previous parse temp data
	COM_BeginParseSession();

	//transfer our data from a char array to a char ptr.
	//needed for the parsing func methinks
	textPtr = charText;

	token = COM_Parse( &textPtr ); //COM_Parse seems to work by splitting up each line of text by the spaces, 
									//and then removes that chunk from the original
	//Okay, we should have the beginning variable first... which should be a '{'

	//from the looks of this, I think we have to do this after
	//every parse call. O_O
	if ( !token[0] ) {
		Com_Printf( S_COLOR_RED "No data found in model data buffer!\n");
		return qfalse;
	}

	if ( Q_stricmp(token, "{" ) ) {
		Com_Printf(S_COLOR_RED "Missing { in %s\n", fileName);
		return qfalse;
	}

	while ( 1 ) {
		prevValue = textPtr; //set a backup
		token = COM_Parse( &textPtr );

		if (!token[0] || !token ) { //we've hit the end of the file. w00t! exit!
			break;
		}
		
		//if we randomly find a brace in here (ie a sub-struct that may have no header)
		//just skip it. :P
		if ( !Q_stricmpn( token, "{", 1 ) ) {
			SkipBracedSection ( &textPtr );
		}

		if ( !Q_stricmpn( token, "animsConfig", 11 ) ) {
			if( COM_ParseString( &textPtr, &token ) ) {
				continue;
			}

			//no valid anim file found.  Don't give up hope though.
			//We have a backup resort at the end if need be. :)
			if ( ( didAnims = UI_ParseAnimationFile( token, pi->animations ) ) == qfalse ) {
				Com_Printf( S_COLOR_RED "WARNING: Was unable to load file %s.\n", token );
			}
			continue;
		}

		//playermodel gender
		else if ( !Q_stricmpn( token, "sex", 3 ) ) {
			if (COM_ParseString( &textPtr, &token ) ) {
				continue;
			}
			if ( token[0] == 'f' || token[0] == 'F' ) {
				pi->gender = GENDER_FEMALE;
			} else if ( token[0] == 'n' || token[0] == 'N' ) {
				pi->gender = GENDER_NEUTER;
			} else {
				pi->gender = GENDER_MALE;
			}
			continue;
		} 

		//character's legs model
		else if ( !Q_stricmpn( token, "legsModel", 9 ) ) {
			
			if( COM_ParseString( &textPtr, &token ) ) {
				continue;
			}

			pi->legsModel = trap_R_RegisterModel( token );
			if (!pi->legsModel) {
				Com_Printf( S_COLOR_RED "ERROR: Unable to load legs model: %s\n", token);
				return qfalse;
			}

			//if loaded no anims yet, copy the legs route to this variable,
			//and we'll try again at the end of the function
			//if ( ci->animIndex == -1 ) {
			Q_strncpyz( legsFileRoute, token, sizeof( legsFileRoute ) );
			//} Actually. just copy it regardless. Just in case

			continue;
		}

		//character's torso model
		else if ( !Q_stricmpn( token, "torsoModel", 10 ) ) {
			if( COM_ParseString( &textPtr, &token ) ) {
				continue;
			}
			pi->torsoModel = trap_R_RegisterModel( token );
			//Com_Printf("Torsomodel passed as %s, %i\n", token, (int)ci->torsoModel);

			if (!pi->torsoModel) {
				Com_Printf( S_COLOR_RED "ERROR: Unable to load torso model: %s\n", token);
				return qfalse;
			}
			continue;
		}

		//character's headmodel
		else if ( !Q_stricmpn( token, "headModel", 9 ) ) {

			//return true = no extra text found on this line - bad! O_O!
			if( COM_ParseString( &textPtr, &token ) ) {
				continue;
			}

			pi->headModel = trap_R_RegisterModel( token );
			if (!pi->headModel) {
				Com_Printf( S_COLOR_RED "ERROR: Unable to load head model: %s\n", token);
				return qfalse;
			}
			continue;
		}

		// Custom bolton models... oi O_o
		else if ( !Q_stricmpn( token, "boltonModels", 12 ) ) {
			//needed coz '{' could also be on next line
			token = COM_Parse( &textPtr );
			if ( !token[0] ) { //if that was it
				break;
			} else { //else, if next character is '{'
				if ( !Q_stricmpn( token, "{", 1 ) ) {
					token = COM_Parse( &textPtr );
					if ( !token[0] ) { 
						break; 
					}	
					//loop till we hit the end of the brackets
					i = 0;

					while ( Q_stricmp( token, "}" ) ) {
						if ( !Q_stricmpn( token, "BOLTON_", 7 ) ) {
							
							pi->boltonTags[i].modelBase = GetIDForString( BoltonTable, token );

							if( COM_ParseString( &textPtr, &token ) ) {
								continue;
							}

							if (!Q_stricmpn( token, "tag_", 4 ) ) {
								Q_strncpyz(pi->boltonTags[i].tagName, token, sizeof (pi->boltonTags[i].tagName) );
							
								if( COM_ParseString( &textPtr, &token ) ) {
									continue;
								}
								pi->boltonTags[i].tagModel = trap_R_RegisterModel( token );
								
								if (!pi->boltonTags[i].tagModel) {
									Com_Printf( S_COLOR_RED "WARNING: Unable to load bolton model: %s\n", token);
								}

								i++;

								if (i > MAX_BOLTONS -1) {
									break;
								}
							}
						}

						//Com_Printf("Index: %i, Name: %s, Handle: %i\n", ci->boltonTags[ci->numBoltOns].modelBase, ci->boltonTags[ci->numBoltOns].tagName, ci->boltonTags[ci->numBoltOns].tagModel  );
						token = COM_Parse( &textPtr );
						if ( !token[0] ) { 
							break; 
						}	
					}
				}
			}
		}

		//whether char is allowed to wear ranks
		else if ( !Q_stricmpn( token, "hasRanks", 8 ) ) {
			if (COM_ParseInt(&textPtr, &n ) ) {
				continue;
			}
			pi->hasRanks = n;
			continue;
		}

		//TiM - The skinset is defined
		else if ( !Q_stricmpn( token, "skinSet", 7 ) ) {
			if ( COM_ParseString( &textPtr, &token ) ) {
				continue;
			}
			
			if ( !UI_ParseSkinSetDataFile( pi, token, charName, skinName ) )
			{
				Com_Printf( S_COLOR_RED "WARNING: Could not load data from specified skin set in char: %s. Attempting to load default.\n", charName );
			}
			else
			{
				skinSetFound = qtrue;
			}
			
			continue;
		}
	}

	if ( !skinSetFound )
	{
		if ( !UI_ParseSkinSetDataFile( pi, va("%s_*", modelName, skinName ), charName, skinName ) )
		{
			Com_Printf( S_COLOR_RED "ERROR: Tried loading default skin set, however it failed.\n");
		}	
	}

	//if any of the models or skins were left blank, then output false. Coz we need them. :P
	if (!pi->headModel || !pi->torsoModel || !pi->legsModel ) {
		Com_Printf( S_COLOR_RED "One or more necessary model files weren't loaded from %s\n", fileName );
		return qfalse;
	}

	if (!pi->headSkin || !pi->torsoSkin || !pi->legsSkin ) {
		Com_Printf( S_COLOR_RED "One or more necessary skin files weren't loaded from %s\n", fileName );
		return qfalse;
	}

	//if modder specified no animations file route, or they did, and it sucked (ie -1 ),
	//Then try looking for one in the same directory as the lower.mdr file

	//k... the goal of this is to take a string like
	//models/players_rpgx/crewman_male/lower.mdr
	//and turn it into
	//models/players_rpgx/crewman_male/animation.cfg

	if ( !didAnims && strlen( legsFileRoute ) > 0 ) {
		//get length of file route
		i = strlen(legsFileRoute);

		while( 1 ) {
			//if we looped all the way to the end.... ie BAD
			if (i <= 0) {
				//we obviously have no animation directory :(
				Com_Printf(S_COLOR_RED "ERROR: Was unable to calculate location of animation.cfg for %s\n", fileName);
				return qfalse;
			}

			//if this is the first '/' we come across from going from the end to the start
			if (legsFileRoute[i] == '/' ) {
				//copy i bytes of data from token to animpath (effectively giving us the route, with no file)
				Q_strncpyz(legsFileRoute, legsFileRoute, (i = i + 2 )); //+2 for the null char these things auto assign at the end... i think
				break;										//won't work without it anyway :P
			}
			i--;
		}

		//add animation.cfg to the end of the string
		Q_strcat(legsFileRoute, sizeof(legsFileRoute), "animation.cfg");

		//Com_Printf( S_COLOR_RED "WARNING: Failed to load animation file specified in model config, attempting to load %s\n", legsFileRoute );

		//Parse it
		if ( !UI_ParseAnimationFile( legsFileRoute, pi->animations) ) {
			Com_Printf( "Tried loading anim data from location %s, however nothing was valid.\n", legsFileRoute );
			return qfalse;
		}
	}
	else {
		if ( !legsFileRoute[0] ) {
			Com_Printf( S_COLOR_RED "Couldn't load/locate any player animation data for player: %s.\n", charName );
			return qfalse;
		}
	}

	//holy fudgenuggets.  after all that checking, we actually made it to the end and have a valid freaking
	//model! OWNED!
	return qtrue;
}


/*
==========================
UI_RegisterClientModelname
==========================
*/
qboolean UI_RegisterClientModelname( playerInfo_t *pi, const char *modelSkinName ) {
	char		charName[MAX_QPATH];
	char		modelName[MAX_QPATH];
	char		skinName[MAX_QPATH];
	//char		filename[MAX_QPATH];
	char		*model, *skin;
	//char		*slash;
	int			len;

	pi->torsoModel = 0;
	pi->headModel = 0;

	if ( !modelSkinName[0] ) {
		return qfalse;
	}

	Q_strncpyz( charName, modelSkinName, sizeof( charName ) );

	/*slash = strchr( modelName, '/' );
	if ( !slash ) {
		// modelName did not include a skin name
		Q_strncpyz( skinName, "default", sizeof( skinName ) );
	} else {
		Q_strncpyz( skinName, slash + 1, sizeof( skinName ) );
		// truncate modelName
		*slash = 0;
	}*/

	//step 1, take the first bit of the string and put it in the charName var.
	if ( ( model = strchr( charName, '/') ) == NULL ) { //if there's no slash
		Q_strncpyz( charName, modelSkinName, sizeof( charName ) ); //just set it
	} else { //otherwise, isolate the first bit, and copy that
		len = strlen( modelSkinName );
		Q_strncpyz( charName, modelSkinName, ((int)len - (int)strlen(model)) + 1 );
	}
	//Com_Printf("%s\n", newInfo.charName);

	//slash = strchr( newInfo.modelName, '/' );
	if ( !model || !model[1] ) {
		// modelName didn not include a skin name
		//Q_strncpyz( newInfo.skinName, "default", sizeof( newInfo.skinName ) );
		Q_strncpyz( modelName, "main", sizeof( modelName ) );
		Q_strncpyz( skinName, "default", sizeof( skinName ) );

		if ( model && !model[1] )
		{//if we had a slash, but nothing after, clear it
			*model = 0;
		}
	} else {
		//*model++; //bypass the slash
		model++;
		len = strlen(model);
		skin = strchr( model, '/' );

		//if there was a model defined, but no skin
		if ( !skin || !skin[1] ) {
			//no skin, but I'm guessing we gotz a model at least
			if ( !skin ) {
				Q_strncpyz( modelName, model, sizeof( modelName ) );
			}
			else {
				if ( !skin[1] ) {
					Q_strncpyz( modelName, model, (int)strlen(model) );
				}
			}

			Q_strncpyz( skinName, "default", sizeof( skinName ) );

			if ( skin && !skin[1] ) {
				*skin = 0;
			}
		} else {
			//*skin++;
			skin++;
			Q_strncpyz( modelName, model, ((int)len - (int)strlen(skin)) );
			Q_strncpyz( skinName, skin, sizeof( skinName ) );
		}

		//Q_strncpyz( newInfo.skinName, slash + 1, sizeof( newInfo.skinName ) );
		// truncate modelName
		*model = 0;
	}

	// load cmodels before models so filecache works
	//try loading the main model
	if ( !UI_ParseModelDataFile( pi, charName, modelName, skinName ) ) 
	{
		Com_Printf( S_COLOR_RED "Was unable to parse model file for character: %s/%s/%s\n", charName, modelName, skinName );
		//if that fails, try and load the model's default data at least
		if ( !UI_ParseModelDataFile( pi, charName, DEFAULT_MODEL, DEFAULT_SKIN ) ) 
		{
			//if THAT fails, try loading our defualt char, with the specfied char's model and skin parms
			if ( !UI_ParseModelDataFile( pi, ui_defaultChar.string, modelName, skinName ) )
			{
				if ( !UI_ParseModelDataFile( pi, DEFAULT_CHAR, DEFAULT_MODEL, DEFAULT_SKIN ) )
				{
					//if all else fails, try and load the normal default model
					return qfalse;
				}
			}
		}
	}

	/*Com_sprintf( filename, sizeof( filename ), "models/players2/%s/lower.mdr", modelName );
	pi->legsModel = trap_R_RegisterModel( filename );
	if ( !pi->legsModel )
	{
		Com_sprintf( filename, sizeof( filename ), "models/players2/%s/lower.md3", modelName );
		pi->legsModel = trap_R_RegisterModel( filename );
		if ( !pi->legsModel )
		{
			Com_Printf( S_COLOR_RED"Failed to load model file %s\n", filename );
			return qfalse;
		}
	}

	Com_sprintf( filename, sizeof( filename ), "models/players2/%s/upper.mdr", modelName );
	pi->torsoModel = trap_R_RegisterModel( filename );
	if ( !pi->torsoModel )
	{
		Com_sprintf( filename, sizeof( filename ), "models/players2/%s/upper.md3", modelName );
		pi->torsoModel = trap_R_RegisterModel( filename );
		if ( !pi->torsoModel ) {
			Com_Printf( S_COLOR_RED"Failed to load model file %s\n", filename );
			return qfalse;
		}
	}

	Com_sprintf( filename, sizeof( filename ), "models/players2/%s/head.md3", modelName );
	pi->headModel = trap_R_RegisterModel( filename );
	if ( !pi->headModel )
	{
		Com_Printf( S_COLOR_RED"Failed to load model file %s\n", filename );
		return qfalse;
	}

	// if any skins failed to load, fall back to default
	if ( !UI_RegisterClientSkin( pi, modelName, skinName ) ) {
		if ( !UI_RegisterClientSkin( pi, modelName, "default" ) ) {
			Com_Printf( "Failed to load skin file: %s : %s\n", modelName, skinName );
			return qfalse;
		}
	}

	// load the animations
	Com_sprintf( filename, sizeof( filename ), "models/players2/%s/animation.cfg", modelName );
	if ( !UI_ParseAnimationFile( filename, pi->animations ) ) {
		Com_Printf( "Failed to load animation file %s\n", filename );
		return qfalse;
	}*/

	return qtrue;
}


/*
===============
UI_PlayerInfo_SetModel
===============
*/
void UI_PlayerInfo_SetModel( playerInfo_t *pi, const char *model ) {
	memset( pi, 0, sizeof(*pi) );
	UI_RegisterClientModelname( pi, model );
	Q_strncpyz( pi->modelName, model, sizeof( pi->modelName ) );
	pi->weapon = WP_0;
	pi->currentWeapon = pi->weapon;
	pi->lastWeapon = pi->weapon;
	pi->pendingWeapon = -1;
	pi->weaponTimer = 0;
	pi->chat = qfalse;
	pi->newModel = qtrue;
	UI_PlayerInfo_SetWeapon( pi, pi->weapon );
}


/*
===============
UI_PlayerInfo_SetInfo
===============
*/
void UI_PlayerInfo_SetInfo( playerInfo_t *pi, int legsAnim, int torsoAnim, vec3_t viewAngles, vec3_t moveAngles, weapon_t weaponNumber, float height, float weight, qboolean chat ) {
	int			currentAnim;
	weapon_t	weaponNum;

	pi->chat = chat;

	// view angles
	VectorCopy( viewAngles, pi->viewAngles );

	// move angles
	VectorCopy( moveAngles, pi->moveAngles );

	//TiM : Clamp weight and height
	pi->height = height;
	pi->weight = weight;

	if ( !pi->weight )
		pi->weight = 1.0f;
	if ( !pi->height )
		pi->height = 1.0f;

	pi->height = Com_Clamp( 0.9f, 1.15f, pi->height );
	pi->weight = Com_Clamp( 0.9f, 1.1f, pi->weight );

	if ( pi->newModel ) {
		pi->newModel = qfalse;

		jumpHeight = 0;
		pi->pendingLegsAnim = 0;
		UI_ForceLegsAnim( pi, legsAnim );
		pi->legs.yawAngle = viewAngles[YAW];
		pi->legs.yawing = qfalse;

		pi->pendingTorsoAnim = 0;
		UI_ForceTorsoAnim( pi, torsoAnim );
		pi->torso.yawAngle = viewAngles[YAW];
		pi->torso.yawing = qfalse;

		if ( weaponNumber != -1 ) {
			pi->weapon = weaponNumber;
			pi->currentWeapon = weaponNumber;
			pi->lastWeapon = weaponNumber;
			pi->pendingWeapon = -1;
			pi->weaponTimer = 0;
			UI_PlayerInfo_SetWeapon( pi, pi->weapon );
		}

		return;
	}

	// weapon
	if ( weaponNumber == -1 ) {
		pi->pendingWeapon = -1;
		pi->weaponTimer = 0;
	}
	else if ( weaponNumber != WP_0 ) {
		pi->pendingWeapon = weaponNumber;
		pi->weaponTimer = dp_realtime + UI_TIMER_WEAPON_DELAY;
	}
	weaponNum = pi->lastWeapon;
	pi->weapon = weaponNum;

	if ( torsoAnim == BOTH_DEATH1 || legsAnim == BOTH_DEATH1 ) {
		torsoAnim = legsAnim = BOTH_DEATH1;
		pi->weapon = pi->currentWeapon = WP_0;
		UI_PlayerInfo_SetWeapon( pi, pi->weapon );

		jumpHeight = 0;
		pi->pendingLegsAnim = 0;
		UI_ForceLegsAnim( pi, legsAnim );

		pi->pendingTorsoAnim = 0;
		UI_ForceTorsoAnim( pi, torsoAnim );

		return;
	}

	// leg animation
	currentAnim = pi->legsAnim & ~ANIM_TOGGLEBIT;
	if ( legsAnim != BOTH_JUMP1 && ( currentAnim == BOTH_JUMP1 || currentAnim == BOTH_LAND1 ) ) {
		pi->pendingLegsAnim = legsAnim;
	}
	else if ( legsAnim != currentAnim ) {
		jumpHeight = 0;
		pi->pendingLegsAnim = 0;
		UI_ForceLegsAnim( pi, legsAnim );
	}

	// torso animation
	//if ( torsoAnim == TORSO_STAND || torsoAnim == TORSO_STAND2) //TiM: TORSO_STAND2
	if ( torsoAnim == UI_GetAnim( ANIM_IDLE, pi->currentWeapon, qtrue ) )
	{	
		/*if ( weaponNum == WP_0 || weaponNum == WP_5 ) 
		{
			torsoAnim = TORSO_STAND2;
		}
		else 
		{
			torsoAnim = TORSO_STAND2;
		}*/

		/*if ( weaponNum == WP_6 || weaponNum == WP_7 ) 
		{
			torsoAnim = TORSO_STAND;
		}
		else 
		{
			torsoAnim = TORSO_STAND2;
		}*/
		torsoAnim = UI_GetAnim( ANIM_IDLE, pi->currentWeapon, qtrue );

	}

	//if ( torsoAnim == TORSO_ATTACK || torsoAnim == TORSO_ATTACK2 )
	if ( torsoAnim == UI_GetAnim( ANIM_ATTACK, pi->currentWeapon, qtrue ) )
	{
		/*if ( weaponNum == WP_0 || weaponNum == WP_5 ) 
		{
			torsoAnim = TORSO_ATTACK2;
		}
		else 
		{
			torsoAnim = TORSO_ATTACK;
		}*/

		/*if ( weaponNum == WP_6 || weaponNum == WP_7 ) 
		{
			torsoAnim = TORSO_ATTACK;
		}
		else 
		{
			torsoAnim = TORSO_ATTACK2;
		}*/

		torsoAnim = UI_GetAnim( ANIM_ATTACK, pi->currentWeapon, qtrue );

		pi->muzzleFlashTime = dp_realtime + UI_TIMER_MUZZLE_FLASH;
		//FIXME play firing sound here
	}

	currentAnim = pi->torsoAnim & ~ANIM_TOGGLEBIT;

	if ( weaponNum != pi->currentWeapon || currentAnim == TORSO_RAISEWEAP1 || currentAnim == TORSO_DROPWEAP1 ) {
		pi->pendingTorsoAnim = torsoAnim;
	}
	else if ( ( /*currentAnim == TORSO_GESTURE ||*/ currentAnim == UI_GetAnim(ANIM_ATTACK, pi->currentWeapon, qtrue) ) && ( torsoAnim != currentAnim ) ) {
		pi->pendingTorsoAnim = torsoAnim;
	}
	else if ( torsoAnim != currentAnim ) {
		pi->pendingTorsoAnim = 0;
		UI_ForceTorsoAnim( pi, torsoAnim );
	}
}
