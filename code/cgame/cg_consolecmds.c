/*
 *
 * Copyright (C) 1999-2000 Id Software, Inc.
 *
 * cg_consolecmds.c -- text commands typed in at the local console, or
 * executed by a key binding
 */

#include "cg_local.h"

#define	 emotesDEF

static void CG_ObjectivesDown_f( void ) {
	cg.showObjectives = qtrue;
}

static void CG_ObjectivesUp_f( void ) 
{
	cg.showObjectives = qfalse;
}

void CG_TargetCommand_f( void ) {
	int32_t		targetNum;
	char	test[4];

	targetNum = CG_CrosshairPlayer();
	if (!targetNum ) {
		return;
	}

	trap_Argv( 1, test, 4 );
	trap_SendConsoleCommand( va( "gc %i %i", targetNum, atoi( test ) ) );
}



/*
=================
CG_SizeUp_f

Keybinding command
=================
*/
static void CG_SizeUp_f (void) {
	trap_Cvar_Set("cg_viewsize", va("%i",(int32_t)(cg_viewsize.integer+10)));
}


/*
=================
CG_SizeDown_f

Keybinding command
=================
*/
static void CG_SizeDown_f (void) {
	trap_Cvar_Set("cg_viewsize", va("%i",(int32_t)(cg_viewsize.integer-10)));
}


/*
=============
CG_Viewpos_f

Debugging command to print the current view position
=============
*/
static void CG_Viewpos_f (void) {
	CG_Printf ("%s (%i %i %i) : %i\n", cgs.mapname, (int32_t)cg.refdef.vieworg[0],
		(int32_t)cg.refdef.vieworg[1], (int32_t)cg.refdef.vieworg[2], 
		(int32_t)cg.refdefViewAngles[YAW]);
}

/*
=============
CG_ClientPos_f

Debugging command to print the current client position
=============
*/
static void CG_ClientPos_f ( void )
{
	CG_Printf( "OUTPUT: %s | ORIGIN( %f %f %f ) | ANGLES( %f %f %f )\n", cgs.mapname, 
																			cg.snap->ps.origin[0],
																			cg.snap->ps.origin[1],
																			cg.snap->ps.origin[2],
																			cg.snap->ps.viewangles[0],
																			cg.snap->ps.viewangles[1],
																			cg.snap->ps.viewangles[2] );
}

static void CG_ScoresDown_f( void ) {
	if ( cg.scoresRequestTime + 2000 < cg.time ) {
		/* the scores are more than two seconds out of data,
		   so request new ones */
		cg.scoresRequestTime = cg.time;
		trap_SendClientCommand( "score" );

		/* leave the current scores up if they were already
		   displayed, but if this is the first hit, clear them out */
		if ( !cg.showScores ) {
			cg.showScores = qtrue;
			cg.numScores = 0;
		}
	} else {
		/* show the cached contents even if they just pressed if it
		   is within two seconds */
		cg.showScores = qtrue;
	}
}

static void CG_ScoresUp_f( void ) {
	cg.showScores = qfalse;
	cg.scoreFadeTime = cg.time;
}

static void CG_TellTarget_f( void ) {
	int32_t		clientNum;
	char	command[128];
	char	message[128];

	clientNum = CG_CrosshairPlayer();
	if ( clientNum == -1 ) {
		return;
	}

	trap_Args( message, 128 );
	Com_sprintf( command, 128, "tell %i %s", clientNum, message );
	trap_SendClientCommand( command );
}

static void CG_TellAttacker_f( void ) {
	int32_t		clientNum;
	char	command[128];
	char	message[128];

	clientNum = CG_LastAttacker();
	if ( clientNum == -1 ) {
		return;
	}

	trap_Args( message, 128 );
	Com_sprintf( command, 128, "tell %i %s", clientNum, message );
	trap_SendClientCommand( command );
}

