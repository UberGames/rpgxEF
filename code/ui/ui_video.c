// Copyright (C) 1999-2000 Id Software, Inc.
//
#include "ui_local.h"

void UI_VideoDriverMenu( void );
void VideoDriver_Lines(int increment);
void UI_VideoData2SettingsMenu( void );


extern void *holdControlPtr;
extern int holdControlEvent;
static void Video_MenuEvent (void* ptr, int event);

#define PIC_MONBAR2		"menu/common/monbar_2.tga"
#define PIC_SLIDER		"menu/common/slider.tga"

// Video Data
typedef struct 
{
	menuframework_s	menu;

	qhandle_t	swooshTop;
	qhandle_t	swooshBottom;
	qhandle_t	swooshTopSmall;
	qhandle_t	swooshBottomSmall;

} videoddata_t;

static videoddata_t	s_videodata;


// Video Drivers
typedef struct 
{
	menuframework_s	menu;

	qhandle_t	swooshTopSmall;
	qhandle_t	swooshBottomSmall;
} videodriver_t;


// Video Data 2
typedef struct 
{
	menuframework_s	menu;

	menuslider_s	gamma_slider;
	menuslider_s	screensize_slider;
	menulist_s		anisotropicfiltering;
	menuaction_s	apply_action2;

	qhandle_t	gamma;
	qhandle_t	top;
} videodata2_t;

typedef struct
{
	int			width;
	int			height;
} videoResolutions_t;

static videodata2_t	s_videodata2;

static int s_graphics_options_Names[] =
{
	MNT_VIDEO_HIGH_QUALITY,
	MNT_VIDEO_NORMAL,
	MNT_VIDEO_FAST,
	MNT_VIDEO_FASTEST,
	MNT_VIDEO_CUSTOM,
	MNT_NONE
};

/*static int s_driver_Names[] =
{
	MNT_VIDEO_DRIVER_DEFAULT,
	MNT_VIDEO_DRIVER_VOODOO,
	MNT_NONE
};*/

extern int s_OffOnNone_Names[];

static int s_resolutions[] = 
{
//	MNT_320X200,
//	MNT_400X300,
	MNT_512X384,
	MNT_640X480,
	MNT_800X600,
	MNT_960X720,
	MNT_1024X768,
	MNT_1152X864,
	MNT_1280X960,
	MNT_1600X1200,
	MNT_2048X1536,
	//MNT_856x480WIDE, //this is dumb :P
	MNT_NONE
};

static int s_aspectRatios[] =
{
	MNT_4X3,
	MNT_16X9,
	MNT_16X10,
	0
};

static char *s_wideResolutions16x9[] =
{
	"854x480",
	"1280x720",
	"1920x1080",
	0
};

static videoResolutions_t videoResolutions16x9[] =
{
	{ 854, 480 },
	{ 1280, 720 },
	{ 1920, 1080 }
};

static char *s_wideResolutions16x10[] =
{
	"1280x800",
	"1440x900",
	"1680x1050",
	"1920x1200",
	"2560x1600",
	0
};

static videoResolutions_t videoResolutions16x10[] =
{
	{ 1280, 800 },
	{ 1440, 900 },
	{ 1680, 1050 },
	{ 1920, 1200 },
	{ 2560, 1600 }
};

static void *s_widescreenResolutions[] = 
{
	&videoResolutions16x9,
	&videoResolutions16x10,
	NULL
};

static void *s_widescreenResStrings[] =
{
	&s_wideResolutions16x9,
	&s_wideResolutions16x10,
	NULL
};

//store the number of widescreen arrays
static int s_wideScreenSets = 2; 

//store the number of resolutions in each array.
//This will be necessary when we change the lists over
static int s_resolutionNums[] = {9,3,5};

//finally for reference sake, use an enum
typedef enum
{
	ASPECTRATIO_4X3,
	ASPECTRATIO_16X9,
	ASPECTRATIO_16X10,
	ASPECTRATIO_MAX
} AspectRatios_e;

static int s_colordepth_Names[] =
{
	MNT_DEFAULT,
	MNT_16BIT,
	MNT_32BIT,
	MNT_NONE
};

/*static int s_lighting_Names[] =
{
	MNT_LIGHTMAP,
	MNT_VERTEX,
	MNT_NONE
};*/

static int s_quality_Names[] =
{
	MNT_LOW,
	MNT_MEDIUM,
	MNT_HIGH,
	MNT_NONE
};

static int s_4quality_Names[] =
{
	MNT_LOW,
	MNT_MEDIUM,
	MNT_HIGH,
	MNT_VERY_HIGH,
	MNT_NONE
};

static int s_tqbits_Names[] =
{
	MNT_DEFAULT,
	MNT_16BIT,
	MNT_32BIT,
	MNT_NONE
};

static int s_filter_Names[] =
{
	MNT_BILINEAR,
	MNT_TRILINEAR,
	MNT_NONE
};



static menubitmap_s			s_video_drivers;
static menubitmap_s			s_video_data;
static menubitmap_s			s_video_data2;

#define ID_MAINMENU		100
#define ID_CONTROLS		101
#define ID_VIDEO		102
#define ID_SOUND		103
#define ID_GAMEOPTIONS	104
#define ID_CDKEY		105
#define ID_VIDEODATA	110
#define ID_VIDEODATA2	111
#define ID_VIDEODRIVERS	112
#define ID_ARROWDWN		113
#define ID_ARROWUP		114
#define ID_INGAMEMENU	115

// Precache stuff for Video Driver
#define MAX_VID_DRIVERS 128
static struct 
{
	menuframework_s		menu;

	char *drivers[MAX_VID_DRIVERS];
	char extensionsString[2*MAX_STRING_CHARS];

	menutext_s		line1;
	menutext_s		line2;
	menutext_s		line3;
	menutext_s		line4;
	menutext_s		line5;
	menutext_s		line6;
	menutext_s		line7;
	menutext_s		line8;
	menutext_s		line9;
	menutext_s		line10;
	menutext_s		line11;
	menutext_s		line12;
	menutext_s		line13;
	menutext_s		line14;
	menutext_s		line15;
	menutext_s		line16;
	menutext_s		line17;
	menutext_s		line18;
	menutext_s		line19;
	menutext_s		line20;
	menutext_s		line21;
	menutext_s		line22;
	menutext_s		line23;
	menutext_s		line24;

	qhandle_t	corner_ll_8_16;
	qhandle_t	corner_ll_16_16;
	qhandle_t	arrow_dn;
	menubitmap_s	arrowdwn;
	menubitmap_s	arrowup;
	int			currentDriverLine;
	int			driverCnt;

	int			activeArrowDwn;
	int			activeArrowUp;
} s_videodriver;


static void* g_videolines[] =
{
	&s_videodriver.line1, 
	&s_videodriver.line2, 
	&s_videodriver.line3, 
	&s_videodriver.line4, 
	&s_videodriver.line5, 
	&s_videodriver.line6, 
	&s_videodriver.line7, 
	&s_videodriver.line8, 
	&s_videodriver.line9, 
	&s_videodriver.line10, 
	&s_videodriver.line11, 
	&s_videodriver.line12, 
	&s_videodriver.line13, 
	&s_videodriver.line14, 
	&s_videodriver.line15, 
	&s_videodriver.line16, 
	&s_videodriver.line17, 
	&s_videodriver.line18, 
	&s_videodriver.line19, 
	&s_videodriver.line20, 
	&s_videodriver.line21, 
	&s_videodriver.line22, 
	&s_videodriver.line23, 
	&s_videodriver.line24, 
	NULL,
};

int video_sidebuttons[3][2] = 
{
	{ 30, 250													},	// Video Data Button
	{ 30, 250 + 6 + (MENU_BUTTON_MED_HEIGHT * 1.5)				},	// Video Drivers Button
	{ 30, 250 + (2 * (6 + (MENU_BUTTON_MED_HEIGHT * 1.5)))		},	// Video Drivers Button
};


void Video_SideButtons(menuframework_s *menu,int menuType);
static void GraphicsOptions_ApplyChanges( void *unused, int notification );

/*
=======================================================================

DRIVER INFORMATION MENU

=======================================================================
*/



#define ID_DRIVERINFOBACK	100

typedef struct
{
	menuframework_s	menu;
	menutext_s		banner;
	menubitmap_s	back;
	menubitmap_s	framel;
	menubitmap_s	framer;
	char			stringbuff[2*MAX_STRING_CHARS];
	char*			strings[64];
	int				numstrings;
} driverinfo_t;

static driverinfo_t	s_driverinfo;


/*
=================
DriverInfo_MenuDraw
=================
*/
static void DriverInfo_MenuDraw( void )
{
	int	i;
	int	y;

	Menu_Draw( &s_driverinfo.menu );

	UI_DrawString( 320, 80, "VENDOR", UI_CENTER|UI_SMALLFONT, color_red, qtrue );
	UI_DrawString( 320, 152, "PIXELFORMAT", UI_CENTER|UI_SMALLFONT, color_red, qtrue );
	UI_DrawString( 320, 192, "EXTENSIONS", UI_CENTER|UI_SMALLFONT, color_red, qtrue );

	UI_DrawString( 320, 80+16, uis.glconfig.vendor_string, UI_CENTER|UI_SMALLFONT, text_color_normal, qtrue );
	UI_DrawString( 320, 96+16, uis.glconfig.version_string, UI_CENTER|UI_SMALLFONT, text_color_normal, qtrue );
	UI_DrawString( 320, 112+16, uis.glconfig.renderer_string, UI_CENTER|UI_SMALLFONT, text_color_normal, qtrue );
	UI_DrawString( 320, 152+16, va ("color(%d-bits) Z(%d-bits) stencil(%d-bits)", uis.glconfig.colorBits, uis.glconfig.depthBits, uis.glconfig.stencilBits), UI_CENTER|UI_SMALLFONT, text_color_normal, qtrue );

	// double column
	y = 192+16;
	for (i=0; i<s_driverinfo.numstrings/2; i++) {
		UI_DrawString( 320-4, y, s_driverinfo.strings[i*2], UI_RIGHT|UI_SMALLFONT, text_color_normal, qtrue );
		UI_DrawString( 320+4, y, s_driverinfo.strings[i*2+1], UI_LEFT|UI_SMALLFONT, text_color_normal, qtrue );
		y += SMALLCHAR_HEIGHT;
	}

	if (s_driverinfo.numstrings & 1)
		UI_DrawString( 320, y, s_driverinfo.strings[s_driverinfo.numstrings-1], UI_CENTER|UI_SMALLFONT, text_color_normal, qtrue );
}

/*
=================
DriverInfo_Cache
=================
*/
void DriverInfo_Cache( void )
{

}

