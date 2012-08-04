// Copyright (C) 1999-2000 Id Software, Inc.
//
// cg_weapons.c -- events and effects dealing with weapons
#include "cg_local.h"
#include "fx_local.h"

//RPG-X : TiM - Weapons Arrays
static int RAweapons[8] = { WP_3, 
							WP_2,
							WP_6,
							WP_7,
							WP_12,
							WP_13,
							WP_11,
							WP_4 
							};

static char *RAweapFileName[8] = {  "padd",
									"tricorder",
									"prifle",
									"tr116",
									"hypospray",
									"dermal_regen",
									"medkit",
									"coffeecup" 
									};
							

/*
=================
CG_RegisterWeapon

The server says this item is used on this level
=================
*/

// kef -- sad? yep.
typedef struct wpnBarrelInfo_s
{
	weapon_t			giTag;
	int					numBarrels;
	int					flashTime;
} wpnBarrelInfo_t;

wpnBarrelInfo_t wpnBarrelData[] =
{
	{WP_1,			0, 0},

	{WP_2,			0, 0},
	{WP_3,				0, 0},
	{WP_4,				0, 0},

	{WP_5,				0, 0},
	{WP_6,	0, 120},
	{WP_7,				1, 60},

	{WP_8,	2, 150},
	{WP_9,		1, 200},
	{WP_10,			1, 130},

	{WP_11,				0, 0},
	{WP_12,		0, 0},
	{WP_13,		0, 0},
	
	{WP_14,			0, 0},
	{WP_15,			0, 0},

	// make sure this is the last entry in this array, please
	{WP_0,				0, 0},
};

//wpnBarrelInfo_t wpnBarrelData[] =
//{
//	{WP_5,				0, 0},
//	{WP_6,	0, 100},
//	{WP_1,				0, 0},
//	{WP_4,	0, 0},
//	{WP_10,				1, 80},
//	{WP_8,	2, 140},
//	{WP_7,	1, 120},
//	{WP_9,		1, 200},
//	{WP_13,		0, 0},
//	{WP_12,		0, 0},
//	{WP_14,	0, 0},
//	{WP_11,		0, 0},
//	{WP_2,			0, 0},
//	{WP_3,				0, 0},
//	{WP_NEUTRINO_PROBE,			0, 0},
//	{WP_7,				0, 90},
//
//	// make sure this is the last entry in this array, please
//	{WP_0,				0},
//};

