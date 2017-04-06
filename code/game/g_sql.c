
#ifdef SQL

#if (defined(_MSC_VER) | defined(__WIN32__))
#include "windows.h"
#endif

#include "sqlite3.h"
#include "g_sql.h"
#include "g_local.h"
#include "md5.h"
#include "g_logger.h"

static sqlite3 *user_db;
static sqlite3 *string_db;

static qboolean G_Sql_Check_StepReturn(int32_t r) {
	switch(r) {
		case SQLITE_ROW:
		case SQLITE_DONE:
			return qfalse;
		default:
			G_Logger(LL_ERROR, "SQL ERROR: %s\n", sqlite3_errmsg(user_db));
			break;
	}
	return qtrue;
}

static qboolean G_Sql_Check_PrepareReturn(int32_t r) {
	switch(r) {
		case SQLITE_OK:
			return qfalse;
		default:
			G_Logger(LL_ERROR, "SQL ERROR: %s\n", sqlite3_errmsg(user_db));
			break;
	}
	return qtrue;
}

static qboolean G_Sql_Check_BindReturn(int32_t r) {
	switch(r) {
		case SQLITE_OK:
			return qfalse;
		default:
			G_Logger(LL_ERROR, "SQL ERROR: %s\n", sqlite3_errmsg(user_db));
			break;
	}
	return qtrue;
}

static char* G_Sql_Md5(const char* s) {
	char* res = NULL;
	unsigned char sig[16];
	struct MD5Context md5c;

	MD5Init(&md5c);
	MD5Update(&md5c, (const unsigned char*)s, strlen(s));
	MD5Final(&md5c, sig);

	res = (char*)malloc(sizeof(char) * MAX_QPATH);
	if(res != NULL) {
		G_LocLogger(LL_DEBUG, "SQL ERROR: was unable to allocate %u byte\n", sizeof(char) * (strlen(s) + 1));
		return NULL;
	}

	sprintf(res, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
					sig[0], sig[1], sig[2], sig[3], sig[4], sig[5], sig[6], sig[7],
					sig[8], sig[9], sig[10], sig[11], sig[12], sig[13], sig[14], sig[15] );
	
	return res;
}

qboolean G_Sql_CreateStringDB(void) {
	int res = 0;
	sqlite3_stmt* stmt = NULL;

	res = sqlite3_open(BASEPATH "/db/strings.db", &string_db);
	if(res != SQLITE_OK) {
		G_LocLogger(LL_ERROR, "%s\n", sqlite3_errmsg(string_db));
		return qfalse;
	}

	res = sqlite3_prepare_v2(string_db, SQL_ENABLE_FOREIGN_KEY_CONSTRAINTS, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		sqlite3_close(string_db);
		level.sqlReady = qfalse;
		return qfalse;
	}
	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		sqlite3_close(string_db);
		level.sqlReady = qfalse;
		return qfalse;
	}
	sqlite3_finalize(stmt);

	res = sqlite3_prepare_v2(string_db, SQL_STRING_CREATE_TABLE, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		sqlite3_close(string_db);
		level.sqlReady = qfalse;
		return qfalse;
	}
	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		sqlite3_close(string_db);
		level.sqlReady = qfalse;
		return qfalse;
	}
	sqlite3_finalize(stmt);

	return qtrue;
}

qboolean G_Sql_CreateUserDB(void) {
	int res = 0;
	sqlite3_stmt* stmt = NULL;

	res = sqlite3_open(BASEPATH "/db/users.db", &user_db);
	if(res != SQLITE_OK) {
		G_LocLogger(LL_ERROR, "%s\n", sqlite3_errmsg(user_db));
		return qfalse;
	}

	res = sqlite3_prepare_v2(user_db, SQL_ENABLE_FOREIGN_KEY_CONSTRAINTS, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		sqlite3_close(user_db);
		level.sqlReady = qfalse;
		return qfalse;
	}
	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		sqlite3_close(user_db);
		level.sqlReady = qfalse;
		return qfalse;
	}
	sqlite3_finalize(stmt);

	res = sqlite3_prepare_v2(user_db, SQL_USER_CREATEUSERTABLE, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		sqlite3_close(user_db);
		level.sqlReady = qfalse;
		return qfalse;
	}
	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		sqlite3_close(user_db);
		level.sqlReady = qfalse;
		return qfalse;
	}
	sqlite3_finalize(stmt);

	res = sqlite3_prepare_v2(user_db, SQL_USER_CREATERIGHTSTABLE, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		sqlite3_close(user_db);
		level.sqlReady = qfalse;
		return qfalse;
	}
	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		sqlite3_close(user_db);
		level.sqlReady = qfalse;
		return qfalse;
	}
	sqlite3_finalize(stmt);

	return qtrue;
}

