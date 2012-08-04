/* Copyright (C) 1999-2000 Id Software, Inc.
 *
 * bg_misc.c -- both games misc functions, all completely stateless
 */

#include "q_shared.h"
#include "bg_public.h"

int		trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode );
void	trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );
void	trap_FS_FCloseFile( fileHandle_t f );
void	trap_FS_Read( void *buffer, int len, fileHandle_t f );

/* If you change these:  PLEASE CHANGE THE COMMENTS ON THE AMMO PICKUPS, WHICH DETAIL THE QUANTITY IN THE CLIP*/
#define AMMO_PHASER_CLIP		50
#define AMMO_COMPRESSION_CLIP	32
#define AMMO_IMOD_CLIP			15
#define AMMO_SCAVENGER_CLIP		30
#define AMMO_STASIS_CLIP		15
#define AMMO_GRENADE_CLIP		10
#define AMMO_TETRION_CLIP		40
#define AMMO_QUANTUM_CLIP		6
#define AMMO_DREADNOUGHT_CLIP	40

char	races[256];

/*
 * TiM : Tidied up for programmer easability... O_o
 * Marcin: not used since 30/12/2008*/
/**
*	Max ammo for each weapon. Unused.
*/
int Max_Ammo[WP_NUM_WEAPONS] =
{
	0,						// WP_0,

	5,						// WP_1

	5,						// WP_2,
	5,						// WP_3,
	5,						// WP_4,	

	5,						// WP_5,	!! this should match PHASER_AMMO_MAX defined in bg_public
	5,						// WP_6,
	5,						// WP_7,
	5,						// WP_8,
	5,						// WP_9,
	5,						// WP_10,			
	
	5,						// WP_11,
	5,						// WP_12,
	5,						// WP_13		

	5,						// WP_14,
	5,						// WP_NEUTRINO_PROBE,

	//64					// WP_7
};

/*QUAKED item_***** ( 0 0 0 ) (-16 -16 -16) (16 16 16) suspended
DO NOT USE THIS CLASS, IT JUST HOLDS GENERAL INFORMATION.
The suspended flag will allow items to hang in the air, otherwise they are dropped to the next surface.

If an item is the target of another entity, it will not spawn in until fired.

An item fires all of its targets when it is picked up.  If the toucher can't carry it, the targets won't be fired.

"notfree" if set to 1, don't spawn in free for all games
"notteam" if set to 1, don't spawn in team games
"notsingle" if set to 1, don't spawn in single player games
"wait"	override the default wait before respawning.  -1 = never respawn automatically, which can be used with targeted spawning.
"random" random number of plus or minus seconds varied from the respawn time
"count" override quantity or duration on most items.
*/

gitem_t	bg_itemlist[] = 
{
	{
		NULL,							/* char			*classname;	*/
		NULL,							/* char			*pickup_sound;*/
		NULL,							/* char			*world_model;*/
		NULL,							/* char			*view_model;*/
		NULL,							/* char			*icon;*/
		NULL,							/* char			*pickup_name; */	/* for printing on pickup */
		0,								/* int			quantity; */		/* for ammo how much, or duration of powerup */
		0,								/* itemType_t  	giType; */			/* IT_* flags */
		0,								/* int			giTag; */
		"",								/* char			*precaches;	*/		/* string of all models and images this item will use */
		""								/* char			*sounds; */			/* string of all sounds this item will use */
	},	/* leave index 0 alone */

	/*
	 *  WEAPONS
	 */

/* WP_1 */
/*QUAKED weapon_imod (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_null_hand", 
		"sound/silence.wav", 				/* "sound/weapons/w_pkup.wav", */
        "models/weapons2/hand/hand_w.md3",  /* "models/weapons2/imod/imod2_w.md3", */ /* world  */
		"models/weapons2/hand/hand.md3",	/* "models/weapons2/imod/imod2.md3", */   /* view   */
		"icons/w_icon_hand", 														  /* icon   */
		" ",																		  /* pickup */
		AMMO_IMOD_CLIP,
		IT_WEAPON,
		WP_1,
		"",																			  /* precache */
		""																			  /* sounds */
	},


/*QUAKED weapon_tricorder (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_tricorder", 
		"sound/weapons/w_pkup.wav",
        "models/weapons2/tricorder/tricorder_w.md3", /* world  */
		"models/weapons2/tricorder/tricorder.md3",	 /* view   */
		"icons/w_icon_tricorder",					 /* icon   */
		"Tricorder",								 /* pickup */
		AMMO_PHASER_CLIP,
		IT_WEAPON,
		WP_2,
		"",											 /* precache */
		""											 /* sounds   */
	},

/*QUAKED weapon_padd (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_padd", 
		"sound/weapons/w_pkup.wav",
        "models/weapons2/padd/padd_w.md3",	/* world  */
		"models/weapons2/padd/padd.md3",	/* view   */
		"icons/w_icon_padd",				/* icon   */
		"Padd",								/* pickup */
		AMMO_PHASER_CLIP,
		IT_WEAPON,
		WP_3,
		"",									/* precache */
		"",									/* sounds    */
	},

/*QUAKED weapon_scavenger (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_coffee", 
		"sound/weapons/w_pkup.wav",
        "models/weapons2/coffeecup/coffee_cup_w.md3", /* world  */
		"models/weapons2/coffeecup/coffee_cup.md3",	  /* view   */
		"icons/w_icon_coffee",						  /* icon   */
		"Coffee, Black",							  /* pickup */
		AMMO_SCAVENGER_CLIP,
		IT_WEAPON,
		WP_4,
		"",											  /* precache */
		""											  /* sounds   */
	},

/*QUAKED weapon_phaser (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_phaser", 
		"sound/weapons/w_pkup.wav",
        "models/weapons2/phaser/phaser_w.md3",	/* world  */
		"models/weapons2/phaser/phaser.md3",	/* view   */
		"icons/w_icon_phaser",					/* icon   */
		"Phaser",								/* pickup */
		AMMO_PHASER_CLIP,
		IT_WEAPON,
		WP_5,
		"",										/* precache */
		""										/* sounds   */
	},

