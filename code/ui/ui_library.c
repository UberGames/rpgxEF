// Copyright (C) 1999-2000 Id Software, Inc.
// RPG-X: UI Library Menu
// Developer: Some Generic Raven d00d....
// RPG-X Porter: TiM
// Function: To load in, cache, initialize and draw a really cool menu that contains all sorts of cool
//(but ultimately useless ;) ) information. ;D
//

#include "ui_local.h"

#define ID_MAINTOPICBUTTON1		11
#define ID_MAINTOPICBUTTON2		12
#define ID_MAINTOPICBUTTON3		13
#define ID_MAINTOPICBUTTON4		14

#define ID_SUBTOPICBUTTON1		15
#define ID_SUBTOPICBUTTON2		16
#define ID_SUBTOPICBUTTON3		17
#define ID_SUBTOPICBUTTON4		18
#define ID_SUBTOPICBUTTON5		19
#define ID_SUBTOPICBUTTON6		20
#define ID_SUBTOPICBUTTON7		21
#define ID_SUBTOPICBUTTON8		22
#define ID_SUBTOPICBUTTON9		23
#define ID_SUBTOPICBUTTON10		24

#define MAXMAINTOPICDISPLAYED		4
#define MAXSUBTOPICDISPLAYED	8

#define MAXMAINTOPIC			25
#define MAXSUBTOPIC				25

#define LOGWAITTIME				25

#define ID_ARROW1UP			2
#define ID_ARROW1DOWN		3
#define ID_ARROW2UP			4
#define ID_ARROW2DOWN		5

#define ID_QUIT		10

void UI_LanguageFilename(char *baseName,char *baseExtension,char *finalName);

static struct 
{
	menuframework_s menu;
	int				mainTopic;
	int				subTopic;
	int				maxMainTopics;
	int				topMainTopic;				// Index to Maintopic at top of viewing screen
	int				cntMainTopic;				// Count of topics loaded in
	int				topSubTopic;				// Index to Subtopic at top of viewing screen
	int				cntSubTopic[MAXSUBTOPIC];	// Count of subtopics loaded in
//	int				menuType;					// 0 if library, 1 if astrometrics
	int				chosenButton;				// Hi-lite subtopic button
	qhandle_t		corner_ur_20_24;
	qhandle_t		corner_lr_18_20;
	qhandle_t		corner_ll_8_47;
	qhandle_t		corner_ll_18_47;
	qhandle_t		corner_lr_8_20;
	qhandle_t		leftRound;
	menubitmap_s	playerModel;
	menubitmap_s	quitMenu;
	menubitmap_s	topicArrowDown;
	menubitmap_s	topicArrowUp;
	menubitmap_s	subTopicArrowDown;
	menubitmap_s	subTopicArrowUp;
	menubitmap_s	computerVoice;
	menubitmap_s	mainTopicButton1;
	menubitmap_s	mainTopicButton2;
	menubitmap_s	mainTopicButton3;
	menubitmap_s	mainTopicButton4;
	menubitmap_s	subTopicButton1;
	menubitmap_s	subTopicButton2;
	menubitmap_s	subTopicButton3;
	menubitmap_s	subTopicButton4;
	menubitmap_s	subTopicButton5;
	menubitmap_s	subTopicButton6;
	menubitmap_s	subTopicButton7;
	menubitmap_s	subTopicButton8;
	menubitmap_s	subTopicButton9;
	menubitmap_s	subTopicButton10;
} s_library;

static struct 
{
	menuframework_s menu;
	float			timer;
	qhandle_t		cornerPic;
//	int				menuType;		// 0 = library, 1 = astrometrics
} s_libraryaccessing;

// Log Menu Graphics
typedef enum 
{
	LMG_CURRENT_DESC,
	LMG_BIO_DESC1,
	LMG_BIO_DESC2,
	LMG_BIO_DESC3,
	LMG_BIO_DESC4,
	LMG_BIO_DESC5,
	LMG_BIO_DESC6,
	LMG_BIO_DESC7,
	LMG_BIO_DESC8,
	LMG_BIO_DESC9,
	LMG_BIO_DESC10,
	LMG_BIO_DESC11,
	LMG_BIO_DESC12,
	LMG_BIO_DESC13,
	LMG_BIO_DESC14,
	LMG_BIO_DESC15,
	LMG_BIO_DESC16,
	LMG_BIO_DESC17,
	LMG_BIO_DESC18,
	LMG_BIO_DESC19,
	LMG_BIO_DESC20,
	LMG_BIO_DESC21,
	LMG_BIO_DESC22,
	LMG_BIO_DESC23,
	LMG_BIO_DESC24,
	LMG_BIO_DESC25,
	LMG_MAX
} logmenu_graphics_t;

#define MAXLIBRARYTEXT 50000
static char	LibraryText[MAXLIBRARYTEXT];

static struct 
{
	menuframework_s menu;
	qhandle_t		leftRound;
	qhandle_t		corner_ur_20_24;
	qhandle_t		corner_lr_18_20;
	qhandle_t		corner_ll_8_47;
	qhandle_t		corner_ll_18_47;

	int				lineCnt;	// # of lines in description
	int				currentLog;	// Index to current log being read in
	int				maxButtons;	// Count of stardate buttons
	int				currentButton;	// Currently selected stardate button
	int				currentText;
	int				screenType;		// 0=Normal Logs, 1=PADDs
	menubitmap_s	quitMenu;
	menubitmap_s	buttonArrowUp;
	menubitmap_s	buttonArrowDown;
	menubitmap_s	textArrowDown;
	menubitmap_s	textArrowUp;
	int				topButton;		// Which line is at the top of the displayed buttons
	int				topText;		// Which line is at the top of the text
	int				chosenButton;				// Hi-lite subtopic button
	menubitmap_s	logButton1;
	menubitmap_s	logButton2;
	menubitmap_s	logButton3;
	menubitmap_s	logButton4;
	menubitmap_s	logButton5;
	menubitmap_s	logButton6;
	menubitmap_s	logButton7;
	menubitmap_s	logButton8;
	menubitmap_s	logButton9;
	menubitmap_s	logButton10;
	menubitmap_s	logButton11;
	menubitmap_s	logButton12;
} s_log;

typedef struct 
{
	char	*mainTopic;						// Description of main topic
	char	*mainTopicDesc;					// Description of main topic desc
	char	*subTopic[MAXSUBTOPIC];			// Description of subtopics
	char	*subTopicDesc[MAXSUBTOPIC];		// Description of subtopic desc
	int		textY[MAXSUBTOPIC];				// Y starting point of text
	int		textWidth[MAXSUBTOPIC];			// X width of text 
	char	*text[MAXSUBTOPIC];				// Text
	char	*model[MAXSUBTOPIC];			// Model
	int		modelX[MAXSUBTOPIC];			// Model x location
	int		modelY[MAXSUBTOPIC];			// Model y location
	int		modelDistance[MAXSUBTOPIC];		// Model's distance from camera
	int		modelOriginY[MAXSUBTOPIC];		// Model change in Y origin
	int		modelYaw[MAXSUBTOPIC];			// Model's YAW from camera (0 to rotate)
	int		modelPitch[MAXSUBTOPIC];		// Model's PITCH 
	int		modelRoll[MAXSUBTOPIC];			// Model's ROLL 
	char	*modelLegs[MAXSUBTOPIC];
	char	*modelHead[MAXSUBTOPIC];
	char	*modelTorso[MAXSUBTOPIC];
	qhandle_t	modelHandle[MAXSUBTOPIC];	// Handle to model
	char	*shader[MAXSUBTOPIC];			// Shader
	qhandle_t	shaderHandle[MAXSUBTOPIC];	// Handle to shader
	int		shaderXpos[MAXSUBTOPIC];		// Shader x location
	int		shaderYpos[MAXSUBTOPIC];		// Shader y location
	int		shaderXlength[MAXSUBTOPIC];		// Shader x length
	int		shaderYlength[MAXSUBTOPIC];		// Shader y length
	char	*command[MAXSUBTOPIC];			// Command given when exiting the Astrometrics menu
	char	*sound[MAXSUBTOPIC];			// Wav file to play along with the text
	sfxHandle_t		soundHandle[MAXSUBTOPIC];	// Handle of wav file to play
} libraryText_t;


sfxHandle_t		nullSound;	

libraryText_t libraryText[MAXMAINTOPIC];

#define LOG_MAXDESC 25
char logDesc[LOG_MAXDESC][512];

