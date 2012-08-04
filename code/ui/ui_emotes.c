/*
======================================================

RPG-X Emotes Window
By TiM
14-5-2006

This menu is designed to allow players to choose emotes 
they want their characters to play ingame.

======================================================
*/

#include "ui_local.h"

#define ID_RECENT		1
#define	ID_FAVORITES	2
#define	ID_VIEWALL		3
#define	ID_SITTING		4
#define	ID_CONSOLE		5
#define	ID_GESTURE		6
#define	ID_FULLBODY		7
#define	ID_INJURED		8
#define ID_MISC			9

#define	ID_MAINMENU		15

#define	ID_BIND_EMOTE	16
#define ID_FAV_EMOTE	17
#define ID_DO_EMOTE		18

#define ID_LIST_UP		20
#define ID_LIST_DN		21
#define ID_SCROLLBAR	22

#define	ID_EMOTELIST1	101
#define	ID_EMOTELIST2	102
#define	ID_EMOTELIST3	103
#define	ID_EMOTELIST4	104
#define	ID_EMOTELIST5	105
#define	ID_EMOTELIST6	106
#define	ID_EMOTELIST7	107
#define	ID_EMOTELIST8	108
#define	ID_EMOTELIST9	109
#define	ID_EMOTELIST10	110
#define	ID_EMOTELIST11	111
#define	ID_EMOTELIST12	112

#define	PIC_ARROW_UP		"menu/common/arrow_up_16.tga"
#define	PIC_ARROW_DOWN		"menu/common/arrow_dn_16.tga"

#define	MAX_MENULISTITEMS	12

#define MIN_SCROLLHEIGHT	8
#define MAX_SCROLLRANGE		198
#define	MAX_SCROLLTOP		204

#define	FAV_CVAR			"ui_favoriteEmote"

#define LOW_MEMORY			(5 * 1024 * 1024)

//Defined here so the PlayerModel APIs can handle them :)
static void PlayerEmotes_FillEmotesArray( int emoteCategory );

static void PlayerEmotes_UpdateScrollBar( menuaction_s *bar );
static void PlayerEmotes_SetupScrollBar( menuaction_s *bar );

//Un-necessary, emoteNum can be derived from
//the order in the list
/*typedef struct {
	char	emoteNameUpr[16];

	int		emoteNum;
} listEmote_t;*/

//TiM - data necessary for a scroll bar
typedef struct
{
	qboolean		mouseDown;
	qboolean		doubleStep;

	int				yStart;
} scrollData_t;

typedef struct {
	menuframework_s		menu;

	//main buttons
	menubitmap_s		recentFilter;
	menubitmap_s		favoritesFilter;
	menubitmap_s		viewAllFilter;
	menubitmap_s		sittingFilter;
	menubitmap_s		consoleFilter;
	menubitmap_s		gestureFilter;
	menubitmap_s		fullMotionFilter;
	menubitmap_s		injuredFilter;
	menubitmap_s		miscFilter;

	menubitmap_s		mainMenu;	//Return to game ( or main menu )

	//menu arrows
	menubitmap_s		upArrow;
	menubitmap_s		dnArrow;
	menuaction_s		scrollBar;

	//emote parameters list
	char				emoteTitle[26]; //Emote name displayed at the top
	menufield_s			modelOffset;	//button used to enter in modeloffset data
	
	menuaction_s		emoteBind;		//keybind(s) for this emote
	int					bindValue;		//ASCII index of the key this emote is bound to
	qboolean			keyBindActive;	//True while the code is waiting for the user to enter a new emote bind
	
	menubitmap_s		addFav;			//Add Favorites Button
	menubitmap_s		playEmote;		//Play Emote Button

	int					selectedEmote;
	int					favvedEmote;	//the cvar index this emote is favved at

	//playermodel rendering variables
	menubitmap_s		playerMdl;
	char				playerModel[MAX_QPATH];
	playerInfo_t		playerInfo;
	vec3_t				viewAngles;
	vec3_t				moveAngles;

	//graphics definitions
	qhandle_t			corner_ll_4_18;
	qhandle_t			corner_ll_4_4;
	qhandle_t			corner_ur_18_18;
	qhandle_t			corner_lr_18_4;
	qhandle_t			corner_lr_4_18;

	//active emotes storage definitions
	int					emoteListOffset;	//offset that is incremented/decremented by the arrow tools
	int					numEmotes;		//number of emotes in main list
	menubitmap_s		emotesMenu[MAX_MENULISTITEMS];	//buttons to display the active emote set
	char				emoteNames[MAX_MENULISTITEMS][25]; //local store for the emotes name
	int					mainEmotesList[175];	//the primary emote list, reset each time a new category is picked

	int					prevOffset;	//Save the modeloffset so as to execute the command when we leave teh menu if changed

	int					currentMenu; //Save the current menu... we need this to refresh the fav menu if need be

	//ie this menu was called via the console ( ie a key bind instead of from the main menu)
	qboolean			fromConsole;

	scrollData_t		scrollData;		//TiM - Scroll data
} playerEmotes_t;

playerEmotes_t s_playerEmotes;

/*
=================
Player_SpinPlayer
=================
*/
static void PlayerEmotes_SpinPlayer( void* ptr, int event)
{
	if ( event == QM_ACTIVATED ) 
	{
		uis.spinView = qtrue;
		uis.cursorpx = uis.cursorx;
	}
}

/*
=================
Player_InitModel
=================
*/
static void PlayerEmotes_InitModel( void )
{
	memset( &s_playerEmotes.playerInfo, 0, sizeof(playerInfo_t) );

	UI_PlayerInfo_SetModel( &s_playerEmotes.playerInfo, UI_Cvar_VariableString( "model" ) );

	//Player_UpdateModel( ANIM_IDLE );
	VectorClear( s_playerEmotes.viewAngles );
	VectorClear( s_playerEmotes.moveAngles );
	s_playerEmotes.viewAngles[YAW] = uis.lastYaw;
	s_playerEmotes.viewAngles[PITCH] = 0;
	s_playerEmotes.viewAngles[ROLL] = 0;
	s_playerEmotes.moveAngles[YAW] = 0; //s_main.playerViewangles[YAW];

	UI_PlayerInfo_SetInfo( &s_playerEmotes.playerInfo, 
							BOTH_STAND1, 
							BOTH_STAND1, 
							s_playerEmotes.viewAngles,
							s_playerEmotes.moveAngles,
							WP_0,
							trap_Cvar_VariableValue( "height" ), 
							trap_Cvar_VariableValue( "weight" ), 
							qfalse );
}

/*
=================
Player_DrawPlayer
=================
*/
static void PlayerEmotes_DrawPlayer( void ) //*self ) 
{
	vec3_t			origin = {-20, 5, -4 };//{ 0, 3.8, 0};
	char			buf[MAX_QPATH];

	if( trap_MemoryRemaining() <= LOW_MEMORY ) {
		UI_DrawProportionalString( s_playerEmotes.playerMdl.generic.x, s_playerEmotes.playerMdl.generic.y + s_playerEmotes.playerMdl.height / 2, "LOW MEMORY", UI_LEFT, color_red );
		return;
	}

	trap_Cvar_VariableStringBuffer( "model", buf, sizeof( buf ) );

	//if model is changed in the console
	if ( Q_stricmp( buf, s_playerEmotes.playerInfo.modelName ) ) {
		UI_PlayerInfo_SetModel( &s_playerEmotes.playerInfo, buf);

		s_playerEmotes.viewAngles[YAW] = uis.lastYaw; //yaw

		UI_PlayerInfo_SetInfo( &s_playerEmotes.playerInfo, BOTH_STAND1, BOTH_STAND1, s_playerEmotes.viewAngles, vec3_origin, WP_0, trap_Cvar_VariableValue( "height" ), trap_Cvar_VariableValue( "weight" ), qfalse );		
	
		//reload the menu just in case
		PlayerEmotes_FillEmotesArray( s_playerEmotes.currentMenu );
	}

	UI_DrawPlayer( s_playerEmotes.playerMdl.generic.x, s_playerEmotes.playerMdl.generic.y, s_playerEmotes.playerMdl.width, s_playerEmotes.playerMdl.height, origin, &s_playerEmotes.playerInfo, uis.realtime );
}

