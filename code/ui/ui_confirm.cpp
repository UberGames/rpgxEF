// Copyright (C) 1999-2000 Id Software, Inc.
//
/*
=======================================================================

CONFIRMATION MENU

=======================================================================
*/


#include "ui_local.h"
#include "ui_logger.h"

enum ui_confirmIDs_e {
	ID_CONFIRM_NO = 10,
	ID_CONFIRM_YES
};


typedef struct
{
	menuframework_s menu;

	menubitmap_s	no;
	menubitmap_s	yes;

	/*const*/ char * question;
	void(*draw)(void);
	void(*action)(qboolean result);
	qhandle_t		cornerPic;
} confirmMenu_t;


static confirmMenu_t	s_confirm;


/*
=================
ConfirmMenu_Event
=================
*/
static void ConfirmMenu_Event(void* ptr, int32_t event)
{
	UI_LogFuncBegin();
	qboolean	result;

	if (event != QM_ACTIVATED)
	{
		UI_LogFuncEnd();
		return;
	}

	UI_PopMenu();

	if (((menucommon_s*)ptr)->id == ID_CONFIRM_NO)
	{
		result = qfalse;
	}
	else
	{
		result = qtrue;
	}

	if (s_confirm.action)
	{
		s_confirm.action(result);
	}
	UI_LogFuncEnd();
}


/*
=================
ConfirmMenu_Key
=================
*/
static sfxHandle_t ConfirmMenu_Key(int32_t key)
{
	UI_LogFuncBegin();
	switch (key)
	{
	case K_KP_LEFTARROW:
	case K_LEFTARROW:
	case K_KP_RIGHTARROW:
	case K_RIGHTARROW:
		key = K_TAB;
		break;

	case 'n':
	case 'N':
		ConfirmMenu_Event(&s_confirm.no, QM_ACTIVATED);
		break;

	case 'y':
	case 'Y':
		ConfirmMenu_Event(&s_confirm.yes, QM_ACTIVATED);
		break;
	}

	UI_LogFuncEnd();
	return Menu_DefaultKey(&s_confirm.menu, key);
}


