// Copyright (C) 2006 UberGames
// RPG-X: UI Credits Menu
// Developer: -=TiM=-
// Function: To load in, cache, initialize and draw all of the elements that create the Credits menu
//
// Marcin: I've modified this stuff okay? - 03/01/2009



#include "ui_local.h"
#include "ui_logger.h"

enum ui_creditsLimits_e {
	MAX_MENUS = 9,	//Max number of menus
	MAX_DEVS = 15	//Max number of developers per menu
};

//Just to be on the safe side, I made these increment from 1 rather than 0
//and then compensated in the code. :)
enum ui_creditsIDs_e {
	ID_LEADS = 1,
	ID_CODERS,
	ID_2D,
	ID_3D,
	ID_MAPPING,
	ID_SOUNDS,
	ID_EXTRAS,
	ID_BETAS,
	ID_THANKS,
	ID_MAINMENU = 100
};

static const char MAIN_LOGO_MODEL[] = "models/mapobjects/rpg-x/logo.md3";
static const char LCARS_CORNER_U[] = "menu/common/corner_lr_7_12.tga";
static const char LCARS_LOGO_BACKDROP[] = "menu/rpgx_credits/logo_backdrop";
static const char LCARS_BLIP[] = "sound/interface/button7.wav";

//----------------------------------------------------------------
//TiM : In order to lower memory by recycling any repeated names

//Developer Names
static const char NAME_PHENIX[] = "Dominic 'Phenix' Black";
static const char NAME_JAY[] = "Jason 'J2J' Griffith";
static const char NAME_SHARKY[] = "Nazar 'Sharky' Surmai";
static const char NAME_RED[] = "Stephen 'RedTechie' Shamakian";
static const char NAME_TIM[] = "Timothy 'TiM' Oliver";
static const char NAME_SCOOTER[] = "Scooter";
static const char NAME_MARCIN[] = "Marcin 'turbomarcin' Koziuk";
static const char NAME_GSIO01[] = "Walter 'GSIO01' Hennecke";
static const char NAME_YOUNG[] = "Hendrik 'Harry Young' Gerritzen";
static const char NAME_WILL[] = "William Riker";
static const char NAME_ANT[] = "Anthony";
static const char NAME_TDP[] = "The Dark Project";

//#define	NAME_DI				"Digital Intervention" //TiM: DI is the brand I put all of my high-end 3D artwork under now as standardiazation for client work. :)

static const char NAME_SIMMO[] = "Tom 'Simmo666' Simpson";
static const char NAME_KURO[] = "Kuro-chan"; //Kare to kare no nihongo no na dazo lol. :P

static const char NAME_JAREN[] = "Jaren";

static const char NAME_MONROE[] = "James 'Emorog' Monroe";
static const char NAME_GENE[] = "Gene Roddenberry";
static const char NAME_RAVEN[] = "Raven Software";
static const char NAME_EFPEEPS[] = "The EF RPG Community";
static const char NAME_STEVE[] = "Steve"; //Never even met this d00d lol


static const char NAME_JEROEN[] = "Jeroen";

static const char NAME_LAZ[] = "Laz Rojas";

static const char NAME_PARKER[] = "Chris 'Parker' Vad";
static const char NAME_HOCKING[] = "AdmiralHocking";

//Developer Uber professional sounding titles
static const char TITLE_LEADER[] = "Project Lead";
static const char TITLE_LPROG[] = "Lead Coding";
static const char TITLE_L2DAS[] = "Lead 2-D & Audio Art";
static const char TITLE_L3D[] = "Lead 3-D Art";
static const char TITLE_LMAPPER[] = "Lead Level Design";
static const char TITLE_LLUA[] = "Lead Lua Coding";

//Programming titles
static const char TITLE_PROG[] = "Primary Coding";
static const char TITLE_GPROG[] = "Graphics Coding";
static const char TITLE_ADPROG[] = "Additional Coding";

//2-D Titles
static const char TITLE_L2D[] = "Lead 2-D Art";
static const char TITLE_LCARS[] = "Secondary 2-D Art / LCARS Design"; //Simmo special lol
static const char TITLE_AD2D[] = "Additional 2-D Art";
static const char TITLE_RANKS[] = "Menu Rank Icons Creation"; // Kuro special
static const char TITLE_ACAI[] = "Additional Coding / Many ideas";

//3-D Titles... none O_O

//Mapping titles
static const char TITLE_LEVELART[] = "Level Design";
static const char TITLE_LUAENH[] = "Lua Level Enhancement";

//Audio Titles
static const char TITLE_LAUDIO[] = "Audio Lead";
static const char TITLE_ADAUDIO[] = "Additional Audio";

//Additional Titles
static const char TITLE_CANON[] = "Canon Inspector";//Scooter... and only Scooter hehe
static const char TITLE_DOCU[] = "Documentation";
static const char TITLE_DEUTSCH[] = "German Translation";
static const char TITLE_NEDERLANDS[] = "Dutch Translation";