void CG_Cough_cmd( void )
{
	CG_Printf("                       ,.                      \n");	CG_Printf("          ..:,        :Xt.       ,:.            \n");
    CG_Printf("         ,=+t:       .IRX=       :++=.         \n");	CG_Printf("        .=iVt:.      :RYYI.      .itt+         \n");    
	CG_Printf("       .:tXI=;.      tRtiV;       ,IYY:.       \n");	CG_Printf("      .+;ii=;.      ,XVi+Vt.       :tIi+      \n");    
	CG_Printf("     .;ti;;:.       +RI++IY,        ,+tt=.     \n");	CG_Printf("    ,++YY;.        ,XXi+++X=         ;IYI=.    \n");    
	CG_Printf("    ;ttY+;.    .,=iVRI++++YX+;.       ;VYt;    \n");	CG_Printf("   .;ii+=,   .;IXRRXVi++++iVRXVi:.    ,=iii.   \n");    
	CG_Printf("  .==;ti,  .;YRRVVXYii+++++IVIVRXt,   ,+=tI=   \n");	CG_Printf("  .iitY=, .tRRVXXVRV+++ii++YRXVIYXV;   :tYti,  \n");   
	CG_Printf("  .+iii=,,IBVVXYiiXViiiiiiitVtIXViVR=  ,+t+I:  \n");	CG_Printf("   =+=I:.tBVXVt=;tRIiiiiiiiiXi:=YXiIX; :+=It;  \n");    
	CG_Printf(" .;;tYt:;RVVV+=:,YRiiiiiiiiiYI,.:IXiVY..+IYi=  \n");	CG_Printf(" .ti=t+;tRIXi;, :XViiiiiiiiiIV:  ,YViX=.:titt. \n");
	CG_Printf("  iY++I;YVYY=:  +BIiiiiiiiiiiX=   +XiVi;i++Vi, \n"); CG_Printf(" ,+YYYI:VYYY;. .YRiiiiiiiiiiiVt.  ;RIYt:IIVVi: \n");	
	CG_Printf(" ,+tYXi;YVIX;  ;RVtiiiiIXXtiiVI,  iRIVt,=XVit: \n"); CG_Printf(" .+iiti++XiXI. iBIiiiiYXIIXtiIV: :XXIV++;i+iI;.\n");
	CG_Printf("  ;Ii=ii:VYtRi,VRtiiiVVi=;IXitX=;VBYXI=i+;iV+;.\n"); CG_Printf("  ;tYtVt;;XYIRXBVttiVV+;:.:VYiXVRBVXY+;+IYVt+, \n");	
	CG_Printf("  =iiItii,=XVIRRIttXV+=:..,tRtVBXVRI+=i:iIit+. \n"); CG_Printf("  :t==++I:.=YXYIIiYBXYIttIVRBYtVXXI+;;t+;;+Y=, \n");
	CG_Printf("   +I=;+Y= .:IRItYIVXRRRBBRXXVIRY+=;.:i=;iVi;. \n"); CG_Printf("   .+IYVV+:  +BYXXVXXXXXXXXXVRVVi;:.:;tVYY+=:  \n");	
	CG_Printf("    .+ttii+ .IBXY++ittIIIti++tXXi, .++=tI+;:   \n"); CG_Printf("     ;YYtIY;;VBI+;:,::;;;;;:,:IBt,::tItYV=.    \n");
	CG_Printf("      =IYYI++ti+;,   .......  :Xt;i=iYYI+;.    \n"); CG_Printf("      .:+i++ii;;.             .=i=+i=t+;;:.    \n");
	CG_Printf("        ,tYIVI==:,..       ..,;=+iYIVt:..      \n"); CG_Printf("         ,itt+iIYYti;.   ,;itYIIt:iIi=;.       \n");
	CG_Printf("          .:;;:+tIIVIi:.;iYYIii+=:,;;:.        \n"); CG_Printf("            .  ,:=itIXi.tXYit=;::,  .          \n");	
	CG_Printf("                 .+tti=,,iIt+;.                \n"); CG_Printf("                  .:;;:. ,;;;:.                \n");	
}

/*
=========================
CG_RankList_cmd
TiM: Scans the rank struct, and gets
the names of all the ranks we can use ATM
=========================
*/
void CG_RankList_cmd( void ) {
	int32_t i;

	/* Print Titles */
	CG_Printf( S_COLOR_CYAN "RPG-X: Available Ranks\n");
	CG_Printf( S_COLOR_GREEN "Console Name \t - \t Formal Name\n" );

	/* Loop thru each val and print them */
	for ( i = 0; i < MAX_RANKS; i++ ) {		
		if ( cgs.ranksData[i].consoleName[0] ) 
			CG_Printf( "%s \t - \t %s\n", cgs.ranksData[i].consoleName, cgs.ranksData[i].formalName );
		else
			break;
	}
}

