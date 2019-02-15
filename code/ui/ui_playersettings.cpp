// Copyright (C) 1999-2000 Id Software, Inc.
//
#include "ui_local.h"
#include "ui_logger.h"

enum ui_playersettingsIDs_e {
	ID_NAME = 9,
	ID_CLASS,
	ID_RANK,
	ID_EFFECTS,
	ID_BACK,

	// If this gets changed, also change it in ui_playermodel
	ID_MODEL,
	ID_DATA,
	ID_MAINMENU,
	ID_INGAMEMENU,

	ID_CLASSTXT,
	ID_RANKTXT,
	ID_AGE,
	ID_RACE,
	ID_HEIGHT,
	ID_WEIGHT,

	ID_RANKSETS,
	ID_CLASSSETS,

	ID_COMMIT,

	//ID_LOADPROFILE,
	//ID_SAVEPROFILE
};

enum ui_playersettingsLimits_e {
	MAX_PROFILES = 64
};

static const char PIC_MONBAR2[] = "menu/common/monbar_2.tga";
static const char PIC_SLIDER[] = "menu/common/slider.tga";

extern	sfxHandle_t		menu_out_sound;

static const char PROFILE_PATH[] = "profiles";

static void PlayerSettings_SetMenuItems(void);
//TiM - Ermm....
//We COULD actually base these directly off the cvar values... probably a bit of a mem saver then lol

typedef struct {
	//char	name[36];
	int32_t		classIndex;
	int32_t		rankIndex;

	//char	age[36];
	//char	race[36];

	//float	height;
	//float	weight;
} initialData_t;

typedef struct {
	menuframework_s		menu;

	int32_t					prevMenu;

	//list data
	char				profileList[MAX_PROFILES][32];
	char*				profileListPtr[MAX_PROFILES];

	//profile loader
	/*int32_t					numProfiles;
	menulist_s			profiles;
	menubitmap_s		loadProfile;

	//profile saver
	menufield_s			profileName;
	menubitmap_s		saveProfile;*/

	menubitmap_s		player;
	menubitmap_s		mainmenu;
	menubitmap_s		back;
	menufield_s			name;

	menulist_s			pClass;						//Menu - Class Spin
	menufield_s			classTxt;					//Menu - Class manual input - coz of servers changing the name
	menulist_s			pRank;						//Menu - Rank Spin
	menufield_s			rankTxt;					//Menu - Rank manual input.  What the hey lol
	menufield_s			age;						//Menu - Ye olde player
	menufield_s			race;						//Menu - Ye alien player
	menuslider_s		height;						//Menu - Ye pipsqueak player
	menuslider_s		weight;						//Menu - Ye fatass player

	menulist_s			rankSets;					//If we have more than one rankset, add this scroller so players can choose what they want
	menulist_s			classSets;

	menubitmap_s		commit;						//TiM - Manually have to confirm change now.  Makes stuff feel more secure. :)

	menubitmap_s		model;
	menubitmap_s		data;
	menubitmap_s		item_null;

	qhandle_t			corner_ul_4_4;
	qhandle_t			corner_ur_4_4;
	qhandle_t			corner_ll_4_4;
	qhandle_t			corner_ll_4_18;
	qhandle_t			corner_lr_4_18;

	//menutext_s			playername;					//Menu - PlayerName
	qhandle_t			fxBasePic;
	qhandle_t			fxPic[7];
	playerInfo_t		playerinfo;
	int32_t					current_fx;
	char				playerModel[MAX_QPATH];

	char				*rankList[MAX_RANKS + 1]; //TiM: Local buffer for ranks data ( + 1 slot for 'Other' field + 1 for null terminator)
	int32_t					numRanks;
	char				*rankSetList[MAX_RANKSETS + 1];
	int32_t					numRankSets;

	qboolean			settingsChanged;		//if settings have been altered, enable this so as to enable the 'commit' button
	qboolean			displayChangedSettings;

	initialData_t		initData;				//TiM | Storage for all our initial data. So if/when we change the data, we'll know exactly when to make the commit button solid

	char				*classNameList[MAX_CLASSES + 1]; //+2 = other + null
	int32_t					numClasses;

	char				*classSetList[MAX_CLASSSETS + 1];
	int32_t					numClassSets;
} playersettings_t;

static playersettings_t	s_playersettings;

/*
=======================
PlayerSettings_SettingsAreDifferent

TiM: Is called each time an action
event is received so as to check to see if any settings
have been changed.  If they have, make the 'apply changes'
button appear.
=======================
*/

static qboolean PlayerSettings_SettingsAreDifferent(void) {
	UI_LogFuncBegin();
	qboolean	endResult = qfalse;

	//Name
	if (Q_stricmp(s_playersettings.name.field.buffer, UI_Cvar_VariableString("name"))) {
		//UI_Logger( LL_DEBUG, "Name changed!\n");
		endResult = qtrue;
	}

	//Age
	if (Q_stricmp(s_playersettings.age.field.buffer, UI_Cvar_VariableString("age"))) {
		//UI_Logger( LL_DEBUG, "Age changed!\n");
		endResult = qtrue;
	}

	//Race
	if (Q_stricmp(s_playersettings.race.field.buffer, UI_Cvar_VariableString("race"))) {
		//UI_Logger( LL_DEBUG, "Race changed!\n");
		endResult = qtrue;
	}

	//playerClass field
	if (s_playersettings.pClass.curvalue != s_playersettings.initData.classIndex) {
		//UI_Logger( LL_DEBUG, "Class changed!\n");
		endResult = qtrue;
	}

	//playerRank field
	if (s_playersettings.pRank.curvalue != s_playersettings.initData.rankIndex) {
		//UI_Logger( LL_DEBUG, "Rank changed!\n");
		endResult = qtrue;
	}

	//height... hackily normalized lol
	if (Q_stricmp(va("%2.5f", s_playersettings.height.curvalue), va("%2.5f", atof(UI_Cvar_VariableString("height"))))) {
		//UI_Logger( LL_DEBUG, "Height changed!\n");
		endResult = qtrue;
	}

	//weight... see above rofl
	if (Q_stricmp(va("%2.5f", s_playersettings.weight.curvalue), va("%2.5f", atof(UI_Cvar_VariableString("weight"))))) {
		//UI_Logger( LL_DEBUG, "Weight changed!\n");
		endResult = qtrue;
	}

	//Class Text field
	if (s_playersettings.pClass.curvalue == s_playersettings.numClasses && Q_stricmp(s_playersettings.classTxt.field.buffer, UI_Cvar_VariableString("ui_playerClass"))) {
		//UI_Logger( LL_DEBUG, "Class text changed!\n");
		endResult = qtrue;
	}

	//Rank Text field
	if (s_playersettings.pRank.curvalue == s_playersettings.numRanks && Q_stricmp(s_playersettings.rankTxt.field.buffer, UI_Cvar_VariableString("ui_playerRank"))) {
		//UI_Logger( LL_DEBUG, "Rank text changed!\n");
		endResult = qtrue;
	}
	UI_LogFuncEnd();
	return endResult;
}