/*
=================
UI_DriverInfo_Menu
=================
*/
static void UI_DriverInfo_Menu( void )
{
	char*	eptr;
	int		i;
	int		len;

	// zero set all our globals
	memset( &s_driverinfo, 0 ,sizeof(driverinfo_t) );

	DriverInfo_Cache();

	s_driverinfo.menu.fullscreen = qtrue;
	s_driverinfo.menu.draw       = DriverInfo_MenuDraw;
/*
	s_driverinfo.banner.generic.type  = MTYPE_BTEXT;
	s_driverinfo.banner.generic.x	  = 320;
	s_driverinfo.banner.generic.y	  = 16;
	s_driverinfo.banner.string		  = "DRIVER INFO";
	s_driverinfo.banner.color	      = color_white;
	s_driverinfo.banner.style	      = UI_CENTER;

	s_driverinfo.framel.generic.type  = MTYPE_BITMAP;
	s_driverinfo.framel.generic.name  = DRIVERINFO_FRAMEL;
	s_driverinfo.framel.generic.flags = QMF_INACTIVE;
	s_driverinfo.framel.generic.x	  = 0;
	s_driverinfo.framel.generic.y	  = 78;
	s_driverinfo.framel.width  	      = 256;
	s_driverinfo.framel.height  	  = 329;

	s_driverinfo.framer.generic.type  = MTYPE_BITMAP;
	s_driverinfo.framer.generic.name  = DRIVERINFO_FRAMER;
	s_driverinfo.framer.generic.flags = QMF_INACTIVE;
	s_driverinfo.framer.generic.x	  = 376;
	s_driverinfo.framer.generic.y	  = 76;
	s_driverinfo.framer.width  	      = 256;
	s_driverinfo.framer.height  	  = 334;

	s_driverinfo.back.generic.type	   = MTYPE_BITMAP;
	s_driverinfo.back.generic.name     = DRIVERINFO_BACK0;
	s_driverinfo.back.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_driverinfo.back.generic.callback = DriverInfo_Event;
	s_driverinfo.back.generic.id	   = ID_DRIVERINFOBACK;
	s_driverinfo.back.generic.x		   = 0;
	s_driverinfo.back.generic.y		   = 480-64;
	s_driverinfo.back.width  		   = 128;
	s_driverinfo.back.height  		   = 64;
	s_driverinfo.back.focuspic         = DRIVERINFO_BACK1;
*/
	strcpy( s_driverinfo.stringbuff, uis.glconfig.extensions_string );

	// build null terminated extension strings
	eptr = s_driverinfo.stringbuff;
	while ( s_driverinfo.numstrings<40 && *eptr )
	{
		while ( *eptr && *eptr == ' ' )
			*eptr++ = '\0';

		// track start of valid string
		if (*eptr && *eptr != ' ')
			s_driverinfo.strings[s_driverinfo.numstrings++] = eptr;

		while ( *eptr && *eptr != ' ' )
			eptr++;
	}

	// safety length strings for display
	for (i=0; i<s_driverinfo.numstrings; i++) {
		len = strlen(s_driverinfo.strings[i]);
		if (len > 32) {
			s_driverinfo.strings[i][len-1] = '>';
			s_driverinfo.strings[i][len]   = '\0';
		}
	}

	Menu_AddItem( &s_driverinfo.menu, &s_driverinfo.banner );
	Menu_AddItem( &s_driverinfo.menu, &s_driverinfo.framel );
	Menu_AddItem( &s_driverinfo.menu, &s_driverinfo.framer );
	Menu_AddItem( &s_driverinfo.menu, &s_driverinfo.back );

	UI_PushMenu( &s_driverinfo.menu );
}

/*
=======================================================================

GRAPHICS OPTIONS MENU

=======================================================================
*/


/*static const char *s_drivers[] =
{
	OPENGL_DRIVER_NAME,
	_3DFX_DRIVER_NAME,
	0
};*/

#define ID_BACK2		101
#define ID_FULLSCREEN	102
#define ID_LIST			103
#define ID_MODE			104
#define ID_DRIVERINFO	105
#define ID_GRAPHICS		106
#define ID_DISPLAY		107
//#define ID_SOUND		108
#define ID_NETWORK		109

typedef struct {
	menuframework_s	menu;

	menutext_s		banner;
	menubitmap_s	framel;
	menubitmap_s	framer;

	menutext_s		graphics;
	menutext_s		display;
	menutext_s		sound;
	menutext_s		network;
	
	menulist_s		aspectRatio;
	menulist_s		list;
	menulist_s		mode;
	menulist_s		tq;
	menulist_s  	fs;
	menulist_s  	lighting;
	menulist_s  	allow_extensions;
	menulist_s  	texturebits;
	menulist_s  	colordepth;
	menulist_s  	geometry;
	menulist_s  	filter;
	menutext_s		driverinfo;
	menulist_s		simpleshaders;
	menulist_s		compresstextures;

	menuaction_s	apply;

	menubitmap_s	back;

	//TiM - only update the resolution list when need be
	int				lastRatio;

} graphicsoptions_t;

typedef struct
{
	int mode;
	qboolean fullscreen;
	int	aspectRatio;
	int tq;
	int lighting;
	int colordepth;
	int texturebits;
	int geometry;
	int filter;
	qboolean extensions;
	int simpleshaders;
	int compresstextures;
} InitialVideoOptions_s;

static InitialVideoOptions_s	s_ivo;
static graphicsoptions_t		s_graphicsoptions;	

static InitialVideoOptions_s s_ivo_templates[] =
{
	{
		2, qtrue, 0, 0, 2, 2, 2, 1, 0, qtrue, 0, 0,	// JDC: this was tq 3
	},
	{
		1, qtrue, 0, 0, 0, 0, 2, 0, 0, qtrue, 0, 0,
	},
	{
		0, qtrue, 0, 0, 1, 0, 0, 0, 0, qtrue, 0, 0,
	},
	{
		0, qtrue, 0, 1, 1, 0, 0, 0, 0, qtrue, 1, 0,
	},
	{
		1, qtrue, 0, 0, 0, 0, 1, 0, 0, qtrue, 0, 0,
	}
};

#define NUM_IVO_TEMPLATES ( sizeof( s_ivo_templates ) / sizeof( s_ivo_templates[0] ) )

/*
=================
GraphicsOptions_GetInitialVideo
=================
*/
static void GraphicsOptions_GetInitialVideo( void )
{
	s_ivo.colordepth  = s_graphicsoptions.colordepth.curvalue;
	s_ivo.mode        = s_graphicsoptions.mode.curvalue;
	s_ivo.aspectRatio = s_graphicsoptions.aspectRatio.curvalue;
	s_ivo.fullscreen  = s_graphicsoptions.fs.curvalue;
	s_ivo.extensions  = s_graphicsoptions.allow_extensions.curvalue;
	s_ivo.tq          = s_graphicsoptions.tq.curvalue;
//	s_ivo.lighting    = s_graphicsoptions.lighting.curvalue;
	s_ivo.geometry    = s_graphicsoptions.geometry.curvalue;
	s_ivo.filter      = s_graphicsoptions.filter.curvalue;
	s_ivo.texturebits = s_graphicsoptions.texturebits.curvalue;
	s_ivo.simpleshaders = s_graphicsoptions.simpleshaders.curvalue;
	s_ivo.compresstextures = s_graphicsoptions.compresstextures.curvalue;
}


/*
=================
GraphicsOptions_CheckConfig
=================
*/
static void GraphicsOptions_CheckConfig( void )
{
	int i;

	for ( i = 0; i < NUM_IVO_TEMPLATES; i++ )
	{
		if ( s_ivo_templates[i].colordepth != s_graphicsoptions.colordepth.curvalue )
			continue;
		if ( s_ivo_templates[i].mode != s_graphicsoptions.mode.curvalue )
			continue;
		if ( s_ivo_templates[i].aspectRatio != s_graphicsoptions.aspectRatio.curvalue )
			continue;
		if ( s_ivo_templates[i].fullscreen != s_graphicsoptions.fs.curvalue )
			continue;
		if ( s_ivo_templates[i].tq != s_graphicsoptions.tq.curvalue )
			continue;
//		if ( s_ivo_templates[i].lighting != s_graphicsoptions.lighting.curvalue )
//			continue;
		if ( s_ivo_templates[i].geometry != s_graphicsoptions.geometry.curvalue )
			continue;
		if ( s_ivo_templates[i].filter != s_graphicsoptions.filter.curvalue )
			continue;
		if ( s_ivo_templates[i].simpleshaders != s_graphicsoptions.simpleshaders.curvalue )
			continue;
//		if ( s_ivo_templates[i].compresstextures != s_graphicsoptions.compresstextures.curvalue )
//			continue;

//		if ( s_ivo_templates[i].texturebits != s_graphicsoptions.texturebits.curvalue )
//			continue;
		s_graphicsoptions.list.curvalue = i;
		return;
	}
	s_graphicsoptions.list.curvalue = 4;
}

/*
=================
GraphicsOptions_UpdateMenuItems
=================
*/
static void GraphicsOptions_UpdateMenuItems( void )
{

	if ( s_graphicsoptions.fs.curvalue == 0 )
	{
		s_graphicsoptions.colordepth.curvalue = 0;
		s_graphicsoptions.colordepth.generic.flags |= QMF_GRAYED;
	}
	else
	{
		s_graphicsoptions.colordepth.generic.flags &= ~QMF_GRAYED;
	}

	if ( s_graphicsoptions.allow_extensions.curvalue == 0 )
	{
		if ( s_graphicsoptions.texturebits.curvalue == 0 )
		{
			s_graphicsoptions.texturebits.curvalue = 1;
		}
	}

	s_graphicsoptions.apply.generic.flags	|= QMF_GRAYED;
	s_graphicsoptions.apply.generic.flags&= ~ QMF_BLINK;

	if ( s_ivo.mode != s_graphicsoptions.mode.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~QMF_GRAYED;
		s_graphicsoptions.apply.generic.flags |= QMF_BLINK;
	}

	if ( s_ivo.aspectRatio != s_graphicsoptions.aspectRatio.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~QMF_GRAYED;
		s_graphicsoptions.apply.generic.flags |= QMF_BLINK;
	}

	if ( s_ivo.fullscreen != s_graphicsoptions.fs.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~QMF_GRAYED;
		s_graphicsoptions.apply.generic.flags |= QMF_BLINK;
	}

	if ( s_ivo.extensions != s_graphicsoptions.allow_extensions.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~QMF_GRAYED;
		s_graphicsoptions.apply.generic.flags |= QMF_BLINK;
	}
	if ( s_ivo.tq != s_graphicsoptions.tq.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~QMF_GRAYED;
		s_graphicsoptions.apply.generic.flags |= QMF_BLINK;
	}

/*	if ( s_ivo.lighting != s_graphicsoptions.lighting.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~QMF_GRAYED;
		s_graphicsoptions.apply.generic.flags |= QMF_BLINK;
	}
*/
	if ( s_ivo.colordepth != s_graphicsoptions.colordepth.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~QMF_GRAYED;
		s_graphicsoptions.apply.generic.flags |= QMF_BLINK;
	}

	if ( s_ivo.texturebits != s_graphicsoptions.texturebits.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~QMF_GRAYED;
		s_graphicsoptions.apply.generic.flags |= QMF_BLINK;
	}

	if ( s_ivo.simpleshaders != s_graphicsoptions.simpleshaders.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~QMF_GRAYED;
		s_graphicsoptions.apply.generic.flags |= QMF_BLINK;
	}

	if ( s_ivo.compresstextures != s_graphicsoptions.compresstextures.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~QMF_GRAYED;
		s_graphicsoptions.apply.generic.flags |= QMF_BLINK;
	}

	if ( s_ivo.geometry != s_graphicsoptions.geometry.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~QMF_GRAYED;
		s_graphicsoptions.apply.generic.flags |= QMF_BLINK;
	}

	if ( s_ivo.filter != s_graphicsoptions.filter.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~QMF_GRAYED;
		s_graphicsoptions.apply.generic.flags |= QMF_BLINK;
	}

	//TiM - check for widescreen
	if ( s_graphicsoptions.aspectRatio.curvalue != s_graphicsoptions.lastRatio )
	{
		if ( s_graphicsoptions.aspectRatio.curvalue == ASPECTRATIO_4X3 )
		{
			s_graphicsoptions.mode.itemnames = NULL;
			s_graphicsoptions.mode.listnames = NULL;
			s_graphicsoptions.mode.listnames = s_resolutions;
			s_graphicsoptions.mode.numitems = s_resolutionNums[ASPECTRATIO_4X3];
			s_graphicsoptions.mode.curvalue = 1;
		}
		else
		{
			s_graphicsoptions.mode.listnames = NULL;
			s_graphicsoptions.mode.itemnames = NULL;
			s_graphicsoptions.mode.itemnames = (const char **)s_widescreenResStrings[s_graphicsoptions.aspectRatio.curvalue-1];
			s_graphicsoptions.mode.numitems = s_resolutionNums[s_graphicsoptions.aspectRatio.curvalue];
			s_graphicsoptions.mode.curvalue = 0;
		}

		s_graphicsoptions.lastRatio = s_graphicsoptions.aspectRatio.curvalue;
	}

	GraphicsOptions_CheckConfig();
}	