/*
=========================
CG_ClassList_cmd
TiM: Scans the class struct, and gets
the names of all the ranks we can use ATM
=========================
*/
void CG_ClassList_cmd( void ) {
	int32_t i;

	/* Print Titles */
	CG_Printf( S_COLOR_CYAN "RPG-X: Available Classes\n");
	CG_Printf( S_COLOR_GREEN "Formal Name\n" );

	/* Loop thru each val and print them */
	for ( i = 0; i < MAX_CLASSES; i++ ) {		
		if ( cgs.classData[i].formalName[0] ) 
			CG_Printf( "%s\n", cgs.classData[i].formalName );
		else
			break;
	}
}

/*
=========================
CG_BeamList_cmd
TiM: Returns a list showing
the index of each target_location
ent so people know the data needed
to beam to various locations.
=========================
*/
void CG_BeamList_cmd( void ) {
	const char *locStr;
	int32_t i;

	/* Print Titles */
	CG_Printf( S_COLOR_CYAN "RPG-X Current Beam Locations\n" );
	CG_Printf( S_COLOR_GREEN "Location Name \t - \t Location Index\n" );

	/* Based off the string data that is transmitted to the CG on Init
	   Get the name and index of each location */
	for ( i = 1; i < MAX_LOCATIONS; i++ ) {
		locStr = CG_ConfigString( CS_LOCATIONS + i );

		if ( locStr[0] ) {
			CG_Printf( "%s \t - \t%i\n", locStr, i );
		}
		locStr = NULL; /* reset just in case */
	}
}

/*
=========================
CG_Emote_f
TiM: The first portion of the
emote system.  While JKA works
by storing a copy of animations 
on the server (something we cannot
replicate easily here without
destroying user freedom futhur >.<), so it
automatically knows the length of time
each anim should run for, here, we'll
hacikly override this by calculating the 
run length on the client as they have the data, 
and then transmitting it to the server. 
Hacky, I know.

And if other players don't have the same model,
it could potentially show animation glitches.
Although not as bad as the alternative...
=========================
*/

void CG_Emote_f( void ) {
	const char		*argStr;
	emoteList_t		*emote = NULL;
	int32_t				i;
	animation_t		*anims;
	int32_t				animLength;
	qboolean		emoteFound=qfalse;

	argStr = CG_Argv( 1 );
	if ( !argStr[0] ) {
		CG_Printf( S_COLOR_RED "ERROR: No emote specified.\n" );
		return;
	}

	/* TiM: Hack override for eyes shut, angry eyes and alert mode.
	   No more data is needed */
	if ( !Q_stricmp( argStr, "eyes_shut" ) || !Q_stricmp( argStr, "eyes_frown" ) || !Q_stricmpn( argStr, "alert", 5 ) || !Q_stricmpn( argStr, "alert2", 6 ) ) 
	{
		trap_SendClientCommand( va( "doEmote %s", argStr ) );
		return;
	}

	if ( cg.predictedPlayerEntity.currentState.eFlags & EF_DEAD ) {
		CG_Printf( S_COLOR_RED "ERROR: Cannot play emotes when dead.\n" );
		return;
	} 

	/* find out emote in the list
	   value of numEmotes calced in bg_misc.c
	   or if an int32_t was supplied as an arg, use that */
	for ( i = 0; i < bg_numEmotes; i++ ) 
	{ 
		emote = &bg_emoteList[i];

		if ( emote && !Q_stricmp( emote->name, argStr ) ) 
		{
			emoteFound = qtrue;
			break;
		}
	}

	if ( !emoteFound ) {
		CG_Printf( S_COLOR_RED "ERROR: Specified emote not found\n" );
		return;
	}

	anims = &cg_animsList[cgs.clientinfo[ cg.predictedPlayerState.clientNum ].animIndex].animations[ emote->enumName ];
	
	/* if anim num less than 0, then this is a stub anim */
	if ( !anims || anims->numFrames < 0 ) {
		CG_Printf( S_COLOR_RED "ERROR: Current character cannot perform that emote.\n" );
		return;
	}

	/* Anim length for lower model */
	if ( !( emote->animFlags & EMOTE_LOOP_UPPER ) && !( emote->animFlags & EMOTE_LOOP_LOWER ) ) {
		animLength = anims->numFrames * anims->frameLerp;
	}
	else {
		animLength = -1;
	}

	/* send the command to the server */
	trap_SendClientCommand( va( "doEmote %i %i", i, animLength ) );

	/* add this emote to the emotes recently played menu */
	{
		int32_t j;
		char* cvar;
		char buffer[256];
		qboolean foundSlot=qfalse;

		for ( j = 1; j <= NUM_CVAR_STORES; j++ ) {
			cvar = va( "ui_recentEmote%i", j );

			/* found a free slot */
			trap_Cvar_VariableStringBuffer( cvar, buffer, 256 );

			/* oh this emote's already here... no point adding it again */
			if ( atoi(buffer) == i ) {
				foundSlot = qtrue;
				break;				
			}

			if ( atoi(buffer) == -1 ) {
				trap_Cvar_Set( cvar, va( "%i", i ) );
				foundSlot = qtrue;
				break;
			}
		}

		/* whup... no slot found. better push them all forward one */
		if ( !foundSlot ) {
			char* cvar2;

			for ( j = 2; j <= NUM_CVAR_STORES; j++ ) {
				cvar = va( "ui_recentEmote%i", j-1 );
				cvar2 = va( "ui_recentEmote%i", j );
				
				trap_Cvar_VariableStringBuffer( cvar2, buffer, 256 );
				trap_Cvar_Set( cvar, va( "%i", atoi(buffer) ) );

				if ( j == NUM_CVAR_STORES ) {
					cvar = va( "ui_recentEmote%i", NUM_CVAR_STORES );
					trap_Cvar_Set( cvar, va( "%i", i ) );
				}
			}
		}
	}
}