menugraphics_s logmenu_graphics[LMG_MAX] = 
{
	MG_VAR,		0.0,	0,		0,		0,		0,		NULL,		0,	0,		0,		0,		0,					0,		0,							CT_NONE,		NULL,	// LMG_CURRENT_DESC

//	type		timer	x		y		width	height	file/text			graphic,	min		max		target					inc		style						color		pointer
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[0],0,		0,		0,		0,		LMG_BIO_DESC2,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC1
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[1],0,		0,		0,		0,		LMG_BIO_DESC3,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC2
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[2],0,		0,		0,		0,		LMG_BIO_DESC4,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC3
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[3],0,		0,		0,		0,		LMG_BIO_DESC5,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC4
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[4],0,		0,		0,		0,		LMG_BIO_DESC6,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC5
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[5],0,		0,		0,		0,		LMG_BIO_DESC7,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC6
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[6],0,		0,		0,		0,		LMG_BIO_DESC8,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC7
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[7],0,		0,		0,		0,		LMG_BIO_DESC9,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC8
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[8],0,		0,		0,		0,		LMG_BIO_DESC10,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC9
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[9],0,		0,		0,		0,		LMG_BIO_DESC11,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC10
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[10],0,		0,		0,		0,		LMG_BIO_DESC12,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC11
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[11],0,		0,		0,		0,		LMG_BIO_DESC13,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC12
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[12],0,		0,		0,		0,		LMG_BIO_DESC14,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC13
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[13],0,		0,		0,		0,		LMG_BIO_DESC15,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC14
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[14],0,		0,		0,		0,		LMG_BIO_DESC16,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC15
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[15],0,		0,		0,		0,		LMG_BIO_DESC17,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC16
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[16],0,		0,		0,		0,		LMG_BIO_DESC18,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC17
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[17],0,		0,		0,		0,		LMG_BIO_DESC19,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC18
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[18],0,		0,		0,		0,		LMG_BIO_DESC20,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC19
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[19],0,		0,		0,		0,		LMG_BIO_DESC21,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC20
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[20],0,		0,		0,		0,		LMG_BIO_DESC22,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC21
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[21],0,		0,		0,		0,		LMG_BIO_DESC23,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC22
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[22],0,		0,		0,		0,		LMG_BIO_DESC24,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC23
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[23],0,		0,		0,		0,		LMG_BIO_DESC25,		0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC24
	MG_STRING,	0.0,	242,	  0,	0,		0,		logDesc[24],0,		0,		0,		0,		0,	/*Null*/			0,		UI_TINYFONT,				CT_VLTGOLD1,	NULL,	// LMG_BIO_DESC25
};

/*
UI_LanguageFilename - create a filename with an extension based on the value in g_language
*/
void UI_LanguageFilename(char *baseName,char *baseExtension,char *finalName)
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

/*
=================
CrewMenu_Blinkies
=================
*/
void LogMenu_Blinkies (void)
{
	int descI;

	// Turning on description a line at a time
	if ((logmenu_graphics[LMG_CURRENT_DESC].timer < uis.realtime) && (logmenu_graphics[LMG_CURRENT_DESC].type == MG_VAR))
	{
		descI = logmenu_graphics[LMG_CURRENT_DESC].target;
		if (!descI)
		{
			logmenu_graphics[LMG_CURRENT_DESC].type = MG_OFF;
		}
		else
		{
			if (s_log.lineCnt >= (descI - LMG_BIO_DESC1))
			{
				logmenu_graphics[descI].type = MG_STRING;	// Turnon string
				trap_S_StartLocalSound( uis.menu_datadisp2_snd, CHAN_MENU1 );
				logmenu_graphics[LMG_CURRENT_DESC].target = logmenu_graphics[descI].target;	// Set up next line
				logmenu_graphics[LMG_CURRENT_DESC].timer = uis.realtime + LOGWAITTIME;
			}
		}
	}	
}

/*
=================
SplitLogDesc
=================
*/
static void SplitLogDesc(char *s,int width)
{
	int	lineWidth,currentWidth,charCnt,currentLineI;
	char *holds;
	char holdChar[2];
	int	nextLine;

	// Clean out any old data
	memset(logDesc,0,sizeof(logDesc));

	// Break into individual lines
	holds = s;

	lineWidth = width;	// How long (in pixels) a line can be
	currentWidth = 0;
	holdChar[1] = '\0';
	charCnt= 0;
	currentLineI = 0;
	s_log.lineCnt = 0;

	while( *s ) 
	{
		++charCnt;
		holdChar[0] = *s;

		// Advance to next line.
		if ((*s == '/') && (*(s + 1)  == 'n'))
		{
			s++;	// Advance to next character
			currentWidth = 0;
			nextLine = qtrue;
		}
		else
		{
			nextLine = qfalse;
			currentWidth += UI_ProportionalStringWidth(holdChar,UI_TINYFONT);
			currentWidth +=1; // The space between characters
		}

		if ((currentWidth >= lineWidth ) || (nextLine))
		{//Reached max length of this line
			//step back until we find a space

			while((currentWidth) && (*s != ' '))
			{
				holdChar[0] = *s;
				currentWidth -= UI_ProportionalStringWidth(holdChar,UI_TINYFONT);
				--s;
				--charCnt;
			}

			Q_strncpyz( logDesc[currentLineI], holds, charCnt);
			logDesc[currentLineI][charCnt] = 0; //NULL

			++currentLineI;
			currentWidth = 0;
			charCnt = 0;

			holds = s;
			++holds;

			s_log.lineCnt++;

			if (currentLineI > LOG_MAXDESC)
			{
				break;
			}

		}	
		++s;
	}

	++charCnt;  // So the NULL will be properly placed at the end of the string of Q_strncpyz
	Q_strncpyz( logDesc[currentLineI], holds, charCnt);
	logDesc[currentLineI][charCnt] = 0; //NULL

}

/*
=================
TurnOnLogDesc
=================
*/
static void TurnOnLogDesc(char *s,int lineWidth,int startY)
{
	int	y,i;

	logmenu_graphics[LMG_BIO_DESC1].type = MG_STRING;
	logmenu_graphics[LMG_CURRENT_DESC].target = logmenu_graphics[LMG_BIO_DESC1].target;	// Set up next line

	logmenu_graphics[LMG_CURRENT_DESC].type = MG_VAR;
	logmenu_graphics[LMG_CURRENT_DESC].timer = uis.realtime + LOGWAITTIME;

	// Split up big description line
	SplitLogDesc(s,lineWidth);

	y = startY;

	for (i=LMG_BIO_DESC1; i<(LMG_BIO_DESC1 +LOG_MAXDESC );i++)
	{
		logmenu_graphics[i].x = 242;
		logmenu_graphics[i].y = y;
		y += 12;
	}


}

static void UI_Draw3DModel( float x, float y, float w, float h, qhandle_t model, vec3_t origin, vec3_t angles) 
{
	refdef_t		refdef;
	refEntity_t		ent;

	UI_AdjustFrom640( &x, &y, &w, &h );

	memset( &refdef, 0, sizeof( refdef ) );

	memset( &ent, 0, sizeof( ent ) );
	AnglesToAxis( angles, ent.axis );
	
	VectorCopy( origin, ent.origin );
	ent.hModel = model;
//	if (Cvar_VariableValue("r_dynamiclight") ) {
//		ent.renderfx = RF_LOWLIGHT|RF_NOSHADOW;		// keep it dark, and no stencil shadows
//	} else {
//		ent.renderfx = RF_NOSHADOW;		// no stencil shadows
//	}
	refdef.rdflags = RDF_NOWORLDMODEL;

	AxisClear( refdef.viewaxis );

	refdef.fov_x = 40;
	refdef.fov_y = 30;

	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;

	refdef.time = uis.realtime;

	trap_R_ClearScene();
	trap_R_AddRefEntityToScene( &ent );

	origin[0] -= 50;	// + = behind, - = in front
	origin[1] += 50;	// + = left, - = right
	origin[2] += 50;	// + = above, - = below
	trap_R_AddLightToScene( origin, 500, 1.0, 1.0, 1.0 );

	trap_R_RenderScene( &refdef );
}

//void Controls_DrawPlayer( void *self );
//void Controls_UpdateModel( int anim );

/*
================
UI_LibraryDrawMD3Model
================
*/
static void UI_LibraryDrawMD3Model(qhandle_t modelHandle,int x, int y,int modelDistance,int modelYaw,int modelPitch,int modelRoll,int modelOriginY)
{
	vec3_t	origin = {50,0,2};
	vec3_t	angles;

	if (modelDistance)
	{
		origin[0] = modelDistance;
	}
	else
	{
		origin[0] = 50;
	}

	if (modelOriginY)
	{
		origin[2] = modelOriginY;
	}
	else
	{
		origin[2] = 2;
	}

	angles[PITCH] = modelPitch;

	if (modelYaw)
	{
		angles[YAW]   = modelYaw;
	}
	else
	{
		angles[YAW]   = 20.0/300.0 * uis.realtime;
	}


	if (modelRoll)
	{
		angles[ROLL]  = modelRoll;
	}
	else
	{
		angles[ROLL]  = 0;
	}

	UI_Draw3DModel( x, y, 447, 305, modelHandle, origin, angles);

}

