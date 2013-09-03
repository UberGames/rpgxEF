#ifndef G_LOGGER_H_
#define G_LOGGER_H_

#include "g_local.h"

enum {
	LL_ALWAYS,
	LL_ERROR,
	LL_WARN,
	LL_INFO,
	LL_DEBUG,
	LL_TRACE
};

#define G_LogFuncBegin() G_LocLogger(LL_TRACE, "%s - Begin\n", __FUNCTION__)
#define G_LogFuncEnd() G_LocLogger(LL_TRACE, "%s - End\n", __FUNCTION__)
#define G_LocLogger(X,...) _G_LocLogger(__FILE__, __LINE__, X, __VA_ARGS__) 
void QDECL G_Logger(int level, char* fmt, ...) __attribute__ ((format (printf, 2, 3)));
void QDECL _G_LocLogger(const char* file, int line, int level, char* fmt, ...) __attribute__ ((format (printf, 4, 5)));

#endif /* G_LOGGER_H_ */
