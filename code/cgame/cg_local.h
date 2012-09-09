// Copyright (C) 1999-2000 Id Software, Inc.
//

#include "../game/q_shared.h"
#include "tr_types.h"
#include "../game/bg_public.h"
#include "cg_public.h"

// The entire cgame module is unloaded and reloaded on each level change,
// so there is NO persistant data between levels on the client side.
// If you absolutely need something stored, it can either be kept
// by the server in the server stored userinfos, or stashed in a cvar.

//RPG-X | Phenix | 13/02/2005
#define N00bSoundCount		12

#define	POWERUP_BLINKS		5

#define Q_FLASH_TIME 450

#define	POWERUP_BLINK_TIME	1000
#define	FADE_TIME			200
#define	PULSE_TIME			200
#define	DAMAGE_DEFLECT_TIME	100
#define	DAMAGE_RETURN_TIME	400
#define DAMAGE_TIME			500
#define	LAND_DEFLECT_TIME	150
#define	LAND_RETURN_TIME	300
#define	STEP_TIME			200
#define	DUCK_TIME			100
#define	PAIN_TWITCH_TIME	200
#define	WEAPON_SELECT_TIME	1400
#define	ITEM_SCALEUP_TIME	1000

// Zoom vars
#define	ZOOM_TIME			150
#define MAX_ZOOM_FOV		5.0f
#define ZOOM_IN_TIME		1000.0f	
#define ZOOM_OUT_TIME		100.0f
#define ZOOM_START_PERCENT	0.3f	

#define	ITEM_BLOB_TIME		200
#define	MUZZLE_FLASH_TIME	20
#define	SINK_TIME			1000		// time for fragments to sink into ground before going away
#define	ATTACKER_HEAD_TIME	10000
#define	REWARD_TIME			3000

#define	PULSE_SCALE			1.5			// amount to scale up the icons when activating

#define	MAX_STEP_CHANGE		32

#define	MAX_VERTS_ON_POLY	10
#define	MAX_MARK_POLYS		256

#define STAT_MINUS			10	// num frame for '-' stats digit

#define	ICON_SIZE			48
#define	CHAR_WIDTH			32
#define	CHAR_HEIGHT			48
#define	TEXT_ICON_SPACE		4

#define	TEAMCHAT_WIDTH		80
#define TEAMCHAT_HEIGHT		8

// very large characters
#define	GIANT_WIDTH			32
#define	GIANT_HEIGHT		48

#define NUM_FONT_BIG   1
#define NUM_FONT_SMALL 2

#define	NUM_CROSSHAIRS		12

#define MAX_OBJECTIVES		16
#define	MAX_OBJ_LENGTH		1024
#define	MAX_OBJ_TEXT_LENGTH	(MAX_OBJECTIVES*MAX_OBJ_LENGTH)

//beam defines used to calc player fading in beam sequence
#define PLAYER_BEAM_FADE	 1500	//RPG-X: TiM - Time index for when player model fades in/out in a beam sequence
#define PLAYER_BEAM_FADETIME 1000	//Length of time the player will be fading out

typedef enum {
	FOOTSTEP_NORMAL,
	FOOTSTEP_BORG,
	FOOTSTEP_REAVER,
	FOOTSTEP_SPECIES,
	FOOTSTEP_WARBOT,
	FOOTSTEP_METAL,
	FOOTSTEP_SPLASH,
	FOOTSTEP_BOOT,
	FOOTSTEP_GRASS,
	FOOTSTEP_GRAVEL,
	FOOTSTEP_SNOW,
	FOOTSTEP_WOOD,

	FOOTSTEP_TOTAL
} footstep_t;

//RPG-X | GSIO01 | 20/05/2009:
typedef enum {
	LANDSOUND_NORMAL,
	LANDSOUND_METAL,
	LANDSOUND_GRASS,
	LANDSOUND_GRAVEL,
	LANDSOUND_SNOW,
	LANDSOUND_WOOD,

	LANDSOUND_TOTAL
} landsound_t;

// For the holodoor intro

#define TIME_INIT		100
#define TIME_DOOR_START	1000
#define TIME_DOOR_DUR	(TIME_FADE_START-TIME_DOOR_START)
#define TIME_FADE_START	3000
#define TIME_FADE_DUR	(TIME_INTRO-TIME_FADE_START)

/*==============================================
TiM - Since the majority of the body models use the same anim config,
I'm going to adapt the same logic from JKA where common animations 
are stored in a global array and then whenever a new model is called,
a check is done to see we already have the anims parsed, rather than 
parse them allll over again. Might shave a few seconds off of loadtime 
and put them towards more precious roleplaying. :)

Hrm, they'll also need to be parsed CG and G for the emote system to fully
work.
*/

typedef struct animsList_s {
	char			animFileRoute[MAX_QPATH];

	animation_t		animations[MAX_ANIMATIONS];
} animsList_t;

//TiM: Borrowed from EF SP
//Data struct for necessary dynamic animation sound effects.
//Useful for proper footstep sync and emote-based sound FX.
#define MAX_ANIM_SOUNDS			64
#define	MAX_RANDOM_ANIMSOUNDS	8
typedef struct animsounds_s 
{
	int		keyFrame;							//Frame to play sound on
	int		soundIndex[MAX_RANDOM_ANIMSOUNDS];	//sound file to play - FIXME: should be an index, handle random some other way?
	int		numRandomAnimSounds;				//string variable min for va("...%d.wav", Q_irand(lowestVa, highestVa))
	int		probability;						//chance sound will play, zero value will not run this test (0 = 100% basically)
} animsounds_t;

typedef struct animsSndList_s {
	char			animSndFileRoute[MAX_QPATH];

	animsounds_t	upperAnimSounds[MAX_ANIM_SOUNDS];
	animsounds_t	lowerAnimSounds[MAX_ANIM_SOUNDS];
} animsSndList_t;

//TiM : JKA had 32, but since each potential char could have a
//dif anim list in EF, I'll allocate enough space if need be.
//Hopefully, if I've done my job, we won't actually ever exceed 2. :)
extern animsSndList_t		cg_animsSndList[MAX_CLIENTS];
extern int					cg_numSndAnims;

extern animsList_t			cg_animsList[MAX_CLIENTS];
extern int					cg_numAnims;

//Unlike EF SP, the anim and sound configs here are kept separately.
//I plan to make the animsounds config into a parameter in the model
//script file.  It won't use up more resources, and it'll give modders additional
//flexibilty if they wish

//================================================

//Struct to hold tricorder info strings
//typedef char tricString_t[MAX_TOKEN_CHARS];

/*=================================================
TiM: Rank Structs
The data in these structs are necessary for each seperate 
rank in order to be loaded and displayed in the client
base.
==================================================*/

//CLR_GREY, //Grey is only used if there is no real class or rank. So it's default
typedef enum {
	CLR_RED,
	CLR_GOLD,
	CLR_TEAL,
	CLR_GREEN,

	MAX_COLORS
} rankColor_t;

//Data needed for each rank entry in the TAB menu
typedef struct {
	float		w;
	float		h;

	int			s1;
	int			t1;

	int			s2;
	int			t2;

	int			gWidth;
	int			gHeight;

	qhandle_t	graphic;
} rankMenuData_t;

/*Data relating to the loadage
and displayage of rank pips
on player models.  I've set it up
this way so models are cached AS THEY ARE USED.
The point being to limit the number of shaders
we'll actually need cached at any given point.
That MAX_SHADERS error ticks me off... T_T
*/
typedef struct {
	char			boltModelPath[MAX_QPATH];
	qhandle_t		boltModel;

	char			boltShaderPath[MAX_QPATH];
	qhandle_t		boltShader;

	qboolean		triedToLoad;

	qboolean		admiralRank;
} rankModelData_t;

//POSSIBLE FIXME: A string value for each rank may be needed at some stage... not sure
//Yeah... we need the formal name for the tricorder scanner.
//And we'll also need the consoleName so we can list the possible choices if needed.
typedef struct {
	char			consoleName[MAX_QPATH];
	char			formalName[MAX_QPATH];

	rankMenuData_t	rankMenuData[MAX_COLORS];

	rankModelData_t	rankModelData;
} ranksData_t;

typedef struct {
	rankMenuData_t	rankMenuData;

	rankModelData_t	rankModelData;	
} defaultRankData_t;

#define MAX_CROSSHAIRS 15

//=================================================
//TiM: Importable - Custom Crosshair Data
typedef struct {
	qboolean	noDraw;

	vec4_t		color;

	int			s1;
	int			t1;

	int			s2;
	int			t2;

	//qhandle_t	graphic;
} crosshairsData_t;

//=================================================

typedef struct {
	char		formalName[25];
	int			radarColor[3];
	int			iconColor;
	qboolean	showRanks;
	qboolean	isMedic;
	qboolean	isBorg;
} cg_classData_t;

// player entities need to track more information
// than any other type of entity.

// note that not every player entity is a client entity,
// because corpses after respawn are outside the normal
// client numbering range

// when changing animation, set animationTime to frameTime + lerping time
// The current lerp will finish out, then it will lerp to the new animation
typedef struct {
	int			oldFrame;
	int			oldFrameTime;		// time when ->oldFrame was exactly on

	int			frame;
	int			frameTime;			// time when ->frame will be exactly on

	float		backlerp;

	float		yawAngle;
	qboolean	yawing;
	float		pitchAngle;
	qboolean	pitching;
	
	//TiM
	float		rollAngle;
	qboolean	rolling;
	//eTiM

	int			animationNumber;	// may include ANIM_TOGGLEBIT
	animation_t	*animation;
	int			animationTime;		// time when the first frame of the animation will be exact
} lerpFrame_t;


typedef struct {
	lerpFrame_t		head, legs, torso;
	int				painTime;
	int				painDirection;	// flip from 0 to 1
	qboolean		empty;
	int				lightningFiring;

	// machinegun spinning
	float			barrelAngle;
	int				barrelTime;
	qboolean		barrelSpinning;
} playerEntity_t;

//=================================================

//TiM - struct containing all the necessary data required
//for the SP style transporter. Due to really whacky design on id's behalf,
//this data has to be global enough for players AND weapons
//to accept it. :P 
typedef struct {
	//RPG-X | TiM - Time when a transport powerup is detected.
	//I originally only had one variable, but found trying to track two 
	//different powerups with one value was causing some undesired render effects.
	//This method is far better in the fact it means much cleaner render effects,
	//and also if there is a delay in the server-side teleport function, the visual FX
	//on this side will still remain in sync. :)
	//int				beamTime;
	qboolean		beamInDetected;
	float			beamAlpha;	//put in here so it's in scope for me to pass it to the weapons renderer
	int				beamTimeParam;
} beamData_t;

//=================================================