void CG_RegisterWeapon( int weaponNum ) {
	weaponInfo_t	*weaponInfo;
	gitem_t			*item, *ammo;
	char			path[MAX_QPATH];
	vec3_t			mins, maxs;
	int				i;
	int				numBarrels = 0;
	wpnBarrelInfo_t	*barrelInfo = NULL;


	weaponInfo = &cg_weapons[weaponNum];

	if ( weaponNum == 0 ) {
		return;
	}

	if ( weaponInfo->registered ) {
		return;
	}

	memset( weaponInfo, 0, sizeof( *weaponInfo ) );
	weaponInfo->registered = qtrue;

	for ( item = bg_itemlist + 1 ; item->classname ; item++ ) {
		if ( item->giType == IT_WEAPON && item->giTag == weaponNum ) {
			/*if ( weaponNum == WP_10 ) {
				Com_Printf( S_COLOR_RED "Registering %s with pickup name of %s\n", bg_itemlist[10].classname, bg_itemlist[10].pickup_name );
			}*/
			weaponInfo->item = item;
			break;
		}
	}
	if ( !item->classname ) {
		CG_Error( "Couldn't find weapon %i", weaponNum );
	}
	CG_RegisterItemVisuals( item - bg_itemlist );

	weaponInfo->weaponModel = trap_R_RegisterModel( item->world_model );

	// kef -- load in-view model
	weaponInfo->viewModel = trap_R_RegisterModel(item->view_model);

	// calc midpoint for rotation
	trap_R_ModelBounds( weaponInfo->weaponModel, mins, maxs );
	for ( i = 0 ; i < 3 ; i++ ) {
		weaponInfo->weaponMidpoint[i] = mins[i] + 0.5 * ( maxs[i] - mins[i] );
	}

	weaponInfo->weaponIcon = trap_R_RegisterShader( item->icon );

	for ( ammo = bg_itemlist + 1 ; ammo->classname ; ammo++ ) {
		if ( ammo->giType == IT_AMMO && ammo->giTag == weaponNum ) {
			break;
		}
	}
//	if ( ammo->classname && ammo->world_model ) {
//		weaponInfo->ammoModel = trap_R_RegisterModel( ammo->world_model );
//	}

	strcpy( path, item->view_model );
	COM_StripExtension( path, path );
	strcat( path, "_flash.md3" );
	weaponInfo->flashModel = trap_R_RegisterModel( path );

	for (barrelInfo = wpnBarrelData; barrelInfo->giTag != WP_0; barrelInfo++)
	{
		if (barrelInfo->giTag == weaponNum)
		{
			numBarrels = barrelInfo->numBarrels;
			break;
		}
	}
	for (i=0; i< numBarrels; i++) {
		Q_strncpyz( path, item->view_model, MAX_QPATH );
		COM_StripExtension( path, path );
		if (i)
		{
			strcat( path, va("_barrel%d.md3", i+1));
		}
		else
			strcat( path, "_barrel.md3" );
		weaponInfo->barrelModel[i] = trap_R_RegisterModel( path );
	}

	strcpy( path, item->view_model );
	COM_StripExtension( path, path );
	strcat( path, "_hand.md3" );
	weaponInfo->handsModel = trap_R_RegisterModel( path );

	if ( !weaponInfo->handsModel ) {
		weaponInfo->handsModel = trap_R_RegisterModel( "models/weapons2/prifle/prifle_hand.md3" );
	}

	switch ( weaponNum ) {
	case WP_5:
		MAKERGB( weaponInfo->flashDlightColor, 0, 0, 0 );

		weaponInfo->firingSound = trap_S_RegisterSound( SOUND_DIR "phaser/phaserfiring.wav" );
		weaponInfo->altFiringSound = trap_S_RegisterSound( SOUND_DIR "phaser/altphaserfiring.wav" );
		weaponInfo->flashSound = trap_S_RegisterSound( SOUND_DIR "phaser/phaserstart.wav" );
		weaponInfo->altFlashSnd = trap_S_RegisterSound( SOUND_DIR "phaser/altphaserstart.wav" );
		weaponInfo->stopSound = trap_S_RegisterSound(SOUND_DIR "phaser/phaserstop.wav");
		weaponInfo->altStopSound = trap_S_RegisterSound(SOUND_DIR "phaser/altphaserstop.wav");

		cgs.media.phaserShader			= trap_R_RegisterShader( "gfx/misc/phaser_stx" );
		cgs.media.phaserEmptyShader		= trap_R_RegisterShader( "gfx/misc/phaserempty" );

		cgs.media.phaserAltShader		= trap_R_RegisterShader("gfx/effects/whitelaser");	// "gfx/misc/phaser_alt" ); 

		cgs.media.phaserAltEmptyShader	= trap_R_RegisterShader( "gfx/misc/phaser_altempty" ); 
		cgs.media.phaserMuzzleEmptyShader= trap_R_RegisterShader( "models/weapons2/phaser/muzzle_empty" );

		break;

	case WP_13:
		weaponInfo->firingSound = trap_S_RegisterSound( SOUND_DIR "dermal_regen/dm_1.wav" );
		weaponInfo->altFiringSound = trap_S_RegisterSound( SOUND_DIR "dermal_regen/dm_2.wav" );
		break;

	case WP_10:
		//weaponInfo->missileTrailFunc = FX_StasisProjectileThink;
		weaponInfo->missileModel = trap_R_RegisterModel( "models/weapons2/alien_disruptor/disruptor_bolt.md3" );
		weaponInfo->missileDlight = 70;
		MAKERGB( weaponInfo->missileDlightColor, 0.0, 1.0, 0.0 );
		MAKERGB( weaponInfo->flashDlightColor, 0.0, 1.0, 0.0 );

		weaponInfo->altFlashSnd = trap_S_RegisterSound( SOUND_DIR "alien_disruptor/fire.wav" );
		weaponInfo->mainHitSound = trap_S_RegisterSound(SOUND_DIR "stasis/hit_wall.wav");
	
		weaponInfo->flashSound = trap_S_RegisterSound( SOUND_DIR "alien_disruptor/disruptorstart.wav" );
		weaponInfo->stopSound = trap_S_RegisterSound(SOUND_DIR "alien_disruptor/disruptorstop.wav");
		weaponInfo->firingSound = trap_S_RegisterSound( SOUND_DIR "alien_disruptor/disruptorfiring.wav" );

		cgs.media.disruptorBolt				= trap_R_RegisterShader( "gfx/misc/disruptor_bolt" );
		cgs.media.disruptorStreak			= trap_R_RegisterShader( "gfx/misc/disruptor_streak" );
		//cgs.media.altIMOD2Shader			= trap_R_RegisterShader( "gfx/misc/IMOD2alt" );
		//cgs.media.dnBoltShader				= trap_R_RegisterShader( "gfx/misc/dnBolt" );

		cgs.media.greenParticleShader		= trap_R_RegisterShader( "gfx/misc/greenparticle" );
		cgs.media.greenParticleStreakShader	= trap_R_RegisterShader( "gfx/misc/greenparticle_anamorphic" );

		cgs.media.disruptorBeam				= trap_R_RegisterShader( "gfx/misc/disruptor" );

		break;

	case WP_8:
		weaponInfo->missileModel = trap_R_RegisterModel( "models/weapons2/launcher/projectile.md3" );
		if(rpg_ctribgrenade.integer == 1)//RPG-X: - RedTechie Possible Hack! FIX | TiM: Heh, you're a possible hack :)
		{
			weaponInfo->alt_missileModel = trap_R_RegisterModel( "models/weapons2/launcher/projectile2a.md3" );
			weaponInfo->altFlashSnd = trap_S_RegisterSound( SOUND_DIR "glauncher/alt_fire.wav" );
			weaponInfo->altHitSound = trap_S_RegisterSound( "sound/weapons/glauncher/beepa.wav" );
			cgs.media.grenadeAltStickSound = trap_S_RegisterSound(SOUND_DIR "glauncher/alt_stick.wav");
			
		}
		else
		{
			weaponInfo->alt_missileModel = trap_R_RegisterModel( "models/weapons2/launcher/projectile2.md3" );
			weaponInfo->altFlashSnd = trap_S_RegisterSound( SOUND_DIR "glauncher/alt_fire.wav" );
			weaponInfo->altHitSound = trap_S_RegisterSound( SOUND_DIR "glauncher/beep.wav" );
			cgs.media.grenadeAltStickSound = trap_S_RegisterSound(SOUND_DIR "glauncher/alt_stick.wav");
		}

		weaponInfo->missileTrailFunc = FX_GrenadeThink;
		//TiM : No flash anymore
		MAKERGB( weaponInfo->flashDlightColor, 0.0, 0.0, 0.0 );
		//MAKERGB( weaponInfo->flashDlightColor, 0.6, 0.6, 1 );

		weaponInfo->flashSound = trap_S_RegisterSound( SOUND_DIR "glauncher/fire.wav" );
		cgs.media.grenadeBounceSound1 = trap_S_RegisterSound(SOUND_DIR "glauncher/bounce1.wav");
		cgs.media.grenadeBounceSound2 = trap_S_RegisterSound(SOUND_DIR "glauncher/bounce2.wav");
		cgs.media.grenadeExplodeSound = trap_S_RegisterSound(SOUND_DIR "glauncher/explode.wav");
		cgs.media.grenadeAltExplodeSnd	= trap_S_RegisterSound(SOUND_DIR "glauncher/alt_explode.wav" );
		
		cgs.media.orangeTrailShader			= trap_R_RegisterShader( "gfx/misc/orangetrail" );
		cgs.media.compressionMarkShader		= trap_R_RegisterShader( "gfx/damage/burnmark1" );
		cgs.media.whiteLaserShader		= trap_R_RegisterShader( "gfx/effects/whitelaser" );
		cgs.media.borgEyeFlareShader	= trap_R_RegisterShader( "gfx/misc/borgeyeflare" );
		break;

	case WP_4:
		//MAKERGB( weaponInfo->flashDlightColor, 1, 0.6, 0.6 );

		/*weaponInfo->flashSound = trap_S_RegisterSound( SOUND_DIR "scavenger/fire.wav" );
		weaponInfo->altFlashSnd = trap_S_RegisterSound( SOUND_DIR "scavenger/alt_fire.wav" );
		weaponInfo->mainHitSound = trap_S_RegisterSound(SOUND_DIR "scavenger/hit_wall.wav");
		weaponInfo->altHitSound = trap_S_RegisterSound(SOUND_DIR "scavenger/alt_explode.wav");
		weaponInfo->missileTrailFunc = FX_ScavengerProjectileThink;
		weaponInfo->alt_missileTrailFunc = FX_ScavengerAltFireThink;
//		weaponInfo->wiTrailTime = 100;
//		weaponInfo->trailRadius = 8;
		cgs.media.tetrionFlareShader		= trap_R_RegisterShader( "gfx/misc/tet1" );
		cgs.media.tetrionTrail2Shader		= trap_R_RegisterShader( "gfx/misc/trail2" );
		cgs.media.redFlareShader			= trap_R_RegisterShader( "gfx/misc/red_flare" );

		cgs.media.scavengerAltShader		= trap_R_RegisterShader( "gfx/misc/scavaltfire" );
		cgs.media.scavExplosionFastShader	= trap_R_RegisterShader( "scavExplosionFast" );
		cgs.media.scavExplosionSlowShader	= trap_R_RegisterShader( "scavExplosionSlow" );
		cgs.media.compressionMarkShader		= trap_R_RegisterShader( "gfx/damage/burnmark1" );*/
		break;

	case WP_9:
		MAKERGB( weaponInfo->flashDlightColor, 0.6, 0.6, 1 );	//Bluish

		weaponInfo->missileTrailFunc = FX_QuantumThink;
		weaponInfo->alt_missileTrailFunc = FX_QuantumAltThink;
		
		weaponInfo->missileDlight = 75;
		weaponInfo->alt_missileDlight = 100;
		MAKERGB( weaponInfo->missileDlightColor, 1.0, 1.0, 0.5);	//yellowish

		weaponInfo->flashSound = trap_S_RegisterSound( SOUND_DIR "quantum/fire.wav" );
		weaponInfo->altFlashSnd = trap_S_RegisterSound( SOUND_DIR "quantum/alt_fire.wav" );

		weaponInfo->mainHitSound = trap_S_RegisterSound( SOUND_DIR "quantum/hit_wall.wav" );;		
		weaponInfo->altHitSound = trap_S_RegisterSound( SOUND_DIR "quantum/alt_hit_wall.wav" );;		

		cgs.media.whiteRingShader			= trap_R_RegisterShader( "gfx/misc/whitering" );
		cgs.media.orangeRingShader			= trap_R_RegisterShader( "gfx/misc/orangering" );
		cgs.media.quantumExplosionShader	= trap_R_RegisterShader( "quantumExplosion" );
		cgs.media.quantumFlashShader		= trap_R_RegisterShader( "yellowflash" );
		//cgs.media.bigBoomShader				= trap_R_RegisterShader( "gfx/misc/bigboom" );
		cgs.media.orangeTrailShader			= trap_R_RegisterShader( "gfx/misc/orangetrail" );
		cgs.media.compressionMarkShader		= trap_R_RegisterShader( "gfx/damage/burnmark1" );
		cgs.media.orangeTrailShader			= trap_R_RegisterShader( "gfx/misc/orangetrail" );
		cgs.media.quantumRingShader	    	= trap_R_RegisterShader( "gfx/misc/detpack3" );
		cgs.media.quantumBoom		    	= trap_S_RegisterSound ( SOUND_DIR "explosions/explode5.wav" );
		break;

	case WP_1:
		/*MAKERGB( weaponInfo->flashDlightColor, 0.6, 0.6, 1 );

		weaponInfo->flashSound = trap_S_RegisterSound( SOUND_DIR "IMOD/fire.wav" );
		weaponInfo->altFlashSnd = trap_S_RegisterSound( SOUND_DIR "IMOD/alt_fire.wav" );

		cgs.media.IMODShader			= trap_R_RegisterShader( "gfx/misc/IMOD" );
		cgs.media.IMOD2Shader			= trap_R_RegisterShader( "gfx/misc/IMOD2" );
		cgs.media.altIMODShader			= trap_R_RegisterShader( "gfx/misc/IMODalt" );
		cgs.media.altIMOD2Shader		= trap_R_RegisterShader( "gfx/misc/IMOD2alt" );
		cgs.media.imodExplosionShader	= trap_R_RegisterShader( "imodExplosion" );*/
		break;

	case WP_6:
		if(!grp_berp.integer) {
			MAKERGB( weaponInfo->flashDlightColor, 0.59, 0.24, 0.25 );
			MAKERGB( weaponInfo->missileDlightColor, 0.59, 0.24, 0.25 );
		} else {
			MAKERGB( weaponInfo->flashDlightColor, 0.16, 0.32, 0.5 );
			MAKERGB( weaponInfo->missileDlightColor, 0.16, 0.32, 0.5 );
		}

		weaponInfo->missileModel = trap_R_RegisterModel( "models/weapons2/prifle/prifle_bolt.md3" );
		weaponInfo->missileDlight = 90;
		
		weaponInfo->flashSound = trap_S_RegisterSound( SOUND_DIR "prifle/fire.wav" );

		weaponInfo->altFlashSnd = trap_S_RegisterSound( SOUND_DIR "prifle/phaserriflestart.wav" );
		weaponInfo->altStopSound = trap_S_RegisterSound(SOUND_DIR "prifle/phaserriflestop.wav");
		weaponInfo->altFiringSound = trap_S_RegisterSound( SOUND_DIR "prifle/phaserriflefiring.wav" );

		weaponInfo->mainHitSound = trap_S_RegisterSound( SOUND_DIR "prifle/impact.wav" );;		
		
		cgs.media.prifleImpactShader	= trap_R_RegisterShader( "gfx/effects/prifle_hit" );
		cgs.media.compressionAltBeamShader	= trap_R_RegisterShader( "gfx/effects/prifle_altbeam" );
		cgs.media.compressionAltBlastShader	= trap_R_RegisterShader( "gfx/effects/prifle_altblast" );
		cgs.media.compressionMarkShader		= trap_R_RegisterShader( "gfx/damage/burnmark1" );
		cgs.media.prifleBolt				= trap_R_RegisterShader( "gfx/misc/priflebolt" );

		cgs.media.liteRedParticleStreakShader = trap_R_RegisterShader( "gfx/misc/literedparticle_anamorphic" );
		cgs.media.liteRedParticleShader		  = trap_R_RegisterShader( "gfx/misc/literedparticle" );

		cgs.media.flashlightModel			= trap_R_RegisterModel( "models/weapons2/prifle/prifle_flashlight.md3" ); //RPG-X : TiM - flashlight model

		cgs.media.prifleBeam				= trap_R_RegisterShader( "gfx/misc/phaser_rifle" );

		break;

/*	case WP_7: 
		MAKERGB( weaponInfo->flashDlightColor, 0.16, 0.16, 1 ); 
        weaponInfo->flashSound			= trap_S_RegisterSound( "sound/weapons/hitonhead.wav" ); 
        weaponInfo->altFlashSnd			= trap_S_RegisterSound( "sound/weapons/guncharge.wav" ); 
		cgs.media.tetrionTrail2Shader		= trap_R_RegisterShader( "gfx/misc/trail2" );
		cgs.media.compressionMarkShader	= trap_R_RegisterShader( "gfx/damage/burnmark1" );
		weaponInfo->mainHitSound = trap_S_RegisterSound( SOUND_DIR "prifle/impact.wav" );
        break;*/
	/*	
	case WP_7:										//OLD CODE (replaced for TR116)
		MAKERGB( weaponInfo->flashDlightColor, 0.6, 0.6, 1 );
		
		weaponInfo->flashSound = trap_S_RegisterSound( SOUND_DIR "tetrion/fire.wav" );
		weaponInfo->altFlashSnd = trap_S_RegisterSound( SOUND_DIR "tetrion/alt_fire.wav" );
		cgs.media.tetrionRicochetSound1 = trap_S_RegisterSound(SOUND_DIR "tetrion/ricochet1.wav");
		cgs.media.tetrionRicochetSound2 = trap_S_RegisterSound(SOUND_DIR "tetrion/ricochet2.wav");
		cgs.media.tetrionRicochetSound3 = trap_S_RegisterSound(SOUND_DIR "tetrion/ricochet3.wav");

		weaponInfo->missileTrailFunc = FX_TetrionProjectileThink;
		weaponInfo->alt_missileTrailFunc = FX_TetrionProjectileThink;

		cgs.media.greenBurstShader			= trap_R_RegisterShader( "gfx/misc/greenburst" );
		cgs.media.greenTrailShader			= trap_R_RegisterShader( "gfx/misc/greentrail" );
		cgs.media.tetrionTrail2Shader		= trap_R_RegisterShader( "gfx/misc/trail2" );
		cgs.media.tetrionFlareShader		= trap_R_RegisterShader( "gfx/misc/tet1" );
		cgs.media.borgFlareShader			= trap_R_RegisterShader( "gfx/misc/borgflare" );
		cgs.media.bulletmarksShader			= trap_R_RegisterShader( "textures/decals/bulletmark4" );
		break;
*/
	case WP_12:
		weaponInfo->flashSound = weaponInfo->altFlashSnd = trap_S_RegisterSound( "sound/items/jetpuffmed.wav" );
		break;

	case WP_2:
		weaponInfo->firingSound= trap_S_RegisterSound( "sound/items/tricorderscan.wav" ); //altFlashSnd
		weaponInfo->altFiringSound = trap_S_RegisterSound( "sound/ambience/voyager/medictricorder.wav" ); //flashSound
		
		//weaponInfo->isAnimSndBased = qtrue;
		break;

	case WP_3:
		weaponInfo->firingSound = trap_S_RegisterSound( SOUND_DIR "padd/padd_1.wav" ); //flashSound
		weaponInfo->altFiringSound = trap_S_RegisterSound( SOUND_DIR "padd/padd_2.wav" ); //altFlashSnd

		weaponInfo->isAnimSndBased = qtrue;
		break;

	case WP_15:
		weaponInfo->firingSound  = trap_S_RegisterSound( SOUND_DIR "hyperspanner/spanner_1.wav" );
		weaponInfo->altFiringSound = trap_S_RegisterSound( SOUND_DIR "hyperspanner/spanner_2.wav" );
		break;

	case WP_7:
		weaponInfo->flashSound = trap_S_RegisterSound( SOUND_DIR "hitonhead.wav" );
		weaponInfo->altFlashSnd = weaponInfo->flashSound;
		//weaponInfo->altFlashSnd = trap_S_RegisterSound( "sound/weapons/guncharge.wav" );
		break;

//Toolkit
	case WP_14:
		weaponInfo->flashSound = trap_S_RegisterSound( SOUND_DIR "toolkit/toolkit_1.wav" );
		weaponInfo->altFlashSnd = trap_S_RegisterSound( SOUND_DIR "toolkit/toolkit_2.wav" );
		break;

//Medkit
	case WP_11:
		weaponInfo->flashSound = trap_S_RegisterSound( SOUND_DIR "medkit/medkit_1.wav" );
		weaponInfo->altFlashSnd = trap_S_RegisterSound( SOUND_DIR "medkit/medkit_2.wav" );
		break;

	 default:
		MAKERGB( weaponInfo->flashDlightColor, 1, 1, 1 );
		weaponInfo->flashSound = trap_S_RegisterSound( SOUND_DIR "prifle/fire.wav" );
		break;
	}
}

