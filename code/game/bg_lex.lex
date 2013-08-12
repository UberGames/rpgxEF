%top {
#include "q_shared.h"
#include "bg_lex.h"

#define YY_FATAL_ERROR(msg) bgLexFatalError(msg, yyscanner)
#define UNUSED(x) ((void)(x))
}

DIGIT [0-9]
INT "-"?{DIGIT}+
DOUBLE "-"?{DIGIT}+"."?{DIGIT}*
KEYWORD [a-zA-Z]+[a-zA-Z0-9]*

%option reentrant
%option noyywrap
%option extra-type="bgLexMorphem*"

%%
\"[^\"]*\" {
	char *s = yytext; s++;
	yyextra->type = LMT_STRING;
	yyextra->data.str = malloc(strlen(yytext) - 1);
	memset(yyextra->data.str, 0, strlen(yytext) - 1);
	strncpy(yyextra->data.str, s, strlen(yytext) - 2);
	yyextra->column += strlen(yytext);
	return LMT_STRING;
}
{INT} {
	yyextra->type = LMT_INT;
	yyextra->data.numInteger = atoi(yytext);
	yyextra->column += strlen(yytext);
	return LMT_INT;
}
{DOUBLE} {
	yyextra->type = LMT_DOUBLE;
	yyextra->data.numDouble = atof(yytext);
	yyextra->column += strlen(yytext);
	return LMT_DOUBLE;
}
{KEYWORD} {
	yyextra->type = LMT_SYMBOL;
	yyextra->data.symbol = bgLex_textToSymbol(yytext);
	yyextra->column += strlen(yytext);
	return LMT_SYMBOL;
}
"{"" "+{INT}" "+{INT}" "+{INT}" "+"}" {
	double a, b, c;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf }", &a, &b, &c);
	yyextra->data.vector3[0] = a;
	yyextra->data.vector3[1] = b;
	yyextra->data.vector3[2] = c;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR3;
	return LMT_VECTOR3;
}

"{"" "+{INT}" "+{INT}" "+{DOUBLE}" "+"}" {
	double a, b, c;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf }", &a, &b, &c);
	yyextra->data.vector3[0] = a;
	yyextra->data.vector3[1] = b;
	yyextra->data.vector3[2] = c;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR3;
	return LMT_VECTOR3;
}
"{"" "+{INT}" "+{DOUBLE}" "+{INT}" "+"}" {
	double a, b, c;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf }", &a, &b, &c);
	yyextra->data.vector3[0] = a;
	yyextra->data.vector3[1] = b;
	yyextra->data.vector3[2] = c;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR3;
	return LMT_VECTOR3;
}
"{"" "+{INT}" "+{DOUBLE}" "+{DOUBLE}" "+"}" {
	double a, b, c;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf }", &a, &b, &c);
	yyextra->data.vector3[0] = a;
	yyextra->data.vector3[1] = b;
	yyextra->data.vector3[2] = c;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR3;
	return LMT_VECTOR3;
}
"{"" "+{DOUBLE}" "+{INT}" "+{INT}" "+"}" {
	double a, b, c;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf }", &a, &b, &c);
	yyextra->data.vector3[0] = a;
	yyextra->data.vector3[1] = b;
	yyextra->data.vector3[2] = c;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR3;
	return LMT_VECTOR3;
}
"{"" "+{DOUBLE}" "+{INT}" "+{DOUBLE}" "+"}" {
	double a, b, c;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf }", &a, &b, &c);
	yyextra->data.vector3[0] = a;
	yyextra->data.vector3[1] = b;
	yyextra->data.vector3[2] = c;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR3;
	return LMT_VECTOR3;
}
"{"" "+{DOUBLE}" "+{DOUBLE}" "+{INT}" "+"}" {
	double a, b, c;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf }", &a, &b, &c);
	yyextra->data.vector3[0] = a;
	yyextra->data.vector3[1] = b;
	yyextra->data.vector3[2] = c;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR3;
	return LMT_VECTOR3;
}
"{"" "+{DOUBLE}" "+{DOUBLE}" "+{DOUBLE}" "+"}" {
	double a, b, c;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf }", &a, &b, &c);
	yyextra->data.vector3[0] = a;
	yyextra->data.vector3[1] = b;
	yyextra->data.vector3[2] = c;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR3;
	return LMT_VECTOR3;
}
"{"" "+{INT}" "*","" "+{INT}" "*","" "+{INT}" "+"}" {
	double a, b, c;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf }", &a, &b, &c);
	yyextra->data.vector3[0] = a;
	yyextra->data.vector3[1] = b;
	yyextra->data.vector3[2] = c;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR3;
	return LMT_VECTOR3;
}