qboolean G_Sql_Init(void) {
	level.sqlReady = qfalse;

	if(sql_use.integer == 0) {
		return qtrue;
	}

	if(G_Sql_CreateUserDB() == qfalse) {
		return qfalse;
	}

	if(G_Sql_CreateStringDB() == qfalse) {
		return qfalse;
	}

	G_Logger(LL_ALWAYS, "SQL initialization complete\n");

	level.sqlReady = qtrue;
	return qtrue;
}

void G_Sql_Shutdown(void) {

	G_Logger(LL_ALWAYS, "SQL shutdown\n");

	if(sql_use.integer == 0) {
		return;
	}

	if(level.sqlReady == qfalse) {
		return;
	}

	level.sqlReady = qfalse;
	sqlite3_close(user_db);
	sqlite3_close(string_db);
}

qboolean G_Sql_UserDB_Del(const char* username) {
	sqlite3_stmt* stmt = NULL;
	int32_t res = 0;
	int32_t id = -1;

	if(username == NULL) {
		G_LocLogger(LL_ERROR, "username is NULL\n");
		return qfalse;
	}

	if(!level.sqlReady) {
		G_LocLogger(LL_ERROR, "SQL is not initialized!\n");
		return qfalse;
	}

	res = sqlite3_prepare_v2(user_db, SQL_USER_GET_UID, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		return qfalse;
	}

	res = sqlite3_bind_text(stmt, 1, username, sizeof(username), SQLITE_STATIC);
	if(G_Sql_Check_BindReturn(res)) {
		return qfalse;
	}

	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		return qfalse;
	}

	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		return qfalse;
	}

	if(res == SQLITE_ROW) {
		id = sqlite3_column_int(stmt, 0);
	} else if(res == SQLITE_DONE) {
		G_Logger(LL_ALWAYS, "SQL: User \'%s\' not found\n", username);
		return qfalse;
	} 
	sqlite3_finalize(stmt);

	if(id == -1) {
		G_LocLogger(LL_ERROR, "No user %s found\n", username);
		return qfalse;
	}

	res = sqlite3_prepare_v2(user_db, SQL_USER_DELTE_RIGHTS, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		return qfalse;
	}

	res = sqlite3_bind_int(stmt, 1, id);
	if(G_Sql_Check_BindReturn(res)) {
		return qfalse;
	}

	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		return qfalse;
	}
	sqlite3_finalize(stmt);

	res = sqlite3_prepare_v2(user_db, SQL_USER_DELETE, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		return qfalse;
	}

	res = sqlite3_bind_text(stmt, 1, username, sizeof(username), SQLITE_STATIC);
	if(G_Sql_Check_BindReturn(res)) {
		return qfalse;
	}

	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		return qfalse;
	}
	sqlite3_finalize(stmt);

	return qtrue;
}

