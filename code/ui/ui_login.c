/**********************************************************************
	UI_LOGIN.C

	User inferface for user login (sql)
**********************************************************************/
#include "ui_local.h"

static byte sqlkey;

typedef struct {
	menuframework_s menu;
	sfxHandle_t		openingVoice;
	menubitmap_s	cancel;
	menubitmap_s	login;
	menufield_s		username;
	menufield_s		password;
} login_t;

login_t s_login;

#define ID_COMPUTERVOICE	6

#define ID_CANCEL			10
#define ID_LOGIN			100

void UI_LoginMenu_Cache (void);

/*
=================
UI_LoginSetSqlkey
=================
*/
void UI_LoginSetSqlkey(int key) {
	sqlkey = (byte)key;
}

/*
=================
M_Login_Event
=================
*/
static void M_Login_Event(void* ptr, int notification) {
	int id;

	id = ((menucommon_s*)ptr)->id;

	switch(id) {
		case ID_QUIT:
			if(notification == QM_ACTIVATED)
				UI_PopMenu();
			break;
		case ID_LOGIN:
			if(notification == QM_ACTIVATED) {
				// do login
			}
			break;
	}
}

/*
=================
LoginMenu_Key
=================
*/
sfxHandle_t LoginMenu_Key(int key) {
	return (Menu_DefaultKey(&s_login.menu, key));
}

extern qhandle_t			leftRound;
extern qhandle_t			corner_ul_24_60;
extern qhandle_t			corner_ll_12_60;

/*
=================
M_LoginMenu_Graphics
=================
*/
static void M_LoginMenu_Graphics(void) {
	// do graphic here
}

/*
===============
LoginMenu_Draw
===============
*/
static void LoginMenu_Draw(void) {
	M_LoginMenu_Graphics();

	Menu_Draw(&s_login.menu);
}

/*
===============
UI_LoginMenu_Cache
===============
*/
void UI_LoginMenu_Cache(void) {
	leftRound = trap_R_RegisterShaderNoMip("menu/common/halfroundl_24.tga");
	corner_ul_24_60 = trap_R_RegisterShaderNoMip("menu/common/corner_ul_24_60.tga");
	corner_ll_12_60 = trap_R_RegisterShaderNoMip("menu/common/corner_ll_12_60.tga");
}

/*
===============
LoginMenu_Init
===============
*/
void LoginMenu_Init(void) {
	int y, pad, x;
	int i, width;

	//init menu
}

/*
===============
UI_LoginMenu
===============
*/
void UI_LoginMenu(void) {
	memset(&s_login, 0, sizeof(s_login));

	uis.menusp = 0;

	ingameFlag = qtrue;

	Mouse_Show();

	UI_LoginMenu_Cache();

	LoginMenu_Init();

	UI_PushMenu(&s_login.menu);

	Menu_AdjustCursor(&s_login.menu, 1);
}

