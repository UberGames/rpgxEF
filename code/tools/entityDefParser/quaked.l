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

	out = fopen(argv[2], "r");
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
	}

	out = fopen(argv[2], "a");
	if(!out) {
		return;
	}

	if(buf != NULL) {
		fprintf(out, "%s", buf);
		free(buf);
	}

	yyin = in;

	yylex();
}
