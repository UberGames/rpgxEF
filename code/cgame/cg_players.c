//
// cg_players.c -- handle the media and animation for player entities

#define cg_players_c				//RPG-X: J2J - Added to help solve LNK2005 errors (special case for cg_players.c)

#include "cg_local.h"
#include "cg_screenfx.h"
#include "fx_local.h"
//#include "cg_anims.h"				//RPG-X: J2J - Added for animation string table.

const char	*cg_customSoundNames[MAX_CUSTOM_SOUNDS] = {
	"*death1.wav",
	"*death2.wav",
	"*death3.wav",
	"*jump1.wav",
	"*pain25.wav",
	"*pain50.wav",
	"*pain75.wav",
	"*pain100.wav",
	"*falling1.wav",
	"*gasp.wav",
	"*drown.wav",
	"*fall1.wav",
	"*taunt1.wav",
	"*taunt2.wav",
	"*taunt3.wav",
	"*taunt4.wav",
	"*taunt5.wav"
};

stringID_table_t BoltonTable[BOLTON_MAX + 1] =
{
	{ ENUM2STRING(BOLTON_HEAD) },
	{ ENUM2STRING(BOLTON_TORSO) },
	{ ENUM2STRING(BOLTON_LEGS) },
	{ NULL, -1 }
};

int timeParam;
//int beamTimeParam; //RPG-X : TiM - Beaming

int entNum;
extern char* BG_RegisterRace( const char *name );

/*
================
CG_CustomSound

================
*/
sfxHandle_t	CG_CustomSound( int clientNum, const char *soundName ) {
	clientInfo_t *ci;
	int			i;

	if ( soundName[0] != '*' ) {
		return trap_S_RegisterSound( soundName );
	}

	if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
		clientNum = 0;
	}
	ci = &cgs.clientinfo[ clientNum ];

	for ( i = 0 ; i < MAX_CUSTOM_SOUNDS && cg_customSoundNames[i] ; i++ ) {
		if ( !strcmp( soundName, cg_customSoundNames[i] ) ) {
			return ci->sounds[i];
		}
	}

	CG_Error( "Unknown custom sound: %s", soundName );
	return 0;
}

/*
=============================================================================

ANIM SOUND CONFIG LOADING AND PLAYING

=============================================================================
*/

/*
void CG_PlayerAnimSounds( animsounds_t *animSounds, int frame, const vec3_t org, int entNum )

play any keyframed sounds - only when start a new frame
This func is called once for legs and once for torso
*/
//void CG_PlayerAnimSounds( animsounds_t *animSounds, int frame, int entNum, qboolean metal )
void CG_PlayerAnimSounds( animsounds_t *animSounds, int frame, int eNum, int surfType)
{
	int		i;
	int		holdSnd = -1;
	qboolean	playSound = qfalse;

	/*if ( entNum == cg.predictedPlayerState.clientNum && !cg.renderingThirdPerson )
	{//player in first person view does not play any keyframed sounds
		return;
	}*/

	// Check for anim sound
	for (i=0;i<MAX_ANIM_SOUNDS;++i)
	{
		if (animSounds[i].soundIndex[0] == -1)	// No sounds in array
		{
			break;
		}

		if (animSounds[i].keyFrame == frame)
		{
			// are there variations on the sound?
			/*if ( !metal ) {
				holdSnd = animSounds[i].soundIndex[ irandom( 0, animSounds[i].numRandomAnimSounds ) ];
			}
			else {
				holdSnd = cgs.media.footsteps[ FOOTSTEP_METAL ][irandom(0, 3)];
			}*/
			switch(surfType) {
				case 1: //metal
					holdSnd = cgs.media.footsteps[ FOOTSTEP_METAL ][irandom(0, 3)];
					break;
				case 2: //grass
					holdSnd = cgs.media.footsteps[ FOOTSTEP_GRASS ][irandom(0, 3)];
					break;
				case 3: //gravel
					holdSnd = cgs.media.footsteps[ FOOTSTEP_GRAVEL ][irandom(0, 3)];
					break;
				case 4: // snow
					holdSnd = cgs.media.footsteps[ FOOTSTEP_SNOW ][irandom(0, 3)];
					break;
				case 5: // wood
					holdSnd = cgs.media.footsteps[ FOOTSTEP_WOOD ][irandom(0, 3)];
					break;
				default:
					holdSnd = animSounds[i].soundIndex[irandom(0, animSounds[i].numRandomAnimSounds)];
					break;
			}

			// Determine probability of playing sound
			if (!animSounds[i].probability)	// 100% 
			{
				playSound = qtrue;
			}
			else if (animSounds[i].probability > irandom(0, 99) )
			{
				playSound = qtrue;
			}
			break;
		}
	}

	// Play sound
	if (holdSnd != -1 && playSound)
	{
		if (holdSnd != 0)	// 0 = default sound, ie file was missing
		{
			trap_S_StartSound( NULL, eNum, CHAN_BODY, holdSnd ); //CHAN_AUTO
		}

	}
}

void ParseAnimationSndBlock(const char *filename, animsounds_t *animSounds, animation_t *animations, int *i,char **text_p) 
{
	char		*token;
	char		soundString[MAX_QPATH];
	int			lowestVal, highestVal;
	int			animNum, num, n;

	// get past starting bracket
	while(1) 
	{
		token = COM_Parse( text_p );
		if ( !Q_stricmp( token, "{" ) ) 
		{
			break;
		}
	}

	animSounds +=  *i;

	// read information for each frame
	while ( 1 ) 
	{
		// Get base frame of sequence
		token = COM_Parse( text_p );
		if ( !token || !token[0]) 
		{
			break;
		}

		if ( !Q_stricmp( token, "}" ) )		// At end of block 
		{
			break;
		}

		//Compare to same table as animations used 
		//	so we don't have to use actual numbers for animation first frames,
		//	just need offsets.
		//This way when animation numbers change, this table won't have to be updated,
		//	at least not much.
		animNum = GetIDForString(animTable, token);
		if(animNum == -1)
		{//Unrecognized ANIM ENUM name, or we're skipping this line, keep going till you get a good one
			Com_Printf(S_COLOR_YELLOW"WARNING: Unknown token %s in animSound file %s\n", token, filename );
			continue;
		}

		if ( animations[animNum].numFrames == 0 )
		{//we don't use this anim
			Com_Printf(S_COLOR_YELLOW"WARNING: %s animsounds.cfg: anim %s not used by this model\n", filename, token);
	
			// Get offset to frame within sequence
			token = COM_Parse( text_p );
			//get soundstring
			token = COM_Parse( text_p );
			//get lowest value
			token = COM_Parse( text_p );
			//get highest value
			token = COM_Parse( text_p );
			//get probability
			token = COM_Parse( text_p );

			continue;
		}

		animSounds->keyFrame = animations[animNum].firstFrame;

		// Get offset to frame within sequence
		token = COM_Parse( text_p );
		if ( !token ) 
		{
			break;
		}
		animSounds->keyFrame += atoi( token );

		//get soundstring
		token = COM_Parse( text_p );
		if ( !token ) 
		{
			break;
		}		
		strcpy(soundString, token);

		//get lowest value
		token = COM_Parse( text_p );
		if ( !token ) 
		{//WARNING!  BAD TABLE!
			break;
		}
		lowestVal = atoi( token );

		//get highest value
		token = COM_Parse( text_p );
		if ( !token ) 
		{//WARNING!  BAD TABLE!
			break;
		}
		highestVal = atoi( token );

		//Now precache all the sounds
		//NOTE: If we can be assured sequential handles, we can store sound indices
		//		instead of strings, unfortunately, if these sounds were previously
		//		registered, we cannot be guaranteed sequential indices.  Thus an array
		if(lowestVal && highestVal)
		{
			for ( n = lowestVal, num = 0; n <= highestVal && num < MAX_RANDOM_ANIMSOUNDS; n++, num++ )
			{
				animSounds->soundIndex[num] = trap_S_RegisterSound( va( soundString, n ) );
			}
			animSounds->numRandomAnimSounds = num - 1;
		}
		else
		{
			animSounds->soundIndex[0] = trap_S_RegisterSound( va( soundString ) );
#ifndef FINAL_BUILD
			if ( !animSounds->soundIndex[0] )
			{//couldn't register it - file not found
				Com_Printf( S_COLOR_RED "ParseAnimationSndBlock: sound %s does not exist (animsound.cfg %s)!\n", soundString, filename );
			}
#endif
			animSounds->numRandomAnimSounds = 0;
		}


		//get probability
		token = COM_Parse( text_p );
		if ( !token ) 
		{//WARNING!  BAD TABLE!
			break;
		}

		animSounds->probability = atoi( token );
		++animSounds;
		++*i;
	}	
}


/*
======================
CG_ParseAnimationSndFile

Read a configuration file containing animation sounds 
models/players/munro/animsounds.cfg, etc

This file's presence is not required

======================
*/
static int CG_ParseAnimationSndFile( const char *filename, int animFileIndex ) 
{
	char		*text_p;
	int			len;
	char		*token;
	char		text[20000];
	fileHandle_t	f;
	int			i, j, upper_i, lower_i;
	animsounds_t	*lowerAnimSounds;
	animsounds_t	*upperAnimSounds;
	animation_t		*animations;

	/*if ( knownAnimFileSets[animFileIndex].soundsCached )
	{//already cached this one
		return;
	}*/

	for ( i = 0; i < cg_numSndAnims; i++ ) {
		if ( !Q_stricmp( filename, cg_animsSndList[i].animSndFileRoute ) )  {
			return i;
		}
	}

	//Mark this anim set so that we know we tried to load he sounds, don't care if the load failed
	//knownAnimFileSets[animFileIndex].soundsCached = qtrue;

	animations		= cg_animsList[animFileIndex].animations;
	lowerAnimSounds = cg_animsSndList[cg_numSndAnims].lowerAnimSounds;
	upperAnimSounds = cg_animsSndList[cg_numSndAnims].upperAnimSounds;

	//initialize anim sound array
	for(i = 0; i < MAX_ANIM_SOUNDS; i++)
	{
		upperAnimSounds[i].numRandomAnimSounds = 0;
		lowerAnimSounds[i].numRandomAnimSounds = 0;
		for(j = 0; j < MAX_RANDOM_ANIMSOUNDS; j++)
		{
			upperAnimSounds[i].soundIndex[j] = -1;
			lowerAnimSounds[i].soundIndex[j] = -1;
		}
	}

	// load the file
	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( len <= 0 ) 
	{//no file
		return -1;
	}
	if ( len >= sizeof( text ) - 1 ) 
	{
		CG_Printf( "File %s too long\n", filename );
		return -1;
	}

	trap_FS_Read( text, len, f );
	text[len] = 0;
	trap_FS_FCloseFile( f );

	// parse the text
	text_p = text;
	upper_i =0;
	lower_i =0;

	// read information for batches of sounds (UPPER or LOWER)
	while ( 1 ) 
	{
		// Get base frame of sequence
		token = COM_Parse( &text_p );
		if ( !token || !token[0] ) 
		{
			break;
		}

		if ( !Q_stricmp(token,"UPPERSOUNDS") )	// A batch of upper sounds
		{
			ParseAnimationSndBlock( filename, upperAnimSounds, animations, &upper_i, &text_p ); 
		}

		else if ( !Q_stricmp(token,"LOWERSOUNDS") )	// A batch of lower sounds
		{
			ParseAnimationSndBlock( filename, lowerAnimSounds, animations, &lower_i, &text_p ); 
		}
	}

	i = cg_numSndAnims;
	cg_numSndAnims++;

	return i;
}

/*
=============================================================================

MODEL SCRIPT LOADING

=============================================================================
*/