// centity_t have a direct corespondence with gentity_t in the game, but
// only the entityState_t is directly communicated to the cgame
typedef struct centity_s {
	entityState_t	currentState;	// from cg.frame
	entityState_t	nextState;		// from cg.nextFrame, if available
	qboolean		interpolate;	// true if next is valid to interpolate to
	qboolean		currentValid;	// true if cg.frame holds this entity

	int				muzzleFlashTime;	// move to playerEntity?
	int				previousEvent;
	int				thinkFlag;

	int				trailTime;		// so missile trails can handle dropped initial packets
	int				miscTime;

	vec3_t			damageAngles;
	int				damageTime;
	int				deathTime;

	playerEntity_t	pe;

	beamData_t		beamData;
	
	int				cloakTime;		//time index we started cloaking, for the Q-Flash
	int				decloakTime;
	qboolean		wasCloaked;

	// Unused
	int				errorTime;		// decay the error from this time
//	vec3_t			errorOrigin;
//	vec3_t			errorAngles;
	
	qboolean		extrapolated;	// false if origin / angles is an interpolation
	vec3_t			rawOrigin;
	vec3_t			rawAngles;

	vec3_t			beamEnd;

	// exact interpolated position of entity on this frame
	vec3_t			lerpOrigin;
	vec3_t			lerpAngles;

	int				turboTime;
	//qboolean		unclampAngles;			//TiM - Unstor player clamp angles
	qboolean		clampAngles;
} centity_t;

//TiM - stores the time index of each client to be used when they enter a turbolift.
//it's been placed here to ensure no changes will be made to it, unlike the cent structs
extern int cg_liftEnts[MAX_CLIENTS];

//======================================================================

// local entities are created as a result of events or predicted actions,
// and live independantly from all server transmitted entities

typedef struct markPoly_s {
	struct markPoly_s	*prevMark, *nextMark;
	int			time;
	qhandle_t	markShader;
	qboolean	alphaFade;		// fade alpha instead of rgb
	float		color[4];
	poly_t		poly;
	polyVert_t	verts[MAX_VERTS_ON_POLY];
} markPoly_t;


typedef enum {
	LE_MARK,
	LE_EXPLOSION,
	LE_SPRITE_EXPLOSION,
	LE_MOVE_SCALE_FADE,
	LE_FALL_SCALE_FADE,
	LE_FADE_RGB,
	LE_SCALE_FADE,
	LE_SCALE_FADE_SPRITE,
	LE_SCALE_SINE, //RPG-X: TiM
	LE_LINE,
	LE_LINE2,
	LE_OLINE,
	LE_TRAIL,
	LE_VIEWSPRITE,
	LE_BEZIER,
	LE_QUAD,
	LE_CYLINDER,
	LE_ELECTRICITY,
	LE_PARTICLE,
	LE_SPAWNER,
	LE_FRAGMENT,
	LE_STASISDOOR
} leType_t;

typedef enum {
	LEF_NONE			= 0x0000,			// Use for "no flag"
	LEF_PUFF_DONT_SCALE = 0x0001,			// do not scale size over time
	LEF_TUMBLE			= 0x0002,			// tumble over time, used for ejecting shells
	LEF_MOVE			= 0x0004,			// Sprites and models and trails use this when they use velocity
	LEF_USE_COLLISION	= 0x0008,			// Sprites, models and trails use this when they want to collide and bounce.
	LEF_ONE_FRAME		= 0x0010,			// One frame only is to be shown of the entity.
	LEF_ONE_FRAME_DONE	= 0x0020,			// This bit is set AFTER the one frame is shown.
	LEF_FADE_RGB		= 0x0040,			// Force fading through color.
	LEF_SINE_SCALE		= 0x0080,			// TiM-Used for non-linear ratio calcs
	LEF_REVERSE_SCALE	= 0x0100			// TiM-Make the scale decrease instead of increase
} leFlag_t;

typedef enum {
	LEMT_NONE,
} leMarkType_t;			// fragment local entities can leave marks on walls

typedef struct localEntity_s {
	struct localEntity_s	*prev, *next;
	leType_t		leType;
	int				leFlags;

	int				startTime;
	int				endTime;

	float			lifeRate;			// 1.0 / (endTime - startTime)

	trajectory_t	pos;
	trajectory_t	angles;

	float			bounceFactor;		// 0.0 = no bounce, 1.0 = perfect

	float			color[4];

	float			frameRate;
	int				numFrames;

//	float			radius;
//	float			dradius;

//	float			length;
//	float			dlength;

	float			alpha;
	float			dalpha;

	float			shaderRate;
	int				numShaders;
	qhandle_t		*leShaders;

	float			light;
	vec3_t			lightColor;

	leMarkType_t		leMarkType;

	union {
		struct {
			float radius;
			float dradius;
			vec3_t startRGB;
			vec3_t dRGB;
		} sprite;
		struct {
			float width;
			float dwidth;
			float length;
			float dlength;
			vec3_t startRGB;
			vec3_t dRGB;
		} trail;
		struct {
			float width;
			float dwidth;
			// Below are bezier specific.
			vec3_t			control1;				// initial position of control points
			vec3_t			control2;
			vec3_t			control1_velocity;		// initial velocity of control points
			vec3_t			control2_velocity;
			vec3_t			control1_acceleration;	// constant acceleration of control points
			vec3_t			control2_acceleration;
		} line;
		struct {
			float width;
			float dwidth;
			float width2;
			float dwidth2;
			vec3_t startRGB;
			vec3_t dRGB;
		} line2;
		struct {
			float width;
			float dwidth;
			float width2;
			float dwidth2;
			float height;
			float dheight;
		} cylinder;
		struct {
			float width;
			float dwidth;
		} electricity;
		struct
		{
			// fight the power! open and close brackets in the same column!
			float radius;
			float dradius;
			qboolean (*thinkFn)(struct localEntity_s *le);
			vec3_t	dir;	// magnitude is 1, but this is oldpos - newpos right before the
							//particle is sent to the renderer
			// may want to add something like particle::localEntity_s *le (for the particle's think fn)
		} particle;
		struct
		{
			qboolean	dontDie;
			vec3_t		dir;
			float		variance;
			int			delay;
			int			nextthink;
			qboolean	(*thinkFn)(struct localEntity_s *le);
			int			data1;
			int			data2;
		} spawner;
		struct
		{
			float radius;
		} fragment;
	} data;

	refEntity_t		refEntity;	

	vec3_t	addOrigin;
} localEntity_t;

// kef -- there may well be a cleaner way of doing this, but the think functions for particles
//will need access to these CG_AddXXXXXX functions, so...
void CG_AddMoveScaleFade( localEntity_t *le );
void CG_AddScaleFade( localEntity_t *le );
void CG_AddFallScaleFade( localEntity_t *le );
void CG_AddExplosion( localEntity_t *ex );
void CG_AddSpriteExplosion( localEntity_t *le );
void CG_AddScaleFadeSprite( localEntity_t *le );
void CG_AddQuad( localEntity_t *le );
void CG_AddLine( localEntity_t *le );
void CG_AddOLine( localEntity_t *le );
void CG_AddLine2( localEntity_t *le );
void CG_AddTrail( localEntity_t *le ); 
void CG_AddViewSprite( localEntity_t *le );
void CG_AddBezier( localEntity_t *le );
void CG_AddCylinder( localEntity_t *le );
void CG_AddElectricity( localEntity_t *le );
// IMPORTANT!! Don't make CG_AddParticle or CG_AddSpawner available here to prevent unpalateable recursion possibilities
//

//======================================================================


typedef struct {
	int				client;
	int				score;
	int				ping;
	int				time;
	int				scoreFlags;
//	int				faveTarget;
//	int				faveTargetKills;	// # of times you killed fave target
	int				worstEnemy;
	int				worstEnemyKills;	// # of times worst Enemy Killed you
	int				faveWeapon;			// your favorite weapon
	int				killedCnt;			// # of times you were killed
} score_t;

//**********************************************************
//RPG-X Player Model Additional Data
//This is the struct where all the data loaded from .model files
//is parsed to.  From there, it can be plugged straight into
//the original model loading pipeline.... I think
//**********************************************************
#define MAX_BOLTONS		10
#define MAX_TALK_SKINS	4

typedef enum {
	BOLTON_HEAD = 0,
	BOLTON_TORSO,
	BOLTON_LEGS,
	BOLTON_MAX
} boltonLoc_t;

//min and max value for any timed events
typedef struct {
	int nextTime;

	int minSeconds;
	int maxSeconds;
} charSecs_t;

typedef struct {
	int			modelBase;
	char		tagName[MAX_QPATH];
	qhandle_t	tagModel;
} boltonTags_t;


// each client has an associated clientInfo_t
// that contains media references necessary to present the
// client model and other color coded effects
// this is regenerated each time a client's configstring changes,
// usually as a result of a userinfo (name, model, etc) change
#define	MAX_CUSTOM_SOUNDS	32
typedef struct {
	qboolean		infoValid;

	char			name[MAX_QPATH];
	team_t			team;
	pclass_t		pClass;

	int				botSkill;		// 0 = not bot, 1-5 = bot

	vec3_t			color;

	int				score;			// updated by score servercmds
	int				location;		// location index for team mode
	int				health;			// you only get this info about your teammates
	int				armor;
	int				curWeapon;

	int				handicap;
	int				wins, losses;	// in tourney mode

	int				powerups;		// so can display quad/flag status
	qboolean		eliminated;		// so can display quad/flag status

	qboolean		hasRanks;		//displays ranks on model or not

	// when clientinfo is changed, the loading of models/skins/sounds
	// can be deferred until you are dead, to prevent hitches in
	// gameplay
	char			charName[MAX_QPATH];
	char			modelName[MAX_QPATH];
	char			skinName[MAX_QPATH];
	qboolean		deferred;

	vec3_t			headOffset;		// move head in icon views
	footstep_t		footsteps;
	gender_t		gender;			// from model
	char			soundPath[MAX_QPATH];	//from model

	qhandle_t		legsModel;
	qhandle_t		legsSkin;

	qhandle_t		torsoModel;
	qhandle_t		torsoSkin;

	qhandle_t		headModel;
	qhandle_t		headSkin;

	//int			numBoltOns;
	boltonTags_t	boltonTags[MAX_BOLTONS];

	//TiM - Additional Parameters
	qhandle_t		headSkinBlink;

	//frowning
	qhandle_t		headSkinFrown;
	qhandle_t		headSkinFrownBlink;

	charSecs_t		headBlinkTime;
	//charSecs_t	headFrownTime;

	//TiM
	qhandle_t		headSkinTalk[MAX_TALK_SKINS];
	int				nextTalkTime;
	int				currentTalkSkin;

	int				nextTalkAngle; //Time index until the new talk angles are calculated
	int				talkDifferential; //Length of time between changes, without cg.time added
	vec3_t			talkAngles; //decimal values to offset the head orientation
	int				headDebounceTime; //Time to spend checking for talking checks
	
	qhandle_t		holsterModel; //RPG-X : TiM
	//--

	//char			race[256];

	qhandle_t		modelIcon;

	//TiM - Whoa... this thing is pretty huge now thanks to the 50+ anims I added :/
	//animation_t		animations[MAX_ANIMATIONS];
	//animation_t		*animations;
	int				animIndex; //index to the array cell with the anim data we want in it. :)
	int				animSndIndex; //index to the array cell where the sound data for certain anims is kept.

	sfxHandle_t		sounds[MAX_CUSTOM_SOUNDS];
	int				numTaunts;

	qboolean		animsFlushed;

	//Player Model System Custom Parameters
	//TiM
	char			age[MAX_NAME_LENGTH];
	float			height;
	float			weight;
	char			race[MAX_NAME_LENGTH];
	//--

	int			modelOffset;

	qboolean	isAdmin;	//local store to determine if client is an admin or not (for both class and login)
	
	qboolean	isHazardModel;	//yeh lame lol

	int			silentCloak;
} clientInfo_t;


