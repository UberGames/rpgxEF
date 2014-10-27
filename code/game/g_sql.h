
#ifndef _G_SQL_H_
#define _G_SQL_H_

enum g_sqlLimits_e {
	MAX_SQL_RESULT = 4096
};

static const char SQL_ENABLE_FOREIGN_KEY_CONSTRAINTS[] = "PRAGMA foreign_keys = ON;";

static const char SQL_BEGIN_TRANSACTION[] = "BEGIN TRANSACTION";

static const char SQL_ROLLBACK_TRANSACTION[] = "ROLLBACK TRANSACTION";

static const char SQL_COMMIT_TRANSACTION[] = "COMMIT TRANSACTION";

static const char SQL_USER_CREATEUSERTABLE[] = {
	"CREATE TABLE IF NOT EXISTS rpgx_users ( "\
	"id INTEGER PRIMARY KEY AUTOINCREMENT, "\
	"username TEXT NOT NULL , "\
	"password TEXT NOT NULL "\
	")"
};

static const char  SQL_USER_CREATERIGHTSTABLE[] = {
	"CREATE TABLE IF NOT EXISTS rpgx_userRights ( "\
	"id INT NOT NULL PRIMARY KEY, "\
	"admin BIT NOT NULL, "\
	"rights LONG NOT NULL, "\
	"FOREIGN KEY(id) REFERENCES rpgx_users(id) "\
	")"
};

static const char  SQL_USER_DELETE[] = "DELETE FROM rpgx_users WHERE username = :UNAME";

static const char  SQL_USER_DELTE_RIGHTS[] = "DELETE FROM rpgx_userRights WHERE id = :ID";

static const char  SQL_USER_ADD[] = "INSERT INTO rpgx_users VALUES(NULL,:USERNAME,:PASSWORD)";

static const char  SQL_USER_ADD_RIGHTS[] = "INSERT INTO rpgx_userRights VALUES(:ID, 0, 0)";

static const char  SQL_USER_MOD_RIGHTS[] = "UPDATE rpgx_userRights SET rights = :RIGHTS WHERE id = :ID";

static const char  SQL_USER_GET_RIGHTS[] = "SELECT rights FROM rpgx_userRights WHERE id = :ID";

static const char  SQL_USER_CHECK_ADMIN[] = "SELECT admin FROM rpgx_userRights WHERE id = :ID";

static const char  SQL_USER_GET_PASSWORD[] = "SELECT password FROM rpgx_users WHERE id = :ID";
static const char  SQL_USER_SET_PASSWORD[] = "UPDATE rpgx_users SET password = :PASSWORD WHERE id = :ID";

static const char  SQL_USER_GET_UID[] = "SELECT id FROM rpgx_users WHERE username = :UNAME";

static const char  SQL_USER_LOGIN[] = "SELECT id FROM rpgx_users WHERE username = :UNAME AND password = :PASSWORD";

