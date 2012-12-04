#ifndef EXPORT_LYX_H_
#define EXPORT_LYX_H_

#include "../../game/list.h"

typedef struct function_s* function_p;
struct function_s {
	char* name;
	list_p desc;
	list_p params;
} function_s;
function_p create_function(void);
void destroy_function(function_p f);

typedef struct desc_s* desc_p;
struct desc_s {
	char* text;
} desc_s;
desc_p create_desc(void);
void destroy_desc(desc_p d);

typedef enum { FLOAT, ENTITY, VECTOR  } pType;

typedef struct param_s* param_p;
struct param_s {
	pType type;
	char* name;
	list_p desc;
} param_s;
param_p create_param(void);
void destroy_param(param_p p);

void write_function(function_p f);
void write_desc(list_p d);
void write_param(param_p p);

#define BEGIN_SECTION "\\begin_layout Section"
#define BEGIN_SUBSECTION "\\begin_layout Subsection"
#define BEGIN_STANDART "\\begin_layout Standart"
#define END_LAYOUT "\\ent_layout

#define BEGIN_TABULAR "\
\\begin_layout Standard\
\\begin_inset Tabular\
<lyxtabular version=\"3\" rows=\"2\" columns=\"3\">\
<features tabularvalignment=\"middle\">\
<column alignment=\"center\" valignment=\"top\" width=\"0\">\
<column alignment=\"center\" valignment=\"top\" width=\"0\">\
<column alignment=\"center\" valignment=\"top\" width=\"0\">"

#endif
