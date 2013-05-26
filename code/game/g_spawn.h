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

void SP_info_player_deathmatch (gentity_t *ent);
void SP_info_player_intermission (gentity_t *ent);
void SP_info_firstplace(gentity_t *ent);
void SP_info_secondplace(gentity_t *ent);
void SP_info_thirdplace(gentity_t *ent);
void SP_info_podium(gentity_t *ent);

void SP_func_plat (gentity_t *ent);
void SP_func_forcefield (gentity_t *ent);
void SP_func_static (gentity_t *ent);
void SP_func_rotating (gentity_t *ent);
void SP_func_bobbing (gentity_t *ent);
void SP_func_pendulum( gentity_t *ent );
void SP_func_button (gentity_t *ent);
void SP_func_door (gentity_t *ent);
void SP_func_train (gentity_t *ent);
void SP_func_timer (gentity_t *self);
void SP_func_usable (gentity_t *self);
void SP_func_breakable (gentity_t *self);
void SP_func_door_rotating (gentity_t *ent);
void SP_func_brushmodel(gentity_t *ent); // for brushmodel hijacking :D
void SP_func_lightchange(gentity_t *ent); // "toggling" light
void SP_func_targetmover(gentity_t *ent);
void SP_func_stasis_door(gentity_t *ent);

void SP_trigger_always (gentity_t *ent);
void SP_trigger_multiple (gentity_t *ent);
void SP_trigger_push (gentity_t *ent);
void SP_trigger_teleport (gentity_t *ent);
void SP_trigger_hurt (gentity_t *ent);
void SP_trigger_transporter (gentity_t *ent);
void SP_trigger_radiation( gentity_t *ent );

void SP_target_remove_powerups( gentity_t *ent );
void SP_target_give (gentity_t *ent);
void SP_target_delay (gentity_t *ent);
void SP_target_speaker (gentity_t *ent);
void SP_target_print (gentity_t *ent);
void SP_target_laser (gentity_t *self);
void SP_target_character (gentity_t *ent);
void SP_target_teleporter( gentity_t *ent );
void SP_target_relay (gentity_t *ent);
void SP_target_kill (gentity_t *ent);
void SP_target_location (gentity_t *ent);
void SP_target_push (gentity_t *ent);
void SP_target_counter (gentity_t *self);
void SP_target_objective (gentity_t *self);
void SP_target_boolean (gentity_t *ent); //RPG-X | Phenix | 13/06/2004
void SP_target_gravity (gentity_t *ent); //RPG-X | Phenix | 13/06/2004
void SP_target_shake (gentity_t *ent); //RPG-X | Phenix | 16/11/2004
void SP_target_evosuit (gentity_t *ent); //RPG-X | Phenix | 16/11/2004
void SP_target_turbolift ( gentity_t *ent); //RPG-X | TiM
void SP_target_doorLock ( gentity_t *ent); //RPG-X | GSIO01 | 08/05/2009
void SP_target_repair ( gentity_t *ent); //RPG-X | GSIO01 | 09/05/2009
void SP_target_alert(gentity_t *ent); //RPG-X | GSIO01 | 11/05/2009
void SP_target_warp(gentity_t *ent); //RPG-X | GSIO01 | 19/05/2009
void SP_target_deactivate(gentity_t *ent);
void SP_target_serverchange(gentity_t *ent);
void SP_target_levelchange(gentity_t *ent);
void SP_target_shaderremap(gentity_t *ent);
void SP_target_selfdestruct(gentity_t *ent);
void SP_target_zone(gentity_t *ent);
void SP_target_shiphealth(gentity_t *ent);

void SP_light (gentity_t *self);
void SP_info_null (gentity_t *self);
void SP_info_notnull (gentity_t *self);
void SP_info_camp (gentity_t *self);
void SP_path_corner (gentity_t *self);

void SP_misc_model(gentity_t *ent);
void SP_misc_model_breakable(gentity_t *ent);
void SP_misc_portal_camera(gentity_t *ent);
void SP_misc_portal_surface(gentity_t *ent);
void SP_misc_turret(gentity_t *base);
void SP_laser_arm(gentity_t *base);
void SP_misc_ammo_station( gentity_t *ent );

void SP_shooter_rocket( gentity_t *ent );
void SP_shooter_plasma( gentity_t *ent );
void SP_shooter_grenade( gentity_t *ent );
void SP_shooter_torpedo( gentity_t *ent );

// extra Trek stuff
void SP_fx_spark ( gentity_t *ent );
void SP_fx_steam ( gentity_t *ent );
void SP_fx_bolt ( gentity_t *ent );
void SP_fx_transporter( gentity_t *ent );
void SP_fx_drip( gentity_t *ent );

//RPG-X - TiM: SP ripped visual FX
void SP_fx_fountain ( gentity_t *ent );
void SP_fx_surface_explosion ( gentity_t *ent );
void SP_fx_blow_chunks( gentity_t *ent );
void SP_fx_smoke( gentity_t *ent );
void SP_fx_electrical_explosion( gentity_t *ent );

//RPG-X | RPG-X | 09/05/2009: additional Trek fx
void SP_fx_phaser(gentity_t *ent);
void SP_fx_torpedo( gentity_t *ent );

//RPG-X | RPG-X | 01/07/2009: additional fx
void SP_fx_particleFire(gentity_t *ent);
void SP_fx_fire(gentity_t *ent);

// Additional ports from SP by Harry Young
void SP_fx_cooking_steam( gentity_t	*ent );
void SP_fx_electricfire( gentity_t	*ent );
//void SP_fx_forge_bolt( gentity_t *ent );
//void SP_fx_plasma( gentity_t *ent );
//void SP_fx_stream( gentity_t *ent );
//void SP_fx_transporter_stream( gentity_t *ent );
//void SP_fx_explosion_trail( gentity_t *ent );
//void SP_fx_borg_energy_beam( gentity_t *ent );
void SP_fx_shimmery_thing( gentity_t *ent );
void SP_fx_borg_bolt( gentity_t *ent );

// new mover
void SP_func_mover(gentity_t *ent);
void SP_path_point(gentity_t *ent);

// ui entities
void SP_ui_transporter(gentity_t *ent);
void SP_ui_msd(gentity_t *ent);
void SP_ui_holodeck(gentity_t *ent);

// cinematic entities
void SP_cinematic_camera(gentity_t *ent);

#endif /* _G_SPAWN_H */