// each WP_* weapon enum has an associated weaponInfo_t
// that contains media references necessary to present the
// weapon and its effects
typedef struct weaponInfo_s {
	qboolean		registered;
	gitem_t			*item;

	qhandle_t		handsModel;			// the hands don't actually draw, they just position the weapon
	qhandle_t		weaponModel;		// this is the pickup model
	qhandle_t		viewModel;			// this is the in-view model used by the player
	qhandle_t		barrelModel[4];		// Trek weapons have lots of barrels
	qhandle_t		flashModel;

	vec3_t			weaponMidpoint;		// so it will rotate centered instead of by tag

	vec3_t			flashDlightColor;

	sfxHandle_t		flashSound;		// fast firing weapons randomly choose
	sfxHandle_t		altFlashSnd;
	sfxHandle_t		stopSound;
	sfxHandle_t		altStopSound;
	sfxHandle_t		firingSound;
	sfxHandle_t		altFiringSound;
//	sfxHandle_t		missileSound;
	sfxHandle_t		alt_missileSound;
	sfxHandle_t		mainHitSound;
	sfxHandle_t		altHitSound;

	qhandle_t		weaponIcon;

//	qhandle_t		ammoModel;

	qhandle_t		missileModel;
	void			(*missileTrailFunc)( centity_t *, const struct weaponInfo_s *wi );
	float			missileDlight;
	vec3_t			missileDlightColor;

	qhandle_t		alt_missileModel;
	void			(*alt_missileTrailFunc)( centity_t *, const struct weaponInfo_s *wi );
	float			alt_missileDlight;

	qboolean		isAnimSndBased;	//TiM - play this sound, only if the player has no anim sound cfg.
} weaponInfo_t;


// each IT_* item has an associated itemInfo_t
// that constains media references necessary to present the
// item and its effects
typedef struct {
	qboolean		registered;
	qhandle_t		model;
	qhandle_t		icon;
} itemInfo_t;


typedef struct {
	int				itemNum;
} powerupInfo_t;

typedef struct {
	char		text[MAX_OBJ_LENGTH];
	char		abridgedText[MAX_OBJ_LENGTH];
	qboolean	complete;
} objective_t;

//TiM - Data needed for FPS Body CVAR
typedef struct {
	int		anim;
	int		offset;
	float	sizeOffset;
} fpsBody_t;

//======================================================================

// all cg.stepTime, cg.duckTime, cg.landTime, etc are set to cg.time when the action
// occurs, and they will have visible effects for #define STEP_TIME or whatever msec after
 
typedef struct {
	int			clientFrame;		// incremented each frame
	
	qboolean	demoPlayback;
	qboolean	levelShot;			// taking a level menu screenshot
	int			deferredPlayerLoading;
	qboolean	loading;			// don't defer players at initial startup
	qboolean	intermissionStarted;	// don't play voice rewards, because game will end shortly

	// there are only one or two snapshot_t that are relevent at a time
	int			latestSnapshotNum;	// the number of snapshots the client system has received
	int			latestSnapshotTime;	// the time from latestSnapshotNum, so we don't need to read the snapshot yet

	snapshot_t	*snap;				// cg.snap->serverTime <= cg.time
	snapshot_t	*nextSnap;			// cg.nextSnap->serverTime > cg.time, or NULL
	snapshot_t	activeSnapshots[2];

	float		frameInterpolation;	// (float)( cg.time - cg.frame->serverTime ) / (cg.nextFrame->serverTime - cg.frame->serverTime)

	qboolean	thisFrameTeleport;
	qboolean	nextFrameTeleport;

	int			frametime;		// cg.time - cg.oldTime

	int			time;			// this is the time value that the client
								// is rendering at.
	int			oldTime;		// time at last frame, used for missile trails and prediction checking

	int			physicsTime;	// either cg.snap->time or cg.nextSnap->time

	int			timelimitWarnings;	// 5 min, 1 min, overtime
	int			fraglimitWarnings;

	qboolean	renderingThirdPerson;		// during deaths, chasecams, etc

	// prediction state
	qboolean	hyperspace;				// true if prediction has hit a trigger_teleport
	playerState_t	predictedPlayerState;
	centity_t		predictedPlayerEntity;
	qboolean	validPPS;				// clear until the first call to CG_PredictPlayerState
	int			predictedErrorTime;
	vec3_t		predictedError;

	float		stepChange;				// for stair up smoothing
	int			stepTime;

	float		duckChange;				// for duck viewheight smoothing
	int			duckTime;

	float		landChange;				// for landing hard
	int			landTime;

	// input state sent to server
	int			weaponSelect;

	// auto rotating items
	vec3_t		autoAngles;
	vec3_t		autoAxis[3];
	vec3_t		autoAnglesFast;
	vec3_t		autoAxisFast[3];

	// view rendering
	refdef_t	refdef;
	vec3_t		refdefViewAngles;		// will be converted to refdef.viewaxis

	// zoom key
	qboolean	zoomed;
	qboolean	zoomLocked;
	int			zoomTime;
	float		zoomSensitivity;

	// information screen text during loading
	char		infoScreenText[MAX_STRING_CHARS];

	// scoreboard
	int			scoresRequestTime;
	int			numScores;
	int			teamScores[2];
	score_t		scores[MAX_CLIENTS];
	qboolean	showScores;
	int			scoreFadeTime;
	char		killerName[MAX_NAME_LENGTH];

	// centerprinting
	int			centerPrintTime;
	int			centerPrintCharWidth;
	int			centerPrintY;
	char		centerPrint[1024];
	int			centerPrintLines;

	// low ammo warning state
	int			lowAmmoWarning;		// 1 = low, 2 = empty

	// kill timers for carnage reward
	int			lastKillTime;

	// crosshair client ID
	int			crosshairClientNum;
	int			crosshairClientTime;

	// powerup active flashing
	int			powerupActive;
	int			powerupTime;

	// attacking player
	int			attackerTime;

	// reward medals
	int			rewardTime;
	int			rewardCount;
	qhandle_t	rewardShader;

	// warmup countdown
	int			warmup;
	int			warmupCount;

	//==========================

	int			itemPickup;
	int			itemPickupTime;
	int			itemPickupBlendTime;	// the pulse around the crosshair is timed seperately

	int			weaponSelectTime;
	int			weaponAnimation;
	int			weaponAnimationTime;

	// blend blobs
	float		damageTime;
	float		damageX, damageY, damageValue, damageShieldValue;

	// status bar head
	//RPG-X | Phenix | 09/06/2005
	// Raven commented this out!
	float		headYaw;
	float		headEndPitch;
	float		headEndYaw;
	int			headEndTime;
	float		headStartPitch;
	float		headStartYaw;
	int			headStartTime;

	int			interfaceStartupTime;	// Timer for menu graphics
	int			interfaceStartupDone;	// True when menu is done starting up

	// view movement
	float		v_dmg_time;
	float		v_dmg_pitch;
	float		v_dmg_roll;

	vec3_t		kick_angles;	// weapon kicks
	vec3_t		kick_origin;

	// temp working variables for player view
	float		bobfracsin;
	int			bobcycle;
	float		xyspeed;

	//Shake information
	float		shake_intensity;
	int			shake_duration;
	int			shake_start;

	int			shake_serverIndex;	//end time for the server so client-side shakes don't stop it
	//TiM : RPG-X Shake Info To try and make it look better
	int			shake_nextLerp, shake_lastLerp; //next cg.time value where a new random lerp value will be chosen (Updating per frame makes me motion sick due to its l33t jerkiness )
	vec3_t		shake_LerpAngle, shake_LerpOrigin; //Relevant data to lerp to
	vec3_t		shake_LastAngle, shake_LastOrigin;

	// development tool
	refEntity_t		testModelEntity;
	char			testModelName[MAX_QPATH];
	qboolean		testGun;

	int			loadLCARSStage;
	int			loadLCARScnt;
	qboolean	showObjectives;
	int			mod;//method O' death

	//RPG-X | Phenix | 08/06/2005
	//RPG-X | TiM    | 13/2/2006 - Moved here for standardisation
	int		adminMsgTime;
	char	adminMsgMsg[MAX_OBJ_LENGTH];

	//TiM : ThirdPersonZoom Booleans
	//These are inserted into the thirdperson camera func
	qboolean	zoomedForward;
	qboolean	zoomedBackward;
	qboolean	zoomedLeft;
	qboolean	zoomedRight;
	qboolean	zoomedUp;
	qboolean	zoomedDown;

	qboolean	zoomAngleLeft;
	qboolean	zoomAngleRight;

	qboolean	zoomPitchDown;
	qboolean	zoomPitchUp;

	//TiM - The body in FPS mode
	fpsBody_t	fpsBody;

	//TiM - A local boolean for the TPS view
	qboolean	thirdPersonNoLerp;

	// cinematics	
	int					cinematicFade;
} cg_t;


typedef enum
{
	MT_NONE = 0,
	MT_METAL,
	MT_GLASS,
	MT_GLASS_METAL,
	MT_WOOD,
	MT_STONE,

	NUM_CHUNK_TYPES

} material_type_t;

#define NUM_CHUNKS		6

