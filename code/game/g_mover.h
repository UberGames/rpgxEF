#ifndef G_MOVER_H_
#define G_MOVER_H_

#include "g_local.h"

/**
 * \brief Run a mover.
 *
 * \param ent Mover to run.
 */
void G_Mover_Run(gentity_t* ent);

/**
 * \brief Touch function for doors.
 *
 * \param ent The mover.
 * \param other The touching entity.
 * \param trace A trace.
 */
void G_Mover_TouchDoorTrigger(gentity_t* ent, gentity_t* other, trace_t* trace);

/**
 * \brief Use function for binary movers.
 *
 * \param ent The mover.
 * \param other Other entity.
 * \param activator Activator.
 */
void G_Mover_UseBinaryMover(gentity_t* ent, gentity_t* other, gentity_t* activator);

#endif /* G_MOVER_H_ */
