#ifndef _G_SPAWN_H
#define _G_SPAWN_H

//
// fields are needed for spawning from the entity string
//
typedef enum {
	F_INT, 
	F_FLOAT,
	F_LSTRING,			// string on disk, pointer in memory, TAG_LEVEL
	F_GSTRING,			// string on disk, pointer in memory, TAG_GAME
	F_VECTOR,
	F_VECTOR4,
	F_ANGLEHACK,
	F_ENTITY,			// index on disk, pointer in memory
	F_ITEM,				// index on disk, pointer in memory
	F_CLIENT,			// index on disk, pointer in memory
	F_IGNORE
} fieldtype_t;


typedef struct
{
	char	*name;
	int		ofs;
	fieldtype_t	type;
	int		flags;
} field_t;

void SP_info_player_deathmatch(/*@shared@*/ gentity_t *ent);
void SP_info_player_intermission(/*@shared@*/ gentity_t *ent);
void SP_info_firstplace(/*@shared@*/ gentity_t *ent);
void SP_info_secondplace(/*@shared@*/ gentity_t *ent);
void SP_info_thirdplace(/*@shared@*/ gentity_t *ent);
void SP_info_podium(/*@shared@*/ gentity_t *ent);

void SP_func_plat (/*@shared@*/ gentity_t *ent);
void SP_func_forcefield(/*@shared@*/ gentity_t *ent);
void SP_func_static(/*@shared@*/ gentity_t *ent);
void SP_func_rotating(/*@shared@*/ gentity_t *ent);
void SP_func_bobbing(/*@shared@*/ gentity_t *ent);
void SP_func_pendulum(/*@shared@*/  gentity_t *ent );
void SP_func_button(/*@shared@*/ gentity_t *ent);
void SP_func_door(/*@shared@*/ gentity_t *ent);
void SP_func_train(/*@shared@*/ gentity_t *ent);
void SP_func_timer(/*@shared@*/ gentity_t *self);
void SP_func_usable(/*@shared@*/ gentity_t *self);
void SP_func_breakable(/*@shared@*/ gentity_t *self);
void SP_func_door_rotating(/*@shared@*/ gentity_t *ent);
void SP_func_brushmodel(/*@shared@*/ gentity_t *ent); // for brushmodel hijacking :D
void SP_func_lightchange(/*@shared@*/ gentity_t *ent); // "toggling" light
void SP_func_targetmover(/*@shared@*/ gentity_t *ent);
void SP_func_stasis_door(/*@shared@*/ gentity_t *ent);

void SP_trigger_always(/*@shared@*/ gentity_t *ent);
void SP_trigger_multiple(/*@shared@*/ gentity_t *ent);
void SP_trigger_push(/*@shared@*/ gentity_t *ent);
void SP_trigger_teleport(/*@shared@*/ gentity_t *ent);
void SP_trigger_hurt(/*@shared@*/ gentity_t *ent);
void SP_trigger_transporter(/*@shared@*/ gentity_t *ent);
void SP_trigger_radiation(/*@shared@*/ gentity_t *ent);

void SP_target_remove_powerups(/*@shared@*/ gentity_t *ent);
void SP_target_give(/*@shared@*/ gentity_t *ent);
void SP_target_delay(/*@shared@*/ gentity_t *ent);
void SP_target_speaker(/*@shared@*/ gentity_t *ent);
void SP_target_print(/*@shared@*/ gentity_t *ent);
void SP_target_laser(/*@shared@*/ gentity_t *self);
void SP_target_character(/*@shared@*/ gentity_t *ent);
void SP_target_teleporter(/*@shared@*/ gentity_t *ent);
void SP_target_relay(/*@shared@*/ gentity_t *ent);
void SP_target_kill(/*@shared@*/ gentity_t *ent);
void SP_target_location(/*@shared@*/ gentity_t *ent);
void SP_target_push(/*@shared@*/ gentity_t *ent);
void SP_target_counter(/*@shared@*/ gentity_t *self);
void SP_target_objective(/*@shared@*/ gentity_t *self);
void SP_target_boolean(/*@shared@*/ gentity_t *ent); //RPG-X | Phenix | 13/06/2004
void SP_target_gravity(/*@shared@*/ gentity_t *ent); //RPG-X | Phenix | 13/06/2004
void SP_target_shake(/*@shared@*/ gentity_t *ent); //RPG-X | Phenix | 16/11/2004
void SP_target_evosuit(/*@shared@*/ gentity_t *ent); //RPG-X | Phenix | 16/11/2004
void SP_target_turbolift(/*@shared@*/ gentity_t *ent); //RPG-X | TiM
void SP_target_doorLock(/*@shared@*/ gentity_t *ent); //RPG-X | GSIO01 | 08/05/2009
void SP_target_repair(/*@shared@*/ gentity_t *ent); //RPG-X | GSIO01 | 09/05/2009
void SP_target_alert(/*@shared@*/ gentity_t *ent); //RPG-X | GSIO01 | 11/05/2009
void SP_target_warp(/*@shared@*/ gentity_t *ent); //RPG-X | GSIO01 | 19/05/2009
void SP_target_deactivate(/*@shared@*/ gentity_t *ent);
void SP_target_serverchange(/*@shared@*/ gentity_t *ent);
void SP_target_levelchange(/*@shared@*/ gentity_t *ent);
void SP_target_shaderremap(/*@shared@*/ gentity_t *ent);
void SP_target_selfdestruct(/*@shared@*/ gentity_t *ent);
void SP_target_zone(/*@shared@*/ gentity_t *ent);
void SP_target_shiphealth(/*@shared@*/ gentity_t *ent);