// all of the model, shader, and sound references that are
// loaded at gamestate time are stored in cgMedia_t
// Other media that can be tied to clients, weapons, or items are
// stored in the clientInfo_t, itemInfo_t, weaponInfo_t, and powerupInfo_t
typedef struct {
	qhandle_t	charsetShader;
	qhandle_t	charsetPropTiny;
	qhandle_t	charsetProp;
	qhandle_t	charsetPropBig;
//	qhandle_t	charsetPropGlow;
	qhandle_t	charsetPropB;
	qhandle_t	whiteShader;
	qhandle_t	white2Shader;

	qhandle_t	redFlagModel;
	qhandle_t	blueFlagModel;
	qhandle_t	redFlagShader[4];
	qhandle_t	blueFlagShader[4];

	qhandle_t	teamStatusBar;

	qhandle_t	deferShader;
	//qhandle_t	eliminatedShader;

//	recently added Trek shaders

	qhandle_t	phaserShader;
	qhandle_t	phaserEmptyShader;
	qhandle_t	phaserAltShader;
	qhandle_t	phaserAltEmptyShader;
	qhandle_t	phaserMuzzleEmptyShader;

	qhandle_t	testDetpackShader3;
	qhandle_t	testDetpackRingShader1;
	qhandle_t	testDetpackRingShader2;
	qhandle_t	testDetpackRingShader3;
	qhandle_t	testDetpackRingShader4;
	qhandle_t	testDetpackRingShader5;
	qhandle_t	testDetpackRingShader6;

	qhandle_t	detpackPlacedIcon;

	//qhandle_t	dnBoltShader;

	//qhandle_t	stasisRingShader;
	//qhandle_t	stasisAltShader;

	qhandle_t	disruptorBolt;
	qhandle_t	disruptorStreak;
		
	qhandle_t	whiteRingShader;
	qhandle_t	orangeRingShader;
	qhandle_t	quantumExplosionShader;
	qhandle_t	quantumFlashShader;

	//qhandle_t	scavengerAltShader;

	/*qhandle_t	imodExplosionShader;
	qhandle_t	IMODShader;
	qhandle_t	IMOD2Shader;
	qhandle_t	altIMODShader;
	qhandle_t	altIMOD2Shader;*/

	qhandle_t	prifleBeam;
	qhandle_t	prifleImpactShader;
	qhandle_t	compressionAltBeamShader;
	qhandle_t	compressionAltBlastShader;
	qhandle_t	prifleBolt;
	qhandle_t	flashlightModel;

	qhandle_t	disruptorBeam;
	//qhandle_t	greenBurstShader;
	//qhandle_t	greenTrailShader;
	//qhandle_t	tetrionTrail2Shader;
	//qhandle_t	tetrionFlareShader;
	qhandle_t	borgFlareShader;
	//qhandle_t	bulletmarksShader;
	qhandle_t	borgLightningShaders[4];

	//qhandle_t	bigBoomShader;
	//qhandle_t	scavExplosionFastShader;
	//qhandle_t	scavExplosionSlowShader;

	qhandle_t	sunnyFlareShader;
	qhandle_t	scavMarkShader;
	qhandle_t	sparkShader;
	qhandle_t	spark2Shader;
	qhandle_t	steamShader;
	qhandle_t	fireShader; //RPG-X | Marcin | 24/12/2008
	qhandle_t	fire2Shader; //RPG-X | Marcin | 24/12/2008
	qhandle_t	fire3Shader; //RPG-X | Marcin | 24/12/2008
	qhandle_t	smokeShader;
	//qhandle_t	IMODMarkShader;
	qhandle_t	waterDropShader;
	qhandle_t	oilDropShader;
	qhandle_t	greenDropShader;

	//RPG-X | GSIO01 | 11/05/2009:
	qhandle_t	quantumGlow;
	qhandle_t	quantumRays;
	qhandle_t	photonGlow;
	qhandle_t	photonRay;
	qhandle_t	fireParticle;

	qhandle_t	explosionModel;
	qhandle_t	nukeModel;
	//qhandle_t	electricalExplosionFastShader;// These are used to have a bit of variation in the explosions
	qhandle_t	electricalExplosionSlowShader;
	qhandle_t	surfaceExplosionShader;
	//qhandle_t	purpleParticleShader;
	qhandle_t	blueParticleShader;
	qhandle_t	ltblueParticleShader;

	qhandle_t	yellowParticleShader;
	qhandle_t	orangeParticleShader;
	qhandle_t	dkorangeParticleShader;
	qhandle_t	orangeTrailShader;
	qhandle_t	compressionMarkShader;

	qhandle_t	redFlareShader;
	qhandle_t	redRingShader;
	qhandle_t	redRing2Shader;
	qhandle_t	bigShockShader;
	qhandle_t	plasmaShader;
	qhandle_t	bolt2Shader;
	qhandle_t	quantumRingShader;

	//qhandle_t	holoOuchShader;
	qhandle_t	painBlobShader;
	qhandle_t	painShieldBlobShader;
	//qhandle_t	shieldBlobShader;
	//qhandle_t	halfShieldShader;
	//qhandle_t	holoDecoyShader;

	qhandle_t	trans1Shader;
	qhandle_t	trans2Shader;

	//TiM - SP Transporter FX shaders
	qhandle_t	transport1Shader;
	qhandle_t	transport2Shader;

	qhandle_t	fountainShader;
	qhandle_t	rippleShader;
//	end recently added Trek shaders

	//qhandle_t	teamRedShader;
	//qhandle_t	teamBlueShader;

	//qhandle_t	chatShader;
	qhandle_t	connectionShader;

	//qhandle_t	selectShader;
	//qhandle_t	crosshairShader[NUM_CROSSHAIRS];
	qhandle_t	laserShader; // LASER

	qhandle_t	lagometerShader;
	qhandle_t	backTileShader;
	//qhandle_t	noammoShader;

	qhandle_t	smokePuffRageProShader;
	qhandle_t	waterBubbleShader;

	qhandle_t	numberShaders[11];
	qhandle_t	smallnumberShaders[11];

	qhandle_t	shadowMarkShader;

	//qhandle_t	botSkillShaders[5];

	//TiM
	//qhandle_t	pClassShaders[NUM_PLAYER_CLASSES];
	//qhandle_t	borgIconShader;
	//qhandle_t	borgQueenIconShader;
	//qhandle_t	heroSpriteShader;
	//-TiM


	// Zoom
	qhandle_t	zoomMaskShader;
	qhandle_t	zoomMask116Shader;
	qhandle_t	zoomGlow116Shader;
	/*qhandle_t	zoomBarShader;
	qhandle_t	zoomInsertShader;
	qhandle_t	zoomArrowShader;
	qhandle_t	ammoslider;*/

	// wall mark shaders
	qhandle_t	wakeMarkShader;
	qhandle_t	holeMarkShader;
	qhandle_t	energyMarkShader;	

	// powerup shaders
	//qhandle_t	quadShader;
	//qhandle_t	redQuadShader;
	//qhandle_t	quadWeaponShader;
	//qhandle_t	invisShader;
	//qhandle_t	regenShader;
	//qhandle_t	battleSuitShader;
	//qhandle_t	battleWeaponShader;
	//qhandle_t	hastePuffShader;
	//qhandle_t	flightPuffShader;
	//qhandle_t	seekerModel;
	qhandle_t	disruptorShader;
	qhandle_t	explodeShellShader;
	qhandle_t	quantumDisruptorShader;
	qhandle_t	borgFullBodyShieldShader;

	qhandle_t	transportShader; //RPG-X : TiM - SP Transport Shader
	qhandle_t	transportKlingon;
	qhandle_t	transportRomulan;
	qhandle_t	transportCardassian;

	qhandle_t	weaponPlaceholderShader;
	qhandle_t	rezOutShader;
	qhandle_t	electricBodyShader;

	//eyebeam/tripwire shaders
	qhandle_t	whiteLaserShader;
	qhandle_t	borgEyeFlareShader;

	// weapon effect models
	//qhandle_t	ringFlashModel;

	// weapon effect shaders
	//qhandle_t	bloodExplosionShader;

	// special effects models
	qhandle_t	teleportEffectModel;
	qhandle_t	teleportEffectShader;
	qhandle_t	shieldActivateShaderBlue;
	//RPG-X START | GSIO01 | 09/05/2009
	qhandle_t	shieldActivateShaderBorg;
	qhandle_t	shieldActivateShaderYellow;
	//RPG-Y END
	//qhandle_t	shieldDamageShaderBlue;
	qhandle_t	shieldActivateShaderRed;
	qhandle_t	shieldDamageShaderRed;
	
	qhandle_t	holoDoorModel;
	qhandle_t	chunkModels[NUM_CHUNK_TYPES][NUM_CHUNKS];
	
	//RPG-X: RedTechie - Register Endcaps for scoreboard
	qhandle_t	scoreboardtopleft;
	qhandle_t	scoreboardtopright;
	qhandle_t	scoreboardbotleft;
	qhandle_t	scoreboardbotright;

	//RPG-X: RedTechie - Register new healthbar
	qhandle_t	healthbigcurve;
	qhandle_t	healthendcap;

	//TiM
	qhandle_t	healthSineWave;
	
	//RPG-X: RedTechie - Registered Cloak Sprite
	qhandle_t	cloakspriteShader;

	qhandle_t	scoreboardEndcap;
	qhandle_t	weaponbox;
	qhandle_t	weaponbox2;
	qhandle_t	corner_12_24;
	qhandle_t	corner_8_16_b;

	qhandle_t	weaponcap1;
	qhandle_t	weaponcap2;

	// medals shown during gameplay
	/*qhandle_t	medalImpressive;
	qhandle_t	medalExcellent;
	qhandle_t	medalFirstStrike;
	qhandle_t	medalAce;
	qhandle_t	medalExpert;
	qhandle_t	medalMaster;
	qhandle_t	medalChampion;*/

	// Holodeck doors
	//qhandle_t	doorbox;

	//RPG-X Mod
	//qhandle_t	rpgxlogo;
	qhandle_t	greenParticleShader;
	qhandle_t	greenParticleStreakShader;
	//qhandle_t	blueParticleStreakShader;

	qhandle_t	liteRedParticleStreakShader;
	qhandle_t	liteRedParticleShader;

	qhandle_t	probeBeam;
	qhandle_t	probeDecal;

	qhandle_t	regenDecal;

	//RPG-X : Weapon Holsters
	qhandle_t	phaserHolster;
	qhandle_t	phaserHolsterInner;

	qhandle_t	tricorderHolster;
	qhandle_t	tricorderHolsterInner;

	//TR-116 Scope
	qhandle_t	tr116EyeScope;

	//SIMS module
	qhandle_t	simsModule;

	//EVA
	qhandle_t	evaInterior;

	qhandle_t	medicalScanner;

	//hazard Helmet
	qhandle_t	hazardHelmet;

	// stasis door model
	qhandle_t   stasisDoorModel;

	// sounds
	//sfxHandle_t	quadSound;
	//sfxHandle_t	selectSound;
	sfxHandle_t	useNothingSound;
	//sfxHandle_t	wearOffSound;
	sfxHandle_t	footsteps[FOOTSTEP_TOTAL][4];
	sfxHandle_t	holoOpenSound;
	sfxHandle_t	teleInSound;
	//sfxHandle_t	teleOutSound;
	//RPG-X
	sfxHandle_t	transportSound;
	//sfxHandle_t	noAmmoSound;
	sfxHandle_t	respawnSound;
	sfxHandle_t talkSound;
	//sfxHandle_t landSound;
	sfxHandle_t landSound[LANDSOUND_TOTAL]; //RPG-X | GSIO01 | 20/05/2009
	sfxHandle_t jumpPadSound;

	sfxHandle_t oneMinuteSound;
	sfxHandle_t fiveMinuteSound;
	sfxHandle_t suddenDeathSound;

	//sfxHandle_t threeFragSound;
	//sfxHandle_t twoFragSound;
	//sfxHandle_t oneFragSound;

	//sfxHandle_t hitSound;
	//sfxHandle_t shieldHitSound;
	//sfxHandle_t shieldPierceSound;
	//sfxHandle_t hitTeamSound;

	//sfxHandle_t rewardImpressiveSound;
	//sfxHandle_t rewardExcellentSound;
	//sfxHandle_t rewardDeniedSound;
	//sfxHandle_t rewardFirstStrikeSound;
	//sfxHandle_t rewardAceSound;
	//sfxHandle_t rewardExpertSound;
	//sfxHandle_t rewardMasterSound;
	//sfxHandle_t rewardChampionSound;

	//sfxHandle_t takenLeadSound;
	//sfxHandle_t tiedLeadSound;
	//sfxHandle_t lostLeadSound;

	sfxHandle_t watrInSound;
	sfxHandle_t watrOutSound;
	sfxHandle_t watrUnSound;

	//sfxHandle_t flightSound;
	sfxHandle_t medkitSound;
	sfxHandle_t	borgBeamInSound;

	// Interface sounds
	sfxHandle_t interfaceSnd1;

	// teamplay sounds
	//sfxHandle_t redLeadsSound;
	//sfxHandle_t blueLeadsSound;
	//sfxHandle_t teamsTiedSound;

	// tournament sounds
	sfxHandle_t	count3Sound;
	sfxHandle_t	count2Sound;
	sfxHandle_t	count1Sound;
	sfxHandle_t	countFightSound;
	sfxHandle_t	countPrepareSound;

	// CTF sounds
	/*sfxHandle_t	ctfStealSound;
	sfxHandle_t	ctfReturnSound;
	sfxHandle_t	ctfScoreSound;
	sfxHandle_t ctfYouStealVoiceSound;
	sfxHandle_t ctfYouDroppedVoiceSound;
	sfxHandle_t ctfYouReturnVoiceSound; 
	sfxHandle_t ctfYouScoreVoiceSound;
	sfxHandle_t ctfTheyStealVoiceSound; 
	sfxHandle_t ctfTheyDroppedVoiceSound;
	sfxHandle_t ctfTheyReturnVoiceSound;
	sfxHandle_t ctfTheyScoreVoiceSound; */

	//
	// trek sounds
	//
	sfxHandle_t	envSparkSound1;
	sfxHandle_t	envSparkSound2;
	sfxHandle_t	envSparkSound3;
	sfxHandle_t defaultPickupSound;
	sfxHandle_t grenadeAltStickSound;
	sfxHandle_t grenadeBounceSound1;
	sfxHandle_t grenadeBounceSound2;
	sfxHandle_t grenadeExplodeSound;
	//sfxHandle_t tetrionRicochetSound1;
	//sfxHandle_t tetrionRicochetSound2;
	//sfxHandle_t tetrionRicochetSound3;
	sfxHandle_t invulnoProtectSound;
	//sfxHandle_t regenSound;
	//sfxHandle_t poweruprespawnSound;
	sfxHandle_t disintegrateSound;
	sfxHandle_t disintegrate2Sound;
	sfxHandle_t playerExplodeSound;
	sfxHandle_t waterDropSound1;
	sfxHandle_t waterDropSound2;
	sfxHandle_t waterDropSound3;
	sfxHandle_t	holoInitSound;
	sfxHandle_t	holoDoorSound;
	sfxHandle_t	holoFadeSound;
	sfxHandle_t phaserEmptySound;
	sfxHandle_t	quantumBoom;
	//RPG-X: RedTechie - Better explo sounds for glauncher
	sfxHandle_t	grenadeAltExplodeSnd;
	//RPG-X: RedTechie - Shake command sound
	sfxHandle_t	ShakeSound;
	//RPG-X | Phenix | 13/02/2005
	sfxHandle_t N00bSound[N00bSoundCount];
	//RPG-X | Phenix | 08/06/2005
	sfxHandle_t	AdminMsgSound;
	//RPG-X | TiM
	sfxHandle_t splatSound;
	//RPG-X | TiM
	sfxHandle_t tedTextSound;

	// Zoom
	sfxHandle_t	zoomStart;
	sfxHandle_t	zoomStart116;
	sfxHandle_t	zoomLoop;
	sfxHandle_t	zoomEnd;
	sfxHandle_t	zoomEnd116;
	sfxHandle_t	tr116Chirp;
	sfxHandle_t	tr116Whir;

	// Chunk sounds
	sfxHandle_t metalChunkSound;
	sfxHandle_t	glassChunkSound;
	sfxHandle_t	woodChunkSound;
	sfxHandle_t	stoneChunkSound;

	//TiM : Splosion sounds
	sfxHandle_t	surfaceExpSound[3];
	sfxHandle_t	electricExpSound[3];
	sfxHandle_t	bigSurfExpSound;

	//TiM - Q Flash sound
	sfxHandle_t	qFlash;

	qhandle_t	loading1;
	qhandle_t	loading2;
	qhandle_t	loading3;
	qhandle_t	loading4;
	qhandle_t	loading5;
	qhandle_t	loading6;
	qhandle_t	loading7;
	qhandle_t	loading8;
	qhandle_t	loading9;
	qhandle_t	loadingcircle;
	qhandle_t	loadingquarter;
	qhandle_t	loadingcorner;
	qhandle_t	loadingtrim;
	qhandle_t	circle;
	qhandle_t	circle2;
	qhandle_t	corner_12_18;
	qhandle_t	halfroundr_22;
	qhandle_t	corner_ul_20_30;
	qhandle_t	corner_ll_8_30;

	//TiM - OPTIMIZATION:
	//Used 2 API functions to cut down the
	//shader usage on this thing significantly ^_^
	qhandle_t	radarShader;
	/*qhandle_t	rd_up;
	qhandle_t	rd_down;
	qhandle_t	rd_level;
	qhandle_t	rd_red_up;
	qhandle_t	rd_red_down;
	qhandle_t	rd_red_level;
	qhandle_t	rd_blue_up;
	qhandle_t	rd_blue_down;
	qhandle_t	rd_blue_level;
	qhandle_t	rd_white_up;
	qhandle_t	rd_white_down;
	qhandle_t	rd_white_level;
	qhandle_t	rd_teal_up;
	qhandle_t	rd_teal_down;
	qhandle_t	rd_teal_level;
	qhandle_t	rd_black_up;
	qhandle_t	rd_black_down;
	qhandle_t	rd_black_level;
	qhandle_t	rd_injured_up;
	qhandle_t	rd_injured_down;*/
	qhandle_t	rd_injured_level;

	qhandle_t	radarMain;

	//RPG-X (J2J) Rank Images for Score Board (Arrays for each class type)
	//TiM: *shudder* There was a better way!
	/*qhandle_t  ri_Civ;
	qhandle_t	ri_Crewman[4];
	qhandle_t	ri_Cadet1[4];
	qhandle_t	ri_Cadet2[4];
	qhandle_t	ri_Cadet3[4];
	qhandle_t	ri_Cadet4[4];
	qhandle_t	ri_Ensign[4];
	qhandle_t	ri_Ltjg[4];
	qhandle_t	ri_Lt[4];
	qhandle_t	ri_Ltcmdr[4];
	qhandle_t	ri_Cmdr[4];
	qhandle_t	ri_Capt[4];
	qhandle_t	ri_Cmmdr[4];
	qhandle_t	ri_Admr2[4];
	qhandle_t	ri_Admr3[4];
	qhandle_t	ri_Admr4[4];
	qhandle_t	ri_Admr5[4];*/

	//RPG-X: J2J - This is for sharky's crosshiar for each weapon.
	//TiM: hrmmmm..... O_o
	//qhandle_t	crosshair[15];
	qhandle_t	crosshairSheet;

	//TiM : LensFlare Parts
	qhandle_t	flareCore;
	qhandle_t	flareStreak;
	qhandle_t	flareChroma;
	qhandle_t	flareRadial;
	qhandle_t	flareStraight;
	qhandle_t	flareInverseRad;
	qhandle_t	flareHaze;

	qhandle_t	orangeStarShader;

	//Q Flash sprite
	qhandle_t	qFlashSprite;

} cgMedia_t;