/*
=================
CG_RegisterItemVisuals

The server says this item is used on this level
=================
*/
void CG_RegisterItemVisuals( int itemNum ) {
	itemInfo_t		*itemInfo;
	gitem_t			*item;

	itemInfo = &cg_items[ itemNum ];
	if ( itemInfo->registered ) {
		return;
	}

	item = &bg_itemlist[ itemNum ];

	memset( itemInfo, 0, sizeof( &itemInfo ) );
	itemInfo->registered = qtrue;

	itemInfo->model = trap_R_RegisterModel( item->world_model );

	itemInfo->icon = trap_R_RegisterShader( item->icon );

	if ( item->giType == IT_WEAPON ) {
		CG_RegisterWeapon( item->giTag );
	}

	// since the seeker uses the scavenger rifes sounds, we must precache the scavenger rifle stuff if we hit the item seeker
/*	if ( item->giTag == PW_FLASHLIGHT)
	{
		CG_RegisterWeapon( WP_4 );
	}*/

	// hang onto the handles for holdable items in case they're useable (e.g. detpack)
/*	if (IT_HOLDABLE == item->giType)
	{
		// sanity check
		if ( (item->giTag < HI_NUM_HOLDABLE) && (item->giTag > 0) ) // use "> 0" cuz first slot should be empty
		{
			if (item->world_model[1])
			{
				cgs.useableModels[item->giTag] = trap_R_RegisterModel( item->useablemodel );
			}
			else
			{
				cgs.useableModels[item->giTag] = itemInfo->model];
			}
		}
	}
*/
}


/*
========================================================================================

VIEW WEAPON

========================================================================================
*/

/*
=================
CG_MapTorsoToWeaponFrame

=================
*/
static int CG_MapTorsoToWeaponFrame( clientInfo_t *ci, int frame ) {
	animation_t	*anim;

	// change weapon
	anim = &cg_animsList[ci->animIndex].animations[TORSO_DROPWEAP1];
	if ( frame >= anim->firstFrame 
		&& frame < anim->firstFrame + 9 ) {
		return frame - anim->firstFrame + 6;
	}

	// stand attack
	anim = &cg_animsList[ci->animIndex].animations[BOTH_ATTACK3];
	if ( frame >= anim->firstFrame && frame < anim->firstFrame + 6 ) {
		return 1 + frame - anim->firstFrame;
	}

	// stand attack 2
	anim = &cg_animsList[ci->animIndex].animations[BOTH_ATTACK2];
	if ( frame >= anim->firstFrame && frame < anim->firstFrame + 6 ) {
		return 1 + frame - anim->firstFrame;
	}

	anim = &cg_animsList[ci->animIndex].animations[TORSO_WEAPONREADY1];
	if ( frame >= anim->firstFrame && frame < anim->firstFrame + 6 ) {
		return 1 + frame - anim->firstFrame;
	}

	// change weapon
	//USED TO BE TORSO_RAISE
/*	if ( frame >= ci->animations[TORSO_DROPWEAP1].firstFrame 
		&& frame < ci->animations[TORSO_DROPWEAP1].firstFrame + 9 ) {
		return frame - ci->animations[TORSO_DROPWEAP1].firstFrame + 6;
	}

	// stand attack
	if ( frame >= ci->animations[BOTH_ATTACK3].firstFrame 
		&& frame < ci->animations[BOTH_ATTACK3].firstFrame + 6 ) {
		return 1 + frame - ci->animations[BOTH_ATTACK3].firstFrame;
	}

	// stand attack 2
	if ( frame >= ci->animations[BOTH_ATTACK2].firstFrame 
		&& frame < ci->animations[BOTH_ATTACK2].firstFrame + 6 ) {
		return 1 + frame - ci->animations[BOTH_ATTACK2].firstFrame;
	}*/
	
	return 0;
}

/*
==============
CG_CalculateWeaponPosition
==============
*/
//BOOKMARK
static void CG_CalculateWeaponPosition( vec3_t origin, vec3_t angles ) {
	float	scale;
	int		delta;
	float	fracsin;

	VectorCopy( cg.refdef.vieworg, origin );
	VectorCopy( cg.refdefViewAngles, angles );

	// on odd legs, invert some angles
	if ( cg.bobcycle & 1 ) {
		scale = -cg.xyspeed;
	} else {
		scale = cg.xyspeed;
	}

	// gun angles from bobbing
	angles[ROLL] += scale * cg.bobfracsin * 0.005;
	angles[YAW] += scale * cg.bobfracsin * 0.01;
	angles[PITCH] += cg.xyspeed * cg.bobfracsin * 0.005;

	// drop the weapon when landing
	delta = cg.time - cg.landTime;
	if ( delta < LAND_DEFLECT_TIME ) {
		origin[2] += cg.landChange*0.25 * delta / LAND_DEFLECT_TIME;
	} else if ( delta < LAND_DEFLECT_TIME + LAND_RETURN_TIME ) {
		origin[2] += cg.landChange*0.25 * 
			(LAND_DEFLECT_TIME + LAND_RETURN_TIME - delta) / LAND_RETURN_TIME;
	}

#if 0
	// drop the weapon when stair climbing
	delta = cg.time - cg.stepTime;
	if ( delta < STEP_TIME/2 ) {
		origin[2] -= cg.stepChange*0.25 * delta / (STEP_TIME/2);
	} else if ( delta < STEP_TIME ) {
		origin[2] -= cg.stepChange*0.25 * (STEP_TIME - delta) / (STEP_TIME/2);
	}
#endif

	// idle drift
	scale = cg.xyspeed + 40;
	fracsin = sin( cg.time * 0.001 );
	angles[ROLL] += scale * fracsin * 0.01;
	angles[YAW] += scale * fracsin * 0.01;
	angles[PITCH] += scale * fracsin * 0.01;
}


/*
===============
CG_LightningBolt

Origin will be the exact tag point, which is slightly
different than the muzzle point used for determining hits.
The cent should be the non-predicted cent if it is from the player,
so the endpoint will reflect the simulated strike (lagging the predicted
angle)
===============
*/

#define RANGE_BEAM (2048.0)
#define BEAM_VARIATION	6

void CG_LightningBolt( centity_t *cent, vec3_t origin ) 
{
	trace_t		trace;
//	gentity_t	*traceEnt;
	vec3_t		startpos, endpos, forward;
	qboolean	spark = qfalse, impact = qtrue;

	if ( cg.snap->ps.pm_type == PM_INTERMISSION ) 
	{
		return;		// Don't draw a phaser during an intermission you crezzy mon!
	}

	//Must be a durational weapon
	if ( cent->currentState.clientNum == cg.snap->ps.clientNum && !cg.renderingThirdPerson && !(cent->currentState.eFlags & EF_ITEMPLACEHOLDER ) ) //fuck decoys
	{ 
		// different checks for first person view
		if ( cg.snap->ps.weapon == WP_15 || 
			cg.snap->ps.weapon == WP_5 || 
			cg.snap->ps.weapon == WP_13 || 
			(cg.snap->ps.eFlags & EF_ALT_FIRING && cg.snap->ps.weapon == WP_6 )
			|| (!(cg.snap->ps.eFlags & EF_ALT_FIRING) && cg.snap->ps.weapon == WP_10 ) 
			)
		{	/*continue*/	}
		else
			return;
	} else {
		if ( cent->currentState.weapon == WP_15 || 
			cent->currentState.weapon == WP_5 || 
			cent->currentState.weapon == WP_13 || 
			(cent->currentState.eFlags & EF_ALT_FIRING && (cent->currentState.weapon == WP_6) ) ||
			(!(cent->currentState.eFlags & EF_ALT_FIRING) && cent->currentState.weapon == WP_10)
			)
		{	/*continue*/	}
		else
			return;
	}

	// Find the impact point of the beam
	if ( cent->currentState.clientNum == cg.snap->ps.clientNum
		&& !cg.renderingThirdPerson ) {
		// take origin from view
/*		
		VectorCopy( cg.refdef.vieworg, origin );
		VectorMA( origin, -8, cg.refdef.viewaxis[2], origin );
		VectorMA( origin, 8, cg.refdef.viewaxis[0], origin );
		VectorMA( origin, -2, cg.refdef.viewaxis[1], origin );
*/
		VectorCopy( cg.refdef.viewaxis[0], forward );
		VectorCopy( cg.refdef.vieworg, startpos);
	} 
	else 
	{
		// take origin from entity
		if ( cent->currentState.clientNum == cg.snap->ps.clientNum )
			AngleVectors( cg.predictedPlayerState.viewangles, forward, NULL, NULL );
		else
			AngleVectors( cent->lerpAngles, forward, NULL, NULL );
		VectorCopy( origin, startpos);

		// Check first from the center to the muzzle.
		CG_Trace(&trace, cent->lerpOrigin, vec3_origin, vec3_origin, origin, cent->currentState.number, MASK_SHOT);
		if (trace.fraction < 1.0)
		{	// We hit something here...  Stomp the muzzle back to the eye...
			VectorCopy(cent->lerpOrigin, startpos);
			if ( cg.snap->ps.eFlags & EF_FULL_ROTATE && Q_fabs( cg.snap->ps.viewangles[PITCH] ) > 89.0f )
				startpos[2] -= 20;
			else
				startpos[2] += cg.snap->ps.viewheight;
		}
	}

	VectorMA( startpos, RANGE_BEAM, forward, endpos );

	// Add a subtle variation to the beam weapon's endpoint
	/*for (i = 0; i < 3; i ++ )
	{
		endpos[i] += crandom() * BEAM_VARIATION;
	}*/

	CG_Trace( &trace, startpos, vec3_origin, vec3_origin, endpos, cent->currentState.number, MASK_SHOT );

//	traceEnt = &g_entities[ trace.entityNum ];

	// Make sparking be a bit less frame-rate dependent..also never add sparking when we hit a surface with a NOIMPACT flag
	if (!(trace.surfaceFlags & SURF_NOIMPACT))
	{
		spark = qtrue;
	}

	// Don't draw certain kinds of impacts when it hits a player and such..or when we hit a surface with a NOIMPACT flag
	if ( cg_entities[trace.entityNum].currentState.eType == ET_PLAYER || (trace.surfaceFlags & SURF_NOIMPACT) )
	{
		impact = qfalse;
	}
	
	// Add in the effect
	switch ( cent->currentState.weapon )
	{
	case WP_5:
		if (cg.snap->ps.rechargeTime == 0)
		{
			if (  cent->currentState.eFlags & EF_ALT_FIRING )
				FX_PhaserAltFire( origin, trace.endpos, trace.plane.normal, spark, impact, cent->pe.empty );
			else
				FX_PhaserFire( origin, trace.endpos, trace.plane.normal, spark, impact, cent->pe.empty );
		}
		break;
	case WP_6:
		if ( cent->currentState.eFlags & EF_ALT_FIRING )
		{
			FX_PrifleBeamFire( origin, trace.endpos, trace.plane.normal, spark, impact, cent->pe.empty );
		}
		break;
	case WP_15:
		if ( cent->currentState.eFlags & EF_ALT_FIRING )
			FX_ProbeBeam( origin, forward, cent->currentState.clientNum, qtrue );
		else
			FX_ProbeBeam( origin, forward, cent->currentState.clientNum, qfalse );
		break;

	case WP_13:
		if ( cent->currentState.eFlags & EF_ALT_FIRING )
			FX_RegenBeam( origin, forward, cent->currentState.clientNum, qtrue );
		else
			FX_RegenBeam( origin, forward, cent->currentState.clientNum, qfalse );
		break;

	case WP_10:
		if ( cent->currentState.eFlags & EF_FIRING && !(cent->currentState.eFlags & EF_ALT_FIRING) )
			FX_DisruptorBeamFire( origin, trace.endpos, trace.plane.normal, spark, impact, cent->pe.empty );

/*	case WP_13:
		if (!(cent->currentState.eFlags & EF_ALT_FIRING))
		{
			vec3_t org;

			// Move the beam back a bit to help cover up the poly edges on the fire beam
			VectorMA( origin, -4, forward, org );
			FX_DreadnoughtFire( org, trace.endpos, trace.plane.normal, spark, impact );
		}
		break;*/
	}
}