"{"" "+{INT}" "*","" "+{INT}" "*","" "+{DOUBLE}" "+"}" {
	double a, b, c;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf }", &a, &b, &c);
	yyextra->data.vector3[0] = a;
	yyextra->data.vector3[1] = b;
	yyextra->data.vector3[2] = c;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR3;
	return LMT_VECTOR3;
}
"{"" "+{INT}" "*","" "+{DOUBLE}" "*","" "+{INT}" "+"}" {
	double a, b, c;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf }", &a, &b, &c);
	yyextra->data.vector3[0] = a;
	yyextra->data.vector3[1] = b;
	yyextra->data.vector3[2] = c;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR3;
	return LMT_VECTOR3;
}
"{"" "+{INT}" "*","" "+{DOUBLE}" "*","" "+{DOUBLE}" "+"}" {
	double a, b, c;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf }", &a, &b, &c);
	yyextra->data.vector3[0] = a;
	yyextra->data.vector3[1] = b;
	yyextra->data.vector3[2] = c;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR3;
	return LMT_VECTOR3;
}
"{"" "+{DOUBLE}" "*","" "+{INT}" "*","" "+{INT}" "+"}" {
	double a, b, c;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf }", &a, &b, &c);
	yyextra->data.vector3[0] = a;
	yyextra->data.vector3[1] = b;
	yyextra->data.vector3[2] = c;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR3;
	return LMT_VECTOR3;
}
"{"" "+{DOUBLE}" "*","" "+{INT}" "*","" "+{DOUBLE}" "+"}" {
	double a, b, c;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf }", &a, &b, &c);
	yyextra->data.vector3[0] = a;
	yyextra->data.vector3[1] = b;
	yyextra->data.vector3[2] = c;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR3;
	return LMT_VECTOR3;
}
"{"" "+{DOUBLE}" "*","" "+{DOUBLE}" "*","" "+{INT}" "+"}" {
	double a, b, c;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf }", &a, &b, &c);
	yyextra->data.vector3[0] = a;
	yyextra->data.vector3[1] = b;
	yyextra->data.vector3[2] = c;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR3;
	return LMT_VECTOR3;
}
"{"" "+{DOUBLE}" "*","" "+{DOUBLE}" "*","" "+{DOUBLE}" "+"}" {
	double a, b, c;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf }", &a, &b, &c);
	yyextra->data.vector3[0] = a;
	yyextra->data.vector3[1] = b;
	yyextra->data.vector3[2] = c;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR3;
	return LMT_VECTOR3;
}
"{"" "+{INT}" "+{INT}" "+{INT}" "+{INT}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{INT}" "+{INT}" "+{INT}" "+{DOUBLE}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{INT}" "+{INT}" "+{DOUBLE}" "+{INT}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{INT}" "+{INT}" "+{DOUBLE}" "+{DOUBLE}" "+"}" { 
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{INT}" "+{DOUBLE}" "+{INT}" "+{INT}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{INT}" "+{DOUBLE}" "+{INT}" "+{DOUBLE}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{INT}" "+{DOUBLE}" "+{DOUBLE}" "+{INT}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{INT}" "+{DOUBLE}" "+{DOUBLE}" "+{DOUBLE}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{DOUBLE}" "+{INT}" "+{INT}" "+{INT}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{DOUBLE}" "+{INT}" "+{INT}" "+{DOUBLE}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{DOUBLE}" "+{INT}" "+{DOUBLE}" "+{INT}" "+"}" { 
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{DOUBLE}" "+{INT}" "+{DOUBLE}" "+{DOUBLE}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{DOUBLE}" "+{DOUBLE}" "+{INT}" "+{INT}" "+"}" { 
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{DOUBLE}" "+{DOUBLE}" "+{INT}" "+{DOUBLE}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{DOUBLE}" "+{DOUBLE}" "+{DOUBLE}" "+{INT}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{DOUBLE}" "+{DOUBLE}" "+{DOUBLE}" "+{DOUBLE}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf %lf %lf %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{INT}" "*","" "+{INT}" "*","" "+{INT}" "*","" "+{INT}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf, %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{INT}" "*","" "+{INT}" "*","" "+{INT}" "*","" "+{DOUBLE}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf, %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{INT}" "*","" "+{INT}" "*","" "+{DOUBLE}" "*","" "+{INT}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf, %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{INT}" "*","" "+{INT}" "*","" "+{DOUBLE}" "*","" "+{DOUBLE}" "+"}" { 
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf, %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{INT}" "*","" "+{DOUBLE}" "*","" "+{INT}" "*","" "+{INT}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf, %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{INT}" "*","" "+{DOUBLE}" "*","" "+{INT}" "*","" "+{DOUBLE}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf, %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{INT}" "*","" "+{DOUBLE}" "*","" "+{DOUBLE}" "*","" "+{INT}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf, %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{INT}" "*","" "+{DOUBLE}" "*","" "+{DOUBLE}" "*","" "+{DOUBLE}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf, %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{DOUBLE}" "*","" "+{INT}" "*","" "+{INT}" "*","" "+{INT}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf, %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{DOUBLE}" "*","" "+{INT}" "*","" "+{INT}" "*","" "+{DOUBLE}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf, %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{DOUBLE}" "*","" "+{INT}" "*","" "+{DOUBLE}" "*","" "+{INT}" "+"}" { 
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf, %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{DOUBLE}" "*","" "+{INT}" "*","" "+{DOUBLE}" "*","" "+{DOUBLE}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf, %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{DOUBLE}" "*","" "+{DOUBLE}" "*","" "+{INT}" "*","" "+{INT}" "+"}" { 
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf, %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{DOUBLE}" "*","" "+{DOUBLE}" "*","" "+{INT}" "*","" "+{DOUBLE}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf, %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{DOUBLE}" "*","" "+{DOUBLE}" "*","" "+{DOUBLE}" "*","" "+{INT}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf, %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{"" "+{DOUBLE}" "*","" "+{DOUBLE}" "*","" "+{DOUBLE}" "*","" "+{DOUBLE}" "+"}" {
	double a, b, c, d;

	// cppcheck-suppress "invalidscanf"
	sscanf(yytext, "{ %lf, %lf, %lf, %lf }", &a, &b, &c, &d);
	yyextra->data.vector4[0] = a;
	yyextra->data.vector4[1] = b;
	yyextra->data.vector4[2] = c;
	yyextra->data.vector4[3] = d;
	yyextra->column += strlen(yytext);
	yyextra->type = LMT_VECTOR4;
	return LMT_VECTOR4;
}
"{" {
	yyextra->type = LMT_SYMBOL;
	yyextra->data.symbol = LSYM_OBRACEC;
	yyextra->column++;
	return LMT_SYMBOL;
}
"}" {
	yyextra->type = LMT_SYMBOL;
	yyextra->data.symbol = LSYM_CBRACEC;
	yyextra->column++;
	return LMT_SYMBOL;
}
"(" {
	yyextra->type = LMT_SYMBOL;
	yyextra->data.symbol = LSYM_OBRACE;
	yyextra->column++;
	return LMT_SYMBOL;
}
")" {
	yyextra->type = LMT_SYMBOL;
	yyextra->data.symbol = LSYM_CBRACE;
	yyextra->column++;
	return LMT_SYMBOL;
}
"[" {
	yyextra->type = LMT_SYMBOL;
	yyextra->data.symbol = LSYM_OBRACESQ;
	yyextra->column++;
	return LMT_SYMBOL;
}
"]" {
	yyextra->type = LMT_SYMBOL;
	yyextra->data.symbol = LSYM_CBRACESQ;
	yyextra->column++;
	return LMT_SYMBOL;
}
";" {
	yyextra->type = LMT_SYMBOL;
	yyextra->data.symbol = LSYM_SEMICOLON;
	yyextra->column++;
	return LMT_SYMBOL;
}
[ \t] {
	yyextra->column++;
	yyextra->type = LMT_IGNORE;
	return LMT_IGNORE;
}
\n {
	yyextra->line++;
	yyextra->column = 0;
	yyextra->type = LMT_IGNORE;
	return LMT_IGNORE;
}
. {
	yyextra->column++;
	yyextra->type = LMT_IGNORE;
	return LMT_IGNORE;
}
%%

#ifdef LEX_TEXT
int main(int argc, char* argv[]) {
	char buff[4096];
	yyscan_t scanner;
	FILE* f;
	bgLex* lex;

	f = fopen("test.txt", "r");
	fread(buff, 4096, 1, f);
	fclose(f);
  
	lex = bgLex_create(buff);
	while(bgLex_lex(lex)) {
		printf("type=%i symbol=%i yytext=%s\n", lex->morphem.type, lex->morphem.data.symbol, yyget_text(lex->lex));
	}
	bgLex_destroy(lex);

	return EXIT_SUCCESS;
}
#endif

bgLex* bgLex_create(char* data) {
	bgLex* l = malloc(sizeof(bgLex));

	/* HACK HACK HACK ... get rid of some compiler warnings */
	UNUSED(yyunput);
	UNUSED(input);

	if(l != NULL) {
		l->morphem = malloc(sizeof(bgLexMorphemData));

		if(l->morphem == NULL) {
			free(l);
			return NULL;
		}

		l->morphem->line = 0;
		l->morphem->column = 0;

		yylex_init_extra(l->morphem, &l->lex);
		l->buf = yy_scan_string(data,l->lex);
	}

	return l;
}

void bgLex_destroy(bgLex* lex) {
	if(lex == NULL) {
		return;
	}

	if(lex->buf != NULL) {
		yy_delete_buffer(lex->buf, lex->lex);
	}

	if(lex->lex != NULL) {
		yylex_destroy(lex->lex);
	}

	if(lex->morphem != NULL) {
		free(lex->morphem);
	}

	free(lex);
}

int bgLex_lex(bgLex* lex) {
	int res;

	if(lex->morphem->data.str != NULL) {
		free(lex->morphem->data.str);
	}

	/* skip LMT_IGNORE */
	while(1) {
		res = yylex(lex->lex);
		if(lex->morphem->type != LMT_IGNORE) {
			break;
		}
	}

	return res;
}

bgLexSymbol bgLex_textToSymbol(char* text) {
	if(strcmp("ServerChangeConfig", text) == 0) {
		return LSYM_SERVER_CHANGE_CONFIG;
	}

	if(strcmp("Server", text) == 0) {
		return LSYM_SERVER;
	}

	if(strcmp("HolodeckData", text) == 0) {
		return LSYM_HOLODECK_DATA;
	}

	if(strcmp("Program", text) == 0) {
		return LSYM_PROGRAM;
	}

	if(strcmp("MapChangeConfig", text) == 0) {
		return LSYM_MAP_CHANGE_CONFIG;
	}

	if(strcmp("Map", text) == 0) {
		return LSYM_MAP;
	}

	if(strcmp("LocationsList2", text) == 0) {
		return LSYM_LOCATIONS_LIST_2;
	}

	if(strcmp("LocationsList", text) == 0) {
		return LSYM_LOCATIONS_LIST;
	}

	if(strcmp("message", text) == 0) {
		return LSYM_MESSAGE;
	}

	if(strcmp("consoleName", text) == 0) {
		return LSYM_CONSOLE_NAME;
	}

	if(strcmp("formalName", text) == 0) {
		return LSYM_FORMAL_NAME;
	}

	if(strcmp("radarColor", text) == 0) {
		return LSYM_RADAR_COLOR;
	}

	if(strcmp("iconColor", text) == 0) {
		return LSYM_ICON_COLOR;
	}

	if(strcmp("NULL", text) == 0) {
		return LSYM_NULL;
	}

	if(strcmp("modelSkin", text) == 0) {
		return LSYM_MODEL_SKIN;
	}

	if(strcmp("weapons", text) == 0) {
		return LSYM_WEAPONS;
	}

	if(strcmp("hasRanks", text) == 0) {
		return LSYM_HAS_RANKS;
	}

	if(strcmp("teal", text) == 0) {
		return LSYM_TEAL;
	}

	if(strcmp("medical", text) == 0) {
		return LSYM_MEDICAL;
	}

	if(strcmp("gold", text) == 0) {
		return LSYM_GOLD;
	}

	if(strcmp("green", text) == 0) {
		return LSYM_GREEN;
	}

	if(strcmp("marine", text) == 0) {
		return LSYM_MARINE;
	}

	if(strcmp("red", text) == 0) {
		return LSYM_RED;
	}

	if(strcmp("WP_TRICORDER", text) == 0) {
		return LSYM_WP_TRICORDER;
	}

	if(strcmp("WP_PADD", text) == 0) {
		return LSYM_WP_PADD;
	}

	if(strcmp("WP_COFFEE", text) == 0) {
    return LSYM_WP_COFFEE;
	}

	if(strcmp("WP_PHASER", text) == 0) {
		return LSYM_WP_PHASER;
	}

	if(strcmp("WP_COMPRESSION_RIFLE", text) == 0) {
		return LSYM_WP_COMPRESSION_RIFLE;
	}

	if(strcmp("WP_TR116", text) == 0) {
		return LSYM_WP_TR116;
	}

	if(strcmp("WP_GRENADE_LAUNCHER", text) == 0) {
		return LSYM_WP_GRENADE_LAUNCHER;
	}

	if(strcmp("WP_QUANTUM_BURST", text) == 0) {
		return LSYM_WP_QUANTUM_BURST;
	}

	if(strcmp("WP_MEDKIT", text) == 0) {
		return LSYM_WP_MEDKIT;
	}

	if(strcmp("WP_VOYAGER_HYPO", text) == 0) {
		return LSYM_WP_VOYAGER_HYPO;
	}

	if(strcmp("WP_DERMAL_REGEN", text) == 0) {
		return LSYM_WP_DERMAL_REGEN;
	}

	if(strcmp("WP_TOOLKIT", text) == 0) {
		return LSYM_WP_TOOLKIT;
	}

	if(strcmp("WP_HYPERSPANNER", text) == 0) {
		return LSYM_WP_HYPERSPANNER;
	}

	if(strcmp("WP_DISRUPTOR", text) == 0) {
		return LSYM_WP_DISRUPTOR;
	}

	if(strcmp("WP_NULL", text) == 0) {
		return LSYM_WP_NULL;
	}

	if(strcmp("WP_NULL_HANDS", text) == 0) {
		return LSYM_WP_NULL_HANDS;
	}

	if(strcmp("WP_0", text) == 0) {
		return LSYM_WP_0;
	}

	if(strcmp("WP_1", text) == 0) {
		return LSYM_WP_1;
	}

	if(strcmp("WP_2", text) == 0) {
		return LSYM_WP_2;
	}

	if(strcmp("WP_3", text) == 0) {
		return LSYM_WP_3;
	}

	if(strcmp("WP_4", text) == 0) {
		return LSYM_WP_4;
	}

	if(strcmp("WP_5", text) == 0) {
		return LSYM_WP_5;
	}

	if(strcmp("WP_6", text) == 0) {
		return LSYM_WP_6;
	}

	if(strcmp("WP_7", text) == 0) {
		return LSYM_WP_7;
	}

	if(strcmp("WP_8", text) == 0) {
		return LSYM_WP_8;
	}

	if(strcmp("WP_9", text) == 0) {
		return LSYM_WP_9;
	}

	if(strcmp("WP_10", text) == 0) {
		return LSYM_WP_10;
	}

	if(strcmp("WP_11", text) == 0) {
		return LSYM_WP_11;
	}

	if(strcmp("WP_12", text) == 0) {
		return LSYM_WP_12;
	}

	if(strcmp("WP_13", text) == 0) {
		return LSYM_WP_13;
	}

	if(strcmp("WP_14", text) == 0) {
		return LSYM_WP_14;
	}

	if(strcmp("WP_15", text) == 0) {
		return LSYM_WP_15;
	}

	if(strcmp("admin", text) == 0) {
		return LSYM_ADMIN;
	}

	if(strcmp("borg", text) == 0) {
		return LSYM_BORG;
	}

	if(strcmp("n00b", text) == 0) {
		return LSYM_N00B;
	}

	if(strcmp("noShow", text) == 0) {
		return LSYM_NO_SHOW;
	}

	if(strcmp("MenuTextureDef", text) == 0) {
		return LSYM_MENU_TEXTURE_DEF;
	}

	if(strcmp("ConsoleName", text) == 0) {
		return LSYM_CONSOLE_NAME;
	}

	if(strcmp("FormalName", text) == 0) {
		return LSYM_FORMAL_NAME;
	}

	if(strcmp("MenuTextureRed", text) == 0) {
		return LSYM_MENU_TEXTURE_RED;
	}

	if(strcmp("MenuTextureTeal", text) == 0) {
		return LSYM_MENU_TEXTURE_TEAL;
	}

	if(strcmp("MenuTextureGold", text) == 0) {
		return LSYM_MENU_TEXTURE_GOLD;
	}

	if(strcmp("MenuTextureGreen", text) == 0) {
		return LSYM_MENU_TEXTURE_GREEN;
	}

	if(strcmp("BoltModel", text) == 0) {
		return LSYM_BOLT_MODEL;
	}

	if(strcmp("BoltShader", text) == 0) {
		return LSYM_BOLT_SHADER;
	}

	if(strcmp("AdmiralRank", text) == 0) {
		return LSYM_ADMIRAL_RANK;
	}

	if(strcmp("true", text) == 0) {
		return LSYM_TRUE;
	}

	if(strcmp("false", text) == 0) {
		return LSYM_FALSE;
	}

	return LSYM_UNKNOWN;
}

void bgLexFatalError(const char* msg, void* lex) {
	Com_Printf("LEXER ERROR: %s\n", msg);
}