/*
================
UI_DrawLibraryMDRModel

================
*/
static void UI_DrawLibraryMDRModel(qhandle_t modelHandle,int x, int y)
{
	void *voidPtr;

//	vec3_t	origin = {50,0,2};
	vec3_t	angles;

	angles[PITCH] = 0;
	angles[YAW]   = 20.0/300.0 * uis.realtime;
	angles[ROLL]  = 0;

	voidPtr = (void *) &s_library.playerModel;
//	Controls_DrawPlayer(voidPtr);
//	Controls_UpdateModel( 0 );

}

/*
=================
ClearLibraryDesc
=================
*/
void ClearLibraryDesc(void)
{
	int i;

	for (i=0;i<(LMG_MAX - LMG_BIO_DESC1);++i)
	{
		logmenu_graphics[LMG_BIO_DESC1 + i].type = MG_OFF;	// Turn off text
	}
}

/*
=================
ChangeLibraryDesc
=================
*/
void ChangeLibraryDesc(int id)
{
	int i,y,width;

	if (s_library.subTopic == id)
	{
		return;	//	Just hitting the same key again
	}
	else	// Turn off old description 
	{

		s_library.subTopic = id;

		for (i=0;i<(LMG_MAX - LMG_BIO_DESC1);++i)
		{
			logmenu_graphics[LMG_BIO_DESC1 + i].type = MG_OFF;	// Turn off text
		}
	}

	if (!libraryText[s_library.mainTopic].textY[s_library.subTopic])
	{
		y = 120;
	}
	else
	{
		y = libraryText[s_library.mainTopic].textY[s_library.subTopic];
	}

	for (i=LMG_BIO_DESC1; i<(LMG_BIO_DESC1 +LOG_MAXDESC );i++)
	{
		logmenu_graphics[i].x = 242;
		logmenu_graphics[i].y = y;
		y += 12;
	}

	if (!libraryText[s_library.mainTopic].textWidth[s_library.subTopic])
	{
		width = 360;
	}
	else
	{
		width = libraryText[s_library.mainTopic].textWidth[s_library.subTopic];
	}

	// Turn on description for new ID
	TurnOnLogDesc(libraryText[s_library.mainTopic].text[s_library.subTopic],
		width,libraryText[s_library.mainTopic].textY[s_library.subTopic]);

	//if ((libraryText[s_library.mainTopic].soundHandle[s_library.subTopic]) && 
	//	(s_library.computerVoice.textEnum == MBT_COMPUTERVOICEON))
	//{
	//	trap_S_StartLocalSound( libraryText[s_library.mainTopic].soundHandle[s_library.subTopic], CHAN_MENU2 );
//	}

}

/*
=================
M_Transporter_Event
=================
*/
void M_Library_Event (void* ptr, int notification)
{
	int	id,i;
	menubitmap_s	*holdSubTopicButton,*holdMainTopicButton;

	if (notification != QM_ACTIVATED)
	{
		return;
	}

	id = ((menucommon_s*)ptr)->id;

	switch (id)
	{
		/*case ID_COMPUTERVOICE:

			if (s_library.computerVoice.textEnum == MBT_COMPUTERVOICEON)
			{
				s_library.computerVoice.textEnum = MBT_COMPUTERVOICEOFF;
				trap_S_StartLocalSound( nullSound, CHAN_MENU2 );
			}
			else
			{
				s_library.computerVoice.textEnum = MBT_COMPUTERVOICEON;
			}
			break;
		*/
		case ID_ARROW1UP:
			if ((s_library.topMainTopic - 1)  >= 0)
			{
				s_library.topMainTopic--;

				holdMainTopicButton = &s_library.mainTopicButton1;

				for (i=s_library.topMainTopic;i<(s_library.topMainTopic + MAXMAINTOPICDISPLAYED);i++)
				{
					holdMainTopicButton->textPtr	= libraryText[i].mainTopic;
					holdMainTopicButton++;
				}
			}

			// Show down arrow indicator
			s_library.topicArrowDown.generic.flags &= ~QMF_HIDDEN;

			// Show up arrow indicator???
			if (s_library.topMainTopic == 0)
			{
				s_library.topicArrowUp.generic.flags |= QMF_HIDDEN;
			}
			else
			{
				s_library.topicArrowUp.generic.flags &= ~QMF_HIDDEN;
			}

			break;

		case ID_ARROW1DOWN:
			if (((s_library.topMainTopic + 1) + MAXMAINTOPICDISPLAYED) <= s_library.cntMainTopic)
			{

				s_library.topMainTopic++;

				holdMainTopicButton = &s_library.mainTopicButton1;

				for (i=s_library.topMainTopic;i<(s_library.topMainTopic + MAXMAINTOPICDISPLAYED);i++)
				{
					holdMainTopicButton->textPtr	= libraryText[i].mainTopic;
					holdMainTopicButton++;
				}
			}

			// Show up arrow indicator
			s_library.topicArrowUp.generic.flags &= ~QMF_HIDDEN;

			// Show down arrow indicator???
			if ((s_library.topMainTopic + MAXMAINTOPICDISPLAYED) >= s_library.cntMainTopic)
			{
				s_library.topicArrowDown.generic.flags |= QMF_HIDDEN;
			}
			else
			{
				s_library.topicArrowDown.generic.flags &= ~QMF_HIDDEN;
			}

			break;

		case ID_ARROW2UP:
			if ((s_library.topSubTopic - 1)  >= 0)
			{
				s_library.topSubTopic--;

				holdSubTopicButton = &s_library.subTopicButton1;

				for (i=s_library.topSubTopic;i<(s_library.topSubTopic + MAXSUBTOPICDISPLAYED);i++)
				{
					holdSubTopicButton->textPtr	= libraryText[s_library.mainTopic].subTopic[i];
					holdSubTopicButton++;
				}

				// Hi-lite button when moved
				if ((s_library.chosenButton >= 0) &&
					(s_library.chosenButton <=MAXSUBTOPICDISPLAYED))	// Hi-lite new button
				{
					holdSubTopicButton = &s_library.subTopicButton1 + (s_library.chosenButton);
					holdSubTopicButton->textcolor	= CT_BLACK;
				}

				s_library.chosenButton++;
				if ((s_library.chosenButton >= 0) &&
					(s_library.chosenButton <=MAXSUBTOPICDISPLAYED))	// Hi-lite new button
				{
					holdSubTopicButton = &s_library.subTopicButton1 + (s_library.chosenButton);
					holdSubTopicButton->textcolor	= CT_VLTGOLD1;
				}
			}

			// Show down arrow indicator
			s_library.subTopicArrowDown.generic.flags &= ~QMF_HIDDEN;

			// Show up arrow indicator???
			if (s_library.topSubTopic == 0)
			{
				s_library.subTopicArrowUp.generic.flags |= QMF_HIDDEN;
			}
			else
			{
				s_library.subTopicArrowUp.generic.flags &= ~QMF_HIDDEN;				
			}

			break;

		case ID_ARROW2DOWN:
			if (((s_library.topSubTopic + 1) + MAXSUBTOPICDISPLAYED) <= s_library.cntSubTopic[s_library.mainTopic])
			{
				s_library.topSubTopic++;

				holdSubTopicButton = &s_library.subTopicButton1;

				for (i=s_library.topSubTopic;i<(s_library.topSubTopic + MAXSUBTOPICDISPLAYED);i++)
				{
					holdSubTopicButton->textPtr	= libraryText[s_library.mainTopic].subTopic[i];
					holdSubTopicButton++;
				}

				// Hi-lite button when moved
				if ((s_library.chosenButton >= 0) &&
					(s_library.chosenButton <=MAXSUBTOPICDISPLAYED))	// Hi-lite new button
				{
					holdSubTopicButton = &s_library.subTopicButton1 + (s_library.chosenButton);
					holdSubTopicButton->textcolor	= CT_BLACK;
				}

				s_library.chosenButton--;
				if ((s_library.chosenButton >= 0) &&
					(s_library.chosenButton <=MAXSUBTOPICDISPLAYED))	// Hi-lite new button
				{
					holdSubTopicButton = &s_library.subTopicButton1 + (s_library.chosenButton);
					holdSubTopicButton->textcolor	= CT_VLTGOLD1;
				}
			}

			// Show up arrow indicator
			s_library.subTopicArrowUp.generic.flags &= ~QMF_HIDDEN;

			// Show down arrow indicator???
			if ((s_library.topSubTopic + MAXSUBTOPICDISPLAYED) >= s_library.cntSubTopic[s_library.mainTopic])
			{
				s_library.subTopicArrowDown.generic.flags |= QMF_HIDDEN;
			}
			else
			{
				s_library.subTopicArrowDown.generic.flags &= ~QMF_HIDDEN;
			}

			break;

		case ID_QUIT:
			UI_PopMenu();
			if (libraryText[s_library.mainTopic].command[s_library.subTopic])
			{
				trap_Cmd_ExecuteText( EXEC_APPEND, libraryText[s_library.mainTopic].command[s_library.subTopic]);
			}

			trap_S_StartLocalSound( nullSound, CHAN_MENU1 );

			break;
		case ID_MAINTOPICBUTTON1:
		case ID_MAINTOPICBUTTON2:
		case ID_MAINTOPICBUTTON3:
		case ID_MAINTOPICBUTTON4:

			// Blackout all main topic buttons
			holdMainTopicButton = &s_library.mainTopicButton1;
			for (i=0;i<MAXMAINTOPICDISPLAYED;i++)
			{
				holdMainTopicButton->textcolor	= CT_BLACK;
				++holdMainTopicButton;
			}

			// Highlight chosen button
			holdMainTopicButton = &s_library.mainTopicButton1 + (id - ID_MAINTOPICBUTTON1);
			holdMainTopicButton->textcolor	= CT_LTGOLD1;



			// Blackout all subtopic buttons
			holdSubTopicButton = &s_library.subTopicButton1;
			for (i=0;i<MAXSUBTOPICDISPLAYED;i++)
			{
				holdSubTopicButton->textcolor	= CT_BLACK;
				++holdSubTopicButton;
			}

			// Highlight top subtopic button
			holdSubTopicButton = &s_library.subTopicButton1;
 			holdSubTopicButton->textcolor	= CT_VLTGOLD1;


			s_library.mainTopic = s_library.topMainTopic + (id-ID_MAINTOPICBUTTON1);

			holdSubTopicButton = &s_library.subTopicButton1;
			for (i=0;i<MAXSUBTOPICDISPLAYED;i++)
			{
				holdSubTopicButton->textPtr	= libraryText[s_library.mainTopic].subTopic[i];
				if (libraryText[s_library.mainTopic].subTopicDesc[i])
				{
					holdSubTopicButton->generic.flags	= QMF_HIGHLIGHT_IF_FOCUS;
				}
				else
				{
					holdSubTopicButton->generic.flags	= QMF_HIDDEN;
				}
				holdSubTopicButton++;
			}
			ClearLibraryDesc();

			s_library.subTopic = -1;
			s_library.topSubTopic = 0;
			ChangeLibraryDesc(s_library.topSubTopic);

			s_library.subTopicArrowUp.generic.flags = QMF_HIDDEN;
			s_library.topSubTopic = 0;
			// Show down arrow indicator???
			if ((s_library.topSubTopic + MAXSUBTOPICDISPLAYED) >= s_library.cntSubTopic[s_library.mainTopic])
			{
				s_library.subTopicArrowDown.generic.flags = QMF_HIDDEN;
			}
			else
			{
				s_library.subTopicArrowDown.generic.flags &= ~QMF_HIDDEN;
			}

			break;
		case ID_SUBTOPICBUTTON1:
		case ID_SUBTOPICBUTTON2:
		case ID_SUBTOPICBUTTON3:
		case ID_SUBTOPICBUTTON4:
		case ID_SUBTOPICBUTTON5:
		case ID_SUBTOPICBUTTON6:
		case ID_SUBTOPICBUTTON7:
		case ID_SUBTOPICBUTTON8:
		case ID_SUBTOPICBUTTON9:
		case ID_SUBTOPICBUTTON10:

			// Blackout all subtopic buttons
			holdSubTopicButton = &s_library.subTopicButton1;
			for (i=0;i<MAXSUBTOPICDISPLAYED;i++)
			{
				holdSubTopicButton->textcolor	= CT_BLACK;
				++holdSubTopicButton;
			}

			// Highlight chosen button
//			if (((id + s_library.topSubTopic - ID_SUBTOPICBUTTON1) > 0) &&
//				((id + s_library.topSubTopic - ID_SUBTOPICBUTTON1) < MAXSUBTOPICDISPLAYED))
			holdSubTopicButton = &s_library.subTopicButton1 + (id - ID_SUBTOPICBUTTON1);
			holdSubTopicButton->textcolor	= CT_VLTGOLD1;

			s_library.chosenButton = id - ID_SUBTOPICBUTTON1;

			ChangeLibraryDesc(s_library.topSubTopic + (id-ID_SUBTOPICBUTTON1));

//			if ((libraryText[s_library.mainTopic].soundHandle[s_library.subTopic]) && 
//				(s_library.computerVoice.textEnum == MBT_COMPUTERVOICEON))
//			{
//				trap_S_StartLocalSound( libraryText[s_library.mainTopic].soundHandle[s_library.subTopic], CHAN_MENU2 );
//			}

			// If there's a .mdr
			if (libraryText[s_library.mainTopic].modelLegs[s_library.subTopic])
			{
				trap_Cvar_Set( "legsmodel", libraryText[s_library.mainTopic].modelLegs[s_library.subTopic] );
			}

			if (libraryText[s_library.mainTopic].modelHead[s_library.subTopic])
			{
				trap_Cvar_Set( "headmodel", libraryText[s_library.mainTopic].modelHead[s_library.subTopic] );
			}

			if (libraryText[s_library.mainTopic].modelTorso[s_library.subTopic])
			{
				trap_Cvar_Set( "torsomodel", libraryText[s_library.mainTopic].modelTorso[s_library.subTopic] );
			}
		
			s_library.playerModel.generic.x	= libraryText[s_library.mainTopic].modelX[s_library.subTopic];
			s_library.playerModel.generic.y	= libraryText[s_library.mainTopic].modelY[s_library.subTopic];

			break;
	}
}