/*
=================
ApplyChanges - Apply the changes from the video data screen
=================
*/
static void ApplyChanges2( void *unused, int notification )
{
	if (notification != QM_ACTIVATED)
		return;

	trap_Cmd_ExecuteText( EXEC_APPEND, "vid_restart\n" );
}
/*
=================
GraphicsOptions_ApplyChanges
=================
*/
static void GraphicsOptions_ApplyChanges( void *unused, int notification )
{
	if (notification != QM_ACTIVATED)
		return;

	switch ( s_graphicsoptions.texturebits.curvalue  )
	{
	case 0:
		trap_Cvar_SetValue( "r_texturebits", 0 );
		break;
	case 1:
		trap_Cvar_SetValue( "r_texturebits", 16 );
		break;
	case 2:
		trap_Cvar_SetValue( "r_texturebits", 32 );
		break;
	}
	trap_Cvar_SetValue( "r_picmip", 3 - s_graphicsoptions.tq.curvalue );
	trap_Cvar_SetValue( "r_allowExtensions", s_graphicsoptions.allow_extensions.curvalue );
	trap_Cvar_SetValue( "r_fullscreen", s_graphicsoptions.fs.curvalue );
	//trap_Cvar_Set( "r_glDriver", ( char * ) s_drivers[s_graphicsoptions.driver.curvalue] );

	//TiM - handle the mode coupled with highscreen
	if ( s_graphicsoptions.aspectRatio.curvalue == ASPECTRATIO_4X3 )
		trap_Cvar_SetValue( "r_mode", (s_graphicsoptions.mode.curvalue + 2) );
	else
	{
		//GSIO - TiM you actually forget that there is r_customPixelAspect/r_customaspect as well
		if(s_graphicsoptions.aspectRatio.curvalue == ASPECTRATIO_16X9) {
			trap_Cvar_SetValue("r_customPixelAspect", 1.7); //vEF
			trap_Cvar_SetValue("r_customaspect", 1.7); //ioEF
		} else {
			trap_Cvar_SetValue("r_customPixelAspect", 1.6);
			trap_Cvar_SetValue("r_customaspect", 1.6);
		}
		trap_Cvar_SetValue( "r_mode", -1 );
		trap_Cvar_SetValue( "r_customWidth",	(((videoResolutions_t *)s_widescreenResolutions[s_graphicsoptions.aspectRatio.curvalue-1])[s_graphicsoptions.mode.curvalue]).width );
		trap_Cvar_SetValue( "r_customHeight",	(((videoResolutions_t *)s_widescreenResolutions[s_graphicsoptions.aspectRatio.curvalue-1])[s_graphicsoptions.mode.curvalue]).height );
	}

	trap_Cvar_SetValue( "r_lowEndVideo", s_graphicsoptions.simpleshaders.curvalue );
	
	trap_Cvar_SetValue( "r_ext_compress_textures", s_graphicsoptions.compresstextures.curvalue );
	
	switch ( s_graphicsoptions.colordepth.curvalue )
	{
	case 0:
		trap_Cvar_SetValue( "r_colorbits", 0 );
		trap_Cvar_SetValue( "r_depthbits", 0 );
		trap_Cvar_SetValue( "r_stencilbits", 0 );
		break;
	case 1:
		trap_Cvar_SetValue( "r_colorbits", 16 );
		trap_Cvar_SetValue( "r_depthbits", 16 );
		trap_Cvar_SetValue( "r_stencilbits", 0 );
		break;
	case 2:
		trap_Cvar_SetValue( "r_colorbits", 32 );
		trap_Cvar_SetValue( "r_depthbits", 24 );
		break;
	}
//	trap_Cvar_SetValue( "r_vertexLight", s_graphicsoptions.lighting.curvalue );

	if ( s_graphicsoptions.geometry.curvalue == 2 )
	{
		trap_Cvar_SetValue( "r_lodBias", 0 );
		trap_Cvar_SetValue( "r_subdivisions", 4 );
	}
	else if ( s_graphicsoptions.geometry.curvalue == 1 )
	{
		trap_Cvar_SetValue( "r_lodBias", 1 );
		trap_Cvar_SetValue( "r_subdivisions", 12 );
	}
	else
	{
		trap_Cvar_SetValue( "r_lodBias", 1 );
		trap_Cvar_SetValue( "r_subdivisions", 20 );
	}

	if ( s_graphicsoptions.filter.curvalue )
	{
		trap_Cvar_Set( "r_textureMode", "GL_LINEAR_MIPMAP_LINEAR" );
	}
	else
	{
		trap_Cvar_Set( "r_textureMode", "GL_LINEAR_MIPMAP_NEAREST" );
	}

	trap_Cmd_ExecuteText( EXEC_APPEND, "vid_restart\n" );
}

/*
=================
GraphicsOptions_Event
=================
*/
static void GraphicsOptions_Event( void* ptr, int event ) 
{
	InitialVideoOptions_s *ivo;

	if( event != QM_ACTIVATED ) {
	 	return;
	}

	switch( ((menucommon_s*)ptr)->id ) {

	case ID_LIST:
		ivo = &s_ivo_templates[s_graphicsoptions.list.curvalue];

		s_graphicsoptions.tq.curvalue				= ivo->tq;
//		s_graphicsoptions.lighting.curvalue			= ivo->lighting;
		s_graphicsoptions.colordepth.curvalue		= ivo->colordepth;
		s_graphicsoptions.texturebits.curvalue		= ivo->texturebits;
		s_graphicsoptions.geometry.curvalue			= ivo->geometry;
		s_graphicsoptions.filter.curvalue			= ivo->filter;
		s_graphicsoptions.fs.curvalue				= ivo->fullscreen;
		s_graphicsoptions.simpleshaders.curvalue	= ivo->simpleshaders;
		s_graphicsoptions.compresstextures.curvalue	= ivo->compresstextures;

		//TiM - revert the widescreen settings
		if ( s_graphicsoptions.aspectRatio.curvalue != ASPECTRATIO_4X3 )
		{
			s_graphicsoptions.aspectRatio.curvalue = s_graphicsoptions.lastRatio = ASPECTRATIO_4X3;

			s_graphicsoptions.mode.itemnames = NULL;
			s_graphicsoptions.mode.listnames = s_resolutions;
			s_graphicsoptions.mode.numitems = s_resolutionNums[ASPECTRATIO_4X3];
		}
		s_graphicsoptions.mode.curvalue				= ivo->mode;

		break;

	case ID_DRIVERINFO:
		UI_DriverInfo_Menu();
		break;

	case ID_BACK2:
		UI_PopMenu();
		break;

	case ID_GRAPHICS:
		break;

	case ID_DISPLAY:
		UI_PopMenu();
		UI_VideoDataMenu();		// Move to the Video Menu
//		UI_DisplayOptionsMenu();
		break;

//	case ID_SOUND:
//		UI_PopMenu();
//		UI_SoundOptionsMenu();
//		break;

	case ID_NETWORK:
		UI_PopMenu();
		UI_NetworkOptionsMenu();
		break;
	}
}


/*
================
GraphicsOptions_MenuDraw
================
*/
void GraphicsOptions_MenuDraw (void)
{
//APSFIX - rework this
	GraphicsOptions_UpdateMenuItems();

	Menu_Draw( &s_graphicsoptions.menu );
}

static qboolean GraphicsOptions_CheckWidescreen( void )
{
	int						j, i;
	int						customWidth = trap_Cvar_VariableValue( "r_customWidth" );
	int						customHeight = trap_Cvar_VariableValue( "r_customHeight" );
	videoResolutions_t		*v;

	//double check
	if ( s_graphicsoptions.mode.curvalue >= 0 )
		return qfalse;

	for ( j=0; j < s_wideScreenSets; j++ )
	{
		v = (videoResolutions_t *)s_widescreenResolutions[j];

		//check if it matches the resolution
		for ( i = 0; i < s_resolutionNums[j+1]; i++) //offset by 1 to avoid the 4X3 option
		{
			if ( v[i].width == customWidth && v[i].height == customHeight )
			{
				s_graphicsoptions.aspectRatio.curvalue = s_graphicsoptions.lastRatio = j+1;

				//configure the mode list accordingly
				s_graphicsoptions.mode.listnames = 0;
				s_graphicsoptions.mode.itemnames = 0;
				s_graphicsoptions.mode.itemnames = (const char **)s_widescreenResStrings[s_graphicsoptions.aspectRatio.curvalue-1];
				s_graphicsoptions.mode.numitems = s_resolutionNums[s_graphicsoptions.aspectRatio.curvalue];

				s_graphicsoptions.mode.curvalue = i;
				return qtrue;
			}
		}
	}

	//ffs... no luck
	return qfalse;
}

