// Copyright (C) 1999-2000 Id Software, Inc.
//
// bg_misc.c -- both games misc functions, all completely stateless

#include "q_shared.h"
#include "bg_public.h"

int				trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode );
void			trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );
void	trap_FS_FCloseFile( fileHandle_t f );
void	trap_FS_Read( void *buffer, int len, fileHandle_t f );

// If you change these:  PLEASE CHANGE THE COMMENTS ON THE AMMO PICKUPS, WHICH DETAIL THE QUANTITY IN THE CLIP
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

int Max_Ammo[WP_NUM_WEAPONS] =
{
	0,						// WP_NONE,
	50,						// WP_PHASER,	!! this should match PHASER_AMMO_MAX defined in bg_public
	128,					// WP_COMPRESSION_RIFLE,
	60,						// WP_IMOD,			
	100,					// WP_SCAVENGER_RIFLE,	
	50,						// WP_STASIS,			
	30,						// WP_GRENADE_LAUNCHER,
	120,					// WP_TETRION_DISRUPTOR,
	20,						// WP_QUANTUM_BURST,	
	50,					// WP_DREADNOUGHT,		
	50,					// WP_VOYAGER_HYPO,
	50,					// WP_BORG_ASSIMILATOR,
	100,						// WP_BORG_WEAPON,
	50,						// WP_TRICORDER,
	50,						// WP_PADD,
	50,						// WP_ENGTOOL,
	64						// WP_TR116

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
		NULL,							//char		*classname;	// spawning name
		NULL,							//char		*pickup_sound;
		NULL,							//char		*world_model;
		NULL,							//char		*view_model;
/* icon */		NULL,					//char		*icon;
/* pickup */	NULL,					//char		*pickup_name;	// for printing on pickup
		0,								//int			quantity;		// for ammo how much, or duration of powerup
		0,								//itemType_t  giType;			// IT_* flags
		0,								//int			giTag;
/* precache */ "",						//char		*precaches;		// string of all models and images this item will use
/* sounds */ ""							//char		*sounds;		// string of all sounds this item will use
	},	// leave index 0 alone

	//
	// WEAPONS 
	//

/*QUAKED weapon_phaser (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_phaser", 
		"sound/weapons/w_pkup.wav",
        "models/weapons2/phaser/phaser_w.md3",	//world
		"models/weapons2/phaser/phaser.md3",	//view
/* icon */		"icons/w_icon_phaser",
/* pickup */	"Phaser",
		AMMO_PHASER_CLIP,
		IT_WEAPON,
		WP_PHASER,
