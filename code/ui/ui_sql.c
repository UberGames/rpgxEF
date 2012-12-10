/**********************************************************************
	UI_sql.C

	User interface trigger from within game
**********************************************************************/

#include "ui_local.h"

typedef struct {
	menuframework_s menu;

	char *username;
	char *password;
} sql_t;

sql_t s_sql;

#define ID_QUIT 10
#define ID_LOGIN 11

/*
=================
M_sql_Event
=================
*/
static void M_sql_Event(void *ptr, int notification) {
	int id;

	id = ((menucommon_s*)ptr)->id;

	switch (id)
	{
	case ID_QUIT:
		if(notification == QM_ACTIVATED) {
			UI_PopMenu();
		}
		break;
	case ID_LOGIN:
		if(notification == QM_ACTIVATED) {
			// do login
		}
		break;
	default:
		break;
	}

}
