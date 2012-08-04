// Copyright (C) 1999-2000 Id Software, Inc.
//
#include "ui_local.h"

/*
===============================================================================

CONNECTION SCREEN

===============================================================================
*/

qboolean	passwordNeeded = qtrue;
menufield_s passwordField;

static connstate_t	lastConnState;
static char			lastLoadingText[MAX_INFO_VALUE];

static void UI_ReadableSize ( char *buf, int bufsize, int value )
{
	if (value > 1024*1024*1024 ) { // gigs
		Com_sprintf( buf, bufsize, "%d", value / (1024*1024*1024) );
		Com_sprintf( buf+strlen(buf), bufsize-strlen(buf), ".%02d GB", 
			(value % (1024*1024*1024))*100 / (1024*1024*1024) );
	} else if (value > 1024*1024 ) { // megs
		Com_sprintf( buf, bufsize, "%d", value / (1024*1024) );
		Com_sprintf( buf+strlen(buf), bufsize-strlen(buf), ".%02d MB", 
			(value % (1024*1024))*100 / (1024*1024) );
	} else if (value > 1024 ) { // kilos
		Com_sprintf( buf, bufsize, "%d KB", value / 1024 );
	} else { // bytes
		Com_sprintf( buf, bufsize, "%d bytes", value );
	}
}

// Assumes time is in msec
static void UI_PrintTime ( char *buf, int bufsize, int time ) {
	time /= 1000;  // change to seconds

	if (time > 3600) { // in the hours range
		Com_sprintf( buf, bufsize, "%d hr %d min", time / 3600, (time % 3600) / 60 );
	} else if (time > 60) { // mins
		Com_sprintf( buf, bufsize, "%d min %d sec", time / 60, time % 60 );
	} else  { // secs
		Com_sprintf( buf, bufsize, "%d sec", time );
	}
}


static void UI_DisplayDownloadInfo( const char *downloadName ) {
	const char *dlText	= menu_normal_text[MNT_DOWNLOADING];
	const char *etaText	= menu_normal_text[MNT_ESTIMATEDTIMELEFT];
	const char *xferText	= menu_normal_text[MNT_TRANSFERRATE];
	const char *estimating = menu_normal_text[MNT_ESTIMATING];
	char *XofXcopied = (char *) menu_normal_text[MNT_OFCOPIED];
	char *Xcopied	= (char *) menu_normal_text[MNT_COPIED];
	char *xSeconds = (char *) menu_normal_text[MNT_SEC];

	const int dlYpos = 288;
	const int etaYpos = dlYpos + (SMALLCHAR_HEIGHT*1.25);
	const int copiedYpos = etaYpos + (SMALLCHAR_HEIGHT*1.25);
	const int xferYpos = copiedYpos + (SMALLCHAR_HEIGHT*1.25);

	int downloadSize, downloadCount, downloadTime;
	char dlSizeBuf[64], totalSizeBuf[64], xferRateBuf[64], dlTimeBuf[64];
	int xferRate;
	int width, leftWidth;
	int style = UI_LEFT|UI_SMALLFONT|UI_DROPSHADOW;
	const char *s;



	downloadSize = trap_Cvar_VariableValue( "cl_downloadSize" );
	downloadCount = trap_Cvar_VariableValue( "cl_downloadCount" );
	downloadTime = trap_Cvar_VariableValue( "cl_downloadTime" );

	leftWidth = width = UI_ProportionalStringWidth( dlText,UI_SMALLFONT  ) * UI_ProportionalSizeScale( style );
	width = UI_ProportionalStringWidth( etaText,UI_SMALLFONT  ) * UI_ProportionalSizeScale( style );
	if (width > leftWidth) leftWidth = width;
	width = UI_ProportionalStringWidth( xferText,UI_SMALLFONT  ) * UI_ProportionalSizeScale( style );
	if (width > leftWidth) leftWidth = width;
	leftWidth += 16;

	UI_DrawProportionalString( 8, dlYpos, dlText, style, color_white );
	UI_DrawProportionalString( 8, etaYpos, etaText, style, color_white );
	UI_DrawProportionalString( 8, xferYpos, xferText, style, color_white );

	if (downloadSize > 0) {
		s = va( "%s (%d%%)", downloadName, downloadCount * 100 / downloadSize );
	} else {
		s = downloadName;
	}

	UI_DrawProportionalString( leftWidth, dlYpos, s, style, color_white );

	UI_ReadableSize( dlSizeBuf,		sizeof dlSizeBuf,		downloadCount );
	UI_ReadableSize( totalSizeBuf,	sizeof totalSizeBuf,	downloadSize );

	if (downloadCount < 4096 || !downloadTime) {
		UI_DrawProportionalString( leftWidth, etaYpos, estimating, style, color_white );
		UI_DrawProportionalString( leftWidth, copiedYpos, va(XofXcopied, dlSizeBuf, totalSizeBuf), style, color_white );
	} else {
		if ((uis.realtime - downloadTime) / 1000) {
			xferRate = downloadCount / ((uis.realtime - downloadTime) / 1000);
		} else {
			xferRate = 0;
		}
		UI_ReadableSize( xferRateBuf, sizeof xferRateBuf, xferRate );

		// Extrapolate estimated completion time
		if (downloadSize && xferRate) {
			int n = downloadSize / xferRate; // estimated time for entire d/l in secs

			// We do it in K (/1024) because we'd overflow around 4MB
			UI_PrintTime ( dlTimeBuf, sizeof dlTimeBuf, (n - (((downloadCount/1024) * n) / (downloadSize/1024))) * 1000);

			UI_DrawProportionalString( leftWidth, etaYpos, dlTimeBuf, style, color_white );
			UI_DrawProportionalString( leftWidth, copiedYpos, va(XofXcopied, dlSizeBuf, totalSizeBuf), style, color_white );
		} else {
			UI_DrawProportionalString( leftWidth, etaYpos, estimating, style, color_white );
			if (downloadSize) {
				UI_DrawProportionalString( leftWidth, copiedYpos, va(XofXcopied, dlSizeBuf, totalSizeBuf), style, color_white );
			} else {
				UI_DrawProportionalString( leftWidth, copiedYpos, va(Xcopied, dlSizeBuf), style, color_white );
			}
		}

		if (xferRate) {
			UI_DrawProportionalString( leftWidth, xferYpos, va(xSeconds, xferRateBuf), style, color_white );
		}
	}
}

