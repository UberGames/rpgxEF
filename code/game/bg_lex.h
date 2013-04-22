#ifndef G_LEX_H_
#define G_LEX_H_

#include "q_shared.h"

typedef enum {
	MTYPE_VECTOR,
	MTYPE_NUMBER,
	MTYPE_STRING,
	MTYPE_KEYWORD,
	MTYPE_BRACE_OPEN,
	MTYPE_BRACE_CLOSE,
	MTYPE_SQBRACE_OPEN,
	MTYPE_SQBRACE_CLOSE,
	MTYPE_CUBRACE_OPEN,
	MTYPE_CUBRACE_CLOSE,
	MTYPE_MAX
} morphType;

typedef struct morph_s morph_t;
struct morph_s {
	morphType type;
	union val
	{
		vec3_t	vector;
		double	number;
		char*	string;
		char*	keyword;
	} value;
};

typedef struct lex_s lex_t;
struct lex_s {
	void* yylex;
	morph_t morph;
};

extern lex_t* lex_create(void);
extern morphType lex(lex_t* lexer);


#endif /* G_LEX_H_ */