/*
======================
CG_ParseAnimationFile

Read a configuration file containing animation coutns and rates
models/players_rpgx/munro/animation.cfg, etc

TiM: Small modification.  Based on the nuber of animations parsed,
this will return an index to the cell in the global animation array 
where the relevant animation data is kept.  Based on both the JKA concept and the EF method
of caching assets, this is far more efficient since it means that if two people use models
with the same body models using the same animation data, the anim data will ony need be loaded once :)

A lot more efficient considering how many freakin more animations we introduced with this model system lol.
======================
*/
//
//static qboolean	CG_ParseAnimationFile( const char *filename, clientInfo_t *ci ) {
static int	CG_ParseAnimationFile( const char* filename/*, clientInfo_t *ci*/ ) {
	char		*text_p, *prev;
	int			len;
	int			i;
	char		*token;
	float		fps;
	int			skip;
	char		text[20000];
	fileHandle_t	f;
	animation_t *animations;

	//CG_Printf( "Anim is %s\n", filename );

	if ( cg_numAnims > 0 ) {
		for ( i = 0; i <= cg_numAnims; i++ ) {
			if ( !Q_stricmpn( cg_animsList[i].animFileRoute, filename, (int)strlen( filename ) ) ) { //We found a matching anim set
				//Com_Printf( S_COLOR_RED "Using index: %i\n", i );
				return i;
			}
		}
	}

	// load the file
	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( len <= 0 ) {
		CG_Printf( S_COLOR_RED "File %s not found\n", filename );
		return -1; //qfalse
	}
	if ( len >= sizeof( text ) - 1 ) {
		CG_Printf( S_COLOR_RED "File %s too long\n", filename );
		return -1; //qfalse
	}
	trap_FS_Read( text, len, f );
	text[len] = 0;
	trap_FS_FCloseFile( f );


	//animations = ci->animations;
	animations = cg_animsList[cg_numAnims].animations;

	//copy the file name to the gloabl anims array.  It doesn't matter
	//if it returns false, since the same cell will be flushed on the next call then.
	memset( cg_animsList[cg_numAnims].animFileRoute, 0, MAX_QPATH );
	Q_strncpyz( cg_animsList[cg_numAnims].animFileRoute, filename, MAX_QPATH );

	//flush the anims
	memset( animations, 0, sizeof( animations ) );

	// parse the text
	text_p = text;
	skip = 0;	// quite the compiler warning

	/*
	ci->footsteps = FOOTSTEP_NORMAL;
	VectorClear( ci->headOffset );
	ci->gender = GENDER_MALE;
	
	Q_strncpyz(ci->soundPath, ci->modelName, sizeof(ci->soundPath));*/

	// read optional parameters
	while ( 1 ) {
		prev = text_p;	// so we can unget
		token = COM_Parse( &text_p );
		if ( !token ) {
			break;
		}
		/*if ( !Q_stricmp( token, "footsteps" ) ) {
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}
			if ( !Q_stricmp( token, "default" ) || !Q_stricmp( token, "normal" ) ) {
				ci->footsteps = FOOTSTEP_NORMAL;
			} else if ( !Q_stricmp( token, "borg" ) ) {
				ci->footsteps = FOOTSTEP_BORG;
			} else if ( !Q_stricmp( token, "reaver" ) ) {
				ci->footsteps = FOOTSTEP_REAVER;
			} else if ( !Q_stricmp( token, "species" ) ) {
				ci->footsteps = FOOTSTEP_SPECIES;
			} else if ( !Q_stricmp( token, "warbot" ) ) {
				ci->footsteps = FOOTSTEP_WARBOT;
			} else if ( !Q_stricmp( token, "boot" ) ) {		
				ci->footsteps = FOOTSTEP_BOOT;
			} else if ( !Q_stricmp( token, "flesh" ) ) {	// Old Q3 defaults, for compatibility.	-PJL
				ci->footsteps = FOOTSTEP_SPECIES;
			} else if ( !Q_stricmp( token, "mech" ) ) {		// Ditto
				ci->footsteps = FOOTSTEP_BORG;
			} else if ( !Q_stricmp( token, "energy" ) ) {	// Ditto
				ci->footsteps = FOOTSTEP_BORG;
			} else {
				CG_Printf( "Bad footsteps parm in %s: %s\n", filename, token );
			}
			continue;
		} else if ( !Q_stricmp( token, "headoffset" ) ) {
			for ( i = 0 ; i < 3 ; i++ ) {
				token = COM_Parse( &text_p );
				if ( !token ) {
					break;
				}
				ci->headOffset[i] = atof( token );
			}
			continue;
		} else if ( !Q_stricmp( token, "sex" ) ) {
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}
			if ( token[0] == 'f' || token[0] == 'F' ) {
				ci->gender = GENDER_FEMALE;
			} else if ( token[0] == 'n' || token[0] == 'N' ) {
				ci->gender = GENDER_NEUTER;
			} else {
				ci->gender = GENDER_MALE;
			}
			continue;
		} else if ( !Q_stricmp( token, "soundpath" ) ) {
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}
			Q_strncpyz(ci->soundPath,token,sizeof (ci->soundPath) );
			continue;
		} */

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
		if ( !token ) {
			break;
		}
		animations[i].firstFrame = atoi( token );
		// leg only frames are adjusted to not count the upper body only frames
		if ( i == LEGS_KNEEL1 ) { //LEGS_WALKCR
			skip = animations[LEGS_KNEEL1].firstFrame - animations[TORSO_ACTIVATEMEDKIT1].firstFrame; //TORSO_GESTURE
		}
		if ( i >= LEGS_KNEEL1 ) {
			animations[i].firstFrame -= skip;
		}

		token = COM_Parse( &text_p );
		if ( !token ) {
			break;
		}
		animations[i].numFrames = atoi( token );

		token = COM_Parse( &text_p );
		if ( !token ) {
			break;
		}
		animations[i].loopFrames = atoi( token );

		token = COM_Parse( &text_p );
		if ( !token ) {
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
		CG_Printf( S_COLOR_RED "Error parsing animation file: %s", filename );
		//return qfalse;
		return -1;
	}

	//CG_Printf( S_COLOR_RED "Cached File: %s\n", cgs.animsList[cgs.numAnims].animFileRoute );
	//return ++cg_numAnims;

	i = cg_numAnims;
	if ( cg_numAnims < MAX_CLIENTS ) {
		cg_numAnims++; //offset for the next time :)
	}

	return i;
	//return qtrue;
}

/*
======================
CG_InitModelData
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

static void CG_InitModelData( clientInfo_t *ci ) {

	ci->holsterModel = 0;
	ci->hasRanks = qfalse;

	//initialize all model + skin data as 0, so it can be told if they don't get
	//values assigned in the script parser, in which case we exit.
	ci->headModel = 0;
	ci->torsoModel = 0;
	ci->legsModel = 0;

	ci->headSkin = 0;
	ci->headSkinBlink = 0; //doesn't matter if left 0; won't end the parser
	ci->headSkinFrown = 0;
	ci->headSkinFrownBlink = 0;
	ci->torsoSkin = 0;
	ci->legsSkin = 0;

	//doesn't matter if left 0
	ci->headBlinkTime.minSeconds = 0;
	ci->headBlinkTime.maxSeconds = 0;
	
	ci->nextTalkTime = 0;
	ci->currentTalkSkin = 0;

	ci->headSkinTalk[0] = 0;
	ci->headSkinTalk[1] = 0;
	ci->headSkinTalk[2] = 0;
	ci->headSkinTalk[3] = 0;

	//animation.cfg former inits
	ci->footsteps = FOOTSTEP_NORMAL;
	VectorClear( ci->headOffset );
	ci->gender = GENDER_MALE;
	Q_strncpyz(ci->soundPath, ci->charName, sizeof(ci->soundPath));

	//set animIndex to -1. if still -1 at the end, we return false, coz we gotz no anims
	ci->animIndex = -1;
	ci->animSndIndex = -1;

	memset( &ci->boltonTags, 0, sizeof(ci->boltonTags));
	
	ci->isHazardModel = qfalse;
}

/*
=====================
CG_ParseSkinSetDataFile
by TiM

Parses a separate.skinset
file to get the skin data 
for this model.
======================
*/

static qboolean CG_ParseSkinSetDataFile( clientInfo_t *ci, const char *skinSetFrame, const char *charName, const char *skinName )
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

	if ( ( skinStar = strstr( skinSetFrame, "*" ) ) == NULL )
	{
		CG_Printf( S_COLOR_RED "ERROR: No '*' specified in model skin set!\n" );
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
			CG_Printf( "ERROR: The '*' in %s must be on either the start or end, not the middle.\n", skinSetFrame );
			return qfalse;
		}
	}

	//CG_Printf( S_COLOR_RED "DEBUG: skinSetName = %s \n", skinSetName );

	Com_sprintf( skinSetRoute, sizeof( skinSetRoute ), "models/players_rpgx/%s/%s.skinset", charName, skinSetName );

	len = trap_FS_FOpenFile( skinSetRoute, &f, FS_READ );

	if ( len <= 0 )
	{
		CG_Printf( S_COLOR_RED "ERROR: Could not open file: %s\n", skinSetRoute );
		return qfalse;
	}

	if ( len > sizeof( buffer) - 1 )
	{
		CG_Printf( S_COLOR_RED "ERROR: Imported file is too big for buffer: %s. Len is %i\n", skinSetRoute, len );
		return qfalse;
	}

	trap_FS_Read( buffer, len, f );

	trap_FS_FCloseFile( f );

	if ( !buffer[0] )
	{
		CG_Printf( S_COLOR_RED "ERROR: Could not import data from %s\n", skinSetRoute );
		return qfalse;
	}

	buffer[len] = '\0';

	textPtr = buffer;

	token = COM_Parse( &textPtr );

	if ( Q_stricmp( token, "{" ) )
	{
		CG_Printf( S_COLOR_RED "ERROR: Skinset %s did not start with a '{'\n", skinSetRoute );
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
			if ( !Q_stricmp( token, "headSkinBlink" ) ) {
				if ( COM_ParseString( &textPtr, &token ) ) {
					continue;
				}

				if ( !cg_noBlinkingHeads.integer ) {
					ci->headSkinBlink = trap_R_RegisterSkin( token );

					//We'll alert them, but not cancel the loop
					if ( !ci->headSkinBlink )
						Com_Printf( S_COLOR_RED "WARNING: Couldn't load headSkinBlink: %s\n", token);
				}

				continue;
			}

			//head blink time
			else if ( !Q_stricmpn( token, "headBlinkTime", 13 ) ) {
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

				if ( !cg_noBlinkingHeads.integer ) {
					ci->headBlinkTime.minSeconds = n;
					ci->headBlinkTime.maxSeconds = i;
				}
				continue;
			}

			else if ( !Q_stricmpn( token, "headSkinFrownBlink", 18 ) ) {
				if ( COM_ParseString( &textPtr, &token ) ) {
					continue;
				}

				if ( !cg_noFrowningHeads.integer ) {
					ci->headSkinFrownBlink = trap_R_RegisterSkin( token );

					if ( !ci->headSkinFrownBlink )
						Com_Printf( S_COLOR_RED "WARNING: Was unable to load frown blink skin: %s\n", token );
				}

				continue;
			}

			else if ( !Q_stricmpn( token, "headSkinFrown", 13 ) ) {
				if ( COM_ParseString( &textPtr, &token ) ) {
					continue;
				}

				//Only cache if we want to
				if ( !cg_noFrowningHeads.integer ) {
					ci->headSkinFrown = trap_R_RegisterSkin( token );
				}

				if ( !cg_noFrowningHeads.integer && !ci->headSkinFrown ) {
					Com_Printf( S_COLOR_RED "WARNING: Couldn't load frowning skin: %s\n", token );
				}
				continue;
			}

			else if ( !Q_stricmpn( token, "torsoSkin", 9 ) ) {
				if (COM_ParseString( &textPtr, &token ) ) {
					continue;
				}

				ci->torsoSkin = trap_R_RegisterSkin( token );
				if (!ci->torsoSkin ) {
					Com_Printf( S_COLOR_RED "ERROR: Couldn't load torsoSkin: %s\n", token);
				}
				continue;
			}

			else if ( !Q_stricmpn( token, "legsSkin", 8 ) ) {
				if (COM_ParseString( &textPtr, &token ) ) {
					continue;
				}

				ci->legsSkin = trap_R_RegisterSkin( token );
				if (!ci->legsSkin ) {
					Com_Printf( S_COLOR_RED "ERROR: Couldn't load legsSkin: %s\n", token);
				}
				continue;
			}

			else if ( !Q_stricmpn( token, "headSkinTalk", 12 ) && !cg_noTalkingHeads.integer ) {
				
				token = COM_Parse( &textPtr ); //parse
				if ( !token[0] ) { //error check
					break;
				}

				// if we found no {, then scrub the whole thing
				if ( Q_stricmpn( token, "{", 1 ) ) {
					continue;
				}
				else {
					i = 0;
					while ( 1 ) {
						token = COM_Parse( &textPtr ); //parse
						if ( !token[0] ) { //error check
							break;
						}
						
						ci->headSkinTalk[i] = trap_R_RegisterSkin( token );
						if ( !ci->headSkinTalk[i] ) {
							Com_Printf( S_COLOR_RED "ERROR: Unable to parse headSkinTalk file: %s\n", token );
							break;
						}
						i++;

						//Com_Printf("Registered Skin: %i\n", i);

						if ( !Q_stricmpn( token, "}", 1) ) {
							break;
						}

						if ( i >= MAX_TALK_SKINS ) {
							break;
						}
					}
				}
				continue;
			}
			//head skin
			else if ( !Q_stricmpn( token, "headSkin", 8 ) ) {
				if ( COM_ParseString( &textPtr, &token ) ) {
					continue;
				}

				ci->headSkin = trap_R_RegisterSkin( token );
				if ( !ci->headSkin ) {
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
CG_ParseModelDataFile
by TiM

Reads in the .model file 
needed to put together 
a character model.
======================
*/
static qboolean CG_ParseModelDataFile( clientInfo_t *ci, const char *charName, 
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
	char			animSndFileRoute[MAX_QPATH];
	qboolean		skinSetFound=qfalse;
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
	CG_InitModelData( ci );
	
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
			
			ci->animIndex = CG_ParseAnimationFile( token );

			//no valid anim file found.  Don't give up hope though.
			//We have a backup resort at the end if need be. :)
			if ( ci->animIndex == -1 ) {
				Com_Printf( S_COLOR_RED "WARNING: Was unable to load file %s.\n", token );
			}

			continue;
		}

		//anim sounds config file
		else if ( !Q_stricmpn( token, "animSoundsConfig", 16 ) ) {
			if ( COM_ParseString( &textPtr, &token ) ) {
				continue;
			}

			//check to see if we have a valid animlist we can sync these
			//sounds to.  if not, we'll put the file route asside, and
			//try again at the end.
			if ( ci->animIndex >= 0 ) {
				ci->animSndIndex = CG_ParseAnimationSndFile( token, ci->animIndex );
				
				if ( ci->animSndIndex == -1 ) {
					Com_Printf( S_COLOR_RED "WARNING: Unable to load file: %s\n", token );
				}
			}
			else {
				Q_strncpyz( animSndFileRoute, token, sizeof( animSndFileRoute ) );
			}

			continue;
		}

		//character's legs model
		else if ( !Q_stricmpn( token, "legsModel", 9 ) ) {
			
			if( COM_ParseString( &textPtr, &token ) ) {
				continue;
			}

			ci->legsModel = trap_R_RegisterModel( token );
			if (!ci->legsModel) {
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
			ci->torsoModel = trap_R_RegisterModel( token );
			//Com_Printf("Torsomodel passed as %s, %i\n", token, (int)ci->torsoModel);

			if (!ci->torsoModel) {
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

			ci->headModel = trap_R_RegisterModel( token );
			if (!ci->headModel) {
				Com_Printf( S_COLOR_RED "ERROR: Unable to load head model: %s\n", token);
				return qfalse;
			}
			continue;
		}

		//holster model (basically just a null md3 with 2 tags: one for phaser, other for tric)
		else if ( !Q_stricmpn( token, "holsterModel", 12 ) ) {
			
			if( COM_ParseString( &textPtr, &token ) ) {
				continue;
			}

			ci->holsterModel = trap_R_RegisterModel( token );

			//You'd hope like hell this will never happen. :P	
			if (!ci->holsterModel) {
				Com_Printf( S_COLOR_RED "ERROR: Unable to load holster model: %s\n", token);
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
							
							ci->boltonTags[i].modelBase = GetIDForString( BoltonTable, token );

							if( COM_ParseString( &textPtr, &token ) ) {
								continue;
							}

							if (!Q_stricmpn( token, "tag_", 4 ) ) {
								Q_strncpyz(ci->boltonTags[i].tagName, token, sizeof (ci->boltonTags[i].tagName) );
							
								if( COM_ParseString( &textPtr, &token ) ) {
									continue;
								}
								ci->boltonTags[i].tagModel = trap_R_RegisterModel( token );
								
								if (!ci->boltonTags[i].tagModel) {
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
			if ( n > 0 )
				ci->hasRanks = qtrue;
			else
				ci->hasRanks = qfalse;
			continue;
		} 

		//player footsteps.
		//FIXME: Is it possible to make these things dynamic, so we can
		//put in our own footstep sounds?
		/*else if ( !Q_stricmp( token, "footsteps" ) ) {
			token = COM_Parse( &textPtr );
			if ( !token ) {
				break;
			}
			if ( !Q_stricmp( token, "default" ) || !Q_stricmp( token, "normal" ) ) {
				ci->footsteps = FOOTSTEP_NORMAL;
			} else if ( !Q_stricmp( token, "borg" ) ) {
				ci->footsteps = FOOTSTEP_BORG;
			} else if ( !Q_stricmp( token, "reaver" ) ) {
				ci->footsteps = FOOTSTEP_REAVER;
			} else if ( !Q_stricmp( token, "species" ) ) {
				ci->footsteps = FOOTSTEP_SPECIES;
			} else if ( !Q_stricmp( token, "warbot" ) ) {
				ci->footsteps = FOOTSTEP_WARBOT;
			} else if ( !Q_stricmp( token, "boot" ) ) {		
				ci->footsteps = FOOTSTEP_BOOT;
			} else if ( !Q_stricmp( token, "flesh" ) ) {	// Old Q3 defaults, for compatibility.	-PJL
				ci->footsteps = FOOTSTEP_SPECIES;
			} else if ( !Q_stricmp( token, "mech" ) ) {		// Ditto
				ci->footsteps = FOOTSTEP_BORG;
			} else if ( !Q_stricmp( token, "energy" ) ) {	// Ditto
				ci->footsteps = FOOTSTEP_BORG;
			} else {
				CG_Printf( "Bad footsteps parm in %s: %s\n", fileName, token );
			}
			continue;
		} */
		
		//offset for player head in the scoreboard or whatever
		else if ( !Q_stricmp( token, "headoffset" ) ) {
			for ( i = 0 ; i < 3 ; i++ ) {
				token = COM_Parse( &textPtr );
				if ( !token ) {
					break;
				}
				ci->headOffset[i] = atof( token );
			}
			continue;
		}

		//what gender the character is
		else if ( !Q_stricmpn( token, "sex", 3 ) ) {
			if (COM_ParseString( &textPtr, &token ) ) {
				continue;
			}
			if ( token[0] == 'f' || token[0] == 'F' ) {
				ci->gender = GENDER_FEMALE;
			} else if ( token[0] == 'n' || token[0] == 'N' ) {
				ci->gender = GENDER_NEUTER;
			} else {
				ci->gender = GENDER_MALE;
			}
			continue;
		} 

		//file path to model sound files
		else if ( !Q_stricmpn( token, "soundPath", 9 ) ) {
			if (COM_ParseString( &textPtr, &token ) ){
				continue;
			}
			
			Q_strncpyz( ci->soundPath, token, sizeof(ci->soundPath) );
			continue;
		}

		//TiM - The skinset is defined
		else if ( !Q_stricmpn( token, "skinSet", 7 ) ) {
			if ( COM_ParseString( &textPtr, &token ) ) {
				continue;
			}
			
			if ( CG_ParseSkinSetDataFile( ci, token, charName, skinName ) )
			{
				skinSetFound = qtrue;
			}
			
			continue;
		}
	}

	//if any of the models or skins were left blank, then output false. Coz we need them. :P
	if (!ci->headModel || !ci->torsoModel || !ci->legsModel ) {
		Com_Printf( S_COLOR_RED "One or more necessary model files weren't loaded from %s\n", fileName );
		return qfalse;
	}

	if ( !skinSetFound )
	{
		if ( !CG_ParseSkinSetDataFile( ci, va("%s_*", modelName, skinName ), charName, skinName ) )
		{
			CG_Printf( S_COLOR_RED "ERROR: Tried loading default skin set, however it failed.\n");
		}	
	}

	if (!ci->headSkin || !ci->torsoSkin || !ci->legsSkin ) {
		
		Com_Printf( S_COLOR_RED "One or more necessary skin files weren't loaded from %s\n", fileName );
		return qfalse;
	}

	//if modder specified no animations file route, or they did, and it sucked (ie -1 ),
	//Then try looking for one in the same directory as the lower.mdr file

	//k... the goal of this is to take a string like
	//models/players_rpgx/crewman_male/lower.mdr
	//and turn it into
	//models/players_rpgx/crewman_male/animation.cfg

	if ( ci->animIndex == -1 && strlen( legsFileRoute ) > 0 ) {
		//get length of file route
		i = (int)strlen(legsFileRoute);

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

		//parse it ^_^
		ci->animIndex = CG_ParseAnimationFile( legsFileRoute );

		if ( ci->animIndex < 0 ) {
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

	//We'll check again if we can load a sound config file after everything else
	if ( ci->animSndIndex == -1 && animSndFileRoute[0] )
	{
		ci->animSndIndex = CG_ParseAnimationSndFile( animSndFileRoute, ci->animIndex );
		
		if ( ci->animSndIndex == -1 ) {
			Com_Printf( S_COLOR_RED "ERROR: Unable to load sound config file: %s.\n", animSndFileRoute );
		}
	}

	ci->animsFlushed = qfalse;

	//TiM: Cheap hack - let us specifically check for hazard models
	if ( !Q_stricmp( modelName, "hazard" ) )
		ci->isHazardModel = qtrue;

	//holy fudgenuggets.  after all that checking, we actually made it to the end and have a valid freaking
	//model! OWNED!
	return qtrue;
}

/*
=============================================================================

CLIENT INFO

=============================================================================
*/

/*
//This function has been rpg-x'ed®! (by J2J and fixed by RedTechie)
static qboolean	CG_ParseAnimationFile( const char *filename, clientInfo_t *ci ) {
	char		*text_p;
	int			len;
	int			i;
	char		*token;
//	char		aniname[255];
	float		fps;
	int			skip;
	char		text[20000];
//	char		text2[20000];
	fileHandle_t	f;
	animation_t *animations;

	//Com_Printf(S_COLOR_RED"MAX_ANIMATIONS = %i\n", MAX_ANIMATIONS);			//not needed

	animations = ci->animations;

	// load the file
	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( len <= 0 ) {
		return qfalse;
	}
	if ( len >= sizeof( text ) - 1 ) {
		CG_Printf( "File %s too long\n", filename );
		return qfalse;
	}
	trap_FS_Read( text, len, f );
	text[len] = 0;
	trap_FS_FCloseFile( f );

	// parse the text
	text_p = text;
	skip = 0;	// quite the compiler warning

	//initialize anim array so that from 0 to MAX_ANIMATIONS, set default values of 0 1 0 100
	for(i = 0; i < MAX_ANIMATIONS; i++)
	{
		animations[i].firstFrame = 0;
		animations[i].numFrames = 0;
		animations[i].loopFrames = -1;
		animations[i].frameLerp = 100; //Before redtechie change 0
		animations[i].initialLerp = 100;//Before redtechie change 0
	}

	while(1) {

		token = COM_Parse( &text_p );
		if ( !token || !token[0] ) {
			break;
		}
		
		i = GetIDForString(animTable, token);

		if(i == -1)
		{
			Com_Printf(S_COLOR_RED"WARNING: Unknown token %s in %s\n", token, filename);
			continue;
		}
		
		token = COM_Parse( &text_p );
		if ( !token ) {
			break;
		}
		animations[i].firstFrame = atoi( token );

		token = COM_Parse( &text_p );
		if ( !token ) {
			break;
		}
		animations[i].numFrames = atoi( token );

		token = COM_Parse( &text_p );
		if ( !token ) {
			break;
		}

		animations[i].loopFrames = atoi( token );

		token = COM_Parse( &text_p );
		if ( !token ) {
			break;
		}
		fps = atof( token );
		
		//Com_Printf(S_COLOR_RED"INFO: fps = %s - %f\n", token, fps);			//debug only

		if ( fps == 0 ) 
		{
			fps = 1;//Don't allow divide by zero error
		}
		if ( fps < 0 )
		{//backwards
			animations[i].frameLerp = floor(1000.0f / fps);
		}
		else
		{
			animations[i].frameLerp = ceil(1000.0f / fps);
		}

		animations[i].initialLerp = ceil(1000.0f / fabs(fps));
		
	}
	return qtrue;
}


*/
/*
==========================
\CG_RegisterClientSkin
==========================
*/
/*
==========================
CG_RegisterClientSkin
==========================
*/
/*static qboolean	CG_RegisterClientSkin( clientInfo_t *ci, const char *modelName, const char *skinName ) {
	char		filename[MAX_QPATH];

	Com_sprintf( filename, sizeof( filename ), "models/players2/%s/lower_%s.skin", modelName, skinName );
	ci->legsSkin = trap_R_RegisterSkin( filename );

	Com_sprintf( filename, sizeof( filename ), "models/players2/%s/upper_%s.skin", modelName, skinName );
	ci->torsoSkin = trap_R_RegisterSkin( filename );

	Com_sprintf( filename, sizeof( filename ), "models/players2/%s/head_%s.skin", modelName, skinName );
	ci->headSkin = trap_R_RegisterSkin( filename );

	//Com_sprintf( filename, sizeof( filename ), "models/players2/%s/groups.cfg", modelName);
	//strcpy(ci->race, BG_RegisterRace( filename ));

	if ( !ci->legsSkin || !ci->torsoSkin || !ci->headSkin ) {
		return qfalse;
	}

	return qtrue;
}*/

/*
==========================
CG_RegisterClientModelname
==========================
*/
static qboolean CG_RegisterClientModelname( clientInfo_t *ci, const char *charName, const char *modelName, const char *skinName ) {
	char		filename[MAX_QPATH];

	// load cmodels before models so filecache works

	/*Com_sprintf( filename, sizeof( filename ), "models/players2/%s/lower.mdr", modelName );
	ci->legsModel = trap_R_RegisterModel( filename );
	if ( !ci->legsModel ) {
		Com_sprintf( filename, sizeof( filename ), "models/players2/%s/lower.md3", modelName );
		ci->legsModel = trap_R_RegisterModel( filename );
		if ( !ci->legsModel ) {
			Com_Printf( S_COLOR_RED"Failed to load model file %s\n", filename );
			return qfalse;
		}
	}
	Com_sprintf( filename, sizeof( filename ), "models/players2/%s/upper.mdr", modelName );
	ci->torsoModel = trap_R_RegisterModel( filename );
	if ( !ci->torsoModel ) {
		Com_sprintf( filename, sizeof( filename ), "models/players2/%s/upper.md3", modelName );
		ci->torsoModel = trap_R_RegisterModel( filename );
		if ( !ci->torsoModel ) {
			Com_Printf( "Failed to load model file %s\n", filename );
			return qfalse;
		}
	}

	Com_sprintf( filename, sizeof( filename ), "models/players2/%s/head.md3", modelName );
	ci->headModel = trap_R_RegisterModel( filename );
	if ( !ci->headModel ) {
		Com_Printf( "Failed to load model file %s\n", filename );
		return qfalse;
	}

	// if any skins failed to load, return failure
	if ( !CG_RegisterClientSkin( ci, modelName, skinName ) ) {
		Com_Printf( "Failed to load skin file: %s : %s\n", modelName, skinName );
		return qfalse;
	}

	// load the animations
	Com_sprintf( filename, sizeof( filename ), "models/players2/%s/animation.cfg", modelName );
	if ( !CG_ParseAnimationFile( filename, ci ) ) {\
		Com_Printf( "Failed to load animation file %s\n", filename );
		return qfalse;
	}*/

	if ( !CG_ParseModelDataFile( ci, charName, modelName, skinName) ) {
		//Com_Printf( S_COLOR_RED "ERROR: Failed to parse .model file for character: %s/%s/%s\n", charName, modelName, skinName );
		return qfalse;
	}

	/*Com_sprintf( filename, sizeof( filename ), "models/players2/%s/icon_%s.jpg", modelName, skinName );
	ci->modelIcon = trap_R_RegisterShaderNoMip( filename );
	if ( !ci->modelIcon ) {
		Com_Printf( "Failed to load icon file: %s\n", filename );
		return qfalse;
	}*/

	Com_sprintf( filename, sizeof( filename ), "models/players_rpgx/%s/model_icon.jpg", charName );
	ci->modelIcon = trap_R_RegisterShaderNoMip( filename );
	if ( !ci->modelIcon ) {
		Com_Printf( S_COLOR_RED "ERROR: Failed to load icon file: %s\n", filename );
		//return qfalse;
	}

	return qtrue;
}

/*
====================
CG_ColorFromString
====================
*/
/*static void CG_ColorFromString( const char *v, vec3_t color ) {
	int val;

	VectorClear( color );

	val = atoi( v );

	if ( val < 1 || val > 7 ) {
		VectorSet( color, 1, 1, 1 );
		return;
	}

	if ( val & 1 ) {
		color[2] = 1.0f;
	}
	if ( val & 2 ) {
		color[1] = 1.0f;
	}
	if ( val & 4 ) {
		color[0] = 1.0f;
	}
}*/

/*
===================
CG_LoadClientInfo

Load it now, taking the disk hits.
This will usually be deferred to a safe time
===================
*/
static void CG_LoadClientInfo( clientInfo_t *ci , int clientNum) {
	const char	*dir, *fallback;
	int			i;
	const char	*s;
	char		temp_string[200];

	//if ( !CG_RegisterClientModelname( ci, ci->modelName, ci->skinName ) ) {

	//Com_Printf("charName = %s, modelName = %s, skinName = %s\n", ci->charName, ci->modelName, ci->skinName);
	if ( !CG_RegisterClientModelname( ci, ci->charName, ci->modelName, ci->skinName ) ) {
		if ( cg_buildScript.integer ) {
			CG_Error( "CG_RegisterClientModelname( %s/%s/%s ) failed", ci->charName, ci->modelName, ci->skinName );
		}
		
		Com_Printf( S_COLOR_RED "ERROR: Failed to parse .model file for character: %s/%s/%s\n", ci->charName, ci->modelName, ci->skinName );

		if ( !CG_RegisterClientModelname( ci, ci->charName, DEFAULT_MODEL, ci->skinName ) )
		{
			if ( !CG_RegisterClientModelname( ci, cg_defaultChar.string, DEFAULT_MODEL, ci->skinName ) )
			{
				// fall back
				if ( cgs.gametype >= GT_TEAM ) 
				{
					// keep skin name
					if ( !CG_RegisterClientModelname( ci, DEFAULT_CHAR, DEFAULT_MODEL, ci->skinName ) ) {
						CG_Error( "DEFAULT_CHAR / model /skin ( %s/%s/%s ) failed to register",
							DEFAULT_CHAR, DEFAULT_MODEL, ci->skinName );
					}
				} 
				else 
				{
					if ( !CG_RegisterClientModelname( ci, cg_defaultChar.string, DEFAULT_MODEL, DEFAULT_SKIN ) )
					{
						if ( !CG_RegisterClientModelname( ci, DEFAULT_CHAR, DEFAULT_MODEL, DEFAULT_SKIN ) ) 
						{
							CG_Error( "DEFAULT_CHAR (%s) failed to register", DEFAULT_CHAR );
						}
					}
				}
			}
		}
	}

	// sounds
	dir = ci->soundPath;
	fallback = (ci->gender==GENDER_FEMALE)?"hm_female":"hm_male";

	for ( i = 0 ; i < MAX_CUSTOM_SOUNDS ; i++ ) {
		s = cg_customSoundNames[i];
		if ( !s ) {
			break;
		}
		ci->sounds[i] = trap_S_RegisterSound( va("sound/voice/%s/misc/%s", dir, s + 1) );
		if ( !ci->sounds[i] ) {
			ci->sounds[i] = trap_S_RegisterSound( va("sound/player/%s/%s", fallback, s + 1) );
		}
	}

	ci->deferred = qfalse;

	Com_sprintf(temp_string, sizeof(temp_string), "%s/%s/%s", ci->charName, ci->modelName, ci->skinName);
	updateSkin(clientNum, temp_string);

	// reset any existing players and bodies, because they might be in bad
	// frames for this new model
	for ( i = 0 ; i < MAX_GENTITIES ; i++ ) {
		if ( cg_entities[i].currentState.clientNum == clientNum
			&& cg_entities[i].currentState.eType == ET_PLAYER ) 
		{
			CG_ResetPlayerEntity( &cg_entities[i] );
		}
	}
}


// we need to check here to see if the clientinfo model variable is the same as the one that is in the 
// clientinfo block. This is because it is possible for the server to change skins on us when we hit a CTF
// teamplay game where groups are defined.
// most of the time this will not hit

void updateSkin(int clientNum, char *new_model)
{
	char		model_string[200];

	// create string to be checked against
	trap_Cvar_VariableStringBuffer("model", model_string, sizeof(model_string) );

	if (Q_stricmp(new_model, model_string) && cg.validPPS && (clientNum == cg.predictedPlayerState.clientNum))
	{
		trap_Cvar_Set_No_Modify ("model",new_model);
	}
}


/*
======================
CG_CopyClientInfoModel
======================
*/
static void CG_CopyClientInfoModel( clientInfo_t *from, clientInfo_t *to ) {
	//int i;

	VectorCopy( from->headOffset, to->headOffset );
	to->footsteps = from->footsteps;
	to->gender = from->gender;
	to->numTaunts = from->numTaunts;

	to->legsModel = from->legsModel;
	to->legsSkin = from->legsSkin;
	to->torsoModel = from->torsoModel;
	to->torsoSkin = from->torsoSkin;
	to->headModel = from->headModel;
	to->headSkin = from->headSkin;
	to->modelIcon = from->modelIcon;

	to->animIndex = from->animIndex;
	to->animSndIndex = from->animSndIndex;

	to->hasRanks	 = from->hasRanks;

	//Blinking
	if ( from->headSkinBlink ) {
		to->headSkinBlink = from->headSkinBlink;

		if ( from->headBlinkTime.maxSeconds > 0 ) {
			//memcpy( to->headBlinkTime, from->headBlinkTime, sizeof( to->headBlinkTime ) );
			to->headBlinkTime = from->headBlinkTime;
		}
	}

	//Frowning/Blink Frowning
	if ( from->headSkinFrown ) {
		to->headSkinFrown = from->headSkinFrown;

		if ( from->headSkinFrownBlink )
			to->headSkinFrownBlink = from->headSkinFrownBlink;
	}

	//Copy over bolton info
	/*if ( from->boltonTags[0].tagModel && from->boltonTags[0].tagName ) { //if there actually is bolton data...
		for (i = 0; i < MAX_BOLTONS; i++ ) { //loop thru all of them
			if ( from->boltonTags[i].tagModel && from->boltonTags[i].tagName ) { //only work on ones that actually have data.
				to->boltonTags[i].modelBase = from->boltonTags[i].modelBase;
				to->boltonTags[i].tagModel = from->boltonTags[i].tagModel;
				Q_strncpyz( to->boltonTags[i].tagName, from->boltonTags[i].tagName, sizeof (to->boltonTags[i].tagName) );*/
				memcpy( to->boltonTags, from->boltonTags, sizeof(to->boltonTags) );
//			}
//		}
//	}

	//Talking skin data
	/*if ( from->headSkinTalk[0] ) {
		for (i = 0; i < MAX_TALK_SKINS; i++ ) {
			if ( from->headSkinTalk[i] ) {*/
				memcpy( to->headSkinTalk, from->headSkinTalk, sizeof( to->headSkinTalk ) );
	/*		}
		}
	}*/

	to->holsterModel = from->holsterModel;

	Q_strncpyz( to->soundPath, from->soundPath, sizeof (to->soundPath) );
	//memcpy( to->animations, from->animations, sizeof( to->animations ) );
	//TiM : New animation method :)
	to->animIndex = from->animIndex;
	memcpy( to->sounds, from->sounds, sizeof( to->sounds ) );

	to->isHazardModel = from->isHazardModel;
}

/*
======================
CG_ScanForExistingClientInfo
======================
*/
static qboolean CG_ScanForExistingClientInfo( clientInfo_t *ci ) {
	int		i;
	clientInfo_t	*match;

	for ( i = 0 ; i < cgs.maxclients ; i++ ) {
		match = &cgs.clientinfo[ i ];
		if ( !match->infoValid ) {
			continue;
		}
		if ( match->deferred ) {
			continue;
		}
		if ( !Q_stricmp(ci->charName, match->charName) && !Q_stricmp( ci->modelName, match->modelName )
			&& !Q_stricmp( ci->skinName, match->skinName ) ) {
			// this clientinfo is identical, so use it's handles

			ci->deferred = qfalse;

			CG_CopyClientInfoModel( match, ci );

			return qtrue;
		}
	}

	// nothing matches, so defer the load
	return qfalse;
}

/*
======================
CG_SetDeferredClientInfo

We aren't going to load it now, so grab some other
client's info to use until we have some spare time.
======================
*/
static void CG_SetDeferredClientInfo( clientInfo_t *ci, int clientNum ) {
	int		i;
	clientInfo_t	*match;

	// if we are in teamplay, only grab a model if the skin is correct
	if ( cgs.gametype >= GT_TEAM ) {
		// this is ONLY for optimization - it's exactly the same effect as CG_LoadClientInfo
		for ( i = 0 ; i < cgs.maxclients ; i++ ) {
			match = &cgs.clientinfo[ i ];
			if ( !match->infoValid ) {
				continue;
			}
			if ( Q_stricmp( ci->skinName, match->skinName ) ) {
				continue;
			}
			ci->deferred = qtrue;
			CG_CopyClientInfoModel( match, ci );
			return;
		}

		// load the full model, because we don't ever want to show
		// an improper team skin.  This will cause a hitch for the first
		// player, when the second enters.  Combat shouldn't be going on
		// yet, so it shouldn't matter
		CG_LoadClientInfo( ci, clientNum );
		return;
	}

	// find the first valid clientinfo and grab its stuff
	for ( i = 0 ; i < cgs.maxclients ; i++ ) {
		match = &cgs.clientinfo[ i ];
		if ( !match->infoValid ) {
			continue;
		}

		ci->deferred = qtrue;
		CG_CopyClientInfoModel( match, ci );
		return;
	}

	// we should never get here...
	CG_Printf( "CG_SetDeferredClientInfo: no valid clients!\n" );

	CG_LoadClientInfo( ci ,clientNum);
}


/*
======================
CG_NewClientInfo
======================
*/
void CG_NewClientInfo( int clientNum ) {
	clientInfo_t *ci;
	clientInfo_t newInfo;
	const char	*configstring;
	const char	*v;
	char		*model = NULL;
	char		*skin = NULL;
	size_t		len;
	//int			i;

	ci = &cgs.clientinfo[clientNum];

	configstring = CG_ConfigString( clientNum + CS_PLAYERS );
	if ( !configstring[0] ) {
		memset( ci, 0, sizeof( *ci ) );
		return;		// player just left
	}

	// build into a temp buffer so the defer checks can use
	// the old value
	memset( &newInfo, 0, sizeof( newInfo ) );

	// isolate the player's name
	v = Info_ValueForKey(configstring, "n");
	Q_strncpyz( newInfo.name, v, sizeof( newInfo.name ) );

	// colors
	//v = Info_ValueForKey( configstring, "c1" );
	//CG_ColorFromString( v, newInfo.color );

	// bot skill
	v = Info_ValueForKey( configstring, "skill" );
	newInfo.botSkill = atoi( v );

	// handicap
	v = Info_ValueForKey( configstring, "hc" );
	newInfo.handicap = atoi( v );

	// wins
	v = Info_ValueForKey( configstring, "w" );
	newInfo.wins = atoi( v );

	// losses
	v = Info_ValueForKey( configstring, "l" );
	newInfo.losses = atoi( v );

	// team
	v = Info_ValueForKey( configstring, "t" );
	newInfo.team = atoi( v );

	// playerclass
	v = Info_ValueForKey( configstring, "p" );
	newInfo.pClass = atoi( v );

	// model
	v = Info_ValueForKey( configstring, "model" );
	if ( cg_forceModel.integer ) {
		// forcemodel makes everyone use a single model
		// to prevent load hitches
		char charStr[MAX_QPATH];
		char *model;
		char *skin;
		size_t len;

		trap_Cvar_VariableStringBuffer( "model", charStr, sizeof( charStr ) );
		if ( ( model = strchr( charStr, '/' ) ) == NULL) {
			model = "main";
			skin = "default";
		} else {
			*model = 0; //*model++ = 0;
			len = strlen(model);

			//if there was a slash, but no model afterwards
			if ( !model || !model[1] ) {
				model = "main";
			}

			if ( ( skin = strchr( model, '/' ) ) == NULL ) {
				skin = "default";
			} else {
				*skin = 0; //*skin++ = 0;

				if ( !skin || !skin[1] ) {
					skin = "default";
				}

				Com_sprintf( model, len - strlen(skin), model);
			}
		}

		Q_strncpyz( newInfo.skinName, skin, sizeof( newInfo.skinName ) );
		Q_strncpyz( newInfo.modelName, model, sizeof( newInfo.modelName ) );
		Q_strncpyz( newInfo.charName, charStr, sizeof( newInfo.charName ) );

//		Q_strncpyz( newInfo.modelName, DEFAULT_MODEL, sizeof( newInfo.modelName ) );
//		Q_strncpyz( newInfo.skinName, "default", sizeof( newInfo.skinName ) );

		if ( cgs.gametype >= GT_TEAM ) {
			// keep skin name
			skin = strchr( v, '/' );
			if ( model ) {
				Q_strncpyz( newInfo.skinName, skin + 1, sizeof( newInfo.skinName ) );
			}
		}
	} else {
		//Q_strncpyz( newInfo.modelName, v, sizeof( newInfo.modelName ) );
		//Okay! Here we go! We gotta take a string like kulhane/admiral/teal
		//divide it, and put it into three different vars, accounting for user n00biness
		//(ie mistakes and stuff) along the way.
		
		//step 1, take the first bit of the string and put it in the charName var.
		if ( ( model = strchr( v, '/') ) == NULL ) { //if there's no slash
			Q_strncpyz( newInfo.charName, v, sizeof( newInfo.charName ) ); //just set it
		} else { //otherwise, isolate the first bit, and copy that
			len = strlen( v );
			Q_strncpyz( newInfo.charName, v, ((int)len - (int)strlen(model)) + 1 );
		}

		//Com_Printf("%s\n", newInfo.charName);

		//slash = strchr( newInfo.modelName, '/' );
		if ( !model || !model[1] ) {
			// modelName didn not include a skin name
			//Q_strncpyz( newInfo.skinName, "default", sizeof( newInfo.skinName ) );
			Q_strncpyz( newInfo.modelName, "main", sizeof( newInfo.modelName ) );
			Q_strncpyz( newInfo.skinName, "default", sizeof( newInfo.skinName ) );

			if ( model && !model[1] )
			{//if we had a slash, but nothing after, clear it
				*model = 0;
			}
		} else {
			model++; //bypass the slash //QVMNOTE
			len = strlen(model);
			skin = strchr( model, '/' );

			//if there was a model defined, but no skin
			if ( !skin || !skin[1] ) {
				//no skin, but I'm guessing we gotz a model at least
				if ( !skin ) {
					Q_strncpyz( newInfo.modelName, model, sizeof( newInfo.modelName ) );
				}
				else {
					if ( !skin[1] ) {
						Q_strncpyz( newInfo.modelName, model, (int)strlen(model) );
					}
				}

				Q_strncpyz( newInfo.skinName, "default", sizeof( newInfo.skinName ) );

				if ( skin && !skin[1] ) {
					*skin = 0;
				}
			} else {
				skin++; //QVMNOTE
				Q_strncpyz( newInfo.modelName, model, ((int)len - (int)strlen(skin)) );
				Q_strncpyz( newInfo.skinName, skin, sizeof( newInfo.skinName ) );
			}

			//Q_strncpyz( newInfo.skinName, slash + 1, sizeof( newInfo.skinName ) );
			// truncate modelName
			*model = 0;
		}
	
	}

	//TiM: PMS - age
	v = Info_ValueForKey( configstring, "age" );
	Q_strncpyz( newInfo.age, v, sizeof(newInfo.age) );

	//PMS - height
	v = Info_ValueForKey( configstring, "height" );
	newInfo.height = atof( v );

	//PMS - weight
	v = Info_ValueForKey( configstring, "weight" );
	newInfo.weight = atof( v );

	//PMS - race
	v = Info_ValueForKey( configstring, "race" );
	Q_strncpyz( newInfo.race, v, sizeof(newInfo.race) );

	//TiM : Offset for the emote system and solid chairs
	v = Info_ValueForKey( configstring, "of" );
	newInfo.modelOffset = atoi( v );
	//CG_Printf( "Set modeloffset as: %f\n", newInfo.modelOffset );

	v = Info_ValueForKey( configstring, "admin" );
	newInfo.isAdmin = atoi( v );

	//ensure the health value is carried over
	//it normally only gets updated when it itself is changed
	//newInfo.health = ci->health;
	//Actually... this might actually screw it up on server start time

	// scan for an existing clientinfo that matches this modelname
	// so we can avoid loading checks if possible
	if ( !CG_ScanForExistingClientInfo( &newInfo ) ) {
		qboolean	forceDefer;

		forceDefer = trap_MemoryRemaining() < 2000000;

		// if we are defering loads, just have it pick the first valid
		if ( forceDefer || 
			( cg_deferPlayers.integer && !cg_buildScript.integer && !cg.loading && 
			((clientNum != cg.predictedPlayerState.clientNum) && cg.validPPS) ) ) {
			// keep whatever they had if it won't violate team skins
			if ( ci->infoValid && 
				( cgs.gametype < GT_TEAM || !Q_stricmp( newInfo.skinName, ci->skinName ) ) ) {
				CG_CopyClientInfoModel( ci, &newInfo );
				newInfo.deferred = qtrue;
			} else {
				// use whatever is available
				CG_SetDeferredClientInfo( &newInfo, clientNum );
			}
			// if we are low on memory, leave them with this model
			if ( forceDefer ) {
				CG_Printf( "Memory is low.  Using deferred model.\n" );
				newInfo.deferred = qfalse;
			}
		} else {
			CG_LoadClientInfo( &newInfo, clientNum );
		}
	}

	// replace whatever was there with the new one
	newInfo.infoValid = qtrue;
	*ci = newInfo;
}


/*
======================
CG_LoadDeferredPlayers

Called each frame when a player is dead
and the scoreboard is up
so deferred players can be loaded
======================
*/
void CG_LoadDeferredPlayers( void ) {
	int		i;
	clientInfo_t	*ci;

	// scan for a deferred player to load
	for ( i = 0, ci = cgs.clientinfo ; i < cgs.maxclients ; i++, ci++ ) {
		if ( ci->infoValid && ci->deferred ) {
			// if we are low on memory, leave it deferred
			if ( trap_MemoryRemaining() < 4000000 ) {
				CG_Printf( "Memory is low.  Using deferred model.\n" );
				ci->deferred = qfalse;
				continue;
			}
			CG_LoadClientInfo( ci, i );
//			break;
		}
	}
}

/*
======================
CG_NewDecoyInfo

TiM: A decoy was spawned,
so the relevant data will
be set up so it may be displayed
independantly of its spawner player.
======================
*/
void CG_NewDecoyInfo( int decoyNum ) {
	clientInfo_t	*ci;
	char			*userinfo;
	int				i;
	char			*v;
	char			*temp, *temp2;
	//char			charName[MAX_QPATH], modelName[MAX_QPATH], skinName[MAX_QPATH];
	int				len;
	qboolean		noMemoryLeft=qfalse;

	ci = &cgs.decoyInfo[decoyNum];
	noMemoryLeft = ( trap_MemoryRemaining() < 4000000 );

	//First, check if force player models is on.  if so, copy all the data from us to the decoy.
	//Or, if we're low on memory, let's do this anyway
	if ( cg_forceModel.integer || noMemoryLeft ) {
		*ci = cgs.clientinfo[cg.predictedPlayerState.clientNum];
		
		if ( noMemoryLeft )
			CG_Printf( S_COLOR_RED "WARNING: Very little memory remains.  Decoy data is being deferred to player's active data.\n" );
		
		ci->infoValid = qtrue;
		return;
	}

	//CG_Printf( S_COLOR_RED "decoy ID: %i\n", decoyNum );

	//Get the necessary decoy data
	userinfo = (char *)CG_ConfigString( CS_DECOYS + decoyNum );
	
	if ( !userinfo || !userinfo[0] ) { //No data, so flush and continue
		memset( ci, 0, sizeof( clientInfo_t ) );
		return;
	}

	//CG_Printf( S_COLOR_RED "%s\n", userinfo );

	//Get model string
	v = Info_ValueForKey( userinfo, "model" );

	//First thing's first.  We need to isolate these into three strings: model, char, skin
	{
		//step 1, take the first bit of the string and put it in the charName var.
		if ( ( temp = strchr( v, '/') ) == NULL ) { //if there's no slash
			Q_strncpyz( ci->charName, v, sizeof( ci->charName ) ); //just set it
		} else { //otherwise, isolate the first bit, and copy that
			len = strlen( v );
			Q_strncpyz( ci->charName, v, ((int)len - (int)strlen(temp)) + 1 );
		}

		//Com_Printf("%s\n", newInfo.charName);

		//slash = strchr( newInfo.modelName, '/' );
		if ( !temp || !temp[1] ) {
			// modelName did not include a skin name
			//Q_strncpyz( newInfo.skinName, "default", sizeof( newInfo.skinName ) );
			Q_strncpyz( ci->modelName, "main", sizeof( ci->modelName ) );
			Q_strncpyz( ci->skinName, "default", sizeof( ci->skinName ) );

			if ( temp && !temp[1] )
			{//if we had a slash, but nothing after, clear it
				*temp = 0;
			}
		} else {
			temp++; //bypass the slash
			len = strlen(temp);
			temp2 = strchr( temp, '/' );

			//if there was a model defined, but no skin
			if ( !temp2 || !temp2[1] ) {
				//no skin, but I'm guessing we gotz a model at least
				if ( !temp2 ) {
					Q_strncpyz( ci->modelName, temp, sizeof( ci->modelName ) );
				}
				else {
					if ( !temp2[1] ) {
						Q_strncpyz( ci->modelName, temp, (int)strlen(temp) );
					}
				}

				Q_strncpyz( ci->skinName, "default", sizeof( ci->skinName ) );

				if ( temp2 && !temp2[1] ) {
					*temp2 = 0;
				}
			} else {
				temp2++;
				Q_strncpyz( ci->modelName, temp, ((int)len - (int)strlen(temp2)) );
				Q_strncpyz( ci->skinName, temp2, sizeof( ci->skinName ) );
			}
		}
	}

	//k... get the additional parms from the config string n' put em here
	v = Info_ValueForKey( userinfo, "weight" );
	ci->weight	= atof( v );
	v = Info_ValueForKey( userinfo, "height" );
	ci->height	= atof( v );
	v = Info_ValueForKey( userinfo, "moOf" );
	ci->modelOffset = atoi( v );
	v = Info_ValueForKey( userinfo, "c" );
	ci->pClass = atoi( v );

	ci->animsFlushed = qtrue;

	//Okay... if another player actively has the skin we want, let's pilfer that rather than load it like a schmuck rofl.
	{
		clientInfo_t	*match;

		for ( i = 0; i < cgs.maxclients; i++ ) {
			match = &cgs.clientinfo[i];

			//We found a match! ^_^
			if ( !Q_stricmp( match->charName, ci->charName ) && 
				 !Q_stricmp( match->modelName, ci->modelName ) &&
				 !Q_stricmp( match->skinName, ci->skinName ) )
			{
				CG_CopyClientInfoModel( match, ci );
				ci->infoValid = qtrue;
				return;
			}
		}
	}


	//*sigh... guess worse came to worse... we gotta fricken load it. :'(
	if ( !CG_ParseModelDataFile( ci, ci->charName, ci->modelName, ci->skinName ) ) 
	{
		CG_Printf( S_COLOR_RED "ERROR: Unable to load character for decoy: %s/%s/%s\n", ci->charName, ci->modelName, ci->skinName );
		
		if ( !CG_ParseModelDataFile( ci, ci->charName, DEFAULT_MODEL, DEFAULT_SKIN ) )
		{
			if (!CG_ParseModelDataFile( ci, cg_defaultChar.string, ci->modelName, ci->skinName ) )
			{
				if ( !CG_ParseModelDataFile( ci, cg_defaultChar.string, DEFAULT_MODEL, DEFAULT_SKIN ) )
				{
					//if we hit this.... oh so bad... O_o
					if ( !CG_ParseModelDataFile( ci, DEFAULT_CHAR, DEFAULT_MODEL, DEFAULT_SKIN ) )
						CG_Error( "DEFAULT_CHAR / model / skin ( %s/%s/%s ) failed to register", DEFAULT_CHAR, DEFAULT_MODEL, DEFAULT_SKIN );
				}
			}
		}
	}

	ci->infoValid = qtrue;
}

/*
=============================================================================

PLAYER ANIMATION

=============================================================================
*/


/*
===============
CG_SetLerpFrameAnimation

may include ANIM_TOGGLEBIT
===============
*/
static void CG_SetLerpFrameAnimation( clientInfo_t *ci, lerpFrame_t *lf, int newAnimation ) {
	animation_t	*anim;

	lf->animationNumber = newAnimation;
	newAnimation &= ~ANIM_TOGGLEBIT;

	if ( newAnimation < 0 || newAnimation >= MAX_ANIMATIONS ) {
		CG_Error( "Bad animation number: %i", newAnimation );
	}

	//CG_Printf("animIndex: %i\n", ci->animIndex );
	//anim = &ci->animations[ newAnimation ];
	anim = &cg_animsList[ ci->animIndex ].animations[ newAnimation ];
	//CG_Printf(S_COLOR_RED "%i\n", ci->animIndex );

	lf->animation = anim;
	lf->animationTime = lf->frameTime + anim->initialLerp;

	if ( cg_debugAnim.integer ) {
		CG_Printf( "Anim: %s (%i)\n", GetStringForID(animTable, newAnimation), newAnimation );
		//CG_Printf( "Anim: %i\n", newAnimation );
	}
}

/*
===============
CG_RunLerpFrame

Sets cg.snap, cg.oldFrame, and cg.backlerp
cg.time should be between oldFrameTime and frameTime after exit
===============
*/
static qboolean CG_RunLerpFrame( clientInfo_t *ci, lerpFrame_t *lf, int newAnimation, float speedScale ) {
	int			f;
	animation_t	*anim;
	qboolean	newFrame = qfalse;
	float		frameLerp;

	// debugging tool to get no animations
	if ( cg_animSpeed.integer == 0 ) {
		lf->oldFrame = lf->frame = lf->backlerp = 0;
		return qfalse;
	}

	// see if the animation sequence is switching
	if ( newAnimation != lf->animationNumber || !lf->animation ) {
		CG_SetLerpFrameAnimation( ci, lf, newAnimation );
	}

	// if we have passed the current frame, move it to
	// oldFrame and calculate a new frame
	if ( cg.time >= lf->frameTime ) {
		lf->oldFrame = lf->frame;
		lf->oldFrameTime = lf->frameTime;

		// get the next frame based on the animation
		anim = lf->animation;
		if ( !anim || !anim->frameLerp ) {
			return qfalse;		// shouldn't happen
		}
		//RPG-X Check.  Anims with lengths < 0 are emote stubs.
		//If we get one, hardcode to override to default standing.
		//Otherwise, we'll get complicated glitches.
		if ( anim->numFrames < 0 ) {
			CG_SetLerpFrameAnimation( ci, lf, BOTH_STAND1 );
		}

		//TiM - Calc frame lerp scale here, else the frames
		//just snap to each other
		frameLerp = (float)anim->frameLerp + (anim->frameLerp*(1.0f - speedScale));
		if ( frameLerp < 1.0f )
			frameLerp = 1.0f;

		//CG_Printf( "Lerp: %f\n", frameLerp );

		if ( cg.time < lf->animationTime ) {
			lf->frameTime = lf->animationTime;		// initial lerp
		} else {
			lf->frameTime = lf->oldFrameTime + frameLerp;//anim->frameLerp;
		}
		f = ( lf->frameTime - lf->animationTime ) / frameLerp;//anim->frameLerp;
		//f *= speedScale;		// adjust for haste, etc
		if ( f >= anim->numFrames ) {
			f -= anim->numFrames;
			if ( anim->loopFrames ) {
				f %= anim->loopFrames;
				f += anim->numFrames - anim->loopFrames;
			} else {
				f = anim->numFrames - 1;
				// the animation is stuck at the end, so it
				// can immediately transition to another sequence
				lf->frameTime = cg.time;
			}
		}
		lf->frame = anim->firstFrame + f;
		if ( cg.time > lf->frameTime ) {
			lf->frameTime = cg.time;
			if ( cg_debugAnim.integer ) {
				CG_Printf( "Clamp lf->frameTime\n");
			}
		}
		newFrame = qtrue;
	}

	if ( lf->frameTime > cg.time + 200 ) {
		lf->frameTime = cg.time;
	}

	if ( lf->oldFrameTime > cg.time ) {
		lf->oldFrameTime = cg.time;
	}
	// calculate current lerp value
	if ( lf->frameTime == lf->oldFrameTime ) {
		lf->backlerp = 0;
	} else {
		lf->backlerp = 1.0 - (float)( cg.time - lf->oldFrameTime ) / ( lf->frameTime - lf->oldFrameTime );
	}

	return newFrame;
}


/*
===============
CG_ClearLerpFrame
===============
*/
//This function has been rpg-x'ed®! (by RedTechie)
void CG_ClearLerpFrame( clientInfo_t *ci, lerpFrame_t *lf, int animationNumber ) { //RPG-X: RedTechie - Changed type from       static void      to     void
	
	if(!lf) return;

	lf->frameTime = lf->oldFrameTime = cg.time; 
	lf->animation = 0;
	CG_SetLerpFrameAnimation( ci, lf, animationNumber );
	lf->oldFrame = lf->frame = lf->animation->firstFrame;

	//RPG-X: RedTechie - Added this block of code
	/*if ( lf->animation->frameLerp < 0 )
	{//Plays backwards
		lf->oldFrame = lf->frame = (lf->animation->firstFrame + lf->animation->numFrames);
	}
	else
	{
		lf->oldFrame = lf->frame = lf->animation->firstFrame;
	}*/
	
	//TiM - Put in, for now
	
}


/*
===============
CG_PlayerAnimation
===============
*/
extern qboolean PM_PlayerWalking( int anim );
extern qboolean PM_PlayerRunning( int anim );
extern qboolean PM_PlayerCrouchWalking( int anim );

static void CG_PlayerAnimation( centity_t *cent, int *legsOld, int *legs, float *legsBackLerp,
						int *torsoOld, int *torso, float *torsoBackLerp ) {
	clientInfo_t	*ci;
	int				clientNum;
	float			speedScale=1;
	qboolean		newLegsFrame = qfalse;
	qboolean		newTorsoFrame = qfalse;
	//float			speed;
	qboolean		isDecoy = cent->currentState.eFlags & EF_ITEMPLACEHOLDER;

	clientNum = cent->currentState.clientNum;

	if ( cg_noPlayerAnims.integer ) {
		*legsOld = *legs = *torsoOld = *torso = 0;
		return;
	}

	//if ( cent->currentState.powerups & ( 1 << PW_HASTE ) ) {
	//	speedScale = 1.5;
	//} else {
	//	speedScale = 1;
	//}
	//CG_Printf( "Vel: %f\n", VectorLength(cent->currentState.pos.trDelta));
	
	//TiM
	// 250 = default running speed
	// 125 = default walking speed
	// 90 = default crouchwalk speed
	//if ( !isDecoy )
	//{
	//	if ( clientNum == cg.snap->ps.clientNum )
	//		speed = VectorLength( cg.predictedPlayerState.velocity );
	//	else
	//		speed = VectorLength(cent->currentState.pos.trDelta);
	//	//if ( speed < 50.0f )
	//	//speed = 50.0f;

	//	if ( PM_PlayerWalking( cent->currentState.legsAnim ) )
	//	{
	//		speedScale = speed / 125.0f;
	//	}
	//	else if ( PM_PlayerRunning( cent->currentState.legsAnim ) )
	//	{
	//		speedScale = speed / 250.0f;
	//	}
	//	else if ( PM_PlayerCrouchWalking( cent->currentState.legsAnim ) )
	//	{
	//		speedScale = speed / 90.0f;
	//	}
	//	else
	//	{
	//		speedScale = 1.0f;
	//	}
	//}


	if ( isDecoy ) 
		ci = &cgs.decoyInfo[ cent->currentState.eventParm ];
	else
		ci = &cgs.clientinfo[ clientNum ];

	// do the shuffle turn frames locally
	if ( !cent->clampAngles && cent->pe.legs.yawing && ( cent->currentState.legsAnim & ~ANIM_TOGGLEBIT ) == BOTH_STAND1 ) { //TORSO_STAND
		newLegsFrame = CG_RunLerpFrame( ci, &cent->pe.legs, LEGS_TURN1, speedScale );
	} else {
		newLegsFrame = CG_RunLerpFrame( ci, &cent->pe.legs, cent->currentState.legsAnim, speedScale );
	}

	if( newLegsFrame && ci->animSndIndex >= 0 && !(cent->currentState.powerups & ( 1 << PW_INVIS ) ) )
	{
		trace_t	tr;
		vec3_t	endPoint;
		//qboolean metal = qfalse; //Uberhack meant specifically for metal clank surfaces
		vec3_t	mins = { -16, -16, 0 };
		vec3_t	maxs = { 16, 16, 0 };
		int surfType;

		//TiM: Lower based sounds are always to do with things like shoes clopping n' stuff.
		//This portion of code makes sure the player is on a solid surface in order to play this sound
		VectorCopy( cent->lerpOrigin, endPoint);
		endPoint[2] -= 24.50f;
		CG_Trace( &tr, cent->lerpOrigin, mins, maxs, endPoint, cent->currentState.clientNum, MASK_PLAYERSOLID | CONTENTS_LADDER );
		//trap_CM_BoxTrace( &tr, cent->lerpOrigin, endPoint, mins, maxs, 0, MASK_PLAYERSOLID );
		//metal = tr.surfaceFlags & SURF_METALSTEPS || (tr.contents & CONTENTS_LADDER);
		if(tr.surfaceFlags & SURF_METALSTEPS || (tr.contents & CONTENTS_LADDER))
			surfType = 1;
		else if(tr.surfaceFlags & SURF_GRASS)
			surfType = 2;
		else if(tr.surfaceFlags & SURF_GRAVEL)
			surfType = 3;
		else if(tr.surfaceFlags & SURF_SNOW)
			surfType = 4;
		else if(tr.surfaceFlags & SURF_WOOD)
			surfType = 5;
		else
			surfType = 0;

		//if there's something below us, or we're free floating in something like water/lava/slime
		if ( tr.fraction != 1.0f || (tr.contents & MASK_WATER ) || (tr.contents & CONTENTS_LADDER) ) {
			CG_PlayerAnimSounds( cg_animsSndList[ci->animSndIndex].lowerAnimSounds, cent->pe.legs.frame, cent->currentState.clientNum, /*metal*/surfType );
		}	
	}

	*legsOld = cent->pe.legs.oldFrame;
	*legs = cent->pe.legs.frame;
	*legsBackLerp = cent->pe.legs.backlerp;

	//if ( PM_PlayerWalking( cent->currentState.torsoAnim ) )
	//{
	//	speedScale *= speed / 125.0f;
	//}
	//else if ( PM_PlayerRunning( cent->currentState.torsoAnim ) )
	//{
	//	speedScale *= speed / 250.0f;
	//}
	//else if ( PM_PlayerCrouchWalking( cent->currentState.torsoAnim  ) )
	//{
	//	speedScale *= speed / 90.0f;
	//}
	//else
	//{
	//	speedScale = 1.0f;
	//}

	newTorsoFrame = CG_RunLerpFrame( ci, &cent->pe.torso, cent->currentState.torsoAnim, speedScale );

	if( newTorsoFrame && ci->animSndIndex >= 0 )
	{
		CG_PlayerAnimSounds( cg_animsSndList[ci->animSndIndex].upperAnimSounds, cent->pe.torso.frame, cent->currentState.clientNum, /*qfalse*/0 );
	}

	*torsoOld = cent->pe.torso.oldFrame;
	*torso = cent->pe.torso.frame;
	*torsoBackLerp = cent->pe.torso.backlerp;
}

/*
=============================================================================

PLAYER ANGLES

=============================================================================
*/

/*
==================
CG_SwingAngles
==================
*/
static void CG_SwingAngles( float destination, float swingTolerance, float clampTolerance,
					float speed, float *angle, qboolean *swinging, qboolean pitch ) {
	float	swing;
	float	move;
	float	scale;

	swing = AngleSubtract( destination, *angle  );

	/*if ( canSwing ) {
		if ( swing == 0 ) {
			*swinging = qfalse;
		} else if ( swing >= clampTolerance || swing <= -clampTolerance ) {
			*swinging = qtrue;
		} else {
			*swinging = qtrue;
		}
	} 
	else {*/
		if ( !*swinging ) {
			// see if a swing should be started
			//swing = AngleSubtract( *angle, destination );

			if ( swing == 0 ) {
				*swinging = qfalse;
			} 

			else if ( !pitch && ( swing >= clampTolerance || swing <= -clampTolerance ) ) {
				*swinging = qtrue;
			}

			if ( pitch )
				*swinging = qtrue;

			/*else if ( swing > swingTolerance || swing < -swingTolerance ) {
				*swinging = qtrue;
			}*/
		}
	//}

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
		move = cg.frametime * scale * speed;
		if ( move >= swing ) {
			move = swing;
			*swinging = qfalse;
		}
		*angle = AngleMod( *angle + move );
	} else if ( swing < 0 ) {
		move = cg.frametime * scale * -speed;
		if ( move <= swing ) {
			move = swing;
			*swinging = qfalse;
		}
		*angle = AngleMod( *angle + move );
	}

	// clamp to no more than tolerance
	swing = AngleSubtract( destination, *angle );
	if ( swing > clampTolerance ) 
	{
		*angle = AngleMod( destination - (clampTolerance - 1) ); //clampTolerance
	} 
	else if ( swing < -clampTolerance ) 
	{
		*angle = AngleMod( destination + (clampTolerance - 1) ); //clampTolerance
	}
}

/*
=================
CG_AddPainTwitch
=================
*/
static void CG_AddPainTwitch( centity_t *cent, vec3_t torsoAngles ) {
	int		t;
	float	f;

	t = cg.time - cent->pe.painTime;
	if ( t >= PAIN_TWITCH_TIME ) {
		return;
	}

	f = 1.0 - (float)t / PAIN_TWITCH_TIME;

	if ( cent->pe.painDirection ) {
		torsoAngles[ROLL] += 20 * f;
	} else {
		torsoAngles[ROLL] -= 20 * f;
	}
}


/*
===============
CG_PlayerAngles

Handles seperate torso motion

  legs pivot based on direction of movement

  head always looks exactly at cent->lerpAngles

  if motion < 20 degrees, show in head only
  if < 45 degrees, also show in torso
===============
*/

//static float yawClamped;
//static float headClamp;

#define YAW_DELTA		100 //max yaw a head can turn around without looking like an exorcist spoof ;P  
#define PITCH_DELTA		35  //max pitch a head can tilt before looking like the player sepearated their neck O_o

extern qboolean PM_PlayerIdling ( int torsoAnim, int legsAnim );

static void CG_PlayerAngles( centity_t *cent, vec3_t legs[3], vec3_t torso[3], vec3_t head[3] ) {
	vec3_t		legsAngles, torsoAngles, headAngles;
	float		dest;
	//float		delta;
	static	int	movementOffsets[8] = { 0, 22, 45, -22, 0, 22, -45, -22 }; //{ 0, 22, 45, -22, 0, 22, -45, -22 };
	vec3_t		velocity;
	float		speed;
	int			dir;
	qboolean	offsetPitch;
	clientInfo_t* ci;
	int			i;

	qboolean	LockBodyYaw=qfalse; //RPG-X:TiM

	if ( cent->currentState.eFlags & EF_ITEMPLACEHOLDER )
		ci = &cgs.decoyInfo[cent->currentState.eventParm];
	else
		ci = &cgs.clientinfo[cent->currentState.clientNum];

	if ( cent->currentState.eFlags & EF_CLAMP_ALL ) {

		VectorSet( headAngles, 0, cent->pe.legs.yawAngle, 0 );
		VectorSet( torsoAngles, 0, cent->pe.legs.yawAngle, 0 );
		VectorSet( legsAngles, 0, cent->pe.legs.yawAngle, 0 );

		AnglesSubtract( headAngles, torsoAngles, headAngles );
		AnglesSubtract( torsoAngles, legsAngles, torsoAngles );
		AnglesToAxis( legsAngles, legs );
		AnglesToAxis( torsoAngles, torso );
		AnglesToAxis( headAngles, head );
		return;
	}

	if( /*( cent->currentState.legsAnim & ~ANIM_TOGGLEBIT ) != BOTH_LADDER_DWN1  
			&& ( cent->currentState.legsAnim & ~ANIM_TOGGLEBIT ) != BOTH_LADDER_UP1 
			&& ( cent->currentState.legsAnim & ~ANIM_TOGGLEBIT ) != BOTH_LADDER_IDLE 
			&&*/ !( cent->currentState.eFlags & EF_CLAMP_BODY ) ) 
	{
		LockBodyYaw = qfalse;
	}
	else { 
		LockBodyYaw = qtrue; 

		cent->pe.torso.pitchAngle = 0;
	}
	//}
	
	VectorCopy( cent->lerpAngles, headAngles );
	VectorClear( legsAngles );
	VectorClear( torsoAngles );		
	headAngles[YAW] = AngleMod( headAngles[YAW] );
	//headAngles[PITCH] = AngleMod( headAngles[PITCH] );

	/*if ( LockBodyYaw && yawClamped == 0.0 )
		yawClamped = headAngles[YAW];
	else if ( !LockBodyYaw ) {
		yawClamped = 0.0;
	}*/

	if ( LockBodyYaw ) {
		float deltaYaw;
		float turnRatio;
		float finalPitch;

		//calc ratio of delta from origin yaw to current yaw
		turnRatio = Q_fabs( AngleDelta( cent->pe.torso.yawAngle, headAngles[YAW] ) ) / 90.0f;
		if ( turnRatio > 1.0f )
			turnRatio = 1.0f;
		if ( turnRatio < 0.0f )
			turnRatio = 0.0f;

		finalPitch = (float)PITCH_DELTA - ( 10.0f * turnRatio );

		//CG_Printf( "Pitch Before: %f\n", headAngles[PITCH] );

		//handle head pitch
		//if players are looking down, clamp it so the more yaw there is, the higher they'll be looking.
		//reason being, most humans can't bury their face that far into their shoulders.
		if ( headAngles[PITCH] > finalPitch && headAngles[PITCH] > 0 ) //Looking down. weirdly enough
		{
			headAngles[PITCH] = finalPitch;
		}
		else if ( headAngles[PITCH] < -PITCH_DELTA )
		{
			//delta = headAngles[YAW] + AngleMod( cent->pe.legs.yawAngle + YAW_DELTA);
			//headAngles[YAW] = AngleMod(cent->pe.legs.yawAngle - YAW_DELTA) + delta;
			headAngles[PITCH] = -PITCH_DELTA;
		}

		//if head yaw is about to rip off the exorcist
		/*if ( ( headAngles[YAW] > AngleMod( cent->pe.legs.yawAngle + YAW_DELTA) ) 
		&& ( headAngles[YAW] < AngleMod( cent->pe.legs.yawAngle + 180.0f) ) ) 
		{
			//delta = Q_fabs(headAngles[YAW] - AngleMod( cent->pe.legs.yawAngle + YAW_DELTA));
			//headAngles[YAW] = AngleMod(cent->pe.legs.yawAngle + YAW_DELTA) - delta;
			headAngles[YAW] = AngleMod(cent->pe.legs.yawAngle + YAW_DELTA);
		}
		else if ( headAngles[YAW] < AngleMod( cent->pe.legs.yawAngle - YAW_DELTA) 
			&& ( headAngles[YAW] > AngleMod( cent->pe.legs.yawAngle + 180.0f) ) ) 
		{
			//delta = headAngles[YAW] + AngleMod( cent->pe.legs.yawAngle + YAW_DELTA);
			//headAngles[YAW] = AngleMod(cent->pe.legs.yawAngle - YAW_DELTA) + delta;
			headAngles[YAW] = AngleMod(cent->pe.legs.yawAngle - YAW_DELTA);
		}*/

		if ( Q_fabs( deltaYaw = AngleDelta( headAngles[YAW], cent->pe.torso.yawAngle ) ) > YAW_DELTA ) {
			if ( deltaYaw > 0 ) {
				headAngles[YAW] = AngleNormalize360 ( cent->pe.torso.yawAngle + YAW_DELTA );
			}
			else {
				headAngles[YAW] = AngleNormalize360 ( cent->pe.torso.yawAngle - YAW_DELTA );
			}
		}
	//CG_Printf( "Pitch After: %f\n", headAngles[PITCH] );
	}

	// --------- yaw -------------

	// allow yaw to drift a bit
	if ( !PM_PlayerIdling( cent->currentState.torsoAnim, cent->currentState.legsAnim ) 
		|| ( cg_liftEnts[cent->currentState.clientNum] > 0/*(cg.time - cgs.levelStartTime)*/ ) ) {
		// if not standing still, always point all in the same direction
		cent->pe.torso.yawing = qtrue;	// always center
		cent->pe.torso.pitching = qtrue;	// always center
		cent->pe.legs.yawing = qtrue;	// always center
		offsetPitch = qfalse;
	} 
	else {
		offsetPitch = qtrue;
	}

	// adjust legs for movement dir
	if ( cent->currentState.eFlags & EF_DEAD ) {
		// don't let dead bodies twitch
		dir = 0;
	} else {
		dir = cent->currentState.angles2[YAW];
		if ( dir < 0 || dir > 7 ) {
			CG_Error( "Bad player movement angle" );
		}
	}

	//RPG-X Ladder disables character yawing coz it's like they're spinning on air
	if( !LockBodyYaw ) 
	{
		legsAngles[YAW] = headAngles[YAW] ;
		torsoAngles[YAW] = headAngles[YAW] ;

		if ( !cg_liftEnts[cent->currentState.clientNum] )
		{
			legsAngles[YAW]	+= movementOffsets[ dir ];
			torsoAngles[YAW] += 0.25 * movementOffsets[ dir ];
		}

		CG_SwingAngles( torsoAngles[YAW], 25, 90, cg_swingSpeed.value, &cent->pe.torso.yawAngle, &cent->pe.torso.yawing, qfalse );
		CG_SwingAngles( legsAngles[YAW], 40, 90, cg_swingSpeed.value, &cent->pe.legs.yawAngle, &cent->pe.legs.yawing, qfalse );
	}

	//TiM - if turbolifting, rotate us, but then don't lerp
	if ( cg_liftEnts[cent->currentState.clientNum] > 0/*(cg.time - cgs.levelStartTime)*/ ) 
	{
		if ( !cent->pe.legs.yawing )
		{
			cent->clampAngles = qtrue;
		}
	}

	if ( cent->clampAngles ) {
		//torsoAngles[YAW] = headAngles[YAW];
		//legsAngles[YAW] = headAngles[YAW];
		cent->pe.torso.yawAngle = headAngles[YAW];
		cent->pe.legs.yawAngle = headAngles[YAW];
	}

	torsoAngles[YAW] = cent->pe.torso.yawAngle;
	legsAngles[YAW] = cent->pe.legs.yawAngle;

	if ( !cg_liftEnts[cent->currentState.clientNum] && cent->clampAngles > qfalse )
		cent->clampAngles = qfalse;

	// --------- pitch -------------

	//TiM : Add an offset so they don't lean as much when idling
	//Make it default elsewise tho
	// only show a fraction of the pitch angle in the torso

	//Com_Printf( "headPitch: %f\n", headAngles[PITCH] );
	if ( headAngles[PITCH] > 180 ) {
		dest = (-360 + headAngles[PITCH]) * (offsetPitch==qtrue ? 0.45 : 0.75); //(offsetPitch ? 0.95 : 0.75)
	} else {
		//if offsetPitch enabled (ie, we're in the ideal pose for the cool neck-only rotation)
		//this will make the actual torso pitch delay a tad until it's passed a threshold
		//of 30 degrees in either direction.  The overall aim of this is to try and minimize
		//the torso movement so it's more realistically subtle, instead of stupidly, physics defyingly
		//obvious (like being able to bend on a 90 degree ange >_< )
		if ( offsetPitch ) {
			if ( headAngles[PITCH] > 30 ) {
				dest = (headAngles[PITCH] - 30 ) * 0.60;
			} else if ( headAngles[PITCH] < -40 ) {
				dest = ( headAngles[PITCH] + 40 ) * 0.45;
			} else {
				dest = 0;
			}	
		}
		else {
			dest = headAngles[PITCH] * 0.75;
		}
	}

	//I had to lock down the pitch when dead.  The player's head was going thru the floor O_o
	if( !LockBodyYaw && !( cent->currentState.eFlags & EF_DEAD ) /*&& !( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson )*/ ) 
	{ //(offsetPitch ? 10 : 30)

		if (cent->currentState.eFlags & EF_FULL_ROTATE ) 
		{
			//CG_Printf("Lerp Detected\n");
			legsAngles[PITCH] = headAngles[PITCH];
			CG_SwingAngles( legsAngles[PITCH], 15, 30, 0.1, &cent->pe.legs.pitchAngle, &cent->pe.legs.pitching, qtrue );
			legsAngles[PITCH] = cent->pe.legs.pitchAngle;

			torsoAngles[PITCH] = headAngles[PITCH];
			CG_SwingAngles( torsoAngles[PITCH], 15, 30, 0.1, &cent->pe.torso.pitchAngle, &cent->pe.torso.pitching, qtrue );
			torsoAngles[PITCH] = cent->pe.torso.pitchAngle;
		}
		else 
		{
			CG_SwingAngles( dest, 15, 30, 0.1, &cent->pe.torso.pitchAngle, &cent->pe.torso.pitching, qtrue );
			torsoAngles[PITCH] = cent->pe.torso.pitchAngle;
		}
	}

	//Com_Printf("Bitwise operation >>: %i, Bitwise Operation &: %i\n", Q_log2(8192), 10 & 255 );
	//Com_Printf("Atof: %f\n", atof( "0.43284673   t" ) );

	// --------- talking ----------
	// ------ head rotation -------
	if ( cent->currentState.eFlags & EF_TALKING ) {
		ci->headDebounceTime = cg.time + 1000;
	}

	if (ci->headDebounceTime > cg.time /*&& !LockBodyYaw*/ ) {
		if ( cent->currentState.eFlags & EF_TALKING ) {
			if ( cg.time > ci->nextTalkAngle || (!ci->talkAngles[PITCH] && !ci->talkAngles[YAW] && !ci->talkAngles[ROLL]) ) {

				for ( i = 0; i < 3; i++ ) {
					ci->talkAngles[i] = flrandom( -4, 4 );
				}

				ci->talkDifferential = irandom( 200, 500 );
				ci->nextTalkAngle = cg.time + ci->talkDifferential;
			}

		} 
		else {
			if ( ci->talkAngles[0] != 1 && ci->talkAngles[1] != 1 && ci->talkAngles[2] != 1 ) {
				//VectorCopy(ci->talkAngles, cent->lerpAngles);
				ci->talkDifferential = 300;
				VectorSet(ci->talkAngles, 0, 0, 0 );
			}
		}
		//Com_Printf("Yaw Offset: %f, Yaw: %f, LerpYaw: %f\n", ci->talkAngles[YAW], headAngles[YAW], cent->lerpAngles[YAW]);

		//if ( (cent->pe.head.pitchAngle != 0.0 && cent->pe.head.yawAngle != 0.0 && cent->pe.head.yawAngle != 0.0)
		//	&& !VectorCompare( ci->talkAngles, vec3_origin) ) {

			CG_SwingAngles( ci->talkAngles[PITCH], 30, 30, ci->talkDifferential*0.00005, &cent->pe.head.pitchAngle, &cent->pe.head.pitching, qtrue );
			CG_SwingAngles( ci->talkAngles[YAW], 30, 30, ci->talkDifferential*0.00005, &cent->pe.head.yawAngle, &cent->pe.head.yawing, qtrue );
			CG_SwingAngles( ci->talkAngles[ROLL], 30, 30, ci->talkDifferential*0.00005, &cent->pe.head.rollAngle, &cent->pe.head.rolling, qtrue );

			headAngles[PITCH] = AngleMod(headAngles[PITCH] + cent->pe.head.pitchAngle);
			headAngles[YAW] = AngleMod(headAngles[YAW] + cent->pe.head.yawAngle);
			headAngles[ROLL] = AngleMod(headAngles[ROLL] + cent->pe.head.rollAngle);
		//}
	}

	// --------- roll -------------
	//TiM - After I reintegrated velocity into
	//the player state, this code randomly started working lol!

	// lean towards the direction of travel
	VectorCopy( cent->currentState.pos.trDelta, velocity );
	speed = VectorNormalize( velocity );
	if ( speed ) {
		vec3_t	axis[3];
		float	side;

		speed *= 0.04; //0.05 - TiM

		AnglesToAxis( legsAngles, axis );
		side = speed * DotProduct( velocity, axis[1] );
		legsAngles[ROLL] -= side;

		side = speed * DotProduct( velocity, axis[0] );
		legsAngles[PITCH] += side;
	}

	// pain twitch
	CG_AddPainTwitch( cent, torsoAngles );

	// pull the angles back out of the hierarchial chain
	AnglesSubtract( headAngles, torsoAngles, headAngles );
	AnglesSubtract( torsoAngles, legsAngles, torsoAngles );
	AnglesToAxis( legsAngles, legs );
	AnglesToAxis( torsoAngles, torso );
	AnglesToAxis( headAngles, head );
}


//==========================================================================

/*
===============
CG_HasteTrail
===============
*/
/*static void CG_HasteTrail( centity_t *cent ) {
	localEntity_t	*smoke;
	vec3_t			origin, pos2;
	int				anim;

	if ( cent->trailTime > cg.time ) {
		return;
	}
	anim = cent->pe.legs.animationNumber & ~ANIM_TOGGLEBIT;
	if ( anim != BOTH_RUN1 && anim != BOTH_RUN1 ) { //LEGS_RUN
		return;
	}

	cent->trailTime += 100;
	if ( cent->trailTime < cg.time ) {
		cent->trailTime = cg.time;
	}

	VectorCopy( cent->lerpOrigin, origin );
	origin[0] += flrandom(-5,5);
	origin[1] += flrandom(-5,5);
	origin[2] -= 15;

	AngleVectors(cent->lerpAngles, pos2, NULL, NULL);
	pos2[2]=0;
	VectorMA(origin, -22.0, pos2, pos2);

	smoke = FX_AddLine(origin, pos2, 1.0, 20.0, -12.0, 0.7, 0.0, 500, cgs.media.hastePuffShader);
}*/

/*
===============
CG_FlightTrail
===============
*/
/*static void CG_FlightTrail( centity_t *cent ) {
	localEntity_t	*smoke;
	vec3_t			origin;
	vec3_t			vel;
	vec3_t			startrgb={0.5, 0.5, 0.5};
	vec3_t			endrgb={0.0,0.0,0.0};

	VectorCopy( cent->lerpOrigin, origin );
	origin[2] -= flrandom(10,18);

	VectorSet(vel, flrandom(-10,10), flrandom(-10, 10), flrandom(-30,-50));

	smoke = FX_AddSprite2(origin, vel, qfalse, 4.0, 4.0, 0.5, 0.0, startrgb, endrgb, flrandom(0,360), 0, 500, cgs.media.flightPuffShader);
}*/

/*
===============
CG_TrailItem
===============
*/
/*static void CG_TrailItem( centity_t *cent, qhandle_t hModel ) {
	refEntity_t		ent;
	vec3_t			angles;
	float			frame;

	if ( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson ) 
	{
		return;
	}

	memset( &ent, 0, sizeof( ent ) );

	VectorCopy( cent->lerpAngles, angles );
	angles[PITCH] = 0;
	angles[ROLL] = 0;
	angles[YAW] += 180.0;		// It's facing the wrong way.
	AnglesToAxis( angles, ent.axis );

	VectorMA( cent->lerpOrigin, 12, ent.axis[0], ent.origin );
	ent.origin[2] += 4;

	// Make it animate.
	frame = (cg.time / 100.0);
	ent.renderfx|=RF_WRAP_FRAMES;

	ent.oldframe = (int)frame;
	ent.frame = (int)frame+1;
	ent.backlerp = (float)(ent.frame) - frame;

	// if the player is looking at himself in 3rd person, don't show the flag solid, 'cause he can't see!!!
	if (cent->currentState.number == cg.snap->ps.clientNum)
	{
		ent.shaderRGBA[3] = 128;
		ent.renderfx |= RF_FORCE_ENT_ALPHA;
	}

	VectorScale(ent.axis[0], 0.75, ent.axis[0]);
	VectorScale(ent.axis[1], 0.9, ent.axis[1]);
	VectorScale(ent.axis[2], 0.9, ent.axis[2]);
	ent.nonNormalizedAxes = qtrue;

#if 0	// This approach is used if you want the item to autorotate.  Since this is the flag, we don't.
	VectorScale( cg.autoAxis[0], 0.75, ent.axis[0] );
	VectorScale( cg.autoAxis[1], 0.75, ent.axis[1] );
	VectorScale( cg.autoAxis[2], 0.75, ent.axis[2] );
#endif

	ent.hModel = hModel;
	trap_R_AddRefEntityToScene( &ent );
}*/

/*
===============
CG_PlayerPowerups
===============
*/
static void CG_PlayerPowerups( centity_t *cent ) {
	int		powerups;

	powerups = cent->currentState.powerups;
	if ( !powerups ) {
		return;
	}

	// quad gives a dlight
/*	if ( powerups & ( 1 << PW_QUAD ) ) {
		trap_R_AddLightToScene( cent->lerpOrigin, 200 + (rand()&31), 0.2, 0.2, 1.0 );
	}*/

	// invul gives a dlight
/*	if ( powerups & ( 1 << PW_BOLTON ) ) {
		trap_R_AddLightToScene( cent->lerpOrigin, 200 + (rand()&31), 0.8, 0.8, 0.2 );
	}*/

	// borg adapt gives a dlight
	//RPG-X TiM
	/*if ( powerups & ( 1 << PW_BEAMING ) ) {
		trap_R_AddLightToScene( cent->lerpOrigin, 200 + (rand()&31), 0.2, 1.0, 0.2 );
	}*/

	// flight plays a looped sound
//	if ( powerups & ( 1 << PW_FLIGHT ) ) {
//		trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin, cgs.media.flightSound );
//	}

	// redflag
	/*if ( powerups & ( 1 << PW_REDFLAG ) ) {
		CG_TrailItem( cent, cgs.media.redFlagModel );
		trap_R_AddLightToScene( cent->lerpOrigin, 200 + (rand()&31), 1, 0.2, 0.2 );
	}*/

	// blueflag | RGP-X | GSIO01 | 08/05/2009: no blueflag no more... now borgadapt
	if ( powerups & ( 1 << PW_BORG_ADAPT ) ) {
		//CG_TrailItem( cent, cgs.media.blueFlagModel );
		//trap_R_AddLightToScene( cent->lerpOrigin, 200 + (rand()&31), 0.2, 0.2, 1 );
		trap_R_AddLightToScene( cent->lerpOrigin, 200 + (rand()&31), 0.2, 1.0, 0.2 );
	}

	// haste leaves smoke trails
	/*if ( powerups & ( 1 << PW_HASTE ) && (cent->currentState.groundEntityNum==ENTITYNUM_WORLD)) {
		CG_HasteTrail( cent );
	}*/

	// haste leaves smoke trails
//	if ( powerups & ( 1 << PW_FLIGHT ) && (cent->currentState.groundEntityNum!=ENTITYNUM_WORLD)) {
//		CG_FlightTrail( cent );
//	}

	// seeker coolness
	/*if ( powerups & ( 1 << PW_FLASHLIGHT ) )
	{
		CG_Seeker(cent);
	}*/
}




/*
===============
CG_PlayerFloatSprite

Float a sprite over the player's head
===============
*/
static void CG_PlayerFloatSprite( centity_t *cent, qhandle_t shader ) {
	int				rf;
	refEntity_t		ent;
	int				team;

	if ( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson ) {
		rf = RF_THIRD_PERSON;		// only show in mirrors
	} else {
		rf = 0;
	}

	team = cgs.clientinfo[ cent->currentState.clientNum ].team;

	memset( &ent, 0, sizeof( ent ) );
	VectorCopy( cent->lerpOrigin, ent.origin );
	ent.origin[2] += 48;
	ent.reType = RT_SPRITE;
	ent.customShader = shader;
	ent.data.sprite.radius = 10;
	ent.renderfx = rf;
	if (team==TEAM_RED)
	{
		ent.shaderRGBA[0] = 255;
		ent.shaderRGBA[1] = 64;
		ent.shaderRGBA[2] = 64;
	}
	else if (team==TEAM_BLUE)
	{
		ent.shaderRGBA[0] = 64;
		ent.shaderRGBA[1] = 64;
		ent.shaderRGBA[2] = 255;
	}
	else
	{
		ent.shaderRGBA[0] = 255;
		ent.shaderRGBA[1] = 255;
		ent.shaderRGBA[2] = 255;
	}
	ent.shaderRGBA[3] = 255;
	trap_R_AddRefEntityToScene( &ent );
}



/*
===============
CG_PlayerSprites

Float sprites over the player's head
===============
*/
static void CG_PlayerSprites( centity_t *cent ) {
//	int		team;

	if ( cent->currentState.eFlags & EF_CONNECTION )
	{
		CG_PlayerFloatSprite( cent, cgs.media.connectionShader );
		return;
	}

/*
	if ( cent->currentState.eFlags & EF_TALK )
	{
		if ( cgs.clientinfo[cent->currentState.number].pClass == PC_ACTIONHERO )
		{
			CG_PlayerFloatSprite( cent, cgs.media.heroSpriteShader );
		}
		if ( cgs.clientinfo[cent->currentState.number].pClass == PC_BORG )
		{
			if ( (cg_entities[cent->currentState.number].currentState.powerups&(1<<PW_LASER)) )
			{
				CG_PlayerFloatSprite( cent, cgs.media.borgQueenIconShader );
			}
			else
			{
				CG_PlayerFloatSprite( cent, cgs.media.borgIconShader );
			}
		}
		CG_PlayerFloatSprite( cent, cgs.media.chatShader );
		return;
	}

	//label the action hero
	if ( cgs.clientinfo[cent->currentState.number].pClass == PC_ACTIONHERO )
	{
		CG_PlayerFloatSprite( cent, cgs.media.heroSpriteShader );
		return;
	}

	//Special hack: if it's Borg who has regen going, must be Borg queen
	if ( cgs.clientinfo[cent->currentState.number].pClass == PC_BORG )
	{
		if ( (cg_entities[cent->currentState.number].currentState.powerups&(1<<PW_LASER)) )
		{
			CG_PlayerFloatSprite( cent, cgs.media.borgQueenIconShader );
			return;
		}
	}

	//NOTE: Borg *Queen* should have been caught above
	if ( cgs.clientinfo[cent->currentState.number].pClass == PC_BORG )
	{
		CG_PlayerFloatSprite( cent, cgs.media.borgIconShader );
		return;
	}

	if ( cent->currentState.eFlags & EF_AWARD_FIRSTSTRIKE ) {
		CG_PlayerFloatSprite( cent, cgs.media.medalFirstStrike );
		return;
	}

	if ( cent->currentState.eFlags & EF_AWARD_IMPRESSIVE ) {
		CG_PlayerFloatSprite( cent, cgs.media.medalImpressive );
		return;
	}

	if ( cent->currentState.eFlags & EF_AWARD_EXCELLENT ) {
		CG_PlayerFloatSprite( cent, cgs.media.medalExcellent );
		return;
	}

	if ( cent->currentState.eFlags & EF_AWARD_ACE ) {
		CG_PlayerFloatSprite( cent, cgs.media.medalAce );
		return;
	}

	if ( cent->currentState.eFlags & EF_AWARD_EXPERT ) {
		CG_PlayerFloatSprite( cent, cgs.media.medalExpert );
		return;
	}

	if ( cent->currentState.eFlags & EF_AWARD_MASTER ) {
		CG_PlayerFloatSprite( cent, cgs.media.medalMaster );
		return;
	}

	if ( cent->currentState.eFlags & EF_AWARD_CHAMPION ) {
		CG_PlayerFloatSprite( cent, cgs.media.medalChampion );
		return;
	}

	team = cgs.clientinfo[ cent->currentState.clientNum ].team;
	if ( !(cent->currentState.eFlags & EF_DEAD) && 
			cg.snap->ps.persistant[PERS_TEAM] == team &&
			cgs.gametype >= GT_TEAM &&
			cent->currentState.number != cg.snap->ps.clientNum )	// Don't show a sprite above a player's own head in 3rd person.
	{
		if (team==TEAM_RED)
		{
			CG_PlayerFloatSprite( cent, cgs.media.teamRedShader );
		}
		else if (team==TEAM_BLUE)
		{
			CG_PlayerFloatSprite( cent, cgs.media.teamBlueShader );
		}
		// else don't show an icon.  There currently are no other team types.

		return;
	}*/

	//RPG-X: RedTechie - Cloak sprite basiclly other admins will see this only to tell if that player is cloaked and a admin
	//if(	cent->currentState.powerups & ( 1 << PW_INVIS ) ){
	//	CG_PlayerFloatSprite( cent, cgs.media.cloakspriteShader );
	//	return;
	//}
return;

}

/*
===============
CG_CalcBeamAlpha
By TiM

Calculates the current point
in a transport cycle so we 
can use it as a fade percentage
Used in shadows, and on the player
model itself
===============
*/
#define PLAYER_BEAM_FADETIME_DIV 1.0/(float)PLAYER_BEAM_FADETIME

void CG_CalcBeamAlpha( int powerups, beamData_t *beamData ) {
	float beamAlpha = 1.0;
	int bTime = 0;


	if ( ( powerups & ( 1 << PW_BEAM_OUT ) ) || ( powerups & ( 1 << PW_QUAD ) ) ) {
		//TiM - SP transporter FX, also base alpha off of phase in transport cycle
		//bTime = cg.time - beamData->beamTimeParam;

		bTime = cg.time - beamData->beamTimeParam;

		if (bTime > PLAYER_BEAM_FADE ) {
			if ( bTime < ( PLAYER_BEAM_FADE + PLAYER_BEAM_FADETIME) ) {
				beamAlpha = (float)( bTime - PLAYER_BEAM_FADE ) * PLAYER_BEAM_FADETIME_DIV;

				//if we're beaming out, invert the alpha value (so we fade out, not in )
				if ( powerups & ( 1 << PW_BEAM_OUT ) ) {
					beamAlpha = 1.0 - beamAlpha;
				}

				//Com_Printf( "Alpha = %f\n", beamAlpha );
			}
			else {
				if ( powerups & ( 1 << PW_BEAM_OUT ) ) {
					beamAlpha = 0.0;
				}
				else {
					beamAlpha = 1.0;
				}
			}
		}
		else {
			if ( powerups & ( 1 << PW_BEAM_OUT ) ) {
				beamAlpha = 1.0;
			}
			else {
				beamAlpha = 0.0;
			}
		}
		//CG_Printf( "BeamTime: %i, Alpha: %f\n", bTime, beamAlpha );
	}

	beamData->beamAlpha = beamAlpha;
}

/*
===============
CG_PlayerShadow

Returns the Z component of the surface being shadowed

  should it return a full plane instead of a Z?
===============
*/
#define	SHADOW_DISTANCE		128
static qboolean CG_PlayerShadow( centity_t *cent, vec3_t origin, float *shadowPlane, float sizeOffset ) {
	vec3_t		end, mins = {-7, -7, 0}, maxs = {7, 7, 2};
	trace_t		trace;
	float		alpha;

	*shadowPlane = 0;

	if ( cg_shadows.integer == 0 ) {
		return qfalse;
	}

	// no shadows when invisible
	//TiM - handled in two phases.  When invis powerup is active, and beyond flash time, or invis is inactvie, and before flashtime
	if ( 
		( ( cent->currentState.powerups & ( 1 << PW_INVIS ) && cent->cloakTime > 0 && cg.time > cent->cloakTime + Q_FLASH_TIME * 0.5 ) 
			|| ( !(cent->currentState.powerups & ( 1 << PW_INVIS )) && cent->decloakTime > 0 && cg.time < cent->decloakTime + Q_FLASH_TIME * 0.5 ) ) )
	{
		return qfalse;
	}

	// send a trace down from the player to the ground
	//VectorCopy( cent->lerpOrigin, end );
	VectorCopy( origin, end );
	end[2] -= SHADOW_DISTANCE;

	//trap_CM_BoxTrace( &trace, cent->lerpOrigin, end, mins, maxs, 0, MASK_PLAYERSOLID );
	trap_CM_BoxTrace( &trace, origin, end, mins, maxs, 0, MASK_PLAYERSOLID );

	// no shadow if too high
	if ( trace.fraction == 1.0 ) {
		return qfalse;
	}

	*shadowPlane = trace.endpos[2] + 1;

	if ( cg_shadows.integer != 1 ) {	// no mark for stencil or projection shadows
		return qtrue;
	}

	// fade the shadow out with height
	alpha = 1.0 - trace.fraction;

	//transporter FX - beam alpha
	alpha *= cent->beamData.beamAlpha;

	if ( alpha == 0.0 ) {
		return qfalse;
	}

	// --TiM

	// add the mark as a temporary, so it goes directly to the renderer
	// without taking a spot in the cg_marks array
	CG_ImpactMark( cgs.media.shadowMarkShader, trace.endpos, trace.plane.normal, 
		cent->pe.legs.yawAngle, 1,1,1,alpha, qfalse, ( 16 * sizeOffset ), qtrue );

	return qtrue;
}


/*
===============
CG_PlayerSplash

Draw a mark at the water surface
===============
*/
static void CG_PlayerSplash( centity_t *cent ) {
	vec3_t		start, end;
	trace_t		trace;
	int			contents;
	polyVert_t	verts[4];

	float		beamRatio = 1.0;

	if ( !cg_shadows.integer ) {
		return;
	}

	VectorCopy( cent->lerpOrigin, end );
	end[2] -= 24;

	// if the feet aren't in liquid, don't make a mark
	// this won't handle moving water brushes, but they wouldn't draw right anyway...
	contents = trap_CM_PointContents( end, 0 );
	if ( !( contents & ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) ) ) {
		return;
	}

	VectorCopy( cent->lerpOrigin, start );
	start[2] += 32;

	// if the head isn't out of liquid, don't make a mark
	contents = trap_CM_PointContents( start, 0 );
	if ( contents & ( CONTENTS_SOLID | CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) ) {
		return;
	}

	// trace down to find the surface
	trap_CM_BoxTrace( &trace, start, end, NULL, NULL, 0, ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) );

	if ( trace.fraction == 1.0 ) {
		return;
	}

	if ( (cent->currentState.powerups & ( 1 << PW_BEAM_OUT ) ) && ( cg.time > ( cent->beamData.beamTimeParam + 2000 ) ) ) {
		//beamRatio = 1.0f - (( (float)cg.time - ( (float)cent->beamData.beamTimeParam + 2000.0f ) ) / 2000.0f);
		beamRatio = 1.0f - (( (float)cg.time - ( (float)cent->beamData.beamTimeParam + 2000.0f ) ) * 0.0005f);
		//CG_Printf( "Beam Out: %f\n", beamRatio );
	}

	if ( cent->currentState.powerups & ( 1 << PW_QUAD ) ) {
		//beamRatio = ( ( (float)cg.time - (float)cent->beamData.beamTimeParam ) / 2000.0f );
		beamRatio = ( ( (float)cg.time - (float)cent->beamData.beamTimeParam ) * 0.0005f );
		//CG_Printf( "Beam In: %f\n", beamRatio );
	}

	if ( beamRatio > 1.0 ) {
		beamRatio = 1.0;
	}
	
	if ( beamRatio < 0.0 ) {
		beamRatio = 0.0;
	}

	// create a mark polygon
	VectorCopy( trace.endpos, verts[0].xyz );
	verts[0].xyz[0] -= 32 * beamRatio;
	verts[0].xyz[1] -= 32 * beamRatio;
	verts[0].st[0] = 0;
	verts[0].st[1] = 0;
	verts[0].modulate[0] = 255;
	verts[0].modulate[1] = 255;
	verts[0].modulate[2] = 255;
	verts[0].modulate[3] = 255;

	VectorCopy( trace.endpos, verts[1].xyz );
	verts[1].xyz[0] -= 32 * beamRatio;
	verts[1].xyz[1] += 32 * beamRatio;
	verts[1].st[0] = 0;
	verts[1].st[1] = 1;
	verts[1].modulate[0] = 255;
	verts[1].modulate[1] = 255;
	verts[1].modulate[2] = 255;
	verts[1].modulate[3] = 255;

	VectorCopy( trace.endpos, verts[2].xyz );
	verts[2].xyz[0] += 32 * beamRatio;
	verts[2].xyz[1] += 32 * beamRatio;
	verts[2].st[0] = 1;
	verts[2].st[1] = 1;
	verts[2].modulate[0] = 255;
	verts[2].modulate[1] = 255;
	verts[2].modulate[2] = 255;
	verts[2].modulate[3] = 255;

	VectorCopy( trace.endpos, verts[3].xyz );
	verts[3].xyz[0] += 32 * beamRatio;
	verts[3].xyz[1] -= 32 * beamRatio;
	verts[3].st[0] = 1;
	verts[3].st[1] = 0;
	verts[3].modulate[0] = 255;
	verts[3].modulate[1] = 255;
	verts[3].modulate[2] = 255;
	verts[3].modulate[3] = 255;

	trap_R_AddPolyToScene( cgs.media.wakeMarkShader, 4, verts );
}
static int	timestamp;

/*
===============
CG_AddRefEntityWithPowerups

Adds a piece with modifications or duplications for powerups
Also called by CG_Missile for quad rockets, but nobody can tell...
===============
*/
void CG_AddRefEntityWithPowerups( refEntity_t *ent, int powerups, int eFlags, beamData_t *beamData, int cloakTime, int decloakTime, qboolean borg )
{

	//TiM : No more flickering. Flickering Starfleet officers is bad
	/*if ( eFlags & EF_ITEMPLACEHOLDER )			// Hologram Decoy
	{
		float f1, f2;

		// We used EF_ITEMPLACEHOLDER flag to indicate that this 'player' model
		// is actually a holographic decoy.  Now there is a chance that the
		// decoy will flicker a bit because of ordering with alpha shaders...

		// The lowest the alpha goes is 4.0-2.5-1.0=0.5.
		f1 = 4.0 + 2.5*sin(52.423 + cg.time/205.243);
		f2 = sin(14.232 + cg.time/63.572);
		
		f1 = f1+f2;
		if (f1 > 1.0)
		{	// Just draw him solid.
			if ( cg.snap->ps.persistant[PERS_CLASS] == PC_TECH )
			{//technicians can see decoys as grids
				ent->customShader = cgs.media.rezOutShader;
				ent->shaderRGBA[0] =
				ent->shaderRGBA[1] =
				ent->shaderRGBA[2] = 128;
			}

			trap_R_AddRefEntityToScene( ent );		
		}
		else
		{	// Draw him faded.
			if (f1 > 0.8)
			{	// Don't have alphas over 0.8, it just looks bad.
				f1=0.8;
			}
			else if (f1 < 0.1)
			{
				f1=0.1;
			}

			ent->renderfx |= RF_FORCE_ENT_ALPHA;	// Override the skin shader info and use this alpha value.
			ent->shaderRGBA[3] = 255.0*f1;			
			trap_R_AddRefEntityToScene( ent );		
			ent->renderfx &= ~RF_FORCE_ENT_ALPHA;	
			ent->shaderRGBA[3] = 255;				

			// ...with a static shader.
			ent->customShader = cgs.media.holoDecoyShader;
			ent->shaderRGBA[0] = 
			ent->shaderRGBA[1] = 
			ent->shaderRGBA[2] = 255.0*(1.0-f1);	// More solid as the player fades out...
			trap_R_AddRefEntityToScene(ent);
		}

		return;
	}*/
	if ((eFlags & EF_DEAD) && (timestamp > cg.time))
	{	// Dead.  timestamp holds the time of death.
		
		float alpha;
		int a;

		// First draw the entity itself.
		//alpha = (timestamp - cg.time)/2500.0;
		alpha = (timestamp - cg.time) * 0.0004;
		ent->renderfx |= RF_FORCE_ENT_ALPHA;
		a = alpha * 255.0;
		if (a <= 0)
			a=1;
		ent->shaderRGBA[3] = a;
		trap_R_AddRefEntityToScene( ent );
		ent->renderfx &= ~RF_FORCE_ENT_ALPHA;
		ent->shaderRGBA[3] = 255;
		
		// Now draw the static shader over it.
		// Alpha in over half the time, out over half.
		alpha = sin(M_PI*alpha);
		a = alpha * 255.0;
		if (a <= 0)
			a=1;
		ent->customShader = cgs.media.rezOutShader;
		ent->shaderRGBA[0] =
		ent->shaderRGBA[1] =
		ent->shaderRGBA[2] = a;
		trap_R_AddRefEntityToScene( ent );
		ent->shaderRGBA[0] =
		ent->shaderRGBA[1] =
		ent->shaderRGBA[2] = 255;
	}
	else if( powerups & ( 1 << PW_BORG_ADAPT ) ) 
	{
		ent->renderfx |= RF_FORCE_ENT_ALPHA;
		ent->shaderRGBA[3] = 255;
		trap_R_AddRefEntityToScene(ent);
		ent->customShader = cgs.media.borgFullBodyShieldShader;
		trap_R_AddRefEntityToScene(ent);
		return;
	}
	else if ( powerups & ( 1 << PW_INVIS ) || ( !(powerups & ( 1 << PW_INVIS )) && decloakTime > 0 ) )
	{
		if ( ( cloakTime <= 0 && decloakTime <= 0 ) || ( decloakTime > 0 && cg.time < ( decloakTime + Q_FLASH_TIME * 0.5 ) )
				|| ( cloakTime > 0 && cg.time > ( cloakTime + Q_FLASH_TIME * 0.5 ) ) )
		{
			if ( cgs.clientinfo[cg.snap->ps.clientNum].isAdmin/*cg.snap->ps.persistant[PERS_CLASS] == PC_ADMIN*/ )
			{//admins can see cloaked people
				//RPG-X: RedTechie - Pretty Admin Stuff
				//ent->customShader = cgs.media.teleportEffectShader;
				ent->renderfx |= RF_FORCE_ENT_ALPHA;
				ent->shaderRGBA[3] = (unsigned char)(0.4f * 255.0f);
				trap_R_AddRefEntityToScene( ent );
			}
			else
				return;
			//ent->customShader = cgs.media.invisShader; //TiM : No point since it's a 100% transparent shader. Use the EF_NODRAW flag instead
		}
		else
			trap_R_AddRefEntityToScene( ent );
	}
	else if (powerups & (1<<PW_DISINTEGRATE))
	{
		int dtime; 

		dtime = cg.time-timeParam;
		if (dtime < 1000)
		{
			ent->renderfx |= RF_FORCE_ENT_ALPHA;
			ent->shaderRGBA[3] = 255 - (dtime)*0.25;
			trap_R_AddRefEntityToScene( ent );
			ent->renderfx &= ~RF_FORCE_ENT_ALPHA;
			ent->shaderRGBA[3] = 255;
		}
		
		if (dtime < 2000)
		{
			ent->customShader = cgs.media.disruptorShader;
			//ent->shaderTime = timeParam / 1000.0f;
			ent->shaderTime = timeParam * 0.001f;
			trap_R_AddRefEntityToScene( ent );
		}
	}
	else if (powerups & (1<<PW_EXPLODE))
	{
		int dtime; 

		dtime = cg.time-timeParam;

		if (dtime < 300)
		{
			ent->renderfx |= RF_FORCE_ENT_ALPHA;
			ent->shaderRGBA[3] = (int)(255.0 - (dtime / 300.0) * 254.0); 
			trap_R_AddRefEntityToScene( ent );
			ent->renderfx &= ~RF_FORCE_ENT_ALPHA;
			ent->shaderRGBA[3] = 255;
		}

		if (dtime < 500)
		{
			ent->customShader = cgs.media.explodeShellShader;
			ent->renderfx |= RF_CAP_FRAMES;
			//ent->shaderTime = timeParam / 1000.0f;
			ent->shaderTime = timeParam * 0.001f;
			trap_R_AddRefEntityToScene( ent );
			ent->renderfx &= ~RF_CAP_FRAMES;
		}
	}
	else if (powerups & (1<<PW_GHOST))
	{
		ent->renderfx |= RF_FORCE_ENT_ALPHA;
		//ent->shaderRGBA[3] = 100 + 50*sin(cg.time/200.0);
		ent->shaderRGBA[3] = 100 + 50*sin(cg.time * 0.005);
		trap_R_AddRefEntityToScene( ent );
		ent->renderfx &= ~RF_FORCE_ENT_ALPHA;
		ent->shaderRGBA[3] = 255;
	}
	//SP Transporter Effect
	else if ( powerups & ( 1 << PW_BEAM_OUT ) || powerups & ( 1 << PW_QUAD ) )
	{
		int btime;
		btime = cg.time - beamData->beamTimeParam;

		if ( btime <= PLAYER_BEAM_FADE ) {
			if ( powerups & ( 1 << PW_BEAM_OUT ) ) {
				ent->shaderRGBA[3] = 255;
			}
			else {
				ent->shaderRGBA[3] = 0;
			}
		}
		else if ( btime >= ( PLAYER_BEAM_FADE + PLAYER_BEAM_FADETIME ) ) {
			if ( powerups & ( 1 << PW_BEAM_OUT ) ) {
				ent->shaderRGBA[3] = 0;
			}
			else {
				ent->shaderRGBA[3] = 255;
			}
		}

		if (btime > PLAYER_BEAM_FADE && btime < (PLAYER_BEAM_FADE + PLAYER_BEAM_FADETIME) )
		{
			ent->renderfx |= RF_FORCE_ENT_ALPHA;
			ent->shaderRGBA[3] = (int)(255 * beamData->beamAlpha);
		}

		if ( ent->shaderRGBA[3] > 0 ) {
			trap_R_AddRefEntityToScene( ent );
			ent->renderfx &= ~RF_FORCE_ENT_ALPHA;
			ent->shaderRGBA[3] = 255;
		}
		
		if ( btime < 4100 ) {
			ent->customShader = cgs.media.transportShader;
			//ent->shaderTime = beamData->beamTimeParam / 1000.0f;
			ent->shaderTime = beamData->beamTimeParam * 0.001f;
			trap_R_AddRefEntityToScene( ent );
		}
	}
	else
	{
		trap_R_AddRefEntityToScene( ent );
		
		//if we did have third person alpha
		if ( ent->renderfx & RF_FORCE_ENT_ALPHA ) {
			ent->renderfx &= ~RF_FORCE_ENT_ALPHA;
		}
		// Quad should JUST be on the weapon now, sparky.
/*		if ( powerups & ( 1 << PW_QUAD ) ) 
		{
			if (team == TEAM_RED)
				ent->customShader = cgs.media.redQuadShader;
			else
				ent->customShader = cgs.media.quadShader;
			trap_R_AddRefEntityToScene( ent );
		}
*/		

		/*if ( powerups & ( 1 << PW_LASER ) ) {
			if ( ( ( cg.time / 100 ) % 10 ) == 1 ) {
				ent->customShader = cgs.media.regenShader;
				trap_R_AddRefEntityToScene( ent );
			}
		}*/
		/*if ( powerups & ( 1 << PW_BEAMING ))
		{
			ent->customShader = cgs.media.borgFullBodyShieldShader;
			trap_R_AddRefEntityToScene( ent );
			return;
		}*/
/*		if ( powerups & ( 1 << PW_BOLTON )) 
		{
				ent->customShader = cgs.media.battleSuitShader;
			trap_R_AddRefEntityToScene( ent );
			return;
		}*/
		/*if (powerups & (1 << PW_OUCH))
		{
			ent->customShader = cgs.media.holoOuchShader;
			// set rgb to 1 of 16 values from 0 to 255. don't use random so that the three
			//parts of the player model as well as the gun will all look the same
			ent->shaderRGBA[0] = 
			ent->shaderRGBA[1] = 
			ent->shaderRGBA[2] = ((cg.time % 17)*0.0625)*255.0;//irandom(0,255);
			trap_R_AddRefEntityToScene(ent);
		}*/
		
		if (powerups & (1<<PW_ARCWELD_DISINT))
		{
			int dtime; 

			dtime = cg.time-timeParam;

			if (dtime < irandom(0,4000))
			{
				// Add an electrical shell, faded out over the first three seconds.
				ent->customShader = cgs.media.electricBodyShader;
//				ent->shaderTime = timeParam / 1000.0f;
				ent->shaderRGBA[0] = 
				ent->shaderRGBA[1] = 
				//ent->shaderRGBA[2] = (int)(1.0 + ((4000.0 - dtime) / 4000.0f * 254.0f ));
				ent->shaderRGBA[2] = (int)(1.0 + ((4000.0 - dtime) * 0.00025f + 256.0f));
				ent->shaderRGBA[3] = 255;
				trap_R_AddRefEntityToScene( ent );

				if ( random() > 0.95f )
				{
					// Play a zap sound to go it.
					trap_S_StartSound (ent->origin, entNum, CHAN_AUTO, cg_weapons[WP_13].altHitSound);
				}
			}
		}
	}
}

#define MAX_SHIELD_TIME	2500.0
#define MIN_SHIELD_TIME	1750.0


void CG_PlayerShieldHit(int entitynum, vec3_t dir, int amount)
{
	centity_t *cent;
	int	time;

	if (entitynum<0 || entitynum >= MAX_CLIENTS)
	{
		return;
	}

	cent = &cg_entities[entitynum];

	if (amount > 100)
	{
		time = cg.time + MAX_SHIELD_TIME;		// 2 sec.
	}
	else
	{
		time = cg.time + 500 + amount*20;
	}

	if (time > cent->damageTime)
	{
		cent->damageTime = time;
		VectorScale(dir, -1, dir);
		vectoangles(dir, cent->damageAngles);
	}
}


/*void CG_DrawPlayerShield(centity_t *cent, vec3_t origin)
{
	refEntity_t ent;
	int			alpha;
	float		scale;
	
	// Don't draw the shield when the player is dead.
	if (cent->currentState.eFlags & EF_DEAD)
	{
		return;
	}

	memset( &ent, 0, sizeof( ent ) );

	VectorCopy( origin, ent.origin );
	ent.origin[2] += 10.0;
	AnglesToAxis( cent->damageAngles, ent.axis );

	alpha = 255.0 * ((cent->damageTime - cg.time) / MIN_SHIELD_TIME) + irandom(0, 16);
	if (alpha>255)
		alpha=255;

	// Make it bigger, but tighter if more solid
	scale = 1.8 - ((float)alpha*(0.4/255.0));		// Range from 1.4 to 1.8
	VectorScale( ent.axis[0], scale, ent.axis[0] );
	VectorScale( ent.axis[1], scale, ent.axis[1] );
	VectorScale( ent.axis[2], scale, ent.axis[2] );

	ent.hModel = cgs.media.explosionModel;
	ent.customShader = cgs.media.halfShieldShader;
	ent.shaderRGBA[0] = alpha;
	ent.shaderRGBA[1] = alpha;
	ent.shaderRGBA[2] = alpha;
	ent.shaderRGBA[3] = 255;
	trap_R_AddRefEntityToScene( &ent );
}*/


/*void CG_PlayerHitFX(centity_t *cent)
{
	centity_t *curent;

	// only do the below fx if the cent in question is...uh...me, and it's first person.
	if (cent->currentState.clientNum != cg.predictedPlayerState.clientNum || cg.renderingThirdPerson)
	{
		// Get the NON-PREDICTED player entity, because the predicted one doesn't have the damage info on it.
		curent = &cg_entities[cent->currentState.number];

		if (curent->damageTime > cg.time)
		{
			CG_DrawPlayerShield(curent, cent->lerpOrigin);
		}

		return;
	}
}*/

//------------------------------------
/*void CG_BorgEyebeam( centity_t *cent, const refEntity_t *parent )
{
	qboolean	large = qfalse;
	vec3_t		beamOrg, beamEnd;
	trace_t		trace;
	refEntity_t	temp;

	CG_PositionEntityOnTag( &temp, parent, parent->hModel, "tag_ear");

	if ( VectorCompare( temp.origin, parent->origin ))
	{
		// Vectors must be the same so the tag_ear wasn't found
		return;
	}
	//Note the above will also prevent the beam from being drawn in first person view if you don't have a tag_ear

	// well, we are in thirdperson or whatnot, so we should just render from a bolt-on ( tag_ear )
	if ( cent->currentState.clientNum != cg.predictedPlayerState.clientNum 
					|| cg.renderingThirdPerson 
					|| cg.snap->ps.pm_type == PM_INTERMISSION )
	{
		VectorCopy( temp.origin, beamOrg );
		VectorMA( beamOrg, 1024, temp.axis[0], beamEnd );//forward to end
	}
	else
	{
		vec3_t axis[3];

		// stupid offset hack
		AnglesToAxis( cent->lerpAngles, axis );
		VectorMA( cent->lerpOrigin, 26, axis[2], beamOrg );//up
		VectorMA( beamOrg,		   -26, axis[1], beamOrg );//right
//		VectorMA( beamOrg,		  0.2f, axis[0], beamOrg );//forward
		VectorMA( beamOrg,		  1024, axis[0], beamEnd );//forward to end
		large = qtrue; // render a fatter line
	}
			
	trap_CM_BoxTrace( &trace, beamOrg, beamEnd, NULL, NULL, 0, MASK_SHOT );
	VectorCopy( trace.endpos, beamEnd );
	VectorMA( beamOrg, 0.5, parent->axis[0], beamOrg );//forward

	FX_BorgEyeBeam( beamOrg, beamEnd, trace.plane.normal, large );
}*/

//------------------------------------------------------------------------------
//Equip mode features

void CG_AttachHolsters ( centity_t *cent, refEntity_t *parent, int weapon )
{
	refEntity_t holster;
	refEntity_t holsterInner;

	//CG_FlushRefEnt( parent, &holster );
	//CG_FlushRefEnt( parent, &holsterInner );

	memset( &holster, 0, sizeof( holster ) );
	VectorCopy( parent->lightingOrigin, holster.lightingOrigin );
	holster.shadowPlane = parent->shadowPlane;
	holster.renderfx = parent->renderfx;

	memset( &holsterInner, 0, sizeof( holsterInner ) );
	VectorCopy( parent->lightingOrigin, holsterInner.lightingOrigin );
	holsterInner.shadowPlane = parent->shadowPlane;
	holsterInner.renderfx = parent->renderfx;

	holster.hModel = (weapon == WP_5) ? cgs.media.phaserHolster : cgs.media.tricorderHolster;
	holsterInner.hModel = (weapon == WP_5) ? cgs.media.phaserHolsterInner : cgs.media.tricorderHolsterInner;

	if ( !holster.hModel ) {
		return;
	}

	CG_PositionEntityOnTag( &holster, parent, parent->hModel, (weapon == WP_5) ? "tag_p_holster" : "tag_t_holster" );
	CG_PositionEntityOnTag( &holsterInner, &holster, holster.hModel, (weapon == WP_5) ? "tag_phaser" : "tag_tricorder" );

	CG_AddRefEntityWithPowerups( &holster, cent->currentState.powerups, cent->currentState.eFlags, &cent->beamData, cent->cloakTime, cent->decloakTime, qfalse );
	
	if ( cent->currentState.weapon != weapon ) {
		CG_AddRefEntityWithPowerups( &holsterInner, cent->currentState.powerups, cent->currentState.eFlags, &cent->beamData, cent->cloakTime, cent->decloakTime, qfalse );
	}

}

void CG_AttachTools ( centity_t *cent, refEntity_t *parent, int weaponNum )
{
	refEntity_t tool;
	weaponInfo_t *weaponInfo;
	
	weaponInfo = &cg_weapons[weaponNum];	

	//CG_FlushRefEnt( parent, &holster );
	//CG_FlushRefEnt( parent, &holsterInner );

	memset( &tool, 0, sizeof( tool ) );
	VectorCopy( parent->lightingOrigin, tool.lightingOrigin );
	tool.shadowPlane = parent->shadowPlane;
	tool.renderfx = parent->renderfx;

	tool.hModel = weaponInfo->weaponModel;

	if ( !tool.hModel ) {
		return;
	}

	CG_PositionEntityOnTag( &tool, parent, parent->hModel, (weaponNum == WP_11) ? "tag_torso" : "tag_lhand" );

	if ( cent->currentState.weapon != weaponNum ) {
		CG_AddRefEntityWithPowerups( &tool, cent->currentState.powerups, cent->currentState.eFlags, &cent->beamData, cent->cloakTime, cent->decloakTime, qfalse );
	}

}
//--------------------------------------------------------------
//CG version of the flashlight+laser.

extern qboolean	CG_CalcMuzzlePoint( centity_t *cent, vec3_t muzzle, qboolean isDecoy );

void CG_AddBeamFX( centity_t *cent ) {
	//refEntity_t			ent;
	trace_t				tr;
	vec3_t				origin, forward, end;
	qboolean				isDecoy;

	if ( !( cent->currentState.powerups & ( 1 << PW_LASER ) ) && 
		 !( cent->currentState.powerups & ( 1 << PW_FLASHLIGHT ) ) ) 
	{
		return;
	}
	
	isDecoy = (qboolean)(cent->currentState.eFlags & EF_ITEMPLACEHOLDER );

	//Get origin to render effect
	//==============================
	//if it's us, we get special treatment, wielding uber-smooth feelings. :)
	if ( cent->currentState.clientNum == cg.predictedPlayerState.clientNum && !isDecoy ) {
		VectorCopy( cg.predictedPlayerState.origin, origin );
		origin[2] += (float)cg.predictedPlayerState.viewheight * cgs.clientinfo[cg.predictedPlayerState.clientNum].height;

		AngleVectors( cg.predictedPlayerState.viewangles, forward, NULL, NULL );
	}
	else { //else, we'll employ a rather hacky (but Official Ravensoft made) way to guess the data we'll need here on CG
		if ( CG_CalcMuzzlePoint( cent, origin, isDecoy ) )
			if ( !isDecoy )
				AngleVectors( cent->lerpAngles, forward, NULL, NULL );
			else
				AngleVectors( cent->currentState.angles, forward, NULL, NULL );
		else
			return;
	}
	
	//perform the trace
	VectorMA( origin, 8192, forward, end );
	CG_Trace( &tr, origin, NULL, NULL, end, cent->currentState.clientNum, CONTENTS_SOLID );

	if (tr.surfaceFlags & SURF_NOIMPACT || tr.surfaceFlags & SURF_SKY ) {
		return;
	}

	//offset back in a little, so we can actually see the ent lol
	VectorMA( tr.endpos, -0.5, forward, end );

	//==============================

	if ( cent->currentState.powerups & ( 1 << PW_LASER ) && cent->beamData.beamTimeParam == 0 ) {
		/*// create the render entity
		memset (&ent, 0, sizeof(ent));
		VectorCopy( end, ent.origin);

		ent.reType = RT_SPRITE;
		//ent.radius = 2;
		//ent.rotation = 0;
		ent.data.sprite.radius = 2;
		ent.customShader = cgs.media.laserShader;
		trap_R_AddRefEntityToScene( &ent );*/

		//TiM: based on the shadow code instead.  This is so the sprite
		//is always aligned to the normal of the surface it hits, making it
		//act more like a fricken laser. :)
		CG_ImpactMark( cgs.media.laserShader, end, tr.plane.normal, 
		0,1,1,1,1, qfalse, 2, qtrue );
	}

	if ( cent->currentState.powerups & ( 1 << PW_FLASHLIGHT ) && cent->beamData.beamTimeParam == 0 ) {
		vec3_t delta;
		float	dist, intensity;

		//TiM - calc radius based on distance.
		//So it'll simulate real flashlights getting bigger/smaller the closer u are
		VectorSubtract( end, origin, delta );
		dist = VectorNormalize( delta );
		//intensity = ( dist / 256.0f );
		intensity = ( dist * 0.00390625f );

		if ( intensity > 1.0f )
			intensity = 1.0f;
		if ( intensity < 0.3f )
			intensity = 0.3f;

		trap_R_AddLightToScene( end, intensity * 200.0f, 1, 1, 1); //200
	}
}

//--------------------------------------------------------------

/*
===============
CG_Player
===============
*/
#define EYES_BLINK_TIME 250
void CG_Player( centity_t *cent ) {
	clientInfo_t	*ci;
	refEntity_t		legs;
	refEntity_t		torso;
	refEntity_t		head;
	int				clientNum;
	int				renderfx;
	qboolean		shadow, borg = qfalse;
	float			shadowPlane = 0;
	//alpha value
	float			alpha = cg_thirdPersonAlpha.value;
	int i = 0;
	rankModelData_t*	rankModelData = NULL;
	qboolean		rankDataValidated = qfalse;
	qboolean		isDecoy = qfalse;

	// the client number is stored in clientNum.  It can't be derived
	// from the entity number, because a single client may have
	// multiple corpses on the level using the same clientinfo
	clientNum = cent->currentState.clientNum;
	if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
		CG_Error( "Bad clientNum on player entity");
	}

	isDecoy = cent->currentState.eFlags & EF_ITEMPLACEHOLDER;

	//if we're a decoy, use the decoy info instead
	if ( isDecoy )
		ci = &cgs.decoyInfo[ cent->currentState.eventParm ];
	else
		ci = &cgs.clientinfo[ clientNum ];

	// it is possible to see corpses from disconnected players that may
	// not have valid clientinfo
	if ( !ci->infoValid ) {
		return;
	}

	//borg = ( ci->pClass == PC_BORG ? qtrue : qfalse );
	borg = qfalse;

	// If I'm a borg and there is another borg teleporting, at least allow me to see a trail, then return
	/*if ( cg.snap->ps.persistant[PERS_CLASS]	== PC_BORG && borg && ( cent->currentState.eFlags & EF_NODRAW ))
	{
		FX_BorgTeleportTrails( cent->lerpOrigin );
		return;
	}*/

	// add the talk baloon or disconnect icon (not in intermission)
	if ( !cg.intermissionStarted ) 
	{
		//RPG-X: RedTechie - Make sure there a admin before printing sprites above there head
		if(cgs.clientinfo[cg.snap->ps.clientNum].isAdmin/*cg.snap->ps.persistant[PERS_CLASS] == PC_ADMIN*/){
			CG_PlayerSprites( cent );
		}
	}

	//TiM Little hack for RGBA shader rendering
	//if we're completely invisible... save us some processing power lol
	if ( alpha <= 0.0 ) {
		return;
	}

	if (cent->currentState.eFlags & EF_NODRAW)
	{	// Don't draw anymore...
		return;
	}

	memset( &legs, 0, sizeof(legs) );
	memset( &torso, 0, sizeof(torso) );
	memset( &head, 0, sizeof(head) );

	//TiM : Manually refresh anims here to make sure it's updated around the board.
	//Previously, new players would be loaded, but anim data wouldn't, resulting in really weird looking poses.
	//Absolutely flushing all current animation data, forcing the code to reset is the only way I managed to get it working O_o
	if ( !ci->animsFlushed ) {
		lerpFrame_t *lf;
		lerpFrame_t *tf;

		lf = &cent->pe.legs;
		tf = &cent->pe.torso;
		
		lf->animation		= tf->animation			= NULL;
		lf->animationNumber = tf->animationNumber	= 0;
		lf->animationTime	= tf->animationTime		= 0;
		lf->backlerp		= tf->backlerp			= 0;
		lf->frame			= tf->frame				= 0;
		lf->frameTime		= tf->frameTime			= 0;
		lf->oldFrame		= tf->oldFrame			= 0;
		lf->oldFrameTime	= tf->oldFrameTime		= 0;

		//TiM : As much as I'd love to do this, it'll delete current angles too,
		//which results in players abruptly turning sharply when changing models.
		//And might possibly make corpses do the same... which would look creepy lol

		//memset( &cent->pe.legs, 0, sizeof ( cent->pe.legs ) );
		//memset( &cent->pe.torso, 0, sizeof ( cent->pe.torso ) );
		ci->animsFlushed = qtrue;
	}

	// get the rotation information
	CG_PlayerAngles( cent, legs.axis, torso.axis, head.axis );

	// get the animation state (after rotation, to allow feet shuffle)
	CG_PlayerAnimation( cent, &legs.oldframe, &legs.frame, &legs.backlerp,
		 &torso.oldframe, &torso.frame, &torso.backlerp );

	// add powerups floating behind the player
	CG_PlayerPowerups( cent );

	//backup catch for the lift entity
	if ( cg_liftEnts[cent->currentState.clientNum] > 0 && cg_liftEnts[cent->currentState.clientNum] < (cg.time - cgs.levelStartTime) )
		cg_liftEnts[cent->currentState.clientNum] = 0;

	//==========================================================

	/*CG_CalcBeamAlpha( cent->currentState.powerups, &cent->beamData );

	// add the shadow
	if ( !(cent->currentState.eFlags & EF_ITEMPLACEHOLDER) )
	{
		shadow = CG_PlayerShadow( cent, &shadowPlane, ci->height );
	}
	else
	{	//  - unless we are a hologram...
		shadow=qfalse;
		shadowPlane=0;
	}*/

	// add a water splash if partially in and out of water
	//CG_PlayerSplash( cent );

	// get the player model information
	renderfx = 0;

	if ( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson && !cg_firstPersonBody.integer && !isDecoy ) {
		renderfx |= RF_THIRD_PERSON;			// only draw in mirrors
	}
	
	/*if ( cg_shadows.integer == 3 && shadow ) {
		renderfx |= RF_SHADOW_PLANE;
	}*/

	renderfx |= RF_LIGHTING_ORIGIN;			// use the same origin for all

	//append this flag if zooming thru the TR-116.
	//It'll render players over everything else.  Not quite as good as the TV 116, but it'll do the job. :)
	if ( cg.snap->ps.weapon == WP_7 && cg.zoomed ) {
		renderfx |= RF_DEPTHHACK; //This flag makes making haxx for EF SO EASY!!!! O_O!!!!!
	}

	// if we've been hit, display proper fullscreen fx
	//CG_PlayerHitFX(cent);

	// If we are dead, set up the correct fx.
	if (cent->currentState.eFlags & EF_DEAD)
	{
		if (cent->deathTime==0)
			cent->deathTime = cg.time;		// Set Death Time so you can see yourself

		if (cent->currentState.time > cg.time)
		{	// Fading out.
			timestamp=cent->currentState.time;
			shadow = qfalse;
			shadowPlane = 0;
		}
		else
		{
			timestamp = 0;
		}
	}
	else
		cent->deathTime = 0;

	//
	// add the legs
	//
	legs.hModel = ci->legsModel;
	legs.customSkin = ci->legsSkin;

	VectorCopy( cent->lerpOrigin, legs.origin );

	//TiM - A cheap override for some things.
	//If emoting, offset the player model.  Good for solid chairs n stuff
	//CG_Printf( "Offset = %f\n", ci->modelOffset );
	if ( ci->modelOffset != 0 ) {
		if ( cent->currentState.eFlags & EF_CLAMP_BODY || cent->currentState.eFlags & EF_CLAMP_ALL || cent->currentState.eFlags & EF_ITEMPLACEHOLDER ) {
			//TiM: A bit of a hacky override.  We don't want this on ladder climbing, yet the emote code
			//now forms the basis of the ladder clamp code
			if ( (cent->currentState.legsAnim & ~ANIM_TOGGLEBIT) != BOTH_LADDER_DWN1 &&
					(cent->currentState.legsAnim & ~ANIM_TOGGLEBIT) != BOTH_LADDER_UP1 &&
					(cent->currentState.legsAnim & ~ANIM_TOGGLEBIT) != BOTH_LADDER_IDLE )
			{
				vec3_t	offset;
				vec3_t	legsYaw;

				VectorSet( legsYaw, 0, cent->pe.legs.yawAngle, 0 );

				AngleVectors( legsYaw, offset, NULL, NULL );

				VectorMA( legs.origin, ci->modelOffset, offset, legs.origin );
			}
		}
	}

	VectorCopy( cent->lerpOrigin, legs.lightingOrigin );
	//legs.shadowPlane = shadowPlane;
	legs.renderfx = renderfx;

	legs.nonNormalizedAxes = qtrue;
	VectorScale( legs.axis[0], ci->height, legs.axis[0]);
	VectorScale( legs.axis[1], (ci->height * ci->weight), legs.axis[1]); //weight... i think
	VectorScale( legs.axis[2], ci->height, legs.axis[2]);
	legs.origin[2] = legs.origin[2] - (24.0f * (1.0f - ci->height));
	//Com_Printf( "weight = %f, race = %s\n", ci->weight, ci->race);

	if ( cg.predictedPlayerState.clientNum == cent->currentState.clientNum && cg.renderingThirdPerson && cg_thirdPersonAlpha.value != 1.0 && !isDecoy ) {
		legs.renderfx |= RF_FORCE_ENT_ALPHA;
		legs.shaderRGBA[3] = (unsigned char)(alpha * 255.0f);
	}

	VectorCopy (legs.origin, legs.oldorigin);	// don't positionally lerp at all

	// Setup the param, in case it is needed.
	timeParam = cent->deathTime;
	entNum = cent->currentState.number;
	//CG_AddRefEntityWithPowerups( &legs, cent->currentState.powerups, cent->currentState.eFlags, &cent->beamData, borg );

	// if the model failed, allow the default nullmodel to be displayed
	if (!legs.hModel) {
		return;
	}

	//
	// add the torso
	//
	torso.hModel = ci->torsoModel;
	if (!torso.hModel) {
		return;
	}

	torso.customSkin = ci->torsoSkin;

	VectorCopy( cent->lerpOrigin, torso.lightingOrigin );

	CG_PositionRotatedEntityOnTag( &torso, &legs, ci->legsModel, "tag_torso" );

	torso.nonNormalizedAxes = qtrue;
	torso.shadowPlane = shadowPlane;
	torso.renderfx = renderfx;

	if ( cg.predictedPlayerState.clientNum == cent->currentState.clientNum && cg.renderingThirdPerson && cg_thirdPersonAlpha.value != 1.0 ) {
		torso.renderfx |= RF_FORCE_ENT_ALPHA;
		torso.shaderRGBA[3] = (unsigned char)(alpha * 255.0f);
	}

	//TiM - If first person rendering, offset the view vector
	if ( !isDecoy && cent->currentState.clientNum == cg.snap->ps.clientNum && cg_firstPersonBody.integer && !cg.renderingThirdPerson /*&& 
		 !( cent->currentState.eFlags & EF_CLAMP_BODY )*/ ) {
		vec3_t	legsYaw;
		vec3_t	offset;
		
		if ( cent->currentState.legsAnim != cg.fpsBody.anim ) {
			refEntity_t		ref;
			vec3_t			temp;

			CG_PositionEntityOnTag( &ref, &torso, torso.hModel, "tag_head");
			VectorSubtract( cg.refdef.vieworg, ref.origin, temp );

			//make the body smaller if need be
			//if ( cg.refdef.vieworg[2] < ref.origin[2] )
			//{
			//	cg.fpsBody.sizeOffset = (cg.refdef.vieworg[2] - legs.origin[2]) / (ref.origin[2] - legs.origin[2]);
			//}
			//else
			//	cg.fpsBody.sizeOffset = 0.0f;

			cg.fpsBody.sizeOffset = 0.9f;

			//normalize, but don't take Z-axis (vertical) into account
			//cg.fpsBody.offset = 0 + (int)sqrt( (temp[0]*temp[0]) + (temp[1]*temp[1]) );
			cg.fpsBody.offset = 2;

			cg.fpsBody.anim = cent->currentState.legsAnim ; //+15 for good measure :P
		}

		if ( cg.fpsBody.offset > 0.0f )
		{
			VectorSet( legsYaw, 0, cent->pe.legs.yawAngle, 0 );
			AngleVectors( legsYaw, offset, NULL, NULL );
			VectorMA( legs.origin, -cg.fpsBody.offset, offset, legs.origin );
			VectorMA( torso.origin, -cg.fpsBody.offset, offset, torso.origin );
		}

		if ( cg.fpsBody.sizeOffset > 0.0f )
		{
			VectorScale( legs.axis[0], cg.fpsBody.sizeOffset, legs.axis[0] );
			VectorScale( legs.axis[1], cg.fpsBody.sizeOffset, legs.axis[1] );
			VectorScale( legs.axis[2], cg.fpsBody.sizeOffset, legs.axis[2] );
			VectorScale( torso.axis[0], cg.fpsBody.sizeOffset, torso.axis[0] );
			VectorScale( torso.axis[1], cg.fpsBody.sizeOffset, torso.axis[1] );
			VectorScale( torso.axis[2], cg.fpsBody.sizeOffset, torso.axis[2] );
			
			legs.origin[2] = legs.origin[2] - (24.0f * (1.0f - cg.fpsBody.sizeOffset));
			torso.origin[2] = torso.origin[2] - (24.0f * (1.0f - cg.fpsBody.sizeOffset));
		}

		//CG_Printf( "Anim: %i Offset: %i\n", cg.fpsBody.anim, cg.fpsBody.offset );
	}

	//CG_AddRefEntityWithPowerups( &torso, cent->currentState.powerups, cent->currentState.eFlags, &cent->beamData, borg );

	//
	// add the head
	//
	head.hModel = ci->headModel;
	if (!head.hModel) {
		return;
	}

	head.customSkin = ci->headSkin;

	if ( !cg_noFrowningHeads.integer && cent->currentState.eFlags & EF_EYES_ANGRY && ci->headSkinFrown ) {
		head.customSkin = ci->headSkinFrown;
	}

	//BLINK CODE
	//if we have a valid blink skin...
	//Or we're frowning and we have a frown blink skin
	if ( (!cg_noBlinkingHeads.integer && ci->headSkinBlink) 
		|| 
			(( !cg_noFrowningHeads.integer && 
			( cent->currentState.eFlags & EF_EYES_ANGRY ) 
			&& ci->headSkinFrownBlink) ) 
		) 
	{
		
		//if time exceeded blink time and length of time to allocate blink
		//Validate that the blink time won't cause errors
		if ( ( ci->headBlinkTime.minSeconds < ci->headBlinkTime.maxSeconds ) ) {
			if ( cg.time > ci->headBlinkTime.nextTime ) {
				//if blink time has been exceeded, assign a new one
				ci->headBlinkTime.nextTime = ( irandom( ci->headBlinkTime.minSeconds, ci->headBlinkTime.maxSeconds ) * 1000 ) + cg.time;
			}
		}

		//if time has exceeded next blink time, and is still under 250 from then
		//Or if we've kicked t3h bucket and we're stone dead right now
		//or if we emoted our eyes shut
		if ( ( ( cent->currentState.eFlags & EF_EYES_SHUT ) 
				|| ( cent->currentState.eFlags & EF_DEAD )  
				|| ( cg.time < ci->headBlinkTime.nextTime && cg.time > ci->headBlinkTime.nextTime - EYES_BLINK_TIME ) )
			)
		{
			if ( cent->currentState.eFlags & EF_EYES_ANGRY ) 
			{
				if ( ci->headSkinFrownBlink )
					head.customSkin = ci->headSkinFrownBlink;
			}
			else 
			{
				if ( ci->headSkinBlink )
					head.customSkin = ci->headSkinBlink;
			}
		}
		//Com_Printf( "Max: %i, Min: %i, NextTime: %i, Time: %i\n", ci->headBlinkTime.maxSeconds, ci->headBlinkTime.minSeconds, ci->headBlinkTime.nextTime, cg.time );
	}

	//TALK CODE
	if ( !cg_noTalkingHeads.integer && (cent->currentState.eFlags & EF_TALKING) && ci->headSkinTalk[0] ) {
		if (cg.time > ci->nextTalkTime ) {

			while ( i == ci->currentTalkSkin ) {
				i = irandom( 0, 3 ); 
			}
			ci->currentTalkSkin = i;
			ci->nextTalkTime = cg.time + irandom( 100, 150 );
		}
		if ( ci->headSkinTalk[ci->currentTalkSkin] )
			head.customSkin = ci->headSkinTalk[ci->currentTalkSkin];
	}

	VectorCopy( cent->lerpOrigin, head.lightingOrigin );

	CG_PositionRotatedEntityOnTag( &head, &torso, ci->torsoModel, "tag_head" );

	head.nonNormalizedAxes = qtrue;
	head.shadowPlane = shadowPlane;
	head.renderfx = renderfx;

	if ( cg.predictedPlayerState.clientNum == cent->currentState.clientNum && cg.renderingThirdPerson && cg_thirdPersonAlpha.value != 1.0 ) {
		head.renderfx |= RF_FORCE_ENT_ALPHA;
		head.shaderRGBA[3] = (unsigned char)(alpha * 255.0f);
	}

	if ( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson && cg_firstPersonBody.integer )
		head.renderfx |= RF_THIRD_PERSON;
	//CG_AddRefEntityWithPowerups( &head, cent->currentState.powerups, cent->currentState.eFlags, &cent->beamData, borg );

	/*if ( borg && cgs.pModAssimilation )
	{
		CG_BorgEyebeam( cent, &head );
	}*/

	//player must have just uncloaked.  Do the reverse effect
	if ( !(cent->currentState.powerups & ( 1 << PW_INVIS )) && cent->wasCloaked ) {
		//play flash VFX
		if ( cent->decloakTime > 0 ) {
			if ( cg.time < cent->decloakTime + Q_FLASH_TIME ) {
				//calc the middle waypoint for the model
				vec3_t mins, maxs, headTop, avg;

				trap_R_ModelBounds( head.hModel, mins, maxs );
				VectorCopy( head.origin, headTop );
				headTop[2] += ( maxs[2] - mins[2] );

				VectorCopy( cent->lerpOrigin, mins );
				mins[2] -= 12;

				VectorCopy( headTop, maxs );

				VectorAverage( mins, maxs, avg );
				
				if(!ci->silentCloak) {
					//play the VFX
					FX_qFlash( cent, avg, cent->decloakTime );
				}
			}
			else {
				if ( cent->wasCloaked ) {
					cent->wasCloaked = qfalse;
				}
			}
		}
		
		if ( cent->decloakTime <= 0 ) {
			cent->decloakTime = cg.time;
			if(!ci->silentCloak) {
				trap_S_StartSound ( NULL, cent->currentState.clientNum, CHAN_AUTO, cgs.media.qFlash );
			}
		}
	}
	else {
		if ( cent->decloakTime >= 0 ) {
			cent->decloakTime = -1;
		}
	}

	//Get the index for when we cloaked. We'll use this to stay visible for a split second after we cloaked to enable to proper Q-Effect
	//Player just cloaked
	if ( cent->currentState.powerups & ( 1 << PW_INVIS ) ) {
		if ( cent->cloakTime > 0 && cg.time < cent->cloakTime + Q_FLASH_TIME) {
			//calc the middle waypoint for the model
			vec3_t mins, maxs, headTop, avg;

			trap_R_ModelBounds( head.hModel, mins, maxs );
			VectorCopy( head.origin, headTop );
			headTop[2] += ( maxs[2] - mins[2] );

			VectorCopy( cent->lerpOrigin, mins );
			mins[2] -= 12;

			VectorCopy( headTop, maxs );

			VectorAverage( mins, maxs, avg );
			
			if(!ci->silentCloak) {
				//play the VFX
				FX_qFlash( cent, avg, cent->cloakTime );
			}

		}
		
		if ( cent->cloakTime <= 0 ) {
			cent->wasCloaked = qtrue;
			cent->cloakTime = cg.time;
			if(!ci->silentCloak) {
				trap_S_StartSound ( NULL, cent->currentState.clientNum, CHAN_AUTO, cgs.media.qFlash );
			}
		}
	}
	else {
		if ( cent->cloakTime >= 0 )
			cent->cloakTime = -1;
	}

	//Like EF_DEAD, but when we're beaming
	//=========================================================
	if ( cent->currentState.powerups & ( 1 << PW_BEAM_OUT ) /*&& !( cent->currentState.powerups & ( 1 << PW_INVIS ) )*/ ) {
		vec3_t mins, maxs, headTop;

		if (cent->beamData.beamTimeParam == 0 && !cent->beamData.beamInDetected ) {
			cent->beamData.beamTimeParam = cg.time;
			CG_AddFullScreenEffect( SCREENFX_SP_TRANSPORTER_OUT, clientNum );
		}

		//Calculate the bounding region of head so beam flares will end there
		trap_R_ModelBounds( head.hModel, mins, maxs );
		VectorCopy( head.origin, headTop );
		headTop[2] += ( maxs[2] - mins[2] );

		if ( !(cent->currentState.powerups & ( 1 << PW_INVIS )) )
			FX_SPTransporterLensFlares( cent, headTop, cent->beamData.beamTimeParam );
	}
	else {
		if ( !cent->beamData.beamInDetected ) {
			cent->beamData.beamTimeParam = 0;
		}
	}


	if ( cent->currentState.powerups & ( 1 << PW_QUAD ) && !( cent->currentState.powerups & ( 1 << PW_INVIS ) ) ) {
		vec3_t mins, maxs, headTop;
		
		if ( cent->beamData.beamInDetected == qfalse ) {
			cent->beamData.beamTimeParam = cg.time;
			cent->beamData.beamInDetected = qtrue;
			CG_AddFullScreenEffect( SCREENFX_SP_TRANSPORTER_IN, clientNum );

			//normally player ents reset themselves... but we don't to ourselves >.<
			//so do that now
			if ( cent->currentState.clientNum == cg.snap->ps.clientNum ) {
				CG_ResetPlayerEntity( cent );
			}
		}

		trap_R_ModelBounds( head.hModel, mins, maxs );
		VectorCopy( head.origin, headTop );
		headTop[2] += ( maxs[2] - mins[2] );

		FX_SPTransporterLensFlares( cent, headTop, cent->beamData.beamTimeParam );
	}
	else {
		if ( cent->beamData.beamInDetected ) {
			cent->beamData.beamTimeParam = 0;
			cent->beamData.beamInDetected = qfalse;
		}
	}
	//=========================================================

	CG_CalcBeamAlpha( cent->currentState.powerups, &cent->beamData );

	// add the shadow
	if ( /*!(cent->currentState.eFlags & EF_ITEMPLACEHOLDER)*/ qtrue )
	{
		shadow = CG_PlayerShadow( cent, legs.origin, &shadowPlane, ci->height );
	}
	else
	{	//  - unless we are a hologram...
		shadow=qfalse;
		shadowPlane=0;
	}

	if ( cg_shadows.integer == 3 && shadow ) {
		legs.renderfx |= RF_SHADOW_PLANE;
		torso.renderfx |= RF_SHADOW_PLANE;
		head.renderfx |= RF_SHADOW_PLANE;
	}

	//TiM : Shadow calc must appear after beam code, or else we get intermittent glitches on transitioning
	legs.shadowPlane = shadowPlane;
	torso.shadowPlane = shadowPlane;
	head.shadowPlane = shadowPlane;

	//TiM - Added here so it's better in the hierarchy
	CG_PlayerSplash( cent );

	CG_AddRefEntityWithPowerups( &legs, cent->currentState.powerups, cent->currentState.eFlags, &cent->beamData, cent->cloakTime, cent->decloakTime, borg );
	CG_AddRefEntityWithPowerups( &torso, cent->currentState.powerups, cent->currentState.eFlags, &cent->beamData, cent->cloakTime, cent->decloakTime, borg );
	CG_AddRefEntityWithPowerups( &head, cent->currentState.powerups, cent->currentState.eFlags, &cent->beamData, cent->cloakTime, cent->decloakTime, borg );

	//TiM: Add player based interpolated vfx such as flashlight and the fricken laser. :)
	CG_AddBeamFX( cent );

	//
	// add the gun / barrel / flash
	//
	CG_AddPlayerWeapon( &torso, NULL, cent );

	//=============================================================================
	//TR-116 EyeScope!! :)
	//
	if ( cent->currentState.weapon == WP_7 ) 
	{
		refEntity_t		eyeScope;

		memset( &eyeScope, 0, sizeof( eyeScope ) );
		VectorCopy( cent->lerpOrigin, eyeScope.lightingOrigin );
		eyeScope.shadowPlane = shadowPlane;
		eyeScope.renderfx = renderfx;
		eyeScope.hModel = cgs.media.tr116EyeScope;

		if ( !isDecoy && cent->currentState.clientNum == cg.snap->ps.clientNum && !cg.renderingThirdPerson )
			eyeScope.renderfx |= RF_THIRD_PERSON;

		if ( eyeScope.hModel ) {
			CG_PositionEntityOnTag( &eyeScope, &head, head.hModel, "tag_eyescope" );

			if ( !VectorCompare( eyeScope.origin, head.origin ) ) {
				CG_AddRefEntityWithPowerups( &eyeScope, cent->currentState.powerups, cent->currentState.eFlags, &cent->beamData, cent->cloakTime, cent->decloakTime, borg );
			}
		}
			
	}

	//SIMS Module
	if ( cent->currentState.powerups & ( 1 << PW_FLASHLIGHT ) )
	{
		refEntity_t		flashlight;

		memset( &flashlight, 0, sizeof( flashlight ) );
		VectorCopy( cent->lerpOrigin, flashlight.lightingOrigin );
		flashlight.shadowPlane = shadowPlane;
		flashlight.renderfx = renderfx;
		flashlight.hModel = cgs.media.simsModule;

		if ( !isDecoy && cent->currentState.clientNum == cg.snap->ps.clientNum && !cg.renderingThirdPerson )
			flashlight.renderfx |= RF_THIRD_PERSON;

		if ( flashlight.hModel ) {
			CG_PositionEntityOnTag( &flashlight, &head, head.hModel, "tag_flashlight" );

			if ( !VectorCompare( flashlight.origin, head.origin ) ) {
				CG_AddRefEntityWithPowerups( &flashlight, cent->currentState.powerups, cent->currentState.eFlags, &cent->beamData, cent->cloakTime, cent->decloakTime, borg );
			}
		}
	}

	//medical scanner
	if ( cgs.classData[ci->pClass].isMedic && cent->currentState.weapon == WP_2 )
	{
		refEntity_t		scanner;

		memset( &scanner, 0, sizeof( scanner ) );
		VectorCopy( cent->lerpOrigin, scanner.lightingOrigin );
		scanner.shadowPlane = shadowPlane;
		scanner.renderfx = renderfx;
		scanner.hModel = cgs.media.medicalScanner;

		if ( scanner.hModel ) {
			CG_PositionEntityOnTag( &scanner, &torso, torso.hModel, "tag_lhand" );

			if ( !VectorCompare( scanner.origin, torso.origin ) ) {
				CG_AddRefEntityWithPowerups( &scanner, cent->currentState.powerups, cent->currentState.eFlags, &cent->beamData, cent->cloakTime, cent->decloakTime, borg );
			}
		}		
	}

	//

	//=============================================================================
	//
	// attach teh holsters
	//
	if ( cent->currentState.powerups & ( 1 << PW_BOLTON ) )
	{
		if ( ci->isHazardModel && !( !cg.renderingThirdPerson && cent->currentState.clientNum == cg.snap->ps.clientNum ) )
		{
			refEntity_t		hazardHelmet;

			memset( &hazardHelmet, 0, sizeof( hazardHelmet ) );
			VectorCopy( cent->lerpOrigin, hazardHelmet.lightingOrigin );
			hazardHelmet.shadowPlane = shadowPlane;
			hazardHelmet.renderfx = renderfx;
			hazardHelmet.hModel = cgs.media.hazardHelmet;

			if ( cgs.media.hazardHelmet )
			{
				CG_PositionEntityOnTag( &hazardHelmet, &torso, torso.hModel, "tag_head" );

				//derive new rotation axis from head model
				//else the helmet doesn't rotate with the head, wielding funny results
				AxisCopy( head.axis, hazardHelmet.axis );

				if ( !VectorCompare( hazardHelmet.origin, vec3_origin ) )
				{
					CG_AddRefEntityWithPowerups( &hazardHelmet, cent->currentState.powerups, cent->currentState.eFlags, &cent->beamData, cent->cloakTime, cent->decloakTime, borg );
				}
			}
		}
		else if ( ci->holsterModel )
		{ 
			refEntity_t		holsterTags;

			memset( &holsterTags, 0, sizeof(holsterTags) );
			VectorCopy( cent->lerpOrigin, holsterTags.lightingOrigin );
			holsterTags.shadowPlane = shadowPlane;
			holsterTags.renderfx = renderfx;
			holsterTags.hModel = ci->holsterModel;

			if ( holsterTags.hModel ) {

				CG_PositionEntityOnTag( &holsterTags, &legs, legs.hModel, "tag_torso" );

				trap_R_AddRefEntityToScene( &holsterTags );
		
				//TiM - Added an extra bit of data to entitystate, so now, the game
				//should transmit each player's weapon list here, so we can make it dynamic now

				//if player has the phaser, bolt it
				if ( cent->currentState.time2 & 1 << WP_5 ) {
					CG_AttachHolsters( cent, &holsterTags, WP_5 );
				}

				//if player has tricorder
				if ( cent->currentState.time2 & 1 << WP_2 ) {
					CG_AttachHolsters( cent, &holsterTags, WP_2 );
				}

				//if player has medkit
				if ( cent->currentState.time2 & 1 << WP_11 ) {
					CG_AttachTools( cent, &torso, WP_11 );
				}

				//if player has toolkit
				if ( cent->currentState.time2 & 1 << WP_14 ) {
					CG_AttachTools( cent, &torso, WP_14 );
				}

				/*switch ( ci->pClass ) {
					case PC_COMMAND:
					case PC_ENGINEER:
					case PC_SECURITY:
					case PC_SCIENCE:
					case PC_ADMIN:
						CG_AttachHolsters( cent, &holsterTags, WP_5 );
						break;
				}
			
				switch ( ci->pClass ) {
					case PC_ENGINEER:
					case PC_SECURITY:
					case PC_SCIENCE:
					case PC_MEDICAL:
					case PC_ADMIN:
						CG_AttachHolsters( cent, &holsterTags, WP_2 );
						break;
				}

				switch ( ci->pClass ) {
					case PC_ENGINEER:
						CG_AttachTools( cent, &torso, WP_14 );
						break;
				}

				switch ( ci->pClass ) {
					case PC_MEDICAL:
						CG_AttachTools( cent, &torso, WP_11 );
						break;
				}*/
			}
		}
	}
	//=============================================================================
	//
	//Custom Boltons

	//Com_Printf("Index: %i, Name: %s, Handle: %i\n", ci->boltonTags[0].modelBase, ci->boltonTags[0].tagName, ci->boltonTags[0].tagModel);

	//If we have boltons...
	if ( ci->boltonTags[0].tagModel ) { //We'll test by seeing if a model was cached into the first slot
		refEntity_t		boltOns[MAX_BOLTONS];
		refEntity_t*	target = NULL;
		qhandle_t		targetModel;

		for (i = 0; ( ( i < MAX_BOLTONS ) || ( ci->boltonTags[i].tagModel || ci->boltonTags[i].tagName[0] ) ); i++ ) {
			//if there's no data in there... no point to rendering it
			//ROFL... uh, we can't use .modelBase since 0 is a valid entry for that one :P
			if ( !ci->boltonTags[i].tagName[0] || !ci->boltonTags[i].tagModel ) {
				break;
			}

			memset( &boltOns[i], 0, sizeof(boltOns[i]) );
			VectorCopy( cent->lerpOrigin, boltOns[i].lightingOrigin );
			boltOns[i].renderfx = renderfx;
			boltOns[i].shadowPlane = shadowPlane;

			boltOns[i].hModel = ci->boltonTags[i].tagModel;

			//Get the necessary data for which mesh to bolt to
			switch ( ci->boltonTags[i].modelBase ){
				case BOLTON_HEAD:
					target = &head;
					targetModel = ci->headModel;
					break;
				case BOLTON_TORSO:
					target = &torso;
					targetModel = ci->torsoModel;
					break;
				case BOLTON_LEGS:
				default:
					target = &legs;
					targetModel = ci->legsModel;
					break;
			}
			CG_PositionEntityOnTag( &boltOns[i], target, targetModel, ci->boltonTags[i].tagName );

			if ( VectorCompare( boltOns[i].origin, target->origin ) ) {
				break;
			}
			CG_AddRefEntityWithPowerups( &boltOns[i], cent->currentState.powerups, cent->currentState.eFlags, &cent->beamData, cent->cloakTime, cent->decloakTime, borg );
		}
	}

	//=============================================================================
	// teh much talked about rank system :)
	//
	//

	//First, check class to determine whether we'll have default or actual rank models displayed
	if ( !cgs.classData[ci->pClass].showRanks || !ci->hasRanks /*ci->pClass == PC_NOCLASS || ci->pClass == PC_ALIEN || ci->pClass == PC_N00B*/ ) {
		if ( cgs.defaultRankData.rankModelData.boltModelPath[0] ) {
			rankModelData = &cgs.defaultRankData.rankModelData;
			rankDataValidated = qtrue;
		}
	}
	else { //Q_log2
		if ( cgs.ranksData[cg.scores[clientNum].score].rankModelData.boltModelPath[0] ) {

			//Ah... found the error with the ranks displaying wrong. Turns out cg.scores[val] is sorted by score ranking, not clientNum
			//Gotta manually find ourselves in the list.
			for ( i = 0; i < MAX_CLIENTS; i++ ) {
				if ( cg.scores[i].client == clientNum ) {
					break;
				}
			}

			//rankModelData = &cgs.ranksData[Q_log2( cg.scores[clientNum].score )].rankModelData;
			rankModelData = &cgs.ranksData[cg.scores[i].score].rankModelData;
			if ( rankModelData ) 
				rankDataValidated = qtrue;
		}
	}

	if ( !rankDataValidated || cg_noDynamicRanks.integer != 0 ) {
		return;
	}

	//Now check to see if we actually got any data
	//I've purposely created this deferred kind of rank loading
	//system to try and minimize the amount of shader slots this thing will use at one time. :P
	//No point in loading it until we use it
	if ( rankModelData->boltModelPath[0] ) {
		//if we didn't try to cache it, try now
		if ( !rankModelData->boltModel && !rankModelData->triedToLoad ) {
			rankModelData->boltModel = trap_R_RegisterModel( rankModelData->boltModelPath );

			if ( !rankModelData->boltModel ) {
				Com_Printf( S_COLOR_RED "Unable to load model file: %s\n", rankModelData->boltModelPath );
			}

			if ( rankModelData->boltShaderPath[0] ) {
				//rankModelData->boltShader = trap_R_RegisterSkin( rankModelData->boltShaderPath );
				rankModelData->boltShader = trap_R_RegisterShader( rankModelData->boltShaderPath );

				if ( !rankModelData->boltShader ) {
					Com_Printf( S_COLOR_RED "Unable to load skin: %s\n", rankModelData->boltShaderPath );
				}
			}

			rankModelData->triedToLoad = qtrue;
		}
	}

	//Now for the actual renderisation of the model

	if ( rankModelData->boltModel ) {
		refEntity_t	rankPip;

		memset( &rankPip, 0, sizeof( rankPip ) );
		VectorCopy( cent->lerpOrigin, rankPip.lightingOrigin );
		rankPip.shadowPlane = shadowPlane;
		rankPip.renderfx = renderfx;

		rankPip.hModel = rankModelData->boltModel;
		if ( rankModelData->boltShader ) {
			//rankPip.customSkin = rankModelData->boltShader;
			rankPip.customShader = rankModelData->boltShader;
		}

		CG_PositionEntityOnTag( &rankPip, &head, head.hModel, "tag_pip" );

		if ( VectorCompare( rankPip.origin, head.origin ) ) {
			return;
		}

		CG_AddRefEntityWithPowerups( &rankPip, cent->currentState.powerups, cent->currentState.eFlags, &cent->beamData, cent->cloakTime, cent->decloakTime, borg );

		//for admiral ranks, save the data and re-use, but offset origin+angles
		if ( rankModelData->admiralRank ) {
			/*refEntity_t	rankPip1;

			memset( &rankPip1, 0, sizeof( rankPip1 ) );
			VectorCopy( cent->lerpOrigin, rankPip1.lightingOrigin );
			rankPip.shadowPlane = shadowPlane;
			rankPip.renderfx = renderfx;

			rankPip1.hModel = rankModelData->boltModel;
			if ( rankModelData->boltShader ) {
				rankPip1.customSkin = rankModelData->boltShader;
			}*/

			//CG_PositionEntityOnTag( &rankPip1, &head, head.hModel, "tag_pip1" );
			CG_PositionEntityOnTag( &rankPip, &head, head.hModel, "tag_pip1" );

			if ( VectorCompare( rankPip.origin, head.origin ) ) {
				return;
			}

			CG_AddRefEntityWithPowerups( &rankPip, cent->currentState.powerups, cent->currentState.eFlags, &cent->beamData, cent->cloakTime, cent->decloakTime, borg );

		}
	}

	//TiM - try out the procedural rank quad maker
	//*sob* no normals = doesn't work :'(
	//{
	//	refEntity_t	pip;
	//	polyVert_t	verts[4];

	//	//use this to quickly get the tag porsition + orientation
	//	CG_PositionEntityOnTag( &pip, &head, head.hModel, "tag_pip" );
	//
	//	if ( VectorCompare( pip.origin, vec3_origin ) || cent->beamData.beamAlpha < 0.5f )
	//		return;

	//	//lower left
	//	verts[0].modulate[0] = 0xff;
	//	verts[0].modulate[1] = 0xff;
	//	verts[0].modulate[2] = 0xff;
	//	verts[0].modulate[3] = 0xff;
	//	verts[0].st[0]		 = 0;
	//	verts[0].st[1]		 = 1;
	//
	//	//upper left
	//	verts[1].modulate[0] = 0xff;
	//	verts[1].modulate[1] = 0xff;
	//	verts[1].modulate[2] = 0xff;
	//	verts[1].modulate[3] = 0xff;
	//	verts[1].st[0]		 = 0;
	//	verts[1].st[1]		 = 0;

	//	//upper right
	//	verts[2].modulate[0] = 0xff;
	//	verts[2].modulate[1] = 0xff;
	//	verts[2].modulate[2] = 0xff;
	//	verts[2].modulate[3] = 0xff;
	//	verts[2].st[0]		 = 1;
	//	verts[2].st[1]		 = 0;

	//	//lower right
	//	verts[3].modulate[0] = 0xff;
	//	verts[3].modulate[1] = 0xff;
	//	verts[3].modulate[2] = 0xff;
	//	verts[3].modulate[3] = 0xff;
	//	verts[3].st[0]		 = 1;
	//	verts[3].st[1]		 = 1;

	//	//offset each vert per axis
	//	//lower left
	//	VectorMA( pip.origin, -10, pip.axis[1], verts[0].xyz ); //shift the vert right
	//	VectorMA( verts[0].xyz, -5, pip.axis[2], verts[0].xyz );

	//	//upper left
	//	VectorMA( pip.origin, -10, pip.axis[1], verts[1].xyz ); //shift the vert right
	//	VectorMA( verts[1].xyz, 5, pip.axis[2], verts[1].xyz );

	//	//upper right
	//	VectorMA( pip.origin, 10, pip.axis[1], verts[2].xyz ); //shift the vert right
	//	VectorMA( verts[2].xyz, 5, pip.axis[2], verts[2].xyz );

	//	//lower right
	//	VectorMA( pip.origin, 10, pip.axis[1], verts[3].xyz ); //shift the vert right
	//	VectorMA( verts[3].xyz, -5, pip.axis[2], verts[3].xyz );

	//	trap_R_AddPolyToScene( rankModelData->boltShader, 4, verts );
	//}
}

//=====================================================================

/*
===============
CG_ResetPlayerEntity

A player just came into view or teleported, so reset all animation info
===============
*/
void CG_ResetPlayerEntity( centity_t *cent ) {
	qboolean	isDecoy=qfalse;
	
	isDecoy = (cent->currentState.eFlags & EF_ITEMPLACEHOLDER);

	cent->errorTime = -99999;		// guarantee no error decay added
	cent->extrapolated = qfalse;	

	if ( !isDecoy ) {
		CG_ClearLerpFrame( &cgs.clientinfo[ cent->currentState.clientNum ], &cent->pe.legs, cent->currentState.legsAnim );
		CG_ClearLerpFrame( &cgs.clientinfo[ cent->currentState.clientNum ], &cent->pe.torso, cent->currentState.torsoAnim );
	}
	else {
		CG_ClearLerpFrame( &cgs.decoyInfo[ cent->currentState.eventParm ], &cent->pe.legs, cent->currentState.legsAnim );
		CG_ClearLerpFrame( &cgs.decoyInfo[ cent->currentState.eventParm ], &cent->pe.torso, cent->currentState.torsoAnim );
	}

	BG_EvaluateTrajectory( &cent->currentState.pos, cg.time, cent->lerpOrigin );
	BG_EvaluateTrajectory( &cent->currentState.apos, cg.time, cent->lerpAngles );

	VectorCopy( cent->lerpOrigin, cent->rawOrigin );
	VectorCopy( cent->lerpAngles, cent->rawAngles );

	//TiM - Only do this if not emoting.  Otherwise, if we go away, then come back, clamped players face different directions O_O
	//ignore this for decoys too. they're static, so this won't affect them anyway

	if ( !isDecoy && ( ( cent->currentState.eFlags & EF_CLAMP_BODY) || ( cent->currentState.eFlags & EF_CLAMP_ALL) ) ) {
		//blah
	}
	else {
		memset( &cent->pe.legs, 0, sizeof( cent->pe.legs ) );
		cent->pe.legs.yawAngle = cent->rawAngles[YAW];
		cent->pe.legs.yawing = qfalse;
		cent->pe.legs.pitchAngle = 0;
		cent->pe.legs.pitching = qfalse;

		memset( &cent->pe.torso, 0, sizeof( cent->pe.legs ) );
		cent->pe.torso.yawAngle = cent->rawAngles[YAW];
		cent->pe.torso.yawing = qfalse;
		cent->pe.torso.pitchAngle = cent->rawAngles[PITCH];
		cent->pe.torso.pitching = qfalse;
	}

	if ( cg_debugPosition.integer ) {
		CG_Printf("%i ResetPlayerEntity yaw=%i\n", cent->currentState.number, cent->pe.torso.yawAngle );
	}
}