qboolean G_Sql_UserDB_Add(const char* username, const char* password) {
	sqlite3_stmt* stmt = NULL;
	int32_t res = 0;
	int32_t id = 0;
	char* hashedpw = NULL;

	if(username == NULL) {
		G_LocLogger(LL_ERROR, "username is NULL\n");
		return qfalse;
	}

	if(password == NULL) {
		G_LocLogger(LL_ERROR, "password is NULL\n");
		return qfalse;
	}

	if(!level.sqlReady) {
		G_LocLogger(LL_ERROR, "SQL is not initialized!\n");
		return qfalse;
	}

	res = sqlite3_prepare_v2(user_db, SQL_USER_GET_UID, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		return qfalse;
	}
	res = sqlite3_bind_text(stmt, 1, username, sizeof(username), SQLITE_STATIC);
	if(G_Sql_Check_BindReturn(res)) {
		return qfalse;
	}

	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		return qfalse;
	}
	if(res == SQLITE_ROW) {
		G_Logger(LL_ALWAYS, "SQL: There already exists a user with username \'%s\'\n", username);
		G_Logger(LL_ALWAYS, "SQL: If you lost your password please contact an admin with access to the database.\n");
		sqlite3_finalize(stmt);
		return qfalse;
	}
	sqlite3_finalize(stmt);

	hashedpw = G_Sql_Md5(password);
	if(hashedpw == NULL) {
		return qfalse;
	}

	res = sqlite3_prepare_v2(user_db, SQL_USER_ADD, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		return qfalse;
	}
	res = sqlite3_bind_text(stmt, 1, username, sizeof(username), SQLITE_STATIC);
	if(G_Sql_Check_BindReturn(res)) {
		return qfalse;
	}
	res = sqlite3_bind_text(stmt, 2, hashedpw, sizeof(hashedpw), free);
	if(G_Sql_Check_BindReturn(res)) {
		return qfalse;
	}
	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		return qfalse;
	}

	if(hashedpw != NULL) {
		free(hashedpw);
	}

	res = sqlite3_prepare_v2(user_db, SQL_USER_GET_UID, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		return qfalse;
	}
	res = sqlite3_bind_text(stmt, 1, username, sizeof(username), SQLITE_STATIC);
	if(G_Sql_Check_BindReturn(res)) {
		return qfalse;
	}

	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		return qfalse;
	}
	if(res == SQLITE_ROW) {
		id = sqlite3_column_int(stmt, 0);
	} else {
		G_Logger(LL_ALWAYS, "SQL: There already exists a user with username \'%s\'\n", username);
		sqlite3_finalize(stmt);
		return qfalse;
	}
	sqlite3_finalize(stmt);

	res = sqlite3_prepare_v2(user_db, SQL_USER_ADD_RIGHTS, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		return qfalse;
	}
	res = sqlite3_bind_int(stmt, 1, id);
	if(G_Sql_Check_BindReturn(res)) {
		return qfalse;
	}
	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		return qfalse;
	}
	sqlite3_finalize(stmt);

	return qtrue;
}

qboolean G_Sql_UserDB_Login(const char *uName, const char *pwd, int clientnum) {
	sqlite3_stmt* stmt = NULL;
	int res, id;
	char *hashedpw;

	if(!level.sqlReady) {
		G_Printf(S_COLOR_RED "[SQL-Error] SQL is not initialized!\n");
		return qfalse;
	}

	res = sqlite3_prepare_v2(user_db, SQL_USER_LOGIN, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		return qfalse;
	}

	res = sqlite3_bind_text(stmt, 1, uName, sizeof(uName), SQLITE_STATIC);
	if(G_Sql_Check_BindReturn(res)) {
		return qfalse;
	}

	hashedpw = G_Sql_Md5(pwd);
	res = sqlite3_bind_text(stmt, 2, hashedpw, sizeof(hashedpw), SQLITE_STATIC);
	if(G_Sql_Check_BindReturn(res)) {
		return qfalse;
	}

	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		return qfalse;
	}

	if(hashedpw != NULL) {
		free(hashedpw);
	}

	if(res == SQLITE_ROW) {
		id = sqlite3_column_int(stmt, 0);
		level.clients[clientnum].uid = id;
	} else {
		return qfalse;
	}

	res = sqlite3_prepare_v2(user_db, SQL_USER_CHECK_ADMIN, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		level.clients[clientnum].uid = -1;
		return qfalse;
	}

	res = sqlite3_bind_int(stmt, 1, id);
	if(G_Sql_Check_BindReturn(res)) {
		level.clients[clientnum].uid = -1;
		return qfalse;
	}

	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		level.clients[clientnum].uid = -1;
		return qfalse;
	}

	if(res == SQLITE_ROW) {
		level.clients[clientnum].LoggedAsAdmin = qtrue;
	} else {
		level.clients[clientnum].uid = -1;
		return qfalse;
	}


	return qtrue;
}