/*
=================
Player_DoEmote
TiM: Called to make the
player model onscreen
do the emote animation
=================
*/
static void Player_DoEmote( int emoteNum ) {
	emoteList_t		*emote;
	int	torsoAnim	= BOTH_STAND1;
	int	legsAnim	= BOTH_STAND1;
	int	legsTimer	= 0;
	int torsoTimer	= 0;

	emote = &bg_emoteList[ emoteNum ];

	//Com_Printf( S_COLOR_RED "Emote num: %i, Enum: %i, Legs time: %i\n", emoteNum, emote->enumName );

	if ( !emote )
		return;

	if ( emote->bodyFlags & EMOTE_LOWER )
	{
		legsAnim = emote->enumName;

		if ( emote->enumLoop > 0 )
			s_playerEmotes.playerInfo.lowerLoopEmote = emote->enumLoop;
		else
			s_playerEmotes.playerInfo.lowerLoopEmote = 0;
	}
	
	if ( emote->bodyFlags & EMOTE_UPPER )
	{
		torsoAnim = emote->enumName;

		if ( emote->enumLoop > 0 )
			s_playerEmotes.playerInfo.upperLoopEmote = emote->enumLoop;
		else
			s_playerEmotes.playerInfo.upperLoopEmote = 0;
	}

	if ( !(emote->animFlags & EMOTE_LOOP_LOWER) ) {
		legsTimer = s_playerEmotes.playerInfo.animations[ emote->enumName ].numFrames * s_playerEmotes.playerInfo.animations[ emote->enumName ].frameLerp;
		s_playerEmotes.playerInfo.lowerEmoting = qtrue;
	}

	if ( !(emote->animFlags & EMOTE_LOOP_UPPER) ) {
		torsoTimer = s_playerEmotes.playerInfo.animations[ emote->enumName ].numFrames * s_playerEmotes.playerInfo.animations[ emote->enumName ].frameLerp;
		s_playerEmotes.playerInfo.upperEmoting = qtrue;
	}

	s_playerEmotes.playerInfo.legsAnimationTimer = legsTimer;
	s_playerEmotes.playerInfo.torsoAnimationTimer = torsoTimer;

	s_playerEmotes.viewAngles[YAW] = uis.lastYaw;
	UI_PlayerInfo_SetInfo( &s_playerEmotes.playerInfo, legsAnim, torsoAnim, s_playerEmotes.viewAngles, vec3_origin, WP_0, trap_Cvar_VariableValue( "height" ), trap_Cvar_VariableValue( "weight" ), qfalse );
}

/*
===============
PlayerEmotes_BuildEmotesList
TiM: Fills in the main menu list
from the main emote array.
Called upon new list defines as
well as when the arrow key is clicked
===============
*/
static void PlayerEmotes_BuildEmotesList( int *emoteListOffset ) 
{
	int i;
	int offset;

	//clamp the offset value 
	if ( *emoteListOffset < 0 ) {
		*emoteListOffset = 0;
		return;
	}

	if ( s_playerEmotes.numEmotes > MAX_MENULISTITEMS && *emoteListOffset > s_playerEmotes.numEmotes - MAX_MENULISTITEMS ) {
		*emoteListOffset = s_playerEmotes.numEmotes - MAX_MENULISTITEMS;
		return;
	}

	//clear all of the previous list data
	memset( &s_playerEmotes.emoteNames, 0, sizeof( s_playerEmotes.emoteNames ) );

	//populate the list
	for ( i = 0; i < MAX_MENULISTITEMS; i++ ) {
		offset = *emoteListOffset + i;

		//if there's no data (ie there are no emotes this far), 
		//make the button hidden and continue
		if ( i > s_playerEmotes.numEmotes || s_playerEmotes.mainEmotesList[offset] == -1 || s_playerEmotes.mainEmotesList[offset] >= bg_numEmotes ) {
			s_playerEmotes.emotesMenu[i].generic.flags = ( QMF_INACTIVE | QMF_HIDDEN );
			s_playerEmotes.emotesMenu[i].textPtr = NULL;
			continue;
		}

		Q_strncpyz( s_playerEmotes.emoteNames[i], bg_emoteList[ s_playerEmotes.mainEmotesList[offset] ].name, sizeof( s_playerEmotes.emoteNames[i] ) );
		Q_strupr( s_playerEmotes.emoteNames[i] );

		s_playerEmotes.emotesMenu[i].generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
		s_playerEmotes.emotesMenu[i].textPtr = s_playerEmotes.emoteNames[i];
	}
}

/*
===============
PlayerEmotes_FillEmotesArray
TiM: Called when a new category
button is pressed.  It flushes the 
main list of emotes, and re-populates it with the 
new category.  Then it reloads the main list
with these new emotes
===============
*/
static void PlayerEmotes_FillEmotesArray( int emoteCategory ) {
	int	i;
	emoteList_t	*emote;

	//reset the list and counting data
	//NB: -1 instead of 0 since 0 is a valid emote
	for ( i = 0; i < 175; i++ ) {
		s_playerEmotes.mainEmotesList[i] = -1;
	}
	s_playerEmotes.numEmotes = 0;

	s_playerEmotes.currentMenu = emoteCategory; 

	//The first few categories are pre-set
	//so we'll manually handle those
	switch ( emoteCategory ) 
	{
		//this one's a kicker ROFL
		case ID_VIEWALL:
			for ( i = 0; i < bg_numEmotes; i++ ) {
				//TiM: Make sure we don't include emote stubs.  No point
				emote = &bg_emoteList[ i ];
				if ( !emote || (emote->enumName >= 0 && emote->enumName < MAX_ANIMATIONS && s_playerEmotes.playerInfo.animations[ emote->enumName ].numFrames < 0) || !emote->name[0] )
					continue;

				s_playerEmotes.mainEmotesList[ s_playerEmotes.numEmotes ] = i;
				s_playerEmotes.numEmotes++; 
			}

			break;
		//Console stored emote lists
		case ID_RECENT:
		case ID_FAVORITES:
			{
				char consoleName[25];
				char fullName[32];
				char cvarValue[5];
				int emoteNum;

				//favorites and recent are basically the same, so with a quick condition here,
				//we can re-use the same code for both :)
				if ( emoteCategory == ID_RECENT )
					Q_strncpyz( consoleName, "ui_recentEmote", sizeof( consoleName ) );
				else
					Q_strncpyz( consoleName, FAV_CVAR, sizeof( consoleName ) );

				//reversed so the thing that was added last is displayed first :)
				for ( i = NUM_CVAR_STORES; i > 0; i-- ) {
					Com_sprintf( fullName, sizeof( fullName ), "%s%i", consoleName, i );

					//even tho we are loading int values from these CVARs, we'll be handling them like strings at first.
					//reason being, "0" is a valid emote number, "" isn't
					trap_Cvar_VariableStringBuffer( fullName, cvarValue, sizeof( cvarValue ) );

					if ( !cvarValue[0] || !Q_stricmp( cvarValue, "-1" ) )
						continue;
					
					emoteNum = atoi( cvarValue );

					//error check the int
					if ( emoteNum >= bg_numEmotes || emoteNum < 0 ) {
						continue;
					}

					//TiM: Make sure we don't include emote stubs.  No point
					emote = &bg_emoteList[ emoteNum ];
					if ( !emote || (emote->enumName >= 0 && emote->enumName < MAX_ANIMATIONS && s_playerEmotes.playerInfo.animations[ emote->enumName ].numFrames < 0) )
						continue;

					//add to the list
					s_playerEmotes.mainEmotesList[s_playerEmotes.numEmotes] = emoteNum;
					s_playerEmotes.numEmotes++;
				}
			}
			break;
		
		//specific per-category emotes
		case ID_SITTING:
		case ID_CONSOLE:
		case ID_GESTURE:
		case ID_FULLBODY:
		case ID_INJURED:
		case ID_MISC:
			{
				int	emoteIndex;
				
				//find out the actual ID we need from that lot up there
				//lol a case in a case
				switch ( emoteCategory ) {
					case ID_SITTING:
						emoteIndex = TYPE_SITTING;
						break;
					case ID_CONSOLE:
						emoteIndex = TYPE_CONSOLE;
						break;
					case ID_GESTURE:
						emoteIndex = TYPE_GESTURE;
						break;
					case ID_FULLBODY:
						emoteIndex = TYPE_FULLBODY;
						break;
					case ID_INJURED:
						emoteIndex = TYPE_INJURED;
						break;
					case ID_MISC:
						emoteIndex = TYPE_MISC;
						break;
					default:
						emoteIndex = TYPE_NONE;
						break;
				}
			
				if ( emoteIndex < 0 ) {
					break;
				}

				//loop thru all the emotes, and add any that have a matching Index
				for ( i = 0; i < bg_numEmotes; i++ ) {
					//TiM: Make sure we don't include emote stubs.  No point
					emote = &bg_emoteList[ i ];
					if ( !emote || (emote->enumName >= 0 && emote->enumName < MAX_ANIMATIONS && s_playerEmotes.playerInfo.animations[ emote->enumName ].numFrames < 0) )
						continue;					
					
					if ( bg_emoteList[i].emoteType == emoteIndex ) {
						s_playerEmotes.mainEmotesList[s_playerEmotes.numEmotes] = i;
						s_playerEmotes.numEmotes++;
					}
				}
			}
			break;
	}

	//if the number of emotes exceeded our displayable total, then activate the arrow buttons
	if ( s_playerEmotes.numEmotes > MAX_MENULISTITEMS ) {
		s_playerEmotes.upArrow.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
		s_playerEmotes.dnArrow.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	}

	s_playerEmotes.emoteListOffset = 0;
	PlayerEmotes_BuildEmotesList( &s_playerEmotes.emoteListOffset );
}