/*
======================
CG_MachinegunSpinAngle
======================
*/
#define		SPIN_SPEED	0.9
#define		COAST_TIME	1000
static float	CG_MachinegunSpinAngle( centity_t *cent ) {
	int		delta;
	float	angle;
	float	speed;

	delta = cg.time - cent->pe.barrelTime;
	if ( cent->pe.barrelSpinning ) {
		angle = cent->pe.barrelAngle + delta * SPIN_SPEED;
	} else {
		if ( delta > COAST_TIME ) {
			delta = COAST_TIME;
		}

		speed = 0.5 * ( SPIN_SPEED + (float)( COAST_TIME - delta ) / COAST_TIME );
		angle = cent->pe.barrelAngle + delta * speed;
	}

	if ( cent->pe.barrelSpinning == !(cent->currentState.eFlags & EF_FIRING) ) {
		cent->pe.barrelTime = cg.time;
		cent->pe.barrelAngle = AngleMod( angle );
		cent->pe.barrelSpinning = !!(cent->currentState.eFlags & EF_FIRING);
	}

	return angle;
}


/*
========================
CG_AddWeaponWithPowerups
========================
*/

static void CG_AddWeaponWithPowerups( refEntity_t *gun, int powerups, beamData_t* beamData, int cloakTime, int decloakTime ) //
{
	// add powerup effects
	if ( powerups & ( 1 << PW_INVIS ) || ( !(powerups & ( 1 << PW_INVIS )) && decloakTime > 0 ) ) {
		
		//TiM - modified so it persists during the first bit of cloaking / last of decloaking
		if ( ( cloakTime <= 0 && decloakTime <= 0 ) || ( decloakTime > 0 && cg.time < ( decloakTime + Q_FLASH_TIME * 0.5 ) )
				|| ( cloakTime > 0 && cg.time > ( cloakTime + Q_FLASH_TIME * 0.5 ) ) )
		{		
			if ( /*cg.snap->ps.persistant[PERS_CLASS] == PC_ADMIN*/ cgs.clientinfo[cg.snap->ps.clientNum].isAdmin )
			{//admins can see cloaked people
				//gun->customShader = cgs.media.teleportEffectShader;
				//TiM - Make it look cooler - Half invis
				gun->renderfx |= RF_FORCE_ENT_ALPHA;
				gun->shaderRGBA[3] = (unsigned char)(0.4f * 255.0f);
				trap_R_AddRefEntityToScene( gun );
			}
		}
		else
			trap_R_AddRefEntityToScene( gun );

		//gun->customShader = cgs.media.invisShader;
		//trap_R_AddRefEntityToScene( gun );
	} 
	else if ( powerups & ( 1 << PW_BEAM_OUT ) || powerups & ( 1 << PW_QUAD ) )
	{
		int btime;
		btime = cg.time - beamData->beamTimeParam;

		if ( btime <= PLAYER_BEAM_FADE ) {
			if ( powerups & ( 1 << PW_BEAM_OUT ) ) {
				gun->shaderRGBA[3] = 255;
			}
			else {
				gun->shaderRGBA[3] = 0;
			}
		}
		else if ( btime >= ( PLAYER_BEAM_FADE + PLAYER_BEAM_FADETIME ) ) {
			if ( powerups & ( 1 << PW_BEAM_OUT ) ) {
				gun->shaderRGBA[3] = 0;
			}
			else {
				gun->shaderRGBA[3] = 255;
			}
		}

		if (btime > PLAYER_BEAM_FADE && btime < (PLAYER_BEAM_FADE + PLAYER_BEAM_FADETIME) )
		{
			gun->renderfx |= RF_FORCE_ENT_ALPHA;
			gun->shaderRGBA[3] = (int)(255 * beamData->beamAlpha);
		}

		if ( gun->shaderRGBA[3] > 0 ) {
			trap_R_AddRefEntityToScene( gun );
			gun->renderfx &= ~RF_FORCE_ENT_ALPHA;
			gun->shaderRGBA[3] = 255;
		}
		
		//Just a precaution.  Loop it once, then the player should be invisible
		if ( btime < 4100 ) {
			gun->customShader = cgs.media.transportShader;
			gun->shaderTime = beamData->beamTimeParam * 0.001;
			trap_R_AddRefEntityToScene( gun );
		}
	} else if(powerups & (1 << PW_BORG_ADAPT)) {
		gun->renderfx |= RF_FORCE_ENT_ALPHA;
		gun->shaderRGBA[3] = 255;
		trap_R_AddRefEntityToScene(gun);
		gun->customShader = cgs.media.borgFullBodyShieldShader;
		trap_R_AddRefEntityToScene(gun);
		return;
	}
	else {
		trap_R_AddRefEntityToScene( gun );

		if(gun->renderfx & RF_FORCE_ENT_ALPHA) {
			gun->renderfx &= ~RF_FORCE_ENT_ALPHA;
		}

/*		if ( powerups & ( 1 << PW_BOLTON ) ) {
			gun->customShader = cgs.media.battleWeaponShader;
			trap_R_AddRefEntityToScene( gun );
		}*/

/*		if ( powerups & ( 1 << PW_QUAD ) ) {
			gun->customShader = cgs.media.quadWeaponShader;
			trap_R_AddRefEntityToScene( gun );
		}*/
		/*if (powerups & (1 << PW_OUCH))
		{
			gun->customShader = cgs.media.holoOuchShader;
			// set rgb to 1 of 16 values from 0 to 255. don't use random so that the three
			//parts of the player model as well as the gun will all look the same
			gun->shaderRGBA[0] = 
			gun->shaderRGBA[1] = 
			gun->shaderRGBA[2] = ((cg.time % 17)*0.0625)*255;//irandom(0,255);
			trap_R_AddRefEntityToScene(gun);
		}*/
	}
}

/*void CG_CoffeeSteamFirstPerson ( refEntity_t* parent, weaponInfo_t *weapon ) {
	refEntity_t steam;
	vec3_t angle = { 0.0, 0.0, 6.0 };

	CG_PositionEntityOnTag( &steam, parent, weapon->viewModel, "tag_steam1" );

	if ( VectorCompare( steam.origin, parent->origin ) ) {//whelp, for some whacky reason, there's no tag O_o
		return;
	}

	//CG_Steam( steam.origin, angle);
	//Disables the OpenGL Hack where the viewmodel is drawn over EVERYTHING ELSE INCLUDING THE STEAM
	parent->renderfx &= ~RF_DEPTHHACK;

	if (cg.time % 10 == 0 ) {
		FX_AddSprite( steam.origin, 
						angle, qfalse, 
						( random() * 3 + 1), (10), //random() * 4 + 2 //12
						0.6 + random() * 0.4, 0.0,
						random() * 120, //180
						0.0, 
						1300, //300 //random() * 200 + 1200, //300 //
						cgs.media.steamShader );
	}
	localEntity_t *FX_AddSprite(vec3_t origin, vec3_t velocity, qboolean gravity, float scale, float dscale, 
							float startalpha, float endalpha, float roll, float elasticity, 
							float killTime, qhandle_t shader)*/

	//if ( 
/*}

void CG_CoffeeSteamThirdPerson ( refEntity_t* parent, weaponInfo_t *weapon) {
	refEntity_t steam;
	localEntity_t *le = NULL;

	vec3_t angle = { 0.0, 0.0, 6.0 };

	CG_PositionEntityOnTag( &steam, parent, weapon->weaponModel, "tag_steam");

	if ( VectorCompare( steam.origin, parent->origin ) ) {//whelp, for some whacky reason, there's no tag O_o
		return;
	}

	if (cg.time % 10 == 0 ) {
		le = FX_AddSprite( steam.origin, 
						angle, qfalse, 
						( random() * 1.2 + 0.5), ( 5 ), //random() * 4 + 2 //12
						0.6 + random() * 0.4, 0.0,
						random() * 120, //180
						0.0, 
						1300, //300 //random() * 200 + 1200, //300 //
						cgs.media.steamShader );
	}
	localEntity_t *FX_AddSprite(vec3_t origin, vec3_t velocity, qboolean gravity, float scale, float dscale, 
							float startalpha, float endalpha, float roll, float elasticity, 
							float killTime, qhandle_t shader)*/

	//if ( 
//}*/

void CG_CoffeeSteam( refEntity_t* parent, weaponInfo_t *weapon, qboolean thirdperson ) {
	refEntity_t steam;
	localEntity_t *le;

	vec3_t angle = { 0.0, 0.0, 10.0 };

	//FIXME: I probably should name the tag the same thing in both models... O_o
	if ( !thirdperson ) {
		 CG_PositionEntityOnTag( &steam, parent, weapon->viewModel, "tag_steam1" );
	}
	else {
		CG_PositionEntityOnTag( &steam, parent, weapon->weaponModel, "tag_steam");
	}

	if ( VectorCompare( steam.origin, parent->origin ) ) {//whelp, for some whacky reason, there's no tag O_o
		return;
	}

	//CG_Steam( steam.origin, angle);
	//Disables the OpenGL Hack where the viewmodel is drawn over EVERYTHING ELSE INCLUDING THE STEAM
	parent->renderfx &= ~RF_DEPTHHACK;

	if (cg.time % 10 == 0 ) { //release a sprite every .01 of a second
		le = FX_AddSprite( steam.origin, 
						angle, qfalse, 
						( thirdperson ? random() * 1.2 + 0.5 : random() * 1 + 1), ( thirdperson ? 7 : 10), //random() * 4 + 2 //12
						0.05 + random() * 0.1, 0.0,
						random() * 120, //180
						0.0, 
						1500, //300 //random() * 200 + 1200, //300 //
						cgs.media.steamShader );
	}
	/*localEntity_t *FX_AddSprite(vec3_t origin, vec3_t velocity, qboolean gravity, float scale, float dscale, 
							float startalpha, float endalpha, float roll, float elasticity, 
							float killTime, qhandle_t shader)*/
	//Without this, the steam gets drawn behind the cup... which looks weird
	//le->refEntity.renderfx |= RF_DEPTHHACK;
}

/*
=============
getClassColor

RPG-X : TiM - used to determine what color skins the weapons should have applied to them
My way of having to not have to enter in so many conditionals over and over
=============
*/