/*
=========================
CG_LocEdit_f
=========================
*/
static fileHandle_t f;
void CG_LocEdit_f(void) {
	char path[MAX_QPATH];
	char buffer[MAX_STRING_CHARS];
	const char *argptr;

	argptr = CG_Argv(1);

	if(!Q_stricmpn(argptr, "start", 5)) {
		Com_sprintf(path, sizeof(path), "%s", cgs.mapname);

		COM_StripExtension(path, path);
		Com_sprintf(path, sizeof(path), "%s.locations", path);

		trap_FS_FOpenFile(path, &f, FS_READ);

		if(f) {
			CG_Printf(S_COLOR_RED "locedit: %s.locations already exist! Skipping.\n", path);
			trap_FS_FCloseFile(f);
			return;
		}

		trap_FS_FOpenFile(path, &f, FS_APPEND);

		if(f) {
			if((argptr = CG_Argv(2)) != NULL) {
				int32_t i = atoi(argptr);
				if(i) {
					trap_FS_Write("LocationsList2\n", 15, f);
				} else {
					trap_FS_Write("LocationsList\n", 14, f);
				}
				trap_FS_Write("{\n", 2, f);
				CG_Printf(S_COLOR_YELLOW "locedit: file created...\n");
				CG_Printf(S_COLOR_YELLOW "locedit: writing file header...\n");
			} else {
				CG_Printf(S_COLOR_RED "locedit: insufficent number of arguments.\n");
				trap_FS_FCloseFile(f);
				return;
			}
		}
	} else if(!Q_stricmpn(argptr, "stop", 4)) {
		if(!f) {
			CG_Printf(S_COLOR_RED "locedit: no locations file loaded!\n");
			return;
		}
		trap_FS_Write("}", 1, f);
		CG_Printf(S_COLOR_YELLOW "locedit: writing file end...\n");
		trap_FS_FCloseFile(f);
		CG_Printf(S_COLOR_YELLOW "locedit: closed file.\n");
	} else if(!Q_stricmpn(argptr, "add", 3)) {
		if(!f) {
			CG_Printf(S_COLOR_RED "locedit: no locations file loaded!\n");
			return;
		}

		memset(buffer, 0, sizeof(buffer));

		Com_sprintf(buffer, sizeof(buffer), "\t{ %f, %f, %f } { 0, %f, 0 } ", floor(cg.snap->ps.origin[0]),
																				floor(cg.snap->ps.origin[1]),
																				floor(cg.snap->ps.origin[2]),
																				floor(cg.snap->ps.viewangles[1] - 24.0f));

		argptr = CG_Argv(2);
		Com_sprintf(buffer, sizeof(buffer), "%s \"%s\"", buffer, argptr);
		argptr = CG_Argv(3);
		if(argptr) {
			Com_sprintf(buffer, sizeof(buffer), "%s \"%s\"", buffer, argptr);
		}
		Com_sprintf(buffer, sizeof(buffer), "%s;\n", buffer);
		trap_FS_Write(buffer, strlen(buffer), f);
		CG_Printf(S_COLOR_YELLOW "locedit - added location: %s\n", buffer);
	} else if(!Q_stricmpn(argptr, "nl", 2)) {
		if(!f) {
			CG_Printf(S_COLOR_RED "locedit: no locations file loaded!\n");
			return;
		}

		trap_FS_Write("\n", 1, f);
		CG_Printf(S_COLOR_YELLOW "locedit: added an empty line.\n");
	}
}

