/*
Copyright (c) 2011 Zhehao Mao

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
#include <stdlib.h>
#include <string.h>

list_p create_list(){
	list_p list = (list_p) malloc(sizeof(struct list));
	list->length = 0;
	list->first = NULL;
	list->last = NULL;
	list->destructor = free;
	return list;
}

list_iter_p list_iterator(list_p list, char init){
	list_iter_p iter = (list_iter_p)malloc(sizeof(struct list_iter));
	if(init==FRONT){
		iter->current = list->first;
	}
	else if(init==BACK){
		iter->current = list->last;
	}
	else {
		if(iter != NULL)
			free(iter);
		return NULL;
	}
	iter->list = list;
	iter->started = 0;
	return iter;
}

void list_add(list_p list, void* data, int size){
	lnode_p node = (lnode_p)malloc(sizeof(struct linked_node));
	node->data = malloc(size);
	memcpy(node->data, data, size);

	if(list->first==NULL){
		node->prev = NULL;
		node->next = NULL;
		list->first = node;
		list->last = node;
	}
	else{
		list->last->next = node;
		node->prev = list->last;
		node->next = NULL;
		list->last = node;
	}
	list->length++;
}

void* list_current(list_iter_p iter){
	if(iter->started&&iter->current!=NULL)
		return iter->current->data;
	return NULL;
}

void* list_next(list_iter_p iter){
	if(!iter->started&&iter->current!=NULL){
		iter->started=1;
		return iter->current->data;
	}
	if(iter->current!=NULL){
		iter->current = iter->current->next;
		return list_current(iter);
	}
	return NULL;
}

void* list_cycl_next(list_iter_p iter){
	if(!iter->started&&iter->current!=NULL){
		iter->started=1;
		return iter->current->data;
	}
	if(iter->current!=NULL){
		iter->current = iter->current->next;
		if(iter->current == NULL) {
			iter->current = iter->list->first;
		}
		return list_current(iter);
	}
	return NULL;
}

void* list_prev(list_iter_p iter){
	if(!iter->started&&iter->current!=NULL){
		iter->started=1;
		return iter->current->data;
	}
	if(iter->current!=NULL){
		iter->current = iter->current->prev;
		return list_current(iter);
	}
	return NULL;
}

void* list_cycl_prev(list_iter_p iter){
	if(!iter->started&&iter->current!=NULL){
		iter->started=1;
		return iter->current->data;
	}
	if(iter->current!=NULL){
		iter->current = iter->current->prev;
		if(iter->current == NULL) {
			iter->current = iter->list->last;
		}
		return list_current(iter);
	}
	return NULL;
}

void* list_first(list_p list){
	return list->first->data;
}

void* list_last(list_p list){
	return list->last->data;
}

void* list_pop(list_p list){
	lnode_p last = list->last;
	if(last == NULL) return NULL;
	list->last = last->prev;
	void* data = last->data;
	if(last->prev != NULL) {
		last->prev->next = NULL;
	}
	free(last);
	list->length--;
	if(list->length == 0) {
		list->last = list->first = NULL;
	}
	return data;
}

void* list_poll(list_p list){
	lnode_p first = list->first;
	if(first == NULL) return NULL;
	list->first = first->next;
	void* data = first->data;
	if(first->next != NULL) {
		first->next->prev = NULL;
	}
	free(first);
	list->length--;
	if(list->length == 0) {
		list->last = list->first = NULL;
	}
	return data;
}

void list_remove(list_p list, char end){
	void * data;
	if(end == FRONT)
		data = list_poll(list);
	else if (end == BACK)
		data = list_pop(list);
	else return;
	list->destructor(data);
}

void destroy_list(list_p list){
	lnode_p cur = list->first;
	lnode_p next;
	while(cur!=NULL){
		next = cur->next;
		if(list->destructor != NULL) { // only destroy data if there is a destructor set
			list->destructor(cur->data);
		}
		free(cur);
		cur = next;
	}
	free(list);
}

void destroy_iterator(list_iter_p iter) {
	if(iter == NULL) return;

	free(iter);
}