//#define	TITLE_CIN			"Opening Cinematic"
static const char TITLE_SKINSMODELS[] = "Providing Additional Skins and Models";

//Thank you Titles
static const char TITLE_TREK[] = "Creator of Star Trek";
static const char TITLE_EF[] = "Creators of Elite Force";
static const char TITLE_TEST[] = "For Suggestions and Additional Testing";
static const char TITLE_PLAGIA[] = "Creator of the original EF RPG Mod";
static const char TITLE_RAVENCODER[] = "EF Engine Consulting and Assistance";

//----------------------------------------------------------------

static qhandle_t			cornerUpper;
static qhandle_t			spaceBackdrop;

static int32_t i;

static float cm_buttons[9][2] = 
{
	{129,62},
	{129,86},
	{129,109},
	{305,62},
	{305,86},
	{305,109},
	{481,62},
	{481,86},
	{481,109},
};

typedef struct 
{
	menuframework_s	menu;

	menulist_s		list;

	menubitmap_s	mainmenu;
	menubitmap_s	leads;
	menubitmap_s	coders;
	menubitmap_s	twodee;
	menubitmap_s	threedee;
	menubitmap_s	mapping;
	menubitmap_s	sounds;
	menubitmap_s	extras;
	menubitmap_s	betas;
	menubitmap_s	thanks;

	qhandle_t		logoModel;
	sfxHandle_t		pingSound;

	int32_t				startTime;	//the time index the player hit t3h button, so we can calc display times
	int32_t				beepTime;	//increment this each time a new item appears, so we can play beep then. :)

	int32_t				creditsNum;	//Index of the button we last clicked
} credits_t;

static credits_t	s_credits;

typedef	struct
{
	const char*	name;
	const char*	position;
} devInfo_t;

typedef struct
{
	char		mainTitle[MAX_NAME_LENGTH];
	devInfo_t	devInfo[MAX_DEVS];
} creditsInfo_t;

