#ifndef _G_ITEMS_H
#define _G_ITEMS_H

#include "g_local.h"

/**
*	Data structures + functions for the PADD messaging system.
*
*	\author Ubergames - Marcin 
*	\date 06/12/2008
*/
typedef struct
{
	gentity_t*	key;		//!< PADD item entity pointer
	char		owner[64];
	char		value[256];
} paddData_t;

#define PADD_DATA_MAX	256 //!< max number of padds
#define MAX_DROPPED		255 //!< should be reasonable

extern paddData_t	paddData[PADD_DATA_MAX];
extern int			paddDataNum;
extern int			numTotalDropped;

/**
 * Run an item.
 *
 * \param ent The item.
 */
void G_RunItem( gentity_t* ent );

/**
 * Sets the clipping size and plants the object on the floor.
 * Items can't be immediately dropped to floor, because they might
 * be on an entity that hasn't spawned yet.
 *
 * \param ent Entity for item.
 * \param item The item.
 */
void G_SpawnItem (gentity_t* ent, gitem_t* item);

/**
 * Spawns an item and tosses it forward.
 *
 * \param ent An entity to toss from.
 * \param item The item.
 * \param angle Direction to toss to.
 * \return The entity for the item.
 */
/*@shared@*/ /*@null@*/  gentity_t* Drop_Item( gentity_t* ent, gitem_t* item, float angle );

/**
 * Clear all registered items.
 */
void ClearRegisteredItems( void );

/**
 * Register a new item. The item will be added to the precache list.
 *
 * \param item Item to register.
 */
void RegisterItem( gitem_t* item );

/**
 * Write the needed items to a config string so the client will know which ones to precache.
 */
void SaveRegisteredItems( void );

#endif /* _G_ITEMS_H */