/*
=================
GraphicsOptions_SetMenuItems
=================
*/
static void GraphicsOptions_SetMenuItems( void )
{

	s_graphicsoptions.aspectRatio.curvalue	= ASPECTRATIO_4X3; //set aspect to 'Normal' for now

	s_graphicsoptions.mode.curvalue			= (trap_Cvar_VariableValue( "r_mode" ) - 2);
	
	//TiM - adjust for widescreen
	if ( s_graphicsoptions.mode.curvalue < 0 && !GraphicsOptions_CheckWidescreen() ) //less than 0 means custom resolution now
		s_graphicsoptions.mode.curvalue = 1;

	s_graphicsoptions.fs.curvalue = trap_Cvar_VariableValue("r_fullscreen");
	s_graphicsoptions.allow_extensions.curvalue = trap_Cvar_VariableValue("r_allowExtensions");
	s_graphicsoptions.simpleshaders.curvalue = trap_Cvar_VariableValue("r_lowEndVideo");
	s_graphicsoptions.compresstextures.curvalue = trap_Cvar_VariableValue("r_ext_compress_textures");

	s_graphicsoptions.tq.curvalue = 3-trap_Cvar_VariableValue( "r_picmip");
	if ( s_graphicsoptions.tq.curvalue < 0 )
	{
		s_graphicsoptions.tq.curvalue = 0;
	}
	else if ( s_graphicsoptions.tq.curvalue > 3 )
	{
		s_graphicsoptions.tq.curvalue = 3;
	}

//	s_graphicsoptions.lighting.curvalue = trap_Cvar_VariableValue( "r_vertexLight" ) != 0;
	switch ( ( int ) trap_Cvar_VariableValue( "r_texturebits" ) )
	{
	default:
	case 0:
		s_graphicsoptions.texturebits.curvalue = 0;
		break;
	case 16:
		s_graphicsoptions.texturebits.curvalue = 1;
		break;
	case 32:
		s_graphicsoptions.texturebits.curvalue = 2;
		break;
	}

	if ( !Q_stricmp( UI_Cvar_VariableString( "r_textureMode" ), "GL_LINEAR_MIPMAP_NEAREST" ) )
	{
		s_graphicsoptions.filter.curvalue = 0;
	}
	else
	{
		s_graphicsoptions.filter.curvalue = 1;
	}

	if ( trap_Cvar_VariableValue( "r_lodBias" ) > 0 )
	{
		if ( trap_Cvar_VariableValue( "r_subdivisions" ) >= 20 )
		{
			s_graphicsoptions.geometry.curvalue = 0;
		}
		else
		{
			s_graphicsoptions.geometry.curvalue = 1;
		}
	}
	else
	{
		s_graphicsoptions.geometry.curvalue = 2;
	}

	switch ( ( int ) trap_Cvar_VariableValue( "r_colorbits" ) )
	{
	default:
	case 0:
		s_graphicsoptions.colordepth.curvalue = 0;
		break;
	case 16:
		s_graphicsoptions.colordepth.curvalue = 1;
		break;
	case 32:
		s_graphicsoptions.colordepth.curvalue = 2;
		break;
	}

	if ( s_graphicsoptions.fs.curvalue == 0 )
	{
		s_graphicsoptions.colordepth.curvalue = 0;
	}

}



void VideoSideButtonsAction( qboolean result ) 
{
	if ( result )	// Yes - do it
	{
		Video_MenuEvent(holdControlPtr, holdControlEvent);
	}
}

/*
=================
VideoSideButtons_MenuEvent
=================
*/
static void VideoSideButtons_MenuEvent (void* ptr, int event)
{

	if (event != QM_ACTIVATED)
		return;

	holdControlPtr = ptr;
	holdControlEvent = event;

	if (s_graphicsoptions.apply.generic.flags & QMF_BLINK)	// Video apply changes button is flashing
	{
		UI_ConfirmMenu(menu_normal_text[MNT_LOOSEVIDSETTINGS], 0, VideoSideButtonsAction);	
	}
	else	// Go ahead, act normal
	{
		Video_MenuEvent (holdControlPtr, holdControlEvent);
	}
}

/*
=================
Video_MenuEvent
=================
*/
static void Video_MenuEvent (void* ptr, int event)
{
	menuframework_s*	m;

	if (event != QM_ACTIVATED)
		return;

	m = ((menucommon_s*)ptr)->parent;

	switch (((menucommon_s*)ptr)->id)
	{
		case ID_VIDEO:				// You're already in video menus, doofus
			break;

		case ID_ARROWDWN:
			VideoDriver_Lines(1);
			break;

		case ID_ARROWUP:
			VideoDriver_Lines(-1);
			break;

		case ID_VIDEODRIVERS:
			if (m != &s_videodriver.menu)	//	Not already in menu?
			{
				UI_PopMenu();			// Get rid of whatever is ontop
				UI_VideoDriverMenu();	// Move to the Controls Menu
			}
			break;

		case ID_VIDEODATA:
			if (m != &s_videodata.menu)	//	Not already in menu?
			{
				UI_PopMenu();			// Get rid of whatever is ontop
				UI_VideoDataMenu();		// Move to the Controls Menu
			}
			break;

		case ID_VIDEODATA2:
			if (m != &s_videodata2.menu)	//	Not already in menu?
			{
				UI_PopMenu();				// Get rid of whatever is ontop
				UI_VideoData2SettingsMenu();	// Move to the Controls Menu
			}
			break;

		case ID_CONTROLS:
			UI_PopMenu();			// Get rid of whatever is ontop
//			UI_SetupWeaponsMenu();	// Move to the Controls Menu
			break;

		case ID_SOUND:
			UI_PopMenu();			// Get rid of whatever is ontop
			UI_SoundMenu();			// Move to the Sound Menu
			break;

		case ID_GAMEOPTIONS:
			UI_PopMenu();			// Get rid of whatever is ontop
			UI_GameOptionsMenu();	// Move to the Game Options Menu
			break;

		case ID_CDKEY:
			UI_PopMenu();			// Get rid of whatever is ontop
			UI_CDKeyMenu();			// Move to the CD Key Menu
			break;

		case ID_MAINMENU:
			UI_PopMenu();
			break;

		case ID_INGAMEMENU :
			UI_PopMenu();
			break;

	}
}