/*
=================
Library_StatusBar
=================
*/
/*static void Library_StatusBar(void *itemptr) 
{
	int		id;

	id = ((menucommon_s*)itemptr)->id;

	switch (id)
	{
		case ID_MAINTOPICBUTTON1:
		case ID_MAINTOPICBUTTON2:
		case ID_MAINTOPICBUTTON3:
		case ID_MAINTOPICBUTTON4:
			UI_DrawProportionalString( 320, 410, libraryText[id + s_library.topMainTopic-ID_MAINTOPICBUTTON1].mainTopicDesc, UI_CENTER|UI_SMALLFONT, colorTable[CT_VLTGOLD1]);
			break;
		case ID_SUBTOPICBUTTON1:
		case ID_SUBTOPICBUTTON2:
		case ID_SUBTOPICBUTTON3:
		case ID_SUBTOPICBUTTON4:
		case ID_SUBTOPICBUTTON5:
		case ID_SUBTOPICBUTTON6:
		case ID_SUBTOPICBUTTON7:
		case ID_SUBTOPICBUTTON8:
		case ID_SUBTOPICBUTTON9:
		case ID_SUBTOPICBUTTON10:

			UI_DrawProportionalString( 320, 410, 
			libraryText[s_library.mainTopic].subTopicDesc[id + s_library.topSubTopic - ID_SUBTOPICBUTTON1], 
			UI_CENTER|UI_SMALLFONT, colorTable[CT_VLTGOLD1]);
			break;
	}
}*/

/*
=================
LibraryMenu_Key
=================
*/
sfxHandle_t LibraryMenu_Key (int key)
{
	if ( key == K_ESCAPE ) 
	{
		trap_S_StartLocalSound( nullSound, CHAN_MENU1 );
		if (libraryText[s_library.mainTopic].command[s_library.subTopic])
		{
			trap_Cmd_ExecuteText( EXEC_APPEND, libraryText[s_library.mainTopic].command[s_library.subTopic]);
		}
	}

	return ( Menu_DefaultKey( &s_library.menu, key ) );
}

