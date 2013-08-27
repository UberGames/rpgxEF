#ifndef UI_LOGGER_H_
#define UI_LOGGER_H_

#include "UI_local.h"

enum {
	LL_ALWAYS,
	LL_ERROR,
	LL_WARN,
	LL_INFO,
	LL_DEBUG,
	LL_TRACE
};

#define UI_LocLogger(X,...) _UI_LocLogger(__FILE__, __LINE__, X, __VA_ARGS__) 
void QDECL UI_Logger(int level, char* fmt, ...) __attribute__ ((format (printf, 2, 3)));
void QDECL _UI_LocLogger(const char* file, int line, int level, char* fmt, ...) __attribute__ ((format (printf, 4, 5)));

#endif /* UI_LOGGER_H_ */