// The client game static (cgs) structure hold everything
// loaded or calculated from the gamestate.  It will NOT
// be cleared when a tournement restart is done, allowing
// all clients to begin playing instantly
typedef struct {
	gameState_t		gameState;			// gamestate from server
	glconfig_t		glconfig;			// rendering configuration
	float			screenXScale;		// derived from glconfig
	float			screenYScale;
	float			screenXBias;

	int				serverCommandSequence;	// reliable command stream counter
	int				processedSnapshotNum;// the number of snapshots cgame has requested

	qboolean		localServer;		// detected on startup by checking sv_running

	// parsed from serverinfo
	gametype_t		gametype;
	qboolean		pModAssimilation;
	qboolean		pModDisintegration;
	qboolean		pModActionHero;
	qboolean		pModSpecialties;
	qboolean		pModElimination;
	int				dmflags;
	int				teamflags;
	int				fraglimit;
	int				capturelimit;
	int				timelimit;
	int				maxclients;
	char			mapname[MAX_QPATH];
	qboolean		ForceClassColor;	
	char			rankSet[MAX_QPATH];
	char			classSet[MAX_QPATH];

	int				voteTime;
	int				voteYes;
	int				voteNo;
	qboolean		voteModified;			// beep whenever changed
	char			voteString[MAX_STRING_TOKENS];

	int				levelStartTime;

	int				scores1, scores2;		// from configstrings
	int				redflag, blueflag;		// flag status from configstrings

	//
	// locally derived information from gamestate
	//
	qhandle_t		gameModels[MAX_MODELS];
//	qhandle_t		useableModels[HI_NUM_HOLDABLE];
	sfxHandle_t		gameSounds[MAX_SOUNDS];

	int				numInlineModels;
	qhandle_t		inlineDrawModel[MAX_MODELS];
	vec3_t			inlineModelMidpoints[MAX_MODELS];

	clientInfo_t	clientinfo[MAX_CLIENTS];

	// teamchat width is *3 because of embedded color codes
	char			teamChatMsgs[TEAMCHAT_HEIGHT][TEAMCHAT_WIDTH*3+1];
	int				teamChatMsgTimes[TEAMCHAT_HEIGHT];
	int				teamChatPos;
	int				teamLastChatPos;

	// media
	cgMedia_t		media;

	//TiM: ranks data
	defaultRankData_t	defaultRankData;
	ranksData_t			ranksData[MAX_RANKS];

	//TiM: crosshair data
	crosshairsData_t	crosshairsData[MAX_CROSSHAIRS];

	//decoy memory spaces
	clientInfo_t		decoyInfo[MAX_DECOYS];

	//During game load, check to see if we have any locations
	//so we may edit the scoreboard to account for them
	qboolean			locations;

	//TiM - static class data
	cg_classData_t		classData[MAX_CLASSES];

	//objectives
	objective_t			objectives[MAX_OBJECTIVES];

	//widescreen variables
	widescreen_t		widescreen;

	//scannable tricorder dynamic text
	char				scannableStrings[MAX_SCANNABLES][36];

	qboolean			scannablePanels;
} cgs_t;