/*
=================
PlayerSettings_DrawPlayer
RPG-X: TiM - Modified it so it actually fit in the box,
and had an appropriately set FOV
=================
*/
static void PlayerSettings_DrawPlayer(void *self)
{
	UI_LogFuncBegin();
	menubitmap_s	*b;
	vec3_t			viewangles;
	vec3_t			origin = { -40, 2.5, -4 }; //-3.8
	char			buf[MAX_QPATH];

	viewangles[YAW] = uis.lastYaw; //180 - 30
	viewangles[PITCH] = 0;
	viewangles[ROLL] = 0;

	trap_Cvar_VariableStringBuffer("model", buf, sizeof(buf));

	if (Q_stricmp(buf, s_playersettings.playerModel)) {

		UI_PlayerInfo_SetModel(&s_playersettings.playerinfo, buf);
		strcpy(s_playersettings.playerModel, buf);

		uis.lastYaw = viewangles[YAW] = 180 - 30; //180 - 30
	}

	UI_PlayerInfo_SetInfo(&s_playersettings.playerinfo, BOTH_WALK1, BOTH_WALK1, viewangles, vec3_origin, WP_0, s_playersettings.height.curvalue, s_playersettings.weight.curvalue, qfalse);

	b = (menubitmap_s*)self;
	UI_DrawPlayer((float)b->generic.x, (float)b->generic.y, (float)b->width, (float)b->height, origin, &s_playersettings.playerinfo, (int32_t)(uis.realtime / 1.5));

	UI_LogFuncEnd();
}


/*
=================
PlayerSettings_SaveChanges
=================
*/
static void PlayerSettings_SaveChanges(void)
{
	UI_LogFuncBegin();
	// name
	trap_Cvar_Set("name", s_playersettings.name.field.buffer);

	//class
	if (s_playersettings.pClass.curvalue == s_playersettings.numClasses) {
		if (s_playersettings.classTxt.field.buffer[0]) {
			if (!ingameFlag)
				trap_Cvar_Set("ui_playerClass", s_playersettings.classTxt.field.buffer);
			else
				trap_Cmd_ExecuteText(EXEC_APPEND, va("class %s\n", s_playersettings.classTxt.field.buffer));
		}
	}
	else {
		if (!ingameFlag)
			trap_Cvar_Set("ui_playerClass", uis.classData[s_playersettings.pClass.curvalue].classNameConsole);
		else
			trap_Cmd_ExecuteText(EXEC_APPEND, va("class %s\n", uis.classData[s_playersettings.pClass.curvalue].classNameConsole));
	}

	//Save current class index to the initData
	s_playersettings.initData.classIndex = s_playersettings.pClass.curvalue;


	//rank
	if (s_playersettings.pRank.curvalue == s_playersettings.numRanks) {
		if (s_playersettings.rankTxt.field.buffer[0]) {
			if (!ingameFlag)
				trap_Cvar_Set("ui_playerRank", s_playersettings.rankTxt.field.buffer);
			else
				trap_Cmd_ExecuteText(EXEC_APPEND, va("rank %s\n", s_playersettings.rankTxt.field.buffer));
		}
	}
	else {
		if (!ingameFlag)
			trap_Cvar_Set("ui_playerRank", Q_strlwr(uis.rankSet.rankNames[s_playersettings.pRank.curvalue].consoleName));
		else
			trap_Cmd_ExecuteText(EXEC_APPEND, va("rank %s\n", uis.rankSet.rankNames[s_playersettings.pRank.curvalue].consoleName));
	}

	uis.currentRank = s_playersettings.pRank.curvalue;

	//Save teh rank index to the init data set
	s_playersettings.initData.rankIndex = s_playersettings.pRank.curvalue;

	//age
	trap_Cvar_Set("age", s_playersettings.age.field.buffer);
	//race
	trap_Cvar_Set("race", s_playersettings.race.field.buffer);

	//height
	trap_Cvar_SetValue("height", s_playersettings.height.curvalue);
	//weight
	trap_Cvar_SetValue("weight", s_playersettings.weight.curvalue);

	//Display the 'applied msg'
	s_playersettings.displayChangedSettings = qtrue;

	UI_LogFuncEnd();
}


/*
=================
PlayerSettings_MenuKey
=================
*/
static sfxHandle_t PlayerSettings_MenuKey(int32_t key)
{
	return Menu_DefaultKey(&s_playersettings.menu, key);
}