//Arrays were cool for the first vers of the credits menu,
//But structs kick the utter crap out of straight arrays :)
static creditsInfo_t	creditsInfo[MAX_MENUS] = 
{
	//The d00ds leading this shizzie :)
	{ 
		"PROJECT LEADS", 
		{
			{ NAME_PHENIX,			TITLE_LEADER },
			{ NAME_GSIO01,			TITLE_LEADER },			
			{ NAME_JAY,				TITLE_LPROG },
			{ NAME_SHARKY,			TITLE_L2DAS },
			{ NAME_TIM,				TITLE_L3D },
			{ NAME_WILL,			TITLE_LMAPPER }, 
		}
	},

	//The d00ds l33tzor haxxoring this shizzie.
	{ 
		"PROGRAMMING",
		{
			{ NAME_JAY,				TITLE_LPROG },
			{ NAME_GSIO01,			TITLE_LPROG },
			{ NAME_TIM,				TITLE_PROG },
			{ NAME_PHENIX,			TITLE_PROG },
			{ NAME_MARCIN,			TITLE_PROG }, // Scooter was originally here...sorry... not enough space!
			{ NAME_RED,				TITLE_ADPROG },
			//{ NAME_SCOOTER,			TITLE_ADPROG }, //one had to go.
			{ NAME_YOUNG,			TITLE_LLUA }  
		}
	},

	//The d00ds painting and texturing this shizzie.
	{ 
		"2-D ART",
		{
			{ NAME_SHARKY,			TITLE_L2D },
			{ NAME_SIMMO,			TITLE_LCARS },
			{ NAME_TIM,				TITLE_AD2D },
			{ NAME_SCOOTER,			TITLE_AD2D },
			{ "Alexander Richardson", "Master System Display's" },
			{ NAME_KURO,			TITLE_RANKS },
		}
	},

	//The d00d modeling this shizzie.  I feel so alone rofl. 
	{ 
		"3-D ART",
		{
			{ NAME_TIM,				TITLE_L3D },
			//{ "Ralph Schoberth",		"Enterprise-E LightWave Mesh" }, //FixMe: Put mesh credits in a smaller section below here somehow lol
		}
	},

	//The d00ds mapping this shizzie.
	{ 
		"LEVEL DESIGN",
		{
			{ NAME_WILL,				TITLE_LMAPPER },
			{ NAME_ANT,					TITLE_LEVELART },
			//{ "Jack Amzadi",			"Level Artist" }, //Seriously... did we see ANYTHING come out of these guys? O_o
			//{ "Johan",				"Level Artist" },
			//{ "RED-RUM",				"Level Artist" },
			{ NAME_PHENIX,				TITLE_LEVELART },
			//{ NAME_SCOOTER,			TITLE_LEVELART }, //I think he wanted to be taken off
			//{ NAME_WILL,				TITLE_LEVELART },
			{ NAME_YOUNG,				TITLE_LUAENH }  
		}
	},

	//The d00ds... uh.. soundzor-ing this shizzie. :)
	{ 
		"AUDIO DESIGN",
		{
			{ NAME_SHARKY,				TITLE_LAUDIO },
			{ NAME_PHENIX,				TITLE_ADAUDIO },
			{ NAME_TIM,					TITLE_ADAUDIO },
			{ NAME_SCOOTER,				TITLE_ADAUDIO },
			{ NAME_TDP,					TITLE_ADAUDIO },
		}
	},

	{ 
		"ADDITIONAL SUPPORT",
		{
			{ NAME_SCOOTER,				TITLE_CANON },
			//{ "Highlander",			"Public Relations" }, //He didn't publicly relate anything in this mod, this edition. Plus I want the space at the bottom of this lol
			{ NAME_GSIO01,				TITLE_DEUTSCH },
			{ NAME_JEROEN,				TITLE_NEDERLANDS },
			{ NAME_MARCIN,				TITLE_NEDERLANDS },
			{ NAME_LAZ,					TITLE_SKINSMODELS },
			//{ NAME_PHENIX,			TITLE_DOCU },
			//{ NAME_SHARKY,			TITLE_DOCU },
			//{ NAME_RED,				TITLE_DOCU },
			//{ NAME_DI,				TITLE_CIN },
			{ NAME_PARKER,				TITLE_ACAI },
			{ NAME_HOCKING,				TITLE_AD2D }
		}
	},

	//The d00ds beta testing this shizzie 
	{ 
		"BETA TESTERS",
		{
			{ "Jordan",					"" },
			{ "sharpkiller",			"" },
			{ "Telex Ferra",			"" },
			{ "AdmiralHocking",			"" },
			{ "Lee Wolfgang",			"" },
			{ "Quince",					"" },
			{ NAME_YOUNG,				"" },  
			/*{ "Alex L.",				"" },
			{ "Chase Benedict",			"" },
			{ "Jake Conhale",			"" },
			{ "James Young",			"" },
			{ "Kadratis Velevere",		"" },
			{ "Ricksal0224",			"" },
			{ "SimmerALPHA",			"" },
			{ "Slayer",					"" },
			{ NAME_MARCIN,				"" },
			{ "Tuskin"					"" }*/
			/*{ "Alex L.",				"" }, 
			{ "Alex Mcpherson",			"" },
			{ "AlphaOmega",				"" },
			{ "Andrew",					"" },
			{ "Crusader",				"" },
			{ "Diaz",					"" },
			{ "Fred",					"" },
			{ "Jake Conhale",			"" },
			{ "Martin",					"" },
			{ "Mr Fibbles",				"" },
			{ "Myntz",					"" },
			{ "Nuttycomputer",			"" },
			{ "Rigs",					"" },*/
			//{ "Pending...",			"" }
		}
	},

	//Totally awesome d00ds whose input contributed greatly to this shizzie. :)
	{ 
		"SPECIAL THANKS",	
		{
			{ NAME_GENE,			TITLE_TREK },
			{ NAME_RAVEN,			TITLE_EF },
			{ NAME_EFPEEPS,			TITLE_TEST },
			{ NAME_STEVE,			TITLE_PLAGIA },
			{ NAME_MONROE,			TITLE_RAVENCODER }
		}
	}

};