//char *getClassColor ( void )
//{
//		/*if (( cg.snap->ps.persistant[PERS_CLASS] == PC_SECURITY ) 
//		|| ( cg.snap->ps.persistant[PERS_CLASS] == PC_ENGINEER)) 
//		{
//			return "default";	
//		}
//			
//		if (( cg.snap->ps.persistant[PERS_CLASS] == PC_SCIENCE ) 
//			|| ( cg.snap->ps.persistant[PERS_CLASS] == PC_MEDICAL )
//			|| ( cg.snap->ps.persistant[PERS_CLASS] == PC_ALPHAOMEGA22 ))
//		{
//				return "teal";
//		}	
//			
//		if ((cg.snap->ps.persistant[PERS_CLASS] == PC_COMMAND) 
//		|| (cg.snap->ps.persistant[PERS_CLASS] == PC_ADMIN))
//		{
//				return "red";	
//		}
//		if ( cg.snap->ps.persistant[PERS_CLASS] == PC_NOCLASS )  {
//			return "NULL";
//		}*/
//
//		//lolz... this time, let's base it off of current model
//		//rather than class
//		cgs.clientinfo[0].
//
//		return "default";
//}


/*
=============
CG_AddPlayerWeapon

Used for both the view weapon (ps is valid) and the world modelother character models (ps is NULL)
The main player will have this called for BOTH cases, so effects like light and
sound should only be done on the world model case.
=============
*/
void CG_AddPlayerWeapon( refEntity_t *parent, playerState_t *ps, centity_t *cent ) {
	refEntity_t	gun;
	refEntity_t	barrel;
	refEntity_t	flash;
	vec3_t		angles;
	weapon_t	weaponNum;
	weaponInfo_t	*weapon;
	centity_t	*nonPredictedCent;
	int				i = 0, numBarrels = 0;
	wpnBarrelInfo_t	*barrelInfo = NULL;

	char filename[MAX_QPATH];
	char* skinColor;

	weaponNum = cent->currentState.weapon;

	CG_RegisterWeapon( weaponNum );
	weapon = &cg_weapons[weaponNum];

	// add the weapon
	memset( &gun, 0, sizeof( gun ) );
	VectorCopy( parent->lightingOrigin, gun.lightingOrigin );
	gun.shadowPlane = parent->shadowPlane;
	gun.renderfx = parent->renderfx;

	// set custom shading for railgun refire rate
	/*if ( ps ) {
		if ( cg.predictedPlayerState.weapon == WP_1 
			&& cg.predictedPlayerState.weaponstate == WEAPON_FIRING ) {
			float	f;

			f = (float)cg.predictedPlayerState.weaponTime / 1500;
			gun.shaderRGBA[1] = 0;
			gun.shaderRGBA[0] = 
			gun.shaderRGBA[2] = 255 * ( 1.0 - f );
		} else {
			gun.shaderRGBA[0] = 255;
			gun.shaderRGBA[1] = 255;
			gun.shaderRGBA[2] = 255;
			gun.shaderRGBA[3] = 255;
		}
	}*/

	if (ps)
	{
		qhandle_t	skin;

		gun.hModel = weapon->viewModel;
		
		skinColor = cgs.clientinfo[cg.snap->ps.clientNum].skinName;

		//if ( skinColor != "NULL" ) { //RPG-X : TiM - Will change the color of the band on the viewmodel's arm, depending what class
		if(!Q_stricmpn(skinColor, "NULL", 4)) {
			for ( i = 0; i < 8; i++ ) {
				if ( cg.predictedPlayerState.weapon == (RAweapons[i]) ) {
					Com_sprintf( filename, sizeof( filename ),"models/weapons2/%s/model_%s.skin", RAweapFileName[i], skinColor ); //Formulate the skin route
					
					skin = trap_R_RegisterSkin ( filename );
					
					if ( !skin )
						break;

					gun.customSkin = skin; //and 'plonk' it on the model :)
					break;
				}
			}
		}

	}
	else
	{
		gun.hModel = weapon->weaponModel;
	}

	if (!gun.hModel) {
		return;
	}

	if ( !ps ) {
		// add weapon stop sound
		if ( !( cent->currentState.eFlags & EF_FIRING ) && !( cent->currentState.eFlags & EF_ALT_FIRING ) && cent->pe.lightningFiring &&
			cg.predictedPlayerState.ammo[cg.predictedPlayerState.weapon] )
		{
			if (weapon->stopSound)
			{
				trap_S_StartSound( cent->lerpOrigin, cent->currentState.number, CHAN_WEAPON, weapon->stopSound );
			}
			else if (weapon->altStopSound )
			{
				trap_S_StartSound( cent->lerpOrigin, cent->currentState.number, CHAN_WEAPON, weapon->altStopSound );
			}
		}
		cent->pe.lightningFiring = qfalse;
		if ( cent->currentState.eFlags & EF_ALT_FIRING )
		{
			// hark, I smell hackery afoot
			if ((weaponNum == WP_5) && !(cg.predictedPlayerState.ammo[WP_5]))
			{
				trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin, cgs.media.phaserEmptySound );
				cent->pe.lightningFiring = qtrue;
			}
			else if ( weapon->altFiringSound && !weapon->isAnimSndBased )
			{
				trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin, weapon->altFiringSound );
				cent->pe.lightningFiring = qtrue;
			}

			if ( weaponNum == WP_14 || weaponNum == WP_11 ) {
				cent->pe.lightningFiring = qtrue;
			}
		}
		else if ( cent->currentState.eFlags & EF_FIRING )
		{
			if ((weaponNum == WP_5) && !(cg.predictedPlayerState.ammo[WP_5]))
			{
				trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin, cgs.media.phaserEmptySound );
				cent->pe.lightningFiring = qtrue;
			}
			else if ( weapon->firingSound && !weapon->isAnimSndBased )
			{
				trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin, weapon->firingSound );
				cent->pe.lightningFiring = qtrue;
			}

			//TiM: Haxxor.  I want the medkit + toolkit sounds to play only once when u hold them down
			if ( weaponNum == WP_14 || weaponNum == WP_11 ) {
				cent->pe.lightningFiring = qtrue;
			}
		}
	}


	CG_PositionEntityOnTag( &gun, parent, parent->hModel, "tag_weapon");
	//RPG-X : TiM - A little variety here :) Toolkit gets attached to player model's left hand, medkit on waist :)
	//Hack: I dunno why, but unless I specified thirdperson (ie (!ps) ), the viewmodel went crazy. :P
	if (!ps) {
		if (( weaponNum == WP_14 ) ) { //Toolkit //cg.predictedPlayerState.weapon
			CG_PositionEntityOnTag( &gun, parent, parent->hModel, "tag_lhand");
		}
		else if (( weaponNum == WP_11 ) ) { //Medkit
			CG_PositionEntityOnTag( &gun, parent, parent->hModel, "tag_torso");
		}
		/*else {
			CG_PositionEntityOnTag( &gun, parent, parent->hModel, "tag_weapon");
		}*/
		//TiM: also in the hopes of keeping the weapon scale constant in contrast to the player model
		gun.nonNormalizedAxes = qfalse;
	}
		
	if ( weaponNum == WP_4 ) {
		if ( !ps ) {
			if ( !(!cg.renderingThirdPerson && cent->currentState.clientNum == cg.predictedPlayerState.clientNum) )
				CG_CoffeeSteam( &gun, weapon, qtrue );
		}
		//else {
		//	CG_CoffeeSteam( &gun, weapon, qfalse );
		//}
	}

	CG_AddWeaponWithPowerups( &gun, cent->currentState.powerups, &cent->beamData, cent->cloakTime, cent->decloakTime );

	// add the spinning barrel
	//
	// 
	for (barrelInfo = wpnBarrelData; barrelInfo->giTag != WP_0; barrelInfo++)
	{
		if (barrelInfo->giTag == weaponNum)
		{
			numBarrels = barrelInfo->numBarrels;
			break;
		}
	}

	// don't add barrels to world model...only viewmodels
	if (ps)
	{
		for (i = 0; i < numBarrels; i++)
		{
			memset( &barrel, 0, sizeof( barrel ) );
			VectorCopy( parent->lightingOrigin, barrel.lightingOrigin );
			barrel.shadowPlane = parent->shadowPlane;
			barrel.renderfx = parent->renderfx;

			barrel.hModel = weapon->barrelModel[i];
			angles[YAW] = 0;
			angles[PITCH] = 0;
			if ( weaponNum == WP_7) {
				angles[ROLL] = CG_MachinegunSpinAngle( cent );
			} else {
				angles[ROLL] = 0;//CG_MachinegunSpinAngle( cent );
			}
			AnglesToAxis( angles, barrel.axis );

			if (!i) {
				CG_PositionRotatedEntityOnTag( &barrel, parent, weapon->handsModel, "tag_barrel" );
			} else {
				CG_PositionRotatedEntityOnTag( &barrel, parent, weapon->handsModel, va("tag_barrel%d",i+1) );
			}

			CG_AddWeaponWithPowerups( &barrel, cent->currentState.powerups, &cent->beamData, cent->cloakTime, cent->decloakTime );
		}
	}

	// make sure we aren't looking at cg.predictedPlayerEntity for LG
	nonPredictedCent = &cg_entities[cent->currentState.clientNum];

	// if the index of the nonPredictedCent is not the same as the clientNum
	// then this is a fake player (like on teh single player podiums), so
	// go ahead and use the cent
	if( ( nonPredictedCent - cg_entities ) != cent->currentState.clientNum ) {
		nonPredictedCent = cent;
	}

	//Com_Printf("eType: %i, eventParm: %i, weaponNum: %i\n", cent->currentState.eType, cent->currentState.eventParm, weaponNum);
	if ( weaponNum == WP_6 
		&& 
		cent->currentState.powerups & ( 1 << PW_FLASHLIGHT ) 
		&& 
		cent->beamData.beamTimeParam == 0 
		&&
		( !(cent->currentState.powerups & ( 1 << PW_INVIS ))
		|| cent->currentState.clientNum == cg.predictedPlayerState.clientNum )
		) 
		{ //FIXME: TiM - need to know if flashlight is on or off at the time :S
		refEntity_t flashlight;

		memset( &flashlight, 0, sizeof( flashlight ) );
		VectorCopy( parent->lightingOrigin, flashlight.lightingOrigin );
		flashlight.shadowPlane = parent->shadowPlane;
		flashlight.renderfx = parent->renderfx;

		flashlight.hModel = cgs.media.flashlightModel;
		if (!flashlight.hModel) {
			return;
		}

		angles[YAW] = 0;
		angles[PITCH] = 0;
		angles[ROLL] = 0;

		AnglesToAxis( angles, flashlight.axis );

		if (ps)
		{	// Rendering inside the head...
			CG_PositionRotatedEntityOnTag( &flashlight, &gun, weapon->viewModel, "tag_flashlight");
		}
		else
		{	// Rendering outside the head...
			CG_PositionRotatedEntityOnTag( &flashlight, &gun, weapon->weaponModel, "tag_flashlight");
		}
		trap_R_AddRefEntityToScene( &flashlight );
	}

	// add the flash
	if ( (	weaponNum == WP_5 ||
			weaponNum == WP_13)
		&& ( nonPredictedCent->currentState.eFlags & EF_FIRING ) ) 
	{
		// continuous flash
	} 
	else 
	{
		// impulse flash
		//if ( cg.time - cent->muzzleFlashTime > MUZZLE_FLASH_TIME) {
		if ( cg.time - cent->muzzleFlashTime > wpnBarrelData[weaponNum-1].flashTime )
		{
			return;
		}
	}

	memset( &flash, 0, sizeof( flash ) );
	VectorCopy( parent->lightingOrigin, flash.lightingOrigin );
	flash.shadowPlane = parent->shadowPlane;
	flash.renderfx = parent->renderfx;

	flash.hModel = weapon->flashModel;
	if (!flash.hModel) {
		return;
	}

	angles[YAW] = 0;
	angles[PITCH] = 0;
	angles[ROLL] = 0; //angles[ROLL] = crandom() * 10; //RPG-X - TiM: This stops the lensflare on the muzzle from jiggling around

	AnglesToAxis( angles, flash.axis );

	//TiM - Instead of briefly showing the flash, show it scaling down
	if (weaponNum != WP_5 && 
		weaponNum != WP_15 && 
		weaponNum != WP_13 && 
		!(weaponNum == WP_6 && (cent->currentState.eFlags & EF_ALT_FIRING) )  &&
		!(weaponNum == WP_10 && !(cent->currentState.eFlags & EF_ALT_FIRING) ) 
		)
	{
		float scale;
		scale = (1.0f - ( (float)(cg.time - cent->muzzleFlashTime) / (float)wpnBarrelData[weaponNum-1].flashTime )) * 2.0f;

		flash.nonNormalizedAxes = qtrue;
		VectorScale( flash.axis[0], scale, flash.axis[0] );
		VectorScale( flash.axis[1], scale, flash.axis[1] );
		VectorScale( flash.axis[2], scale, flash.axis[2] );
	}

	//TiM - quick hack
	//jiggle the scale of the phaser rifle on alt fire around
	if ( (weaponNum == WP_6 && (cent->currentState.eFlags & EF_ALT_FIRING))
		||
			( weaponNum == WP_10 && !(cent->currentState.eFlags & EF_ALT_FIRING)) )
	{
		float min, max;

		if ( weaponNum == WP_6 )
		{
			min = 1.3f;
			max = 1.6f;
		}
		else
		{
			min = 0.8f;
			max = 0.9f;
		}	

		VectorScale( flash.axis[0], flrandom(min, max), flash.axis[0] ); 
		VectorScale( flash.axis[1], flrandom(min, max), flash.axis[1] );
		VectorScale( flash.axis[2], flrandom(min, max), flash.axis[2] );
	}

	if (cent->pe.empty)
	{	// Make muzzle flash wussy when empty.
		flash.customShader = cgs.media.phaserMuzzleEmptyShader;
	}

	if (ps)
	{	// Rendering inside the head...
		CG_PositionRotatedEntityOnTag( &flash, &gun, weapon->viewModel, "tag_flash" );
	}
	else
	{	// Rendering outside the head...
		CG_PositionRotatedEntityOnTag( &flash, &gun, weapon->weaponModel, "tag_flash" );
	}

	if ( !(cent->currentState.powerups & ( 1 << PW_INVIS ))
		|| cent->currentState.clientNum == cg.predictedPlayerState.clientNum ) 
	{
		trap_R_AddRefEntityToScene( &flash );
	}

	if ( ps || cg.renderingThirdPerson || cent->currentState.number != cg.predictedPlayerState.clientNum || cg_firstPersonBody.integer ) 
	{
		// add phaser/dreadnought
		// grrr nonPredictedCent doesn't have the proper empty setting
		nonPredictedCent->pe.empty = cent->pe.empty;
		CG_LightningBolt( nonPredictedCent, flash.origin );

		// make a dlight for the flash
		if ( (weapon->flashDlightColor[0] || weapon->flashDlightColor[1] || weapon->flashDlightColor[2]) && !(cent->currentState.powerups & ( 1 << PW_INVIS ) ) ) {
			trap_R_AddLightToScene( flash.origin, 200 + (rand()&31), weapon->flashDlightColor[0],
				weapon->flashDlightColor[1], weapon->flashDlightColor[2] );
		}
	}
}