/*
=================
M_VideoDataMenu_Graphics
=================
*/
void M_VideoDataMenu_Graphics (void)
{
	UI_MenuFrame(&s_videodata.menu);

	UI_DrawProportionalString(  74,  66, "207",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  84, "44909",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  188, "357",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  206, "250624",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  395, "456730-1",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	UI_Setup_MenuButtons();

	// Rest of Bottom1_Graphics
//	trap_R_SetColor( colorTable[CT_LTORANGE]);
//	UI_DrawHandlePic(  30, 203, 47, 69, uis.whiteShader);	// Top Left column above two buttons
//	UI_DrawHandlePic(  30, 344, 47, 45, uis.whiteShader);	// Top Left column below two buttons

	// Brackets around Video Data
	trap_R_SetColor( colorTable[CT_LTPURPLE1]);
	UI_DrawHandlePic(158,163, 16, 16, uis.graphicBracket1CornerLU);
	UI_DrawHandlePic(158,179,  8, 233, uis.whiteShader);
	UI_DrawHandlePic(158,412, 16, -16, uis.graphicBracket1CornerLU);	//LD

	UI_DrawHandlePic(174,163, 320, 8, uis.whiteShader);	// Top line

	UI_DrawHandlePic(494,163, 128, 128, s_videodata.swooshTop);			// Top swoosh

	UI_DrawHandlePic(501,188, 110, 54, uis.whiteShader);	// Top right column

	UI_DrawHandlePic(501,348, 110, 55, uis.whiteShader);	// Bottom right column

	UI_DrawHandlePic(494,406, 128, 128, s_videodata.swooshBottom);		// Bottom swoosh

	UI_DrawHandlePic(174,420, 320, 8, uis.whiteShader);	// Bottom line
}
/*
=================
VideoData_MenuDraw
=================
*/
static void VideoData_MenuDraw (void)
{
	GraphicsOptions_UpdateMenuItems();

	M_VideoDataMenu_Graphics();

	Menu_Draw( &s_videodata.menu );
}

/*
=================
UI_VideoDataMenu_Cache
=================
*/
void UI_VideoDataMenu_Cache(void)
{
	s_videodata.swooshTop = trap_R_RegisterShaderNoMip("menu/common/swoosh_top.tga");
	s_videodata.swooshBottom= trap_R_RegisterShaderNoMip("menu/common/swoosh_bottom.tga");
	s_videodata.swooshTopSmall= trap_R_RegisterShaderNoMip("menu/common/swoosh_topsmall.tga");
	s_videodata.swooshBottomSmall= trap_R_RegisterShaderNoMip("menu/common/swoosh_bottomsmall.tga");
}


/*
=================
VideoData_MenuInit
=================
*/
static void VideoData_MenuInit( void )
{
	int x,y,width,inc;

	UI_VideoDataMenu_Cache();

	// Menu Data
	s_videodata.menu.nitems						= 0;
	s_videodata.menu.wrapAround					= qtrue;
	s_videodata.menu.draw						= VideoData_MenuDraw;
	s_videodata.menu.fullscreen					= qtrue;
	s_videodata.menu.descX						= MENU_DESC_X;
	s_videodata.menu.descY						= MENU_DESC_Y;
	s_videodata.menu.listX						= 230;
	s_videodata.menu.listY						= 188;
	s_videodata.menu.titleX						= MENU_TITLE_X;
	s_videodata.menu.titleY						= MENU_TITLE_Y;
	s_videodata.menu.titleI						= MNT_CONTROLSMENU_TITLE;
	s_videodata.menu.footNoteEnum				= MNT_VIDEOSETUP;

	x = 170;
	y = 178;
	width = 145;
	
	s_graphicsoptions.list.generic.type			= MTYPE_SPINCONTROL;
	s_graphicsoptions.list.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_graphicsoptions.list.generic.x			= x;
	s_graphicsoptions.list.generic.y			= y;
	s_graphicsoptions.list.generic.callback		= GraphicsOptions_Event;
	s_graphicsoptions.list.generic.id			= ID_LIST;
	s_graphicsoptions.list.textEnum				= MBT_VIDEOOPTIONS;
	s_graphicsoptions.list.textcolor			= CT_BLACK;
	s_graphicsoptions.list.textcolor2			= CT_WHITE;
	s_graphicsoptions.list.color				= CT_DKPURPLE1;
	s_graphicsoptions.list.color2				= CT_LTPURPLE1;
	s_graphicsoptions.list.textX				= 5;
	s_graphicsoptions.list.textY				= 2;
	s_graphicsoptions.list.listnames			= s_graphics_options_Names;
	s_graphicsoptions.list.width				= width;

	inc = 20;
	y += inc;
	s_graphicsoptions.allow_extensions.generic.type		= MTYPE_SPINCONTROL;
	s_graphicsoptions.allow_extensions.generic.flags	= QMF_HIGHLIGHT_IF_FOCUS;
	s_graphicsoptions.allow_extensions.generic.x		= x;
	s_graphicsoptions.allow_extensions.generic.y		= y;
	s_graphicsoptions.allow_extensions.textEnum			= MBT_VIDEOGLEXTENTIONS;
	s_graphicsoptions.allow_extensions.textcolor		= CT_BLACK;
	s_graphicsoptions.allow_extensions.textcolor2		= CT_WHITE;
	s_graphicsoptions.allow_extensions.color			= CT_DKPURPLE1;
	s_graphicsoptions.allow_extensions.color2			= CT_LTPURPLE1;
	s_graphicsoptions.allow_extensions.textX			= 5;
	s_graphicsoptions.allow_extensions.textY			= 2;
	s_graphicsoptions.allow_extensions.listnames		= s_OffOnNone_Names;
	s_graphicsoptions.allow_extensions.width			= width;

	y += inc;
	s_graphicsoptions.aspectRatio.generic.type		= MTYPE_SPINCONTROL;
	s_graphicsoptions.aspectRatio.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_graphicsoptions.aspectRatio.generic.x			= x;
	s_graphicsoptions.aspectRatio.generic.y			= y;
	s_graphicsoptions.aspectRatio.textEnum			= MBT_ASPECTRATIO;
	s_graphicsoptions.aspectRatio.textcolor			= CT_BLACK;
	s_graphicsoptions.aspectRatio.textcolor2		= CT_WHITE;
	s_graphicsoptions.aspectRatio.color				= CT_DKPURPLE1;
	s_graphicsoptions.aspectRatio.color2			= CT_LTPURPLE1;
	s_graphicsoptions.aspectRatio.textX				= 5;
	s_graphicsoptions.aspectRatio.textY				= 2;
	s_graphicsoptions.aspectRatio.listnames			= s_aspectRatios;
	s_graphicsoptions.aspectRatio.width				= width;

	y += inc;
	// references/modifies "r_mode"
	// TiM: can now be potentially '-1', 
	// in which case 'r_customeheight' and 'r_customwidth'
	// will be used
	s_graphicsoptions.mode.generic.type					= MTYPE_SPINCONTROL;
	s_graphicsoptions.mode.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_graphicsoptions.mode.generic.x					= x;
	s_graphicsoptions.mode.generic.y					= y;
	s_graphicsoptions.mode.generic.callback				= GraphicsOptions_Event;
	s_graphicsoptions.mode.textEnum						= MBT_VIDEOMODE;
	s_graphicsoptions.mode.textcolor					= CT_BLACK;
	s_graphicsoptions.mode.textcolor2					= CT_WHITE;
	s_graphicsoptions.mode.color						= CT_DKPURPLE1;
	s_graphicsoptions.mode.color2						= CT_LTPURPLE1;
	s_graphicsoptions.mode.textX						= 5;
	s_graphicsoptions.mode.textY						= 2;
	s_graphicsoptions.mode.listnames					= s_resolutions;
	s_graphicsoptions.mode.width						= width;

	y += inc;
	s_graphicsoptions.colordepth.generic.type			= MTYPE_SPINCONTROL;
	s_graphicsoptions.colordepth.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_graphicsoptions.colordepth.generic.x				= x;
	s_graphicsoptions.colordepth.generic.y				= y;
	s_graphicsoptions.colordepth.textEnum				= MBT_VIDEOCOLORDEPTH;
	s_graphicsoptions.colordepth.textcolor				= CT_BLACK;
	s_graphicsoptions.colordepth.textcolor2				= CT_WHITE;
	s_graphicsoptions.colordepth.color					= CT_DKPURPLE1;
	s_graphicsoptions.colordepth.color2					= CT_LTPURPLE1;
	s_graphicsoptions.colordepth.textX					= 5;
	s_graphicsoptions.colordepth.textY					= 2;
	s_graphicsoptions.colordepth.listnames				= s_colordepth_Names;
	s_graphicsoptions.colordepth.width					= width;

	y += inc;
	s_graphicsoptions.fs.generic.type			= MTYPE_SPINCONTROL;
	s_graphicsoptions.fs.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_graphicsoptions.fs.generic.x				= x;
	s_graphicsoptions.fs.generic.y				= y;
	s_graphicsoptions.fs.textEnum				= MBT_VIDEOFULLSCREEN;
	s_graphicsoptions.fs.textcolor				= CT_BLACK;
	s_graphicsoptions.fs.textcolor2				= CT_WHITE;
	s_graphicsoptions.fs.color					= CT_DKPURPLE1;
	s_graphicsoptions.fs.color2					= CT_LTPURPLE1;
	s_graphicsoptions.fs.textX					= 5;
	s_graphicsoptions.fs.textY					= 2;
	s_graphicsoptions.fs.listnames				= s_OffOnNone_Names;
	s_graphicsoptions.fs.width					= width;
/*
	y += inc;
	s_graphicsoptions.lighting.generic.type				= MTYPE_SPINCONTROL;
	s_graphicsoptions.lighting.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_graphicsoptions.lighting.generic.x				= x;
	s_graphicsoptions.lighting.generic.y				= y;
	s_graphicsoptions.lighting.textEnum					= MBT_VIDEOLIGHTING;
	s_graphicsoptions.lighting.textcolor				= CT_BLACK;
	s_graphicsoptions.lighting.textcolor2				= CT_WHITE;
	s_graphicsoptions.lighting.color					= CT_DKPURPLE1;
	s_graphicsoptions.lighting.color2					= CT_LTPURPLE1;
	s_graphicsoptions.lighting.textX					= 5;
	s_graphicsoptions.lighting.textY					= 2;
	s_graphicsoptions.lighting.listnames				= s_lighting_Names;
	s_graphicsoptions.lighting.width						= width;
*/
	y += inc;
	// references/modifies "r_lodBias" & "subdivisions"
	s_graphicsoptions.geometry.generic.type				= MTYPE_SPINCONTROL;
	s_graphicsoptions.geometry.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_graphicsoptions.geometry.generic.x				= x;
	s_graphicsoptions.geometry.generic.y				= y;
	s_graphicsoptions.geometry.textEnum					= MBT_VIDEOGEOMETRY;
	s_graphicsoptions.geometry.textcolor				= CT_BLACK;
	s_graphicsoptions.geometry.textcolor2				= CT_WHITE;
	s_graphicsoptions.geometry.color					= CT_DKPURPLE1;
	s_graphicsoptions.geometry.color2					= CT_LTPURPLE1;
	s_graphicsoptions.geometry.textX					= 5;
	s_graphicsoptions.geometry.textY					= 2;
	s_graphicsoptions.geometry.listnames				= s_quality_Names;
	s_graphicsoptions.geometry.width					= width;

	y += inc;
	s_graphicsoptions.tq.generic.type		= MTYPE_SPINCONTROL;
	s_graphicsoptions.tq.generic.flags		= QMF_HIGHLIGHT_IF_FOCUS;
	s_graphicsoptions.tq.generic.x			= x;
	s_graphicsoptions.tq.generic.y			= y;
	s_graphicsoptions.tq.textEnum			= MBT_VIDEOTEXTUREDETAIL;
	s_graphicsoptions.tq.textcolor			= CT_BLACK;
	s_graphicsoptions.tq.textcolor2			= CT_WHITE;
	s_graphicsoptions.tq.color				= CT_DKPURPLE1;
	s_graphicsoptions.tq.color2				= CT_LTPURPLE1;
	s_graphicsoptions.tq.textX				= 5;
	s_graphicsoptions.tq.textY				= 2;
	s_graphicsoptions.tq.listnames			= s_4quality_Names;
	s_graphicsoptions.tq.width					= width;

	y += inc;
	// references/modifies "r_textureBits"
	s_graphicsoptions.texturebits.generic.type				= MTYPE_SPINCONTROL;
	s_graphicsoptions.texturebits.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_graphicsoptions.texturebits.generic.x					= x;
	s_graphicsoptions.texturebits.generic.y					= y;
	s_graphicsoptions.texturebits.textEnum					= MBT_VIDEOTEXTUREBITS;
	s_graphicsoptions.texturebits.textcolor					= CT_BLACK;
	s_graphicsoptions.texturebits.textcolor2				= CT_WHITE;
	s_graphicsoptions.texturebits.color						= CT_DKPURPLE1;
	s_graphicsoptions.texturebits.color2					= CT_LTPURPLE1;
	s_graphicsoptions.texturebits.textX						= 5;
	s_graphicsoptions.texturebits.textY						= 2;
	s_graphicsoptions.texturebits.listnames					= s_tqbits_Names;
	s_graphicsoptions.texturebits.width					= width;

	y += inc;
	// references/modifies "r_textureMode"
	s_graphicsoptions.filter.generic.type					= MTYPE_SPINCONTROL;
	s_graphicsoptions.filter.generic.flags					= QMF_HIGHLIGHT_IF_FOCUS;
	s_graphicsoptions.filter.generic.x						= x;
	s_graphicsoptions.filter.generic.y						= y;
	s_graphicsoptions.filter.textEnum						= MBT_VIDEOTEXTUREFILTER;
	s_graphicsoptions.filter.textcolor						= CT_BLACK;
	s_graphicsoptions.filter.textcolor2						= CT_WHITE;
	s_graphicsoptions.filter.color							= CT_DKPURPLE1;
	s_graphicsoptions.filter.color2							= CT_LTPURPLE1;
	s_graphicsoptions.filter.textX							= 5;
	s_graphicsoptions.filter.textY							= 2;
	s_graphicsoptions.filter.listnames						= s_filter_Names;
	s_graphicsoptions.filter.width					= width;

	y += inc;
	// references/modifies "r_lowEndVideo"
	s_graphicsoptions.simpleshaders.generic.type				= MTYPE_SPINCONTROL;
	s_graphicsoptions.simpleshaders.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_graphicsoptions.simpleshaders.generic.x					= x;
	s_graphicsoptions.simpleshaders.generic.y					= y;
	s_graphicsoptions.simpleshaders.textEnum					= MBT_SIMPLESHADER;
	s_graphicsoptions.simpleshaders.textcolor					= CT_BLACK;
	s_graphicsoptions.simpleshaders.textcolor2				= CT_WHITE;
	s_graphicsoptions.simpleshaders.color						= CT_DKPURPLE1;
	s_graphicsoptions.simpleshaders.color2					= CT_LTPURPLE1;
	s_graphicsoptions.simpleshaders.textX						= 5;
	s_graphicsoptions.simpleshaders.textY						= 2;
	s_graphicsoptions.simpleshaders.listnames					= s_OffOnNone_Names;
	s_graphicsoptions.simpleshaders.width					= width;

	y += inc;
	// references/modifies "r_ext_compress_textures"
	s_graphicsoptions.compresstextures.generic.type				= MTYPE_SPINCONTROL;
	s_graphicsoptions.compresstextures.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_graphicsoptions.compresstextures.generic.x				= x;
	s_graphicsoptions.compresstextures.generic.y				= y;
	s_graphicsoptions.compresstextures.textEnum					= MBT_COMPRESSEDTEXTURES;
	s_graphicsoptions.compresstextures.textcolor				= CT_BLACK;
	s_graphicsoptions.compresstextures.textcolor2				= CT_WHITE;
	s_graphicsoptions.compresstextures.color					= CT_DKPURPLE1;
	s_graphicsoptions.compresstextures.color2					= CT_LTPURPLE1;
	s_graphicsoptions.compresstextures.textX					= 5;
	s_graphicsoptions.compresstextures.textY					= 2;
	s_graphicsoptions.compresstextures.listnames				= s_OffOnNone_Names;
	s_graphicsoptions.compresstextures.width					= width;

	s_graphicsoptions.apply.generic.type				= MTYPE_ACTION;
	s_graphicsoptions.apply.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS|QMF_GRAYED;
	s_graphicsoptions.apply.generic.x					= 501;
	s_graphicsoptions.apply.generic.y					= 245;
	s_graphicsoptions.apply.generic.callback			= GraphicsOptions_ApplyChanges;
	s_graphicsoptions.apply.textEnum					= MBT_VIDEOAPPLYCHANGES;
	s_graphicsoptions.apply.textcolor					= CT_BLACK;
	s_graphicsoptions.apply.textcolor2					= CT_WHITE;
	s_graphicsoptions.apply.textcolor3					= CT_LTGREY;
	s_graphicsoptions.apply.color						= CT_DKPURPLE1;
	s_graphicsoptions.apply.color2						= CT_LTPURPLE1;
	s_graphicsoptions.apply.color3						= CT_DKGREY;
	s_graphicsoptions.apply.textX						= 5;
	s_graphicsoptions.apply.textY						= 80;
	s_graphicsoptions.apply.width						= 110;
	s_graphicsoptions.apply.height						= 100;

	SetupMenu_TopButtons(&s_videodata.menu,MENU_VIDEODATA,&s_graphicsoptions.apply);

	Menu_AddItem( &s_videodata.menu, ( void * )&s_graphicsoptions.list);
	Menu_AddItem( &s_videodata.menu, ( void * )&s_graphicsoptions.allow_extensions);
	Menu_AddItem( &s_videodata.menu, ( void * )&s_graphicsoptions.aspectRatio);
	Menu_AddItem( &s_videodata.menu, ( void * )&s_graphicsoptions.mode);
	Menu_AddItem( &s_videodata.menu, ( void * )&s_graphicsoptions.colordepth);
	Menu_AddItem( &s_videodata.menu, ( void * )&s_graphicsoptions.fs);
//	Menu_AddItem( &s_videodata.menu, ( void * )&s_graphicsoptions.lighting);
	Menu_AddItem( &s_videodata.menu, ( void * )&s_graphicsoptions.geometry);
	Menu_AddItem( &s_videodata.menu, ( void * )&s_graphicsoptions.tq);
	Menu_AddItem( &s_videodata.menu, ( void * )&s_graphicsoptions.texturebits);
	Menu_AddItem( &s_videodata.menu, ( void * )&s_graphicsoptions.filter);
	Menu_AddItem( &s_videodata.menu, ( void * )&s_graphicsoptions.simpleshaders);
	Menu_AddItem( &s_videodata.menu, ( void * )&s_graphicsoptions.compresstextures);
	Menu_AddItem( &s_videodata.menu, ( void * )&s_graphicsoptions.apply);


	Video_SideButtons(&s_videodata.menu,ID_VIDEODATA);

	GraphicsOptions_SetMenuItems();
	GraphicsOptions_GetInitialVideo();

	/*if ( uis.glconfig.driverType == GLDRV_ICD &&
		 uis.glconfig.hardwareType == GLHW_3DFX_2D3D )
	{
		s_graphicsoptions.driver.generic.flags |= QMF_HIDDEN|QMF_INACTIVE;
	}*/
}


/*
=================
UI_VideoDataMenu
=================
*/
void UI_VideoDataMenu( void )
{
	VideoData_MenuInit();

	UI_PushMenu( &s_videodata.menu );
}

/*
=================
Video_SideButtons
=================
*/
void Video_SideButtons(menuframework_s *menu,int menuType)
{

	// Button Data
	s_video_data.generic.type				= MTYPE_BITMAP;      
	s_video_data.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_video_data.generic.x					= video_sidebuttons[0][0];
	s_video_data.generic.y					= video_sidebuttons[0][1];
	s_video_data.generic.name				= GRAPHIC_SQUARE;
	s_video_data.generic.id					= ID_VIDEODATA;
	if (menuType == ID_VIDEODATA)
	{
		s_video_data.generic.callback			= VideoSideButtons_MenuEvent;
	}
	else
	{
		s_video_data.generic.callback			= Video_MenuEvent;
	}
	s_video_data.width						= MENU_BUTTON_MED_WIDTH - 10;
	s_video_data.height						= MENU_BUTTON_MED_HEIGHT;
	s_video_data.color						= CT_DKPURPLE1;
	s_video_data.color2						= CT_LTPURPLE1;
	s_video_data.textX						= 5;
	s_video_data.textY						= 2;
	s_video_data.textEnum					= MBT_VIDEODATA;
	if (menuType == ID_VIDEODATA)
	{
		s_video_data.textcolor				= CT_WHITE;
		s_video_data.textcolor2				= CT_WHITE;
		s_video_data.generic.flags			= QMF_GRAYED;
	}
	else
	{
		s_video_data.textcolor				= CT_BLACK;
		s_video_data.textcolor2				= CT_WHITE;
	}

	s_video_data2.generic.type				= MTYPE_BITMAP;      
	s_video_data2.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_video_data2.generic.x					= video_sidebuttons[1][0];
	s_video_data2.generic.y					= video_sidebuttons[1][1];
	s_video_data2.generic.name				= GRAPHIC_SQUARE;
	s_video_data2.generic.id				= ID_VIDEODATA2;
	if (menuType == ID_VIDEODATA)
	{
		s_video_data2.generic.callback			= VideoSideButtons_MenuEvent;
	}
	else
	{
		s_video_data2.generic.callback			= Video_MenuEvent;
	}
	s_video_data2.width						= MENU_BUTTON_MED_WIDTH - 10;
	s_video_data2.height					= MENU_BUTTON_MED_HEIGHT;
	s_video_data2.color						= CT_DKPURPLE1;
	s_video_data2.color2					= CT_LTPURPLE1;
	s_video_data2.textX						= 5;
	s_video_data2.textY						= 2;
	s_video_data2.textEnum					= MBT_VIDEODATA2;
	s_video_data2.textcolor					= CT_WHITE;
	s_video_data2.textcolor2				= CT_WHITE;
	if (menuType == ID_VIDEODATA2)
	{
		s_video_data2.textcolor				= CT_WHITE;
		s_video_data2.textcolor2			= CT_WHITE;
		s_video_data2.generic.flags			= QMF_GRAYED;
	}
	else
	{
		s_video_data2.textcolor				= CT_BLACK;
		s_video_data2.textcolor2			= CT_WHITE;
	}

	s_video_drivers.generic.type			= MTYPE_BITMAP;      
	s_video_drivers.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_video_drivers.generic.x				= video_sidebuttons[2][0];
	s_video_drivers.generic.y				= video_sidebuttons[2][1];
	s_video_drivers.generic.name			= GRAPHIC_SQUARE;
	s_video_drivers.generic.id				= ID_VIDEODRIVERS;
	if (menuType == ID_VIDEODATA)
	{
		s_video_drivers.generic.callback			= VideoSideButtons_MenuEvent;
	}
	else
	{
		s_video_drivers.generic.callback			= Video_MenuEvent;
	}
	s_video_drivers.width					= MENU_BUTTON_MED_WIDTH - 10;
	s_video_drivers.height					= MENU_BUTTON_MED_HEIGHT;
	s_video_drivers.color					= CT_DKPURPLE1;
	s_video_drivers.color2					= CT_LTPURPLE1;
	s_video_drivers.textX					= 5;
	s_video_drivers.textY					= 2;
	s_video_drivers.textEnum				= MBT_VIDEODRIVERS;
	if (menuType == ID_VIDEODRIVERS)
	{
		s_video_drivers.textcolor			= CT_WHITE;
		s_video_drivers.textcolor2			= CT_WHITE;
		s_video_drivers.generic.flags		= QMF_GRAYED;
	}
	else
	{
		s_video_drivers.textcolor			= CT_BLACK;
		s_video_drivers.textcolor2			= CT_WHITE;
	}

	Menu_AddItem( menu, ( void * )&s_video_data);
	Menu_AddItem( menu, ( void * )&s_video_data2);
	Menu_AddItem( menu, ( void * )&s_video_drivers);

}


/*
=================
VideoDriver_Lines
=================
*/
void VideoDriver_Lines(int increment)
{
	int		i,i2;

	s_videodriver.currentDriverLine += increment;

	i=0;
	i2 = 0;

	i = (s_videodriver.currentDriverLine * 2);
	if (i<0)
	{
		s_videodriver.currentDriverLine = 0;
		return;
	}

	if (i>s_videodriver.driverCnt)
	{
		s_videodriver.currentDriverLine = (s_videodriver.driverCnt/2);
		return;
	}
	else if (i==s_videodriver.driverCnt)
	{
		s_videodriver.currentDriverLine = (s_videodriver.driverCnt/2) - 1;
		return;
	}

	if (!s_videodriver.drivers[i + 22])
	{
		s_videodriver.currentDriverLine -= increment;
		s_videodriver.activeArrowDwn = qfalse;
		return;
	}

	for (; i < MAX_VID_DRIVERS; i++)
	{
		if (s_videodriver.drivers[i])
		{
			if (i2<24)
			{
				((menutext_s *)g_videolines[i2])->string	= s_videodriver.drivers[i];      
				i2++;
			}
		}
		else 
		{
			if (i2<24)
			{
				((menutext_s *)g_videolines[i2])->string	= NULL;      
				i2++;
			}
			else 
			{
				break;
			}
		}
	}	

	// Set up arrows

	if (increment > 0)
	{
		s_videodriver.activeArrowUp = qtrue;
	}

	if (s_videodriver.currentDriverLine < 1)
	{
		s_videodriver.activeArrowUp = qfalse;
	}

	if (i2>= 24)
	{
		s_videodriver.activeArrowDwn = qtrue;
	}

	i = (s_videodriver.currentDriverLine * 2);
	if (!s_videodriver.drivers[i + 24]) 
	{
		s_videodriver.activeArrowDwn = qfalse;
		return;
	}

}

/*
=================
VideoDriver_LineSetup
=================
*/
void VideoDriver_LineSetup(void)
{
	char	*bufhold;
	char	*eptr;
	int		i;

	strcpy( s_videodriver.extensionsString, uis.glconfig.extensions_string );
	eptr = s_videodriver.extensionsString;
	i=0;

	s_videodriver.driverCnt = 0;

	while ( i < MAX_VID_DRIVERS && *eptr )
	{
		while ( *eptr )
		{
			bufhold = eptr;

			while(*bufhold !=  ' ')
			{
				++bufhold;
			}
			*bufhold = 0;

			s_videodriver.drivers[i] = eptr;

			if (i<24)
			{
				((menutext_s *)g_videolines[i])->string	= eptr;      
			}

			bufhold++;	
			eptr = bufhold;
			s_videodriver.driverCnt++;
			i++;
		}
	}	

	// Set down arrows
	if (i> 24)
	{
		s_videodriver.activeArrowDwn = qtrue;
	}

	s_videodriver.currentDriverLine = 0;

}

/*
=================
VideoDriver_MenuKey
=================
*/
sfxHandle_t VideoDriver_MenuKey (int key)
{
	return ( Menu_DefaultKey( &s_videodriver.menu, key ) );
}

/*
=================
M_VideoDriverMenu_Graphics
=================
*/
void M_VideoDriverMenu_Graphics (void)
{
	float labelColor[] = { 0, 1.0, 0, 1.0 };
	float textColor[] = { 1, 1, 1, 1 };
	int x,y,x2,x3;

	UI_MenuFrame(&s_videodriver.menu);

	UI_DrawProportionalString(  74,  66, "207",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  84, "44909",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  188, "357",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  206, "250624",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  395, "456730-1",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	UI_Setup_MenuButtons();

//	trap_R_SetColor( colorTable[CT_DKPURPLE1]);
//	UI_DrawHandlePic(  30, 203, 47, 69, uis.whiteShader);	// Top Left column above two buttons
//	UI_DrawHandlePic(  30, 344, 47, 45, uis.whiteShader);	// Top Left column below two buttons

	// Top Frame
	UI_DrawHandlePic( 178, 136,  32, -32, s_videodriver.corner_ll_8_16);	// UL
	UI_DrawHandlePic( 178, 221,  32,  32, s_videodriver.corner_ll_8_16);	// LL
	UI_DrawHandlePic( 556, 136, -32, -32, s_videodriver.corner_ll_8_16);	// UR
	UI_DrawHandlePic( 556, 221, -32,  32, s_videodriver.corner_ll_8_16);	// LR
	UI_DrawHandlePic(194,157, 378,  8, uis.whiteShader);	// Top line
	UI_DrawHandlePic(178,165,  16, 60, uis.whiteShader);	// Left side
	UI_DrawHandlePic(572,165,  16, 60, uis.whiteShader);	// Right side
	UI_DrawHandlePic(194,224, 378,  8, uis.whiteShader);	// Bottom line


	// Lower Frame
	UI_DrawHandlePic( 178, 226,  32, -32, s_videodriver.corner_ll_16_16);	// UL
	UI_DrawHandlePic( 178, 414,  32,  32, s_videodriver.corner_ll_16_16);	// LL
	UI_DrawHandlePic( 556, 226, -32, -32, s_videodriver.corner_ll_16_16);	// UR
	UI_DrawHandlePic( 556, 414, -32,  32, s_videodriver.corner_ll_16_16);	// LR

	UI_DrawHandlePic( 194, 237, 378,  16, uis.whiteShader);	// Top line
	UI_DrawHandlePic( 178, 252,  16, 168, uis.whiteShader);	// Left side

	UI_DrawHandlePic( 572, 261,  16,  15, uis.whiteShader);	// Right side
	UI_DrawHandlePic( 572, 279,  16, 114, uis.whiteShader);	// Right side
	UI_DrawHandlePic( 572, 396,  16,  15, uis.whiteShader);	// Right side

	UI_DrawHandlePic( 194, 419, 378,  16, uis.whiteShader);	// Bottom line

	trap_R_SetColor( colorTable[CT_LTGOLD1]);
	if (s_videodriver.activeArrowUp)
	{
		UI_DrawHandlePic( 382, 237, 32,  -14, s_videodriver.arrow_dn);	
	}

	if (s_videodriver.activeArrowDwn)
	{
		UI_DrawHandlePic( 382, 422, 32,   14, s_videodriver.arrow_dn);	
	}

	x = 204;
	x2 = 259;
	x3 = x2 + 150;
	y = 168;
	UI_DrawProportionalString( x, y, menu_normal_text[MNT_VENDOR], UI_LEFT|UI_TINYFONT, labelColor );
	UI_DrawProportionalString( x2, y, uis.glconfig.vendor_string, UI_LEFT|UI_TINYFONT, textColor );
	y += 14;
	UI_DrawProportionalString( x, y, menu_normal_text[MNT_VERSION], UI_LEFT|UI_TINYFONT, labelColor );
	UI_DrawProportionalString( x2, y, uis.glconfig.version_string, UI_LEFT|UI_TINYFONT, textColor );
	y += 14;
	UI_DrawProportionalString( x, y, menu_normal_text[MNT_RENDERER], UI_LEFT|UI_TINYFONT, labelColor );
	UI_DrawProportionalString( x2, y, uis.glconfig.renderer_string, UI_LEFT|UI_TINYFONT, textColor );
	y += 14;
	UI_DrawProportionalString( x, y, menu_normal_text[MNT_PIXELFORMAT], UI_LEFT|UI_TINYFONT, labelColor );
	UI_DrawProportionalString( x2, y, va("color(%d-bits) Z(%d-bit) stencil(%d-bits)", uis.glconfig.colorBits, uis.glconfig.depthBits, uis.glconfig.stencilBits), UI_LEFT|UI_TINYFONT, textColor );

}

/*
=================
VideoDriver_MenuDraw
=================
*/
static void VideoDriver_MenuDraw (void)
{

	M_VideoDriverMenu_Graphics();

	Menu_Draw( &s_videodriver.menu );
}

/*
=================
UI_VideoDriverMenu_Cache
=================
*/
void UI_VideoDriverMenu_Cache(void)
{
	s_videodriver.corner_ll_16_16 = trap_R_RegisterShaderNoMip("menu/common/corner_ll_16_16.tga");
	s_videodriver.corner_ll_8_16 = trap_R_RegisterShaderNoMip("menu/common/corner_ll_8_16.tga");
	s_videodriver.arrow_dn = trap_R_RegisterShaderNoMip("menu/common/arrow_dn_16.tga");
}


/*
=================
Video_MenuInit
=================
*/
static void VideoDriver_MenuInit( void )
{
	int		i,x,y,x2;

	UI_VideoDriverMenu_Cache();

	s_videodriver.menu.nitems					= 0;
	s_videodriver.menu.wrapAround				= qtrue;
//	s_videodriver.menu.opening					= NULL;
//	s_videodriver.menu.closing					= NULL;
	s_videodriver.menu.draw						= VideoDriver_MenuDraw;
	s_videodriver.menu.key						= VideoDriver_MenuKey;
	s_videodriver.menu.fullscreen				= qtrue;
	s_videodriver.menu.descX					= MENU_DESC_X;
	s_videodriver.menu.descY					= MENU_DESC_Y;
	s_videodriver.menu.listX					= 230;
	s_videodriver.menu.listY					= 188;
	s_videodriver.menu.titleX					= MENU_TITLE_X;
	s_videodriver.menu.titleY					= MENU_TITLE_Y;
	s_videodriver.menu.titleI					= MNT_CONTROLSMENU_TITLE;
	s_videodriver.menu.footNoteEnum				= MNT_VIDEODRIVER;

	SetupMenu_TopButtons(&s_videodriver.menu,MENU_VIDEO,NULL);

	Video_SideButtons(&s_videodriver.menu,ID_VIDEODRIVERS);

	s_videodriver.arrowup.generic.type				= MTYPE_BITMAP;      
	s_videodriver.arrowup.generic.flags				= QMF_HIGHLIGHT_IF_FOCUS;
	s_videodriver.arrowup.generic.x					= 572;
	s_videodriver.arrowup.generic.y					= 262;
	s_videodriver.arrowup.generic.name				= "menu/common/arrow_up_16.tga";
	s_videodriver.arrowup.generic.id				= ID_ARROWUP;
	s_videodriver.arrowup.generic.callback			= Video_MenuEvent;
	s_videodriver.arrowup.width						= 16;
	s_videodriver.arrowup.height					= 16;
	s_videodriver.arrowup.color						= CT_DKBLUE1;
	s_videodriver.arrowup.color2					= CT_LTBLUE1;
	s_videodriver.arrowup.textX						= 0;
	s_videodriver.arrowup.textY						= 0;
	s_videodriver.arrowup.textEnum					= MBT_NONE;
	s_videodriver.arrowup.textcolor					= CT_BLACK;
	s_videodriver.arrowup.textcolor2				= CT_WHITE;
	Menu_AddItem( &s_videodriver.menu,( void * ) &s_videodriver.arrowup);

	s_videodriver.arrowdwn.generic.type				= MTYPE_BITMAP;      
	s_videodriver.arrowdwn.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_videodriver.arrowdwn.generic.x				= 572;
	s_videodriver.arrowdwn.generic.y				= 397;
	s_videodriver.arrowdwn.generic.name				= "menu/common/arrow_dn_16.tga";
	s_videodriver.arrowdwn.generic.id				= ID_ARROWDWN;
	s_videodriver.arrowdwn.generic.callback			= Video_MenuEvent;
	s_videodriver.arrowdwn.width					= 16;
	s_videodriver.arrowdwn.height					= 16;
	s_videodriver.arrowdwn.color					= CT_DKBLUE1;
	s_videodriver.arrowdwn.color2					= CT_LTBLUE1;
	s_videodriver.arrowdwn.textX					= 0;
	s_videodriver.arrowdwn.textY					= 0;
	s_videodriver.arrowdwn.textEnum					= MBT_NONE;
	s_videodriver.arrowdwn.textcolor				= CT_BLACK;
	s_videodriver.arrowdwn.textcolor2				= CT_WHITE;
	Menu_AddItem( &s_videodriver.menu, ( void * ) &s_videodriver.arrowdwn);

	s_videodriver.activeArrowDwn = qfalse;
	s_videodriver.activeArrowUp = qfalse;

	x = 204;
	x2 = 404;
	y = 260;

	for (i=0;i<24;i++)
	{
		((menutext_s *)g_videolines[i])->generic.type		= MTYPE_TEXT;      
		((menutext_s *)g_videolines[i])->generic.flags		= QMF_LEFT_JUSTIFY | QMF_INACTIVE;
		((menutext_s *)g_videolines[i])->generic.y			= y;
		if ((i % 2 ) == 0)
		{
			((menutext_s *)g_videolines[i])->generic.x			= x;
		}
		else 
		{
			((menutext_s *)g_videolines[i])->generic.x			= x2;
			y +=13;
		}


		((menutext_s *)g_videolines[i])->buttontextEnum		= MBT_NONE;
		((menutext_s *)g_videolines[i])->style				= UI_TINYFONT | UI_LEFT;
		((menutext_s *)g_videolines[i])->color				= colorTable[CT_LTPURPLE1];
		Menu_AddItem( &s_videodriver.menu, ( void * )g_videolines[i]);

	}

	// Print extensions
	VideoDriver_LineSetup();
}


/*
=================
UI_VideoDriverMenu
=================
*/
void UI_VideoDriverMenu( void )
{
	if (!s_videodriver.menu.initialized)
	{
		VideoDriver_MenuInit();
	}

	UI_PushMenu( &s_videodriver.menu );
}

/*
=================
GammaCallback2
=================
*/
void GammaCallback2( void *s, int notification )
{
	if (notification != QM_ACTIVATED)
		return;

	s_videodata2.apply_action2.generic.flags &= ~QMF_GRAYED;
	s_videodata2.apply_action2.generic.flags |= QMF_BLINK;

	GammaCallback(s,notification );

}


/*
=================
M_VideoData2Menu_Graphics
=================
*/
void M_VideoData2Menu_Graphics (void)
{
	int y;

	UI_MenuFrame(&s_videodata2.menu);

	UI_DrawProportionalString(  74,  66, "925",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  84, "88PK",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  188, "8125",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  206, "358677",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(  74,  395, "3-679",UI_RIGHT|UI_TINYFONT, colorTable[CT_BLACK]);

	UI_Setup_MenuButtons();

	y = 191;
	if (uis.glconfig.deviceSupportsGamma)
	{
		trap_R_SetColor( colorTable[CT_DKGREY]);
		UI_DrawHandlePic(  178, y, 68, 68, uis.whiteShader);	// 
		trap_R_SetColor( colorTable[CT_WHITE]);
		UI_DrawHandlePic(  180, y+2, 64, 64, s_videodata2.gamma);	// Starfleet graphic

		UI_DrawProportionalString( 256,  y + 5, menu_normal_text[MNT_GAMMA_LINE1],UI_SMALLFONT, colorTable[CT_LTGOLD1]);
		UI_DrawProportionalString( 256,  y + 25, menu_normal_text[MNT_GAMMA_LINE2],UI_SMALLFONT, colorTable[CT_LTGOLD1]);
		UI_DrawProportionalString( 256,  y + 45, menu_normal_text[MNT_GAMMA_LINE3],UI_SMALLFONT,colorTable[CT_LTGOLD1]);
	}
	else
	{
		UI_DrawProportionalString( 178,  y + 5, menu_normal_text[MNT_GAMMA2_LINE1],UI_SMALLFONT, colorTable[CT_LTGOLD1]);
		UI_DrawProportionalString( 178,  y + 25,menu_normal_text[MNT_GAMMA2_LINE2],UI_SMALLFONT, colorTable[CT_LTGOLD1]);
	}


	// Brackets around Video Data
	trap_R_SetColor( colorTable[CT_LTPURPLE1]);
	UI_DrawHandlePic(158,163, 16, 16, uis.graphicBracket1CornerLU);
	UI_DrawHandlePic(158,179,  8, 233, uis.whiteShader);
	UI_DrawHandlePic(158,412, 16, -16, uis.graphicBracket1CornerLU);	//LD

	UI_DrawHandlePic(174,163, 408, 8, uis.whiteShader);	// Top line

	UI_DrawHandlePic(579,163, 32, 16, s_videodata2.top);	// Corner, UR
	UI_DrawHandlePic(581,179, 30, 121, uis.whiteShader);	// Top right column
	UI_DrawHandlePic(581,303, 30, 109, uis.whiteShader);	// Bottom right column
	UI_DrawHandlePic(579,412, 32, -16, s_videodata2.top);	// Corner, LR

	UI_DrawHandlePic(174,420, 408, 8, uis.whiteShader);	// Bottom line
}

/*
=================
VideoData2_MenuDraw
=================
*/
static void VideoData2_MenuDraw (void)
{

	M_VideoData2Menu_Graphics();

	Menu_Draw( &s_videodata2.menu );
}


/*
=================
UI_VideoData2Menu_Cache
=================
*/
void UI_VideoData2Menu_Cache(void)
{
	s_videodata2.top = trap_R_RegisterShaderNoMip("menu/common/corner_ur_8_30.tga");
	s_videodata2.gamma = trap_R_RegisterShaderNoMip("menu/special/gamma_test.tga");
	trap_R_RegisterShaderNoMip(PIC_MONBAR2);
	trap_R_RegisterShaderNoMip(PIC_SLIDER);
}

/*
=================
VideoData2_MenuInit
=================
*/
static void VideoData2_MenuInit( void )
{
	int x,y;

	UI_VideoData2Menu_Cache();

	// Menu Data
	s_videodata2.menu.nitems						= 0;
	s_videodata2.menu.wrapAround					= qtrue;
//	s_videodata2.menu.opening						= NULL;
//	s_videodata2.menu.closing						= NULL;
	s_videodata2.menu.draw							= VideoData2_MenuDraw;
	s_videodata2.menu.fullscreen					= qtrue;
	s_videodata2.menu.descX							= MENU_DESC_X;
	s_videodata2.menu.descY							= MENU_DESC_Y;
	s_videodata2.menu.listX							= 230;
	s_videodata2.menu.listY							= 188;
	s_videodata2.menu.titleX						= MENU_TITLE_X;
	s_videodata2.menu.titleY						= MENU_TITLE_Y;
	s_videodata2.menu.titleI						= MNT_CONTROLSMENU_TITLE;
	s_videodata2.menu.footNoteEnum					= MNT_VIDEOSETUP;

	SetupMenu_TopButtons(&s_videodata2.menu,MENU_VIDEO,NULL);

	Video_SideButtons(&s_videodata2.menu,ID_VIDEODATA2);

	x = 180;
	y = 269;
	s_videodata2.gamma_slider.generic.type		= MTYPE_SLIDER;
	s_videodata2.gamma_slider.generic.x			= x + 162;
	s_videodata2.gamma_slider.generic.y			= y;
	s_videodata2.gamma_slider.generic.flags		= QMF_SMALLFONT;
	s_videodata2.gamma_slider.generic.callback	= GammaCallback2;
	s_videodata2.gamma_slider.minvalue			= 5;
	s_videodata2.gamma_slider.maxvalue			= 30;
	s_videodata2.gamma_slider.color				= CT_DKPURPLE1;
	s_videodata2.gamma_slider.color2			= CT_LTPURPLE1;
	s_videodata2.gamma_slider.generic.name		= PIC_MONBAR2;
	s_videodata2.gamma_slider.width				= 256;
	s_videodata2.gamma_slider.height			= 32;
	s_videodata2.gamma_slider.focusWidth		= 145;
	s_videodata2.gamma_slider.focusHeight		= 18;
	s_videodata2.gamma_slider.picName			= GRAPHIC_SQUARE;
	s_videodata2.gamma_slider.picX				= x;
	s_videodata2.gamma_slider.picY				= y;
	s_videodata2.gamma_slider.picWidth			= MENU_BUTTON_MED_WIDTH + 21;
	s_videodata2.gamma_slider.picHeight			= MENU_BUTTON_MED_HEIGHT;
	s_videodata2.gamma_slider.textX				= MENU_BUTTON_TEXT_X;
	s_videodata2.gamma_slider.textY				= MENU_BUTTON_TEXT_Y;
	s_videodata2.gamma_slider.textEnum			= MBT_BRIGHTNESS;
	s_videodata2.gamma_slider.textcolor			= CT_BLACK;
	s_videodata2.gamma_slider.textcolor2		= CT_WHITE;
	s_videodata2.gamma_slider.thumbName			= PIC_SLIDER;
	s_videodata2.gamma_slider.thumbHeight		= 32;
	s_videodata2.gamma_slider.thumbWidth		= 16;
	s_videodata2.gamma_slider.thumbGraphicWidth	= 9;
	s_videodata2.gamma_slider.thumbColor		= CT_DKBLUE1;
	s_videodata2.gamma_slider.thumbColor2		= CT_LTBLUE1;

	s_videodata2.apply_action2.generic.type				= MTYPE_ACTION;
	s_videodata2.apply_action2.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS|QMF_GRAYED;
	s_videodata2.apply_action2.generic.x				= 490;
	s_videodata2.apply_action2.generic.y				= 191;
	s_videodata2.apply_action2.generic.callback			= ApplyChanges2;
	s_videodata2.apply_action2.textEnum					= MBT_ACCEPT;
	s_videodata2.apply_action2.textcolor				= CT_BLACK;
	s_videodata2.apply_action2.textcolor2				= CT_WHITE;
	s_videodata2.apply_action2.textcolor3				= CT_LTGREY;
	s_videodata2.apply_action2.color					= CT_DKPURPLE1;
	s_videodata2.apply_action2.color2					= CT_LTPURPLE1;
	s_videodata2.apply_action2.color3					= CT_DKGREY;
	s_videodata2.apply_action2.textX					= 5;
	s_videodata2.apply_action2.textY					= 30;
	s_videodata2.apply_action2.width					= 82;
	s_videodata2.apply_action2.height					= 70;

	y = 330;
	s_videodata2.screensize_slider.generic.type		= MTYPE_SLIDER;
	s_videodata2.screensize_slider.generic.x		= x + 162;
	s_videodata2.screensize_slider.generic.y		= y;
	s_videodata2.screensize_slider.generic.flags	= QMF_SMALLFONT;
	s_videodata2.screensize_slider.generic.callback	= ScreensizeCallback;
	s_videodata2.screensize_slider.minvalue			= 30;
	s_videodata2.screensize_slider.maxvalue			= 100;
	s_videodata2.screensize_slider.color			= CT_DKPURPLE1;
	s_videodata2.screensize_slider.color2			= CT_LTPURPLE1;
	s_videodata2.screensize_slider.generic.name		= PIC_MONBAR2;
	s_videodata2.screensize_slider.width			= 256;
	s_videodata2.screensize_slider.height			= 32;
	s_videodata2.screensize_slider.focusWidth		= 145;
	s_videodata2.screensize_slider.focusHeight		= 18;
	s_videodata2.screensize_slider.picName			= GRAPHIC_SQUARE;
	s_videodata2.screensize_slider.picX				= x;
	s_videodata2.screensize_slider.picY				= y;
	s_videodata2.screensize_slider.picWidth			= MENU_BUTTON_MED_WIDTH + 21;
	s_videodata2.screensize_slider.picHeight		= MENU_BUTTON_MED_HEIGHT;
	s_videodata2.screensize_slider.textX			= MENU_BUTTON_TEXT_X;
	s_videodata2.screensize_slider.textY			= MENU_BUTTON_TEXT_Y;
	s_videodata2.screensize_slider.textEnum			= MBT_SCREENSIZE;
	s_videodata2.screensize_slider.textcolor		= CT_BLACK;
	s_videodata2.screensize_slider.textcolor2		= CT_WHITE;
	s_videodata2.screensize_slider.thumbName		= PIC_SLIDER;
	s_videodata2.screensize_slider.thumbHeight		= 32;
	s_videodata2.screensize_slider.thumbWidth		= 16;
	s_videodata2.screensize_slider.thumbGraphicWidth= 9;
	s_videodata2.screensize_slider.thumbColor		= CT_DKBLUE1;
	s_videodata2.screensize_slider.thumbColor2		= CT_LTBLUE1;

	y += 34;
	s_videodata2.anisotropicfiltering.generic.type			= MTYPE_SPINCONTROL;      
	s_videodata2.anisotropicfiltering.generic.flags			= QMF_HIGHLIGHT_IF_FOCUS;
	s_videodata2.anisotropicfiltering.generic.x				= x;
	s_videodata2.anisotropicfiltering.generic.y				= y;
	s_videodata2.anisotropicfiltering.generic.name			= GRAPHIC_BUTTONRIGHT;
	s_videodata2.anisotropicfiltering.generic.callback		= AnisotropicFilteringCallback; 
	s_videodata2.anisotropicfiltering.color					= CT_DKPURPLE1;
	s_videodata2.anisotropicfiltering.color2					= CT_LTPURPLE1;
	s_videodata2.anisotropicfiltering.textX					= MENU_BUTTON_TEXT_X;
	s_videodata2.anisotropicfiltering.textY					= MENU_BUTTON_TEXT_Y;
	s_videodata2.anisotropicfiltering.textEnum				= MBT_ANISOTROPICFILTERING;
	s_videodata2.anisotropicfiltering.textcolor				= CT_BLACK;
	s_videodata2.anisotropicfiltering.textcolor2				= CT_WHITE;	
	s_videodata2.anisotropicfiltering.listnames				= s_OffOnNone_Names;


	Menu_AddItem( &s_videodata2.menu, ( void * )&s_videodata2.gamma_slider);
	if (!uis.glconfig.deviceSupportsGamma)
	{
		Menu_AddItem( &s_videodata2.menu, ( void * )&s_videodata2.apply_action2);
	}
	Menu_AddItem( &s_videodata2.menu, ( void * )&s_videodata2.screensize_slider);
	Menu_AddItem( &s_videodata2.menu, ( void * )&s_videodata2.anisotropicfiltering);

}

/*
=================
UI_VideoData2SettingsGetCvars
=================
*/
static void	UI_VideoData2SettingsGetCvars()
{
	s_videodata2.gamma_slider.curvalue = trap_Cvar_VariableValue( "r_gamma" ) *  10.0f;
	s_videodata2.screensize_slider.curvalue = trap_Cvar_VariableValue( "cg_viewsize" );
	s_videodata2.anisotropicfiltering.curvalue = trap_Cvar_VariableValue( "r_ext_texture_filter_anisotropic" );
}

/*
=================
UI_VideoData2SettingsMenu
=================
*/
void UI_VideoData2SettingsMenu( void )
{
	UI_VideoData2SettingsGetCvars();

	if (!s_videodata2.menu.initialized)
	{
		VideoData2_MenuInit();
	}

	UI_PushMenu( &s_videodata2.menu );
}