/*=================================================================================
Third Person Camera View Commands
TiM : These commands activate code in the thirdperson rendering code to let players
zoom around their characters smoothly.
==================================================================================*/

void CG_ThirdPersonForwardDown_f ( void ) {
	if ( !cg.zoomedForward ) {
		cg.zoomedForward = qtrue;
	}
}
void CG_ThirdPersonForwardUp_f ( void ) {
	if ( cg.zoomedForward ) {
		cg.zoomedForward = qfalse;
	}
}



void CG_ThirdPersonBackwardDown_f ( void ) {
	if ( !cg.zoomedBackward ) {
		cg.zoomedBackward = qtrue;
	}
}
void CG_ThirdPersonBackwardUp_f ( void ) {
	if ( cg.zoomedBackward ) {
		cg.zoomedBackward = qfalse;
	}
}



void CG_ThirdPersonLeftDown_f ( void ) {
	if ( !cg.zoomedLeft ) {
		cg.zoomedLeft = qtrue;
	}
}
void CG_ThirdPersonLeftUp_f ( void ) {
	if ( cg.zoomedLeft ) {
		cg.zoomedLeft = qfalse;
	}
}



void CG_ThirdPersonRightDown_f ( void ) {
	if ( !cg.zoomedRight ) {
		cg.zoomedRight = qtrue;
	}
}
void CG_ThirdPersonRightUp_f ( void ) {
	if ( cg.zoomedRight ) {
		cg.zoomedRight = qfalse;
	}
}



void CG_ThirdPersonUpDown_f ( void ) {
	if ( !cg.zoomedUp ) {
		cg.zoomedUp = qtrue;
	}
}
void CG_ThirdPersonUpUp_f ( void ) {
	if ( cg.zoomedUp ) {
		cg.zoomedUp = qfalse;
	}
}



void CG_ThirdPersonDownDown_f ( void ) {
	if ( !cg.zoomedDown ) {
		cg.zoomedDown = qtrue;
	}
}
void CG_ThirdPersonDownUp_f ( void ) {
	if ( cg.zoomedDown ) {
		cg.zoomedDown = qfalse;
	}
}



void CG_ThirdPersonAngleLeftDown_f ( void ) {
	if ( !cg.zoomAngleLeft ) {
		cg.zoomAngleLeft = qtrue;
	}
}
void CG_ThirdPersonAngleLeftUp_f ( void ) {
	if ( cg.zoomAngleLeft ) {
		cg.zoomAngleLeft = qfalse;
	}
}



void CG_ThirdPersonAngleRightDown_f ( void ) {
	if ( !cg.zoomAngleRight ) {
		cg.zoomAngleRight = qtrue;
	}
}
void CG_ThirdPersonAngleRightUp_f ( void ) {
	if ( cg.zoomAngleRight ) {
		cg.zoomAngleRight = qfalse;
	}
}



void CG_ThirdPersonPitchDownDown_f ( void ) {
	if ( !cg.zoomPitchDown ) {
		cg.zoomPitchDown = qtrue;
	}
}
void CG_ThirdPersonPitchDownUp_f ( void ) {
	if ( cg.zoomPitchDown ) {
		cg.zoomPitchDown = qfalse;
	}
}



void CG_ThirdPersonPitchUpDown_f ( void ) {
	if ( !cg.zoomPitchUp ) {
		cg.zoomPitchUp = qtrue;
	}
}
void CG_ThirdPersonPitchUpUp_f ( void ) {
	if ( cg.zoomPitchUp ) {
		cg.zoomPitchUp = qfalse;
	}
}


/*------------------------------------------------------------------------------*/

const char*		cVars[] = { "cg_thirdPersonRange", "cg_thirdPersonAngle",
						"cg_thirdPersonVertOffset", "cg_thirdPersonHorzOffset",
						"cg_thirdPersonPitchOffset" };