//==============================================================================

extern	cgs_t			cgs;
extern	cg_t			cg;
extern	centity_t		cg_entities[MAX_GENTITIES];
extern	weaponInfo_t	cg_weapons[MAX_WEAPONS];
extern	itemInfo_t		cg_items[MAX_ITEMS];
extern	markPoly_t		cg_markPolys[MAX_MARK_POLYS];

extern	vmCvar_t		cg_centertime;
extern	vmCvar_t		cg_runpitch;
extern	vmCvar_t		cg_runroll;
extern	vmCvar_t		cg_bobup;
extern	vmCvar_t		cg_bobpitch;
extern	vmCvar_t		cg_bobroll;
extern	vmCvar_t		cg_swingSpeed;
extern	vmCvar_t		cg_shadows;
extern	vmCvar_t		cg_gibs;
extern	vmCvar_t		cg_drawTimer;
extern	vmCvar_t		cg_drawFPS;
extern	vmCvar_t		cg_drawSnapshot;
extern	vmCvar_t		cg_draw3dIcons;
extern	vmCvar_t		cg_drawIcons;
extern	vmCvar_t		cg_drawAmmoWarning;
extern	vmCvar_t		cg_drawCrosshair;
extern	vmCvar_t		cg_drawCrosshairNames;
extern	vmCvar_t		cg_dynamicCrosshairNames;
extern	vmCvar_t		cg_drawRewards;
extern	vmCvar_t		cg_drawTeamOverlay;
extern	vmCvar_t		cg_teamOverlayUserinfo;
extern	vmCvar_t		cg_crosshairX;
extern	vmCvar_t		cg_crosshairY;
extern	vmCvar_t		cg_crosshairSize;
extern	vmCvar_t		cg_crosshairHealth;
extern	vmCvar_t		cg_drawStatus;
extern	vmCvar_t		cg_draw2D;
extern	vmCvar_t		cg_animSpeed;
extern	vmCvar_t		cg_debugAnim;
extern	vmCvar_t		cg_debugPosition;
extern	vmCvar_t		cg_debugEvents;
extern	vmCvar_t		cg_errorDecay;
extern	vmCvar_t		cg_nopredict;
extern	vmCvar_t		cg_noPlayerAnims;
extern	vmCvar_t		cg_showmiss;
extern	vmCvar_t		cg_footsteps;
extern	vmCvar_t		cg_addMarks;
extern	vmCvar_t		cg_gun_frame;
extern	vmCvar_t		cg_gun_x;
extern	vmCvar_t		cg_gun_y;
extern	vmCvar_t		cg_gun_z;
extern	vmCvar_t		cg_drawGun;
extern	vmCvar_t		cg_viewsize;
extern	vmCvar_t		cg_autoswitch;
extern	vmCvar_t		cg_ignore;
extern	vmCvar_t		cg_simpleItems;
extern	vmCvar_t		cg_fov;
extern	vmCvar_t		cg_zoomFov;
extern	vmCvar_t		cg_thirdPersonRange;
extern	vmCvar_t		cg_thirdPersonAngle;
extern	vmCvar_t		cg_thirdPersonVertOffset; //RPG-X: TiM
extern	vmCvar_t		cg_thirdPersonHorzOffset;
extern  vmCvar_t		cg_thirdPersonAlpha;
extern	vmCvar_t		cg_thirdPersonCameraDamp;
extern	vmCvar_t		cg_thirdPersonTargetDamp;
extern	vmCvar_t		cg_thirdPersonPitchOffset;
extern	vmCvar_t		cg_thirdPerson;
extern	vmCvar_t		cg_stereoSeparation;
extern	vmCvar_t		cg_lagometer;
extern	vmCvar_t		cg_drawAttacker;
extern	vmCvar_t		cg_synchronousClients;
extern	vmCvar_t		cg_teamChatTime;
extern	vmCvar_t		cg_teamChatHeight;
extern	vmCvar_t		cg_stats;
extern	vmCvar_t		cg_reportDamage;
extern	vmCvar_t 		cg_forceModel;
extern	vmCvar_t 		cg_buildScript;
extern	vmCvar_t		cg_paused;
extern	vmCvar_t		cg_blood;
extern	vmCvar_t		cg_predictItems;
extern	vmCvar_t		cg_deferPlayers;
extern	vmCvar_t		cg_disablekillmsgs;
extern	vmCvar_t		cg_drawradar;
extern  vmCvar_t		cg_noclipSpectate;					//RPG-X J2J: Option for client to go no-clip in spectator mode.
extern	vmCvar_t		rpg_ctribgrenade;					//RPG-X: RedTechie - Nevermind :P
extern	vmCvar_t		cg_dynamicCrosshair;				//RPG-X | Phenix | 09/06/2005
extern	vmCvar_t		doomHead;				//RPG-X | Phenix | 09/06/2005
extern	vmCvar_t		cg_dynamiclensflares;  //RPG-X | TiM | 29/6/2005
extern	vmCvar_t		cg_noTalkingHeads;
extern	vmCvar_t		cg_noFrowningHeads;
extern	vmCvar_t		cg_noBlinkingHeads;
extern	vmCvar_t		cg_noDynamicRanks;
extern	vmCvar_t		noAdminChat;
extern	vmCvar_t		cg_firstPersonBody;
extern	vmCvar_t		cg_defaultModel;

extern vmCvar_t			cg_defaultChar;

//RPG-X: TiM - Player Model System Parameters
extern	vmCvar_t		pms_age;
extern	vmCvar_t		pms_height;
extern	vmCvar_t		pms_weight;
extern	vmCvar_t		pms_race;

extern vmCvar_t			emote_Offset;

extern	vmCvar_t		cg_thirdPersonZoomRate;
extern	vmCvar_t		cg_showEntityNums;

//TiM - SecurityCode
extern vmCvar_t			sv_securityHash;
extern vmCvar_t			sv_securityCode;

extern vmCvar_t			cg_handleWidescreen;
extern vmCvar_t			ui_handleWidescreen;

//extern vmCvar_t			cg_chatBGColor;

extern vmCvar_t			cg_chatColor;

//RPG-X | GSIO01 | 11/05/2009
extern vmCvar_t			rpg_forceFieldSet;

// grp cvars
extern vmCvar_t			grp_berp;

// lua
#ifdef CG_LUA
extern vmCvar_t			cg_debugLua;
extern vmCvar_t			cg_logLua;
#endif





qboolean CG_Cvar_ClampInt( const char *name, vmCvar_t *vmCvar, int min, int max );

//
// cg_main.c
//
const char *CG_ConfigString( int index );
const char *CG_Argv( int arg );

void QDECL CG_Printf( const char *msg, ... );
void QDECL CG_Error( const char *msg, ... );
void CG_StartMusic( void );

void CG_UpdateCvars( void );

int CG_CrosshairPlayer( void );
int CG_LastAttacker( void );


//
// cg_view.c
//
void CG_TestModel_f (void);
void CG_TestGun_f (void);
void CG_TestModelNextFrame_f (void);
void CG_TestModelPrevFrame_f (void);
void CG_TestModelNextSkin_f (void);
void CG_TestModelPrevSkin_f (void);
void CG_ZoomDown_f( void );
void CG_ZoomUp_f( void );
void CG_CameraShake( float intensity, int duration, qboolean addRumbleSound );

void CG_DrawActiveFrame( int serverTime, stereoFrame_t stereoView, qboolean demoPlayback );


//
// cg_drawtools.c
//
void CG_PrintInterfaceGraphics(int min,int max);
void CG_AdjustFrom640( float *x, float *y, float *w, float *h );
void CG_FillRect( float x, float y, float width, float height, const float *color );
void CG_DrawPic( float x, float y, float width, float height, qhandle_t hShader );
void CG_DrawStretchPic( float x, float y, float width, float height, float s1, 
								float t1, float s2, float t2, qhandle_t hShader );
void CG_DrawString( float x, float y, const char *string, 
				   float charWidth, float charHeight, const float *modulate );
void UI_DrawProportionalString( int x, int y, const char* str, int style, vec4_t color );
void CG_LoadFonts(void);


void CG_DrawStringExt( int x, int y, const char *string, const float *setColor, 
		qboolean forceColor, qboolean shadow, int charWidth, int charHeight, int maxChars );
void CG_DrawBigString( int x, int y, const char *s, float alpha );
void CG_DrawBigStringColor( int x, int y, const char *s, vec4_t color );
void CG_DrawSmallString( int x, int y, const char *s, float alpha );
void CG_DrawSmallStringColor( int x, int y, const char *s, vec4_t color );

int CG_DrawStrlen( const char *str );

float	*CG_FadeColor( int startMsec, int totalMsec );
float *CG_TeamColor( int team );
void CG_TileClear( void );
void CG_ColorForHealth( vec4_t hcolor );
void CG_GetColorForHealth( int health, int armor, vec4_t hcolor );

int UI_ProportionalStringWidth( const char* str,int style );

qboolean CG_LoadRanks( void );
qboolean CG_LoadCrosshairs( void );
qboolean CG_LoadClasses( void );
//
// cg_draw.c
//
typedef struct
{
	int				type;		// STRING or GRAPHIC
	float			timer;		// When it changes
	int				x;			// X position
	int				y;			// Y positon
	int				width;		// Graphic width
	int				height;		// Graphic height
	char			*file;		// File name of graphic/ text if STRING
	qhandle_t		graphic;	// Handle of graphic if GRAPHIC
	int				min;
	int				max;
	int				color;		// Normal color
	int				style;		// Style of characters
//	void			*pointer;		// To an address
} interfacegraphics_s;

typedef enum 
{
	IG_GROW,

	IG_HEALTH_START,
	IG_HEALTH_BEGINCAP,
	IG_HEALTH_BOX1,
	IG_HEALTH_SLIDERFULL,
	IG_HEALTH_SLIDEREMPTY,
	IG_HEALTH_ENDCAP,
	IG_HEALTH_COUNT,
	IG_HEALTH_END,

	IG_ARMOR_START,
	IG_ARMOR_BEGINCAP,
	IG_ARMOR_BOX1,
	IG_ARMOR_SLIDERFULL,
	IG_ARMOR_SLIDEREMPTY,
	IG_ARMOR_ENDCAP,
	IG_ARMOR_COUNT,
	IG_ARMOR_END,

	IG_AMMO_START,
	IG_AMMO_UPPER_BEGINCAP,
	IG_AMMO_UPPER_ENDCAP,
	IG_AMMO_LOWER_BEGINCAP,
	IG_AMMO_SLIDERFULL,
	IG_AMMO_SLIDEREMPTY,
	IG_AMMO_LOWER_ENDCAP,
	IG_AMMO_COUNT,
	IG_AMMO_END,

	IG_MAX
} interface_graphics_t;