/*
=================
UI_drawCreditNames
TiM: From the various arrays above, this function systematically
draws each value from the array in a specific order whilst playing a sound
for each set
=================
*/
void UI_drawCreditNames( int32_t creditsIndex )
{
	UI_LogFuncBegin();
	int32_t timeDeviation; //used to separate the times when each credit appears
	int32_t yDeviation; //used to place each separate credit down the y-axis each loop
	
	//w00t! I can count the length of arrays dynamically now!!!!!!! ^_^!!!!
	//*SOB* Didn't work :(
	//int32_t arrayLen = sizeof ( creditSec ) / sizeof ( creditSec[0] ); 
	
	//trap_Print ( va( "arrayLen = %i\n", arrayLen ) );

	timeDeviation = 200;
	yDeviation = 40;

	{
		if (uis.realtime >= s_credits.startTime + timeDeviation ) { //After .2 seconds, the title will appear
			UI_DrawProportionalString(  512,  184, creditsInfo[creditsIndex].mainTitle,UI_CENTER|UI_BIGFONT, colorTable[CT_LTGOLD1]);

			/*if(uis.realtime > s_credits.startTime + timeDeviation && uis.realtime < s_credits.startTime + timeDeviation + 20){
				trap_S_StartLocalSound(s_credits.pingSound, CHAN_LOCAL ); //play the sound
			}*/
			if ( s_credits.beepTime == 0 ) {
				trap_S_StartLocalSound(s_credits.pingSound, CHAN_LOCAL );
				s_credits.beepTime++;
			}

			timeDeviation += 200;
		}

		for( i = 0; i < MAX_DEVS; i++ )
		{
			if ( !creditsInfo[creditsIndex].devInfo[i].name[0] ) {
				break;
			}

			if (uis.realtime > s_credits.startTime + timeDeviation) {

				//For entries that have positions (ie, not beta testers )
				if ( i <= 9 && creditsInfo[creditsIndex].devInfo[i].position[0] ) {	
					if(creditsIndex == 1 || creditsIndex == 6) { // make all programmers|additional people fit in
						UI_DrawProportionalString(  420,  169 + yDeviation, creditsInfo[creditsIndex].devInfo[i].name,UI_LEFT|UI_SMALLFONT, colorTable[CT_DKPURPLE2]);
						UI_DrawProportionalString(  420,  187 + yDeviation, creditsInfo[creditsIndex].devInfo[i].position,UI_LEFT|UI_TINYFONT, colorTable[CT_DKPURPLE2]); //18 varaition
					} else {
						UI_DrawProportionalString(  420,  180 + yDeviation, creditsInfo[creditsIndex].devInfo[i].name,UI_LEFT|UI_SMALLFONT, colorTable[CT_DKPURPLE2]);
						UI_DrawProportionalString(  420,  198 + yDeviation, creditsInfo[creditsIndex].devInfo[i].position,UI_LEFT|UI_TINYFONT, colorTable[CT_DKPURPLE2]); //18 varaition
					}
					//Play a beep effect
					if ( i == s_credits.beepTime-1 ) {
						trap_S_StartLocalSound(s_credits.pingSound, CHAN_LOCAL ); //play the beep-in effect
						s_credits.beepTime++;
					}	
				}
				else //Beta Testers
				{
					UI_DrawProportionalString(  420,  180 + yDeviation, creditsInfo[creditsIndex].devInfo[i].name,UI_LEFT|UI_SMALLFONT, colorTable[CT_DKPURPLE2]);
					//UI_DrawProportionalString(  512,  180 + yDeviation, creditsInfo[creditsIndex].devInfo[i+1].name,UI_LEFT|UI_SMALLFONT, colorTable[CT_DKPURPLE2]);
					//i++;

					//Play a beep effect
					if ( i == s_credits.beepTime -1 ) {
						trap_S_StartLocalSound(s_credits.pingSound, CHAN_LOCAL ); //play the beep-in effect
						s_credits.beepTime++;
					}	
				}

				if ( i >= 3) { //So it appears below lead d00d
					if ( creditsInfo[creditsIndex].devInfo[i+8].name[0] && creditsInfo[creditsIndex].devInfo[i+8].position[0] ) {	/*( arrayLen > 11 ) && ( ( i + 10 ) <= arrayLen )*/ 
						UI_DrawProportionalString(  524,  180 + yDeviation, creditsInfo[creditsIndex].devInfo[i+8].name,UI_LEFT|UI_SMALLFONT, colorTable[CT_DKPURPLE2]);
						UI_DrawProportionalString(  524,  198 + yDeviation, creditsInfo[creditsIndex].devInfo[i+8].position,UI_LEFT|UI_TINYFONT, colorTable[CT_DKPURPLE2]); //18 varaition
					}
				}
			}

			//Increment the data for the next iteration
			timeDeviation += 200; //each loop, delay the display time by .2 seconds (hence the text doesn't all appear at once)
			
			if ( creditsInfo[creditsIndex].devInfo[i+1].position[0] )
				if(creditsIndex == 1 || creditsIndex == 6) // make all programmers|additional people fit in
					yDeviation += 30;
				else
					yDeviation += 40; //and don't all display at the same y co-ord
			else
				yDeviation += 18;

		}
	}
	UI_LogFuncEnd();
}

/*
===============
Credits_MenuEvent
===============
*/
static void Credits_MenuEvent( void *ptr, int32_t event ) 
{
	UI_LogFuncBegin();
	if( event != QM_ACTIVATED ) 
	{
		return;
	}

	s_credits.startTime = uis.realtime;
	s_credits.beepTime = 0;

	switch ( ((menucommon_s*)ptr)->id ) 
	{
	case ID_MAINMENU:
		UI_PopMenu();
		break;		

	case ID_LEADS:
	case ID_CODERS:
	case ID_2D:
	case ID_3D:
	case ID_MAPPING:
	case ID_SOUNDS:	
	case ID_EXTRAS:
	case ID_BETAS:
	case ID_THANKS:
		s_credits.creditsNum = ((menucommon_s*)ptr)->id;
		break;
	}
	UI_LogFuncEnd();
}

