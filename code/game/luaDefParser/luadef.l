/*
	Copyright (c) 2012 Walter Julius Hennecke, Ubergames

	Permission is hereby granted, free of charge, to any person obtaining a
	copy of this software and associated documentation files (the
	"Software"), to deal in the Software without restriction, including without
	limitation the rights to use, copy, modify, merge, publish, distribute,
	sublicense, and/or sell copies of the Software, and to permit persons to
	whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included
	in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT
	WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR
	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
	THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
	ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
	AN ACTION OF CONTRACT, TORT OR OTHERWISE,
	ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/


%{
#include <stdio.h>
#include <string.h>

FILE* out;

int lastState = 0;

#define YY_DECL int yylex()
%}

%x C_COMMENT
%x C_FUNCTION
%x C_DESC
%x C_PARAM
%x C_STRING
%x C_MODULE
FUNCTEXT [a-zA-z0-9.(), \t]+
STRING [a-zA-z0-9.(), \t\n!?<>/\\\[\]\{\}]+
VAR [a-zA-Z0-9]+
%%
"/*"				{ BEGIN(C_COMMENT); puts("Found a comment"); 			}
<C_COMMENT>"*/"		 	{ BEGIN(INITIAL); puts("End of comment"); 			}
<C_COMMENT>[ \t\n*]		;
<C_COMMENT>"luamodule"		{ BEGIN(C_MODULE); puts("Found a luamodule");			}
<C_COMMENT>"\\function"		{ BEGIN(C_FUNCTION); puts("Found a \\function"); 		}
<C_COMMENT>"\\desc"		{ BEGIN(C_DESC); puts("Found a \\desc"); 			}
<C_COMMENT>"\\param"		{ BEGIN(C_PARAM); puts("Found a \\param"); 			}
<C_COMMENT>[ \t\n]*		;
<C_COMMENT>.			;

<C_MODULE>[ \t\n]		;
<C_MODULE>{VAR}			{ BEGIN(C_COMMENT); printf("module name: %s\n", yytext); 	}
<C_MODULE>.			;

<C_FUNCTION>";"			{ BEGIN(C_COMMENT); puts("End of function"); 			}
<C_FUNCTION>{FUNCTEXT}		{ printf("function text: %s\n", yytext); 			}
<C_FUNCTION>.			;

<C_DESC>";"			{ BEGIN(C_COMMENT); puts("End of desc"); 			}
<C_DESC>"\""			{ BEGIN(C_STRING); lastState = C_DESC; puts("Found a String"); 	}
<C_DESC>[.\n]+			{ printf("desc text: %s\n", yytext); 				}	
<C_DESC>.			;

<C_PARAM>";"			{ BEGIN(C_COMMENT); puts("End of param"); 			}
<C_PARAM>"\""			{ BEGIN(C_STRING); lastState = C_PARAM; puts("Found a string"); }
<C_PARAM>[ \t]			;
<C_PARAM>"float"		{ printf("foud type: %s\n", yytext); 				}
<C_PARAM>"vec"			{ printf("foud type: %s\n", yytext); 				}
<C_PARAM>"vector"		{ printf("foud type: %s\n", yytext); 				}
<C_PARAM>"ent"			{ printf("foud type: %s\n", yytext); 				}
<C_PARAM>"entity"		{ printf("foud type: %s\n", yytext); 				}
<C_PARAM>{VAR}			{ printf("foud name: %s\n", yytext); 				}
<C_PARAM>.			;

<C_STRING>"\""			{ BEGIN(lastState); puts("End of String"); 			}
<C_STRING>{STRING}		{ printf("string text: %s\n", yytext); 				}
<C_STRING>.			;

"\n"				;
.				;
%%

main(int argc, char *argv[]) {
	FILE *in;
	
	if(argc < 3) {
		printf("Usage: %s <input file> <output file>\n", argv[0]);
		return;
	}

	in = fopen(argv[1], "r");
	if(!in) {
		printf("Could not open \'%s\'.\n", argv[1]);
		return;
	}
	
	out = fopen(argv[2], "a");
	if(!out) {
		fclose(in);
		printf("Could not open \'%s\'.\n", argv[2]);
		return;
	}

	yyin = in;

	yylex();
	
	fclose(in);
	fclose(out);
}
