/*
Copyright (c) 2013 Ubergames

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "list.h"
#include <string.h>

list_p create_list() {
	list_p list = (list_p)malloc(sizeof(struct list));

	if(list == NULL) {
		return NULL;
	}

	list->length = 0;
	list->first = NULL;
	list->last = NULL;
	list->destructor = free;

	return list;
}

list_iter_p list_iterator(list_p list, char init) {
	list_iter_p iter = (list_iter_p)malloc(sizeof(struct list_iter));

	if(iter == NULL) {
		return NULL;
	}

	if(init == LIST_FRONT) {
		iter->current = list->first;
	} else if(init == LIST_BACK) {
		iter->current = list->last;
	} else { // asume front
		iter->current = list->first;
	}

	iter->list = list;
	iter->started = 0;

	return iter;
}

int list_add(list_p list, void* data, dataType_t type, size_t size, char end) {
	lnode_p node = (lnode_p)malloc(sizeof(struct linked_node));

	node->cont = (container_p)malloc(sizeof(container));
	if(node->cont == NULL) {
		return 0;
	}

	node->cont->type = type;
	node->cont->data = malloc(size);
	if(node->cont->data == NULL) {
		return 0;
	}
	memcpy(node->cont->data, data, size);
	node->cont->pointer = 0;
	node->cont->size = size;

	if(list->first == NULL) {
		node->prev = NULL;
		node->next = NULL;
		list->first = node;
		list->last = node;
	} else if(end == LIST_BACK) {
		list->last->next = node;
		node->prev = list->last;
		node->next = NULL;
		list->last = node;
	} else if(end == LIST_FRONT) {
		list->first->prev = node;
		node->next = list->first;
		node->prev = NULL;
		list->first = node;
	} else { // assume back
		list->last->next = node;
		node->prev = list->last;
		node->next = NULL;
		list->last = node;
	}
	list->length++;

	return list->length;
}

int list_append(list_p list, void* data, dataType_t type, size_t size) {
	return list_add(list, data, type, size, LIST_BACK);
}

int list_prepend(list_p list, void* data, dataType_t type, size_t size) {
	return list_add(list, data, type, size, LIST_FRONT);
}

int list_add_ptr(list_p list, void* data, dataType_t type, char end) {
	lnode_p node = (lnode_p)malloc(sizeof(struct linked_node));

	node->cont = (container_p)malloc(sizeof(container));
	if(node->cont == NULL) {
		return 0;
	}

	node->cont->type = type;
	node->cont->data = data;
	node->cont->pointer = 1;
	node->cont->size = 0;

	if(list->first == NULL) {
		node->prev = NULL;
		node->next = NULL;
		list->first = node;
		list->last = node;
	} else if(end == LIST_BACK) {
		list->last->next = node;
		node->prev = list->last;
		node->next = NULL;
		list->last = node;
	} else if(end == LIST_FRONT) {
		list->first->prev = node;
		node->next = list->first;
		node->prev = NULL;
		list->first = node;
	} else { // assume back
		list->last->next = node;
		node->prev = list->last;
		node->next = NULL;
		list->last = node;
	}
	list->length++;

	return list->length;
}

int list_append_ptr(list_p list, void* data, dataType_t type) {
	return list_add_ptr(list, data, type, LIST_BACK);
}

int list_prepend_ptr(list_p list, void* data, dataType_t type) {
	return list_add_ptr(list, data, type, LIST_FRONT);
}

container_p list_current(list_iter_p iter){
	if(iter->started && iter->current != NULL) {
		return iter->current->cont;
	}

	return NULL;
}

container_p list_next(list_iter_p iter) {
	if(!iter->started && iter->current != NULL) {
		iter->started = 1;
		return iter->current->cont;
	}

	if(iter->current != NULL) {
		iter->current = iter->current->next;
		return list_current(iter);
	}

	return NULL;
}

container_p list_cycl_next(list_iter_p iter) {
	if(!iter->started && iter->current != NULL) {
		iter->started = 1;
		return iter->current->cont;
	}
	if(iter->current != NULL) {
		iter->current = iter->current->next;
		if(iter->current == NULL) {
			iter->current = iter->list->first;
		}
		return list_current(iter);
	}
	return NULL;
}

container_p list_prev(list_iter_p iter) {
	if(!iter->started&&iter->current!=NULL) {
		iter->started = 1;
		return iter->current->cont;
	}
	if(iter->current!=NULL) {
		iter->current = iter->current->prev;
		return list_current(iter);
	}
	return NULL;
}

container_p list_cycl_prev(list_iter_p iter){
	if(!iter->started && iter->current != NULL) {
		iter->started =1 ;
		return iter->current->cont;
	}

	if(iter->current!=NULL) {
		iter->current = iter->current->prev;
		if(iter->current == NULL) {
			iter->current = iter->list->last;
		}
		return list_current(iter);
	}

	return NULL;
}

container_p list_first(list_p list) {
	return list->first->cont;
}

container_p list_last(list_p list) {
	return list->last->cont;
}

container_p list_pop(list_p list) {
	container_p cont;
	lnode_p last = list->last;

	if(last == NULL) {
		return NULL;
	}

	list->last = last->prev;
	cont = last->cont;

	if(last->prev != NULL) {
		last->prev->next = NULL;
	}

	free(last);
	list->length--;

	if(list->length == 0) {
		list->last = list->first = NULL;
	}

	return cont;
}

container_p list_poll(list_p list){
	container_p cont;
	lnode_p first = list->first;

	if(first == NULL) {
		return NULL;
	}

	list->first = first->next;
	cont = first->cont;

	if(first->next != NULL) {
		first->next->prev = NULL;
	}

	free(first);
	list->length--;

	if(list->length == 0) {
		list->last = list->first = NULL;
	}

	return cont;
}

void list_remove(list_p list, char end) {
	container_p cont;
	void (*destructor)(void*) = list->destructor;

	if(end == LIST_FRONT) {
		cont = list_poll(list);
	} else if (end == LIST_BACK) {
		cont = list_pop(list);
	} else {
		return;
	}

	if(cont != NULL) {
		if(cont->pointer > 0 && cont->data != NULL) {
			destructor(cont->data);
		}

		free(cont);
	}
}

void destroy_list(list_p list) {
	lnode_p cur = list->first;
	lnode_p next;

	while(cur!=NULL){
		next = cur->next;
		if(list->destructor != NULL) { // only destroy data if there is a destructor set
			if(cur->cont->pointer > 0 && cur->cont->data != NULL) {
				list->destructor(cur->cont->data);
			}
			free(cur->cont);
		}
		free(cur);
		cur = next;
	}
	free(list);
}

void destroy_iterator(list_iter_p iter) {
	if(iter == NULL) {
		return;
	}

	free(iter);
}

