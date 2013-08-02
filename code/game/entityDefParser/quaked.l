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

FILE *in, *out;

#define YY_DECL int yylex()
%}

%x C_QUAKED
%%
"/*QUAKED"		{ BEGIN(C_QUAKED); fprintf(out, "/*QUAKED");  }
<C_QUAKED>"*/"		{ BEGIN(INITIAL); fprintf(out, "*/\n\n");     }
<C_QUAKED>"\t"		{ fprintf(out, "\t"); }
<C_QUAKED>"\n"		{ fprintf(out, "\n"); }
<C_QUAKED>.		{ fprintf(out, "%s", yytext); }
[\n]			;
.			;
%%

main(int argc, char *argv[]) {
	char *buf;
	long len;	

	if(argc < 2) {
		printf("Usage: %s <cfiles> <output file>\n", argv[0]);
	}

	in = fopen(argv[1], "r");
	if(!in) {
		return;
	}

/*	out = fopen(argv[2], "r");
	if(out) {
		fseek(out, 0, SEEK_END);
		len = ftell(out);
		fseek(out, 0, SEEK_SET);

		buf = (char *)malloc(len+1);
		if(!buf) {
			fclose(out);
			return;
		}
		fgets(buf, len, out);
		fclose(out);
		out = NULL;
	}

	printf("%s\n", buf); */
	
	out = fopen(argv[2], "a");
	if(!out) {
		return;
	}

	/*if(buf != NULL) {
		fprintf(out, "%s", buf);
		free(buf);
	}*/

	yyin = in;

	yylex();
}