/*
========================
UI_DrawConnectScreen

This will also be overlaid on the cgame info screen during loading
to prevent it from blinking away too rapidly on local or lan games.
overlay is in cg_info.c, CG_DrawInformation
========================
*/
void UI_DrawConnectScreen( qboolean overlay ) {
	const char		*s;
	uiClientState_t	cstate;
	char			info[MAX_INFO_VALUE];
	int			x,strlength,length;

	Menu_Cache();

	if ( !overlay ) {
		// draw the dialog background
//		trap_R_SetColor( color_white );
//		UI_DrawHandlePic( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, uis.menuBackShader );

		trap_R_SetColor( colorTable[CT_BLACK] );
		UI_DrawHandlePic( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, uis.whiteShader );

		uis.widescreen.state = WIDESCREEN_CENTER;

		trap_R_SetColor( colorTable[CT_DKGREY] );
		UI_DrawHandlePic( 11, 60, 260, 196, uis.whiteShader );

		trap_R_SetColor( NULL );
		UI_DrawHandlePic( 13, 62, 256, 192, uis.menuBackShader );	//correct aspect

		UI_DrawProportionalString( 10, 10, menu_normal_text[MNT_HOLODECKSIMULATION], UI_BIGFONT, colorTable[CT_LTORANGE] );

		strlength = UI_ProportionalStringWidth(menu_normal_text[MNT_HOLODECKSIMULATION],UI_BIGFONT);
		length = 582 - (strlength + 6);

		trap_R_SetColor( colorTable[CT_DKORANGE]);
		UI_DrawHandlePic( 10 + strlength + 6, 11, length,  22,uis.whiteShader);
//		UI_DrawHandlePic( 224, 11, 368,  22,uis.whiteShader);
		UI_DrawHandlePic( 595, 11,  32, 32,uis.halfroundr_22);		// Right End

		trap_R_SetColor( colorTable[CT_DKPURPLE1]);
		UI_DrawHandlePic(  274+301, 232, -32,  32,uis.corner_12_18);		// LR
		UI_DrawHandlePic(  274+301,  52, -32, -32,uis.corner_12_18);		// UR

		UI_DrawHandlePic(  274,		60,		314,	 18,uis.whiteShader);	// Top
		UI_DrawHandlePic(  274,		238,	314,	 18,uis.whiteShader);	//Bottom
		UI_DrawHandlePic(  274,		75,		10,		170,uis.whiteShader);	// Left
		UI_DrawHandlePic(  274+321,	78,		12,		162,uis.whiteShader);	// Right

	}

	uis.widescreen.state = WIDESCREEN_CENTER;

	// see what information we should display
	trap_GetClientState( &cstate );

	x = 288;
	info[0] = '\0';
	if( trap_GetConfigString( CS_SERVERINFO, info, sizeof(info) ) ) 
	{
		UI_DrawProportionalString( x, 80, va( "%s",Info_ValueForKey( info, "mapname" ) ), UI_BIGFONT, colorTable[CT_WHITE] );
	}

	UI_DrawProportionalString( x, 218, va("%s %s",menu_normal_text[MNT_CONNECTINGTO],cstate.servername), UI_SMALLFONT|UI_DROPSHADOW, colorTable[CT_LTGOLD1]);
//	UI_DrawProportionalString( 320, y, menu_normal_text[MNT_PRESSESCAPETOABORT], UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, menu_text_color );

	// display global MOTD at bottom
	UI_DrawProportionalString( SCREEN_WIDTH/2, 262, 
		Info_ValueForKey( cstate.updateInfoString, "motd" ), UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, colorTable[CT_WHITE]  );
	
	// print any server info (server full, bad version, etc)
	if ( cstate.connState < CA_CONNECTED ) {
		char passFailMsg[MAX_TOKEN_CHARS];

		trap_GetConfigString( CS_CON_FAIL, passFailMsg, sizeof( passFailMsg ) );
		if ( passFailMsg[0] ) {
			UI_DrawProportionalString(320, 186, passFailMsg,
				UI_SMALLFONT|UI_DROPSHADOW, colorTable[CT_VLTGOLD1] );
			//y += PROP_HEIGHT;
		}

		UI_DrawProportionalString( x, 192, cstate.messageString, UI_SMALLFONT|UI_DROPSHADOW, colorTable[CT_LTGOLD1]  );
	}

	
#if 0
	// display password field
	if ( passwordNeeded ) {
		s_ingame_menu.x = SCREEN_WIDTH * 0.50 - 128;
		s_ingame_menu.nitems = 0;
		s_ingame_menu.wrapAround = qtrue;

		passwordField.generic.type = MTYPE_FIELD;
		passwordField.generic.name = "Password:";
		passwordField.generic.callback = 0;
		passwordField.generic.x		= 10;
		passwordField.generic.y		= 180;
		Field_Clear( &passwordField.field );
		passwordField.width = 256;
		passwordField.field.widthInChars = 16;
		Q_strncpyz( passwordField.field.buffer, Cvar_VariableString("password"), 
			sizeof(passwordField.field.buffer) );

		Menu_AddItem( &s_ingame_menu, ( void * ) &s_customize_player_action );

		MField_Draw( &passwordField );
	}
#endif

	if ( lastConnState > cstate.connState ) {
		lastLoadingText[0] = '\0';
	}
	lastConnState = cstate.connState;

	switch ( cstate.connState ) {
	case CA_CONNECTING:
		s = va("%s...%i",menu_normal_text[MNT_AWAITINGCHALLENGE], cstate.connectPacketCount);
		break;
	case CA_AUTHORIZING:
		s = va("%s...%i",menu_normal_text[MNT_AWAITINGAUTHORIZATION], cstate.connectPacketCount);
		break;
	case CA_CHALLENGING:
		s = va("%s...%i", menu_normal_text[MNT_AWAITINGCONNECTION],cstate.connectPacketCount);
		break;
	case CA_CONNECTED: {
			char downloadName[MAX_INFO_VALUE];
	
			trap_Cvar_VariableStringBuffer( "cl_downloadName", downloadName, sizeof(downloadName) );
			if (*downloadName) {
				UI_DisplayDownloadInfo( downloadName );
				return;
			}
		}
		s = menu_normal_text[MNT_AWAITINGGAMESTATE];
		break;
	case CA_LOADING:
		return;
	case CA_PRIMED:
		return;
	default:
		return;
	}

	UI_DrawProportionalString( 222, 442, s, UI_SMALLFONT|UI_DROPSHADOW, colorTable[CT_LTGOLD1]  );

	// password required / connection rejected information goes here
}


/*
===================
UI_KeyConnect
===================
*/
void UI_KeyConnect( int key ) {
	if ( key == K_ESCAPE ) {
		trap_Cmd_ExecuteText( EXEC_APPEND, "disconnect\n" );
		return;
	}
}
