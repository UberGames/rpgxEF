
#ifdef SQL

#if (defined(_MSC_VER) | defined(__WIN32__))
#include "windows.h"
#endif

#include "sqlite3.h"
#include "g_sql.h"
#include "g_local.h"
#include "md5.h"

sqlite3	*user_db;
qboolean sql_ready = qfalse;

static qboolean G_Sql_Check_StepReturn(int r) {
	switch(r) {
		case SQLITE_ROW:
		case SQLITE_DONE:
			return qfalse;
		default:
			G_Printf(S_COLOR_RED "SQL ERROR: %s\n", sqlite3_errmsg(user_db));
			break;
	}
	return qtrue;
}

static qboolean G_Sql_Check_PrepareReturn(int r) {
	switch(r) {
		case SQLITE_OK:
			return qfalse;
		default:
			G_Printf(S_COLOR_RED "SQL ERROR: %s\n", sqlite3_errmsg(user_db));
			break;
	}
	return qtrue;
}

static qboolean G_Sql_Check_BindReturn(int r) {
	switch(r) {
		case SQLITE_OK:
			return qfalse;
		default:
			G_Printf(S_COLOR_RED "SQL ERROR: %s\n", sqlite3_errmsg(user_db));
			break;
	}
	return qtrue;
}

static char *G_Sql_Md5(const char *s) {
	char *res;
	unsigned char sig[16];
	struct MD5Context md5c;

	MD5Init(&md5c);
	MD5Update(&md5c, (const unsigned char *)s, strlen(s));
	MD5Final(&md5c, sig);

	res = (char *)malloc(sizeof(char)*MAX_QPATH);
	if(!res) {
		G_Printf(S_COLOR_RED "SQL ERROR: was unable to allocate %u byte\n", sizeof(char)*(strlen(s)+1));
		return NULL;
	}

	sprintf(res, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
					sig[0], sig[1], sig[2], sig[3], sig[4], sig[5], sig[6], sig[7],
					sig[8], sig[9], sig[10], sig[11], sig[12], sig[13], sig[14], sig[15] );
	
	return res;
}

qboolean G_Sql_Init(void) {
	int res;
	sqlite3_stmt *stmt;

	if(!sql_use.integer) return qtrue;

	res = sqlite3_open(BASEPATH "/db/users.db", &user_db);
	if(res != SQLITE_OK) {
		G_Printf(S_COLOR_RED "[SQL-Error] %s\n", sqlite3_errmsg(user_db));
		return qfalse;
	}

	res = sqlite3_prepare_v2(user_db, SQL_ENABLE_FOREIGN_KEY_CONSTRAINTS, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		sqlite3_close(user_db);
		sql_ready = qfalse;
		return qfalse;
	}
	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		sqlite3_close(user_db);
		sql_ready = qfalse;
		return qfalse;
	}
	sqlite3_finalize(stmt);

	res = sqlite3_prepare_v2(user_db, SQL_USER_CREATEUSERTABLE, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		sqlite3_close(user_db);
		sql_ready = qfalse;
		return qfalse;
	}
	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		sqlite3_close(user_db);
		sql_ready = qfalse;
		return qfalse;
	}
	sqlite3_finalize(stmt);

	res = sqlite3_prepare_v2(user_db, SQL_USER_CREATERIGHTSTABLE, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		sqlite3_close(user_db);
		sql_ready = qfalse;
		return qfalse;
	}
	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		sqlite3_close(user_db);
		sql_ready = qfalse;
		return qfalse;
	}
	sqlite3_finalize(stmt);

	sql_ready = qtrue;
	return qtrue;
}

void G_Sql_Shutdown(void) {

	if(!sql_use.integer) return;
	if(sql_ready == qfalse) return;

	sql_ready = qfalse;
	sqlite3_close(user_db);
}

qboolean G_Sql_UserDB_Del(const char *uName) {
	sqlite3_stmt *stmt;
	int res, id = -1;

	if(!sql_ready) {
		G_Printf(S_COLOR_RED "[SQL-Error] SQL is not initialized!\n");
		return qfalse;
	}

	res = sqlite3_prepare_v2(user_db, "SELECT id FROM rpgx_users WHERE username = :UNAME", -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		return qfalse;
	}
	res = sqlite3_bind_text(stmt, 1, uName, sizeof(uName), SQLITE_STATIC);
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
		G_Printf("SQL: User \'%s\' not found\n", uName);
		return qfalse;
	} 
	sqlite3_finalize(stmt);

	if(id == -1) {
		G_Printf(S_COLOR_RED "[SQL-Error] No user %s found\n", uName);
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
	res = sqlite3_bind_text(stmt, 1, uName, sizeof(uName), SQLITE_STATIC);
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

qboolean G_Sql_UserDB_Add(const char *uName, const char *password) {
	sqlite3_stmt *stmt;
	int res, id;
	char *hashedpw;

	if(!sql_ready) {
		G_Printf(S_COLOR_RED "[SQL-Error] SQL is not initialized!\n");
		return qfalse;
	}

	res = sqlite3_prepare_v2(user_db, SQL_USER_GET_UID, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		return qfalse;
	}
	res = sqlite3_bind_text(stmt, 1, uName, sizeof(uName), SQLITE_STATIC);
	if(G_Sql_Check_BindReturn(res)) {
		return qfalse;
	}

	res = sqlite3_step(stmt);
	if(G_Sql_Check_StepReturn(res)) {
		return qfalse;
	}
	if(res == SQLITE_ROW) {
		G_Printf("SQL: There already exists a user with username \'%s\'\n", uName);
		G_Printf("SQL: If you lost your password please contact an admin with access to the database.\n");
		sqlite3_finalize(stmt);
		return qfalse;
	}
	sqlite3_finalize(stmt);

	hashedpw = G_Sql_Md5(password);
	res = sqlite3_prepare_v2(user_db, SQL_USER_ADD, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		return qfalse;
	}
	res = sqlite3_bind_text(stmt, 1, uName, sizeof(uName), SQLITE_STATIC);
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

	res = sqlite3_prepare_v2(user_db, SQL_USER_GET_UID, -1, &stmt, 0);
	if(G_Sql_Check_PrepareReturn(res)) {
		return qfalse;
	}
	res = sqlite3_bind_text(stmt, 1, uName, sizeof(uName), SQLITE_STATIC);
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
 		G_Printf("SQL: There already exists a user with username \'%s\'\n", uName);
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
	sqlite3_stmt *stmt;
	int res, id;
	char *hashedpw;

	if(!sql_ready) {
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

	if(!sql_ready) {
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

	sqlite3_finalize(stmt);
	return qfalse;
}

qboolean G_Sql_UserDB_AddRight(int uid, int right) {
	sqlite3_stmt *stmt;
	int res;
	long rights;

	if(!sql_ready) {
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

	if(!sql_ready) {
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

	if(!uName || !uName[0] || !sql_ready) {
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