vmCvar_t*		TPSVars[] = { &cg_thirdPersonRange, &cg_thirdPersonAngle, 
						&cg_thirdPersonVertOffset, &cg_thirdPersonHorzOffset,
						&cg_thirdPersonPitchOffset };

/* Set the third person values back to their hard-coded CVAR counter parts
   Ie revert any temporary changes. */
void CG_ThirdPersonRevert_f ( void ) {

	int32_t i;
	char	value[MAX_TOKEN_CHARS];

	for (i = 0; i < 5; i++ ){
		trap_Cvar_VariableStringBuffer ( cVars[i], value, sizeof( value ) );
		TPSVars[i]->value = atof( value );
	}
}

/*
 * TiM : If the default values for these CVARs are changed in cg_main.c, update them here.
 * I would consider linking directly to the values, but with scope, and then locating them in the
 * struct array, this is way faster.
 * Resets the values to the game's defaults. Useful if you screwed up the view big time.
 */
void CG_ThirdPersonReset_f ( void ) {
	int32_t	defValues[] = { 80, 0, 16, 0, 0 };
	int32_t i;

	for (i = 0; i < 5; i++ ) {
		TPSVars[i]->value = defValues[i];
		trap_Cvar_Set( cVars[i], va( "%i", defValues[i] ) );
	}
}

/*
 * Takes the current values from all of the thirdperson pointer variables, and sets the
 * hard coded CVARs to the same value, effectively making them permanent
 */
void CG_ThirdPersonCommit_f ( void ) {
	int32_t i;

	for (i = 0; i < 5; i++ ) {
		trap_Cvar_Set( cVars[i], va("%f", TPSVars[i]->value ) );
	}
	CG_Printf( "Current Third Person CVAR values committed.\n" ); 
	//since no screen changes occur. Let the user know something happened.
}

/* Toggles between first and third person */
void CG_ToggleThirdPerson_f ( void ) {
	int32_t value;

	value = !( cg_thirdPerson.integer > 0 ); /* This is cool. It'll toggle the value each call. */

	trap_Cvar_Set( "cg_thirdPerson", va( "%i", value ) );
}

void CG_SqlMenu_f(void) {
	trap_SendConsoleCommand("ui_sqlmenu");
}

/*================================================================================*/

typedef struct {
	char	*cmd;
	void	(*function)(void);
} consoleCommand_t;

