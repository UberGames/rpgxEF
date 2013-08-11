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

 * Create a list_iter object for the linked_list list. The flag init can be
 * either LIST_FRONT or LIST_BACK and indicates whether to start the iterator from the first
 * or last item in the list 
 *
 *	\param list pointer to a list
 *	\param init indicator where to start from
 *	\return A new list iterator.
 *
 */
static list_iter_p list_iterator(list_p list, char init) {
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

/** 
 * Add an item with the given value, type, and size to the list.
 * The data is copied by value, so the original pointer must be freed if it
 * was allocated on the heap. 
 * Returns the length of the list if succesfull else returns 0.
 *
 *	\param list pointer to a list
 *	\param data pointer to data
 *	\param type type of data
 *	\param size size of data
 *	\param end indicator where to add the data
 *	\return Count of elements in the list
 */
static int list_add(list_p list, void* data, dataType_t type, size_t size, char end) {
	lnode_p node = (lnode_p)malloc(sizeof(struct linked_node));

	node->cont = (container_p)malloc(sizeof(container));
	if(node->cont == NULL) {
		return 0;
	}

	node->cont->type = type;
	node->cont->data = malloc(size);
	if(node->cont->data == NULL) {
		free(node);
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

/**
 * Add an item with the given calue, type, and size to the end of the list.
 * The data is copied by value, so the original pointer must be freed if it
 * was allocated on the heap.
 * Returns the length of the list if successfull else returns 0.
 *
 *	\param list pointer to a list
 *	\param data pointer to data
 *	\param type type of data
 *	\param size size of data
 *	\return Count of elements in the list
 */
static int list_append(list_p list, void* data, dataType_t type, size_t size) {
	return list_add(list, data, type, size, LIST_BACK);
}

/**
 * Add an item with the given calue, type, and size to the front of the list.
 * The data is copied by value, so the original pointer must be freed if it
 * was allocated on the heap.
 * Returns the length of the list if successfull else returns 0.
 *
 *	\param list pointer to a list
 *	\param data pointer to data
 *	\param type type of data
 *	\param size size of data
 *	\return Count of elements in the list
 */
static int list_prepend(list_p list, void* data, dataType_t type, size_t size) {
	return list_add(list, data, type, size, LIST_FRONT);
}

/** 
 * Add a pointer to an item with the given value and type to the list.
 * Returns the length of the list if succesfull else returns 0.
 *
 *	\param list pointer to a list
 *	\param data pointer to data
 *	\param type type of the data
 *	\param end indicator where to insert
 *	\return Count of elements in the list
 */
static int list_add_ptr(list_p list, void* data, dataType_t type, char end) {
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

/**
 * Add a pointer to an item with the given calue, type, and size to the end of the list.
 * Returns the length of the list if successfull else returns 0.
 *
 *	\param list pointer to a list
 *	\param data pointer to data
 *	\param type type of data
 *	\return Count of elements in list
 */
static int list_append_ptr(list_p list, void* data, dataType_t type) {
	return list_add_ptr(list, data, type, LIST_BACK);
}

/**
 * Add a pointer to an item with the given calue, type, and size to the front of the list.
 * Returns the length of the list if successfull else returns 0.
 *
 *	\param list pointer to a list
 *	\param data pointer to data
 *	\param type type of data
 *	\return Count of elements in list
 */
static int list_prepend_ptr(list_p list, void* data, dataType_t type) {
	return list_add_ptr(list, data, type, LIST_FRONT);
}

/**
 * Return the data held by the current item pointed to by the iterator 
 *
 *	\param list pointer to a iterator
 *	\return container for the current element
 */
static container_p list_current(list_iter_p iter){
	if(iter->started && iter->current != NULL) {
		return iter->current->cont;
	}

	return NULL;
}

/**
 * Advances the iterator to the next item in the list and returns the data
 * stored there. 
 *
 *	\param list pointer to a iterator
 *	\return container of the next element
 */
static container_p list_next(list_iter_p iter) {
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

/**
 * Advances the iterator to the next item in the list and returns the data
 * stored there. If the end of the list is reached it continues with the first 
 * element of the list.
 *
 *	\param list pointer to a iterator
 *	\return container of the next element
 */
static container_p list_cycl_next(list_iter_p iter) {
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

/**
 * Advances the iterator to the previous item in the list and returns the data
 * stored there. 
 *
 *	\param list pointer to a iterator
 *	\return container of the previous element
 */
static container_p list_prev(list_iter_p iter) {
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

/**
 * Advances the iterator to the previous item in the list and returns the data
 * stored there. If the start of the list is reached it continues with the last 
 * element of the list.
 *
 *	\param list pointer to a iterator
 *	\return container of the previous element
 */
static container_p list_cycl_prev(list_iter_p iter){
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

/**
 * Gets the data stored in the first item of the list or NULL if the list is empty 
 *
 *	\param list pointer to a list
 *	\return container for the first element of the list
 */
static container_p list_first(list_p list) {
	return list->first->cont;
}

/**
 * Gets the data stored in the last item of the list or NULL if the list is empty 
 *
 *	\param list pointer to a list
 *	\return container for the last element of the list
 */
static container_p list_last(list_p list) {
	return list->last->cont;
}

/**
 * Removes the last item in the list (LIFO order) and returns the data stored
 * there. The data returned must be freed later in order to remain memory safe. 
 *
 *	\param list pointer to a list
 *	\return container for the last element of the list
 */
static container_p list_pop(list_p list) {
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

/**
 * Removes the first item in the list (FIFO order) and returns the data stored
 * there. The data return must be freed later in order to remain memory safe. 
 *
 *	\param list pointer to a list
 *	\return container for the first element of the list
 */
static container_p list_poll(list_p list){
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

/**
 * Convenience function for completely destroying an item in the list. If the end
 * flag is LIST_FRONT, an item will be polled from the front of the list and its data
 * freed. If the end flag is set to LIST_BACK, an item will be popped off the end of
 * the list and the data freed. 
 *
 *	\param list pointer to a list
 *	\param end indicator where to remove
 */
static void list_remove(list_p list, char end) {
	container_p cont;

	if(end == LIST_FRONT) {
		cont = list_poll(list);
	} else if (end == LIST_BACK) {
		cont = list_pop(list);
	} else {
		return;
	}

	if(cont != NULL) {
		if(cont->pointer == 0 && cont->data != NULL) {
			list->destructor(cont->data);
		}

		free(cont);
	}
}

/**
 * Function for completely destroying an item in the list at a given index.
 *
 *	\param list pointer to a list
 *	\param idx index of the element to remove
 */
static void list_remove_at(list_p list, int idx) {
	container_p cont;
	list_iter_p iter;
	lnode_p target = NULL;
	int i;

	if(idx < 0 || idx >= list->length || list->length == 0) {
		return;
	}

	iter = list_iterator(list, LIST_FRONT);
	for(cont = list_next(iter), i = 0; cont != NULL; cont = list_next(iter), i++) {
		if(i == idx) {
			target = iter->current;
			break;
		}
	}
	destroy_iterator(iter);

	if(target == NULL) {
		return;
	}

	if(list->length == 1) {
		list->first = list->last = NULL;
		list->length--;

		free(target);
	} else {
		target->prev->next = target->next;
		target->next->prev = target->prev;

		if(target == list->first) {
			list->first = target->next;
		}
		if(target == list->last) {
			list->last = target->prev;
		}
		list->length--;

		free(target);
	}

	if(cont != NULL) {
		if(cont->pointer == 0 && cont->data != NULL) {
			list->destructor(cont->data);
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
			if(cur->cont->pointer == 0 && cur->cont->data != NULL) {
				list->destructor(cur->cont->data);
			}
			free(cur->cont);
		}
		free(cur);
		cur = next;
	}
	free(list);
}

/**
 *	Remove all elements.
 *
 *	\param list pointer to a list
 */
static void list_clear(list_p list) {
	while(list->length > 0) {
		list_remove(list, LIST_BACK);
	}
}

void destroy_iterator(list_iter_p iter) {
	if(iter == NULL) {
		return;
	}

	free(iter);
}

/**
 * Get the element at the given index.
 *
 * \param list a list
 * \param idx index
 * \return element at given index or NULL if index is out of bounds
 */
static container_p list_at(list_p list, int idx) {
	list_iter_p iter;
	container_p c = NULL;
	int i;

	if(idx < 0 || idx >= list->length || list == NULL) {
		return NULL;
	}

	iter = list_iterator(list, LIST_FRONT);
	for(c = list_next(iter), i  = 0; c != NULL; c = list_next(iter), i++) {
		if(i == idx) {
			break;
		}
	}

	return c;
}

list_p create_list() {
	list_p list = (list_p)malloc(sizeof(struct list));

	if(list == NULL) {
		return NULL;
	}

	list->length = 0;
	list->first = NULL;
	list->last = NULL;
	list->destructor = free;
	list->add = list_add;
	list->add_ptr = list_add_ptr;
	list->append = list_append;
	list->append_ptr = list_append_ptr;
	list->at = list_at;
	list->clear = list_clear;
	list->current = list_current;
	list->cycl_next = list_cycl_next;
	list->cycl_prev = list_cycl_prev;
	list->end = list_first;
	list->front = list_last;
	list->iterator = list_iterator;
	list->next = list_next;
	list->poll = list_poll;
	list->pop = list_pop;
	list->prepend = list_prepend;
	list->prepend_ptr = list_prepend_ptr;
	list->prev = list_prev;
	list->remove = list_remove;
	list->removeAt = list_remove_at;

	return list;
}

void list_init(struct list * l, void (*destructor)(void*)) {
	memset(l, 0, sizeof(struct list));
	l->destructor = destructor;
	l->add = list_add;
	l->add_ptr = list_add_ptr;
	l->append = list_append;
	l->append_ptr = list_append_ptr;
	l->at = list_at;
	l->clear = list_clear;
	l->current = list_current;
	l->cycl_next = list_cycl_next;
	l->cycl_prev = list_cycl_prev;
	l->end = list_first;
	l->front = list_last;
	l->iterator = list_iterator;
	l->next = list_next;
	l->poll = list_poll;
	l->pop = list_pop;
	l->prepend = list_prepend;
	l->prepend_ptr = list_prepend_ptr;
	l->prev = list_prev;
	l->remove = list_remove;
	l->removeAt = list_remove_at;
}

