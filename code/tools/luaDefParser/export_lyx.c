#include "export_lyx.h"

#include <stdlib.h>
#include <stdio.h>

function_p create_function(void) {
	function_p n = (function_p)malloc(sizeof(function_s));

	if(n == NULL) return NULL;

	n->desc = create_list();
	if(n->desc == NULL) {
		free(n);
		return NULL;
	}

	n->params = create_list();
	if(n->params == NULL) {
		destroy_list(n->desc);
		free(n);
		return NULL;
	}

	return n;
}

void destroy_function(function_p f) {
	if(f != NULL) {
		if(f->desc != NULL) {
			destroy_list(f->desc);
		}
		if(f->params != NULL) {
			destroy_list(f->params);
		}
		free(f);
	}
}


desc_p create_desc(void) {
	desc_p n = (desc_p)malloc(sizeof(desc_s));
	
	if(n == NULL) return NULL;
	
	return n;
}

void destroy_desc(desc_p d) {
	if(d != NULL) {
		if(d->text != NULL) {
			free(d->text);
		}
		
		free(d);
	}
}

param_p create_param(void) {
	param_p n = (param_p)malloc(sizeof(param_s));
	
	if(n == NULL) return NULL;
	
	n->desc = create_list();
	if(n->desc == NULL) {
		free(n);
		return NULL;
	}
	
	return n;
}

void destroy_param(param_p p) {
	if (p != NULL) {
		if(p->name != NULL) {
			free(p->name);
		}
		if(p->desc != NULL) {
			destroy_list(p->desc);
		}
		
		free(p);
	}
}

void write_desc(list_p d, FILE* f) {
	list_iter_p iter;
	desc_p d;

	if(d == NULL || f == NULL) return;

	fprintf(f, "%s", BEGIN_STANDART);

	for(d = list_next(iter); d != NULL; d = list_next(iter)) {
		fprintf(f, "%s", d->text);
	}

	fprintf(f, "%s", END_LAYOUT);
}

void write_params(list_p p, FILE* f) {
	list_iter_p iter;
	param_p d;
	
	if(p == NULL || f == NULL) return;

	fprintf(f, "%s", BEGIN_TABULAR);
	iter = list_iterator(p);
	for(d = list_next(iter); d != NULL; d = list_next(iter)) {
		write_param(d, f);
	}
	fprintf(f, "%s", END_TABULAR);
}

void write_param(param_p p, FILE* f) {
	list_iter_p iter;
	desc_p d;

	if(p == NULL || f == NULL) return;

	fprintf(f, "%s%s", BEGIN_TABULAR_ROW, BEGIN_TABULAR_CELL);
	switch(p->type) {
	case FLOAT:
		fprintf(f, "float\n");
		break;
	case ENTITY:
		fprintf(f, "entity\n");
		break;
	case VECTOR:
		fprintf(f, "vector\n");
		break;
	default:
		fprintf(stderr, "write_param - Uknown param type %d\n", p->type);
		exit(EXIT_FAILURE);
	}
	fprintf(f, "%s%s", END_TABULAR_CELL, BEGIN_TABULAR_CELL);
	fprintf(f, "%s\n", p->name);
	fprintf(f, "%s%s", END_TABULAR_CELL, BEGIN_TABULAR_CELL);
	iter = list_iterator(p->desc, FRONT);
	for(d = list_next(iter); d != NULL; d = list_next(iter)) {
		fprintf(f, "%s", d->desc);
	}
	fprintf(f, "%s%s%s", END_TABULAR_CELL, END_TABULAR_ROW, END_TABULAR);
}