/*
=================
ConfirmMenu_Draw
=================
*/
static void ConfirmMenu_Draw(void)
{
	UI_LogFuncBegin();
	UI_MenuFrame(&s_confirm.menu);

	// End of upper line
	trap_R_SetColor(colorTable[CT_LTBROWN1]);
	UI_DrawHandlePic(482, 136, MENU_BUTTON_MED_WIDTH - 22, MENU_BUTTON_MED_HEIGHT, uis.whiteShader);
	UI_DrawHandlePic(460 + MENU_BUTTON_MED_WIDTH - 6, 136, -19, MENU_BUTTON_MED_HEIGHT, uis.graphicButtonLeftEnd);	//right

	trap_R_SetColor(colorTable[CT_DKBLUE2]);
	UI_DrawHandlePic(30, 203, 47, 186, uis.whiteShader);	// Left hand column

	UI_DrawProportionalString(74, 66, "286", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 84, "386", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 188, "486", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 207, "8088", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
	UI_DrawProportionalString(74, 395, "12799", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);


	trap_R_SetColor(colorTable[CT_LTBLUE1]);
	UI_DrawHandlePic(132, 142, 128, -64, s_confirm.cornerPic);	// Top Left corner
	UI_DrawHandlePic(132, 352, 128, 64, s_confirm.cornerPic);	// Bottom Left corner

	UI_DrawHandlePic(429, 142, -128, -64, s_confirm.cornerPic);	// Top Right corner
	UI_DrawHandlePic(429, 352, -128, 64, s_confirm.cornerPic);	// Bottom Right corner

	UI_DrawHandlePic(145, 175, 395, 18, uis.whiteShader);		// Top
	UI_DrawHandlePic(132, 193, 47, 175, uis.whiteShader);		// Left side
	UI_DrawHandlePic(510, 193, 47, 175, uis.whiteShader);		// Right side
	UI_DrawHandlePic(147, 365, 65, 18, uis.whiteShader);		// Bottom Left
	UI_DrawHandlePic(477, 365, 65, 18, uis.whiteShader);		// Bottom Right

	//TiM - if message is a long one, break it up and display it on separate lines
	if (strlen(s_confirm.question) < 45)
	{
		UI_DrawProportionalString(345, 269, s_confirm.question, UI_SMALLFONT | UI_CENTER, color_red);
	}
	else
	{
		int32_t		numLines;
		char	*message;
		char	buffer[46];
		int32_t		height;
		int32_t		y;
		int32_t		i, j;
		int32_t		spacePoint = 45;

		message = s_confirm.question;

		numLines = strlen(message) / 45 + 1;
		height = (SMALLCHAR_HEIGHT + 4) * numLines;
		y = 277 - (height * 0.5); //269

		//UI_Logger( LL_DEBUG, "numLines: %i\n", numLines );

		for (i = 0; i < numLines; i++)
		{
			if (!message)
				break;

			if (strlen(message) >= 45)
			{
				for (j = 1; j <= 45; j++) {
					if (message[j] == ' ')
						spacePoint = j;
				}
			}
			else
				spacePoint = strlen(message);

			Q_strncpyz(buffer, message, spacePoint + 1);
			//UI_Logger( LL_DEBUG, "%s\n", buffer );

			UI_DrawProportionalString(345, y, buffer, UI_SMALLFONT | UI_CENTER, color_red);

			y += SMALLCHAR_HEIGHT + 4;
			message += spacePoint;
		}
	}

	Menu_Draw(&s_confirm.menu);

	if (s_confirm.draw)
	{
		s_confirm.draw();
	}
	UI_LogFuncEnd();
}


/*
=================
ConfirmMenu_Cache
=================
*/
void ConfirmMenu_Cache(void)
{
	UI_LogFuncBegin();
	s_confirm.cornerPic = trap_R_RegisterShaderNoMip("menu/common/corner_ll_47_18.tga");
	UI_LogFuncEnd();
}

static const char* TranslateQuestion(const char *question) {
	UI_LogFuncBegin();
	static char translatedQuestion[128];

	if (!strcmp(question, "Unpure client detected. Invalid .PK3 files referenced!")) {
		UI_LogFuncEnd();
		return menu_normal_text[MNT_UNPURECLIENT];
	}

	if (!strcmp(question, "Cannot validate pure client!")) {
		UI_LogFuncEnd();
		return menu_normal_text[MNT_CANNOTVALIDATE];
	}

	if (!strcmp(question, "kicked")) {
		UI_LogFuncEnd();
		return menu_normal_text[MNT_KICKED];
	}

	if (!strcmp(question, "timed out")) {
		UI_LogFuncEnd();
		return menu_normal_text[MNT_TIMEDOUT];
	}

	if (!strcmp(question, "server shut down")) {
		UI_LogFuncEnd();
		return menu_normal_text[MNT_SERVERSHUTDOWN];
	}

	if (!strcmp(question, "disconnected")) {
		UI_LogFuncEnd();
		return menu_normal_text[MNT_DISCONNECTED];
	}

	if (!strcmp(question, "broken download")) {
		UI_LogFuncEnd();
		return menu_normal_text[MNT_BROKENDOWNLOAD];
	}

	if (!strcmp(question, "Server command overflow")) {
		UI_LogFuncEnd();
		return menu_normal_text[MNT_SERVERCOMMANDOVERFLOW];
	}

	if (!strcmp(question, "Lost reliable commands")) {
		UI_LogFuncEnd();
		return menu_normal_text[MNT_LOSTRELIABLECOMMANDS];
	}

	Q_strncpyz(translatedQuestion, question, sizeof(translatedQuestion));
	UI_LogFuncEnd();
	return translatedQuestion;
}

/*
=================
UI_ConfirmMenu
=================
*/
void UI_ConfirmMenu(const char *question, void(*draw)(void), void(*action)(qboolean result))
{
	UI_LogFuncBegin();

	// zero set all our globals
	memset(&s_confirm, 0, sizeof(s_confirm));

	ConfirmMenu_Cache();

	Mouse_Show();

	s_confirm.question = (char *)TranslateQuestion(question);
	s_confirm.draw = draw;
	s_confirm.action = action;

	s_confirm.menu.draw = ConfirmMenu_Draw;
	s_confirm.menu.key = ConfirmMenu_Key;
	s_confirm.menu.wrapAround = qtrue;
	s_confirm.menu.fullscreen = qtrue;
	s_confirm.menu.descX = MENU_DESC_X;
	s_confirm.menu.descY = MENU_DESC_Y;
	s_confirm.menu.titleX = MENU_TITLE_X;
	s_confirm.menu.titleY = MENU_TITLE_Y;
	s_confirm.menu.titleI = MNT_CONFIRMATIONMENU_TITLE;
	s_confirm.menu.footNoteEnum = MNT_CONFIRMATION;

	s_confirm.yes.generic.type = MTYPE_BITMAP;
	s_confirm.yes.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_confirm.yes.generic.x = 215;
	s_confirm.yes.generic.y = 365;
	s_confirm.yes.generic.callback = ConfirmMenu_Event;
	s_confirm.yes.generic.id = ID_CONFIRM_YES;
	s_confirm.yes.generic.name = GRAPHIC_SQUARE;
	s_confirm.yes.width = MENU_BUTTON_MED_WIDTH;
	s_confirm.yes.height = MENU_BUTTON_MED_HEIGHT;
	s_confirm.yes.color = CT_DKPURPLE1;
	s_confirm.yes.color2 = CT_LTPURPLE1;
	s_confirm.yes.textX = MENU_BUTTON_TEXT_X;
	s_confirm.yes.textY = MENU_BUTTON_TEXT_Y;
	s_confirm.yes.textEnum = MBT_GENERIC_YES;
	s_confirm.yes.textcolor = CT_BLACK;
	s_confirm.yes.textcolor2 = CT_WHITE;

	s_confirm.no.generic.type = MTYPE_BITMAP;
	s_confirm.no.generic.flags = QMF_HIGHLIGHT_IF_FOCUS;
	s_confirm.no.generic.x = 371;
	s_confirm.no.generic.y = 365;
	s_confirm.no.generic.callback = ConfirmMenu_Event;
	s_confirm.no.generic.id = ID_CONFIRM_NO;
	s_confirm.no.generic.name = GRAPHIC_SQUARE;
	s_confirm.no.width = 103;
	s_confirm.no.height = MENU_BUTTON_MED_HEIGHT;
	s_confirm.no.color = CT_DKPURPLE1;
	s_confirm.no.color2 = CT_LTPURPLE1;
	s_confirm.no.textX = MENU_BUTTON_TEXT_X;
	s_confirm.no.textY = MENU_BUTTON_TEXT_Y;
	s_confirm.no.textEnum = MBT_GENERIC_NO;
	s_confirm.no.textcolor = CT_BLACK;
	s_confirm.no.textcolor2 = CT_WHITE;

	Menu_AddItem(&s_confirm.menu, &s_confirm.yes);
	if (action) {
		Menu_AddItem(&s_confirm.menu, &s_confirm.no);
	}

	UI_PushMenu(&s_confirm.menu);

	Menu_SetCursorToItem(&s_confirm.menu, &s_confirm.no);

	UI_LogFuncEnd();
}
