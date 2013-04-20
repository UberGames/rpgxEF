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
 * Type for a pointer to a list iterator.
 */
typedef struct list_iter * list_iter_p;

/**
 *	Type for a pointer to a list.
 */
typedef struct list * list_p;

/**
 * Struct describing a list.
 */
struct list{
	int length;					/*!< count of elements in the list */
	lnode_p first;				/*!< first element of the list */
	lnode_p last;				/*!< last element of the list */
	void (*destructor)(void*);	/*!< pointer to destructor for data. Default is free. */
	list_iter_p (*iterator)(list_p list, char init);	/*!< creates a new list iterator */
	int (*add_ptr)(list_p list, void* data, dataType_t type, char end); /*!< add a pointer to the list */
	int (*append_ptr)(list_p list, void* data, dataType_t type); /*!< append a pointer to the list */
	int (*prepend_ptr)(list_p list, void* data, dataType_t type); /*!< prepend a pointer to the list */
	int (*add)(list_p list, void* data, dataType_t type, size_t size, char end); /*!< add data to the list */
	int (*append)(list_p list, void* data, dataType_t type, size_t size); /*!< append data to the list */
	int (*prepend)(list_p list, void* data, dataType_t type, size_t size); /*!< prepend data to the list */
	container_p (*at)(list_p list, int idx); /*!< get container at given index */
	void (*clear)(list_p list); /*!< clear the list */
	container_p (*current)(list_iter_p iter); /*!< get the current element for the iterator */
	container_p (*cycl_next)(list_iter_p iter); /*!< get the next element for the iterator (cyclic access) */
	container_p (*cycl_prev)(list_iter_p iter); /*!< get the previous element for the iterator (cyclic acccess) */
	container_p (*front)(list_p list); /*!< get the first element of the list */
	container_p (*end)(list_p list); /*!< get the last element of the list */
	container_p (*next)(list_iter_p iter); /*!< get the next element for the iterator */
	container_p (*prev)(list_iter_p iter); /*!< get the previous element for the iterator */
	container_p (*poll)(list_p list); /*<! poll */
	container_p (*pop)(list_p list); /*<! pop */
	void (*remove)(list_p list, char end); /*!< remove an element from the list */
	void (*removeAt)(list_p list, int idx); /*!< remove an element at a specified index */
};

/** 
 * Struct describing a list iterator.
 */
struct list_iter {
	list_p	list;			/*!< the list */
	lnode_p current;		/*!< current node */
	char started;			/*!< has iteration started */
};

/** 
 * Create a linked_list object. This pointer is created on the heap and must be
 * cleared with a call to destroy_list to avoid memory leaks.
 *
 *	\return A new list allocated on the heap.
 */
list_p create_list(void);

/**
 * Completely free the data associated with the list. 
 *
 *	\param list pointer to a list
 */
void destroy_list(list_p list);

/**
 * Destroy a list iterator if allocated.
 *
 *	\param list pointer to a iterator
 */
void destroy_iterator(list_iter_p iter);

/**
 * Initialize list. For use on lists that are NOT allocated on the heap.
 *
 *	\param l a list
 *	\param destructor pointer to destructor function
 */
void list_init(struct list * l, void (*destructor)(void*));

#endif