/*QUAKED weapon_compressionrifle (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_compressionrifle", 
		"sound/weapons/w_pkup.wav",
        "models/weapons2/prifle/prifle_w.md3", //world
		"models/weapons2/prifle/prifle.md3",	//view
/* icon */		"icons/w_icon_rifle",
/* pickup */	"Phaser Compression Rifle",
		AMMO_COMPRESSION_CLIP,
		IT_WEAPON,
		WP_6,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED weapon_tetriondisruptor (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_tr116", 
		"sound/weapons/w_pkup.wav",
        "models/weapons2/tr116/tr-116_w.md3",//world
		"models/weapons2/tr116/tr-116.md3",	//view
/* icon */		"icons/w_icon_tr116",
/* pickup */	"TR-116",
		AMMO_TETRION_CLIP,
		IT_WEAPON,
		WP_7,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED weapon_grenadelauncher (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_grenadelauncher",
		"sound/weapons/w_pkup.wav",
        "models/weapons2/launcher/launcher_w.md3",	//world
		"models/weapons2/launcher/launcher.md3",	//view
/* icon */		"icons/w_icon_grenade",
/* pickup */	"Compound Grenade Launcher",
		AMMO_GRENADE_CLIP,
		IT_WEAPON,
		WP_8,
/* precache */ "",
/* sounds */ "sound/weapons/glauncher/bounce1.wav sound/weapons/glauncher/bounce2.wav"
	},

/*QUAKED weapon_quantumburst (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_quantumburst", 
		"sound/weapons/w_pkup.wav",
        "models/weapons2/q_burst/q_burst_w.md3",	//world
		"models/weapons2/q_burst/q_burst.md3",	//view
/* icon */		"icons/w_icon_quantum",
/* pickup */	"Photon Burst",
		AMMO_QUANTUM_CLIP,
		IT_WEAPON,
		WP_9,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED weapon_stasisweapon (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_disruptor",
		"sound/weapons/w_pkup.wav",
        "models/weapons2/alien_disruptor/disruptor_w.md3",	//world
		"models/weapons2/alien_disruptor/disruptor.md3",	//view
/* icon */		"icons/w_icon_disruptor",
/* pickup */	"Disruptor",
		AMMO_STASIS_CLIP,
		IT_WEAPON,
		WP_10,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED weapon_borg_weapon (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_medkit", 
		"sound/weapons/w_pkup.wav",
        "models/weapons2/medkit/medkit_w.md3",	//world
		"models/weapons2/medkit/medkit.md3",	//view
/* icon */		"icons/w_icon_medkit",
/* pickup */	"Medkit",
		AMMO_PHASER_CLIP,
		IT_WEAPON,
		WP_11,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED weapon_voyager_hypo (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_voyager_hypo", 
		"sound/weapons/w_pkup.wav",
		"models/weapons2/hypospray/hypospray_w.md3",	//world
		"models/weapons2/hypospray/hypospray.md3", //view
/* icon */		"icons/w_icon_hypo",
/* pickup */	"Hypo",
		AMMO_PHASER_CLIP,
		IT_WEAPON,
		WP_12,
/* precache */ "",
/* sounds */ ""
	},  

/*QUAKED weapon_dreadnought (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_dermal_regen", 
		"sound/weapons/w_pkup.wav",
        "models/weapons2/dermal_regen/dermal_regen_w.md3",
		"models/weapons2/dermal_regen/dermal_regen.md3",
/* icon */		"icons/w_icon_dermalregen",
/* pickup */	"Dermal Regenerator",
		AMMO_DREADNOUGHT_CLIP,
		IT_WEAPON,
		WP_13,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED weapon_borg_assimilator (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_toolkit", 
		"sound/weapons/w_pkup.wav",
        "models/weapons2/toolkit/toolkit_w.md3",	//world
		"models/weapons2/toolkit/toolkit.md3",	//view
/* icon */		"icons/w_icon_toolkit",
/* pickup */	"Toolkit",
		AMMO_PHASER_CLIP,
		IT_WEAPON,
		WP_14,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED weapon_engtool (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_hyperspanner", 
		"sound/weapons/w_pkup.wav",
        "models/weapons2/hyperspanner/hyperspanner_w.md3",	//world
		"models/weapons2/hyperspanner/hyperspanner.md3",	//view
/* icon */		"icons/w_icon_hyperspanner",
/* pickup */	"Hyperspanner",
		AMMO_PHASER_CLIP,
		IT_WEAPON,
		WP_15,
/* precache */ "",
/* sounds */ ""
	},

	//
	// AMMO ITEMS
	//

/*QUAKED ammo_compressionrifle (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
32 ammo for the compression rifle
*/
	{
		"ammo_compressionrifle",
		"sound/player/pickupenergy.wav",
        "models/powerups/trek/prifle_ammo.md3",	//world
		NULL,
/* icon */		"icons/dm_phaser_sm",
/* pickup */	"Phaser Compression Rifle Ammo",
		AMMO_COMPRESSION_CLIP,
		IT_AMMO,
		WP_6,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED ammo_imod (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
15 ammo for the I-MOD
*/
	{
		"ammo_imod",
		"sound/player/pickupenergy.wav",
        "models/powerups/trek/imod_ammo.md3",	//world
		NULL,
/* icon */		"icons/dm_imod",
/* pickup */	"I-MOD Ammo",
		AMMO_IMOD_CLIP,
		IT_AMMO,
		WP_1,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED ammo_scavenger (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
30 ammo for the scavenger rifle
*/
	{
		"ammo_scavenger",
		"sound/player/pickupenergy.wav",
        "models/powerups/trek/scavenger_ammo.md3",	//world
		NULL,
/* icon */		"icons/dm_scav",
/* pickup */	"Scavenger Weapon Ammo",
		AMMO_SCAVENGER_CLIP,
		IT_AMMO,
		WP_4,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED ammo_stasis (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
15 ammo for the stasis weapon
*/
	{
		"ammo_stasis",
		"sound/player/pickupenergy.wav",
        "models/powerups/trek/stasis_ammo.md3",	//world
		NULL,
/* icon */		"icons/dm_stasis_sm",
/* pickup */	"Stasis Weapon Ammo",
		AMMO_STASIS_CLIP,
		IT_AMMO,
		WP_10,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED ammo_grenades (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
10 ammo for the grenade launcher
*/
	{
		"ammo_grenades",
		"sound/player/pickupenergy.wav",
        "models/powerups/trek/glauncher_ammo.md3",	//world
		NULL,
/* icon */		"icons/dm_glauncher_sm",
/* pickup */	"Compound Grenade Launcher Ammo",
		AMMO_GRENADE_CLIP,
		IT_AMMO,
		WP_8,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED ammo_tetriondisruptor (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
40 ammo for the tetrYon disruptor
*/
	{
		"ammo_tetriondisruptor",
		"sound/player/pickupenergy.wav",
        "models/powerups/trek/tetrion_ammo.md3",	//world
		NULL,
/* icon */		"icons/dm_tetrion_sm",
/* pickup */	"Tetryon Pulse Disruptor Ammo",
		AMMO_TETRION_CLIP,
		IT_AMMO,
		WP_7,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED ammo_quantumburst (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
6 ammo for the quantum burst weapon
*/
	{
		"ammo_quantumburst",
		"sound/player/pickupenergy.wav",
        "models/powerups/trek/torpedo.md3",	//world
		NULL,
/* icon */		"icons/dm_torpedo_sm",
/* pickup */	"Photon Burst Ammo",
		AMMO_QUANTUM_CLIP,
		IT_AMMO,
		WP_9,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED ammo_dreadnought (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
40 ammo for the dreadnought/arc welder
*/
	{
		"ammo_dreadnought",
		"sound/player/pickupenergy.wav",
        "models/powerups/trek/arc_ammo.md3",	//world
		NULL,
/* icon */		"icons/dm_a_arc_sm",
/* pickup */	"Dermal Regenerator Ammo",
		AMMO_DREADNOUGHT_CLIP,
		IT_AMMO,
		WP_13,
/* precache */ "",
/* sounds */ ""
	},

	//
	// ARMOR
	//

/*QUAKED item_armor_shard (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
5 points of shields
*/
	{
		"item_armor_shard", 
		"sound/player/pickupenergy.wav",
        "models/powerups/trek/armor_shard.md3",	//world
		NULL,
/* icon */		"icons/icon_shards",
/* pickup */	"Incremental Shield Boost",
		5,
		IT_ARMOR,
		0,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED item_armor_combat (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
50 points of shields
*/
	{
		"item_armor_combat", 
		"sound/player/pickupenergy.wav",
        "models/powerups/trek/armor.md3",	//world
		NULL,
/* icon */		"icons/dm_armor_sm",
/* pickup */	"Personal Deflector Screen",
		50,
		IT_ARMOR,
		0,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED item_armor_body (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
100 points of shields
*/
	{
		"item_armor_body", 
		"sound/player/suitenergy.wav",
        "models/powerups/trek/armor2.md3",	//world
		NULL,
/* icon */		"icons/dm_superarmor_sm",
/* pickup */	"Isokinetic Deflector Screen",
		100,
		IT_ARMOR,
		0,
/* precache */ "",
/* sounds */ ""
	},

	//
	// HEALTH
	//

/*QUAKED item_hypo_small (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
5 points of health, max of 200
*/
	{
		"item_hypo_small",
		"sound/player/pickuphealth.wav",
        "models/powerups/trek/hypo_single.md3",	//world
		NULL,
/* icon */		"icons/dm_health_sm",
/* pickup */	"Booster Hypospray",
		5,
		IT_HEALTH,
		0,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED item_hypo (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
25 points of health, max of 100
*/
	{
		"item_hypo",
		"sound/player/suithealth.wav",
        "models/powerups/trek/hypo_double.md3",	//world
		NULL,
/* icon */		"icons/dm_health2_sm",
/* pickup */	"Emergency Hypospray",
		25,
		IT_HEALTH,
		0,
/* precache */ "",
/* sounds */ ""
	},

	//
	// HOLDABLE ITEMS
	//

/*QUAKED holdable_transporter (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
pick it up and it stays in your inventory until used, at which time you drop it in front of you and it still
kind of resides in your inventory. when you use it _again_ it activates and anyone can walk through the transporter.
*/
	{
		"holdable_transporter", 
		"sound/items/holdable.wav",
        "models/powerups/trek/transporter.md3",	//world
		NULL,
/* icon */		"icons/dm_transport_sm",
/* pickup */	"Personal Transporter Device",
		60,
		IT_HOLDABLE,
		HI_TRANSPORTER,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED holdable_medkit (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
pick it up and it stays in your inventory until used, at which time it sets your health to 100
*/
	{
		"holdable_medkit", 
		"sound/items/holdable.wav",
        "models/powerups/trek/med_kit.md3", //world
		NULL,
/* icon */		"icons/dm_health3_sm",
/* pickup */	"Portable Medkit",
		60,
		IT_HOLDABLE,
		HI_MEDKIT,
/* precache */ "",
/* sounds */ "sound/items/use_medkit.wav"
	},

	//
	// POWERUP ITEMS
	//

/*QUAKED item_haste (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
for 30 seconds you run at %150 of your normal speed and your firing delays are 3/4 as long
*/
	{
		"item_haste",
		"sound/items/haste.wav",
        "models/powerups/trek/haste.md3",	//world
		NULL,
/* icon */		"icons/dm_haste",
/* pickup */	"Temporal Accelerator",
		30,
		IT_POWERUP,
		PW_HASTE,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED item_invis (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
20 seconds of invisibility
*/
	{
		"item_invis",
		"sound/items/invisibility.wav",
        "models/powerups/trek/invisible.md3",	//world
		NULL,
/* icon */		"icons/dm_invisibility",
/* pickup */	"Personal Cloaking Device",
		20,
		IT_POWERUP,
		PW_INVIS,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED item_flight (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
30 seconds of flight
*/
	{
		"item_flight",
		"sound/items/flight.wav",
        "models/powerups/trek/flight.md3",	//world
		NULL,
/* icon */		"icons/dm_flight",
/* pickup */	"Anti-Gravity Pack",
		30,
		IT_POWERUP,
		PW_FLIGHT,
/* precache */ "",
/* sounds */ "sound/items/flight.wav"
	},

/*QUAKED holdable_detpack (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
BLAMMO!
*/
	{
		"holdable_detpack", 
		"sound/player/pickupenergy.wav",
        "models/powerups/trek/detpak.md3",	//world
		NULL,
/* icon */		"icons/icon_detpack",
/* pickup */	"Ultritium Explosive Charge",
		1,		// 5,
		IT_HOLDABLE,
		HI_DETPACK,
/* precache */ "",
/* sounds */ "sound/weapons/detpacklatch.wav sound/weapons/explosions/detpakexplode.wav"
	},

/*QUAKED holdable_shield (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
About 25 seconds or 250 hit points of a portashield.
*/
	{
		"holdable_shield", 
		"sound/player/pickupenergy.wav",
        "models/powerups/trek/shield_gen.md3",	//world
		NULL,
/* icon */		"icons/icon_shield",
/* pickup */	"Portable Force Field",
		1,
		IT_HOLDABLE,
		HI_SHIELD,
/* precache */ "",
/* sounds */ "sound/weapons/detpacklatch.wav sound/movers/forceup.wav sound/ambience/spark5.wav"
	},	


/*QUAKED Holographic_decoy (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
About 1 minute of a holographic decoy.
*/
	{
		"Holographic_decoy", 
		"sound/items/holdable.wav",
        "models/powerups/trek/decoy.md3",	//world
		NULL,
/* icon */		"icons/icon_decoy",
/* pickup */	"Holographic Decoy",
		1,
		IT_HOLDABLE,
		HI_DECOY,
/* precache */ "",
/* sounds */ ""
	},	

	//
	// New Weapons
	//

	// end of list marker
	{NULL}
};

int		bg_numItems = sizeof(bg_itemlist) / sizeof(bg_itemlist[0]) - 1;

#define		STAND_VIEWHEIGHT		(DEFAULT_VIEWHEIGHT)
#define		SITTING_VIEWHEIGHT		(22) /* TiM: To reduce redundancy here */
#define		CROUCHING_VIEWHEIGHT	(CROUCH_VIEWHEIGHT)
#define		HITBOX_DEFAULT			32
#define		HITBOX_CROUCH			16
#define		HITBOX_NULL				-23 /*0 //-24 doesn't work on patch meshes apparently. That might be more the mapper's fault than mine tho lol */
#define		NULL_ANIM				-1

/*
 * !Main emotes definition arrayzor
 * Suffice it to say... my hands hurt after writing this thing >.<
 */
emoteList_t	bg_emoteList[] = {
	//name					//type				//enumName					//enumLoop				//viewHeight			//hitBox Height			//bodyFlags			//animFlags
	{ "alert",				TYPE_MISC,			-1,							NULL_ANIM,				0,						0,						0,					0,																					},
	{ "alert2",				TYPE_MISC,			-1,							NULL_ANIM,				0,						0,						0,					0,																					},
	{ "assimilated",		TYPE_FULLBODY,		BOTH_ASSIMILATED1,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_ALL	 | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "",					TYPE_NONE,			-1,							NULL_ANIM,				0,						0,						0,					0,																					},
	{ "benchsit1_2stand",	TYPE_SITTING,		BOTH_BENCHSIT1_2STAND,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "benchsit1_fixboot",	TYPE_SITTING,		BOTH_BENCHSIT1_FIXBOOT,		BOTH_BENCHSIT1_IDLE,	SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_REVERTLOOP_BOTH						},
	{ "benchsit1_idle",		TYPE_SITTING,		BOTH_BENCHSIT1_IDLE,		NULL_ANIM,				SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_LOOP_BOTH | EMOTE_OVERRIDE_BOTH							},
	{ "benchsit1to2",		TYPE_SITTING,		BOTH_BENCHSIT1TO2,			BOTH_BENCHSIT2_IDLE,	SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "benchsit2_idle",		TYPE_SITTING,		BOTH_BENCHSIT2_IDLE,		NULL_ANIM,				SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_LOOP_BOTH | EMOTE_OVERRIDE_BOTH							},
	{ "benchsit2to1",		TYPE_SITTING,		BOTH_BENCHSIT2TO1,			BOTH_BENCHSIT1_IDLE,	SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "catch1",				TYPE_FULLBODY,		BOTH_CATCH1,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "console1",			TYPE_CONSOLE,		BOTH_CONSOLE1,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER | EMOTE_LOOP_BOTH							},
	{ "console1_idle",		TYPE_CONSOLE,		BOTH_CONSOLE1IDLE,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "console1_left",		TYPE_CONSOLE,		BOTH_CONSOLE1LEFT,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER | EMOTE_LOOP_BOTH							},
	{ "console1_right",		TYPE_CONSOLE,		BOTH_CONSOLE1RIGHT,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER | EMOTE_LOOP_BOTH							},
	{ "console2",			TYPE_CONSOLE,		BOTH_CONSOLE2,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_LOOP_BOTH | EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH	},
	{ "console3",			TYPE_CONSOLE,		BOTH_CONSOLE3,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER	| EMOTE_LOOP_BOTH							},
	{ "console3_idle",		TYPE_CONSOLE,		BOTH_CONSOLE3IDLE,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER	| EMOTE_LOOP_BOTH							},
	{ "console3_left",		TYPE_CONSOLE,		BOTH_CONSOLE3LEFT,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER	| EMOTE_LOOP_BOTH							},
	{ "console3_right",		TYPE_CONSOLE,		BOTH_CONSOLE3RIGHT,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER	| EMOTE_LOOP_BOTH							},
	{ "console4",			TYPE_CONSOLE,		BOTH_CONSOLE4,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER	| EMOTE_LOOP_BOTH							},
	{ "console5",			TYPE_CONSOLE,		BOTH_CONSOLE5,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER	| EMOTE_LOOP_BOTH							},
	{ "couchsit1_2stand",	TYPE_SITTING,		BOTH_COUCHSIT1_2STAND1,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "couchsit1_left",		TYPE_SITTING,		BOTH_COUCHSIT1_GESTURELEFT,	BOTH_COUCHSIT1_IDLE,	SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER	| EMOTE_REVERTLOOP_BOTH						},
	{ "couchsit1_right",	TYPE_SITTING,		BOTH_COUCHSIT1_GESTURERIGHT,BOTH_COUCHSIT1_IDLE,	SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER	| EMOTE_REVERTLOOP_BOTH						},
	{ "couchsit1_idle",		TYPE_SITTING,		BOTH_COUCHSIT1_IDLE,		NULL_ANIM,				SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "couchsit1_talk",		TYPE_SITTING,		BOTH_COUCHSIT1_TALKGESTURE, BOTH_COUCHSIT1_IDLE,	SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER	| EMOTE_REVERTLOOP_BOTH						},
	{ "couchsit2to1",		TYPE_SITTING,		BOTH_COUCHSIT1_TO2,			BOTH_COUCHSIT1_IDLE,	SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH	| EMOTE_REVERTLOOP_UPPER					},
	{ "couchsit2_idle",		TYPE_SITTING,		BOTH_COUCHSIT2,				NULL_ANIM,				SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "coverup1_end",		TYPE_FULLBODY,		BOTH_COVERUP1_END,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER												},
	{ "coverup1_loop",		TYPE_FULLBODY,		BOTH_COVERUP1_LOOP,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER	| EMOTE_LOOP_BOTH							},
	{ "coverup1_start",		TYPE_FULLBODY,		BOTH_COVERUP1_START,		BOTH_COVERUP1_LOOP,		STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER												},
	{ "cowar1",				TYPE_FULLBODY,		BOTH_COWAR1,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH						},
	{ "crowdlook1",			TYPE_FULLBODY,		BOTH_CROWDLOOK1,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER	| EMOTE_REVERTLOOP_BOTH						},
	{ "crowdlook2",			TYPE_FULLBODY,		BOTH_CROWDLOOK2,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER	| EMOTE_REVERTLOOP_BOTH						},
	{ "crowdlook3",			TYPE_MISC,			BOTH_CROWDLOOK3,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH | EMOTE_LOOP_BOTH						},
	{ "crowdlook4",			TYPE_MISC,			BOTH_CROWDLOOK4,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "dive1",				TYPE_FULLBODY,		BOTH_DIVE1,					NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_ALL	 | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "eyes_shut",			TYPE_MISC,			-1,							0,						0,						0,						0,					0,																					},
	{ "eyes_angry",			TYPE_MISC,			-1,							0,						0,						0,						0,					0,																					},
	{ "gesture2",			TYPE_GESTURE,		BOTH_GESTURE2,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "gesture3",			TYPE_GESTURE,		BOTH_GESTURE3,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "get_up1",			TYPE_FULLBODY,		BOTH_GET_UP1,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "grab1",				TYPE_FULLBODY,		BOTH_GRAB1,					BOTH_GRAB2,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "grab2",				TYPE_FULLBODY,		BOTH_GRAB2,					NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "grab3",				TYPE_FULLBODY,		BOTH_GRAB3,					NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER												},
	{ "grab4",				TYPE_FULLBODY,		BOTH_GRAB4,					NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER | EMOTE_LOOP_BOTH							},
	{ "grabbed1",			TYPE_FULLBODY,		BOTH_GRABBED1,				BOTH_GRABBED2,			STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "grabbed2",			TYPE_FULLBODY,		BOTH_GRABBED2,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "groundshake1",		TYPE_FULLBODY,		BOTH_GROUNDSHAKE1,			BOTH_GROUNDSHAKE1LOOP,	STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "groundshake1loop",	TYPE_FULLBODY,		BOTH_GROUNDSHAKE1LOOP,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "groundshake2",		TYPE_FULLBODY,		BOTH_GROUNDSHAKE2,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "guard_idle1",		TYPE_MISC,			BOTH_GUARD_IDLE1,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "guard_lkrt1",		TYPE_MISC,			BOTH_GUARD_LKRT1,			BOTH_GUARD_IDLE1,		STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "guard_lookaround1",	TYPE_MISC,			BOTH_GUARD_LOOKAROUND1,		BOTH_GUARD_IDLE1,		STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "guilt1",				TYPE_FULLBODY,		BOTH_GUILT1,				NULL_ANIM,				CROUCHING_VIEWHEIGHT,	HITBOX_CROUCH,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "hitwall1",			TYPE_FULLBODY,		BOTH_HITWALL1,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "help1",				TYPE_FULLBODY,		BOTH_HELP1,					NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "injured1",			TYPE_INJURED,		BOTH_INJURED1,				NULL_ANIM,				DEAD_VIEWHEIGHT,		HITBOX_CROUCH,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH,							},
	{ "injured2",			TYPE_INJURED,		BOTH_INJURED2,				NULL_ANIM,				DEAD_VIEWHEIGHT,		HITBOX_CROUCH,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH,							},
	{ "injured3",			TYPE_INJURED,		BOTH_INJURED3,				NULL_ANIM,				DEAD_VIEWHEIGHT,		HITBOX_CROUCH,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH,							},
	{ "injured4",			TYPE_INJURED,		BOTH_INJURED4,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH,							},
	{ "injured4to5",		TYPE_INJURED,		BOTH_INJURED4TO5,			BOTH_INJURED5,			STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "injured5",			TYPE_INJURED,		BOTH_INJURED5,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "injured6",			TYPE_INJURED,		BOTH_INJURED6,				NULL_ANIM,				DEAD_VIEWHEIGHT,		HITBOX_CROUCH,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "injured6_combadge",	TYPE_INJURED,		BOTH_INJURED6COMBADGE,		BOTH_INJURED6,			DEAD_VIEWHEIGHT,		HITBOX_CROUCH,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "injured6_point",		TYPE_INJURED,		BOTH_INJURED6POINT,			BOTH_INJURED6,			DEAD_VIEWHEIGHT,		HITBOX_CROUCH,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "kneel_hand1",		TYPE_MISC,			BOTH_KNEELHAND1,			NULL_ANIM,				CROUCHING_VIEWHEIGHT,	HITBOX_CROUCH,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "laugh1",				TYPE_FULLBODY,		BOTH_LAUGH2,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER												},
	{ "laugh2",				TYPE_FULLBODY,		BOTH_LAUGH1,				NULL_ANIM,				SITTING_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "lean1",				TYPE_MISC,			BOTH_LEAN1,					NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "pain2writhe1",		TYPE_FULLBODY,		BOTH_PAIN2WRITHE1,			BOTH_WRITHING1,			STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "possessed1",			TYPE_FULLBODY,		BOTH_POSSESSED1,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER	| EMOTE_REVERTLOOP_BOTH						},
	{ "possessed2",			TYPE_FULLBODY,		BOTH_POSSESSED2,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER | EMOTE_LOOP_BOTH							},
	{ "psychicshock1",		TYPE_FULLBODY,		BOTH_PSYCHICSHOCK1,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "psychicshock2",		TYPE_FULLBODY,		BOTH_PSYCHICSHOCK2,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "scared2",			TYPE_FULLBODY,		BOTH_SCARED2,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER												},
	{ "shield1",			TYPE_FULLBODY,		BOTH_SHIELD1,				BOTH_SHIELD2,			STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER												},
	{ "shield2",			TYPE_FULLBODY,		BOTH_SHIELD2,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER | EMOTE_LOOP_BOTH							},
	{ "sit1stand",			TYPE_SITTING,		BOTH_SIT1STAND,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_BOTH																	},
	{ "sit1to2",			TYPE_SITTING,		BOTH_SIT1TO2,				BOTH_SIT2,				SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "sit1to3",			TYPE_SITTING,		BOTH_SIT1TO3,				BOTH_SIT3,				SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "sit1",				TYPE_SITTING,		BOTH_SIT1,					NULL_ANIM,				SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "sit2to1",			TYPE_SITTING,		BOTH_SIT2TO1,				BOTH_SIT1,				SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "sit2to3",			TYPE_SITTING,		BOTH_SIT2TO3,				BOTH_SIT3,				SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "sit2",				TYPE_SITTING,		BOTH_SIT2,					NULL_ANIM,				SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "sit3to1",			TYPE_SITTING,		BOTH_SIT3TO1,				BOTH_SIT1,				SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "sit3to2",			TYPE_SITTING,		BOTH_SIT3TO2,				BOTH_SIT2,				SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "sit3",				TYPE_SITTING,		BOTH_SIT3,					NULL_ANIM,				SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "sit4to5",			TYPE_SITTING,		BOTH_SIT4TO5,				BOTH_SIT5,				SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "sit4to6",			TYPE_SITTING,		BOTH_SIT4TO6,				BOTH_SIT6,				SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "sit4",				TYPE_SITTING,		BOTH_SIT4,					NULL_ANIM,				SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "sit5to4",			TYPE_SITTING,		BOTH_SIT5TO4,				BOTH_SIT4,				SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "sit5to6",			TYPE_SITTING,		BOTH_SIT5TO6,				BOTH_SIT6,				SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "sit5",				TYPE_SITTING,		BOTH_SIT5,					NULL_ANIM,				SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "sit6to4",			TYPE_SITTING,		BOTH_SIT6TO4,				BOTH_SIT4,				SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "sit6to5",			TYPE_SITTING,		BOTH_SIT6TO5,				BOTH_SIT5,				SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "sit6",				TYPE_SITTING,		BOTH_SIT6,					NULL_ANIM,				SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "sit7",				TYPE_SITTING,		BOTH_SIT7,					NULL_ANIM,				DEAD_VIEWHEIGHT,		HITBOX_CROUCH,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_LOOP_BOTH,													},
	{ "sit7tostand1",		TYPE_SITTING,		BOTH_SIT7TOSTAND1,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "sleep1",				TYPE_MISC,			BOTH_SLEEP1,				NULL_ANIM,				DEAD_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "sleep1_nose",		TYPE_MISC,			BOTH_SLEEP1_NOSE,			BOTH_SLEEP1,			DEAD_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "sleep1_getup",		TYPE_MISC,			BOTH_SLEEP1GETUP,			NULL_ANIM,				DEAD_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "sleep2",				TYPE_MISC,			BOTH_SLEEP2,				NULL_ANIM,				DEAD_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "sleep2_shift",		TYPE_MISC,			BOTH_SLEEP2_SHIFT,			BOTH_SLEEP2,			DEAD_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "sleep2_getup",		TYPE_MISC,			BOTH_SLEEP2GETUP,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "sleep3",				TYPE_MISC,			BOTH_SLEEP3,				NULL_ANIM,				DEAD_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "sleep3getup",		TYPE_MISC,			BOTH_SLEEP3GETUP,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "snapto1",			TYPE_FULLBODY,		BOTH_SNAPTO1,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "snapto2",			TYPE_FULLBODY,		BOTH_SNAPTO2,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "stand1_random1",		TYPE_GESTURE,		BOTH_STAND1_RANDOM2,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "stand1_random2",		TYPE_GESTURE,		BOTH_STAND1_RANDOM3,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "stand1_random3",		TYPE_GESTURE,		BOTH_STAND1_RANDOM4,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "stand1_random4",		TYPE_GESTURE,		BOTH_STAND1_RANDOM5,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "stand1_random5",		TYPE_GESTURE,		BOTH_STAND1_RANDOM6,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "stand1_random6",		TYPE_GESTURE,		BOTH_STAND1_RANDOM7,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "stand1_random7",		TYPE_GESTURE,		BOTH_STAND1_RANDOM8,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "stand1_random8",		TYPE_GESTURE,		BOTH_STAND1_RANDOM9,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "stand1_random9",		TYPE_GESTURE,		BOTH_STAND1_RANDOM10,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "stand1_random10",	TYPE_GESTURE,		BOTH_STAND1_RANDOM11,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "stand2_random1",		TYPE_GESTURE,		BOTH_STAND2_RANDOM1,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "stand2_random2",		TYPE_GESTURE,		BOTH_STAND2_RANDOM2,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "stand2_random3",		TYPE_GESTURE,		BOTH_STAND2_RANDOM3,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "stand2_random4",		TYPE_GESTURE,		BOTH_STAND2_RANDOM4,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "stand2_random5",		TYPE_GESTURE,		BOTH_STAND2_RANDOM5,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "stand2_random6",		TYPE_GESTURE,		BOTH_STAND2_RANDOM6,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "stand2_random7",		TYPE_GESTURE,		BOTH_STAND2_RANDOM7,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "stand2_random8",		TYPE_GESTURE,		BOTH_STAND2_RANDOM8,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "stand2_random9",		TYPE_GESTURE,		BOTH_STAND2_RANDOM9,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "stand2_random10",	TYPE_GESTURE,		BOTH_STAND2_RANDOM10,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "stand2_random11",	TYPE_GESTURE,		BOTH_STAND2_RANDOM11,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "stand2_random12",	TYPE_GESTURE,		BOTH_STAND2_RANDOM12,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_BOTH										},
	{ "stand3",				TYPE_MISC,			BOTH_STAND3,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH												},
	{ "standup1",			TYPE_FULLBODY,		BOTH_STANDUP1,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "surprised1",			TYPE_FULLBODY,		BOTH_SURPRISED1,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER																},
	{ "surprised2",			TYPE_FULLBODY,		BOTH_SURPRISED2,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER																},
	{ "surprised3",			TYPE_FULLBODY,		BOTH_SURPRISED3,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER																},
	{ "surprised4",			TYPE_FULLBODY,		BOTH_SURPRISED4,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH												},
	{ "surprised5",			TYPE_FULLBODY,		BOTH_SURPRISED5,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_OVERRIDE_UPPER																},
	{ "table_eat1",			TYPE_MISC,			BOTH_TABLE_EAT1,			BOTH_TABLE_IDLE1,		SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER												},
	{ "table_getup1",		TYPE_MISC,			BOTH_TABLE_GETUP1,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER												},
	{ "table_idle1",		TYPE_MISC,			BOTH_TABLE_IDLE1,			NULL_ANIM,				SITTING_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "table_talkgesture1",	TYPE_MISC,			BOTH_TABLE_TALKGESTURE1,	BOTH_TABLE_IDLE1,		STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_UPPER												},
	{ "talkgesture1",		TYPE_GESTURE,		BOTH_TALKGESTURE1,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_DEFAULT,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "talkgesture2",		TYPE_GESTURE,		BOTH_TALKGESTURE2,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "talkgesture3",		TYPE_GESTURE,		TORSO_TALKGESTURE4,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "talkgesture4",		TYPE_GESTURE,		TORSO_TALKGESTURE5,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "writhing2",			TYPE_FULLBODY,		BOTH_WRITHING2,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_BOTH,			EMOTE_CLAMP_BODY | EMOTE_OVERRIDE_BOTH | EMOTE_LOOP_BOTH							},
	{ "combadge1",			TYPE_GESTURE,		TORSO_COMBADGE1,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "combadge2",			TYPE_GESTURE,		TORSO_COMBADGE2,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "combadge3",			TYPE_GESTURE,		TORSO_COMBADGE3,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "combadge4",			TYPE_GESTURE,		TORSO_COMBADGE4,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "equipment1",			TYPE_GESTURE,		TORSO_EQUIPMENT1,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "equipment2",			TYPE_GESTURE,		TORSO_EQUIPMENT2,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "equipment3",			TYPE_GESTURE,		TORSO_EQUIPMENT3,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "grablbackl",			TYPE_GESTURE,		TORSO_GRABLBACKL,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "hand1",				TYPE_GESTURE,		TORSO_HAND1,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "hand2",				TYPE_GESTURE,		TORSO_HAND2,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "handgesture1",		TYPE_GESTURE,		TORSO_HANDGESTURE1,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "handgesture2",		TYPE_GESTURE,		TORSO_HANDGESTURE2,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "handgesture3",		TYPE_GESTURE,		TORSO_HANDGESTURE3,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "handgesture4",		TYPE_GESTURE,		TORSO_HANDGESTURE4,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "handgesture5",		TYPE_GESTURE,		TORSO_HANDGESTURE5,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "handgesture6",		TYPE_GESTURE,		TORSO_HANDGESTURE6,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "handgesture7",		TYPE_GESTURE,		TORSO_HANDGESTURE7,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "handgesture8",		TYPE_GESTURE,		TORSO_HANDGESTURE8,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "handgesture9",		TYPE_GESTURE,		TORSO_HANDGESTURE9,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "handgesture10",		TYPE_GESTURE,		TORSO_HANDGESTURE10,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "handgesture11",		TYPE_GESTURE,		TORSO_HANDGESTURE11,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "handgesture12",		TYPE_GESTURE,		TORSO_HANDGESTURE12,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "handgesture13",		TYPE_GESTURE,		TORSO_HANDGESTURE13,		NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "hypospray",			TYPE_GESTURE,		TORSO_HYPOSPRAY1,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "pokeridle1",			TYPE_GESTURE,		TORSO_POKERIDLE1,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_LOOP_UPPER												},
	{ "pokeridle2",			TYPE_GESTURE,		TORSO_POKERIDLE2,			TORSO_POKERIDLE1,		STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER																},
	{ "pokeridle3",			TYPE_GESTURE,		TORSO_POKERIDLE3,			TORSO_POKERIDLE1,		STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "shout1",				TYPE_GESTURE,		TORSO_SHOUT1,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_LOOP_UPPER												},
	{ "speechless1",		TYPE_GESTURE,		TORSO_SPEECHLESS1,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_LOOP_UPPER												},
	{ "speechless2",		TYPE_GESTURE,		TORSO_SPEECHLESS2,			NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_LOOP_UPPER												},
	{ "taunt",				TYPE_GESTURE,		TORSO_GESTURE,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
	{ "wrist1",				TYPE_GESTURE,		TORSO_WRIST1,				NULL_ANIM,				STAND_VIEWHEIGHT,		HITBOX_NULL,			EMOTE_UPPER,		EMOTE_OVERRIDE_UPPER | EMOTE_REVERTLOOP_UPPER										},
};

/*
 * TiM : Hrmm... this may have been why it was crashing on some people's PCs...
 *Let's try the old fashioned way...
 * int		bg_numEmotes = sizeof( emoteList ) / sizeof ( emoteList[0] );
 */
int		bg_numEmotes = 180;

/*
 * TiM:
 * !In order to set up a list of items we can use in the 'give' command.
 * */
giveItem_t	bg_giveItem[] = {
	//consoleName			giveType				giveValue
	{ "all",				TYPE_ALL,				0						},
	{ "health",				TYPE_HEALTH,			0						},
	{ "weapons",			TYPE_WEAPONS,			0						},
	{ "ammo",				TYPE_AMMO,				0						},
	{ "transporter",		TYPE_HOLDABLE,			HI_TRANSPORTER			},
	{ "forcefield",			TYPE_HOLDABLE,			HI_SHIELD				},
	{ "phaser",				TYPE_WEAPON,			WP_5				},
	{ "phaser_rifle",		TYPE_WEAPON,			WP_6	},
	{ "coffee",				TYPE_WEAPON,			WP_4				},
	{ "disruptor",			TYPE_WEAPON,			WP_10			},
	{ "coffee",				TYPE_WEAPON,			WP_4				},
	{ "admin_gun",			TYPE_WEAPON,			WP_8		},
	{ "tr-116",				TYPE_WEAPON,			WP_7				},
	{ "photon_burst",		TYPE_WEAPON,			WP_9		},
	{ "dermal_regen",		TYPE_WEAPON,			WP_13			},
	{ "hypospray",			TYPE_WEAPON,			WP_12			},
	{ "toolkit",			TYPE_WEAPON,			WP_14				},
	{ "medkit",				TYPE_WEAPON,			WP_11				},
	{ "tricorder",			TYPE_WEAPON,			WP_2			},
	{ "padd",				TYPE_WEAPON,			WP_3					},
	{ "hyperspanner",		TYPE_WEAPON,			WP_15			},
	{ "cloak",				TYPE_POWERUP,			0						},
	{ "flight",				TYPE_POWERUP,			0						},
	{ "god",				TYPE_POWERUP,			0						}
};

/* TiM - Meh... just define the number... doing fancy array stuff seems to make PCs 'splode O_o */
int	bg_numGiveItems = 24;

/**
*	\brief Finds an item by it's classname
*	
*	\return the item
*/
gitem_t *BG_FindItemWithClassname(const char *name)
{
	int i = 0;
	gitem_t *item = NULL;

	if ( (NULL == name) || (0 == name[0]) )
	{
		return NULL;
	}
	for (i = 0; i < bg_numItems; i++)
	{
		item = &bg_itemlist[i];
		if (!strcmp(name, item->classname))
		{
			return item;
		}
	}
	return NULL;
}

/**
*	Finds the classname for a holdable.
*	\return classname for holdable
*/
char *BG_FindClassnameForHoldable(holdable_t pw)
{
	gitem_t *item = BG_FindItemForHoldable(pw);

	if (item)
	{
		return item->classname;
	}
	return NULL;
}

/**
*	Finds item for a powerup.
*	\return the item
*/
gitem_t	*BG_FindItemForPowerup( powerup_t pw ) {
	int		i;

	for ( i = 0 ; i < bg_numItems ; i++ ) {
		if ( (bg_itemlist[i].giType == IT_POWERUP || 
				bg_itemlist[i].giType == IT_TEAM) && 
			bg_itemlist[i].giTag == pw ) {
			return &bg_itemlist[i];
		}
	}

	return NULL;
}


/**
*	Finds item for a holdable.
*	\return the item
*/
gitem_t	*BG_FindItemForHoldable( holdable_t pw ) {
	int		i;

	for ( i = 0 ; i < bg_numItems ; i++ ) {
		if ( bg_itemlist[i].giType == IT_HOLDABLE && bg_itemlist[i].giTag == pw ) {
			return &bg_itemlist[i];
		}
	}

	Com_Error( ERR_DROP, "HoldableItem not found" );

	return NULL;
}


/**
*	Finds item for a weapon.
*	\return the item
*/
gitem_t	*BG_FindItemForWeapon( weapon_t weapon ) {
	gitem_t	*it;
	for ( it = bg_itemlist + 1 ; it->classname ; it++) {
		if ( it->giType == IT_WEAPON && it->giTag == weapon ) {
			return it;
		}
	}

	Com_Error( ERR_DROP, "Couldn't find item for weapon %i", weapon);
	return NULL;
}

/**
*	Finds item for ammo.
*	\return the item
*/
gitem_t	*BG_FindItemForAmmo( weapon_t weapon ) {
	gitem_t	*it;
	
	for ( it = bg_itemlist + 1 ; it->classname ; it++) {
		if ( it->giType == IT_AMMO && it->giTag == weapon ) {
			return it;
		}
	}

	Com_Error( ERR_DROP, "Couldn't find item for ammo %i", weapon);
	return NULL;
}

/**
*	Find a tiem by pickupName.
*	\return the item
*/
gitem_t	*BG_FindItem( const char *pickupName/*const char *classname*/ ) {
	gitem_t	*it;
	
	for ( it = bg_itemlist + 1 ; it->classname ; it++ ) {
		if ( !Q_stricmp( it->pickup_name, pickupName )/*!Q_stricmp( it->classname, classname)*/ ) /* RPG-X: RedTechie - Trying to fix give cmd */
			return it;
	}

	return NULL;
}

/**
*	\brief Checks if player is touching an item.
*
*	Items can be picked up without actually touching their physical bounds to make
*	grabbing them easier
*/
qboolean	BG_PlayerTouchesItem( playerState_t *ps, entityState_t *item, int atTime ) {
	vec3_t		origin;

	BG_EvaluateTrajectory( &item->pos, atTime, origin );

	/* we are ignoring ducked differences here */
	if ( ps->origin[0] - origin[0] > 44
		|| ps->origin[0] - origin[0] < -50
		|| ps->origin[1] - origin[1] > 36
		|| ps->origin[1] - origin[1] < -36
		|| ps->origin[2] - origin[2] > 36
		|| ps->origin[2] - origin[2] < -36 ) {
		return qfalse;
	}

	return qtrue;
}



/**
*	\brief Check if item can be grabbed.
*
*	Returns false if the item should not be picked up.
*	This needs to be the same for client side prediction and server use.
*/

qboolean	BG_CanItemBeGrabbed( const entityState_t *ent, const playerState_t *ps, int maxWeap ) {
	gitem_t	*item;

//_______________________________________________________________

	if ( ent->modelindex < 1 || ent->modelindex >= bg_numItems ) {
		/*Com_Error( ERR_DROP, "BG_CanItemBeGrabbed: index out of range" );
		  Com_Printf ("BG_CanItemBeGrabbed: index out of range\n");*/
		return qfalse;
	}

	item = &bg_itemlist[ent->modelindex];

	/* Marcin| 30/12/2008 */
	if (ps->ammo[item->giTag] >= maxWeap) {
		return qfalse;
	} else {
		return qtrue;
	}
}

//======================================================================

/**
*	Evaluates a trajectory.
*/
void BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result ) {
	float		deltaTime;
	float		phase;

	switch( tr->trType ) {
	case TR_STATIONARY:
	case TR_INTERPOLATE:
		VectorCopy( tr->trBase, result );
		break;
	case TR_LINEAR:
		deltaTime = ( atTime - tr->trTime ) * 0.001;	/* milliseconds to seconds */
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		break;
	case TR_SINE:
		deltaTime = ( atTime - tr->trTime ) / (float) tr->trDuration;
		phase = sin( deltaTime * M_PI * 2 );
		VectorMA( tr->trBase, phase, tr->trDelta, result );
		break;
	case TR_LINEAR_STOP:
		if ( atTime > tr->trTime + tr->trDuration ) {
			atTime = tr->trTime + tr->trDuration;
		}
		deltaTime = ( atTime - tr->trTime ) * 0.001;	/* milliseconds to seconds */
		if ( deltaTime < 0 ) {
			deltaTime = 0;
		}
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		break;
	case TR_GRAVITY:
		deltaTime = ( atTime - tr->trTime ) * 0.001;	/* milliseconds to seconds */
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		result[2] -= 0.5 * DEFAULT_GRAVITY * deltaTime * deltaTime;		/* FIXME: local gravity... */
		break;
	default:
		Com_Error( ERR_DROP, "BG_EvaluateTrajectory: unknown trType: %i", tr->trTime );
		break;
	}
}

/**
*	Determining velocity at a given time
*/
void BG_EvaluateTrajectoryDelta( const trajectory_t *tr, int atTime, vec3_t result ) {
	float	deltaTime;
	float	phase;

	switch( tr->trType ) {
	case TR_STATIONARY:
	case TR_INTERPOLATE:
		VectorClear( result );
		break;
	case TR_LINEAR:
		VectorCopy( tr->trDelta, result );
		break;
	case TR_SINE:
		deltaTime = ( atTime - tr->trTime ) / (float) tr->trDuration;
		phase = cos( deltaTime * M_PI * 2 );	/* derivative of sin = cos */
		phase *= 0.5;
		VectorScale( tr->trDelta, phase, result );
		break;
	case TR_LINEAR_STOP:
		if ( atTime > tr->trTime + tr->trDuration ) {
			VectorClear( result );
			return;
		}
		VectorCopy( tr->trDelta, result );
		break;
	case TR_GRAVITY:
		deltaTime = ( atTime - tr->trTime ) * 0.001;	/* milliseconds to seconds */
		VectorCopy( tr->trDelta, result );
		result[2] -= DEFAULT_GRAVITY * deltaTime;		/* FIXME: local gravity... */
		break;
	default:
		Com_Error( ERR_DROP, "BG_EvaluateTrajectoryDelta: unknown trType: %i", tr->trTime );
		break;
	}
}

/**
*	Handles the sequence numbers
*/
void BG_AddPredictableEventToPlayerstate( int newEvent, int eventParm, playerState_t *ps ) {
	ps->events[ps->eventSequence & (MAX_PS_EVENTS-1)] = newEvent;
	ps->eventParms[ps->eventSequence & (MAX_PS_EVENTS-1)] = eventParm;
	ps->eventSequence++;
}


/**
*	\brief Playerstate to entitystate
*
*	This is done after each set of usercmd_t on the server,
*	and after local prediction on the client
*/
void BG_PlayerStateToEntityState( playerState_t *ps, entityState_t *s, qboolean snap ) {
	int		i;
	char	medicrevive[32];
	int		medicrevive_int;
	
	/* RPG-X: RedTechie - Attempted to fix player going invisible now they just dont go invisible (me being picky) a player is never going to notice this */
	trap_Cvar_VariableStringBuffer( "rpg_medicsrevive", medicrevive, 32 );
	medicrevive_int = atoi(medicrevive);
	if ( ps->pm_type == PM_INTERMISSION || ps->pm_type == PM_SPECTATOR ) {
		s->eType = ET_INVISIBLE;
	} else if ( ps->stats[STAT_HEALTH] <= GIB_HEALTH ) {
		if(medicrevive_int == 1){
			s->eType = ET_PLAYER; /* RPG-X: RedTechie - No gibbing! Before it was s->eType = ET_INVISIBLE; */
		}else{
			s->eType = ET_INVISIBLE;
		}
	} else {
		s->eType = ET_PLAYER;
	}

	s->number = ps->clientNum;

	s->pos.trType = TR_INTERPOLATE;
	VectorCopy( ps->origin, s->pos.trBase );
	if ( snap ) {
		SnapVector( s->pos.trBase );
	}

	/* TiM - Get velocity as well */
	VectorCopy( ps->velocity, s->pos.trDelta );
	if ( snap )
	{
		SnapVector( s->pos.trDelta );
	}

	s->apos.trType = TR_INTERPOLATE;
	VectorCopy( ps->viewangles, s->apos.trBase );
	if ( snap ) {
		SnapVector( s->apos.trBase );
	}

	s->angles2[YAW] = ps->movementDir;
	
	/* TiM */
	s->torsoAnim = ps->stats[TORSOANIM];
	s->legsAnim = ps->stats[LEGSANIM];

	/* TiM : Mental note : DON'T FREEAKIN ACCIDENTLY COMMENT THIS OUT AGAIN!  IT'S KINDA IMPORTANT!!!!!!!!!!!!!!!! */
	s->clientNum = ps->clientNum;		/* ET_PLAYER looks here instead of at number
										   so corpses can also reference the proper config*/

	s->eFlags = ps->eFlags;
	if ( ( !medicrevive_int && ps->stats[STAT_HEALTH] <= 0 ) || 
		( medicrevive_int > 0 && ps->stats[STAT_HEALTH] <= 1 ) ) 
	{ /* RPG-X: TiM: Bah Red... u gotta account for these flags with ur system */
		s->eFlags |= EF_DEAD;			 /* or it screws up the model system */
	} else {
		s->eFlags &= ~EF_DEAD;
	}

	/*==========================================================================
	 * TiM: ^&$*#^^.... T_T
	 * Okay it's official. eFlags is buggy.  Turns out the Ravensoft programmers were
	 * wreaking grief with it too. :P
	 * Although hacky, transposing these flags here is the only way I know to get this data from G to CG.
	 */

	/* Clamp body (not head) flag */
	if ( ps->stats[EMOTES] & EMOTE_CLAMP_BODY ) {
		s->eFlags |= EF_CLAMP_BODY;
	}
	else {
		s->eFlags &= ~EF_CLAMP_BODY;
	}

	/* Clamp whole body flags */
	if ( ps->stats[EMOTES] & EMOTE_CLAMP_ALL ) {
		s->eFlags |= EF_CLAMP_ALL;
	}
	else {
		s->eFlags &= ~EF_CLAMP_ALL;
	}

	if ( ps->stats[EMOTES] & EMOTE_EYES_SHUT ) 
	{
		s->eFlags |= EF_EYES_SHUT;
		/* Com_Printf( "Eyes were shutted.\n" ); */
	}
	else {
		s->eFlags &= ~EF_EYES_SHUT;
	}

	if ( ps->stats[EMOTES] & EMOTE_EYES_PISSED ) {
		s->eFlags |= EF_EYES_ANGRY;
	}
	else {
		s->eFlags &= ~EF_EYES_ANGRY;
	}

	/*==========================================================================*/

	if ( ps->externalEvent ) {
		s->event = ps->externalEvent;
		s->eventParm = ps->externalEventParm;
	} 
	else if ( ps->entityEventSequence < ps->eventSequence ) 
	{
		int		seq;

		if ( ps->entityEventSequence < ps->eventSequence - MAX_PS_EVENTS) {
			ps->entityEventSequence = ps->eventSequence - MAX_PS_EVENTS;
		}
		seq = ps->entityEventSequence & (MAX_PS_EVENTS-1);
		s->event = ps->events[ seq ] | ( ( ps->entityEventSequence & 3 ) << 8 );
		s->eventParm = ps->eventParms[ seq ];
		ps->entityEventSequence++;
	}

	s->weapon = ps->weapon;
	s->groundEntityNum = ps->groundEntityNum;

	s->powerups = 0;
	for ( i = 0 ; i < MAX_POWERUPS ; i++ ) 
	{
		if ( ps->powerups[ i ] ) {
			s->powerups |= 1 << i;
		}
	}

	if ( s->powerups & (1 << PW_FLIGHT) || (ps->powerups[PW_EVOSUIT] && ps->gravity == 0 ) ) 
	{
		s->eFlags |= EF_FULL_ROTATE;
	}
	else 
	{
		s->eFlags &= ~EF_FULL_ROTATE;
	}

	/* TiM: Extra - Transmit the weapons stats as a flag for the 'equip' command */
	s->time2 = ps->stats[STAT_WEAPONS];
}

#define MAX_ITEMNAMES 45

const char *itemnames[MAX_ITEMNAMES] = 
{

"nothing",
"WEAPON_NULL_HAND",
"WEAPON_TRICORDER",
"WEAPON_PADD",
"WEAPON_COFFEE",
"WEAPON_PHASER",
"WEAPON_COMPRESSIONRIFLE",
"WEAPON_TR116",
"WEAPON_GRENADELAUNCHER",
"WEAPON_QUANTUM",
"WEAPON_DISRUPTOR",
"WEAPON_MEDKIT",
"WEAPON_VOYAGER_HYPO",
"WEAPON_DERMAL_REGEN",
"WEAPON_TOOLKIT",
"WEAPON_NEUTRINO_PROBE",

"AMMO_COMPRESSIONRIFLE",
"AMMO_IMOD",
"AMMO_SCAVENGERRIFLE",
"AMMO_STASIS",
"AMMO_GRENADELAUNCHER",
"AMMO_DISRUPTOR",
"AMMO_QUANTUM",
"AMMO_DREADNOUGHT",

"ITEM_ARMOR_SHARD",
"ITEM_ARMOR",
"ITEM_HEAVY_ARMOR",
"ITEM_HYPO_SMALL",
"ITEM_HYPO",

"HOLDABLE_TRANSPORTER",
"HOLDABLE_MEDKIT",
"HOLDABLE_QUADDAMAGE",
"HOLDABLE_BATTLESUIT",
"HOLDABLE_SPEED",
"HOLDABLE_INVISIBILITY",
"HOLDABLE_REGENERATION",
"HOLDABLE_FLIGHT",
"HOLDABLE_REDFLAG",
"HOLDABLE_BLUEFLAG",
"HOLDABLE_DETPACK",
"ITEM_SEEKER",
"HOLDABLE_SHIELD",
"HOLOGRAPHIC_DECOY",	/* decoy temp */

"WEAPON_TR116",

NULL
};


#define MAX_ITEMNAMEFILE	10000 /* 5000 har har */
char	itemNameBuffer[MAX_ITEMNAMEFILE];

/**
*	Explains itself.
*/
void BG_ParseItemsText(char *buff)
{
	char	*token;
	char *buffer;
	int i,len;

	COM_BeginParseSession();

	buffer = buff;
	while ( buffer ) 
	{
		token = COM_ParseExt( &buffer, qtrue );

		i=0;
		while (itemnames[i])
		{
			if (Q_stricmp(token, itemnames[i])==0)
			{
				token = COM_ParseExt( &buffer, qtrue );
				if (token[0])
				{	
					len = strlen(token);
					if (len)
					{
						bg_itemlist[i].pickup_name = (buffer - (len + 1));	/* The +1 is to get rid of the " at the beginning of the sting. */
						*(buffer - 1) = '\0';		/*	Place an string end where is belongs. */
					}
				}

				break;
			}
			i++;
		}

	}
}

/*
*	Creates a filename with an extension based on the value in g_language
*/
void BG_LanguageFilename(char *baseName,char *baseExtension,char *finalName)
{
	char	language[32];
	fileHandle_t	file;

	trap_Cvar_VariableStringBuffer( "g_language", language, 32 );

	/* If it's English then no extension */
	if (language[0]=='\0' || Q_stricmp ("ENGLISH",language)==0)
	{
		Com_sprintf(finalName,MAX_QPATH,"%s.%s",baseName,baseExtension);
	}
	else
	{
		Com_sprintf(finalName,MAX_QPATH,"%s_%s.%s",baseName,language,baseExtension);

		/* Attempt to load the file */
		trap_FS_FOpenFile( finalName, &file, FS_READ );

		if ( file == 0 )	/* This extension doesn't exist, go English. */
		{
			Com_sprintf(finalName,MAX_QPATH,"%s.%s",baseName,baseExtension);	/* the caller will give the error if this isn't there */
		}
		else
		{
			trap_FS_FCloseFile( file );
		}
	}
}

/**
*	Loads item names.
*/
void BG_LoadItemNames(void)
{
	char	fileName[MAX_QPATH];
	int		len;
	fileHandle_t	f;

	BG_LanguageFilename("ext_data/mp_itemnames","dat",fileName);

	len = trap_FS_FOpenFile( fileName, &f, FS_READ );

	if ( !f ) 
	{
		Com_Printf( S_COLOR_RED "BG_LoadItemNames : MP_ITEMNAMES.DAT file not found!\n");
		return;
	}

	if ( len > MAX_ITEMNAMEFILE ) 
	{
		Com_Printf( S_COLOR_RED "BG_LoadItemNames : MP_ITEMNAMES.DAT too big!\n");
		return;
	}

	/* initialise the data area */
	memset(itemNameBuffer, 0, sizeof(itemNameBuffer));	

	trap_FS_Read( itemNameBuffer, len, f );

	trap_FS_FCloseFile( f );

	BG_ParseItemsText(itemNameBuffer);

}

/**
*	Read a configuration file to get the sex
*	models/players_rpgx/munro/animation.cfg
*/
static gender_t	G_ParseAnimationFileSex( const char *filename) {
	char			*text_p;
	int				len;
	char			*token;
	char			text[20000];
	fileHandle_t	f;
	char			animfile[MAX_QPATH];

	/* strcpy(animfile, filename); */
	Q_strncpyz(animfile, filename, sizeof(animfile));
	len = strlen(animfile);
	strcpy(&animfile[len-strlen("groups.cfg")], "animation.cfg");

	/* load the file */
	len = trap_FS_FOpenFile( animfile, &f, FS_READ );
	if ( len <= 0 ) {
		return GENDER_NEUTER;
	}
	if ( len >= sizeof( text ) - 1 ) {
		Com_Printf( "File %s too long\n", animfile );
		trap_FS_FCloseFile( f );
		return GENDER_NEUTER;
	}
	trap_FS_Read( text, len, f );
	text[len] = 0;
	trap_FS_FCloseFile( f );

	/* parse the text */
	text_p = text;

	/* read optional parameters */
	while ( 1 ) {
		token = COM_Parse( &text_p );
		if ( !token[0] ) {
			break;
		}
		if ( !Q_stricmp( token, "sex" ) ) {
			token = COM_Parse( &text_p );
			if ( !token[0] ) {
				break;
			}
			if ( token[0] == 'f' || token[0] == 'F' ) {
				return GENDER_FEMALE;
			} else if ( token[0] == 'n' || token[0] == 'N' ) {
				return GENDER_NEUTER;
			} else {
				return GENDER_MALE;
			}
		}
	}
	return GENDER_MALE;
}

/**
*	Registers an item.
*/
#define MAX_GROUP_FILE_SIZE	5000
char* BG_RegisterRace( const char *name ) {
	char			*text_p;
	char			*token;
	int				len;
	fileHandle_t	f;
	char			text[MAX_GROUP_FILE_SIZE];
	gender_t 		theSex;

	memset (races, 0, sizeof(races));
	memset (text, 0, sizeof(text));

	/* load and parse the skin file */
	len = trap_FS_FOpenFile( name, &f, FS_READ );
	if ( !f ) {
		/* if we didn't get a races file, use an empty one. */
		Com_sprintf(races, sizeof(races), "unknown");
		return races;
	}
	if ( len >= sizeof( text ) - 1) 
	{
		Com_Printf( S_COLOR_RED "file too large: %s is %i, max allowed is %i", name, len, sizeof( text ) );
		trap_FS_FCloseFile( f );
		return races;
	}

	trap_FS_Read( text, len, f );
	trap_FS_FCloseFile( f );
	
	theSex = G_ParseAnimationFileSex(name);
	if (theSex == GENDER_MALE) {
		strcat(races, "Male,");
	} else if (theSex == GENDER_FEMALE) {
		strcat(races, "Female,");
	}

	text_p = text;
	while ( *text_p ) {
		/* get surface name */
		token = COM_Parse( &text_p );

		if ( !token[0] ) {
			break;
		}

		/* if we about to break the races size list then dump us out */
		if (strlen(races) + strlen(token) > 256) {
			break;
		}

		/* add it into the race list */
		strcat(races, token);
		/* put a comma between the names */
		strcat(races, ",");

		if ( *text_p == ',' ) {
			text_p++;
		}

		if (!Q_stricmp ("borg", token) ) {
			if (theSex == GENDER_MALE) {
				/* add it into the race list */
				strcat(races, "BorgMale,");
			} else if (theSex == GENDER_FEMALE) {
				strcat(races, "BorgFemale,");
			} else {
			}
		}

	}

	/* just in case */
	if (!races[0])
	{
		Com_sprintf(races, sizeof(races), "unknown");
	}
	else
	{	/* lose the last comma */
		races[strlen(races)-1] = 0;
	}

	return races;
}

/**
*	Parses the rank names.
*/
qboolean BG_ParseRankNames( char* fileName, rankNames_t rankNames[] ) {
	fileHandle_t	f;
	int				file_len;
	char			charText[20000];
	char*			textPtr;
	char*			token;
	int				i = 0;

	file_len = trap_FS_FOpenFile( fileName, &f, FS_READ );

	if ( file_len<= 0 ) {
		return qfalse;
	}

	if ( file_len >= ( sizeof(charText) - 1) ) {
		Com_Printf( S_COLOR_RED "File length of %s is too long.\n", fileName );
		return qfalse;
	}

	memset( &charText, 0, sizeof( charText ) );
	memset( rankNames, 0, sizeof( rankNames ) );

	trap_FS_Read( charText, file_len, f );

	charText[file_len] = 0;

	trap_FS_FCloseFile( f );

	COM_BeginParseSession();

	textPtr = charText;

	token = COM_Parse( &textPtr );

	if ( !token[0] ) {
		Com_Printf( S_COLOR_RED "No data found in buffer: %s\n", fileName );
		return qfalse;
	}

	if ( Q_stricmpn( token, "{", 1 ) ) {
		Com_Printf( S_COLOR_RED "No beginning { found in %s\n", fileName );
		return qfalse;
	}

	/* Parse out the default cell.  Default has no names anyway,
	   but in case a n00bie modder put names in anyway. */
	SkipBracedSection( &textPtr );

	while( 1 ) {
		/* lastPtr = textPtr; */
		token = COM_Parse( &textPtr );
		if( !token[0] ) {
			break;
		}

		if ( i >= MAX_RANKS ) {
			break;
		}

		/* If we hit an open brace (ie, assuming we hit the start of a new rank cell) */
		if ( !Q_stricmpn( token, "{", 1 ) ) {
			while ( 1 ) {
				token = COM_Parse( &textPtr );
				if( !token[0] ) {
					break;
				}

				/* We hit a MenuTexture entry, since this uses { symbols, we'll skip these to stop errors. */
				if ( !Q_stricmpn( token, "MenuTexture", 11 ) ) {
					SkipRestOfLine( &textPtr );
					continue;
				}

				if ( !Q_stricmpn( token, "ConsoleName", 11) ) {
					if ( COM_ParseString( &textPtr, &token ) ) {
						continue;
					}

					Q_strncpyz( rankNames[i].consoleName, token, sizeof( rankNames[i].consoleName ) );

					continue;
				}
				else if ( !Q_stricmpn( token, "FormalName", 10) ) {
					if ( COM_ParseString( &textPtr, &token ) ) {
						continue;
					}

					Q_strncpyz( rankNames[i].formalName, token, sizeof( rankNames[i].formalName ) );

					continue;
				}
				/* We hit the end of the cell. */
				else if ( !Q_stricmpn( token, "}", 1 ) ) {
					break;
				}
			}

			/* Error check.  If we didn't get both a formal and console name, pwn the caller. ;P */
			if ( !rankNames[i].consoleName[0] || !rankNames[i].formalName[0] ) {
				Com_Printf( S_COLOR_RED "One or more rank names were not found in rank#: %i\n", i );
				return qfalse;
			}
			else {
				i++;
			}
		}
	}
	return qtrue;
}

/*
===========
NextWordEndsHere
===========
*/
char *NextWordEndsHere(char *p)
{
	if (*p != ' ') {
		return p;
	}

	while (*p && *p == ' ') { /* first pass */
		++p;
	}

	while (*p && *p != ' ') { /* second pass */
		++p;
	}

	return p;
}

/*
===========
EndWord
===========
Returns a pointer to the position of the next space, null, or newline.
*/
char *EndWord(char *pos)
{
	while (!*pos && *pos != ' ' && *pos != '\n') {
		++pos;
	}

	return pos;
}