/* precache */ "",
/* sounds */ ""
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
		WP_COMPRESSION_RIFLE,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED weapon_imod (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_imod", 
		"sound/weapons/w_pkup.wav",
        "models/weapons2/imod/imod2_w.md3", //world
		"models/weapons2/imod/imod2.md3",	//view
/* icon */		"icons/w_icon_imod",
/* pickup */	"I-MOD",
		AMMO_IMOD_CLIP,
		IT_WEAPON,
		WP_IMOD,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED weapon_scavenger (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_scavenger", 
		"sound/weapons/w_pkup.wav",
        "models/weapons2/scavenger/scavenger_w.md3", //world
		"models/weapons2/scavenger/scavenger.md3",	//view
/* icon */		"icons/w_icon_scav",
/* pickup */	"Scavenger Rifle",
		AMMO_SCAVENGER_CLIP,
		IT_WEAPON,
		WP_SCAVENGER_RIFLE,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED weapon_stasisweapon (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_stasisweapon",
		"sound/weapons/w_pkup.wav",
        "models/weapons2/alien_disruptor/disruptor_w.md3",	//world
		"models/weapons2/alien_disruptor/disruptor.md3",	//view
/* icon */		"icons/w_icon_disruptor",
/* pickup */	"Disruptor",
		AMMO_STASIS_CLIP,
		IT_WEAPON,
		WP_STASIS,
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
		WP_GRENADE_LAUNCHER,
/* precache */ "",
/* sounds */ "sound/weapons/glauncher/bounce1.wav sound/weapons/glauncher/bounce2.wav"
	},


/*QUAKED weapon_tetriondisruptor (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_tetriondisruptor", 
		"sound/weapons/w_pkup.wav",
        "models/weapons2/tr116/tr-116_w.md3",//world
		"models/weapons2/tr116/tr-116.md3",	//view
/* icon */		"icons/w_icon_tr116",
/* pickup */	"TR-116",
		AMMO_PHASER_CLIP,
		IT_WEAPON,
		WP_TETRION_DISRUPTOR,
/* precache */ "",
/* sounds */ ""
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
		WP_QUANTUM_BURST,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED weapon_dreadnought (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_dreadnought", 
		"sound/weapons/w_pkup.wav",
        "models/weapons2/dermal_regen/dermal_regen_w.md3",
		"models/weapons2/dermal_regen/dermal_regen.md3",
/* icon */		"icons/w_icon_dermalregen",
/* pickup */	"Dermal Regenerator",
		AMMO_DREADNOUGHT_CLIP,
		IT_WEAPON,
		WP_DREADNOUGHT,
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
		WP_COMPRESSION_RIFLE,
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
		WP_IMOD,
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
		WP_SCAVENGER_RIFLE,
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
		WP_STASIS,
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
		WP_GRENADE_LAUNCHER,
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
		WP_TETRION_DISRUPTOR,
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
		WP_QUANTUM_BURST,
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
		WP_DREADNOUGHT,
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


/*QUAKED item_quad (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
multiplies your weapon's damage for 30 seconds
*/
	{
		"item_quad", 
		"sound/items/quaddamage.wav",
        "models/powerups/trek/quad_damage.md3",	//world
		NULL,
/* icon */		"icons/dm_quad",
/* pickup */	"Quantum Weapon Enhancer",
		30,
		IT_POWERUP,
		PW_QUAD,
/* precache */ "",
/* sounds */ "sound/items/damage3.wav"
	},

/*QUAKED item_enviro (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
20 seconds of invulnerability
*/
	{
		"item_enviro",
		"sound/items/protect.wav",
        "models/powerups/trek/armor3.md3",	//world
		NULL,
/* icon */		"icons/envirosuit",
/* pickup */	"Metaphasic Shielding",
		20,
		IT_POWERUP,
		PW_BATTLESUIT,
/* precache */ "",
/* sounds */ "sound/items/protect3.wav"
	},

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

/*QUAKED item_regen (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
for 30 seconds you get 5 health every second, up to 200 health
*/
	{
		"item_regen",
		"sound/items/regeneration.wav",
        "models/powerups/trek/regen.md3",	//world
		NULL,
/* icon */		"icons/regen",
/* pickup */	"Nano-Regenerative Protoplasmer",
		30,
		IT_POWERUP,
		PW_REGEN,
/* precache */ "",
/* sounds */ "sound/items/regen.wav"
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

/*QUAKED team_CTF_redflag (1 0 0) (-24 -24 -16) (24 24 32)
Only in CTF games
*/
	{
		"team_CTF_redflag",
		"sound/voice/computer/misc/haveflag.wav",
        "models/flags/flag_red.md3",	//world  !! must match cg_main media and botfiles/items.c !!
		NULL,
/* icon */		"icons/iconf_red",
/* pickup */	"Red Flag",
		0,
		IT_TEAM,
		PW_REDFLAG,
/* precache */ "",
/* sounds */ "sound/voice/computer/misc/stolen.wav sound/voice/computer/misc/stolen_e.wav sound/voice/computer/misc/returned.wav sound/voice/computer/misc/returned_e.wav"
	},

/*QUAKED team_CTF_blueflag (0 0 1) (-24 -24 -16) (24 24 32)
Only in CTF games
*/
	{
		"team_CTF_blueflag",
		"sound/voice/computer/misc/haveflag.wav",
        "models/flags/flag_blue.md3",//must match cg_main media and botfiles/items.c
		NULL,
/* icon */		"icons/iconf_blu",
/* pickup */	"Blue Flag",
		0,
		IT_TEAM,
		PW_BLUEFLAG,
/* precache */ "",
/* sounds */ "sound/voice/computer/misc/dropped.wav sound/voice/computer/misc/dropped_e.wav sound/voice/computer/misc/scored.wav sound/voice/computer/misc/scored_e.wav"
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

/*QUAKED item_seeker (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
30 seconds of seeker drone
*/
	{
		"item_seeker", 
		"sound/player/pickupenergy.wav",
        "models/powerups/trek/flyer.md3",	//world
		NULL,
/* icon */		"icons/icon_seeker",
/* pickup */	"Seeker Drone",
		30,
		IT_POWERUP,
		PW_SEEKER,
/* precache */ "",
/* sounds */ ""
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

/*QUAKED weapon_voyager_hypo (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_voyager_hypo", 
		"sound/weapons/w_pkup.wav",
        "models/weapons2/hypospray/hypospray_w.md3",	//world
		"models/weapons2/hypospray/hypospray.md3",	//view
/* icon */		"icons/w_icon_hypo",
/* pickup */	"Hypo",
		AMMO_PHASER_CLIP,
		IT_WEAPON,
		WP_VOYAGER_HYPO,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED weapon_borg_assimilator (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_borg_assimilator", 
		"sound/weapons/w_pkup.wav",
        "models/weapons2/toolkit/toolkit_w.md3",	//world
		"models/weapons2/toolkit/toolkit.md3",	//view
/* icon */		"icons/w_icon_toolkit",
/* pickup */	"Toolkit",
		AMMO_PHASER_CLIP,
		IT_WEAPON,
		WP_BORG_ASSIMILATOR,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED weapon_borg_weapon (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_borg_weapon", 
		"sound/weapons/w_pkup.wav",
        "models/weapons2/medkit/medkit_w.md3",	//world
		"models/weapons2/medkit/medkit.md3",	//view
/* icon */		"icons/w_icon_medkit",
/* pickup */	"Medkit",
		AMMO_PHASER_CLIP,
		IT_WEAPON,
		WP_BORG_WEAPON,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED weapon_tricorder (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_tricorder", 
		"sound/weapons/w_pkup.wav",
        "models/weapons2/tricorder/tricorder_w.md3",	//world
		"models/weapons2/tricorder/tricorder.md3",	//view
/* icon */		"icons/w_icon_tricorder",
/* pickup */	"Tricorder",
		AMMO_PHASER_CLIP,
		IT_WEAPON,
		WP_TRICORDER,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED weapon_padd (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_padd", 
		"sound/weapons/w_pkup.wav",
        "models/weapons2/padd/padd_w.md3",	//world
		"models/weapons2/padd/padd.md3",	//view
/* icon */		"icons/w_icon_padd",
/* pickup */	"Padd",
		AMMO_PHASER_CLIP,
		IT_WEAPON,
		WP_PADD,
/* precache */ "",
/* sounds */ ""
	},

/*QUAKED weapon_engtool (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_engtool", 
		"sound/weapons/w_pkup.wav",
        "models/weapons2/neutrino_probe/neutrino_probe_w.md3",	//world
		"models/weapons2/neutrino_probe/neutrino_probe.md3",	//view
/* icon */		"icons/w_icon_neutrinoprobe",
/* pickup */	"Neutrino Probe",
		AMMO_PHASER_CLIP,
		IT_WEAPON,
		WP_ENGTOOL,
/* precache */ "",
/* sounds */ ""
	},

	// end of list marker
	{NULL}
};

int		bg_numItems = sizeof(bg_itemlist) / sizeof(bg_itemlist[0]) - 1;

/*
==============
BG_FindItemWithClassname
==============
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

/*
==============
BG_FindClassnameForHoldable
==============
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

/*
==============
BG_FindItemForPowerup
==============
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


/*
==============
BG_FindItemForHoldable
==============
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


/*
===============
BG_FindItemForWeapon

===============
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

/*
===============
BG_FindItemForAmmo

===============
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

/*
===============
BG_FindItem

===============
*/
gitem_t	*BG_FindItem( const char *classname ) {
	gitem_t	*it;
	
	for ( it = bg_itemlist + 1 ; it->classname ; it++ ) {
		if ( !Q_stricmp( it->classname, classname) )
			return it;
	}

	return NULL;
}

/*
============
BG_PlayerTouchesItem

Items can be picked up without actually touching their physical bounds to make
grabbing them easier
============
*/
qboolean	BG_PlayerTouchesItem( playerState_t *ps, entityState_t *item, int atTime ) {
	vec3_t		origin;

	BG_EvaluateTrajectory( &item->pos, atTime, origin );

	// we are ignoring ducked differences here
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



/*
================
BG_CanItemBeGrabbed

Returns false if the item should not be picked up.
This needs to be the same for client side prediction and server use.


================
*/

qboolean	BG_CanItemBeGrabbed( const entityState_t *ent, const playerState_t *ps ) {
	gitem_t	*item;

//_______________________________________________________________
/* SYNC with g_items global var
int Max_Ammo[WP_NUM_WEAPONS] =
{
	0,						//		WP_NONE,
	50,						// WP_PHASER,	!! this should match PHASER_AMMO_MAX defined in bg_public
	128,					// WP_COMPRESSION_RIFLE,
	60,						// WP_IMOD,			
	100,					// WP_SCAVENGER_RIFLE,	
	50,						// WP_STASIS,			
	30,						// WP_GRENADE_LAUNCHER,
	120,					// WP_TETRION_DISRUPTOR,
	20,						// WP_QUANTUM_BURST,	
	120,					// WP_DREADNOUGHT,		
	100,					// WP_VOYAGER_HYPO,
	100,					// WP_BORG_ASSIMILATOR,
	100						// WP_BORG_WEAPON

};
//_______________________________________________________________
*/


	if ( ent->modelindex < 1 || ent->modelindex >= bg_numItems ) {
		//Com_Error( ERR_DROP, "BG_CanItemBeGrabbed: index out of range" );
		//Com_Printf ("BG_CanItemBeGrabbed: index out of range\n");
		return qfalse;
	}

	// The player used to not be able to pickup stuff when ghosted.  Now it automatically unghosts them.
//	if (ps->powerups[PW_GHOST])
//	{
//		return qfalse;
//	}

	item = &bg_itemlist[ent->modelindex];

	if ( ps->persistant[PERS_CLASS] == PC_BORG && item->giType != IT_TEAM )//FIXME: check for pModAssimilation mode being on
	{//borg cannot pick up anything but flags
		return qfalse;
	}

	switch( item->giType )
	{
	case IT_WEAPON:
		if ( ps->stats[STAT_WEAPONS] & (1 << item->giTag) && ps->ammo[ item->giTag ] >= Max_Ammo[ item->giTag ] )
		{
			return qfalse;
		}
		return qtrue;

	case IT_AMMO:
		if ( ps->ammo[ item->giTag ] >= Max_Ammo[ item->giTag ]) {
			return qfalse;					// can't hold any more
		}
		if ( ps->persistant[PERS_CLASS] != PC_NOCLASS )//FIXME: should be checking g_pModSpecialties->integer != 0 )
		{
			if ( ps->persistant[PERS_CLASS] != PC_ACTIONHERO )
			{
				//only pick it up if you have the weapon
				if ( !(ps->stats[STAT_WEAPONS]&(1<<item->giTag)) )
				{//don't have the weapon that uses this ammo
					return qfalse;
				}
			}
		}
		return qtrue;

	case IT_ARMOR:
		// we also clamp armor to the maxhealth for handicapping
		if ( ps->stats[STAT_ARMOR] >= ps->stats[STAT_MAX_HEALTH] * 2 ) {
			return qfalse;
		}
		return qtrue;

	case IT_HEALTH:
		// small and mega healths will go over the max, otherwise
		// don't pick up if already at max
		if ( item->quantity == 5 || item->quantity == 100 ) {
			if ( ps->stats[STAT_HEALTH] >= ps->stats[STAT_MAX_HEALTH] * 2 ) {
				return qfalse;
			}
			return qtrue;
		}

		if ( ps->stats[STAT_HEALTH] >= ps->stats[STAT_MAX_HEALTH] ) {
			return qfalse;
		}
		return qtrue;

	case IT_POWERUP:
		if ( ps->persistant[PERS_CLASS] == PC_ACTIONHERO )
		{
			if ( item->giTag == PW_REGEN )
			{
				return qfalse;
			}
		}
		else if ( ps->persistant[PERS_CLASS] != PC_NOCLASS )//FIXME: should be checking g_pModSpecialties->integer != 0 )
		{//in specialty mode, only certain classes can use certain powerups
			//FIXME: breaks bots!
			switch( item->giTag )
			{
			case PW_QUAD:
				if ( ps->persistant[PERS_CLASS] != PC_HEAVY )
				{
					return qfalse;
				}
				break;
			case PW_BATTLESUIT:
				if ( ps->persistant[PERS_CLASS] != PC_MEDIC )
				{
					return qfalse;
				}
				break;
			case PW_FLIGHT:
				if ( ps->persistant[PERS_CLASS] != PC_INFILTRATOR )
				{
					return qfalse;
				}
				break;
			}
		}
		return qtrue;	// powerups are always picked up

	case IT_TEAM: // team items, such as flags
		// ent->modelindex2 is non-zero on items if they are dropped
		// we need to know this because we can pick up our dropped flag (and return it)
		// but we can't pick up our flag at base
		if (ps->persistant[PERS_TEAM] == TEAM_RED) {
			if (item->giTag == PW_BLUEFLAG ||
				(item->giTag == PW_REDFLAG && ent->modelindex2) ||
				(item->giTag == PW_REDFLAG && ps->powerups[PW_BLUEFLAG]))
				return qtrue;
		} else if (ps->persistant[PERS_TEAM] == TEAM_BLUE) {
			if (item->giTag == PW_REDFLAG ||
				(item->giTag == PW_BLUEFLAG && ent->modelindex2) ||
				(item->giTag == PW_BLUEFLAG && ps->powerups[PW_REDFLAG]))
				return qtrue;
		}
		return qfalse;

	case IT_HOLDABLE:
		// can only hold one item at a time
		if ( ps->stats[STAT_HOLDABLE_ITEM] ) {
			return qfalse;
		}
		return qtrue;

        case IT_BAD:
            Com_Error( ERR_DROP, "BG_CanItemBeGrabbed: IT_BAD" );
	}

	return qfalse;
}

//======================================================================

/*
================
BG_EvaluateTrajectory

================
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
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
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
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		if ( deltaTime < 0 ) {
			deltaTime = 0;
		}
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		break;
	case TR_GRAVITY:
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		result[2] -= 0.5 * DEFAULT_GRAVITY * deltaTime * deltaTime;		// FIXME: local gravity...
		break;
	default:
		Com_Error( ERR_DROP, "BG_EvaluateTrajectory: unknown trType: %i", tr->trTime );
		break;
	}
}

/*
================
BG_EvaluateTrajectoryDelta

For determining velocity at a given time
================
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
		phase = cos( deltaTime * M_PI * 2 );	// derivative of sin = cos
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
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		VectorCopy( tr->trDelta, result );
		result[2] -= DEFAULT_GRAVITY * deltaTime;		// FIXME: local gravity...
		break;
	default:
		Com_Error( ERR_DROP, "BG_EvaluateTrajectoryDelta: unknown trType: %i", tr->trTime );
		break;
	}
}

/*
===============
BG_AddPredictableEventToPlayerstate

Handles the sequence numbers
===============
*/
void BG_AddPredictableEventToPlayerstate( int newEvent, int eventParm, playerState_t *ps ) {
	ps->events[ps->eventSequence & (MAX_PS_EVENTS-1)] = newEvent;
	ps->eventParms[ps->eventSequence & (MAX_PS_EVENTS-1)] = eventParm;
	ps->eventSequence++;
}


/*
========================
BG_PlayerStateToEntityState

This is done after each set of usercmd_t on the server,
and after local prediction on the client
========================
*/
void BG_PlayerStateToEntityState( playerState_t *ps, entityState_t *s, qboolean snap ) {
	int		i;

	if ( ps->pm_type == PM_INTERMISSION || ps->pm_type == PM_SPECTATOR ) {
		s->eType = ET_INVISIBLE;
	} else if ( ps->stats[STAT_HEALTH] <= GIB_HEALTH ) {
		s->eType = ET_INVISIBLE;
	} else {
		s->eType = ET_PLAYER;
	}

	s->number = ps->clientNum;

	s->pos.trType = TR_INTERPOLATE;
	VectorCopy( ps->origin, s->pos.trBase );
	if ( snap ) {
		SnapVector( s->pos.trBase );
	}

	s->apos.trType = TR_INTERPOLATE;
	VectorCopy( ps->viewangles, s->apos.trBase );
	if ( snap ) {
		SnapVector( s->apos.trBase );
	}

	s->angles2[YAW] = ps->movementDir;
	s->legsAnim = ps->legsAnim;
	s->torsoAnim = ps->torsoAnim;
	s->clientNum = ps->clientNum;		// ET_PLAYER looks here instead of at number
										// so corpses can also reference the proper config
	s->eFlags = ps->eFlags;
	if ( ps->stats[STAT_HEALTH] <= 0 ) {
		s->eFlags |= EF_DEAD;
	} else {
		s->eFlags &= ~EF_DEAD;
	}

	if ( ps->externalEvent ) {
		s->event = ps->externalEvent;
		s->eventParm = ps->externalEventParm;
	} else if ( ps->entityEventSequence < ps->eventSequence ) {
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
	for ( i = 0 ; i < MAX_POWERUPS ; i++ ) {
		if ( ps->powerups[ i ] ) {
			s->powerups |= 1 << i;
		}
	}
}

#define MAX_ITEMNAMES 45

const char *itemnames[MAX_ITEMNAMES] = 
{

"nothing",
"WEAPON_PHASER",
"WEAPON_COMPRESSIONRIFLE",
"WEAPON_IMOD",
"WEAPON_SCAVENGERRIFLE",
"WEAPON_STASIS",
"WEAPON_GRENADELAUNCHER",
"WEAPON_DISRUPTOR",
"WEAPON_QUANTUM",
"WEAPON_DREADNOUGHT",

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
"HOLOGRAPHIC_DECOY",	// decoy temp

"WEAPON_VOYAGER_HYPO",
"WEAPON_BORG_ASSIMILATOR",
"WEAPON_BORG_WEAPON",
"WEAPON_TRICORDER",
"WEAPON_PADD",
"WEAPON_ENGTOOL",
"WEAPON_TR116",

NULL
};


#define MAX_ITEMNAMEFILE	5000
char	itemNameBuffer[MAX_ITEMNAMEFILE];

/*
=================
BG_ParseItemsText
=================
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
						bg_itemlist[i].pickup_name = (buffer - (len + 1));	// The +1 is to get rid of the " at the beginning of the sting.
						*(buffer - 1) = '\0';		//	Place an string end where is belongs.
					}
				}

				break;
			}
			i++;
		}

	}
}

/*
BG_LanguageFilename - create a filename with an extension based on the value in g_language
*/
void BG_LanguageFilename(char *baseName,char *baseExtension,char *finalName)
{
	char	language[32];
	fileHandle_t	file;

	trap_Cvar_VariableStringBuffer( "g_language", language, 32 );

	// If it's English then no extension
	if (language[0]=='\0' || Q_stricmp ("ENGLISH",language)==0)
	{
		Com_sprintf(finalName,MAX_QPATH,"%s.%s",baseName,baseExtension);
	}
	else
	{
		Com_sprintf(finalName,MAX_QPATH,"%s_%s.%s",baseName,language,baseExtension);

		//Attempt to load the file
		trap_FS_FOpenFile( finalName, &file, FS_READ );

		if ( file == 0 )	//	This extension doesn't exist, go English.
		{
			Com_sprintf(finalName,MAX_QPATH,"%s.%s",baseName,baseExtension);	//the caller will give the error if this isn't there
		}
		else
		{
			trap_FS_FCloseFile( file );
		}
	}
}


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

	// initialise the data area
	memset(itemNameBuffer, 0, sizeof(itemNameBuffer));	

	trap_FS_Read( itemNameBuffer, len, f );

	trap_FS_FCloseFile( f );

	BG_ParseItemsText(itemNameBuffer);

}

/*
======================
G_ParseAnimationFileSex

Read a configuration file to get the sex
models/players2/munro/animation.cfg
======================
*/
static gender_t	G_ParseAnimationFileSex( const char *filename) {
	char		*text_p;
	int			len;
	char		*token;
	char		text[20000];
	fileHandle_t	f;
	char		animfile[MAX_QPATH];

	strcpy(animfile, filename);
	len = strlen(animfile);
	strcpy(&animfile[len-strlen("groups.cfg")], "animation.cfg");

	// load the file
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

	// parse the text
	text_p = text;

	// read optional parameters
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

/*
===============
RE_RegisterSkin

===============
*/
#define MAX_GROUP_FILE_SIZE	5000
char* BG_RegisterRace( const char *name ) {
	char	*text_p;
	char	*token;
	int		len;
	fileHandle_t	f;
	char	text[MAX_GROUP_FILE_SIZE];
	gender_t theSex;

	memset (races, 0, sizeof(races));
	memset (text, 0, sizeof(text));

	// load and parse the skin file
	len = trap_FS_FOpenFile( name, &f, FS_READ );
	if ( !f ) {
		// if we didn't get a races file, use an empty one.
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
		// get surface name
		token = COM_Parse( &text_p );

		if ( !token[0] ) {
			break;
		}

		// if we about to break the races size list then dump us out
		if (strlen(races) + strlen(token) > 256) {
			break;
		}

		// add it into the race list
		strcat(races, token);
		// put a comma between the names
		strcat(races, ",");

		if ( *text_p == ',' ) {
			text_p++;
		}

		if (!Q_stricmp ("borg", token) ) {
			if (theSex == GENDER_MALE) {
				// add it into the race list
				strcat(races, "BorgMale,");
			} else if (theSex == GENDER_FEMALE) {
				strcat(races, "BorgFemale,");
			} else {
			}
		}

	}

	// just in case
	if (!races[0])
	{
		Com_sprintf(races, sizeof(races), "unknown");
	}
	else
	{	//lose the last comma
		races[strlen(races)-1] = 0;
	}

	return races;
}