/*
==============
CG_AddViewWeapon

Add the weapon, and flash for the player's view
==============
*/
void CG_AddViewWeapon( playerState_t *ps ) {
	refEntity_t	hand;
	centity_t	*cent;
	clientInfo_t	*ci;
	float		fovOffset;
	vec3_t		angles;
	weaponInfo_t	*weapon;

	if ( ps->persistant[PERS_TEAM] == TEAM_SPECTATOR /*|| (ps->eFlags&EF_ELIMINATED)*/ ) {
		return;
	}

	if ( ps->pm_type == PM_INTERMISSION ) {
		return;
	}

	// no gun if in third person view
	if ( cg.renderingThirdPerson || cg_firstPersonBody.integer ) {
		return;
	}

	// allow the gun to be completely removed
	//TiM: Added alt fire for alt-fire beam weapons
	if ( !cg_drawGun.integer || cg.zoomed ) {
		vec3_t		origin;

		if ( cg.predictedPlayerState.eFlags & EF_FIRING || cg.predictedPlayerState.eFlags & EF_ALT_FIRING )
		{
			// special hack for phaser/dreadnought...
			VectorCopy( cg.refdef.vieworg, origin );
			VectorMA( origin, -8, cg.refdef.viewaxis[2], origin );
			CG_LightningBolt( &cg_entities[ps->clientNum], origin );
		}
		return;
	}

	if ( (cg.zoomed) && (ps->weapon == WP_6) ) { //RPG-X : TiM - People were saying that being able to see the gunsight on the rifle thru the gunsight in zoom mode was weird :P
		return;
	}

	// don't draw if testing a gun model
	if ( cg.testGun ) {
		return;
	}

	// drop gun lower at higher fov
	if ( cg_fov.integer > 80 ) {
		fovOffset = -0.2 * ( cg_fov.integer - 80 );
	} else {
		fovOffset = 0;
	}

	cent = &cg.predictedPlayerEntity;	// &cg_entities[cg.snap->ps.clientNum];
	CG_RegisterWeapon( ps->weapon );
	weapon = &cg_weapons[ ps->weapon ];

	memset (&hand, 0, sizeof(hand));

	// set up gun position
	CG_CalculateWeaponPosition( hand.origin, angles );

	VectorMA( hand.origin, cg_gun_x.value, cg.refdef.viewaxis[0], hand.origin );
	VectorMA( hand.origin, cg_gun_y.value, cg.refdef.viewaxis[1], hand.origin );
	VectorMA( hand.origin, (cg_gun_z.value+fovOffset), cg.refdef.viewaxis[2], hand.origin );

	AnglesToAxis( angles, hand.axis );

	// map torso animations to weapon animations
	if ( cg_gun_frame.integer ) {
		// development tool
		hand.frame = hand.oldframe = cg_gun_frame.integer;
		hand.backlerp = 0;
	} else {
		// get clientinfo for animation map
		ci = &cgs.clientinfo[ cent->currentState.clientNum ];
		hand.frame = CG_MapTorsoToWeaponFrame( ci, cent->pe.torso.frame );
		hand.oldframe = CG_MapTorsoToWeaponFrame( ci, cent->pe.torso.oldFrame );
		hand.backlerp = cent->pe.torso.backlerp;
	}

	hand.hModel = weapon->handsModel;
	hand.renderfx = RF_DEPTHHACK | RF_FIRST_PERSON;

	// add everything onto the hand
	CG_AddPlayerWeapon( &hand, ps, &cg.predictedPlayerEntity );
}

/*
==============================================================================

WEAPON SELECTION

==============================================================================
*/

void static CG_RegisterWeaponIcon( int weaponNum ) {
	weaponInfo_t	*weaponInfo;
	gitem_t			*item;

	weaponInfo = &cg_weapons[weaponNum];

	if ( weaponNum == 0 ) {
		return;
	}

	if ( weaponInfo->registered ) {
		return;
	}

	for ( item = bg_itemlist + 1 ; item->classname ; item++ ) {
		if ( item->giType == IT_WEAPON && item->giTag == weaponNum ) {
			weaponInfo->item = item;
			break;
		}
	}
	if ( !item->classname ) {
		CG_Error( "Couldn't find weapon %i", weaponNum );
	}

	weaponInfo->weaponIcon = trap_R_RegisterShader( item->icon );
}

/*
==================
CG_DrawWeaponIcon
RPG-X | Phenix | 08/06/2005
RPG-X |  TiM   | 5/1/2006
===========================
*/
void CG_DrawWeaponIcon ( int x, int y, int weapon )
{
	/*vec4_t	color;

	color[3] = alpha;
	if ( !color[3] ) {
		return;
	}*/

	CG_RegisterWeaponIcon( weapon );	//short version

	// draw selection marker

	if ( weapon == cg.weaponSelect ) 
	{
		trap_R_SetColor( colorTable[CT_LTPURPLE1] );
	}
	else
	{
		trap_R_SetColor(colorTable[CT_DKPURPLE1]);
	}

	CG_DrawPic( x-4,y-4,38, 38, cgs.media.weaponbox);

	// draw weapon icon
	trap_R_SetColor(colorTable[CT_WHITE]);
	CG_DrawPic( x, y, 32, 32, cg_weapons[weapon].weaponIcon );

	// draw selection marker
	/*if ( weapon == cg.weaponSelect ) {
		CG_DrawPic( x-4, y-4, 40, 40, cgs.media.selectShader );*/
	//}
	trap_R_SetColor( NULL );
}

/*
===================
CG_DrawWeaponSelect
===================
*/

static int	weaponRows[6][3] = { { WP_1, 0, 0 },
								 { WP_2, WP_3, WP_4 },
								 { WP_5, WP_6, WP_7 },
								 { WP_8, WP_9, WP_10 },
								 { WP_11, WP_12, WP_13 },
								 { WP_14, WP_15, 0 } };

void CG_DrawWeaponSelect( void ) {
	int		i, rowCount, cellCount;
	int		bits;
	//int		count;
	int		x, y, w, defaultX, defaultY;
	char	*name;
	float	*color;
	qboolean	WeapOnThisRow = qfalse;
	//vec4_t	color;

	// don't display if dead
	if ( cg.predictedPlayerState.stats[STAT_HEALTH] <= 1 || cg.predictedPlayerState.eFlags & EF_DEAD ) { //RPG-X: RedTechie - No weapons at health 1 (you die at health 1 now)
		return;
	}

	color = CG_FadeColor( cg.weaponSelectTime, WEAPON_SELECT_TIME );
	if ( !color ) {
		return;
	}

	// showing weapon select clears pickup item display, but not the blend blob
	cg.itemPickupTime = 0;

	// count the number of weapons owned
	bits = cg.snap->ps.stats[ STAT_WEAPONS ];
	
	//NEW HUD FOR RPG-X
	defaultX = 18;
	defaultY = 52;
	x = defaultX;

	y = (BIGCHAR_HEIGHT * 2) + 20;

	for ( i = 0, rowCount = 0, cellCount = 0; i < MAX_WEAPONS; i++, cellCount++ ) {
		if ( cellCount == 3 ) { //we've hit the end of the row
			rowCount++; //go to the next row
			cellCount = 0; //reset cell clock

			if ( WeapOnThisRow ) {
				//**** Draw the end caps *****
				//VectorCopy( colorTable[CT_LTPURPLE2], color );
				trap_R_SetColor(colorTable[CT_LTPURPLE2]);
				// Left end cap
				CG_DrawPic( 2, y - 5, 16, 50, cgs.media.weaponcap1); //6
				// Right End Cap
				CG_DrawPic( x - 20 + 16, y - 5, 16, 50, cgs.media.weaponcap2); //2 - 6, 16 - 18
				trap_R_SetColor(NULL);

				y += defaultY;
				x = defaultX;

				WeapOnThisRow = qfalse;
			}

			if ( rowCount >= 6 ) { //if we exceed our rows, that's bad O_o
				break;
			}
		}

		if ( weaponRows[rowCount][cellCount] == 0 ) {
			i--;
			continue;
		}

		if (bits & ( 1 << weaponRows[rowCount][cellCount] ) ) {
			CG_DrawWeaponIcon( x, y, weaponRows[rowCount][cellCount] );
			x += 40;

			if ( !WeapOnThisRow ) {
				WeapOnThisRow = qtrue;
			}
		}
	}

	// END HUD

	// draw the selected names
	if ( cg_weapons[ cg.weaponSelect ].item ) {
		name = cg_weapons[ cg.weaponSelect ].item->pickup_name;
		if ( name ) {
			w= UI_ProportionalStringWidth(name,UI_SMALLFONT);
			UI_DrawProportionalString(x, y, name, UI_SMALLFONT,color);

		}
	}

	trap_R_SetColor( NULL );
}