/*
=================
PlayerSettings_SetMenuItems
=================
*/
static void PlayerSettings_SetMenuItems(void)
{
	UI_LogFuncBegin();
	int32_t		i;

	// Player Name
	Q_strncpyz(s_playersettings.name.field.buffer, UI_Cvar_VariableString("name"), sizeof(s_playersettings.name.field.buffer));

	//Player class
	{
		qboolean classFound = qfalse; //If we don't find a match, we'll use this to know

		s_playersettings.pClass.curvalue = 0;

		for (i = 0; i < s_playersettings.numClasses; i++) {
			if (!Q_stricmp(uis.classData[i].classNameConsole, UI_Cvar_VariableString("ui_playerClass"))) {
				s_playersettings.pClass.curvalue = i;
				classFound = qtrue;
				break;
			}
		}

		//No class found.  Set the field to 'other' and display the text field
		if (!classFound) {
			s_playersettings.pClass.curvalue = s_playersettings.numClasses;//CLASS_OTHER;

			s_playersettings.classTxt.generic.flags &= ~QMF_HIDDEN;

			memset(&s_playersettings.classTxt.field.buffer, 0, sizeof(s_playersettings.classTxt.field.buffer));
			Q_strncpyz(s_playersettings.classTxt.field.buffer, UI_Cvar_VariableString("ui_playerClass"), sizeof(s_playersettings.classTxt.field.buffer));
		}

		s_playersettings.initData.classIndex = s_playersettings.pClass.curvalue;
	}


	//Player Rank
	{
		qboolean	foundRank = qfalse; //0 is a valid rank, so we'll use this to test instead.
		//I would have simply set curvalue to -1 to test, but I was unsure how the controls API would react to a negative value there

		s_playersettings.pRank.curvalue = 0;

		//Loop thru our rank set, and compare what's in the console to it
		for (i = 0; i < MAX_RANKS; i++) {
			if (!Q_stricmp(UI_Cvar_VariableString("ui_playerRank"), uis.rankSet.rankNames[i].consoleName)) {
				s_playersettings.pRank.curvalue = i;
				foundRank = qtrue;
				break;
			}
		}

		//TiM - no rank was found, so set our rank field to 'other'
		if (!foundRank) {
			//set value to the last in the cell
			s_playersettings.pRank.curvalue = s_playersettings.numRanks;

			//Init and show the text field control
			s_playersettings.rankTxt.generic.flags &= ~QMF_HIDDEN;

			memset(&s_playersettings.rankTxt.field.buffer, 0, sizeof(s_playersettings.rankTxt.field.buffer));
			Q_strncpyz(s_playersettings.rankTxt.field.buffer, UI_Cvar_VariableString("ui_playerRank"), sizeof(s_playersettings.rankTxt.field.buffer));
		}

		s_playersettings.initData.rankIndex = s_playersettings.pRank.curvalue;
	}


	//Player Age
	Q_strncpyz(s_playersettings.age.field.buffer, UI_Cvar_VariableString("age"), sizeof(s_playersettings.age.field.buffer));

	//Player Race
	Q_strncpyz(s_playersettings.race.field.buffer, UI_Cvar_VariableString("race"), sizeof(s_playersettings.race.field.buffer));

	//Player Height
	{
		float height = trap_Cvar_VariableValue("height");

		//If height was defined, but filled with no value
		//We don't want infinitely flat players rofl
		if (!height)
			height = 1.0f;

		s_playersettings.height.curvalue = height;
	}


	//Player Weight
	{
		float weight = trap_Cvar_VariableValue("weight");

		//If height was defined, but filled with no value
		//We don't want infinitely thin players rofl
		if (!weight)
			weight = 1.0f;

		s_playersettings.weight.curvalue = weight;
	}

	UI_LogFuncEnd();
}

/*
=================
PlayerSettings_SpinPlayer
=================
*/
static void PlayerSettings_SpinPlayer(void* ptr, int32_t event)
{
	UI_LogFuncBegin();
	if (event == QM_ACTIVATED)
	{
		uis.spinView = qtrue;
		uis.cursorpx = uis.cursorx;
	}
	UI_LogFuncEnd();
}

/*
=================
PlayerSettings_MenuEvent
=================
*/
static void PlayerSettings_MenuEvent(void* ptr, int32_t event)
{
	UI_LogFuncBegin();
	if (event != QM_ACTIVATED)
	{
		UI_LogFuncEnd();
		return;
	}

	s_playersettings.displayChangedSettings = qfalse;
	s_playersettings.settingsChanged = PlayerSettings_SettingsAreDifferent();

	switch (((menucommon_s*)ptr)->id)
	{

	case ID_MODEL:
		UI_PopMenu();
		//PlayerSettings_SaveChanges();
		UI_PlayerModelMenu(s_playersettings.prevMenu);
		break;

	case ID_BACK:
		//PlayerSettings_SaveChanges();
		UI_PopMenu();
		break;

	case ID_MAINMENU:
		//PlayerSettings_SaveChanges();
		UI_MainMenu();
		break;

	case ID_INGAMEMENU:
		//PlayerSettings_SaveChanges();
		UI_InGameMenu();
		break;

	case ID_COMMIT:
		PlayerSettings_SaveChanges();
		s_playersettings.settingsChanged = qfalse;
		break;

	case ID_CLASS:
		if (s_playersettings.pClass.curvalue == s_playersettings.numClasses) {
			s_playersettings.classTxt.generic.flags &= ~QMF_HIDDEN;

			memset(&s_playersettings.classTxt.field.buffer, 0, sizeof(s_playersettings.classTxt.field.buffer));
			Q_strncpyz(s_playersettings.classTxt.field.buffer, UI_Cvar_VariableString("ui_playerClass"), sizeof(s_playersettings.classTxt.field.buffer));
		}
		else {
			memset(&s_playersettings.classTxt.field.buffer, 0, sizeof(s_playersettings.classTxt.field.buffer));
			s_playersettings.classTxt.generic.flags |= QMF_HIDDEN;
		}

		break;

	case ID_RANK:
		if (s_playersettings.pRank.curvalue == s_playersettings.numRanks) {
			s_playersettings.rankTxt.generic.flags &= ~QMF_HIDDEN;

			memset(&s_playersettings.rankTxt.field.buffer, 0, sizeof(s_playersettings.rankTxt.field.buffer));
			Q_strncpyz(s_playersettings.rankTxt.field.buffer, UI_Cvar_VariableString("ui_playerRank"), sizeof(s_playersettings.rankTxt.field.buffer));
		}
		else {
			memset(&s_playersettings.rankTxt.field.buffer, 0, sizeof(s_playersettings.rankTxt.field.buffer));
			s_playersettings.rankTxt.generic.flags |= QMF_HIDDEN;
		}
		break;

	case ID_RANKSETS:
		if (!ingameFlag) {
			if (s_playersettings.rankSetList[s_playersettings.rankSets.curvalue][0])
				trap_Cvar_Set("ui_currentRankSet", s_playersettings.rankSetList[s_playersettings.rankSets.curvalue]);

			UI_InitRanksData(UI_Cvar_VariableString("ui_currentRankSet"));
			s_playersettings.numRanks = UI_PopulateRanksArray(s_playersettings.rankList);

			s_playersettings.pRank.numitems = s_playersettings.numRanks;
		}
		break;

	case ID_CLASSSETS:
		if (!ingameFlag) {
			if (s_playersettings.classSetList[s_playersettings.classSets.curvalue][0])
				trap_Cvar_Set("ui_currentClassSet", s_playersettings.classSetList[s_playersettings.classSets.curvalue]);

			UI_InitClassData(UI_Cvar_VariableString("ui_currentClassSet"));
			s_playersettings.numClasses = UI_PopulateClassArray(s_playersettings.classNameList);

			s_playersettings.pClass.numitems = s_playersettings.numClasses;
		}
		break;

	}
	UI_LogFuncEnd();
}


