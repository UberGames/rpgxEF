#ifndef _G_ITEMS_H
#define _G_ITEMS_H

#include "g_local.h"
#include "g_active.h"

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
/*@shared@*/ /*@null@*/  gentity_t* Drop_Item( gentity_t* ent, gitem_t* item, double angle );

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

/**
 *	Drop a weapon.
 *
 *	\author Ubergames - Marcin 
 *	\date 03/12/2008
 *	\param ent The client.
 *	\param item The item.
 *	\param angle Angle to throw at.
 *	\param flags Entity flags to use.
 *	\param Optional message for PADD dropping.
 */
/*@shared@*/ /*@null@*/  gentity_t *DropWeapon( gentity_t* ent, gitem_t* item, double angle, int32_t flags, char* txt );

/*
================
FinishSpawningDetpack

Traces down to find where an item should rest, instead of letting them
free fall from their spawn points
================
*/
qboolean FinishSpawningDetpack( gentity_t* ent, int32_t itemIndex );

//! Min ammount of weapons a player can have for each weapons
extern vmCvar_t* Min_Weapons[];

//! Max ammount of weapons a player can have for each weapons
extern vmCvar_t* Max_Weapons[];

//RPG-X: Marcin: for easier lookup - 30/12/2008
int32_t Max_Weapon(int32_t num);
int32_t Min_Weapon(int32_t num);

#endif /* _G_ITEMS_H */