/*
===============
PlayerEmotes_HandleNewEmote
TiM: An emotes button was pressed
Now to handle the tonnes of button
checks n' stuff
===============
*/
static void PlayerEmotes_HandleNewEmote( int buttonId ) {			
	int i;
	int	buttonPressed = buttonId - 100; //offset by 100 so they wouldn't get in the way
	char binding[256];

	//get emote from stored list
	s_playerEmotes.selectedEmote = s_playerEmotes.mainEmotesList[ buttonPressed + s_playerEmotes.emoteListOffset - 1 ];

	Q_strncpyz( s_playerEmotes.emoteTitle, bg_emoteList[ s_playerEmotes.selectedEmote ].name, sizeof( s_playerEmotes.emoteTitle ) );
	Q_strupr( s_playerEmotes.emoteTitle );

	//search for binds for this emote
	s_playerEmotes.bindValue = -1;
	for ( i=0; i<256; i++ ){
		trap_Key_GetBindingBuf( i, binding, sizeof( binding ) );

		if ( *binding == 0 )
			continue;

		if ( strstr( binding, bg_emoteList[ s_playerEmotes.selectedEmote ].name ) ) {
			s_playerEmotes.bindValue = i;
			break;
		}
	}
	s_playerEmotes.emoteBind.generic.flags = QMF_CENTER_JUSTIFY | QMF_HIGHLIGHT_IF_FOCUS;

	//check for favorites
	s_playerEmotes.favvedEmote = 0;
	for ( i = 1; i <= NUM_CVAR_STORES; i++ ) {
		if ( (int)trap_Cvar_VariableValue( va( FAV_CVAR "%i", i ) ) == s_playerEmotes.selectedEmote ) {
			s_playerEmotes.favvedEmote = i;
			s_playerEmotes.addFav.textEnum = MBT_KILL_FAV_EMOTE;
			break;
		}
	}

	//set button title either way
	if ( !s_playerEmotes.favvedEmote ) {
		s_playerEmotes.addFav.textEnum = MBT_FAV_EMOTE;
	}

	//either way, make the fav button active
	if ( (s_playerEmotes.addFav.generic.flags & QMF_INACTIVE) )
	{
		s_playerEmotes.addFav.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	}

	//make our player character do teh emote
	Player_DoEmote( s_playerEmotes.selectedEmote );
}

/*
===============
PlayerEmotes_HandleFav
===============
*/
static void PlayerEmotes_HandleFav( void ) {
	int		i;
	char*	cvar;


	//safety net lol
	if ( s_playerEmotes.selectedEmote < 0 )
		return;

	//this emote's been favved, so I guess we're unfaving it now
	if ( s_playerEmotes.favvedEmote > 0 ) {
		cvar = va( FAV_CVAR "%i", s_playerEmotes.favvedEmote );
		
		//double chack. make sure that we've got the right emote
		if ( (int)trap_Cvar_VariableValue( cvar ) == s_playerEmotes.selectedEmote ) {
			//okay, all good.  unfave it
			trap_Cvar_Set( cvar, "-1" );

			//shuffle all the other favorites up the list
			for ( i = s_playerEmotes.favvedEmote + 1; i <= NUM_CVAR_STORES; i++ ) {
				if ( (int)trap_Cvar_VariableValue( va( FAV_CVAR "%i", i - 1 ) ) == -1 ) {
					//set the previous CVAR, the value of this CVAR regardless if it's -1 or not
					trap_Cvar_SetValue( va( FAV_CVAR "%i", i - 1 ), (int)trap_Cvar_VariableValue( va( FAV_CVAR "%i", i ) ) );

					//and then flush out this CVAR
					trap_Cvar_Set( va( FAV_CVAR "%i", i ), "-1" );
				}
			}

			// if fav menu, refresh the list, else just change teh button
			if ( s_playerEmotes.currentMenu == ID_FAVORITES )
				PlayerEmotes_FillEmotesArray( ID_FAVORITES );

			s_playerEmotes.favvedEmote = 0;
			s_playerEmotes.addFav.textEnum = MBT_FAV_EMOTE;
		}
	}
	else { //alraedy unfavved! Let's fav it!
		//search for the first unfavved slot we can
		
		for ( i = 1; i <= NUM_CVAR_STORES; i++ ) {
			
			cvar = va( FAV_CVAR "%i", i );
			if ( (int)trap_Cvar_VariableValue( cvar ) == -1 ) {
				//found a slot
				trap_Cvar_SetValue( cvar, s_playerEmotes.selectedEmote );
				s_playerEmotes.favvedEmote = i;
				break;
			}
		}

		//aw damn, no slots were found
		if ( s_playerEmotes.favvedEmote == 0 ) {
			//okay... so we're going to push the top one off, and shuffle the rest up
			for ( i = 2; i <= NUM_CVAR_STORES; i++ ) {
				cvar = va( FAV_CVAR "%i", i-1 );
				trap_Cvar_SetValue( cvar, (int)trap_Cvar_VariableValue( va( FAV_CVAR"%i", i ) ) );

				if ( i == NUM_CVAR_STORES ) {
					trap_Cvar_SetValue( va( FAV_CVAR"%i", i ), s_playerEmotes.selectedEmote );
					s_playerEmotes.favvedEmote = NUM_CVAR_STORES;
				}
			}
		}

		if ( s_playerEmotes.currentMenu == ID_FAVORITES )
			PlayerEmotes_FillEmotesArray( ID_FAVORITES );
		else {
			s_playerEmotes.addFav.textEnum = MBT_KILL_FAV_EMOTE;
		}
	}
}

/*
===============
PlayerEmotes_ExecuteOffset
===============
*/
static void PlayerEmotes_ExecuteOffset( void ) {
	int offset;

	offset = atoi( s_playerEmotes.modelOffset.field.buffer );

	if ( offset != s_playerEmotes.prevOffset )
		trap_Cmd_ExecuteText( EXEC_APPEND, va( "modelOffset %i\n", offset ) );
}

/*
===============
PlayerEmotes_Event
===============
*/
static void PlayerEmotes_Event( void* ptr, int event ) {

	if ( event != QM_ACTIVATED )
		return;

	s_playerEmotes.keyBindActive = qfalse;

	switch ( ((menucommon_s *)ptr)->id ) {
		//Fav button hit
		case ID_FAV_EMOTE:
			PlayerEmotes_HandleFav();
			break;

		//emote button pressed
		case ID_EMOTELIST1:
		case ID_EMOTELIST2:
		case ID_EMOTELIST3:
		case ID_EMOTELIST4:
		case ID_EMOTELIST5:
		case ID_EMOTELIST6:
		case ID_EMOTELIST7:
		case ID_EMOTELIST8:
		case ID_EMOTELIST9:
		case ID_EMOTELIST10:
		case ID_EMOTELIST11:
		case ID_EMOTELIST12:	
			PlayerEmotes_HandleNewEmote( ((menucommon_s *)ptr)->id );
			break;

		//any of the main buttons
		case ID_RECENT:
		case ID_FAVORITES:
		case ID_VIEWALL:
		case ID_SITTING:
		case ID_CONSOLE:
		case ID_GESTURE:
		case ID_FULLBODY:
		case ID_INJURED:
		case ID_MISC:
			PlayerEmotes_FillEmotesArray( ((menucommon_s *)ptr)->id );
			PlayerEmotes_SetupScrollBar( &s_playerEmotes.scrollBar );
			break;

		case ID_BIND_EMOTE:
			s_playerEmotes.keyBindActive = qtrue;
			break;

		case ID_DO_EMOTE:
			PlayerEmotes_ExecuteOffset();
			if ( s_playerEmotes.selectedEmote >= 0 )
				trap_Cmd_ExecuteText( EXEC_APPEND, va( "emote %s\n", bg_emoteList[s_playerEmotes.selectedEmote].name ) );

			UI_ForceMenuOff();
			break;

		case ID_LIST_UP:
			s_playerEmotes.emoteListOffset--;
			PlayerEmotes_BuildEmotesList( &s_playerEmotes.emoteListOffset );
			PlayerEmotes_UpdateScrollBar( &s_playerEmotes.scrollBar );
			break;
		case ID_LIST_DN:
			s_playerEmotes.emoteListOffset++;
			PlayerEmotes_BuildEmotesList( &s_playerEmotes.emoteListOffset );
			PlayerEmotes_UpdateScrollBar( &s_playerEmotes.scrollBar );
			break;
		case ID_MAINMENU:
			UI_PopMenu();
			break;
	}
}