/*
=================
PlayerSettingsMenu_Graphics
=================
*/
void PlayerSettingsMenu_Graphics(void)
{
	UI_LogFuncBegin();
	// Draw the basic screen layout
	UI_MenuFrame2(&s_playersettings.menu);

	trap_R_SetColor(colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(30, 203, 47, 186, uis.whiteShader);	// Middle left line

	//TiM - eh later
	// Frame around model pictures
	trap_R_SetColor(colorTable[CT_LTORANGE]);
	UI_DrawHandlePic(97, 94, 8, -32, s_playersettings.corner_ll_4_18);		// UL Corner	y = 139 x=114
	UI_DrawHandlePic(97, 312, 8, 32, s_playersettings.corner_ll_4_18);		// LL Corner	x=114
	UI_DrawHandlePic(421, 94, 8, -32, s_playersettings.corner_lr_4_18);		// UR Corner	y= 139
	UI_DrawHandlePic(421, 312, 8, 32, s_playersettings.corner_lr_4_18);		// LR Corner
	UI_DrawHandlePic(97, 125, 4, 188, uis.whiteShader);						// Left side	x=114 y=170 h=97
	UI_DrawHandlePic(424, 125, 4, 188, uis.whiteShader);						// Right side
	UI_DrawHandlePic(103, 106, 320, 18, uis.whiteShader);						// Top			y = 151 x =120
	//UI_DrawHandlePic(  103,		314,	310,	18,		uis.whiteShader);						// Bottom
	UI_DrawHandlePic(103, 314, 90, 18, uis.whiteShader);						//Bottom - Left
	UI_DrawHandlePic(332, 314, 90, 18, uis.whiteShader);						//Bottom - Right

	//line
	//UI_DrawHandlePic( 97, 383, 331, 2, uis.whiteShader );

	UI_DrawProportionalString(109, 108, menu_normal_text[MNT_CHANGEPLAYER], UI_SMALLFONT, colorTable[CT_BLACK]);	// Top

	trap_R_SetColor(colorTable[CT_DKGREY2]);
	UI_DrawHandlePic(439, 79, 151, 295, uis.whiteShader);	// Background

	// Frame around player model
	trap_R_SetColor(colorTable[CT_LTORANGE]);
	UI_DrawHandlePic(435, 50, 8, -32, s_playersettings.corner_ll_4_18);	// UL Corner
	UI_DrawHandlePic(435, 369, 8, 8, s_playersettings.corner_ll_4_4);	// LL Corner
	UI_DrawHandlePic(440, 62, 150, 18, uis.whiteShader);	// Top
	UI_DrawHandlePic(435, 79, 4, 295, uis.whiteShader);	// Left side
	UI_DrawHandlePic(440, 371, 150, 4, uis.whiteShader);	// Bottom

	// Left rounded ends for buttons
	trap_R_SetColor(colorTable[s_playersettings.mainmenu.color]);
	UI_DrawHandlePic(s_playersettings.mainmenu.generic.x - 14, s_playersettings.mainmenu.generic.y,
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	trap_R_SetColor(colorTable[s_playersettings.back.color]);
	UI_DrawHandlePic(s_playersettings.back.generic.x - 14, s_playersettings.back.generic.y,
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	trap_R_SetColor(colorTable[s_playersettings.data.color]);
	UI_DrawHandlePic(s_playersettings.data.generic.x - 14, s_playersettings.data.generic.y,
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	trap_R_SetColor(colorTable[s_playersettings.model.color]);
	UI_DrawHandlePic(s_playersettings.model.generic.x - 14, s_playersettings.model.generic.y,
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	trap_R_SetColor(colorTable[s_playersettings.mainmenu.color]);
	UI_DrawHandlePic(s_playersettings.mainmenu.generic.x - 14, s_playersettings.mainmenu.generic.y,
		MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	UI_DrawProportionalString(74, 28, "881", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 150, "2445", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 206, "600", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 395, "3-44", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);

	//TiM - Player weight and height parameters
	{
		float	ht = s_playersettings.height.curvalue * (float)BASE_HEIGHT;
		float	wt = s_playersettings.weight.curvalue * s_playersettings.height.curvalue * (float)BASE_WEIGHT;
		char	htStr[12];
		char	wtStr[12];

		if (s_playersettings.playerinfo.gender == GENDER_FEMALE)
			wt *= (float)FEMALE_OFFSET;

		Com_sprintf(htStr, sizeof(htStr), "%4.2f%s", ht, HEIGHT_UNIT);
		Com_sprintf(wtStr, sizeof(wtStr), "%4.2f%s", wt, WEIGHT_UNIT);

		UI_DrawProportionalString(359, 259, htStr, UI_LEFT | UI_SMALLFONT, colorTable[CT_LTGOLD1]);
		UI_DrawProportionalString(359, 285, wtStr, UI_LEFT | UI_SMALLFONT, colorTable[CT_LTGOLD1]);
	}

	if (s_playersettings.settingsChanged) {
		s_playersettings.commit.generic.flags &= ~(QMF_GRAYED | QMF_INACTIVE);
		s_playersettings.commit.generic.flags |= (QMF_HIGHLIGHT_IF_FOCUS);
	}
	else {
		s_playersettings.commit.generic.flags = (QMF_GRAYED | QMF_INACTIVE);
	}

	{
		int32_t y = 338;

		if (s_playersettings.rankSets.numitems > 1)
			y += 24;

		if (s_playersettings.displayChangedSettings)
			UI_DrawProportionalString(260, y, menu_normal_text[MNT_CHANGEAPPLIED], UI_CENTER | UI_SMALLFONT, colorTable[CT_LTGOLD1]);
	}

	UI_DrawProportionalString(444, 63, s_playersettings.name.field.buffer, UI_LEFT | UI_SMALLFONT, colorTable[CT_WHITE]);

	UI_LogFuncEnd();

}

/*
=================
PlayerSettings_MenuDraw
=================
*/
static void PlayerSettings_MenuDraw(void)
{
	UI_LogFuncBegin();
	PlayerSettingsMenu_Graphics();
	Menu_Draw(&s_playersettings.menu);
	UI_LogFuncEnd();
}


/*
=================
PlayerSettings_MenuInit
=================
*/
static void PlayerSettings_MenuInit(int32_t menuFrom)
{
	UI_LogFuncBegin();
	int32_t		y;
	int32_t		i;

	memset(&s_playersettings, 0, sizeof(playersettings_t));

	s_playersettings.prevMenu = menuFrom;

	PlayerSettings_Cache();

	//PlayerSettings_LoadProfiles();

	if (ingameFlag) {
		char	info[MAX_TOKEN_CHARS];

		trap_GetConfigString(CS_SERVERINFO, info, sizeof(info));
		UI_InitRanksData(Info_ValueForKey(info, "rpg_rankSet"));
		UI_InitClassData(Info_ValueForKey(info, "rpg_classSet"));

		//UI_LoadClassString();
	}

	//load classes
	s_playersettings.numClasses = UI_PopulateClassArray(s_playersettings.classNameList);

	//load class sets
	s_playersettings.numClassSets = UI_PopulateClassSetArray(s_playersettings.classSetList);
	s_playersettings.classSetList[s_playersettings.numClassSets] = 0;

	//Fill teh ranks array with the current loaded set and add 'other' to the end
	s_playersettings.numRanks = UI_PopulateRanksArray(s_playersettings.rankList);
	//s_playersettings.rankList[s_playersettings.numRanks] = "Other";
	s_playersettings.numRankSets = UI_PopulateRankSetArray(s_playersettings.rankSetList);

	s_playersettings.menu.key = PlayerSettings_MenuKey;
	s_playersettings.menu.wrapAround = qtrue;
	s_playersettings.menu.fullscreen = qtrue;
	s_playersettings.menu.draw = PlayerSettings_MenuDraw;
	s_playersettings.menu.descX = MENU_DESC_X;
	s_playersettings.menu.descY = MENU_DESC_Y;
	s_playersettings.menu.titleX = MENU_TITLE_X;
	s_playersettings.menu.titleY = MENU_TITLE_Y;
	s_playersettings.menu.titleI = MNT_CHANGEPLAYER_TITLE;
	s_playersettings.menu.footNoteEnum = MNT_CHANGEPLAYER;


	s_playersettings.mainmenu.generic.type = MTYPE_BITMAP;
	s_playersettings.mainmenu.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_playersettings.mainmenu.generic.x = 110;
	s_playersettings.mainmenu.generic.y = 391;
	s_playersettings.mainmenu.generic.name = BUTTON_GRAPHIC_LONGRIGHT;
	s_playersettings.mainmenu.generic.callback = PlayerSettings_MenuEvent;
	s_playersettings.mainmenu.width = MENU_BUTTON_MED_WIDTH;
	s_playersettings.mainmenu.height = MENU_BUTTON_MED_HEIGHT;
	s_playersettings.mainmenu.color = CT_DKPURPLE1;
	s_playersettings.mainmenu.color2 = CT_LTPURPLE1;

	if (!ingameFlag)
	{
		s_playersettings.mainmenu.textEnum = MBT_MAINMENU;
		s_playersettings.mainmenu.generic.id = ID_MAINMENU;
	}
	else
	{
		s_playersettings.mainmenu.textEnum = MBT_INGAMEMENU;
		s_playersettings.mainmenu.generic.id = ID_INGAMEMENU;
	}

	s_playersettings.mainmenu.textX = MENU_BUTTON_TEXT_X;
	s_playersettings.mainmenu.textY = MENU_BUTTON_TEXT_Y;
	s_playersettings.mainmenu.textcolor = CT_BLACK;
	s_playersettings.mainmenu.textcolor2 = CT_WHITE;

	s_playersettings.back.generic.type = MTYPE_BITMAP;
	s_playersettings.back.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_playersettings.back.generic.x = 110;
	s_playersettings.back.generic.y = 415;
	s_playersettings.back.generic.name = BUTTON_GRAPHIC_LONGRIGHT;
	s_playersettings.back.generic.id = ID_BACK;
	s_playersettings.back.generic.callback = PlayerSettings_MenuEvent;
	s_playersettings.back.width = MENU_BUTTON_MED_WIDTH;
	s_playersettings.back.height = MENU_BUTTON_MED_HEIGHT;
	s_playersettings.back.color = CT_DKPURPLE1;
	s_playersettings.back.color2 = CT_LTPURPLE1;
	s_playersettings.back.textX = MENU_BUTTON_TEXT_X;
	s_playersettings.back.textY = MENU_BUTTON_TEXT_Y;
	s_playersettings.back.textEnum = MBT_BACK;
	s_playersettings.back.textcolor = CT_BLACK;
	s_playersettings.back.textcolor2 = CT_WHITE;

	y = 134;//144;
	s_playersettings.name.generic.type = MTYPE_FIELD;
	s_playersettings.name.field.widthInChars = MAX_NAMELENGTH;
	s_playersettings.name.field.maxchars = MAX_NAMELENGTH;
	s_playersettings.name.generic.id = ID_NAME;
	s_playersettings.name.generic.callback = PlayerSettings_MenuEvent;
	s_playersettings.name.generic.x = 115 + 5 + UI_ProportionalStringWidth(menu_button_text[MBT_PLAYER_NAME][0], UI_SMALLFONT);//159;//180;
	s_playersettings.name.generic.y = y;//182;
	s_playersettings.name.field.style = UI_SMALLFONT;
	s_playersettings.name.field.titleEnum = MBT_PLAYER_NAME;
	s_playersettings.name.field.titlecolor = CT_LTGOLD1;
	s_playersettings.name.field.textcolor = CT_DKGOLD1;
	s_playersettings.name.field.textcolor2 = CT_LTGOLD1;

	y += 25;//3 * PROP_HEIGHT;
	s_playersettings.pClass.generic.type = MTYPE_SPINCONTROL;
	s_playersettings.pClass.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_playersettings.pClass.generic.id = ID_CLASS;
	s_playersettings.pClass.generic.callback = PlayerSettings_MenuEvent;
	s_playersettings.pClass.generic.x = 113;//134;
	s_playersettings.pClass.generic.y = y;//207;
	s_playersettings.pClass.numitems = MAX_CLASSES;
	s_playersettings.pClass.textEnum = MBT_CLASS;
	s_playersettings.pClass.textcolor = CT_BLACK;
	s_playersettings.pClass.textcolor2 = CT_WHITE;
	s_playersettings.pClass.color = CT_DKPURPLE1;
	s_playersettings.pClass.color2 = CT_LTPURPLE1;
	s_playersettings.pClass.width = 60;//80;
	s_playersettings.pClass.textX = 5;
	s_playersettings.pClass.textY = 2;
	s_playersettings.pClass.itemnames = (const char **)s_playersettings.classNameList;//playerClassList;

	s_playersettings.classTxt.generic.type = MTYPE_FIELD;
	s_playersettings.classTxt.generic.flags = QMF_HIDDEN;
	s_playersettings.classTxt.generic.callback = PlayerSettings_MenuEvent;
	s_playersettings.classTxt.generic.id = ID_CLASSTXT;
	s_playersettings.classTxt.field.widthInChars = 16;
	s_playersettings.classTxt.field.maxchars = MAX_NAMELENGTH;
	s_playersettings.classTxt.generic.x = 205 + 12 + UI_ProportionalStringWidth("Other", UI_SMALLFONT);//315;//159;//180;
	s_playersettings.classTxt.generic.y = y + 3;//118;//182;
	s_playersettings.classTxt.field.style = UI_SMALLFONT;
	s_playersettings.classTxt.field.titleEnum = MBT_OTHER;
	s_playersettings.classTxt.field.titlecolor = CT_LTGOLD1;
	s_playersettings.classTxt.field.textcolor = CT_DKGOLD1;
	s_playersettings.classTxt.field.textcolor2 = CT_LTGOLD1;

	y += 25;
	s_playersettings.pRank.generic.type = MTYPE_SPINCONTROL;
	s_playersettings.pRank.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_playersettings.pRank.generic.id = ID_RANK;
	s_playersettings.pRank.generic.callback = PlayerSettings_MenuEvent;
	s_playersettings.pRank.generic.x = 113;//134;
	s_playersettings.pRank.generic.y = y;//233;
	s_playersettings.pRank.numitems = s_playersettings.numRanks;
	s_playersettings.pRank.textEnum = MBT_RANK;
	s_playersettings.pRank.textcolor = CT_BLACK;
	s_playersettings.pRank.textcolor2 = CT_WHITE;
	s_playersettings.pRank.color = CT_DKPURPLE1;
	s_playersettings.pRank.color2 = CT_LTPURPLE1;
	s_playersettings.pRank.width = 60;//80;
	s_playersettings.pRank.textX = 5;
	s_playersettings.pRank.textY = 2;
	s_playersettings.pRank.itemnames = (const char**)s_playersettings.rankList; //prank_items_formal3;

	s_playersettings.rankTxt.generic.type = MTYPE_FIELD;
	s_playersettings.rankTxt.generic.flags = QMF_HIDDEN;
	s_playersettings.rankTxt.generic.callback = PlayerSettings_MenuEvent;
	s_playersettings.rankTxt.generic.id = ID_RANKTXT;
	s_playersettings.rankTxt.field.widthInChars = 16;
	s_playersettings.rankTxt.field.maxchars = MAX_NAMELENGTH;
	s_playersettings.rankTxt.generic.x = 205 + 12 + UI_ProportionalStringWidth(s_playersettings.rankList[s_playersettings.numRanks], UI_SMALLFONT);//315;
	s_playersettings.rankTxt.generic.y = y + 3;
	s_playersettings.rankTxt.field.style = UI_SMALLFONT;
	s_playersettings.rankTxt.field.titleEnum = MBT_OTHER;
	s_playersettings.rankTxt.field.titlecolor = CT_LTGOLD1;
	s_playersettings.rankTxt.field.textcolor = CT_DKGOLD1;
	s_playersettings.rankTxt.field.textcolor2 = CT_LTGOLD1;

	y += 25;
	s_playersettings.age.generic.type = MTYPE_FIELD;
	s_playersettings.age.generic.id = ID_AGE;
	s_playersettings.age.generic.callback = PlayerSettings_MenuEvent;
	s_playersettings.age.field.widthInChars = MAX_NAMELENGTH;
	s_playersettings.age.field.maxchars = MAX_NAMELENGTH;
	s_playersettings.age.generic.x = 115 + 5 + UI_ProportionalStringWidth(menu_button_text[MBT_AGE][0], UI_SMALLFONT);//159;
	s_playersettings.age.generic.y = y + 3;
	s_playersettings.age.field.style = UI_SMALLFONT;
	s_playersettings.age.field.titleEnum = MBT_AGE;
	s_playersettings.age.field.titlecolor = CT_LTGOLD1;
	s_playersettings.age.field.textcolor = CT_DKGOLD1;
	s_playersettings.age.field.textcolor2 = CT_LTGOLD1;

	y += 25;
	s_playersettings.race.generic.type = MTYPE_FIELD;
	s_playersettings.race.generic.id = ID_RACE;
	s_playersettings.race.generic.callback = PlayerSettings_MenuEvent;
	s_playersettings.race.field.widthInChars = MAX_NAMELENGTH;
	s_playersettings.race.field.maxchars = MAX_NAMELENGTH;
	s_playersettings.race.generic.x = 115 + 5 + UI_ProportionalStringWidth(menu_button_text[MBT_RACE][0], UI_SMALLFONT);//159;
	s_playersettings.race.generic.y = y;
	s_playersettings.race.field.style = UI_SMALLFONT;
	s_playersettings.race.field.titleEnum = MBT_RACE;
	s_playersettings.race.field.titlecolor = CT_LTGOLD1;
	s_playersettings.race.field.textcolor = CT_DKGOLD1;
	s_playersettings.race.field.textcolor2 = CT_LTGOLD1;

	y += 25;
	s_playersettings.height.generic.type = MTYPE_SLIDER;
	s_playersettings.height.generic.x = 115 + 90; //162
	s_playersettings.height.generic.y = y;
	s_playersettings.height.generic.flags = QMF_SMALLFONT;
	s_playersettings.height.generic.id = ID_HEIGHT;
	s_playersettings.height.generic.callback = PlayerSettings_MenuEvent;
	s_playersettings.height.defvalue = 1.0;
	s_playersettings.height.minvalue = 0.9;
	s_playersettings.height.maxvalue = 1.15;
	s_playersettings.height.color = CT_DKPURPLE1;
	s_playersettings.height.color2 = CT_LTPURPLE1;
	s_playersettings.height.generic.name = PIC_MONBAR2;
	s_playersettings.height.width = 256;
	s_playersettings.height.height = 32;
	s_playersettings.height.focusWidth = 145; //145
	s_playersettings.height.focusHeight = 18;
	s_playersettings.height.picName = (char*)GRAPHIC_SQUARE;
	s_playersettings.height.picX = 115;
	s_playersettings.height.picY = y;
	s_playersettings.height.picWidth = 85; //MENU_BUTTON_MED_WIDTH + 21;
	s_playersettings.height.picHeight = MENU_BUTTON_MED_HEIGHT;
	s_playersettings.height.textX = MENU_BUTTON_TEXT_X;
	s_playersettings.height.textY = MENU_BUTTON_TEXT_Y;
	s_playersettings.height.textEnum = MBT_HEIGHT;
	s_playersettings.height.textcolor = CT_BLACK;
	s_playersettings.height.textcolor2 = CT_WHITE;
	s_playersettings.height.thumbName = (char*)PIC_SLIDER;
	s_playersettings.height.thumbHeight = 32;
	s_playersettings.height.thumbWidth = 16;
	s_playersettings.height.thumbGraphicWidth = 9;
	s_playersettings.height.thumbColor = CT_DKBLUE1;
	s_playersettings.height.thumbColor2 = CT_LTBLUE1;

	y += 25;
	s_playersettings.weight.generic.type = MTYPE_SLIDER;
	s_playersettings.weight.generic.x = 115 + 90; //162
	s_playersettings.weight.generic.y = y;
	s_playersettings.weight.generic.flags = QMF_SMALLFONT;
	s_playersettings.weight.generic.callback = PlayerSettings_MenuEvent;
	s_playersettings.weight.generic.id = ID_WEIGHT;
	s_playersettings.weight.defvalue = 1.0;
	s_playersettings.weight.minvalue = 0.9;
	s_playersettings.weight.maxvalue = 1.1;
	s_playersettings.weight.color = CT_DKPURPLE1;
	s_playersettings.weight.color2 = CT_LTPURPLE1;
	s_playersettings.weight.generic.name = PIC_MONBAR2;
	s_playersettings.weight.width = 256;
	s_playersettings.weight.height = 32;
	s_playersettings.weight.focusWidth = 145;
	s_playersettings.weight.focusHeight = 18;
	s_playersettings.weight.picName = (char*)GRAPHIC_SQUARE;
	s_playersettings.weight.picX = 115;
	s_playersettings.weight.picY = y;
	s_playersettings.weight.picWidth = 85; //MENU_BUTTON_MED_WIDTH + 21;
	s_playersettings.weight.picHeight = MENU_BUTTON_MED_HEIGHT;
	s_playersettings.weight.textX = MENU_BUTTON_TEXT_X;
	s_playersettings.weight.textY = MENU_BUTTON_TEXT_Y;
	s_playersettings.weight.textEnum = MBT_WEIGHT;
	s_playersettings.weight.textcolor = CT_BLACK;
	s_playersettings.weight.textcolor2 = CT_WHITE;
	s_playersettings.weight.thumbName = (char*)PIC_SLIDER;
	s_playersettings.weight.thumbHeight = 32;
	s_playersettings.weight.thumbWidth = 16;
	s_playersettings.weight.thumbGraphicWidth = 9;
	s_playersettings.weight.thumbColor = CT_DKBLUE1;
	s_playersettings.weight.thumbColor2 = CT_LTBLUE1;

	if (s_playersettings.numRankSets > 1) {
		s_playersettings.rankSets.generic.type = MTYPE_SPINCONTROL;
		s_playersettings.rankSets.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
		s_playersettings.rankSets.generic.id = ID_RANKSETS;
		s_playersettings.rankSets.generic.callback = PlayerSettings_MenuEvent;
		s_playersettings.rankSets.generic.x = 257;//134;
		s_playersettings.rankSets.generic.y = 391;//233;
		s_playersettings.rankSets.textEnum = MBT_RANKSETS;
		s_playersettings.rankSets.textcolor = CT_BLACK;
		s_playersettings.rankSets.textcolor2 = CT_WHITE;
		s_playersettings.rankSets.color = CT_DKPURPLE1;
		s_playersettings.rankSets.color2 = CT_LTPURPLE1;
		s_playersettings.rankSets.width = 80;//80;
		s_playersettings.rankSets.textX = 5;
		s_playersettings.rankSets.textY = 2;
		s_playersettings.rankSets.itemnames = (const char**)s_playersettings.rankSetList; //rankSets_items_formal3;
	}

	if (s_playersettings.numClassSets > 1)
	{
		s_playersettings.classSets.generic.type = MTYPE_SPINCONTROL;
		s_playersettings.classSets.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
		s_playersettings.classSets.generic.id = ID_CLASSSETS;
		s_playersettings.classSets.generic.callback = PlayerSettings_MenuEvent;
		s_playersettings.classSets.generic.x = 257;//134;
		s_playersettings.classSets.generic.y = 415;//233;
		s_playersettings.classSets.textEnum = MBT_CLASSSETS;
		s_playersettings.classSets.textcolor = CT_BLACK;
		s_playersettings.classSets.textcolor2 = CT_WHITE;
		s_playersettings.classSets.color = CT_DKPURPLE1;
		s_playersettings.classSets.color2 = CT_LTPURPLE1;
		s_playersettings.classSets.width = 80;//80;
		s_playersettings.classSets.textX = 5;
		s_playersettings.classSets.textY = 2;
		s_playersettings.classSets.itemnames = (const char**)s_playersettings.classSetList; //rankSets_items_formal3;
	}

	s_playersettings.commit.generic.type = MTYPE_BITMAP;
	s_playersettings.commit.generic.flags = (QMF_GRAYED | QMF_INACTIVE);
	s_playersettings.commit.generic.x = 196;
	s_playersettings.commit.generic.y = 314;
	s_playersettings.commit.generic.name = GRAPHIC_SQUARE;
	s_playersettings.commit.generic.callback = PlayerSettings_MenuEvent;
	s_playersettings.commit.generic.id = ID_COMMIT;
	s_playersettings.commit.width = 133;
	s_playersettings.commit.height = 18;
	s_playersettings.commit.color = CT_VDKRED1;
	s_playersettings.commit.color2 = CT_DKRED1;
	s_playersettings.commit.textX = MENU_BUTTON_TEXT_X;
	s_playersettings.commit.textY = MENU_BUTTON_TEXT_Y;
	s_playersettings.commit.textcolor = CT_BLACK;
	s_playersettings.commit.textcolor2 = CT_WHITE;
	s_playersettings.commit.textEnum = MBT_COMMIT;

	s_playersettings.data.generic.type = MTYPE_BITMAP;
	s_playersettings.data.generic.name = BUTTON_GRAPHIC_LONGRIGHT;
	s_playersettings.data.generic.flags = QMF_GRAYED;
	s_playersettings.data.generic.id = ID_DATA;
	s_playersettings.data.generic.callback = PlayerSettings_MenuEvent;
	s_playersettings.data.generic.x = 482;
	s_playersettings.data.generic.y = 391;
	s_playersettings.data.width = 128;
	s_playersettings.data.height = 64;
	s_playersettings.data.width = MENU_BUTTON_MED_WIDTH;
	s_playersettings.data.height = MENU_BUTTON_MED_HEIGHT;
	s_playersettings.data.color = CT_DKPURPLE1;
	s_playersettings.data.color2 = CT_LTPURPLE1;
	s_playersettings.data.textX = 5;
	s_playersettings.data.textY = 2;
	s_playersettings.data.textEnum = MBT_PLAYERDATA;
	s_playersettings.data.textcolor = CT_BLACK;
	s_playersettings.data.textcolor2 = CT_WHITE;

	s_playersettings.model.generic.type = MTYPE_BITMAP;
	s_playersettings.model.generic.name = BUTTON_GRAPHIC_LONGRIGHT;
	s_playersettings.model.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_playersettings.model.generic.id = ID_MODEL;
	s_playersettings.model.generic.callback = PlayerSettings_MenuEvent;
	s_playersettings.model.generic.x = 482;
	s_playersettings.model.generic.y = 415;
	s_playersettings.model.width = MENU_BUTTON_MED_WIDTH;
	s_playersettings.model.height = MENU_BUTTON_MED_HEIGHT;
	s_playersettings.model.color = CT_DKPURPLE1;
	s_playersettings.model.color2 = CT_LTPURPLE1;
	s_playersettings.model.textX = 5;
	s_playersettings.model.textY = 2;
	s_playersettings.model.textEnum = MBT_CHANGEMODEL;
	s_playersettings.model.textcolor = CT_BLACK;
	s_playersettings.model.textcolor2 = CT_WHITE;

	s_playersettings.player.generic.type = MTYPE_BITMAP;
	s_playersettings.player.generic.flags = QMF_SILENT;
	s_playersettings.player.generic.ownerdraw = PlayerSettings_DrawPlayer;
	s_playersettings.player.generic.callback = PlayerSettings_SpinPlayer;
	s_playersettings.player.generic.x = 439; //400
	s_playersettings.player.generic.y = 80; //20
	s_playersettings.player.width = 151; //32*7.3
	s_playersettings.player.height = 291; //56*7.3

	s_playersettings.item_null.generic.type = MTYPE_BITMAP;
	s_playersettings.item_null.generic.flags = QMF_LEFT_JUSTIFY | QMF_MOUSEONLY | QMF_SILENT;
	s_playersettings.item_null.generic.x = 0;
	s_playersettings.item_null.generic.y = 0;
	s_playersettings.item_null.width = 640;
	s_playersettings.item_null.height = 480;

	Menu_AddItem(&s_playersettings.menu, &s_playersettings.mainmenu);
	Menu_AddItem(&s_playersettings.menu, &s_playersettings.back);
	Menu_AddItem(&s_playersettings.menu, &s_playersettings.name);
	Menu_AddItem(&s_playersettings.menu, &s_playersettings.classTxt);
	Menu_AddItem(&s_playersettings.menu, &s_playersettings.pClass);
	Menu_AddItem(&s_playersettings.menu, &s_playersettings.rankTxt);
	Menu_AddItem(&s_playersettings.menu, &s_playersettings.pRank);
	Menu_AddItem(&s_playersettings.menu, &s_playersettings.age);
	Menu_AddItem(&s_playersettings.menu, &s_playersettings.race);
	Menu_AddItem(&s_playersettings.menu, &s_playersettings.height);
	Menu_AddItem(&s_playersettings.menu, &s_playersettings.weight);
	Menu_AddItem(&s_playersettings.menu, &s_playersettings.commit);

	Menu_AddItem(&s_playersettings.menu, &s_playersettings.model);
	Menu_AddItem(&s_playersettings.menu, &s_playersettings.data);

	if (s_playersettings.numRankSets > 1 && !ingameFlag) {
		Menu_AddItem(&s_playersettings.menu, &s_playersettings.rankSets);
	}

	if (s_playersettings.numClassSets > 1 && !ingameFlag) {
		Menu_AddItem(&s_playersettings.menu, &s_playersettings.classSets);
	}

	Menu_AddItem(&s_playersettings.menu, &s_playersettings.player);

	//PlayerSettings_AffectModel();

	PlayerSettings_SetMenuItems();

	if (s_playersettings.numRankSets > 1 && !ingameFlag) {
		s_playersettings.rankSets.generic.flags &= ~QMF_HIDDEN;

		for (i = 0; i < s_playersettings.rankSets.numitems; i++) {
			if (!Q_stricmp(UI_Cvar_VariableString("ui_currentRankSet"), s_playersettings.rankSetList[i])) {
				s_playersettings.rankSets.curvalue = i;
				break;
			}
		}

		if (s_playersettings.rankSetList[s_playersettings.rankSets.curvalue][0])
			trap_Cvar_Set("ui_currentRankSet", s_playersettings.rankSetList[s_playersettings.rankSets.curvalue]);

		UI_InitRanksData(UI_Cvar_VariableString("ui_currentRankSet"));
		s_playersettings.numRanks = UI_PopulateRanksArray(s_playersettings.rankList);

		s_playersettings.pRank.numitems = s_playersettings.numRanks;
	}

	if (s_playersettings.numClassSets > 1 && !ingameFlag) {
		s_playersettings.classSets.generic.flags &= ~QMF_HIDDEN;

		for (i = 0; i < s_playersettings.classSets.numitems; i++) {
			if (!Q_stricmp(UI_Cvar_VariableString("ui_currentClassSet"), s_playersettings.classSetList[i])) {
				s_playersettings.classSets.curvalue = i;
				break;
			}
		}

		if (s_playersettings.classSetList[s_playersettings.rankSets.curvalue][0])
			trap_Cvar_Set("ui_currentClassSet", s_playersettings.classSetList[s_playersettings.classSets.curvalue]);

		UI_InitRanksData(UI_Cvar_VariableString("ui_currentClassSet"));
		s_playersettings.numClasses = UI_PopulateClassArray(s_playersettings.classNameList);

		s_playersettings.pClass.numitems = s_playersettings.numClasses;
	}
	UI_LogFuncEnd();
}


/*
=================
PlayerSettings_Cache
=================
*/
void PlayerSettings_Cache(void)
{
	UI_LogFuncBegin();
	s_playersettings.corner_ul_4_4 = trap_R_RegisterShaderNoMip("menu/common/corner_ul_4_4");
	s_playersettings.corner_ur_4_4 = trap_R_RegisterShaderNoMip("menu/common/corner_ur_4_4");
	s_playersettings.corner_ll_4_4 = trap_R_RegisterShaderNoMip("menu/common/corner_ll_4_4");
	s_playersettings.corner_ll_4_18 = trap_R_RegisterShaderNoMip("menu/common/corner_ll_4_18");
	s_playersettings.corner_lr_4_18 = trap_R_RegisterShaderNoMip("menu/common/corner_lr_4_18");

	trap_R_RegisterShaderNoMip(PIC_MONBAR2);
	trap_R_RegisterShaderNoMip(PIC_SLIDER);
	UI_LogFuncEnd();
}


/*
=================
UI_PlayerSettingsMenu
=================
*/
void UI_PlayerSettingsMenu(int32_t menuFrom)
{
	UI_LogFuncBegin();
	PlayerSettings_MenuInit(menuFrom);
	UI_PushMenu(&s_playersettings.menu);
	UI_LogFuncEnd();
}