/*
===============
CG_WeaponSelectable
===============
*/
static qboolean CG_WeaponSelectable( int i ) {
	if ( !cg.snap->ps.ammo[i] ) {
		return qfalse;
	}
	if ( ! (cg.snap->ps.stats[ STAT_WEAPONS ] & ( 1 << i ) ) ) {
		return qfalse;
	}

	return qtrue;
}

extern int altAmmoUsage[];
/*
{
	0,				//WP_0,
	2,				//WP_5,				
	10,				//WP_6,	
	3,				//WP_1,				
	5,				//WP_4,		
	1,				//WP_10,				
	1,				//WP_8,	
	2,				//WP_7,	
	2,				//WP_9,		
	5				//WP_13,
	20,				//WP_12,
	##,				//WP_14,
	##,				//WP_11,

};
*/

/*
===============
CG_WeaponAltSelectable
===============
*/
static qboolean CG_WeaponAltSelectable( int i ) {
	if ( cg.snap->ps.ammo[i] < altAmmoUsage[cg.snap->ps.weapon]) {
		return qfalse;
	}
	if ( ! (cg.snap->ps.stats[ STAT_WEAPONS ] & ( 1 << i ) ) ) {
		return qfalse;
	}

	return qtrue;
}


/*
===============
CG_NextWeapon_f
===============
*/
void CG_NextWeapon_f( void ) {
	int		i; //, topWeapon
	int		original;
//	int		newWeapons[16];
//	int		bits;

	if ( !cg.snap ) {
		return;
	}
	if ( cg.snap->ps.pm_flags & PMF_FOLLOW ) {
		return;
	}

	cg.weaponSelectTime = cg.time;
	original = cg.weaponSelect;

	
	//RPG-X | Phenix | 08/06/2005
	//Removed to be replaced to scroll through our list
	//TiM | 4/1/2006
	//Put back in since I optimized the way weapons are handled
	  for ( i = 0 ; i < 16 ; i++ ) {
		cg.weaponSelect++;
		if ( cg.weaponSelect == 16 ) {
			cg.weaponSelect = 0;
		}
		if ( CG_WeaponSelectable( cg.weaponSelect ) ) {
			break;
		}
	}
	if ( i == 16 ) {
		cg.weaponSelect = original;
	}

	//TiM: Just for the record.  Phenix.  Enumerated value lists.  Look them up.  Use them!
	//Reading this code was really tricky when it didn't have to be >.<
	//ie 1 = WP_5 etc
}

/*
===============
CG_PrevWeapon_f
===============
*/
void CG_PrevWeapon_f( void ) {
	int		i; //, topWeapon
	int		original;
//	int		newWeapons[16];
//	int		bits;

	if ( !cg.snap ) {
		return;
	}
	if ( cg.snap->ps.pm_flags & PMF_FOLLOW ) {
		return;
	}

	cg.weaponSelectTime = cg.time;
	original = cg.weaponSelect;
	
	//RPG-X | Phenix | 08/06/2005
	//Removed to be replaced to scroll through our list
	//TiM | 4/1/2006
	//Put back in since I optimized the way weapons are handled
	for ( i = 0 ; i < 16 ; i++ ) {
		cg.weaponSelect--;
		if ( cg.weaponSelect == -1 ) {
			cg.weaponSelect = 15;
		}
		if ( CG_WeaponSelectable( cg.weaponSelect ) ) {
			break;
		}
	}
	if ( i == 16 ) {
		cg.weaponSelect = original;
	}
}

/*
===============
CG_Weapon_f
===============
*/
/*TiM : Here for reference
static int	weaponRows[6][3] = { WP_1, 0, 0,
								 WP_2, WP_3, WP_4,
								 WP_5, WP_6, WP_7,
								 WP_8, WP_9, WP_10,
								 WP_11, WP_12, WP_13,
								 WP_14, WP_NEUTRINO_PROBE, 0 };*/

void CG_Weapon_f( void ) {
	int		num;
	//int		newWeapons[16];
	int		i;
	int		bits;
	int		weaponsOnRow;
	int		weaponGot[6];
	int		onRow;
	int		onCol;
	int		rowsUsed;
	int		currentWeaponCol;

	if ( !cg.snap ) {
		return;
	}
	if ( cg.snap->ps.pm_flags & PMF_FOLLOW ) {
		return;
	}

	num = atoi( CG_Argv( 1 ) );
	bits = cg.snap->ps.stats[ STAT_WEAPONS ];

	//TiM - 0 = Null hand weapon now
	//if ( num < 1 || num > 15 ) {
	if ( num < 0 || num > 15 ) {
		return;
	}

	cg.weaponSelectTime = cg.time;

	//Hacky Override: 0 = Null hand no matter what.
	if (num == 0 ) {
		if ( bits & ( 1 << WP_1 ) ) {
			cg.weaponSelect = WP_1;
		}
		return;
	}

	//TiM : The code below went into an infinite loop if a high number was 
	//set as an arg to this command.
	//Lemme just insert a check to make sure the code NEVER accepts args higher
	//than the size of our weapons array. I'll put it underneath the weaponSelectTime
	//statement, so the user will still see a response to their input.
	else if ( num > 5 ) {
		return;
	}

	/* RPG-X | Phenix | 02/02/2006
	 * 
	 * Code to group weapons together by keyboard */

	//Init weaponGot values
	/*for (i = 0; i < 6; i++)
		weaponGot[i] = -1;*/
	memset( weaponGot, -1, sizeof( weaponGot ) );

	onCol = 0;
	weaponsOnRow = 0;
	rowsUsed = 0;
	currentWeaponCol = -1;

	//Loop though every weapon in weaponRows (starting on row 2 - WHY TIM WHY!) 
	//TiM: ... because :)
	for ( i = 0, onRow = 1; i < 15; i++ )
	{
		if (onCol == 3)
		{
			onCol = 0;
			weaponsOnRow = 0;
			onRow++;

			if (onRow > 5) //Something has gone wrong!
				break;
		}
		
		if ( weaponRows[onRow][onCol] > 0)
		{ //Double check this is a weapon
			if (( bits & ( 1 << weaponRows[onRow][onCol] ) ) && (weaponsOnRow == 0))
			{ //If we have this weapon And it is the first weapon on this row we have
				weaponGot[rowsUsed] = onRow;
				weaponsOnRow++;
				rowsUsed++;
			}

			if ((cg.predictedPlayerState.weapon == weaponRows[onRow][onCol]) && (rowsUsed == num))
			{ //If this is the selected weapon record what column it is on
				currentWeaponCol = onCol;
			}
		}

		onCol++;
	}	

	//If they selected a row that doesn't exist
	if (weaponGot[num - 1] == -1)
		return; //(dont need to worry about num being zero because of tims hack ^^)

	do
	{ //Loop though this row until we come accross a weapon which the player has got and is not "null" (0)
		currentWeaponCol++;

		if (currentWeaponCol == 3)
		{
			currentWeaponCol = 0;
		}
	} while ((weaponRows[ weaponGot[num - 1] ][currentWeaponCol] == 0) || !( bits & ( 1 << weaponRows[weaponGot[num - 1]][currentWeaponCol] )));

	cg.weaponSelect = weaponRows[weaponGot[num - 1]][currentWeaponCol];

	//TiM - based on the number we pressed, and whichever 
	//weapons we have in sequential order, select the one that corresponds.

	//Start at number 2, skipping null hand.  He owns us all
	/*for ( i = WP_2, weaponCount = 0; i < MAX_WEAPONS; i++ ) {
		//if we have that weapon
		if ( bits & ( 1 << i ) ) {
			weaponCount++;

			if ( weaponCount == num ) {
				cg.weaponSelect = i;
				return;
			}
		}
	}*/
}

/*
===================
CG_OutOfAmmoChange

The current weapon has just run out of ammo
===================
*/
void CG_OutOfAmmoChange( qboolean altfire ) {
	int		i;

	cg.weaponSelectTime = cg.time;

	for ( i = 15 ; i > 0 ; i-- )
	{
		if (altfire)
		{
			if ( CG_WeaponAltSelectable( i ) )
			{
				cg.weaponSelect = i;
				break;
			}
		}
		else
		{
			if ( CG_WeaponSelectable( i ) )
			{
				cg.weaponSelect = i;
				break;
			}
		}
	}
}



/*
===================================================================================================

WEAPON EVENTS

===================================================================================================
*/

/*
================
CG_FireWeapon

Caused by an EV_FIRE_WEAPON event
================
*/
int tris_state = 0;
void CG_FireWeapon( centity_t *cent, qboolean alt_fire ) {
	entityState_t *ent;
	weaponInfo_t	*weap;
	int     rpg_effectsgun;
	int     rpg_tripmines;
	const char	*info;
	//const char	*info2;

	ent = &cent->currentState;
	if ( ent->weapon == WP_0 || ent->weapon == WP_1 ) {
		return;
	}
	if ( ent->weapon >= WP_NUM_WEAPONS ) {
		CG_Error( "CG_FireWeapon: ent->weapon >= WP_NUM_WEAPONS" );
		return;
	}
	weap = &cg_weapons[ ent->weapon ];

	// mark the entity as muzzle flashing, so when it is added it will
	// append the flash to the weapon model
	cent->muzzleFlashTime = cg.time;

	// lightning gun only does this this on initial press
	if (	ent->weapon == WP_5 /*||
			ent->weapon == WP_13*/
			|| ent->weapon == WP_14
			|| ent->weapon == WP_11 
			|| (!(cent->currentState.eFlags & EF_ALT_FIRING) && ent->weapon == WP_10 )
			|| (cent->currentState.eFlags & EF_ALT_FIRING && ent->weapon == WP_6 )
			)
	{
		if ( cent->pe.lightningFiring ) {
			return;
		}
	}

	// play quad sound if needed
/*	if ( cent->currentState.powerups & ( 1 << PW_QUAD ) ) {
		trap_S_StartSound (NULL, cent->currentState.number, CHAN_ITEM, cgs.media.quadSound );
	}*/

	// play a sound
	info = CG_ConfigString( CS_SERVERINFO );
	rpg_tripmines = atoi( Info_ValueForKey( info, "rpg_invisibletripmines" ) );
	rpg_effectsgun = atoi( Info_ValueForKey( info, "rpg_effectsgun" ) );
	if (alt_fire)
	{
		//RPG-X: RedTechie - Wrong place for show tris
		/*if( ent->weapon == WP_7 )
		{
			if(tris_state == 1)
				tris_state = 0;
			else
				tris_state = 1;

			trap_Cvar_Set("r_showtris", va("%i",tris_state));
		}*/
		if ( weap->altFlashSnd )
		{
			//TiM : Hark, I smell hackery again
			//admin alt hypos no fire coz it grinds my teeth
			if ( cgs.clientinfo[cg.snap->ps.clientNum].isAdmin/*cgs.clientinfo[cent->currentState.clientNum].pClass == PC_ADMIN*/ 
				&& 
				cent->currentState.weapon == WP_12 ) {
				return;
			}

			if(ent->weapon == WP_8){
				if(rpg_tripmines != 1){
					trap_S_StartSound( NULL, ent->number, CHAN_WEAPON, weap->altFlashSnd );
				}
			}else{
				trap_S_StartSound( NULL, ent->number, CHAN_WEAPON, weap->altFlashSnd );
			}
		}
	}
	else
	{
		if ( weap->flashSound )
		{
			if(ent->weapon == WP_8){
				if((rpg_effectsgun == 1) || (rpg_tripmines == 1)){
					return;
				}else{
					trap_S_StartSound( NULL, ent->number, CHAN_WEAPON, weap->flashSound );
				}
			}else{
				trap_S_StartSound( NULL, ent->number, CHAN_WEAPON, weap->flashSound );
			}
		}
	}
}