/*
=================
M_LibraryMenu_Graphics
=================
*/
void M_LibraryMenu_Graphics (void)
{
	int nameX,stardateX; //,textIndex; //length

	trap_R_SetColor( colorTable[CT_VDKPURPLE2]);
	UI_DrawHandlePic( 30,  24,16,   32, s_library.leftRound);

	// Left corners
	UI_DrawHandlePic( 181,  24, 32,   32, s_library.corner_ur_20_24);

	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(  23, 354, -32,   32, s_library.corner_lr_18_20);
	UI_DrawHandlePic(  181, 157,  32,   16, s_library.corner_lr_8_20);
	UI_DrawHandlePic(   21, 153, -32,  -16, s_library.corner_lr_8_20);

	// Right corners
	trap_R_SetColor( colorTable[CT_VDKPURPLE2]);
	UI_DrawHandlePic( 202,  24, -32,  32, s_library.corner_ur_20_24);
	UI_DrawHandlePic( 204, 354, -32,  32, s_library.corner_lr_18_20);

	// Lower corners
	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(  30, 386,  64, -16, s_library.corner_ll_8_47);
	UI_DrawHandlePic(  30, 425, 128,  64, s_library.corner_ll_18_47);

	stardateX  = 50;
//	UI_DrawProportionalString( stardateX , 24, menu_normal_text[MNT_TOPICS],  UI_BIGFONT , colorTable[CT_LTPURPLE2]);	

//	length = UI_ProportionalStringWidth( menu_normal_text[MNT_TOPICS],UI_BIGFONT);
//	trap_R_SetColor( colorTable[CT_VDKBLUE1]);
//	UI_DrawHandlePic( stardateX + 4 + length,  24, (196 - (stardateX + 4 + length)), 24, uis.whiteShader);

	// Left side
	trap_R_SetColor( colorTable[CT_VDKPURPLE2]);
	UI_DrawHandlePic( 184,  36,  20,  23, uis.whiteShader);		// Side line1

	trap_R_SetColor( colorTable[CT_VDKPURPLE3]);
	UI_DrawHandlePic( 184,  62,  20,  18, uis.whiteShader);		// Side line2
	UI_DrawHandlePic( 184,  83,  20,  42, uis.whiteShader);		// Side line3
	UI_DrawHandlePic( 184, 128,  20,  18, uis.whiteShader);		// Side line4

	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic( 184, 149,  20,  13, uis.whiteShader);		// Side line3
	UI_DrawHandlePic(  37, 159, 162,   8, uis.whiteShader);		// Bar across

	UI_DrawHandlePic(  30, 165,  20,  12, uis.whiteShader);		// Lower Left side
	UI_DrawHandlePic(  30, 180,  20,  18, uis.whiteShader);		// Lower Left side2
	UI_DrawHandlePic(  30, 201,  20, 130, uis.whiteShader);		// Lower Left side3
	UI_DrawHandlePic(  30, 334,  20,  18, uis.whiteShader);		// Lower Left side4
	UI_DrawHandlePic(  30, 355,  20,  11, uis.whiteShader);		// Lower Left side5

	UI_DrawHandlePic(  51, 368,   8,  18, uis.whiteShader);		// Bottom
	UI_DrawHandlePic( 195, 368,   7,  18, uis.whiteShader);		// Bottom2

	// Right side
	nameX = 592;

//	textIndex = MNT_LIBRARYSTATION;

//	UI_DrawProportionalString( nameX, 24,
//		menu_normal_text[textIndex],   UI_BIGFONT | UI_RIGHT, colorTable[CT_LTPURPLE2]);	
//	length = UI_ProportionalStringWidth( menu_normal_text[textIndex],UI_BIGFONT);

	trap_R_SetColor( colorTable[CT_VDKPURPLE2]);
//	UI_DrawHandlePic( nameX + 4,  24,-16,   32, s_library.leftRound);
//	UI_DrawHandlePic( 218,  24, (nameX - (length +4)) - 218, 24, uis.whiteShader);
	
	UI_DrawProportionalString( 240, 58,va("%s / %s", libraryText[s_library.mainTopic].mainTopic,libraryText[s_library.mainTopic].subTopic[s_library.subTopic]),
	   UI_SMALLFONT , colorTable[CT_VLTGOLD1]);	

	trap_R_SetColor( colorTable[CT_VDKPURPLE2]);
	UI_DrawHandlePic( 211,  32,  20, 22, uis.whiteShader);		// Side line
	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic( 211,  57,  20, 53, uis.whiteShader);		// Side line2
	UI_DrawHandlePic( 211,  113,  20, 248, uis.whiteShader);	// Side line3

	trap_R_SetColor( colorTable[CT_VDKPURPLE2]);
	UI_DrawHandlePic( 218, 368, 394,  18, uis.whiteShader);		// Bottom line

	// Bottom
	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(  33, 391, 578,   8, uis.whiteShader);		// Top line
	UI_DrawHandlePic(  30, 396,  47,  39, uis.whiteShader);		// Side line
	UI_DrawHandlePic(  54, 438,  39,  18, uis.whiteShader);		// Bottom line 1
	trap_R_SetColor( colorTable[CT_VDKPURPLE2]);
	UI_DrawHandlePic(  96, 438, 268,  18, uis.whiteShader);		// Bottom line 2
	UI_DrawHandlePic( 367, 438, 245,  18, uis.whiteShader);		// Bottom line 3
	
	if (libraryText[s_library.mainTopic].model[s_library.subTopic])
	{
		UI_LibraryDrawMD3Model(libraryText[s_library.mainTopic].modelHandle[s_library.subTopic],
			libraryText[s_library.mainTopic].modelX[s_library.subTopic],
			libraryText[s_library.mainTopic].modelY[s_library.subTopic],
			libraryText[s_library.mainTopic].modelDistance[s_library.subTopic],
			libraryText[s_library.mainTopic].modelYaw[s_library.subTopic],
			libraryText[s_library.mainTopic].modelPitch[s_library.subTopic],
			libraryText[s_library.mainTopic].modelRoll[s_library.subTopic],
			libraryText[s_library.mainTopic].modelOriginY[s_library.subTopic]);
	}

	if ((libraryText[s_library.mainTopic].modelHead[s_library.subTopic]) ||
		(libraryText[s_library.mainTopic].modelTorso[s_library.subTopic]) ||
		(libraryText[s_library.mainTopic].modelLegs[s_library.subTopic]))
	{
		UI_DrawLibraryMDRModel(libraryText[s_library.mainTopic].modelHandle[s_library.subTopic],
			libraryText[s_library.mainTopic].modelX[s_library.subTopic],
			libraryText[s_library.mainTopic].modelY[s_library.subTopic]);
	}

	if (libraryText[s_library.mainTopic].shaderHandle[s_library.subTopic])
	{
		trap_R_SetColor( colorTable[CT_WHITE]);
		UI_DrawHandlePic( libraryText[s_library.mainTopic].shaderXpos[s_library.subTopic],
			libraryText[s_library.mainTopic].shaderYpos[s_library.subTopic],  
			libraryText[s_library.mainTopic].shaderXlength[s_library.subTopic],
			libraryText[s_library.mainTopic].shaderYlength[s_library.subTopic], 
			libraryText[s_library.mainTopic].shaderHandle[s_library.subTopic]);
	}

	LogMenu_Blinkies();

	UI_PrintMenuGraphics(logmenu_graphics,LMG_MAX);

}

/*
===============
LibraryMenu_Draw
===============
*/
void LibraryMenu_Draw(void)
{
	M_LibraryMenu_Graphics();
	
	Menu_Draw( &s_library.menu );
}