extern interfacegraphics_s interface_graphics[IG_MAX];

#define SG_OFF		0
#define SG_STRING	1
#define SG_GRAPHIC	2
#define SG_NUMBER	3
#define SG_VAR		4


extern	int sortedTeamPlayers[TEAM_MAXOVERLAY];
extern	int	numSortedTeamPlayers;
extern	int drawTeamOverlayModificationCount;

void CG_AddLagometerFrameInfo( void );
void CG_AddLagometerSnapshotInfo( snapshot_t *snap );
void CG_CenterPrint( const char *str, int y, int charWidth );
void CG_DrawHead( float x, float y, float w, float h, int clientNum, vec3_t headAngles );
void CG_DrawActive( stereoFrame_t stereoView );
void CG_DrawFlagModel( float x, float y, float w, float h, int team );
void CG_DrawTeamBackground( int x, int y, int w, int h, float alpha, int team, qboolean scoreboard );
void CG_DrawNumField (int x, int y, int width, int value,int charWidth,int charHeight,int style);
void CG_DrawObjectiveInformation( void );


//
// cg_player.c
//
void CG_PlayerShieldHit(int entitynum, vec3_t angles, int amount);
void CG_Player( centity_t *cent );
void CG_ResetPlayerEntity( centity_t *cent );
//void CG_AddRefEntityWithPowerups( refEntity_t *ent, int powerups, int eFlags, qboolean borg );
//void CG_AddRefEntityWithPowerups( refEntity_t *ent, int powerups, int eFlags, beamData_t *beamData, qboolean borg );
void CG_AddRefEntityWithPowerups( refEntity_t *ent, int powerups, int eFlags, beamData_t *beamData, int cloakTime, int decloakTime, qboolean borg );
void CG_NewClientInfo( int clientNum );
sfxHandle_t	CG_CustomSound( int clientNum, const char *soundName );
void CG_NewDecoyInfo( int decoyNum );

//
// cg_predict.c
//
void CG_BuildSolidList( void );
int	CG_PointContents( const vec3_t point, int passEntityNum );
void CG_Trace( trace_t *result, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, 
					 int skipNumber, int mask );
void CG_PredictPlayerState( void );
void CG_LoadDeferredPlayers( void );


//
// cg_events.c
//
void CG_CheckEvents( centity_t *cent );
const char	*CG_PlaceString( int rank );
void CG_EntityEvent( centity_t *cent, vec3_t position );
void CG_PainEvent( centity_t *cent, int health );


//
// cg_ents.c
//
void CG_SetEntitySoundPosition( centity_t *cent );
void CG_AddPacketEntities( void );
void CG_Beam( centity_t *cent );
void CG_AdjustPositionForMover( const vec3_t in, int moverNum, int fromTime, int toTime, vec3_t out );

void CG_PositionEntityOnTag( refEntity_t *entity, const refEntity_t *parent, 
							qhandle_t parentModel, char *tagName );
void CG_PositionRotatedEntityOnTag( refEntity_t *entity, const refEntity_t *parent, 
							qhandle_t parentModel, char *tagName );


//
//	cg_env.c
//
void CG_Spark( vec3_t origin, vec3_t dir, int delay, int killTime );
void CG_Steam( vec3_t position, vec3_t dir, int killTime );
void CG_Bolt( centity_t *cent );
void CG_TransporterPad(vec3_t origin);
void CG_Drip(centity_t *cent, int killTime );
void CG_Chunks( vec3_t origin, vec3_t dir, float size, material_type_t type );
void CG_FireLaser( vec3_t start, vec3_t end, vec3_t normal, vec3_t laserRGB, float alpha );
void CG_AimLaser( vec3_t start, vec3_t end, vec3_t normal );
void CG_StasisDoor(centity_t *cent, qboolean close);

//TiM
void CG_FountainSpurt( vec3_t org, vec3_t end );
void CG_ElectricalExplosion( vec3_t start, vec3_t dir, float radius );

//RPG-X | GSIO01 | 09/05/2009
void CG_PhaserFX(centity_t *cent);
void CG_DisruptorFX(centity_t *cent); //RPG-X | Harry Young | 03.12.2011
void CG_TorpedoFX(centity_t *cent);
void CG_ParticleFire(vec3_t origin, int killtime, int size);
void CG_ShowTrigger(centity_t *cent);

// Additional ports from SP by Harry Young
void CG_CookingSteam( vec3_t origin, float radius );
void CG_ElectricFire( vec3_t origin, vec3_t normal );
void ForgeBoltFireback( vec3_t start, vec3_t end, vec3_t velocity, vec3_t user );
void CG_ForgeBolt( centity_t *cent );
void CG_Plasma( vec3_t start, vec3_t end, vec3_t sRGB, vec3_t eRGB, int startalpha, int endalpha );
void CG_ParticleStream( centity_t *cent );
void CG_TransporterStream( centity_t *cent );
void CG_ExplosionTrail( centity_t *cent );
void CG_BorgEnergyBeam( centity_t *cent );
void CG_ShimmeryThing( vec3_t start, vec3_t end, vec3_t content );
void CG_Shimmer( vec3_t position, vec3_t dest, vec3_t dir, vec3_t other );
void CG_ShimmeryThing_Spawner( vec3_t start, vec3_t end, float radius, qboolean taper, int duration );
void CG_Borg_Bolt_static( centity_t *cent );
void CG_Borg_Bolt_dynamic( centity_t *cent );

//
// cg_weapons.c
//
void CG_NextWeapon_f( void );
void CG_PrevWeapon_f( void );
void CG_Weapon_f( void );

void CG_RegisterWeapon( int weaponNum );
void CG_RegisterItemVisuals( int itemNum );

void CG_FireWeapon( centity_t *cent, qboolean alt_fire );
void CG_FireSeeker( centity_t *cent );
void CG_MissileHitWall( centity_t *cent, int weapon, vec3_t origin, vec3_t dir );
void CG_MissileHitPlayer( centity_t *cent, int weapon, vec3_t origin, vec3_t dir);

void CG_AddViewWeapon (playerState_t *ps);
void CG_AddPlayerWeapon( refEntity_t *parent, playerState_t *ps, centity_t *cent );
void CG_DrawWeaponSelect( void );
void CG_DrawWeaponIcon ( int x, int y, int weapon ); //RPG-X | Phenix | 08/06/2005

void CG_OutOfAmmoChange( qboolean altfire );	// should this be in pmove?
void CG_BounceEffect( centity_t *cent, int weapon, vec3_t origin, vec3_t normal );
//void CG_BorgEyebeam( centity_t *cent, const refEntity_t *parent );

void CG_PlayShooterSound(centity_t *cent);

extern int tris_state;

//
// cg_marks.c
//
void	CG_InitMarkPolys( void );
void	CG_AddMarks( void );
void	CG_ImpactMark( qhandle_t markShader, 
				    const vec3_t origin, const vec3_t dir, 
					float orientation, 
				    float r, float g, float b, float a, 
					qboolean alphaFade, 
					float radius, qboolean temporary );

//
// cg_localents.c
//
void	CG_InitLocalEntities( void );
localEntity_t	*CG_AllocLocalEntity( void );
localEntity_t	CG_GetActiveList( void );
void	CG_AddLocalEntities( void );

//
// cg_effects.c
//
localEntity_t *CG_SmokePuff( const vec3_t p, 
				   const vec3_t vel, 
				   float radius,
				   float r, float g, float b, float a,
				   float duration,
				   int startTime,
				   int leFlags,
				   qhandle_t hShader );
void CG_BubbleTrail( vec3_t start, vec3_t end, float spacing );
void CG_SpawnEffect( vec3_t org, refEntity_t *ent_legs, refEntity_t *ent_torso, refEntity_t *ent_head);
void CG_QFlashEvent( vec3_t org );


void CG_Bleed( vec3_t origin, int entityNum );
void CG_Seeker( centity_t *cent );

//RPG-X: TiM : Smoke Ent
//void CG_Smoke( vec3_t origin, vec3_t dir, float radius, float speed, qhandle_t shader );
void CG_Smoke( vec3_t position, vec3_t dir, int killTime, int radius );
//RPG-X: Marcin: Fire
void CG_Fire( vec3_t position, vec3_t dir, int killTime, int radius, int fxEnt );

localEntity_t *CG_MakeExplosion( vec3_t origin, vec3_t dir,
								qhandle_t hModel, qhandle_t shader, int msec, float scale,
								qboolean isSprite );
localEntity_t *CG_MakeExplosion2( vec3_t origin, vec3_t dir, 
								qhandle_t hModel, int numFrames, qhandle_t shader,
								int msec, qboolean isSprite, float scale, int flags );

void CG_SurfaceExplosion( vec3_t origin, vec3_t normal, float radius, float shake_speed, qboolean smoke );
void CG_ExplosionEffects( vec3_t origin, int intensity, int radius);

//
// cg_snapshot.c
//
void CG_ProcessSnapshots( void );

//
// cg_info.c
//
void CG_LoadingString( const char *s );
void CG_LoadingItem( int itemNum );
void CG_LoadingClient( int clientNum );
void CG_DrawInformation( void );

//
// cg_scoreboard.c
//

qboolean CG_DrawScoreboard( void );
void CG_DrawTourneyScoreboard( void );

//
// these lists need to be sync'ed with stuff under g_log in g_local.h
//

// awards
typedef enum {
	AWARD_EFFICIENCY,		// Accuracy
	AWARD_SHARPSHOOTER,		// Most compression rifle frags
	AWARD_UNTOUCHABLE,		// Perfect (no deaths)
	AWARD_LOGISTICS,		// Most pickups
	AWARD_TACTICIAN,		// Kills with all weapons
	AWARD_DEMOLITIONIST,	// Most explosive damage kills
	AWARD_STREAK,			// Ace/Expert/Master/Champion
	AWARD_TEAM,				// MVP/Defender/Warrior/Carrier/Interceptor/Bravery
	AWARD_SECTION31,		// All-around god
	AWARD_MAX
} awardType_t;

typedef enum
{
	TEAM_NONE = 0,			// ha ha! you suck!
	TEAM_MVP,				// most overall points
	TEAM_DEFENDER,			// killed the most baddies near your flag
	TEAM_WARRIOR,			// most frags
	TEAM_CARRIER,			// infected the most people with plague
	TEAM_INTERCEPTOR,		// returned your own flag the most
	TEAM_BRAVERY,			// Red Shirt Award (tm). you died more than anybody. 
	TEAM_MAX
} teamAward_e;