/*
================
CG_FireSeeker

Caused by an EV_FIRE_WEAPON event
================
*/
void CG_FireSeeker( centity_t *cent )
{
	entityState_t *ent;
	weaponInfo_t	*weap;

	ent = &cent->currentState;
	weap = &cg_weapons[ WP_4 ];

	trap_S_StartSound( NULL, ent->number, CHAN_WEAPON, weap->flashSound );
}

/*
=================
CG_MissileHitWall

Caused by an EV_MISSILE_MISS event, or directly by local bullet tracing
=================
*/
void CG_MissileHitWall( centity_t *cent, int weapon, vec3_t origin, vec3_t dir ) 
{
	qhandle_t		mod;
	qhandle_t		mark;
	qhandle_t		shader;
	sfxHandle_t		sfx;
	float			radius;
	float			light;
	vec3_t			lightColor;
	localEntity_t	*le;
	qboolean		isSprite;
	int				duration;
	qboolean		alphaFade;
//	weaponInfo_t	*weaponInfo = &cg_weapons[weapon];

	mark = 0;
	radius = 32;
	sfx = 0;
	mod = 0;
	shader = 0;
	light = 0;
	lightColor[0] = 1;
	lightColor[1] = 1;
	lightColor[2] = 0;

	// set defaults
	isSprite = qfalse;
	duration = 600;

	switch ( weapon ) {
	default:
	case WP_5:
		// no explosion at LG impact, it is added with the beam
		mark = cgs.media.holeMarkShader;
		radius = 12;
		break;	
	case WP_13:
		// no explosion at LG impact, it is added with the beam
		mark = cgs.media.holeMarkShader;
		radius = 12;
		break;
	case WP_8:
		FX_GrenadeExplode( origin, dir );
		return;
		break;
	case WP_10:
		FX_DisruptorWeaponHitWall( origin, dir, 2 ); //cent->currentState.time2
		return;
		break;
	case WP_1:
		/*mod = cgs.media.ringFlashModel;
		shader = cgs.media.imodExplosionShader;
		mark = cgs.media.energyMarkShader;
		radius = 24;*/
		break;
	case WP_6:
		//mod = cgs.media.ringFlashModel;
		//shader = cgs.media.imodExplosionShader;
		//mark = cgs.media.energyMarkShader;
		//radius = 24;
		FX_CompressionExplosion( cent->lerpOrigin, origin, dir, qfalse );
		return;
		break;
	case WP_7:
		//FX_TetrionAltHitWall( origin, dir );
		return;
		break;
/*	case WP_4:
		if (cent->currentState.eFlags & EF_ALT_FIRING)
		{
			FX_ScavengerAltExplode( origin, dir );
		}
		else
		{
			FX_ScavengerWeaponHitWall( origin, dir, qfalse );
		}
		return;
		break;*/
/*	case WP_11:
		if ( !( cent->currentState.eFlags & EF_ALT_FIRING ))
		{
			FX_BorgWeaponHitWall( origin, dir );
		}
		return;
		break;*/

	case WP_9:
		if ( cent->currentState.eFlags & EF_ALT_FIRING )
		{
			FX_QuantumAltHitWall( origin, dir );
		}
		else
		{
			FX_QuantumHitWall( origin, dir );
		}
		return;
		break;
	}

	if ( sfx ) {
		trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_AUTO, sfx );
	}

	//
	// create the explosion
	//
	if ( mod ) {
		le = CG_MakeExplosion( origin, dir, 
							   mod,	shader,
							   duration, 1, isSprite );
		le->light = light;
		VectorCopy( lightColor, le->lightColor );
	}

	//
	// impact mark
	//
	alphaFade = (mark == cgs.media.energyMarkShader);	// plasma fades alpha, all others fade color
	CG_ImpactMark( mark, origin, dir, random()*360, 1,1,1,1, alphaFade, radius, qfalse );
}


/*
=================
CG_MissileHitPlayer
=================
*/
void CG_MissileHitPlayer( centity_t *cent, int weapon, vec3_t origin, vec3_t dir)
{
	if (cent)
	{	// Showing blood is a no-no.

//		CG_Bleed( origin, cent->currentState.otherEntityNum );
	}

	CG_MissileHitWall( cent, weapon, origin, dir );
}


/*
=================
CG_BounceEffect

Caused by an EV_BOUNCE | EV_BOUNCE_HALF event
=================
*/

// big fixme. none of these sounds should be registered at runtime
void CG_BounceEffect( centity_t *cent, int weapon, vec3_t origin, vec3_t normal )
{
	int     rpg_tripmines;
	const char	*info;

	switch( weapon )
	{
	case WP_8:
		info = CG_ConfigString( CS_SERVERINFO );
		rpg_tripmines = atoi( Info_ValueForKey( info, "rpg_invisibletripmines" ) );
		if(rpg_tripmines != 1){
			if ( rand() & 1 ) {
				trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_AUTO, trap_S_RegisterSound(SOUND_DIR "glauncher/bounce1.wav") );
			} else {
				trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_AUTO, trap_S_RegisterSound(SOUND_DIR "glauncher/bounce2.wav") );
			}
		}
		break;

	case WP_7:
		//trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_AUTO, trap_S_RegisterSound ( va(SOUND_DIR "tetrion/ricochet%d.wav", irandom(1, 3)) ) );
		//FX_TetrionRicochet( origin, normal );	
		break;

	default:
		if ( rand() & 1 ) {
			trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_AUTO, trap_S_RegisterSound(SOUND_DIR "glauncher/bounce1.wav") );
		} else {
			trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_AUTO, trap_S_RegisterSound(SOUND_DIR "glauncher/bounce2.wav") );
		}
		break;
	}
}




/*
============================================================================

BULLETS

============================================================================
*/



/*
======================
CG_CalcMuzzlePoint
======================
*/

extern qboolean PM_PlayerCrouching ( int legsAnim );

qboolean	CG_CalcMuzzlePoint( centity_t *cent, vec3_t muzzle, qboolean isDecoy ) {
	vec3_t		forward;
	//centity_t	*cent;
	int			anim;

	/*if ( entityNum == cg.snap->ps.clientNum && !isDecoy ) {
		VectorCopy( cg.snap->ps.origin, muzzle );
		muzzle[2] += cg.snap->ps.viewheight;
		AngleVectors( cg.snap->ps.viewangles, forward, NULL, NULL );
		VectorMA( muzzle, 14, forward, muzzle );
		return qtrue;
	}*/

	//cent = &cg_entities[entityNum];
	if ( !cent->currentValid ) {
		return qfalse;
	}

	//if ( !isDecoy )
		VectorCopy( cent->currentState.pos.trBase, muzzle );
	//else
	//	VectorCopy( cent->currentState.origin, muzzle );

	AngleVectors( cent->currentState.apos.trBase, forward, NULL, NULL );
	anim = cent->currentState.legsAnim & ~ANIM_TOGGLEBIT;
	if ( PM_PlayerCrouching( cent->currentState.legsAnim ) ) {
		muzzle[2] += CROUCH_VIEWHEIGHT;
	} else {
		muzzle[2] += DEFAULT_VIEWHEIGHT;
	}

	VectorMA( muzzle, 14, forward, muzzle );

	return qtrue;

}


/*
================
CG_SurfaceExplosion

Adds an explosion to a surface
================
*/

#define NUM_SPARKS		12
#define NUM_PUFFS		1
#define NUM_EXPLOSIONS	4

void CG_SurfaceExplosion( vec3_t origin, vec3_t normal, float radius, float shake_speed, qboolean smoke )
{
	localEntity_t	*le;
	vec3_t			direction, new_org;
	vec3_t			sprayvel, velocity		= { 0, 0, 0 };
	vec3_t			temp_org, temp_vel;
	float			scale, dscale;
	int				i, numSparks;

	//Sparks

	numSparks = 32 + (random() * 16.0f);
	
	//VectorSet( normal, 0, 0, 1 );

	for ( i = 0; i < numSparks; i++ )
	{	
		scale = 0.25f + (random() * 2.0f);
		dscale = -scale*0.5;

		FXE_Spray( normal, 500, 150, 1.0f, sprayvel);

		FX_AddTrail( origin,
								sprayvel,
								qtrue,
								32.0f,
								-64.0f,
								scale,
								-scale,
								1.0f,
								0.0f,
								0.25f,
								4000.0f,
								cgs.media.sparkShader);
	}

	//Smoke

	//Move this out a little from the impact surface
	VectorMA( origin, 4, normal, new_org );
	VectorSet( velocity, 0.0f, 0.0f, 16.0f );

	for ( i = 0; i < 4; i++ )
	{
		VectorSet( temp_org, new_org[0] + (crandom() * 16.0f), new_org[1] + (crandom() * 16.0f), new_org[2] + (random() * 4.0f) );
		VectorSet( temp_vel, velocity[0] + (crandom() * 8.0f), velocity[1] + (crandom() * 8.0f), velocity[2] + (crandom() * 8.0f) );

		FX_AddSprite(	temp_org,
						temp_vel, 
						qfalse, 
						radius /**96.0f*/ + (random() * 12.0f), 
						16.0f, 
						1.0f, 
						0.0f,
						20.0f + (crandom() * 90.0f),
						0.5f,
						2000.0f, 
						cgs.media.smokeShader);
	}

	//Core of the explosion

	//Orient the explosions to face the camera
	VectorSubtract( cg.refdef.vieworg, origin, direction );
	VectorNormalize( direction );

	//Tag the last one with a light
	le = CG_MakeExplosion2( origin, direction, cgs.media.explosionModel, 5, cgs.media.surfaceExplosionShader, 
							500, qfalse, radius * 0.02f + (random() * 0.3f), LEF_NONE);
	le->light = 150;
	VectorSet( le->lightColor, 0.9f, 0.8f, 0.5f );

	for ( i = 0; i < NUM_EXPLOSIONS-1; i ++)
	{
		VectorSet( new_org, (origin[0] + (32 + (crandom() * 8))*crandom()), (origin[1] + (32 + (crandom() * 8))*crandom()), (origin[2] + (32 + (crandom() * 8))*crandom()) );
		le = CG_MakeExplosion2( new_org, direction, cgs.media.explosionModel, 5, cgs.media.surfaceExplosionShader, 
								300 + (rand() & 99), qfalse, radius * 0.05f + (crandom() *0.3f), LEF_NONE);
	}

	//Shake the camera
	CG_ExplosionEffects( origin, shake_speed, 350 );

}

void CG_PlayShooterSound(centity_t *cent) {
	weaponInfo_t *weap;

	weap = &cg_weapons[cent->currentState.eventParm];
	
	switch(cent->currentState.eventParm) {
		case WP_6:
		case WP_8:
		case WP_9:
			trap_S_StartSound(cent->currentState.origin, cent->currentState.number, CHAN_VOICE, weap->flashSound);
			break;
		case WP_10:
			trap_S_StartSound(cent->currentState.origin, cent->currentState.number, CHAN_VOICE, weap->altFlashSnd);
			break;
	}
}