static consoleCommand_t	commands[] = {
	{ "testgun",				CG_TestGun_f },
	{ "testmodel",				CG_TestModel_f },
	{ "nextframe",				CG_TestModelNextFrame_f },
	{ "prevframe",				CG_TestModelPrevFrame_f },
	{ "nextskin",				CG_TestModelNextSkin_f },
	{ "prevskin",				CG_TestModelPrevSkin_f },
	{ "viewpos",				CG_Viewpos_f },
	{ "+info",					CG_ScoresDown_f },
	{ "-info",					CG_ScoresUp_f },
	{ "+zoom",					CG_ZoomDown_f },
	{ "-zoom",					CG_ZoomUp_f },

	/* TiM : Modelview code */
	{ "+thirdPersonForward",	CG_ThirdPersonForwardDown_f },		/* moving the camera forward */
	{ "-thirdPersonForward",	CG_ThirdPersonForwardUp_f }, 
	{ "+thirdPersonBackward",	CG_ThirdPersonBackwardDown_f },		/* moving the camera backward */
	{ "-thirdPersonBackward",	CG_ThirdPersonBackwardUp_f },
	{ "+thirdPersonLeft",		CG_ThirdPersonLeftDown_f },			/* moving the camera left */
	{ "-thirdPersonLeft",		CG_ThirdPersonLeftUp_f },
	{ "+thirdPersonRight",		CG_ThirdPersonRightDown_f },		/* moving the camera right */
	{ "-thirdPersonRight",		CG_ThirdPersonRightUp_f },
	{ "+thirdPersonUp",			CG_ThirdPersonUpDown_f },			/* moving the camera up */
	{ "-thirdPersonUp",			CG_ThirdPersonUpUp_f },
	{ "+thirdPersonDown",		CG_ThirdPersonDownDown_f },			/* moving the camera down */
	{ "-thirdPersonDown",		CG_ThirdPersonDownUp_f },
	{ "+thirdPersonAngleLeft",	CG_ThirdPersonAngleLeftDown_f },	/* rotating the camera left */
	{ "-thirdPersonAngleLeft",	CG_ThirdPersonAngleLeftUp_f },
	{ "+thirdPersonAngleRight",	CG_ThirdPersonAngleRightDown_f },	/* rotating the camera right */
	{ "-thirdPersonAngleRight",	CG_ThirdPersonAngleRightUp_f },
	{ "+thirdPersonPitchDown",	CG_ThirdPersonPitchDownDown_f },	/* pitching the camera down */
	{ "-thirdPersonPitchDown",	CG_ThirdPersonPitchDownUp_f },
	{ "+thirdPersonPitchUp",	CG_ThirdPersonPitchUpDown_f },		/* pitching the camera up */
	{ "-thirdPersonPitchUp",	CG_ThirdPersonPitchUpUp_f },
	{ "thirdPersonRevert",		CG_ThirdPersonRevert_f },			/* revert current view to previous settings */
	{ "thirdPersonReset",		CG_ThirdPersonReset_f },			/* reset values to CVAR defaults */
	{ "thirdPersonCommit",		CG_ThirdPersonCommit_f },			/* set CVARs to current settings */
	{ "thirdPerson",			CG_ToggleThirdPerson_f },			/* Toggle the 3rd persn CVAR */
	/* TiM */

	{ "clientPos",				CG_ClientPos_f },
	{ "sizeup",					CG_SizeUp_f },
	{ "sizedown",				CG_SizeDown_f },
	{ "weapnext",				CG_NextWeapon_f },
	{ "weapprev",				CG_PrevWeapon_f },
	{ "weapon",					CG_Weapon_f },
	{ "tell_target",			CG_TellTarget_f },
	{ "tell_attacker",			CG_TellAttacker_f },
	{ "tcmd",					CG_TargetCommand_f },
	{ "loaddefered",			CG_LoadDeferredPlayers },			/* spelled wrong, but not changing for demo... */
	{ "+analysis",				CG_ObjectivesDown_f },
	{ "-analysis",				CG_ObjectivesUp_f },
	{ "iloverpg-x",				CG_Cough_cmd },
	{ "rankList",				CG_RankList_cmd },
	{ "locationList",			CG_BeamList_cmd },
	{ "classList",				CG_ClassList_cmd },
	{ "emote",					CG_Emote_f },
	{ "locedit",				CG_LocEdit_f },
	{ "sqlmenu",				CG_SqlMenu_f },
};


/*
=================
CG_ConsoleCommand

The string has been tokenized and can be retrieved with
Cmd_Argc() / Cmd_Argv()
=================
*/
qboolean CG_ConsoleCommand( void ) {
	const char	*cmd;
	int32_t		i;

	cmd = CG_Argv(0);

	for ( i = 0 ; i < sizeof( commands ) / sizeof( commands[0] ) ; i++ ) {
		if ( !Q_stricmp( cmd, commands[i].cmd ) ) {
			commands[i].function();
			return qtrue;
		}
	}

	return qfalse;
}