void SP_light(/*@shared@*/ gentity_t *self);
void SP_info_null(/*@shared@*/ gentity_t *self);
void SP_info_notnull(/*@shared@*/ gentity_t *self);
void SP_info_camp(/*@shared@*/ gentity_t *self);
void SP_path_corner(/*@shared@*/ gentity_t *self);

void SP_misc_model(/*@shared@*/ gentity_t *ent);
void SP_misc_model_breakable(/*@shared@*/ gentity_t *ent);
void SP_misc_portal_camera(/*@shared@*/ gentity_t *ent);
void SP_misc_portal_surface(/*@shared@*/ gentity_t *ent);
void SP_misc_turret(/*@shared@*/ gentity_t *base);
void SP_laser_arm(/*@shared@*/ gentity_t *base);
void SP_misc_ammo_station(/*@shared@*/ gentity_t *ent);

void SP_shooter_rocket(/*@shared@*/ gentity_t *ent);
void SP_shooter_plasma(/*@shared@*/ gentity_t *ent);
void SP_shooter_grenade(/*@shared@*/ gentity_t *ent);
void SP_shooter_torpedo(/*@shared@*/ gentity_t *ent);

// extra Trek stuff
void SP_fx_spark (/*@shared@*/ gentity_t *ent);
void SP_fx_steam (/*@shared@*/ gentity_t *ent);
void SP_fx_bolt (/*@shared@*/ gentity_t *ent);
void SP_fx_transporter(/*@shared@*/ gentity_t *ent);
void SP_fx_drip(/*@shared@*/ gentity_t *ent);

//RPG-X - TiM: SP ripped visual FX
void SP_fx_fountain(/*@shared@*/ gentity_t *ent);
void SP_fx_surface_explosion(/*@shared@*/ gentity_t *ent);
void SP_fx_blow_chunks(/*@shared@*/ gentity_t *ent);
void SP_fx_smoke(/*@shared@*/ gentity_t *ent);
void SP_fx_electrical_explosion(/*@shared@*/ gentity_t *ent);

//RPG-X | RPG-X | 09/05/2009: additional Trek fx
void SP_fx_phaser(/*@shared@*/ gentity_t *ent);
void SP_fx_torpedo(/*@shared@*/ gentity_t *ent);

//RPG-X | RPG-X | 01/07/2009: additional fx
void SP_fx_particleFire(/*@shared@*/ gentity_t *ent);
void SP_fx_fire(/*@shared@*/ gentity_t *ent);

// Additional ports from SP by Harry Young
void SP_fx_cooking_steam(/*@shared@*/ gentity_t	*ent);
void SP_fx_electricfire(/*@shared@*/ gentity_t	*ent);
//void SP_fx_forge_bolt( gentity_t *ent );
//void SP_fx_plasma( gentity_t *ent );
//void SP_fx_stream( gentity_t *ent );
//void SP_fx_transporter_stream( gentity_t *ent );
//void SP_fx_explosion_trail( gentity_t *ent );
//void SP_fx_borg_energy_beam( gentity_t *ent );
void SP_fx_shimmery_thing(/*@shared@*/ gentity_t *ent);
void SP_fx_borg_bolt(/*@shared@*/ gentity_t *ent);

// new mover
void SP_func_mover(/*@shared@*/ gentity_t *ent);
void SP_path_point(/*@shared@*/ gentity_t *ent);

// ui entities
void SP_ui_transporter(/*@shared@*/ gentity_t *ent);
void SP_ui_msd(/*@shared@*/ gentity_t *ent);
void SP_ui_holodeck(/*@shared@*/ gentity_t *ent);

// cinematic entities
void SP_cinematic_camera(/*@shared@*/ gentity_t *ent);

/**
 * \brief Get a string for a custom entity key.
 *
 * Spawn string returns a temporary reference, you must copy the string if you want to keep it.
 *
 * \param[in] key Key to get the value for.
 * \param[in] defaultString Default value for this key.
 * \param[out] out The result.
 * \return Success or fail.
 */
qboolean G_SpawnString( const char* key, const char* defaultString, /*@out@*/ char** out );

/**
 * \brief Get a float for a custom entity key.
 *
 * \param[in] key Key to get the value for.
 * \param[in] defaultString Default value for this key.
 * \param[out] out The result.
 * \return Success or fail.
 */
qboolean G_SpawnFloat( const char* key, const char* defaultString, /*@out@*/ float* out );

/**
 * \brief Get a int for a custom entity key.
 *
 * \param[in] key Key to get the value for.
 * \param[in] defaultString Default value for this key.
 * \param[out] out The result.
 * \return Success or fail.
 */
qboolean G_SpawnInt( const char* key, const char* defaultString, /*@out@*/ int* out );

/**
 * \brief Get a vector for a custom entity key.
 *
 * \param[in] key Key to get the value for.
 * \param[in] defaultString Default value for this key.
 * \param[out] out The result.
 * \return Success or fail.
 */
qboolean G_SpawnVector( const char* key, const char* defaultString, /*@out@*/ float* out );

/**
 * \brief Get a vector for a custom entity key.
 *
 * \param[in] key Key to get the value for.
 * \param[in] defaultString Default value for this key.
 * \param[out] out The result.
 * \return Success or fail.
 */
qboolean G_SpawnVector4( const char* key, const char* defaultString, /*@out@*/ float* out );

/**
 * \brief Spawn all entities from the entity string.
 */
void G_SpawnEntitiesFromString( void );

#endif /* _G_SPAWN_H */