/*
===============
PlayerEmotes_Draw
===============
*/
static void PlayerEmotes_Draw( void ) {
	UI_MenuFrame( &s_playerEmotes.menu );

	//Left side LCARS bars
	trap_R_SetColor( colorTable[CT_LTGOLD1]);
	UI_DrawHandlePic(30,203,  47, 70, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_LTBROWN1]); 
	UI_DrawHandlePic(30,276,  47, 78, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKRED1]); 
	UI_DrawHandlePic(30,357,  47, 32, uis.whiteShader);

	//LCARS Numbers
	UI_DrawProportionalString(  74,  66, "6154",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  84, "67144",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  188, "31456",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	UI_DrawProportionalString(  74,  206, "914344",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  279, "41634",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  360, "23513",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	UI_DrawProportionalString(  74,  395, "56123",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	UI_DrawProportionalString( 584, 142, "2112",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	//Lines for player model frame
	trap_R_SetColor( colorTable[CT_LTBLUE1]);
	UI_DrawHandlePic(  444, 228, 165,   1, uis.whiteShader); //81
	UI_DrawHandlePic(  446, 293, 161,   1, uis.whiteShader); //83
	UI_DrawHandlePic(  444, 365, 165,   1, uis.whiteShader); //81

	UI_DrawHandlePic( 499, 162,   1, 266, uis.whiteShader); //136
	UI_DrawHandlePic( 548, 162,   1, 266, uis.whiteShader); //136

	//Draw Player
	PlayerEmotes_DrawPlayer();

	//Left Bracket around model picture
	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(444,158, 16, 16, uis.graphicBracket1CornerLU); //81
	UI_DrawHandlePic(444,174,  8, 94, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(444,271,  8, 11, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_LTORANGE]);
	UI_DrawHandlePic(446,285,  6, 21, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(444,310,  8, 11, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(444,324,  8, 94, uis.whiteShader);
	UI_DrawHandlePic(444,418, 16, -16, uis.graphicBracket1CornerLU);	//LD

	//Right Bracket around model picture
	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(593,158, -16, 16, uis.graphicBracket1CornerLU); //230
	UI_DrawHandlePic(601,174,  8, 94, uis.whiteShader); //238

	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(601,271,  8, 11, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_LTORANGE]);
	UI_DrawHandlePic(601,285,  6, 21, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(601,310,  8, 11, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(601,324,  8, 94, uis.whiteShader);
	UI_DrawHandlePic(593,418, -16, -16, uis.graphicBracket1CornerLU); //375	//RD

	//Frame around the emotes selection list
	UI_DrawHandlePic(  85,  146,   8,  -32, s_playerEmotes.corner_ll_4_18);	// UL Corner
	UI_DrawHandlePic(  85, 427,   8,  8, s_playerEmotes.corner_ll_4_4);	// LL Corner
	UI_DrawHandlePic(  237,  158,   32,  32, s_playerEmotes.corner_ur_18_18);	// UR Corner
	UI_DrawHandlePic(  239, 426,   32,  8, s_playerEmotes.corner_lr_18_4);	// LR Corner
	UI_DrawHandlePic(  85,  177,   4, 252, uis.whiteShader);	// Left side
	UI_DrawHandlePic(  241,	183,	18, 18, uis.whiteShader ); //Right Side Up Arrow Button
	//UI_DrawHandlePic(  241,  204,   18, 198, uis.whiteShader);	// Right side
	if ( s_playerEmotes.scrollBar.generic.flags & QMF_HIDDEN )
	{
		UI_DrawHandlePic(  241,  204,   18, 198, uis.whiteShader);	// Right side
	}
	else
	{
		if ( s_playerEmotes.scrollBar.generic.y > MAX_SCROLLTOP + 4 )
			UI_DrawHandlePic( 241, 204, 18, s_playerEmotes.scrollBar.generic.y - MAX_SCROLLTOP - 3, uis.whiteShader);
		if ( s_playerEmotes.scrollBar.generic.bottom + 3 < 402 ) //343
			UI_DrawHandlePic( 241, s_playerEmotes.scrollBar.generic.bottom + 3, 18, 402 - 3 - s_playerEmotes.scrollBar.generic.bottom, uis.whiteShader);
	}	
	
	UI_DrawHandlePic(  241, 405, 18, 18, uis.whiteShader );		//Right Side Down Button
	UI_DrawHandlePic(  89,  158, 151,  18, uis.whiteShader);	// Top
	UI_DrawHandlePic(  90, 429, 150,  4, uis.whiteShader);	// Bottom

	//Frame around the specific emote parameters
	UI_DrawHandlePic(  263,  146,   8,  -32, s_playerEmotes.corner_ll_4_18);	// UL Corner
	UI_DrawHandlePic(  263, 427,   8,  8, s_playerEmotes.corner_ll_4_4);	// LL Corner
	UI_DrawHandlePic(  432,  146,   -8,  -32, s_playerEmotes.corner_ll_4_18);	// UR Corner
	UI_DrawHandlePic(  432, 427,   -8,  8, s_playerEmotes.corner_ll_4_4);	// LR Corner
	UI_DrawHandlePic(  263,  178,   4, 249, uis.whiteShader);	// Left side
	UI_DrawHandlePic(  436,  178,   4, 249, uis.whiteShader);	// Right side
	UI_DrawHandlePic(  267,  158, 166,  18, uis.whiteShader);	// Top
	UI_DrawHandlePic(  267, 429, 168,  4, uis.whiteShader);		// Bottom

	//Emote Name / "Select an Emote"
	{
		char text[32];
		
		if ( s_playerEmotes.selectedEmote < 0 )
			Q_strncpyz( text, menu_normal_text[MNT_CHOOSEEMOTE], sizeof( text ) );
		else
			Q_strncpyz( text, s_playerEmotes.emoteTitle, sizeof( text ) );
	
		UI_DrawProportionalString( 351, 189, text, UI_CENTER|UI_SMALLFONT, colorTable[CT_LTGOLD1] );
	}

	//Emote List Text
	UI_DrawProportionalString( 92, 160, menu_normal_text[ MNT_EMOTELIST ], UI_SMALLFONT, colorTable[CT_BLACK] );
	
	//Emote Parameters Text
	UI_DrawProportionalString( 351, 160, menu_normal_text[ MNT_EMOTEPARAM ], UI_CENTER | UI_SMALLFONT, colorTable[CT_BLACK] );

	//set the relevant enum for the binds button
	if ( s_playerEmotes.keyBindActive )
		s_playerEmotes.emoteBind.textEnum = MBT_PRESS_KEY;
	else
		s_playerEmotes.emoteBind.textEnum = MBT_KEY_BIND;

	if ( s_playerEmotes.selectedEmote < 0 )
		s_playerEmotes.playEmote.textEnum = MBT_CHANGE_OFFSET;
	else
		s_playerEmotes.playEmote.textEnum = MBT_DO_EMOTE;

	Menu_Draw( &s_playerEmotes.menu );
}

/*
===============
PlayerEmotes_DrawBinding
===============
*/
static void PlayerEmotes_DrawBinding( void *self ) {
	qboolean		focus;
	menuaction_s	*action;
	int				x,y;
	int				bind;
	char			name[20];
	int				buttonColor;
	int				textColor;
	int				width;

	action = (menuaction_s *)self;

	x = action->generic.x;
	y = action->generic.y;

	focus = ( Menu_ItemAtCursor( action->generic.parent ) == action );

	bind =  s_playerEmotes.bindValue;

	//Get bind name
	if ( bind <= 0 || bind >=256 ) {
		Q_strncpyz( name, "???", sizeof( name ) );
	}
	else {
		trap_Key_KeynumToStringBuf( bind, name, sizeof( name ) );
		Q_strupr( name );
	}

	//get relevant colors
	if ( focus ) {
		buttonColor = CT_LTPURPLE1;
		textColor = CT_WHITE;
	}
	else {
		buttonColor = CT_DKPURPLE1;
		textColor = CT_BLACK;
	}

	if ( focus ) {
		if ( menu_button_text[action->textEnum][1] ) {
			UI_DrawProportionalString( action->generic.parent->descX, action->generic.parent->descY, menu_button_text[action->textEnum][1], UI_LEFT|UI_TINYFONT, colorTable[CT_BLACK]);
		}
	}

	width = action->width;

	if ( !width ) {
		if ( menu_button_text[action->textEnum][0] )
			width = 19 + (SMALLCHAR_WIDTH * strlen(menu_button_text[action->textEnum][0])) + 19;
		else
			width = 19 + (SMALLCHAR_WIDTH * 11) + 19;
	}

	if ( action->generic.flags & QMF_GRAYED )
		trap_R_SetColor( colorMdGrey );
	else
		trap_R_SetColor( colorTable[buttonColor] );

	UI_DrawHandlePic( x - (width>>1), y,  19,  19, uis.graphicButtonLeftEnd);
	UI_DrawHandlePic( x + (width>>1) - 19, y,  -19,  19, uis.graphicButtonLeftEnd);	//right
	UI_DrawHandlePic( ( x - (width>>1)) + 11,  y,  width - 24,  19, uis.whiteShader);
	trap_R_SetColor( NULL );

	//button text
	if ( strlen( menu_button_text[action->textEnum][0] ) ) {
		UI_DrawProportionalString( x + action->textX, y + action->textY, menu_button_text[action->textEnum][0], UI_CENTER | UI_SMALLFONT, colorTable[textColor] );
	}

	//bind text
	if ( action->generic.flags & QMF_GRAYED )
		textColor = CT_DKGREY;
	else
		textColor = CT_WHITE;
	UI_DrawProportionalString( x + action->textX, y + action->textY + MENU_BUTTON_MED_HEIGHT + 4, name, UI_CENTER | UI_SMALLFONT, colorTable[textColor] );
}

/*
=================
PlayerEmotes_DrawScrollBar
=================
*/
static void PlayerEmotes_DrawScrollBar( void *self )
{
	qboolean		focus;
	menuaction_s	*bar;
	int				*y;
	int				color;
	int				newY;
	int				dif;

	bar = (menuaction_s *)self;

	focus = ( Menu_ItemAtCursor( bar->generic.parent ) == bar );

	if ( focus )
		color = bar->color2;
	else
		color = bar->color;

	trap_R_SetColor( colorTable[ color ] );
	UI_DrawHandlePic( bar->generic.x,  bar->generic.y,  bar->width,  bar->height, uis.whiteShader);
	trap_R_SetColor( NULL );

	if ( !s_playerEmotes.scrollData.mouseDown )
		return;

	if ( !trap_Key_IsDown( K_MOUSE1 ) )
	{
		s_playerEmotes.scrollData.mouseDown = qfalse;
		uis.activemenu->noNewSelecting = qfalse;
		return;
	}

	if ( uis.cursory == s_playerEmotes.scrollData.yStart )
		return;

	y = &bar->generic.y;

	newY = *y + (uis.cursory - s_playerEmotes.scrollData.yStart);

	if ( newY+bar->height > MAX_SCROLLTOP + MAX_SCROLLRANGE )
		newY = (MAX_SCROLLTOP + MAX_SCROLLRANGE) - bar->height;

	if ( newY < MAX_SCROLLTOP )
		newY = MAX_SCROLLTOP;

	dif = newY - *y;

	s_playerEmotes.emoteListOffset += dif * (s_playerEmotes.scrollData.doubleStep ? 2 : 1);
	PlayerEmotes_BuildEmotesList( &s_playerEmotes.emoteListOffset );

	*y = newY;
	bar->generic.top = *y;
	bar->generic.bottom = *y + bar->height;

	s_playerEmotes.scrollData.yStart = uis.cursory;
}

/*
=================
PlayerEmotes_SetupScrollBar
=================
*/
static void PlayerEmotes_SetupScrollBar( menuaction_s *bar )
{
	int height;

	//first make sure it's worth enabling this at all
	if ( s_playerEmotes.numEmotes <= MAX_MENULISTITEMS )
	{
		bar->generic.flags = QMF_INACTIVE | QMF_HIDDEN;
		return;
	}

	//show the bar
	bar->generic.flags &= ~(QMF_INACTIVE | QMF_HIDDEN);

	//calculate the necessary height of the bar
	//by default, assume 1 pixel per offset
	height = ( MAX_SCROLLRANGE ) - ( s_playerEmotes.numEmotes -  MAX_MENULISTITEMS );
	
	//ensure box doesn't get too small
	if ( height < MIN_SCROLLHEIGHT )
	{
		//double the step in that case
		//a bit hacky, but no need for 3 since the limit isn't that high
		height = ( MAX_SCROLLRANGE ) - ( s_playerEmotes.numEmotes * 0.5 - MAX_MENULISTITEMS );
		s_playerEmotes.scrollData.doubleStep = qtrue;
	}
	else
	{
		s_playerEmotes.scrollData.doubleStep = qfalse;
	}

	//reset to top
	bar->generic.y = bar->generic.top = MAX_SCROLLTOP;

	bar->height = height;
	bar->generic.bottom = bar->generic.y + height;

}

/*
=================
PlayerEmotes_UpdateScrollBar
=================
*/
static void PlayerEmotes_UpdateScrollBar( menuaction_s *bar )
{
	bar->generic.y = MAX_SCROLLTOP + s_playerEmotes.emoteListOffset*(s_playerEmotes.scrollData.doubleStep ? 0.5 : 1);
	bar->generic.top = bar->generic.y;
	bar->generic.bottom = bar->generic.top + bar->height;
}

/*
===============
PlayerEmotes_KeyEvent
===============
*/
static sfxHandle_t PlayerEmotes_KeyEvent ( int key ) {
	menucommon_s	*s;
	int				i;
	char			command[256];
	int				emoteId;

	s = (menucommon_s *)Menu_ItemAtCursor( &s_playerEmotes.menu );

	if ( s_playerEmotes.keyBindActive ) {
		if (key & K_CHAR_FLAG)
			goto end;

		if ( key < 1 || key > 256 )
			goto end;

		switch ( key ) {
			case K_ESCAPE:
				s_playerEmotes.keyBindActive = qfalse;
				return (menu_out_sound);
			case '`':
				goto end;
				break;
		}

		//unbind this emote from anything else
		for ( i = 1; i < 256; i++ ) {
			trap_Key_GetBindingBuf( i, command, 256 );

			if ( !Q_stricmp( command, va( "emote %s", bg_emoteList[s_playerEmotes.selectedEmote].name ) ) ) {
				trap_Key_SetBinding( i, "" );
			}
		}
		
		//set the new command
		trap_Key_SetBinding( key, va( "emote %s", bg_emoteList[s_playerEmotes.selectedEmote].name ) );
		s_playerEmotes.bindValue = key;

		s_playerEmotes.keyBindActive = qfalse;

		return ( menu_out_sound );
	}
	else {
		//unbind the current key
		if ( s->id == ID_BIND_EMOTE ) {
			switch ( key ) {
				case K_BACKSPACE:
				case K_DEL:
				case K_KP_DEL:
					
					for ( i = 1; i < 256; i++ ) {
						trap_Key_GetBindingBuf( i, command, 256 );

						if ( !Q_stricmp( command, va( "emote %s", bg_emoteList[s_playerEmotes.selectedEmote].name ) ) ) {
							trap_Key_SetBinding( i, "" );
							s_playerEmotes.bindValue = -1;
						}
					}
					return ( menu_out_sound );
			}
		}
	}	

	//TiM - scroll bar
	if ( key == K_MOUSE1 && Menu_ItemAtCursor( &s_playerEmotes.menu ) == &s_playerEmotes.scrollBar )
	{
		uis.activemenu->noNewSelecting = qtrue;
		s_playerEmotes.scrollData.mouseDown = qtrue;
		s_playerEmotes.scrollData.yStart = uis.cursory;
	}

	if ( key == K_MOUSE2 && ( s->id >= ID_EMOTELIST1 && s->id <= ID_EMOTELIST12 ) ) {
		PlayerEmotes_ExecuteOffset();
		
		emoteId = s_playerEmotes.mainEmotesList[ ((s->id - 100)-1)+s_playerEmotes.emoteListOffset ];

		if ( emoteId >= 0 && emoteId < bg_numEmotes ) {
			trap_Cmd_ExecuteText( EXEC_APPEND, va( "wait 5;emote %s\n", bg_emoteList[emoteId].name ) );

			UI_ForceMenuOff();
			return menu_out_sound;
		}
	}

end:
	return ( Menu_DefaultKey( &s_playerEmotes.menu, key ) );
}

void UI_PlayerEmotes_Cache( void ) {
	
	s_playerEmotes.corner_ll_4_4	= trap_R_RegisterShaderNoMip("menu/common/corner_ll_4_4");
	s_playerEmotes.corner_ll_4_18	= trap_R_RegisterShaderNoMip("menu/common/corner_ll_4_18");
	s_playerEmotes.corner_lr_4_18	= trap_R_RegisterShaderNoMip("menu/common/corner_lr_4_18");
	s_playerEmotes.corner_lr_18_4	= trap_R_RegisterShaderNoMip("menu/common/corner_lr_18_4");
	s_playerEmotes.corner_ur_18_18	= trap_R_RegisterShaderNoMip("menu/common/corner_ur_18_18");

	trap_R_RegisterShaderNoMip(PIC_ARROW_UP);
	trap_R_RegisterShaderNoMip(PIC_ARROW_DOWN);
}

/*
===============
PlayerEmotes_Init
===============
*/
static void PlayerEmotes_Init( void ) {
	int			x, y;
	int			i;
	qboolean	showRecent = qfalse;
	
	UI_PlayerEmotes_Cache();
	
	uis.spinView = qfalse;
	uis.lastYaw = 160;
	
	PlayerEmotes_InitModel();

	s_playerEmotes.menu.wrapAround					= qtrue;
	s_playerEmotes.menu.fullscreen					= qtrue;
	s_playerEmotes.menu.draw						= PlayerEmotes_Draw;
	s_playerEmotes.menu.descX						= MENU_DESC_X;
	s_playerEmotes.menu.descY						= MENU_DESC_Y;
	s_playerEmotes.menu.titleX						= MENU_TITLE_X;
	s_playerEmotes.menu.titleY						= MENU_TITLE_Y;
	s_playerEmotes.menu.footNoteEnum				= MNT_EMOTES;
	s_playerEmotes.menu.titleI						= MNT_EMOTES_MENU;
	s_playerEmotes.menu.key							= PlayerEmotes_KeyEvent;

	x = 119;
	y = 57;
	s_playerEmotes.recentFilter.generic.type		= MTYPE_BITMAP;
	s_playerEmotes.recentFilter.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_playerEmotes.recentFilter.generic.x			= x;
	s_playerEmotes.recentFilter.generic.y			= y;
	s_playerEmotes.recentFilter.generic.name		= GRAPHIC_SQUARE;
	s_playerEmotes.recentFilter.generic.id			= ID_RECENT;
	s_playerEmotes.recentFilter.generic.callback	= PlayerEmotes_Event;
	s_playerEmotes.recentFilter.width				= MENU_BUTTON_MED_WIDTH;
	s_playerEmotes.recentFilter.height				= MENU_BUTTON_MED_HEIGHT;
	s_playerEmotes.recentFilter.color				= CT_DKPURPLE1;
	s_playerEmotes.recentFilter.color2				= CT_LTPURPLE1;
	s_playerEmotes.recentFilter.textX				= MENU_BUTTON_TEXT_X;
	s_playerEmotes.recentFilter.textY				= MENU_BUTTON_TEXT_Y;
	s_playerEmotes.recentFilter.textEnum			= MBT_RECENT_FILTER;		
	s_playerEmotes.recentFilter.textcolor			= CT_BLACK;
	s_playerEmotes.recentFilter.textcolor2			= CT_WHITE;

	y += 25;
	s_playerEmotes.favoritesFilter.generic.type		= MTYPE_BITMAP;
	s_playerEmotes.favoritesFilter.generic.flags	= QMF_HIGHLIGHT_IF_FOCUS;
	s_playerEmotes.favoritesFilter.generic.x		= x;
	s_playerEmotes.favoritesFilter.generic.y		= y;
	s_playerEmotes.favoritesFilter.generic.name		= GRAPHIC_SQUARE;
	s_playerEmotes.favoritesFilter.generic.id		= ID_FAVORITES;
	s_playerEmotes.favoritesFilter.generic.callback	= PlayerEmotes_Event;
	s_playerEmotes.favoritesFilter.width			= MENU_BUTTON_MED_WIDTH;
	s_playerEmotes.favoritesFilter.height			= MENU_BUTTON_MED_HEIGHT;
	s_playerEmotes.favoritesFilter.color			= CT_DKPURPLE1;
	s_playerEmotes.favoritesFilter.color2			= CT_LTPURPLE1;
	s_playerEmotes.favoritesFilter.textX			= MENU_BUTTON_TEXT_X;
	s_playerEmotes.favoritesFilter.textY			= MENU_BUTTON_TEXT_Y;
	s_playerEmotes.favoritesFilter.textEnum			= MBT_FAV_FILTER;		
	s_playerEmotes.favoritesFilter.textcolor		= CT_BLACK;
	s_playerEmotes.favoritesFilter.textcolor2		= CT_WHITE;

	y += 25;
	s_playerEmotes.viewAllFilter.generic.type		= MTYPE_BITMAP;
	s_playerEmotes.viewAllFilter.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_playerEmotes.viewAllFilter.generic.x			= x;
	s_playerEmotes.viewAllFilter.generic.y			= y;
	s_playerEmotes.viewAllFilter.generic.name		= GRAPHIC_SQUARE;
	s_playerEmotes.viewAllFilter.generic.id			= ID_VIEWALL;
	s_playerEmotes.viewAllFilter.generic.callback	= PlayerEmotes_Event;
	s_playerEmotes.viewAllFilter.width				= MENU_BUTTON_MED_WIDTH;
	s_playerEmotes.viewAllFilter.height				= MENU_BUTTON_MED_HEIGHT;
	s_playerEmotes.viewAllFilter.color				= CT_DKPURPLE1;
	s_playerEmotes.viewAllFilter.color2				= CT_LTPURPLE1;
	s_playerEmotes.viewAllFilter.textX				= MENU_BUTTON_TEXT_X;
	s_playerEmotes.viewAllFilter.textY				= MENU_BUTTON_TEXT_Y;
	s_playerEmotes.viewAllFilter.textEnum			= MBT_ALL_FILTER;		
	s_playerEmotes.viewAllFilter.textcolor			= CT_BLACK;
	s_playerEmotes.viewAllFilter.textcolor2			= CT_WHITE;

	y = 57;
	x += 153;
	s_playerEmotes.sittingFilter.generic.type		= MTYPE_BITMAP;
	s_playerEmotes.sittingFilter.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_playerEmotes.sittingFilter.generic.x			= x;
	s_playerEmotes.sittingFilter.generic.y			= y;
	s_playerEmotes.sittingFilter.generic.name		= GRAPHIC_SQUARE;
	s_playerEmotes.sittingFilter.generic.id			= ID_SITTING;
	s_playerEmotes.sittingFilter.generic.callback	= PlayerEmotes_Event;
	s_playerEmotes.sittingFilter.width				= MENU_BUTTON_MED_WIDTH;
	s_playerEmotes.sittingFilter.height				= MENU_BUTTON_MED_HEIGHT;
	s_playerEmotes.sittingFilter.color				= CT_DKPURPLE1;
	s_playerEmotes.sittingFilter.color2				= CT_LTPURPLE1;
	s_playerEmotes.sittingFilter.textX				= MENU_BUTTON_TEXT_X;
	s_playerEmotes.sittingFilter.textY				= MENU_BUTTON_TEXT_Y;
	s_playerEmotes.sittingFilter.textEnum			= MBT_SITTING_FILTER;		
	s_playerEmotes.sittingFilter.textcolor			= CT_BLACK;
	s_playerEmotes.sittingFilter.textcolor2			= CT_WHITE;

	y += 25;
	s_playerEmotes.consoleFilter.generic.type		= MTYPE_BITMAP;
	s_playerEmotes.consoleFilter.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_playerEmotes.consoleFilter.generic.x			= x;
	s_playerEmotes.consoleFilter.generic.y			= y;
	s_playerEmotes.consoleFilter.generic.name		= GRAPHIC_SQUARE;
	s_playerEmotes.consoleFilter.generic.id			= ID_CONSOLE;
	s_playerEmotes.consoleFilter.generic.callback	= PlayerEmotes_Event;
	s_playerEmotes.consoleFilter.width				= MENU_BUTTON_MED_WIDTH;
	s_playerEmotes.consoleFilter.height				= MENU_BUTTON_MED_HEIGHT;
	s_playerEmotes.consoleFilter.color				= CT_DKPURPLE1;
	s_playerEmotes.consoleFilter.color2				= CT_LTPURPLE1;
	s_playerEmotes.consoleFilter.textX				= MENU_BUTTON_TEXT_X;
	s_playerEmotes.consoleFilter.textY				= MENU_BUTTON_TEXT_Y;
	s_playerEmotes.consoleFilter.textEnum			= MBT_CONSOLE_FILTER;		
	s_playerEmotes.consoleFilter.textcolor			= CT_BLACK;
	s_playerEmotes.consoleFilter.textcolor2			= CT_WHITE;

	y += 25;
	s_playerEmotes.gestureFilter.generic.type		= MTYPE_BITMAP;
	s_playerEmotes.gestureFilter.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_playerEmotes.gestureFilter.generic.x			= x;
	s_playerEmotes.gestureFilter.generic.y			= y;
	s_playerEmotes.gestureFilter.generic.name		= GRAPHIC_SQUARE;
	s_playerEmotes.gestureFilter.generic.id			= ID_GESTURE;
	s_playerEmotes.gestureFilter.generic.callback	= PlayerEmotes_Event;
	s_playerEmotes.gestureFilter.width				= MENU_BUTTON_MED_WIDTH;
	s_playerEmotes.gestureFilter.height				= MENU_BUTTON_MED_HEIGHT;
	s_playerEmotes.gestureFilter.color				= CT_DKPURPLE1;
	s_playerEmotes.gestureFilter.color2				= CT_LTPURPLE1;
	s_playerEmotes.gestureFilter.textX				= MENU_BUTTON_TEXT_X;
	s_playerEmotes.gestureFilter.textY				= MENU_BUTTON_TEXT_Y;
	s_playerEmotes.gestureFilter.textEnum			= MBT_GESTURE_FILTER;		
	s_playerEmotes.gestureFilter.textcolor			= CT_BLACK;
	s_playerEmotes.gestureFilter.textcolor2			= CT_WHITE;

	x += 153;
	y = 57;
	s_playerEmotes.fullMotionFilter.generic.type		= MTYPE_BITMAP;
	s_playerEmotes.fullMotionFilter.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_playerEmotes.fullMotionFilter.generic.x			= x;
	s_playerEmotes.fullMotionFilter.generic.y			= y;
	s_playerEmotes.fullMotionFilter.generic.name		= GRAPHIC_SQUARE;
	s_playerEmotes.fullMotionFilter.generic.id			= ID_FULLBODY;
	s_playerEmotes.fullMotionFilter.generic.callback	= PlayerEmotes_Event;
	s_playerEmotes.fullMotionFilter.width				= MENU_BUTTON_MED_WIDTH;
	s_playerEmotes.fullMotionFilter.height				= MENU_BUTTON_MED_HEIGHT;
	s_playerEmotes.fullMotionFilter.color				= CT_DKPURPLE1;
	s_playerEmotes.fullMotionFilter.color2				= CT_LTPURPLE1;
	s_playerEmotes.fullMotionFilter.textX				= MENU_BUTTON_TEXT_X;
	s_playerEmotes.fullMotionFilter.textY				= MENU_BUTTON_TEXT_Y;
	s_playerEmotes.fullMotionFilter.textEnum			= MBT_FULLMOTION_FILTER;		
	s_playerEmotes.fullMotionFilter.textcolor			= CT_BLACK;
	s_playerEmotes.fullMotionFilter.textcolor2			= CT_WHITE;

	y += 25;
	s_playerEmotes.injuredFilter.generic.type		= MTYPE_BITMAP;
	s_playerEmotes.injuredFilter.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_playerEmotes.injuredFilter.generic.x			= x;
	s_playerEmotes.injuredFilter.generic.y			= y;
	s_playerEmotes.injuredFilter.generic.name		= GRAPHIC_SQUARE;
	s_playerEmotes.injuredFilter.generic.id			= ID_INJURED;
	s_playerEmotes.injuredFilter.generic.callback	= PlayerEmotes_Event;
	s_playerEmotes.injuredFilter.width				= MENU_BUTTON_MED_WIDTH;
	s_playerEmotes.injuredFilter.height				= MENU_BUTTON_MED_HEIGHT;
	s_playerEmotes.injuredFilter.color				= CT_DKPURPLE1;
	s_playerEmotes.injuredFilter.color2				= CT_LTPURPLE1;
	s_playerEmotes.injuredFilter.textX				= MENU_BUTTON_TEXT_X;
	s_playerEmotes.injuredFilter.textY				= MENU_BUTTON_TEXT_Y;
	s_playerEmotes.injuredFilter.textEnum			= MBT_INJURED_FILTER;		
	s_playerEmotes.injuredFilter.textcolor			= CT_BLACK;
	s_playerEmotes.injuredFilter.textcolor2			= CT_WHITE;

	y += 25;
	s_playerEmotes.miscFilter.generic.type			= MTYPE_BITMAP;
	s_playerEmotes.miscFilter.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_playerEmotes.miscFilter.generic.x				= x;
	s_playerEmotes.miscFilter.generic.y				= y;
	s_playerEmotes.miscFilter.generic.name			= GRAPHIC_SQUARE;
	s_playerEmotes.miscFilter.generic.id			= ID_MISC;
	s_playerEmotes.miscFilter.generic.callback		= PlayerEmotes_Event;
	s_playerEmotes.miscFilter.width					= MENU_BUTTON_MED_WIDTH;
	s_playerEmotes.miscFilter.height				= MENU_BUTTON_MED_HEIGHT;
	s_playerEmotes.miscFilter.color					= CT_DKPURPLE1;
	s_playerEmotes.miscFilter.color2				= CT_LTPURPLE1;
	s_playerEmotes.miscFilter.textX					= MENU_BUTTON_TEXT_X;
	s_playerEmotes.miscFilter.textY					= MENU_BUTTON_TEXT_Y;
	s_playerEmotes.miscFilter.textEnum				= MBT_MISC_FILTER;		
	s_playerEmotes.miscFilter.textcolor				= CT_BLACK;
	s_playerEmotes.miscFilter.textcolor2			= CT_WHITE;

	x = 90;
	y = 179;
	for (i = 0; i < MAX_MENULISTITEMS; i++ ) {
		s_playerEmotes.emotesMenu[i].generic.type		= MTYPE_BITMAP;
		s_playerEmotes.emotesMenu[i].generic.flags		= QMF_INACTIVE | QMF_HIDDEN;
		s_playerEmotes.emotesMenu[i].generic.x			= x;
		s_playerEmotes.emotesMenu[i].generic.y			= y;
		s_playerEmotes.emotesMenu[i].generic.callback	= PlayerEmotes_Event;
		s_playerEmotes.emotesMenu[i].generic.id			= ID_EMOTELIST1+i;
		s_playerEmotes.emotesMenu[i].width				= 129;
		s_playerEmotes.emotesMenu[i].height				= 16;
		s_playerEmotes.emotesMenu[i].color				= CT_DKPURPLE1;
		s_playerEmotes.emotesMenu[i].color2				= CT_LTPURPLE1;
		s_playerEmotes.emotesMenu[i].textPtr			= NULL;
		s_playerEmotes.emotesMenu[i].textX				= 4;
		s_playerEmotes.emotesMenu[i].textY				= 1;
		s_playerEmotes.emotesMenu[i].textcolor			= CT_DKGOLD1;
		s_playerEmotes.emotesMenu[i].textcolor2			= CT_LTGOLD1;
		s_playerEmotes.emotesMenu[i].textStyle			= UI_SMALLFONT;

		y += 21;
	}

	s_playerEmotes.upArrow.generic.type				= MTYPE_BITMAP;
	s_playerEmotes.upArrow.generic.flags			= QMF_INACTIVE | QMF_GRAYED;
	s_playerEmotes.upArrow.generic.x				= 242;
	s_playerEmotes.upArrow.generic.y				= 185;
	s_playerEmotes.upArrow.generic.name				= PIC_ARROW_UP;
	s_playerEmotes.upArrow.generic.id				= ID_LIST_UP;
	s_playerEmotes.upArrow.generic.callback			= PlayerEmotes_Event;
	s_playerEmotes.upArrow.width  					= 16;
	s_playerEmotes.upArrow.height  					= 16;
	s_playerEmotes.upArrow.color					= CT_DKPURPLE1;
	s_playerEmotes.upArrow.color2					= CT_LTPURPLE1;
	s_playerEmotes.upArrow.textX					= MENU_BUTTON_TEXT_X;
	s_playerEmotes.upArrow.textY					= MENU_BUTTON_TEXT_Y;
	s_playerEmotes.upArrow.textcolor				= CT_BLACK;
	s_playerEmotes.upArrow.textcolor2				= CT_WHITE;

	s_playerEmotes.dnArrow.generic.type				= MTYPE_BITMAP;
	s_playerEmotes.dnArrow.generic.flags			= QMF_INACTIVE | QMF_GRAYED;
	s_playerEmotes.dnArrow.generic.x				= 242;
	s_playerEmotes.dnArrow.generic.y				= 407;
	s_playerEmotes.dnArrow.generic.name				= PIC_ARROW_DOWN;
	s_playerEmotes.dnArrow.generic.id				= ID_LIST_DN;
	s_playerEmotes.dnArrow.generic.callback			= PlayerEmotes_Event;
	s_playerEmotes.dnArrow.width  					= 16;
	s_playerEmotes.dnArrow.height  					= 16;
	s_playerEmotes.dnArrow.color					= CT_DKPURPLE1;
	s_playerEmotes.dnArrow.color2					= CT_LTPURPLE1;
	s_playerEmotes.dnArrow.textX					= MENU_BUTTON_TEXT_X;
	s_playerEmotes.dnArrow.textY					= MENU_BUTTON_TEXT_Y;
	s_playerEmotes.dnArrow.textcolor				= CT_BLACK;
	s_playerEmotes.dnArrow.textcolor2				= CT_WHITE;

	s_playerEmotes.mainMenu.generic.type			= MTYPE_BITMAP;      
	s_playerEmotes.mainMenu.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_playerEmotes.mainMenu.generic.x				= 482;
	s_playerEmotes.mainMenu.generic.y				= 136;
	s_playerEmotes.mainMenu.generic.name			= BUTTON_GRAPHIC_LONGRIGHT;
	s_playerEmotes.mainMenu.generic.id				= ID_MAINMENU;
	s_playerEmotes.mainMenu.generic.callback		= PlayerEmotes_Event;
	s_playerEmotes.mainMenu.width					= MENU_BUTTON_MED_WIDTH;
	s_playerEmotes.mainMenu.height					= MENU_BUTTON_MED_HEIGHT;
	s_playerEmotes.mainMenu.color					= CT_DKPURPLE1;
	s_playerEmotes.mainMenu.color2					= CT_LTPURPLE1;
	s_playerEmotes.mainMenu.textX					= MENU_BUTTON_TEXT_X;
	s_playerEmotes.mainMenu.textY					= MENU_BUTTON_TEXT_Y;
	if ( !ingameFlag || !s_playerEmotes.fromConsole)
		s_playerEmotes.mainMenu.textEnum				= MBT_MAINMENU;
	else
		s_playerEmotes.mainMenu.textEnum				= MBT_INGAMERESUME;
	s_playerEmotes.mainMenu.textcolor				= CT_BLACK;
	s_playerEmotes.mainMenu.textcolor2				= CT_WHITE;
	
	s_playerEmotes.modelOffset.generic.type			= MTYPE_FIELD;
	s_playerEmotes.modelOffset.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_playerEmotes.modelOffset.generic.x			= 351;
	s_playerEmotes.modelOffset.generic.y			= 225;
	s_playerEmotes.modelOffset.generic.name			= menu_normal_text[ MNT_MODELOFFSET ]; //TiM : This var was untouched from Q3, so I've modded it for RPG-X functionality
	s_playerEmotes.modelOffset.field.widthInChars	= 14;
	s_playerEmotes.modelOffset.field.maxchars		= 5;
	s_playerEmotes.modelOffset.field.titleEnum		= MBT_MODEL_OFFSET;
	s_playerEmotes.modelOffset.field.textcolor		= CT_WHITE; //CT_DKGOLD1
	s_playerEmotes.modelOffset.field.textcolor2		= CT_WHITE; //CT_DKGOLD1
	s_playerEmotes.modelOffset.field.style			= UI_CENTER | UI_SMALLFONT; //Due to Raven's hacky nature, and my exploiting it therefore, SMALLFONT MUST accompany CENTER

	s_playerEmotes.emoteBind.generic.type			= MTYPE_ACTION;
	s_playerEmotes.emoteBind.generic.flags			= QMF_CENTER_JUSTIFY | QMF_GRAYED | QMF_INACTIVE;
	s_playerEmotes.emoteBind.generic.x				= 351;
	s_playerEmotes.emoteBind.generic.y				= 291;
	s_playerEmotes.emoteBind.generic.id				= ID_BIND_EMOTE;
	s_playerEmotes.emoteBind.generic.callback		= PlayerEmotes_Event;
	s_playerEmotes.emoteBind.generic.ownerdraw		= PlayerEmotes_DrawBinding;
	s_playerEmotes.emoteBind.textEnum				= MBT_KEY_BIND;
	s_playerEmotes.emoteBind.width					= 133;
	s_playerEmotes.emoteBind.height					= 39;
	s_playerEmotes.emoteBind.textY					= 2;

	s_playerEmotes.addFav.generic.type				= MTYPE_BITMAP;
	s_playerEmotes.addFav.generic.flags				= QMF_GRAYED | QMF_INACTIVE;
	s_playerEmotes.addFav.generic.x					= 273;
	s_playerEmotes.addFav.generic.y					= 355;
	s_playerEmotes.addFav.generic.name				= GRAPHIC_SQUARE;
	s_playerEmotes.addFav.generic.id				= ID_FAV_EMOTE;
	s_playerEmotes.addFav.generic.callback			= PlayerEmotes_Event;
	s_playerEmotes.addFav.width						= 157;
	s_playerEmotes.addFav.height					= 19;
	s_playerEmotes.addFav.color						= CT_DKPURPLE1;
	s_playerEmotes.addFav.color2					= CT_LTPURPLE1;
	s_playerEmotes.addFav.textX						= MENU_BUTTON_TEXT_X;
	s_playerEmotes.addFav.textY						= MENU_BUTTON_TEXT_Y;
	s_playerEmotes.addFav.textEnum					= MBT_FAV_EMOTE;
	s_playerEmotes.addFav.textcolor					= CT_BLACK;
	s_playerEmotes.addFav.textcolor2				= CT_WHITE;

	s_playerEmotes.playEmote.generic.type			= MTYPE_BITMAP;
	s_playerEmotes.playEmote.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_playerEmotes.playEmote.generic.x				= 273;
	s_playerEmotes.playEmote.generic.y				= 381;
	s_playerEmotes.playEmote.generic.name			= GRAPHIC_SQUARE;
	s_playerEmotes.playEmote.generic.id				= ID_DO_EMOTE;
	s_playerEmotes.playEmote.generic.callback		= PlayerEmotes_Event;
	s_playerEmotes.playEmote.width					= 157;
	s_playerEmotes.playEmote.height					= 43;
	s_playerEmotes.playEmote.color					= CT_DKPURPLE1;
	s_playerEmotes.playEmote.color2					= CT_LTPURPLE1;
	s_playerEmotes.playEmote.textX					= MENU_BUTTON_TEXT_X;
	s_playerEmotes.playEmote.textY					= MENU_BUTTON_TEXT_Y;
	s_playerEmotes.playEmote.textEnum				= MBT_DO_EMOTE;
	s_playerEmotes.playEmote.textcolor				= CT_BLACK;
	s_playerEmotes.playEmote.textcolor2				= CT_WHITE;

	//Spinbox for player model
	s_playerEmotes.playerMdl.generic.type			= MTYPE_BITMAP;
	s_playerEmotes.playerMdl.generic.flags			= QMF_SILENT; //INACTIVE
	s_playerEmotes.playerMdl.generic.callback		= PlayerEmotes_SpinPlayer;
	s_playerEmotes.playerMdl.generic.x				= 82+363; //440 //25
	s_playerEmotes.playerMdl.generic.y				= 158; //95
	s_playerEmotes.playerMdl.width					= 164; //32*6.6 //211.2 //246.2
	s_playerEmotes.playerMdl.height					= 276; //56*6.6 //369.6 //404.6

	s_playerEmotes.scrollBar.generic.type			= MTYPE_ACTION;
	s_playerEmotes.scrollBar.generic.flags			= QMF_INACTIVE | QMF_HIDDEN;
	s_playerEmotes.scrollBar.generic.x				= 241;
	s_playerEmotes.scrollBar.generic.y				= 204;
	s_playerEmotes.scrollBar.generic.id				= ID_SCROLLBAR;
	s_playerEmotes.scrollBar.generic.ownerdraw		= PlayerEmotes_DrawScrollBar;
	s_playerEmotes.scrollBar.width					= 18;
	s_playerEmotes.scrollBar.height					= MIN_SCROLLHEIGHT;
	s_playerEmotes.scrollBar.color					= CT_DKPURPLE1;
	s_playerEmotes.scrollBar.color2					= CT_LTPURPLE1;

	Menu_AddItem( &s_playerEmotes.menu, &s_playerEmotes.recentFilter );
	Menu_AddItem( &s_playerEmotes.menu, &s_playerEmotes.favoritesFilter );
	Menu_AddItem( &s_playerEmotes.menu, &s_playerEmotes.viewAllFilter );
	Menu_AddItem( &s_playerEmotes.menu, &s_playerEmotes.sittingFilter );
	Menu_AddItem( &s_playerEmotes.menu, &s_playerEmotes.consoleFilter );
	Menu_AddItem( &s_playerEmotes.menu, &s_playerEmotes.gestureFilter );
	Menu_AddItem( &s_playerEmotes.menu, &s_playerEmotes.fullMotionFilter );
	Menu_AddItem( &s_playerEmotes.menu, &s_playerEmotes.injuredFilter );
	Menu_AddItem( &s_playerEmotes.menu, &s_playerEmotes.miscFilter );

	Menu_AddItem( &s_playerEmotes.menu, &s_playerEmotes.upArrow );
	Menu_AddItem( &s_playerEmotes.menu, &s_playerEmotes.scrollBar );
	Menu_AddItem( &s_playerEmotes.menu, &s_playerEmotes.dnArrow );

	for (i = 0; i < MAX_MENULISTITEMS; i++ ) {
		Menu_AddItem( &s_playerEmotes.menu, &s_playerEmotes.emotesMenu[i] );
	}

	Menu_AddItem( &s_playerEmotes.menu, &s_playerEmotes.modelOffset );
	Menu_AddItem( &s_playerEmotes.menu, &s_playerEmotes.emoteBind );
	Menu_AddItem( &s_playerEmotes.menu, &s_playerEmotes.addFav );
	Menu_AddItem( &s_playerEmotes.menu, &s_playerEmotes.playEmote );

	Menu_AddItem( &s_playerEmotes.menu, &s_playerEmotes.playerMdl );
	Menu_AddItem( &s_playerEmotes.menu, &s_playerEmotes.mainMenu );

	//Emote data initialization
	//trap_Cvar_VariableStringBuffer( "modelOffset", modelOffset, sizeof( modelOffset ) );
	s_playerEmotes.prevOffset = (int)trap_Cvar_VariableValue("modelOffset");
	Q_strncpyz( s_playerEmotes.modelOffset.field.buffer, va("%i", s_playerEmotes.prevOffset), s_playerEmotes.modelOffset.field.maxchars );

	s_playerEmotes.selectedEmote = -1;

	//cheesy hack, but it works. if there's nothing in the recent array, do display all
	for ( i = 1; i <= NUM_CVAR_STORES; i++ ) {
		if ( (int)trap_Cvar_VariableValue( va( "ui_recentEmote%i", i ) ) >= 0 ) {
			showRecent = qtrue;
			break;
		}
	}

	if ( !showRecent ) {
		Menu_SetCursorToItem( &s_playerEmotes.menu, &s_playerEmotes.viewAllFilter );
		PlayerEmotes_FillEmotesArray( ID_VIEWALL );
	}
	else {
		Menu_SetCursorToItem( &s_playerEmotes.menu, &s_playerEmotes.recentFilter );
		PlayerEmotes_FillEmotesArray( ID_RECENT );
	}

	PlayerEmotes_SetupScrollBar( &s_playerEmotes.scrollBar );
	PlayerEmotes_UpdateScrollBar( &s_playerEmotes.scrollBar );
}

/*
===============
UI_EmotesMenu
===============
*/
void UI_EmotesMenu( qboolean fromConsole ) {
	memset( &s_playerEmotes, 0, sizeof( s_playerEmotes ) );

	s_playerEmotes.fromConsole = fromConsole;
	PlayerEmotes_Init();

	ingameFlag = qtrue;
	Mouse_Show();

	UI_PushMenu( &s_playerEmotes.menu );
}
