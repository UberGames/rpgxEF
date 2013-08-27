#ifndef CG_LOGGER_H_
#define CG_LOGGER_H_

#include "cg_local.h"

enum {
	LL_ALWAYS,
	LL_ERROR,
	LL_WARN,
	LL_INFO,
	LL_DEBUG,
	LL_TRACE
};

#define CG_LocLogger(X,...) _CG_LocLogger(__FILE__, __LINE__, X, __VA_ARGS__) 
void QDECL CG_Logger(int level, char* fmt, ...) __attribute__ ((format (printf, 2, 3)));
void QDECL _CG_LocLogger(const char* file, int line, int level, char* fmt, ...) __attribute__ ((format (printf, 4, 5)));

#endif /* CG_LOGGER_H_ */