/*
=================
C_MainMenu_Graphics
=================
*/
void CreditsMenu_Graphics (void)
{
	UI_LogFuncBegin();
	// Draw the basic screen layout
	UI_MenuFrame(&s_credits.menu);

	UI_DrawProportionalString(  74,  66, "69",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  84, "604369",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  188, "71",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  206, "6154",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  395, "2004",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	trap_R_SetColor( colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(30,203,  47, 123, uis.whiteShader);	// Top left column square on bottom 3rd 
	UI_DrawHandlePic(30,328,  47, 61, uis.whiteShader);	// Bottom left column square on bottom 3rd

	UI_DrawProportionalString(  74,  206, "38",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  332, "28",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	/*
	Courtesy of EF SP's Credits :P
	*/
	trap_R_SetColor( colorTable[CT_WHITE]);
	UI_DrawHandlePic(85, 169, 306, 256, spaceBackdrop);

	//Left Bracket around galaxy picture
	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(85,169, 16, 16, uis.graphicBracket1CornerLU);
	UI_DrawHandlePic(85,185,  8, 87, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(85,275,  8, 10, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_LTORANGE]);
	UI_DrawHandlePic(87,288,  6, 21, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(85,312,  8, 10, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(85,325,  8, 87, uis.whiteShader);
	UI_DrawHandlePic(85,412, 16, -16, uis.graphicBracket1CornerLU);	//LD

	//Right Bracket around galaxy picture
	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(375,169, -16, 16, uis.graphicBracket1CornerLU);	//RU
	UI_DrawHandlePic(383,185,  8, 87, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(383,275,  8, 10, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_LTORANGE]);
	UI_DrawHandlePic(383,288,  6, 21, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKBROWN1]);
	UI_DrawHandlePic(383,312,  8, 10, uis.whiteShader);

	trap_R_SetColor( colorTable[CT_DKPURPLE2]);
	UI_DrawHandlePic(383,325,  8, 87, uis.whiteShader);
	UI_DrawHandlePic(375,412, -16, -16, uis.graphicBracket1CornerLU);	//RD

		// Description frame
	trap_R_SetColor( colorTable[CT_LTBLUE1]);
	UI_DrawHandlePic( 397, 163,  -16,  -16, cornerUpper);	// Top corner
	UI_DrawHandlePic( 397, 418,  -16,   16, cornerUpper);// Bottom Left Corner

	UI_DrawHandlePic(398, 182,  12, 233, uis.whiteShader);	// Block between top & bottom corner

	UI_DrawHandlePic( 412, 169,  10,   7, uis.whiteShader);	// Top line 
	UI_DrawHandlePic( 425, 169, 187,   7, uis.whiteShader);	// Top line 

	UI_DrawHandlePic( 412, 421,  10,   7, uis.whiteShader);	// Top line 
	UI_DrawHandlePic( 425, 421, 187,   7, uis.whiteShader);	// Bottom line 

	//end

	//UI_Draw3DModel( 230, 290, 245, 245, s_credits.logoModel, m_origin, m_angles, m_mid );

	trap_R_SetColor( colorTable[s_credits.leads.color]);
	UI_DrawHandlePic(s_credits.leads.generic.x - 14, s_credits.leads.generic.y,MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
	UI_DrawHandlePic(s_credits.coders.generic.x - 14, s_credits.coders.generic.y,MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
	UI_DrawHandlePic(s_credits.twodee.generic.x - 14, s_credits.twodee.generic.y,MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
	UI_DrawHandlePic(s_credits.threedee.generic.x - 14, s_credits.threedee.generic.y,MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
	UI_DrawHandlePic(s_credits.mapping.generic.x - 14, s_credits.mapping.generic.y,MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
	UI_DrawHandlePic(s_credits.sounds.generic.x - 14, s_credits.sounds.generic.y,MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
	UI_DrawHandlePic(s_credits.extras.generic.x - 14, s_credits.extras.generic.y,MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
	UI_DrawHandlePic(s_credits.betas.generic.x - 14, s_credits.betas.generic.y,MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);
	UI_DrawHandlePic(s_credits.thanks.generic.x - 14, s_credits.thanks.generic.y,MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);

	UI_LogFuncBegin();
}

static void UI_drawLogo ( void )
{
	UI_LogFuncBegin();
	refdef_t		refdef;
	refEntity_t		ent;
	vec3_t			origin = {0.0, 0.0, 0.0};
	vec3_t			angles;
	float			adjust;
	float			x, y, w, h;
	//vec4_t			color = {0.5, 0, 0, 1};

	// setup the refdef

	memset( &refdef, 0, sizeof( refdef ) );

	refdef.rdflags = RDF_NOWORLDMODEL;

	AxisClear( refdef.viewaxis );

	x = 0;
	y = 0;
	w = 400; //640 //TiM: I don't know exactly, but w and h seem to control the size of the region the model will be drawn in
	h = 480; //120
	UI_AdjustFrom640( &x, &y, &w, &h );
	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;

	adjust = 0; // JDC: Kenneth asked me to stop this 1.0 * sin( (float)uis.realtime / 1000 );
	refdef.fov_x = 17 + adjust; //60 //TiM: Controls width scale
	refdef.fov_y = 19.6875 + adjust; //19.6875

	refdef.time = uis.realtime;

	origin[0] = 800; //300 //TiM: Controls Overall Size
	origin[1] = -22; //TiM: Controls Horizontal Position
	origin[2] = -70; //-32 //TiM: Controls vertical Position

	trap_R_ClearScene();

	// add the model

	memset( &ent, 0, sizeof(ent) );

	adjust = 20.0/300.0 * uis.realtime; //5.0 * sin( (float)uis.realtime / 5000 );
	VectorSet( angles, 0, 180 + adjust, 0 );
	AnglesToAxis( angles, ent.axis );
	ent.hModel = s_credits.logoModel;

	
	if (trap_Cvar_VariableValue("r_dynamiclight") ) {
		ent.renderfx = RF_LOWLIGHT|RF_NOSHADOW;		// keep it dark, and no stencil shadows //RF_LOWBRIGHT
	} else {
		ent.renderfx = RF_NOSHADOW;		// no stencil shadows
	}
	

	VectorCopy( origin, ent.origin );
	VectorCopy( origin, ent.lightingOrigin );
	VectorCopy( ent.origin, ent.oldorigin );

	origin[0] -= 50;	// + = behind, - = in front
	origin[1] += 50;	// + = left, - = right
	origin[2] += 50;	// + = above, - = below
	trap_R_AddLightToScene( origin, 150, 1.0, 1.0, 1.0 );

	trap_R_AddRefEntityToScene( &ent );

	trap_R_RenderScene( &refdef );

	UI_LogFuncBegin();
}

/*
=================
CreditsMenu_Draw
=================
*/
void CreditsMenu_Draw (void)
{
	UI_LogFuncBegin();

	CreditsMenu_Graphics();

	Menu_Draw( &s_credits.menu );

	UI_drawLogo();

	UI_drawCreditNames( s_credits.creditsNum-1 );

	UI_LogFuncBegin();
}

/*
=================
UI_Credits_Cache
=================
*/
void UI_CreditsMenu_Cache( void ) 
{
	UI_LogFuncBegin();
	s_credits.logoModel = trap_R_RegisterModel( MAIN_LOGO_MODEL );
	s_credits.pingSound	= trap_S_RegisterSound( LCARS_BLIP );
	cornerUpper			= trap_R_RegisterShaderNoMip( LCARS_CORNER_U );
	spaceBackdrop		= trap_R_RegisterShaderNoMip( LCARS_LOGO_BACKDROP );
	UI_LogFuncBegin();

}

/*
===============
UI_Credits_MenuInit
===============
*/
static void UI_Credits_MenuInit( void ) 
{
	UI_LogFuncBegin();
	memset( &s_credits, 0 ,sizeof(credits_t) );

	s_credits.creditsNum	= ID_LEADS;
	s_credits.beepTime		= 0;

	// Menu Data
	s_credits.menu.wrapAround					= qtrue;
	s_credits.menu.fullscreen					= qtrue;
	s_credits.menu.draw							= CreditsMenu_Draw;
	s_credits.menu.descX						= MENU_DESC_X;
	s_credits.menu.descY						= MENU_DESC_Y;	
	s_credits.menu.titleX						= MENU_TITLE_X;
	s_credits.menu.titleY						= MENU_TITLE_Y;
	s_credits.menu.titleI						= MNT_CREDITS_TITLE;
	s_credits.menu.footNoteEnum					= MNT_CREDITS;

	UI_CreditsMenu_Cache();

	// Button Data
	s_credits.mainmenu.generic.type				= MTYPE_BITMAP;      
	s_credits.mainmenu.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_credits.mainmenu.generic.x				= 482;
	s_credits.mainmenu.generic.y				= 136;
	s_credits.mainmenu.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	s_credits.mainmenu.generic.id				= ID_MAINMENU;
	s_credits.mainmenu.generic.callback			= Credits_MenuEvent;
	s_credits.mainmenu.width					= MENU_BUTTON_MED_WIDTH;
	s_credits.mainmenu.height					= MENU_BUTTON_MED_HEIGHT;
	s_credits.mainmenu.color					= CT_DKPURPLE1;
	s_credits.mainmenu.color2					= CT_LTPURPLE1;
	s_credits.mainmenu.textX					= MENU_BUTTON_TEXT_X;
	s_credits.mainmenu.textY					= MENU_BUTTON_TEXT_Y;
	s_credits.mainmenu.textEnum					= MBT_MAINMENU;
	s_credits.mainmenu.textcolor				= CT_BLACK;
	s_credits.mainmenu.textcolor2				= CT_WHITE;

	s_credits.leads.generic.type				= MTYPE_BITMAP;      
	s_credits.leads.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_credits.leads.generic.x					= cm_buttons[0][0];
	s_credits.leads.generic.y					= cm_buttons[0][1];
	s_credits.leads.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	s_credits.leads.generic.id					= ID_LEADS;
	s_credits.leads.generic.callback			= Credits_MenuEvent; 
	s_credits.leads.width						= MENU_BUTTON_MED_WIDTH;
	s_credits.leads.height						= MENU_BUTTON_MED_HEIGHT;
	s_credits.leads.color						= CT_DKPURPLE1;
	s_credits.leads.color2						= CT_LTPURPLE1;
	s_credits.leads.textX						= MENU_BUTTON_TEXT_X;
	s_credits.leads.textY						= MENU_BUTTON_TEXT_Y;
	s_credits.leads.textEnum					= MBT_LEADS;
	s_credits.leads.textcolor					= CT_BLACK;
	s_credits.leads.textcolor2					= CT_WHITE;

	s_credits.coders.generic.type				= MTYPE_BITMAP;      
	s_credits.coders.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_credits.coders.generic.x					= cm_buttons[1][0];
	s_credits.coders.generic.y					= cm_buttons[1][1];
	s_credits.coders.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	s_credits.coders.generic.id					= ID_CODERS;
	s_credits.coders.generic.callback			= Credits_MenuEvent; 
	s_credits.coders.width						= MENU_BUTTON_MED_WIDTH;
	s_credits.coders.height						= MENU_BUTTON_MED_HEIGHT;
	s_credits.coders.color						= CT_DKPURPLE1;
	s_credits.coders.color2						= CT_LTPURPLE1;
	s_credits.coders.textX						= MENU_BUTTON_TEXT_X;
	s_credits.coders.textY						= MENU_BUTTON_TEXT_Y;
	s_credits.coders.textEnum					= MBT_CODERS;
	s_credits.coders.textcolor					= CT_BLACK;
	s_credits.coders.textcolor2					= CT_WHITE;

	s_credits.twodee.generic.type				= MTYPE_BITMAP;      
	s_credits.twodee.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_credits.twodee.generic.x					= cm_buttons[2][0];
	s_credits.twodee.generic.y					= cm_buttons[2][1];
	s_credits.twodee.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	s_credits.twodee.generic.id					= ID_2D;
	s_credits.twodee.generic.callback			= Credits_MenuEvent; 
	s_credits.twodee.width						= MENU_BUTTON_MED_WIDTH;
	s_credits.twodee.height						= MENU_BUTTON_MED_HEIGHT;
	s_credits.twodee.color						= CT_DKPURPLE1;
	s_credits.twodee.color2						= CT_LTPURPLE1;
	s_credits.twodee.textX						= MENU_BUTTON_TEXT_X;
	s_credits.twodee.textY						= MENU_BUTTON_TEXT_Y;
	s_credits.twodee.textEnum					= MBT_2D;
	s_credits.twodee.textcolor					= CT_BLACK;
	s_credits.twodee.textcolor2					= CT_WHITE;

	s_credits.threedee.generic.type				= MTYPE_BITMAP;      
	s_credits.threedee.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_credits.threedee.generic.x				= cm_buttons[3][0];
	s_credits.threedee.generic.y				= cm_buttons[3][1];
	s_credits.threedee.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	s_credits.threedee.generic.id				= ID_3D;
	s_credits.threedee.generic.callback			= Credits_MenuEvent; 
	s_credits.threedee.width					= MENU_BUTTON_MED_WIDTH;
	s_credits.threedee.height					= MENU_BUTTON_MED_HEIGHT;
	s_credits.threedee.color					= CT_DKPURPLE1;
	s_credits.threedee.color2					= CT_LTPURPLE1;
	s_credits.threedee.textX					= MENU_BUTTON_TEXT_X;
	s_credits.threedee.textY					= MENU_BUTTON_TEXT_Y;
	s_credits.threedee.textEnum					= MBT_3D;
	s_credits.threedee.textcolor				= CT_BLACK;
	s_credits.threedee.textcolor2				= CT_WHITE;

	s_credits.mapping.generic.type				= MTYPE_BITMAP;      
	s_credits.mapping.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_credits.mapping.generic.x					= cm_buttons[4][0];
	s_credits.mapping.generic.y					= cm_buttons[4][1];
	s_credits.mapping.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	s_credits.mapping.generic.id				= ID_MAPPING;
	s_credits.mapping.generic.callback			= Credits_MenuEvent; 
	s_credits.mapping.width						= MENU_BUTTON_MED_WIDTH;
	s_credits.mapping.height					= MENU_BUTTON_MED_HEIGHT;
	s_credits.mapping.color						= CT_DKPURPLE1;
	s_credits.mapping.color2					= CT_LTPURPLE1;
	s_credits.mapping.textX						= MENU_BUTTON_TEXT_X;
	s_credits.mapping.textY						= MENU_BUTTON_TEXT_Y;
	s_credits.mapping.textEnum					= MBT_MAPPING;
	s_credits.mapping.textcolor					= CT_BLACK;
	s_credits.mapping.textcolor2				= CT_WHITE;

	s_credits.sounds.generic.type				= MTYPE_BITMAP;      
	s_credits.sounds.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_credits.sounds.generic.x					= cm_buttons[5][0];
	s_credits.sounds.generic.y					= cm_buttons[5][1];
	s_credits.sounds.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	s_credits.sounds.generic.id					= ID_SOUNDS;
	s_credits.sounds.generic.callback			= Credits_MenuEvent; 
	s_credits.sounds.width						= MENU_BUTTON_MED_WIDTH;
	s_credits.sounds.height						= MENU_BUTTON_MED_HEIGHT;
	s_credits.sounds.color						= CT_DKPURPLE1;
	s_credits.sounds.color2						= CT_LTPURPLE1;
	s_credits.sounds.textX						= MENU_BUTTON_TEXT_X;
	s_credits.sounds.textY						= MENU_BUTTON_TEXT_Y;
	s_credits.sounds.textEnum					= MBT_SOUNDS;
	s_credits.sounds.textcolor					= CT_BLACK;
	s_credits.sounds.textcolor2					= CT_WHITE;

	s_credits.extras.generic.type				= MTYPE_BITMAP;      
	s_credits.extras.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_credits.extras.generic.x					= cm_buttons[6][0];
	s_credits.extras.generic.y					= cm_buttons[6][1];
	s_credits.extras.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	s_credits.extras.generic.id					= ID_EXTRAS;
	s_credits.extras.generic.callback			= Credits_MenuEvent; 
	s_credits.extras.width						= MENU_BUTTON_MED_WIDTH;
	s_credits.extras.height						= MENU_BUTTON_MED_HEIGHT;
	s_credits.extras.color						= CT_DKPURPLE1;
	s_credits.extras.color2						= CT_LTPURPLE1;
	s_credits.extras.textX						= MENU_BUTTON_TEXT_X;
	s_credits.extras.textY						= MENU_BUTTON_TEXT_Y;
	s_credits.extras.textEnum					= MBT_EXTRAS;
	s_credits.extras.textcolor					= CT_BLACK;
	s_credits.extras.textcolor2					= CT_WHITE;

	s_credits.betas.generic.type				= MTYPE_BITMAP;      
	s_credits.betas.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_credits.betas.generic.x					= cm_buttons[7][0];
	s_credits.betas.generic.y					= cm_buttons[7][1];
	s_credits.betas.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	s_credits.betas.generic.id					= ID_BETAS;
	s_credits.betas.generic.callback			= Credits_MenuEvent; 
	s_credits.betas.width						= MENU_BUTTON_MED_WIDTH;
	s_credits.betas.height						= MENU_BUTTON_MED_HEIGHT;
	s_credits.betas.color						= CT_DKPURPLE1;
	s_credits.betas.color2						= CT_LTPURPLE1;
	s_credits.betas.textX						= MENU_BUTTON_TEXT_X;
	s_credits.betas.textY						= MENU_BUTTON_TEXT_Y;
	s_credits.betas.textEnum					= MBT_BETAS;
	s_credits.betas.textcolor					= CT_BLACK;
	s_credits.betas.textcolor2					= CT_WHITE;

	s_credits.thanks.generic.type				= MTYPE_BITMAP;      
	s_credits.thanks.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_credits.thanks.generic.x					= cm_buttons[8][0];
	s_credits.thanks.generic.y					= cm_buttons[8][1];
	s_credits.thanks.generic.name				= BUTTON_GRAPHIC_LONGRIGHT;
	s_credits.thanks.generic.id					= ID_THANKS;
	s_credits.thanks.generic.callback			= Credits_MenuEvent; 
	s_credits.thanks.width						= MENU_BUTTON_MED_WIDTH;
	s_credits.thanks.height						= MENU_BUTTON_MED_HEIGHT;
	s_credits.thanks.color						= CT_DKPURPLE1;
	s_credits.thanks.color2						= CT_LTPURPLE1;
	s_credits.thanks.textX						= MENU_BUTTON_TEXT_X;
	s_credits.thanks.textY						= MENU_BUTTON_TEXT_Y;
	s_credits.thanks.textEnum					= MBT_THANKS;
	s_credits.thanks.textcolor					= CT_BLACK;
	s_credits.thanks.textcolor2					= CT_WHITE;

	Menu_AddItem( &s_credits.menu, &s_credits.mainmenu );
	Menu_AddItem( &s_credits.menu, &s_credits.leads );
	Menu_AddItem( &s_credits.menu, &s_credits.coders );
	Menu_AddItem( &s_credits.menu, &s_credits.twodee );
	Menu_AddItem( &s_credits.menu, &s_credits.threedee );
	Menu_AddItem( &s_credits.menu, &s_credits.mapping );
	Menu_AddItem( &s_credits.menu, &s_credits.sounds );
	Menu_AddItem( &s_credits.menu, &s_credits.extras );
	Menu_AddItem( &s_credits.menu, &s_credits.betas );
	Menu_AddItem( &s_credits.menu, &s_credits.thanks );
	UI_LogFuncBegin();

}

/*
===============
UI_CreditsMenu
===============
*/
void UI_CreditsMenu( void ) 
{
	UI_LogFuncBegin();
	UI_Credits_MenuInit();
	UI_PushMenu( &s_credits.menu );
	UI_LogFuncBegin();

}