//
// above lists need to be sync'ed with stuff under g_log in g_local.h
//

typedef enum
{
	AWARD_STREAK_NONE = 0,
	AWARD_STREAK_ACE,
	AWARD_STREAK_EXPERT,
	AWARD_STREAK_MASTER,
	AWARD_STREAK_CHAMPION,
	AWARD_STREAK_MAX
} awardStreak_t;




extern char	*cg_medalPicNames[];
extern char	*cg_medalSounds[];

extern char	*cg_medalTeamPicNames[];
extern char	*cg_medalTeamSounds[];

extern char	*cg_medalStreakPicNames[];
extern char	*cg_medalStreakSounds[];

void AW_SPPostgameMenu_f( void );


//
// cg_consolecmds.c
//
qboolean CG_ConsoleCommand( void );
void CG_InitConsoleCommands( void );

//
// cg_servercmds.c
//
void CG_ExecuteNewServerCommands( int latestSequence );
void CG_ParseServerinfo( void );
void CG_SetConfigValues( void );

//
// cg_playerstate.c
//
void CG_Respawn( void );
void CG_TransitionPlayerState( playerState_t *ps, playerState_t *ops );


// cg_players.c
void updateSkin(int clientNum, char *new_model);

/*
RPG-X J2J: This struct is used to convientently store the data loaded from TiM's
           data.cfg file. References what body to use, skins etc..

		   TiM: Haha thanks Jay
*/

//typedef struct
//{
//	char ID[80];
//	char Name[255];
//	char BodyModel[255];
//	char Gender;
//	qboolean Alien;
//	qboolean Humanoid;
//	char DefaultRank[255];
//	char SoundSet[255];
////Allowed Classes
//	qboolean Engineer;
//	qboolean Science;
//	qboolean Command;
//	qboolean Security;
//	qboolean Medical;
//
//}
//X_ModelCfgData;

//===============================================

//
// system traps
// These functions are how the cgame communicates with the main game system
//

// print message on the local console
void		trap_Print( const char *fmt );

// abort the game
void		trap_Error( const char *fmt );

// milliseconds should only be used for performance tuning, never
// for anything game related.  Get time from the CG_DrawActiveFrame parameter
int			trap_Milliseconds( void );

// console variable interaction
void		trap_Cvar_Register( vmCvar_t *vmCvar, const char *varName, const char *defaultValue, int flags );
void		trap_Cvar_Update( vmCvar_t *vmCvar );
void		trap_Cvar_Set( const char *var_name, const char *value );
void		trap_Cvar_Set_No_Modify( const char *var_name, const char *value );
void		trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );

// ServerCommand and ConsoleCommand parameter access
int			trap_Argc( void );
void		trap_Argv( int n, char *buffer, int bufferLength );
void		trap_Args( char *buffer, int bufferLength );

// filesystem access
// returns length of file
int			trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode );
void		trap_FS_Read( void *buffer, int len, fileHandle_t f );
void		trap_FS_Write( const void *buffer, int len, fileHandle_t f );
void		trap_FS_FCloseFile( fileHandle_t f );

// add commands to the local console as if they were typed in
// for map changing, etc.  The command is not executed immediately,
// but will be executed in order the next time console commands
// are processed
void		trap_SendConsoleCommand( const char *text );

// register a command name so the console can perform command completion.
// FIXME: replace this with a normal console command "defineCommand"?
void		trap_AddCommand( const char *cmdName );

// send a string to the server over the network
void		trap_SendClientCommand( const char *s );

// force a screen update, only used during gamestate load
void		trap_UpdateScreen( void );

// model collision
void		trap_CM_LoadMap( const char *mapname );
int			trap_CM_NumInlineModels( void );
clipHandle_t trap_CM_InlineModel( int index );		// 0 = world, 1+ = bmodels
clipHandle_t trap_CM_TempBoxModel( const vec3_t mins, const vec3_t maxs );
int			trap_CM_PointContents( const vec3_t p, clipHandle_t model );
int			trap_CM_TransformedPointContents( const vec3_t p, clipHandle_t model, const vec3_t origin, const vec3_t angles );
void		trap_CM_BoxTrace( trace_t *results, const vec3_t start, const vec3_t end,
					  const vec3_t mins, const vec3_t maxs,
					  clipHandle_t model, int brushmask );
void		trap_CM_TransformedBoxTrace( trace_t *results, const vec3_t start, const vec3_t end,
					  const vec3_t mins, const vec3_t maxs,
					  clipHandle_t model, int brushmask,
					  const vec3_t origin, const vec3_t angles );

// Returns the projection of a polygon onto the solid brushes in the world
int			trap_CM_MarkFragments( int numPoints, const vec3_t *points, 
			const vec3_t projection,
			int maxPoints, vec3_t pointBuffer,
			int maxFragments, markFragment_t *fragmentBuffer );

// normal sounds will have their volume dynamically changed as their entity
// moves and the listener moves
void		trap_S_StartSound( vec3_t origin, int entityNum, int entchannel, sfxHandle_t sfx );

// a local sound is always played full volume
void		trap_S_StartLocalSound( sfxHandle_t sfx, int channelNum );
void		trap_S_ClearLoopingSounds( void );
void		trap_S_AddLoopingSound( int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx );
void		trap_S_UpdateEntityPosition( int entityNum, const vec3_t origin );

// repatialize recalculates the volumes of sound as they should be heard by the
// given entityNum and position
void		trap_S_Respatialize( int entityNum, const vec3_t origin, vec3_t axis[3], int inwater );
sfxHandle_t	trap_S_RegisterSound( const char *sample );		// returns buzz if not found
void		trap_S_StartBackgroundTrack( const char *intro, const char *loop );	// empty name stops music


void		trap_R_LoadWorldMap( const char *mapname );

// all media should be registered during level startup to prevent
// hitches during gameplay
qhandle_t	trap_R_RegisterModel( const char *name );			// returns rgb axis if not found
qhandle_t	trap_R_RegisterSkin( const char *name );			// returns all white if not found
qhandle_t	trap_R_RegisterShader( const char *name );			// returns all white if not found
qhandle_t	trap_R_RegisterShader3D( const char *name );			// returns all white if not found
qhandle_t	trap_R_RegisterShaderNoMip( const char *name );			// returns all white if not found

// a scene is built up by calls to R_ClearScene and the various R_Add functions.
// Nothing is drawn until R_RenderScene is called.
void		trap_R_ClearScene( void );
void		trap_R_AddRefEntityToScene( const refEntity_t *re );

// polys are intended for simple wall marks, not really for doing
// significant construction
void		trap_R_AddPolyToScene( qhandle_t hShader , int numVerts, const polyVert_t *verts );
void		trap_R_AddLightToScene( const vec3_t org, float intensity, float r, float g, float b );
void		trap_R_RenderScene( const refdef_t *fd );
void		trap_R_SetColor( const float *rgba );	// NULL = 1,1,1,1
void		trap_R_DrawStretchPic( float x, float y, float w, float h, 
			float s1, float t1, float s2, float t2, qhandle_t hShader );
void		trap_R_ModelBounds( clipHandle_t model, vec3_t mins, vec3_t maxs );
void		trap_R_LerpTag( orientation_t *tag, clipHandle_t mod, int startFrame, int endFrame, 
					   float frac, const char *tagName );

// The glconfig_t will not change during the life of a cgame.
// If it needs to change, the entire cgame will be restarted, because
// all the qhandle_t are then invalid.
void		trap_GetGlconfig( glconfig_t *glconfig );

// the gamestate should be grabbed at startup, and whenever a
// configstring changes
void		trap_GetGameState( gameState_t *gamestate );

// cgame will poll each frame to see if a newer snapshot has arrived
// that it is interested in.  The time is returned seperately so that
// snapshot latency can be calculated.
void		trap_GetCurrentSnapshotNumber( int *snapshotNumber, int *serverTime );

// a snapshot get can fail if the snapshot (or the entties it holds) is so
// old that it has fallen out of the client system queue
qboolean	trap_GetSnapshot( int snapshotNumber, snapshot_t *snapshot );

// retrieve a text command from the server stream
// the current snapshot will hold the number of the most recent command
// qfalse can be returned if the client system handled the command
// argc() / argv() can be used to examine the parameters of the command
qboolean	trap_GetServerCommand( int serverCommandNumber );

// returns the most recent command number that can be passed to GetUserCmd
// this will always be at least one higher than the number in the current
// snapshot, and it may be quite a few higher if it is a fast computer on
// a lagged connection
int			trap_GetCurrentCmdNumber( void );	

qboolean	trap_GetUserCmd( int cmdNumber, usercmd_t *ucmd );

// used for the weapon select and zoom
void		trap_SetUserCmdValue( int stateValue, float sensitivityScale );

// aids for VM testing
void		testPrintInt( char *string, int i );
void		testPrintFloat( char *string, float f );

int			trap_MemoryRemaining( void );

#define MAX_LENS_FLARES 64

//RPG-X | TiM | 28/06/2005
typedef struct {
	int			width;
	int			height;
	float		offset;
	qboolean	positive;
	vec3_t		color;
	char*		file;
	qhandle_t	graphic;
} lensReflec_s;

typedef struct {
	vec3_t worldCoord;
	int w1;
	int h1;
	vec3_t glowColor;
	float glowOffset;
	float hazeOffset;
	int minDist;
	int maxDist;
	vec3_t streakColor;
	int	streakDistMin;
	int streakDistMax;
	int streakW;
	int streakH;
	qboolean whiteStreaks;
	int reflecDistMin;
	int reflecDistMax;
	qboolean reflecAnamorphic;
	qboolean defReflecs;
	qboolean clamp;
	float maxAlpha;
	int startTime;
	int upTime;
	int holdTime;
	int downTime;
	qboolean qfull;
} lensFlare_t;

extern lensReflec_s lensReflec[10];
//extern lensFlare_t lensFlare[MAX_LENS_FLARES];

void CG_DrawLensFlare( lensFlare_t *flare );
void CG_InitLensFlare( vec3_t worldCoord, 
						int w1, int h1,
						vec3_t glowColor, float glowOffset, float hazeOffset, int minDist, int maxDist,
						vec3_t streakColor, int streakDistMin, int streakDistMax, int streakW, int streakH,  qboolean whiteStreaks, 
						int reflecDistMin, int reflecDistMax, qboolean reflecAnamorphic, qboolean defReflecs, 
						qboolean clamp, float maxAlpha, int startTime, int upTime, int holdTime, int downTime );

//void CG_ParseClassData( void );

//TiM - Allow parts of the lerp code to update the 3rd view if need be
void CG_ResetThirdPersonViewDamp ( void );

/* GSIO - shader remapping */
void trap_R_RemapShader( const char *oldShader, const char *newShader, const char *timeOffset );
void CG_ShaderStateChanged(void);

