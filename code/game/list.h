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

#ifndef __LIBDS_LIST_H__
#define __LIBDS_LIST_H__

/* A C implementation of a doubly-linked list. Contains void pointer values.
Can be used as a LIFO stack of FIFO queue. */

#include <stdlib.h>

#define LIST_FRONT 0
#define LIST_BACK 1

/**
 * Possible type the data in a container may have.
 * LT_DATA means that a custom struct was added.
 */
typedef enum {
	LT_BOOLEAN,
	LT_CHAR,
	LT_UNSIGNED_CHAR,
	LT_SHORT,
	LT_UNSIGNED_SHORT,
	LT_INT,
	LT_UNSIGNED_INT,
	LT_LONG,
	LT_UNSIGNED_LONG,
	LT_DOUBLE,
	LT_STRING,
	LT_DATA,
	LT_MAX
} dataType_t;

/**
 *	Container for data added to the list.
 *	Use of a container allows to add standard c types to the list 
 *	without the need to embed them into a struct.
 */
struct container {
	void*		data;		/*!< pointer to the data */
	size_t		size;		/*!< size of the data    */
	dataType_t	type;		/*!< type of the data    */
	char		pointer;	/*!< determin if the data is a pointer */
} container;

/**
 * Type for a pointer to a container.
 */
typedef struct container* container_p;

/**
 *	Node for a double linked list.
 */
struct linked_node {
	container_p cont;			/*!< cointainer with the data */
	struct linked_node* next;	/*!< next list element */
	struct linked_node* prev;	/*!< previous list element */
};

/**
 * Type for a pointer to a node.
 */
typedef struct linked_node* lnode_p;

/**
 * Struct describing a list.
 */
struct list{
	int length;					/*!< count of elements in the list */
	lnode_p first;				/*!< first element of the list */
	lnode_p last;				/*!< last element of the list */
	void (*destructor)(void*);	/*!< pointer to destructor for data. Default is free. */
};

/**
 *	Type for a pointer to a list.
 */
typedef struct list * list_p;

/** 
 * Struct describing a list iterator.
 */
struct list_iter{
	list_p	list;			/*!< the list */
	lnode_p current;		/*!< current node */
	char started;			/*!< has iteration started */
};

/**
 * Type for a pointer to a list iterator.
 */
typedef struct list_iter * list_iter_p;

/** 
 * Create a linked_list object. This pointer is created on the heap and must be
 * cleared with a call to destroy_list to avoid memory leaks 
 */
list_p create_list(void);

/** 
 * Create a list_iter object for the linked_list list. The flag init can be
 * either LIST_FRONT or LIST_BACK and indicates whether to start the iterator from the first
 * or last item in the list 
 */
list_iter_p list_iterator(list_p list, char init);

/** 
 * Add a pointer to an item with the given value and type to the list.
 * The data is copied by value, so the original pointer must be freed if it
 * was allocated on the heap. 
 * Returns the length of the list if succesfull else returns 0.
 */
int list_add_ptr(list_p list, void* data, dataType_t type, char end);

/**
 * Add a pointer to an item with the given calue, type, and size to the end of the list.
 * The data is copied by value, so the original pointer must be freed if it
 * was allocated on the heap.
 * Returns the length of the list if successfull else returns 0.
 */
int list_append_ptr(list_p list, void* data, dataType_t type);

/**
 * Add a pointer to an item with the given calue, type, and size to the front of the list.
 * The data is copied by value, so the original pointer must be freed if it
 * was allocated on the heap.
 * Returns the length of the list if successfull else returns 0.
 */
int list_prepend_ptr(list_p list, void* data, dataType_t type);

/** 
 * Add an item with the given value, type, and size to the list.
 * The data is copied by value, so the original pointer must be freed if it
 * was allocated on the heap. 
 * Returns the length of the list if succesfull else returns 0.
 */
int list_add(list_p list, void* data, dataType_t type, size_t size, char end);

/**
 * Add an item with the given calue, type, and size to the end of the list.
 * The data is copied by value, so the original pointer must be freed if it
 * was allocated on the heap.
 * Returns the length of the list if successfull else returns 0.
 */
int list_append(list_p list, void* data, dataType_t type, size_t size);

/**
 * Add an item with the given calue, type, and size to the front of the list.
 * The data is copied by value, so the original pointer must be freed if it
 * was allocated on the heap.
 * Returns the length of the list if successfull else returns 0.
 */
int list_prepend(list_p list, void* data, dataType_t type, size_t size);

/**
 * Gets the data stored in the first item of the list or NULL if the list is empty 
 */
container_p list_first(list_p list);
/**
 * Gets the data stored in the last item of the list or NULL if the list is empty 
 */
container_p list_last(list_p list);

/**
 * Removes the last item in the list (LIFO order) and returns the data stored
 * there. The data returned must be freed later in order to remain memory safe. 
 */
container_p list_pop(list_p list);
/**
 * Removes the first item in the list (FIFO order) and returns the data stored
 * there. The data return must be freed later in order to remain memory safe. 
 */
container_p list_poll(list_p list);
/**
 * Convenience function for completely destroying an item in the list. If the end
 * flag is LIST_FRONT, an item will be polled from the front of the list and its data
 * freed. If the end flag is set to LIST_BACK, an item will be popped off the end of
 * the list and the data freed. 
 */
void list_remove(list_p list, char end);

/**
 * Completely free the data associated with the list. 
 */
void destroy_list(list_p list);

/**
 * Destroy a list iterator if allocated.
 */
void destroy_iterator(list_iter_p iter);

/**
 * Return the data held by the current item pointed to by the iterator 
 */
container_p list_current(list_iter_p list);
/**
 * Advances the iterator to the next item in the list and returns the data
 * stored there. 
 */
container_p list_next(list_iter_p list);
/**
 * Advances the iterator to the previous item in the list and returns the data
 * stored there. 
 */
container_p list_prev(list_iter_p list);
/**
 * Advances the iterator to the next item in the list and returns the data
 * stored there. If the end of the list is reached it continues with the first 
 * element of the list.
 */
container_p list_cycl_next(list_iter_p list);
/**
 * Advances the iterator to the previous item in the list and returns the data
 * stored there. If the start of the list is reached it continues with the last 
 * element of the list.
 */
container_p list_cycl_prev(list_iter_p list);

#endif