/*
=================
CG_InitConsoleCommands

Let the client system know about all of our commands
so it can perform tab completion
=================
*/
void CG_InitConsoleCommands( void ) {
	int32_t		i;

	for ( i = 0 ; i < sizeof( commands ) / sizeof( commands[0] ) ; i++ ) {
		trap_AddCommand( commands[i].cmd );
	}

	/*
	 * the game server will interpret these commands, which will be automatically
	 * forwarded to the server after they are not recognized locally
	 *
	 *	TiM: This trap command also adds the commands to the 'tab list' that users can
	 *	use thru the console, so adding any and all game side commands (that we want the users to know about/access of course lol ;P )
	 *	would be a good idea too. :)
	 */
	trap_AddCommand ("kill");
	trap_AddCommand ("say");
	trap_AddCommand ("say_team");
	/* START MOD */
	trap_AddCommand ("say_class");
	trap_AddCommand ("forceName");
	trap_AddCommand ("forceKill");
	trap_AddCommand ("forceKillRadius");
	trap_AddCommand ("forceClass");
	trap_AddCommand ("kickTarget");
	/* END MOD */
	trap_AddCommand ("give");
	trap_AddCommand ("god");
	trap_AddCommand ("notarget");
	trap_AddCommand ("noclip");
	trap_AddCommand ("team");
	trap_AddCommand ("class");
	trap_AddCommand ("follow");
	trap_AddCommand ("levelshot");
	trap_AddCommand ("addbot");
	trap_AddCommand ("setviewpos");
	trap_AddCommand ("vote");
	trap_AddCommand ("callvote");
	trap_AddCommand ("loaddeferred");	/* spelled wrong, but not changing for demo */

	/*TiM - uh START MOD AGAIN */
	trap_AddCommand("laser");
	trap_AddCommand("flashlight");
	trap_AddCommand("cloak");
	trap_AddCommand("flight");
	trap_AddCommand("EVASuit");
	trap_AddCommand("forceName");
	trap_AddCommand("forceKill");
	trap_AddCommand("forceKillRadius");
	trap_AddCommand("shake");
	trap_AddCommand("drag");
	trap_AddCommand("undrag");
	trap_AddCommand("flushTripmines"); /* disarm_tripmines */
	trap_AddCommand("rank");
	trap_AddCommand("forceRank");
	trap_AddCommand("forceModel");
	trap_AddCommand("forcePlayer");
	trap_AddCommand("adminLogin");
	trap_AddCommand("adminList");
	trap_AddCommand("revive");
	trap_AddCommand("n00b");
	trap_AddCommand("msg");
	trap_AddCommand("playMusic");
	trap_AddCommand("stopMusic");
	trap_AddCommand("playSound");
	trap_AddCommand("fxGun");
	trap_AddCommand("flushFX");
	trap_AddCommand("clampInfo");
	trap_AddCommand("spawnChar");
	trap_AddCommand("flushChars");
	trap_AddCommand("flushEmote");
	trap_AddCommand("beamToPlayer");
	trap_AddCommand("beamToLocation");

	trap_AddCommand("kick2");
	trap_AddCommand("botlist");
	trap_AddCommand("addip");
	trap_AddCommand("removeip");
	trap_AddCommand("listip");
	trap_AddCommand("game_memory");
	trap_AddCommand("entitylist");
	trap_AddCommand("useEnt" );

	trap_AddCommand("banUser");
	trap_AddCommand("findID");
	trap_AddCommand("removeID");

	trap_AddCommand("me");
	trap_AddCommand("meLocal");

	trap_AddCommand("mapsList");

	/*
	 * END MOD AGAIN
	 * TiM - May I just say. WOAH! THAT'S A LOT!! O_O!
	 */

    /*
     * START MOD AGAIN - xD
     * by Marcin - 04/12/2008
     */
    trap_AddCommand("drop");
    trap_AddCommand("flushDropped");
    /* END MOD */

	/*
	 * START MOD ANOTHER TIME xD
	 * GSIO01 | 08/05/2009
	 */
	trap_AddCommand("lock");
	trap_AddCommand("ffColor");
	trap_AddCommand("remodulate");
	trap_AddCommand("unlockAll");
	trap_AddCommand("lockAll");
	trap_AddCommand("changeFreq");
	trap_AddCommand("alert");
	trap_AddCommand("msg2");
	trap_AddCommand("forcevote");
	trap_AddCommand("listSPs");
	trap_AddCommand("getEntInfo");
	trap_AddCommand("getOrigin");
	trap_AddCommand("getEntByTargetname");
	trap_AddCommand("getEntByTarget");
	trap_AddCommand("getEntByBmodel");
	trap_AddCommand("setOrigin");
	trap_AddCommand("getBrushEntCount");
	trap_AddCommand("findEntitiesInRadius");
	trap_AddCommand("spawnTEnt");
	trap_AddCommand("flushTEnts");

	/* temp */
	trap_AddCommand("ui_holodeck");

	/* sql */
	/* TODO some might be removed */
	trap_AddCommand("userlogin");
	trap_AddCommand("userAdd");
	trap_AddCommand("sql_setup");
	trap_AddCommand("userMod");
	trap_AddCommand("userDel");

	/* lua */
	#ifdef CG_LUA
	trap_AddCommand("lua_status");
	#endif

	/* cinematic cam test */
	trap_AddCommand("camtest");
	trap_AddCommand("camtestend");

	trap_AddCommand("safezonelist");
	trap_AddCommand("selfdestruct");
	trap_AddCommand("shipdamage");
	trap_AddCommand("shiphealth");
	trap_AddCommand("reloadtorpedos");
	trap_AddCommand("torpedolist");

	/* precacheFile */
	trap_AddCommand("generatePrecacheFile");

	/* test new logger on game side */
	trap_AddCommand("testlogger");

	trap_AddCommand("scrip_call");
}