/*
===============
LibraryMenu_Init
===============
*/
void LibraryMenu_Init(void)
{
	menubitmap_s	*holdMainTopicButton,*holdSubTopicButton;
	int				x,y,pad,i;

	s_library.menu.nitems						= 0;
	s_library.menu.draw							= LibraryMenu_Draw;
	s_library.menu.key							= LibraryMenu_Key;
	s_library.menu.fullscreen					= qtrue;
	s_library.menu.wrapAround					= qtrue;
	s_library.menu.descX						= MENU_DESC_X;
	s_library.menu.descY						= MENU_DESC_Y;
	s_library.menu.titleX						= MENU_TITLE_X;
	s_library.menu.titleY						= MENU_TITLE_Y;
//	s_library.menu.titleI						= MNT_CREDITSMENU_TITLE;
	s_library.menu.footNoteEnum					= MNT_CREDITS;

	s_library.quitMenu.generic.type				= MTYPE_BITMAP;      
	s_library.quitMenu.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_library.quitMenu.generic.x				= 62;
	s_library.quitMenu.generic.y				= 368;
	s_library.quitMenu.generic.name				= GRAPHIC_SQUARE;
	s_library.quitMenu.generic.id				= ID_QUIT;
	s_library.quitMenu.generic.callback			= M_Library_Event; 
	s_library.quitMenu.width					= MENU_BUTTON_MED_WIDTH;
	s_library.quitMenu.height					= MENU_BUTTON_MED_HEIGHT;
	s_library.quitMenu.color					= CT_DKORANGE;
	s_library.quitMenu.color2					= CT_LTORANGE;
	s_library.quitMenu.textX					= MENU_BUTTON_TEXT_X;
	s_library.quitMenu.textY					= MENU_BUTTON_TEXT_Y;
//	s_library.quitMenu.textEnum					= MBT_PERSONALLOGRETURN;
	s_library.quitMenu.textcolor				= CT_BLACK;
	s_library.quitMenu.textcolor2				= CT_WHITE;

	s_library.topicArrowUp.generic.type					= MTYPE_BITMAP;      
	s_library.topicArrowUp.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS | QMF_HIDDEN;
	s_library.topicArrowUp.generic.x					= 185;
	s_library.topicArrowUp.generic.y					= 64;
	s_library.topicArrowUp.generic.name					= "menu/common/arrow_up_16.tga";
	s_library.topicArrowUp.generic.id					= ID_ARROW1UP;
	s_library.topicArrowUp.generic.callback				= M_Library_Event; 
	s_library.topicArrowUp.width						= 18;
	s_library.topicArrowUp.height						= 18;
	s_library.topicArrowUp.color						= CT_DKORANGE;
	s_library.topicArrowUp.color2						= CT_LTORANGE;

	s_library.topicArrowDown.generic.type				= MTYPE_BITMAP;      
	s_library.topicArrowDown.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_library.topicArrowDown.generic.x					= 185;
	s_library.topicArrowDown.generic.y					= 130;
	s_library.topicArrowDown.generic.name				= "menu/common/arrow_dn_16.tga";
	s_library.topicArrowDown.generic.id					= ID_ARROW1DOWN;
	s_library.topicArrowDown.generic.callback			= M_Library_Event; 
	s_library.topicArrowDown.width						= 18;
	s_library.topicArrowDown.height						= 18;
	s_library.topicArrowDown.color						= CT_DKORANGE;
	s_library.topicArrowDown.color2						= CT_LTORANGE;

	s_library.subTopicArrowUp.generic.type				= MTYPE_BITMAP;      
	s_library.subTopicArrowUp.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS | QMF_HIDDEN;
	s_library.subTopicArrowUp.generic.x					= 31;
	s_library.subTopicArrowUp.generic.y					= 182;
	s_library.subTopicArrowUp.generic.name				= "menu/common/arrow_up_16.tga";
	s_library.subTopicArrowUp.generic.id				= ID_ARROW2UP;
	s_library.subTopicArrowUp.generic.callback			= M_Library_Event; 
	s_library.subTopicArrowUp.width						= 18;
	s_library.subTopicArrowUp.height					= 18;
	s_library.subTopicArrowUp.color						= CT_DKORANGE;
	s_library.subTopicArrowUp.color2					= CT_LTORANGE;

	s_library.subTopicArrowDown.generic.type			= MTYPE_BITMAP;      
	s_library.subTopicArrowDown.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_library.subTopicArrowDown.generic.x				= 31;
	s_library.subTopicArrowDown.generic.y				= 336;
	s_library.subTopicArrowDown.generic.name			= "menu/common/arrow_dn_16.tga";
	s_library.subTopicArrowDown.generic.id				= ID_ARROW2DOWN;
	s_library.subTopicArrowDown.generic.callback		= M_Library_Event; 
	s_library.subTopicArrowDown.width					= 18;
	s_library.subTopicArrowDown.height					= 18;
	s_library.subTopicArrowDown.color					= CT_DKORANGE;
	s_library.subTopicArrowDown.color2					= CT_LTORANGE;

/*	s_library.computerVoice.generic.type			= MTYPE_BITMAP;      
	s_library.computerVoice.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_library.computerVoice.generic.x				= 360;
	s_library.computerVoice.generic.y				= 368;
	s_library.computerVoice.generic.name			= GRAPHIC_SQUARE;
	s_library.computerVoice.generic.id				= ID_COMPUTERVOICE;
	s_library.computerVoice.generic.callback		= M_Library_Event; 
	s_library.computerVoice.width					= MENU_BUTTON_MED_WIDTH + 30;
	s_library.computerVoice.height					= MENU_BUTTON_MED_HEIGHT;
	s_library.computerVoice.color					= CT_DKORANGE;
	s_library.computerVoice.CT_DKBROWN1					= CT_LTORANGE;
	s_library.computerVoice.textEnum				= MBT_COMPUTERVOICEON;
	s_library.computerVoice.textX					= MENU_BUTTON_TEXT_X + 10;*/

	Menu_AddItem( &s_library.menu,	&s_library.quitMenu );
	Menu_AddItem( &s_library.menu,	&s_library.topicArrowUp );
	Menu_AddItem( &s_library.menu,	&s_library.topicArrowDown );
	Menu_AddItem( &s_library.menu,	&s_library.subTopicArrowUp );
	Menu_AddItem( &s_library.menu,	&s_library.subTopicArrowDown );
//	Menu_AddItem( &s_library.menu,	&s_library.computerVoice );

	holdMainTopicButton = &s_library.mainTopicButton1;
	pad = 22;
	y =  62;
	x = 30;
	s_library.maxMainTopics = 0;
	s_library.topMainTopic = 0;		// Top of Main topics starts at index 0

	for (i=0;i<MAXMAINTOPICDISPLAYED;i++)
	{
		holdMainTopicButton->generic.type				= MTYPE_BITMAP;      
		holdMainTopicButton->generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
		holdMainTopicButton->generic.x					= x;
		holdMainTopicButton->generic.y					= y;
		holdMainTopicButton->generic.name				= GRAPHIC_BUTTONRIGHT;
		holdMainTopicButton->generic.id					= ID_MAINTOPICBUTTON1 + i;
		holdMainTopicButton->generic.callback			= M_Library_Event; 
//		holdMainTopicButton->generic.statusbarfunc		= Library_StatusBar;
		holdMainTopicButton->width						= MENU_BUTTON_MED_WIDTH + 20;
		holdMainTopicButton->height						= MENU_BUTTON_MED_HEIGHT;
		holdMainTopicButton->color						= CT_DKORANGE;
		holdMainTopicButton->color2						= CT_LTORANGE;
		holdMainTopicButton->textX						= MENU_BUTTON_TEXT_X;
		holdMainTopicButton->textY						= MENU_BUTTON_TEXT_Y;
		holdMainTopicButton->textPtr					= libraryText[i].mainTopic;
		holdMainTopicButton->textcolor					= CT_BLACK;
		holdMainTopicButton->textcolor2					= CT_WHITE;
		holdMainTopicButton->textStyle					= UI_SMALLFONT;

		holdMainTopicButton++;
		s_library.maxMainTopics++;

		y += pad;
	}

	holdMainTopicButton = &s_library.mainTopicButton1;
	for (i=0;i<MAXMAINTOPICDISPLAYED;i++)
	{
		if (libraryText[i].mainTopic)
		{
			Menu_AddItem( &s_library.menu,	holdMainTopicButton );
		}
		holdMainTopicButton++;
	}

	// Make top main topic button text gold
	holdMainTopicButton = &s_library.mainTopicButton1;
	holdMainTopicButton->textcolor	= CT_LTGOLD1;

	s_library.topSubTopic = 0;	// Top of subtopics starts at index 0

	holdSubTopicButton = &s_library.subTopicButton1;
	pad = 22;
	y =  180;
	x = 62;

	for (i=0;i<MAXSUBTOPICDISPLAYED;i++)
	{
		holdSubTopicButton->generic.type				= MTYPE_BITMAP;      
		holdSubTopicButton->generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
		holdSubTopicButton->generic.x					= x;
		holdSubTopicButton->generic.y					= y;
		holdSubTopicButton->generic.name				= GRAPHIC_SQUARE;
		holdSubTopicButton->generic.id					= ID_SUBTOPICBUTTON1 + i;
		holdSubTopicButton->generic.callback			= M_Library_Event; 
//		holdSubTopicButton->generic.statusbarfunc		= Library_StatusBar;
		holdSubTopicButton->width						= MENU_BUTTON_MED_WIDTH;
		holdSubTopicButton->height						= MENU_BUTTON_MED_HEIGHT;
		holdSubTopicButton->color						= CT_LTORANGE;
		holdSubTopicButton->color2						= CT_LTORANGE;
		holdSubTopicButton->textX						= MENU_BUTTON_TEXT_X;
		holdSubTopicButton->textY						= MENU_BUTTON_TEXT_Y;
		holdSubTopicButton->textPtr						= libraryText[0].subTopic[i];
		holdSubTopicButton->textcolor					= CT_BLACK;
		holdSubTopicButton->textcolor2					= CT_WHITE;
		holdSubTopicButton->textStyle					= UI_SMALLFONT;

		if (libraryText[0].subTopicDesc[i])
		{
			holdSubTopicButton->generic.flags	= QMF_HIGHLIGHT_IF_FOCUS;
		}
		else
		{
			holdSubTopicButton->generic.flags	= QMF_HIDDEN;
		}

		Menu_AddItem( &s_library.menu,	holdSubTopicButton );

		holdSubTopicButton++;

		y += pad;
	}

	// Make top subtopic button text gold
	holdSubTopicButton = &s_library.subTopicButton1;
	holdSubTopicButton->textcolor	= CT_VLTGOLD1;

	y = 120;
	for (i=LMG_BIO_DESC1; i<(LMG_BIO_DESC1 +LOG_MAXDESC );i++)
	{
		logmenu_graphics[i].x = 242;
		logmenu_graphics[i].y = y;
		y += 12;
	}

	s_library.mainTopic = 0;
	s_library.subTopic = 0;

	ClearLibraryDesc();

	s_library.playerModel.generic.type			= MTYPE_BITMAP;
	s_library.playerModel.generic.flags			= QMF_INACTIVE;
	s_library.playerModel.generic.x				= 430;
	s_library.playerModel.generic.y				= 95;
	s_library.playerModel.width					= 32*7.6;
	s_library.playerModel.height				= 56*7.6;

	// Show down arrow indicator???
	if ((s_library.topMainTopic + MAXMAINTOPICDISPLAYED) >= s_library.cntMainTopic)
	{
		s_library.topicArrowDown.generic.flags |= QMF_HIDDEN;
	}
	else
	{
		s_library.topicArrowDown.generic.flags &= ~QMF_HIDDEN;
	}

	// Show down arrow indicator???
	if ((s_library.topSubTopic + MAXSUBTOPICDISPLAYED) >= s_library.cntSubTopic[s_library.mainTopic])
	{
		s_library.subTopicArrowDown.generic.flags |= QMF_HIDDEN;
	}
	else
	{
		s_library.subTopicArrowDown.generic.flags &= ~QMF_HIDDEN;
	}

	s_library.subTopic = -1;
	ChangeLibraryDesc(0);
}