qboolean G_Sql_UserDB_CheckRight(int uid, int right) {
	sqlite3_stmt *stmt;
	int res;
	long rights;

	if(!level.sqlReady) {
		G_Printf(S_COLOR_RED "[SQL-Error] SQL is not initialized!\n");
		return qfalse;
	}

	res = sqlite3_prepare_v2(user_db, SQL_USER_GET_RIGHTS, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		return qfalse;
	}

	res = sqlite3_bind_int(stmt, 1, uid);
	if(G_Sql_Check_BindReturn(res)) {
		return qfalse;
	}

	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		return qfalse;
	}

	if(res == SQLITE_ROW) {
		rights = (long)sqlite3_column_int64(stmt, 0);
		if(right & right) {
			sqlite3_finalize(stmt);
			return qtrue;
		} else {
			sqlite3_finalize(stmt);
			return qfalse;
		}
	} else {
		sqlite3_finalize(stmt);
		return qfalse;
	}
}

qboolean G_Sql_UserDB_AddRight(int uid, int right) {
	sqlite3_stmt *stmt;
	int res;
	long rights;

	if(!level.sqlReady) {
		G_Printf(S_COLOR_RED "[SQL-Error] SQL is not initialized!\n");
		return qfalse;
	}

	res = sqlite3_prepare_v2(user_db, SQL_USER_GET_RIGHTS, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		return qfalse;
	}

	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		return qfalse;
	}

	if(res == SQLITE_ROW) {
		rights = (long)sqlite3_column_int64(stmt, 0);
	} else {
		return qfalse;
	}
	sqlite3_finalize(stmt);

	rights |= right;

	res = sqlite3_prepare_v2(user_db, SQL_USER_MOD_RIGHTS, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		return qfalse;
	}

	res = sqlite3_bind_int(stmt, 1, rights);
	if(G_Sql_Check_BindReturn(res)) {
		return qfalse;
	}

	res = sqlite3_bind_int(stmt, 2, uid);
	if(G_Sql_Check_BindReturn(res)) {
		return qfalse;
	}

	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		return qfalse;
	}

	sqlite3_finalize(stmt);
	return qtrue;
}

qboolean G_Sql_UserDB_RemoveRight(int uid, int right) {
	sqlite3_stmt *stmt;
	int res;
	long rights;

	if(!level.sqlReady) {
		G_Printf(S_COLOR_RED "[SQL-Error] SQL is not initialized!\n");
		return qfalse;
	}

	res = sqlite3_prepare_v2(user_db, SQL_USER_MOD_RIGHTS, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		return qfalse;
	}

	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		return qfalse;
	}
	
	if(res == SQLITE_ROW) {
		rights = (long)sqlite3_column_int64(stmt, 0);
	} else {
		return qfalse;
	}
	sqlite3_finalize(stmt);

	rights &= right;

	res = sqlite3_prepare_v2(user_db, SQL_USER_MOD_RIGHTS, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		return qfalse;
	}

	res = sqlite3_bind_int(stmt, 1, rights);
	if(G_Sql_Check_BindReturn(res)) {
		return qfalse;
	}

	res = sqlite3_bind_int(stmt, 2, uid);
	if(G_Sql_Check_BindReturn(res)) {
		return qfalse;
	}

	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		return qfalse;
	}

	sqlite3_finalize(stmt);
	return qtrue;	
}

int	G_Sql_UserDB_GetUID(const char *uName) {
	sqlite3_stmt *stmt;
	int res;
	int uid;

	if(!uName || !uName[0] || !level.sqlReady) {
		return -1;
	}

	res = sqlite3_prepare_v2(user_db, SQL_USER_GET_UID, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		return -1;
	}

	res = sqlite3_bind_text(stmt, 1, uName, sizeof(uName), SQLITE_STATIC);
	if(G_Sql_Check_BindReturn(res)) {
		return -1;
	}

	res = sqlite3_step(stmt);
	if(res == SQLITE_ROW) {
		uid = sqlite3_column_int(stmt, 0);
	} else {
		return -1;
	}

	sqlite3_finalize(stmt);
	return uid;
}

#endif //SQL