static const char  SQL_STRING_CREATE_TABLE[] = {
	"CREATE TABLE IF NOT EXISTS rpgx_strings ( "\
	"id INTEGER PRIMARY KEY AUTOINCREMENT, "\
	"text TEXT NOT NULL, "\
	"text_de TEXT NOT NULL, "\
	"text_nl TEXT NOT NULL "\
	")"
};

																																																																																																																																																																																																																																																					/* Okay. I think this list is way too long and some things should be put together. Here is the original list for reference.
																																																																																																																																																																																																																																																					typedef enum {
																																																																																																																																																																																																																																																					SQLF_GIVE				= 1,
																																																																																																																																																																																																																																																					SQLF_GOD				= 2,
																																																																																																																																																																																																																																																					SQLF_NOCLIP				= 4,
																																																																																																																																																																																																																																																					SQLF_CLOAK				= 8,
																																																																																																																																																																																																																																																					SQLF_EVOSUIT			= 16,
																																																																																																																																																																																																																																																					SQLF_FLIGHT				= 32,
																																																																																																																																																																																																																																																					SQLF_FORCENAME			= 64, //now SQLF_FORCEPARM = 64
																																																																																																																																																																																																																																																					SQLF_FORCEKILL			= 128,
																																																																																																																																																																																																																																																					SQLF_KICK				= 256,
																																																																																																																																																																																																																																																					SQLF_DRAG				= 512,
																																																																																																																																																																																																																																																					SQLF_DISARM				= 1024,
																																																																																																																																																																																																																																																					SQLF_FORCERANK			= 2048, //now SQLF_FORCEPARM = 64
																																																																																																																																																																																																																																																					SQLF_REVIVE				= 4096,
																																																																																																																																																																																																																																																					SQLF_NOOB				= 8192,
																																																																																																																																																																																																																																																					SQLF_MESSAGE			= 16384,
																																																																																																																																																																																																																																																					SQLF_FORCEMODEL			= 32768, //now SQLF_FORCEPARM = 64
																																																																																																																																																																																																																																																					SQLF_MUSIC				= 65536,
																																																																																																																																																																																																																																																					SQLF_SOUND				= 131072, //now SQLF_MUSIC = 65536
																																																																																																																																																																																																																																																					SQLF_USEENT				= 262144,
																																																																																																																																																																																																																																																					SQLF_BEAM				= 524288,
																																																																																																																																																																																																																																																					SQLF_FORCEPLAYER		= 1048576, //now SQLF_FORCEPARM = 64
																																																																																																																																																																																																																																																					SQLF_FX					= 2097152,
																																																																																																																																																																																																																																																					SQLF_CHARS				= 4194304,
																																																																																																																																																																																																																																																					SQLF_CLAMP				= 8388608,
																																																																																																																																																																																																																																																					SQLF_LOCK				= 16777216,
																																																																																																																																																																																																																																																					SQLF_FFSTUFF			= 33554432,
																																																																																																																																																																																																																																																					SQLF_ALERT				= 67108864,
																																																																																																																																																																																																																																																					SQLF_TESS				= 134217728,
																																																																																																																																																																																																																																																					SQLF_FORCECLASS			= 268435456, //now SQLF_FORCEPARM = 64
																																																																																																																																																																																																																																																					SQLF_FORCEVOTE			= 536870912,
																																																																																																																																																																																																																																																					SQLF_SHAKE				= 1073741824, //now SQLF_FX = 2097152
																																																																																																																																																																																																																																																					//SQLF_ENTLIST			= 2147483648 // that is to big ... using SQLF_USEENT for entitylist as it makes sense
																																																																																																																																																																																																																																																					} sql_userflags;*/

																																																																																																																																																																																																																																																					typedef enum {
																																																																																																																																																																																																																																																						SQLF_GIVE = 1,
																																																																																																																																																																																																																																																						SQLF_GOD = 2,
																																																																																																																																																																																																																																																						SQLF_NOCLIP = 4,
																																																																																																																																																																																																																																																						SQLF_CLOAK = 8,
																																																																																																																																																																																																																																																						SQLF_EVOSUIT = 16,
																																																																																																																																																																																																																																																						SQLF_FLIGHT = 32,
																																																																																																																																																																																																																																																						SQLF_FORCEPARM = 64,
																																																																																																																																																																																																																																																						SQLF_FORCEKILL = 128,
																																																																																																																																																																																																																																																						SQLF_KICK = 256,
																																																																																																																																																																																																																																																						SQLF_DRAG = 512,
																																																																																																																																																																																																																																																						SQLF_DISARM = 1024,
																																																																																																																																																																																																																																																						SQLF_SMS = 2048, //Shiphealth, Selfdestruct and associated
																																																																																																																																																																																																																																																						SQLF_REVIVE = 4096,
																																																																																																																																																																																																																																																						SQLF_NOOB = 8192,
																																																																																																																																																																																																																																																						SQLF_MESSAGE = 16384,
																																																																																																																																																																																																																																																						SQLF_DEBUG = 32768, //Debugging Tools
																																																																																																																																																																																																																																																						SQLF_MUSIC = 65536,
																																																																																																																																																																																																																																																						//unused				= 131072,
																																																																																																																																																																																																																																																						SQLF_USEENT = 262144,
																																																																																																																																																																																																																																																						SQLF_BEAM = 524288,
																																																																																																																																																																																																																																																						//unused				= 1048576,
																																																																																																																																																																																																																																																						SQLF_FX = 2097152,
																																																																																																																																																																																																																																																						SQLF_CHARS = 4194304,
																																																																																																																																																																																																																																																						SQLF_CLAMP = 8388608,
																																																																																																																																																																																																																																																						SQLF_LOCK = 16777216,
																																																																																																																																																																																																																																																						SQLF_FFSTUFF = 33554432,
																																																																																																																																																																																																																																																						SQLF_ALERT = 67108864,
																																																																																																																																																																																																																																																						SQLF_TESS = 134217728,
																																																																																																																																																																																																																																																						//unused				= 268435456,
																																																																																																																																																																																																																																																						SQLF_FORCEVOTE = 536870912
																																																																																																																																																																																																																																																						//unused				= 1073741824
																																																																																																																																																																																																																																																						//Maximum Value
																																																																																																																																																																																																																																																					} sql_userflags;

#endif // _G_SQL_H_