/*
=================
UI_ParseLibraryText
=================
*/
static void UI_ParseLibraryText()
{
	char	*token;
	char *buffer,*holdPtr;
	int i,len;

	memset(libraryText,0,sizeof(libraryText));

	s_library.mainTopic = 0;
	s_library.subTopic = 0;
	s_library.cntMainTopic=0;

	COM_BeginParseSession();

	buffer = LibraryText;
	i = 1;	// Zero is null string
	while ( buffer ) 
	{
		token = COM_ParseExt( &buffer, qtrue );

		if (!Q_strncmp(token,"MAINTOPIC",9))
		{
			s_library.mainTopic = atoi(&token[9]);
			s_library.mainTopic--;

			// Get main topic 
			token = COM_ParseExt( &buffer, qtrue );	
			len = strlen(token);
			libraryText[s_library.mainTopic].mainTopic = buffer - (len+1); 
			holdPtr = libraryText[s_library.mainTopic].mainTopic + len;
			*holdPtr = 0; //Null

			s_library.cntSubTopic[s_library.mainTopic] = 0;
			s_library.cntMainTopic++;
		}
		else if (!Q_strncmp(token,"MAINDESC",8))
		{
			// Get main topic desc
			token = COM_ParseExt( &buffer, qtrue );	
			len = strlen(token);
			libraryText[s_library.mainTopic].mainTopicDesc = buffer - (len+1); 
			holdPtr = libraryText[s_library.mainTopic].mainTopicDesc + len;
			*holdPtr = 0; //NULL
		}
		else if (!Q_strncmp(token,"SUBTOPIC",8))
		{
			s_library.subTopic = atoi(&token[8]);
			s_library.subTopic--;

			// Get main topic desc
			token = COM_ParseExt( &buffer, qtrue );	
			len = strlen(token);
			libraryText[s_library.mainTopic].subTopic[s_library.subTopic] = buffer - (len+1); 
			holdPtr = libraryText[s_library.mainTopic].subTopic[s_library.subTopic] + len;
			*holdPtr = 0; //NULL

			s_library.cntSubTopic[s_library.mainTopic]++;

		}
		else if (!Q_strncmp(token,"SUBDESC",7))
		{
			// Get main topic desc
			token = COM_ParseExt( &buffer, qtrue );	
			len = strlen(token);
			libraryText[s_library.mainTopic].subTopicDesc[s_library.subTopic] = buffer - (len+1); 
			holdPtr = libraryText[s_library.mainTopic].subTopicDesc[s_library.subTopic] + len;
			*holdPtr = 0; //NULL
		}
		else if (!Q_strncmp(token,"TEXTWIDTH",9))
		{
			// Get main topic desc
			token = COM_ParseExt( &buffer, qtrue );	
			libraryText[s_library.mainTopic].textWidth[s_library.subTopic] = atoi(token);
		}
		else if (!Q_strncmp(token,"TEXT",4))
		{
			// Get main topic desc
			token = COM_ParseExt( &buffer, qtrue );	
			len = strlen(token);
			libraryText[s_library.mainTopic].text[s_library.subTopic] = buffer - (len+1); 
			holdPtr = libraryText[s_library.mainTopic].text[s_library.subTopic] + len;
			*holdPtr = 0; //NULL
		}
		else if (!Q_strncmp(token,"STARTTEXTY",10))
		{
			token = COM_ParseExt( &buffer, qtrue );	
			libraryText[s_library.mainTopic].textY[s_library.subTopic] = atoi(token);
		}
		else if (!Q_strncmp(token,"MODELLEGS",9))
		{
			token = COM_ParseExt( &buffer, qtrue );	
			len = strlen(token);
			libraryText[s_library.mainTopic].modelLegs[s_library.subTopic] = buffer - (len+1); 
			holdPtr = libraryText[s_library.mainTopic].modelLegs[s_library.subTopic] + len;
			*holdPtr = 0; //NULL
		}
		else if (!Q_strncmp(token,"MODELHEAD",9))
		{
			token = COM_ParseExt( &buffer, qtrue );	
			len = strlen(token);
			libraryText[s_library.mainTopic].modelHead[s_library.subTopic] = buffer - (len+1); 
			holdPtr = libraryText[s_library.mainTopic].modelHead[s_library.subTopic] + len;
			*holdPtr = 0; //NULL
		}
		else if (!Q_strncmp(token,"MODELTORSO",10))
		{
			token = COM_ParseExt( &buffer, qtrue );	
			len = strlen(token);
			libraryText[s_library.mainTopic].modelTorso[s_library.subTopic] = buffer - (len+1); 
			holdPtr = libraryText[s_library.mainTopic].modelTorso[s_library.subTopic] + len;
			*holdPtr = 0; //NULL
		}
		else if (!Q_strncmp(token,"MODELX",6))
		{
			token = COM_ParseExt( &buffer, qtrue );	
			libraryText[s_library.mainTopic].modelX[s_library.subTopic] = atoi(token);
		}
		else if (!Q_strncmp(token,"MODELYAW",8))
		{
			token = COM_ParseExt( &buffer, qtrue );	
			libraryText[s_library.mainTopic].modelYaw[s_library.subTopic] = atoi(token);
		}
		else if (!Q_strncmp(token,"MODELPITCH",10))
		{
			token = COM_ParseExt( &buffer, qtrue );	
			libraryText[s_library.mainTopic].modelPitch[s_library.subTopic] = atoi(token);
		}
		else if (!Q_strncmp(token,"MODELROLL",9))
		{
			token = COM_ParseExt( &buffer, qtrue );	
			libraryText[s_library.mainTopic].modelRoll[s_library.subTopic] = atoi(token);
		}
		else if (!Q_strncmp(token,"MODELY",6))
		{
			token = COM_ParseExt( &buffer, qtrue );	
			libraryText[s_library.mainTopic].modelY[s_library.subTopic] = atoi(token);
		}
		else if (!Q_strncmp(token,"MODELDISTANCE",13))	// Model's distance from camera
		{
			token = COM_ParseExt( &buffer, qtrue );	
			libraryText[s_library.mainTopic].modelDistance[s_library.subTopic] = atoi(token);
		}
		else if (!Q_strncmp(token,"MODELORIGINY",12))	// Model's change in Y origin
		{
			token = COM_ParseExt( &buffer, qtrue );	
			libraryText[s_library.mainTopic].modelOriginY[s_library.subTopic] = atoi(token);
		}
		else if (!Q_strncmp(token,"MODEL",5))
		{
			// Get main topic desc
			token = COM_ParseExt( &buffer, qtrue );	
			len = strlen(token);
			libraryText[s_library.mainTopic].model[s_library.subTopic] = buffer - (len+1); 
			holdPtr = libraryText[s_library.mainTopic].model[s_library.subTopic] + len;
			*holdPtr = 0; //NULL

			libraryText[s_library.mainTopic].modelHandle[s_library.subTopic] = 
				trap_R_RegisterModel(libraryText[s_library.mainTopic].model[s_library.subTopic]);
		}
		else if (!Q_strncmp(token,"SHADERXPOS",10))
		{
			token = COM_ParseExt( &buffer, qtrue );	
			libraryText[s_library.mainTopic].shaderXpos[s_library.subTopic] = atoi(token);
		}
		else if (!Q_strncmp(token,"SHADERYPOS",10))
		{
			token = COM_ParseExt( &buffer, qtrue );	
			libraryText[s_library.mainTopic].shaderYpos[s_library.subTopic] = atoi(token);
		}
		else if (!Q_strncmp(token,"SHADERXLENGTH",13))
		{
			token = COM_ParseExt( &buffer, qtrue );	
			libraryText[s_library.mainTopic].shaderXlength[s_library.subTopic] = atoi(token);
		}
		else if (!Q_strncmp(token,"SHADERYLENGTH",13))
		{
			token = COM_ParseExt( &buffer, qtrue );	
			libraryText[s_library.mainTopic].shaderYlength[s_library.subTopic] = atoi(token);
		}
		else if (!Q_strncmp(token,"SHADER",6))
		{
			// Get main topic desc
			token = COM_ParseExt( &buffer, qtrue );	
			len = strlen(token);
			libraryText[s_library.mainTopic].shader[s_library.subTopic] = buffer - (len+1); 
			holdPtr = libraryText[s_library.mainTopic].shader[s_library.subTopic] + len;
			*holdPtr = 0; //NULL

			libraryText[s_library.mainTopic].shaderHandle[s_library.subTopic] = 
				trap_R_RegisterShaderNoMip(libraryText[s_library.mainTopic].shader[s_library.subTopic]);
		}
		else if (!Q_strncmp(token,"COMMAND",7))
		{
			token = COM_ParseExt( &buffer, qtrue );	
			len = strlen(token);
			libraryText[s_library.mainTopic].command[s_library.subTopic] = buffer - (len+1); 
			holdPtr = libraryText[s_library.mainTopic].command[s_library.subTopic] + len;
			*holdPtr = 0; //NULL
		}
		else if (!Q_strncmp(token,"SOUND",5))
		{
			token = COM_ParseExt( &buffer, qtrue );	
			len = strlen(token);
			libraryText[s_library.mainTopic].sound[s_library.subTopic] = buffer - (len+1); 
			holdPtr = libraryText[s_library.mainTopic].sound[s_library.subTopic] + len;
			*holdPtr = 0; //NULL
		}
	}
}

