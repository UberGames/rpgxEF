#include "export_lyx.h"

#include <stdlib.h>

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