/*
===============
UI_LibraryMenu_Cache
===============
*/
void UI_LibraryMenu_Cache (void)
{
	//char	*buffer/*,*filePtr*/;
//	char* buffer;
	char	filename[MAX_QPATH];
	int		len,i,i2;
	fileHandle_t	f;

	s_library.leftRound = trap_R_RegisterShaderNoMip("menu/common/halfroundl_24.tga");
	s_library.corner_ur_20_24 = trap_R_RegisterShaderNoMip("menu/common/corner_ur_20_24.tga");
	s_library.corner_lr_18_20 = trap_R_RegisterShaderNoMip("menu/common/corner_lr_18_20.tga");
	s_library.corner_ll_18_47 = trap_R_RegisterShaderNoMip("menu/common/corner_ll_47_18.tga");
	s_library.corner_ll_8_47  = trap_R_RegisterShaderNoMip("menu/common/corner_ll_8_47.tga");
	s_library.corner_lr_8_20  = trap_R_RegisterShaderNoMip("menu/common/corner_lr_8_20.tga");

	UI_LanguageFilename("ext_data/sp_library","dat",filename);
	//filePtr = "SP_LIBRARY.DAT";

	len = trap_FS_FOpenFile(  filename, &f, FS_READ );

	if ( !f ) 
	{
		Com_Error(ERR_FATAL, va("UI_LibraryMenu_Cache : sp_library.dat file not found!\n"));
		return;
	}

	if ( len > MAXLIBRARYTEXT ) 
	{
		Com_Printf( S_COLOR_RED "UI_LibraryMenu_Cache : sp_library.dat size (%d) > max (%d)!\n", len, MAXLIBRARYTEXT);
		return;
	}

	// initialise the data area
	memset(LibraryText, 0, sizeof(LibraryText));

	trap_FS_Read( LibraryText, len, f ); //Menutext

	//strncpy( LibraryText, buffer, sizeof( LibraryText ) - 1 );
	trap_FS_FCloseFile( f );

	UI_ParseLibraryText();

	nullSound =	trap_S_RegisterSound("sound/null.wav");

	for (i=0;i<=s_library.mainTopic;i++)		
	{
		for (i2=0;i2<s_library.cntSubTopic[i];i2++)
		{
			if (libraryText[i].sound[i2])
			{
				libraryText[i].soundHandle[i2] = trap_S_RegisterSound(libraryText[i].sound[i2]);
			}
		}
	}
}

/*
===============
UI_Library_SpecialCache
===============
*/
void UI_Library_SpecialCache(void)
{
	UI_LibraryMenu_Cache();
}

/*
===============
LibraryMenu_LoadText
===============
*/
void LibraryMenu_LoadText (void)
{
	UI_LibraryMenu_Cache();
}

/*
===============
UI_LibraryMenu
===============
*/
void UI_LibraryMenu(void)
{
	uis.menusp = 0;

	ingameFlag = qtrue;	// true when in game menu is in use

	Mouse_Show();

	//s_library.menuType = MENUTYPE_LIBRARY;

	LibraryMenu_LoadText();

	LibraryMenu_Init(); 

	UI_PushMenu( &s_library.menu );

	Menu_AdjustCursor( &s_library.menu, 1 );	
}


/*
=================
M_Accessing_Graphics
=================
*/
void LibraryAccessingMenu_Draw (void)
{
	int y;

	y = 50;


	trap_R_SetColor( colorTable[CT_DKPURPLE2]);

	UI_DrawHandlePic( 132, y+ 42,  128,  -64, s_libraryaccessing.cornerPic);	// Top Left corner
	UI_DrawHandlePic( 132, y+252,  128,   64, s_libraryaccessing.cornerPic);	// Bottom Left corner

	UI_DrawHandlePic( 429, y+ 42, -128,  -64, s_libraryaccessing.cornerPic);	// Top Right corner
	UI_DrawHandlePic( 429, y+252, -128,   64, s_libraryaccessing.cornerPic);	// Bottom Right corner

	UI_DrawHandlePic(145, y+75,  395, 18, uis.whiteShader);		// Top
	UI_DrawHandlePic(132, y+93,  47, 175, uis.whiteShader);		// Left side
	UI_DrawHandlePic(510, y+93,  47, 175, uis.whiteShader);		// Right side
	UI_DrawHandlePic(147,y+265,  65, 18, uis.whiteShader);		// Bottom Left
	UI_DrawHandlePic(477,y+265,  65, 18, uis.whiteShader);		// Bottom Right
	UI_DrawHandlePic(214,y+265,  261, 18, uis.whiteShader);		// Bottom

	UI_DrawProportionalString(345,y+159,menu_normal_text[MNT_ACCESSING],UI_SMALLFONT | UI_CENTER,colorTable[CT_LTGOLD1]);

	// Wait a second to display the accessing screen and then go to the Library stuff 
	if (s_libraryaccessing.timer <= uis.realtime)
	{
		UI_PopMenu();
		UI_LibraryMenu();
	}
}

/*
===============
UI_AccessingMenu_Cache
===============
*/
void UI_AccessingMenu_Cache (void)
{
	s_libraryaccessing.cornerPic = trap_R_RegisterShaderNoMip("menu/common/corner_ll_47_18.tga");
}

/*
===============
AccessingMenu_Init
===============
*/
void AccessingMenu_Init(void)
{

	UI_AccessingMenu_Cache();

	s_libraryaccessing.menu.nitems						= 0;
	s_libraryaccessing.menu.draw						= LibraryAccessingMenu_Draw;
	s_libraryaccessing.menu.key							= NULL;
	s_libraryaccessing.menu.fullscreen					= qtrue;
	s_libraryaccessing.menu.wrapAround					= qtrue;
	s_libraryaccessing.menu.descX						= MENU_DESC_X;
	s_libraryaccessing.menu.descY						= MENU_DESC_Y;
	s_libraryaccessing.menu.titleX						= MENU_TITLE_X;
	s_libraryaccessing.menu.titleY						= MENU_TITLE_Y;
//	s_libraryaccessing.menu.titleI						= MNT_CREDITSMENU_TITLE;
	s_libraryaccessing.menu.footNoteEnum				= MNT_CREDITS;

	s_libraryaccessing.timer = uis.realtime + 1000;
}

/*
===============
UI_AccessingMenu
===============
*/
void UI_AccessingMenu()
{
	ingameFlag = qtrue;	// true when in game menu is in use

	Mouse_Hide();

	AccessingMenu_Init(); 
	UI_PushMenu( &s_libraryaccessing.menu );
}