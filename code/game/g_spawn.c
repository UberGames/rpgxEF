// Copyright (C) 1999-2000 Id Software, Inc.
//

#include "g_local.h"
#include "g_spawn.h"

field_t fields[] = {
	{"classname", FOFS(classname), F_LSTRING},
	{"origin", FOFS(s.origin), F_VECTOR},
	{"model", FOFS(model), F_LSTRING},
	{"model2", FOFS(model2), F_LSTRING},
	{"spawnflags", FOFS(spawnflags), F_INT},
	{"speed", FOFS(speed), F_FLOAT},
	{"target", FOFS(target), F_LSTRING},
	{"paintarget", FOFS(paintarget), F_LSTRING},
	{"targetname", FOFS(targetname), F_LSTRING},
	{"message", FOFS(message), F_LSTRING},
	{"team", FOFS(team), F_LSTRING},
	{"splashDamage", FOFS(splashDamage), F_INT},
	{"splashRadius", FOFS(splashRadius), F_INT},
	{"wait", FOFS(wait), F_FLOAT},
	{"random", FOFS(random), F_FLOAT},
	{"count", FOFS(count), F_INT},
	{"material", FOFS(s.powerups), F_INT},
	{"health", FOFS(health), F_INT},
	{"light", 0, F_IGNORE},
	{"dmg", FOFS(damage), F_INT},
	{"angles", FOFS(s.angles), F_VECTOR},
	{"angle", FOFS(s.angles), F_ANGLEHACK},
	{"pos2", FOFS(pos2), F_VECTOR},
	{"apos1", FOFS(apos1), F_VECTOR},
	{"apos2", FOFS(apos2), F_VECTOR},
	{"swapname", FOFS(swapname), F_LSTRING},		//RPG-X Modification | Phenix | 13/06/2004
	{"truename", FOFS(truename), F_LSTRING},
	{"falsename", FOFS(falsename), F_LSTRING},
	{"truetarget", FOFS(truetarget), F_LSTRING}, 
	{"falsetarget", FOFS(falsetarget), F_LSTRING},
	{"booleanstate", FOFS(booleanstate), F_INT},
	{"distance", FOFS(distance), F_FLOAT},	// VALKYRIE: for rotating doors
	{"targetname2", FOFS(targetname2), F_LSTRING },
	{"bluename", FOFS(bluename), F_LSTRING},
	{"greensnd", FOFS(greensound), F_LSTRING},
	{"yellowsnd", FOFS(yellowsound), F_LSTRING},
	{"redsnd", FOFS(redsound), F_LSTRING},
	{"bluesnd", FOFS(bluesound), F_LSTRING},
	{"targetShaderName", FOFS(targetShaderName), F_LSTRING},
	{"targetShaderNewName", FOFS(targetShaderNewName), F_LSTRING},
	#ifdef G_LUA
	{"luaThink", FOFS(luaThink), F_LSTRING},
	{"luaTouch", FOFS(luaTouch), F_LSTRING},
	{"luaUse", FOFS(luaUse), F_LSTRING},
	{"luaHurt", FOFS(luaHurt), F_LSTRING},
	{"luaDie", FOFS(luaDie), F_LSTRING},
	{"luaFree", FOFS(luaFree), F_LSTRING},
	{"luaTrigger", FOFS(luaTrigger), F_LSTRING},
	{"luaReached", FOFS(luaReached), F_LSTRING},
	{"luaReachedAngular", FOFS(luaReachedAngular), F_LSTRING},
	{"luaSpawn", FOFS(luaSpawn), F_LSTRING},

	{"luaParm1", FOFS(luaParm1), F_LSTRING},
	{"luaParm2", FOFS(luaParm2), F_LSTRING},
	{"luaParm3", FOFS(luaParm3), F_LSTRING},
	{"luaParm4", FOFS(luaParm4), F_LSTRING},
	{"luaEntity", FOFS(luaEntity), F_INT},
	#endif
	{"startRGBA", FOFS(startRGBA), F_VECTOR4},
	{"finalRGBA", FOFS(finalRGBA), F_VECTOR4},
	{NULL}
};

qboolean	G_SpawnString( const char *key, const char *defaultString, char **out ) {
	int		i;

	if ( !level.spawning ) {
		*out = (char *)defaultString;
	}

	for ( i = 0 ; i < level.numSpawnVars ; i++ ) {
		if ( !strcmp( key, level.spawnVars[i][0] ) ) {
			*out = level.spawnVars[i][1];
			return qtrue;
		}
	}

	*out = (char *)defaultString;
	return qfalse;
}

qboolean	G_SpawnFloat( const char *key, const char *defaultString, float *out ) {
	char		*s;
	qboolean	present;

	present = G_SpawnString( key, defaultString, &s );
	*out = atof( s );
	return present;
}

qboolean	G_SpawnInt( const char *key, const char *defaultString, int *out ) {
	char		*s;
	qboolean	present;

	present = G_SpawnString( key, defaultString, &s );
	*out = atoi( s );
	return present;
}

qboolean	G_SpawnVector( const char *key, const char *defaultString, float *out ) {
	char		*s;
	qboolean	present;

	present = G_SpawnString( key, defaultString, &s );
	sscanf( s, "%f %f %f", &out[0], &out[1], &out[2] );
	return present;
}

typedef struct {
	char	*name;
	void	(*spawn)(gentity_t *ent);
} spawn_t;

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


spawn_t	spawns[] = {
	// info entities don't do anything at all, but provide positional
	// information for things controlled by other processes
	{"info_player_start", SP_info_player_deathmatch},

	{"NPC_BioHulk", SP_info_player_deathmatch},
	{"NPC_starfleet", SP_info_player_deathmatch},
	{"NPC_starfleet_random", SP_info_player_deathmatch},
	{"NPC_Tuvok", SP_info_player_deathmatch},
	{"NPC_Kim", SP_info_player_deathmatch},
	{"NPC_Doctor", SP_info_player_deathmatch},
	{"NPC_Paris", SP_info_player_deathmatch},
	{"NPC_Torres", SP_info_player_deathmatch},
	{"NPC_Janeway", SP_info_player_deathmatch},
	{"NPC_Seven", SP_info_player_deathmatch},
	{"NPC_Chakotay", SP_info_player_deathmatch},
	{"NPC_Neelix", SP_info_player_deathmatch},
	{"NPC_Vorik", SP_info_player_deathmatch},
	{"NPC_Foster", SP_info_player_deathmatch},
	{"NPC_Munro", SP_info_player_deathmatch},
	{"NPC_MunroScav", SP_info_player_deathmatch},
	{"NPC_Telsia", SP_info_player_deathmatch},
	{"NPC_Biessman", SP_info_player_deathmatch},
	{"NPC_Chang", SP_info_player_deathmatch},
	{"NPC_Chell", SP_info_player_deathmatch},
	{"NPC_Jurot", SP_info_player_deathmatch},
	{"NPC_borg", SP_info_player_deathmatch},
	{"NPC_klingon", SP_info_player_deathmatch},
	{"NPC_Malon", SP_info_player_deathmatch},
	{"NPC_Hirogen", SP_info_player_deathmatch},
	{"NPC_Hirogen_Alpha", SP_info_player_deathmatch},
	{"NPC_Imperial", SP_info_player_deathmatch},
	{"NPC_Imperial_Blue", SP_info_player_deathmatch},
	{"NPC_Imperial_Gold", SP_info_player_deathmatch},
	{"NPC_Imperial_Raider", SP_info_player_deathmatch},
	{"NPC_Stasis", SP_info_player_deathmatch},
	{"NPC_Species8472", SP_info_player_deathmatch},
	{"NPC_Reaver", SP_info_player_deathmatch},
	{"NPC_ReaverGuard", SP_info_player_deathmatch},
	{"NPC_Avatar", SP_info_player_deathmatch},
	{"NPC_Vohrsoth", SP_info_player_deathmatch},
	{"NPC_Desperado", SP_info_player_deathmatch},
	{"NPC_Paladin", SP_info_player_deathmatch},
	{"NPC_ChaoticaGuard", SP_info_player_deathmatch},
	{"NPC_Chaotica", SP_info_player_deathmatch},
	{"NPC_CaptainProton", SP_info_player_deathmatch},
	{"NPC_SatansRobot", SP_info_player_deathmatch},
	{"NPC_Buster", SP_info_player_deathmatch},
	{"NPC_Goodheart", SP_info_player_deathmatch},

	{"info_player_deathmatch",		SP_info_player_deathmatch},
	{"info_player_intermission",	SP_info_player_intermission},
	{"info_null",					SP_info_null},
	{"info_notnull",				SP_info_notnull},		
	{"info_camp",					SP_info_camp},

	{"func_plat",					SP_func_plat},
	{"func_button",					SP_func_button},
	{"func_door",					SP_func_door},
	{"func_forcefield",				SP_func_forcefield},
	{"func_static",					SP_func_static},
	{"func_rotating",				SP_func_rotating},
	{"func_bobbing",				SP_func_bobbing},
	{"func_pendulum",				SP_func_pendulum},
	{"func_train",					SP_func_train},
	{"func_group",					SP_info_null},
	{"func_timer",					SP_func_timer},			// rename trigger_timer?
	{"func_usable",					SP_func_usable},
	{"func_breakable",				SP_func_breakable},
	{"func_door_rotating",			SP_func_door_rotating},
	{"func_brushmodel",				SP_func_brushmodel}, // Hijack me haha
	{"func_lightchange",			SP_func_lightchange},
	{"func_targetmover",			SP_func_targetmover},
	{"func_stasis_door",			SP_func_stasis_door},

	// Triggers are brush objects that cause an effect when contacted
	// by a living player, usually involving firing targets.
	// While almost everything could be done with
	// a single trigger class and different targets, triggered effects
	// could not be client side predicted (push and teleport).
	{"trigger_always",				SP_trigger_always},
	{"trigger_multiple",			SP_trigger_multiple},
	{"trigger_push",				SP_trigger_push},
	{"trigger_teleport",			SP_trigger_teleport},
	{"trigger_hurt",				SP_trigger_hurt},
	{"trigger_transporter",			SP_trigger_transporter},
	{"trigger_radiation",			SP_trigger_radiation},

	// targets perform no action by themselves, but must be triggered
	// by another entity
	{"target_give",					SP_target_give},
	{"target_remove_powerups",		SP_target_remove_powerups},
	{"target_delay",				SP_target_delay},
	{"target_speaker",				SP_target_speaker},
	{"target_print",				SP_target_print},
	{"target_laser",				SP_target_laser},
	{"target_teleporter",			SP_target_teleporter},
	{"target_relay",				SP_target_relay},
	{"target_kill",					SP_target_kill},
	{"target_position",				SP_info_notnull},
	{"target_location",				SP_target_location},
	{"target_push",					SP_target_push},
	{"target_counter",				SP_target_counter},
	{"target_objective",			SP_target_objective},
	{"target_boolean",				SP_target_boolean}, // RPG-X | Phenix | 13/06/2004
	{"target_gravity",				SP_target_gravity},	//RPG-X Phenix/J2J 03/08/04
	{"target_shake",				SP_target_shake},	//RPG-X Phenix/J2J 16/11/04
	{"target_evosuit",				SP_target_evosuit},	//RPG-X Phenix/J2J 16/11/04 - RedTechie: Fixed a typo you have evo suit pointing to shake function
	{"target_turbolift",			SP_target_turbolift},
	{"target_doorlock",				SP_target_doorLock}, //RPG-X | GSIO01 | 08/05/2009
	{"target_repair",				SP_target_repair},	 //RPG-X | GSIO01 | 09/05/2009
	{"target_alert",				SP_target_alert},	//RPG-X | GSIO01
	{"target_warp",					SP_target_warp},	//RPG-X | GSIO01 | 19/05/2009
	{"target_deactivate",			SP_target_deactivate},
	{"target_serverchange",			SP_target_serverchange},
	{"target_levelchange",			SP_target_levelchange},
	{"target_shaderremap",			SP_target_shaderremap},
	{"target_selfdestruct",			SP_target_selfdestruct},
	{"target_safezone",				SP_target_zone},
	{"target_zone",					SP_target_zone},
	{"target_shiphealth",			SP_target_shiphealth},

	{"light",						SP_light},
	{"path_corner",					SP_path_corner},

	{"misc_teleporter_dest",		SP_info_notnull},
	{"misc_model",					SP_misc_model},
	{"misc_model_breakable",		SP_misc_model_breakable},
	{"misc_portal_surface",			SP_misc_portal_surface},
	{"misc_portal_camera",			SP_misc_portal_camera},
	{"misc_turret",					SP_misc_turret},
	{"misc_laser",					SP_laser_arm},
	{"misc_ammo_station",			SP_misc_ammo_station},

	{"shooter_rocket",				SP_shooter_rocket},
	{"shooter_grenade",				SP_shooter_grenade},
	{"shooter_plasma",				SP_shooter_plasma},
	{"shooter_torpedo",				SP_shooter_torpedo},

	{"team_CTF_redplayer",			SP_info_player_deathmatch},
	{"team_CTF_blueplayer",			SP_info_player_deathmatch},

	{"team_CTF_redspawn",			SP_info_player_deathmatch},
	{"team_CTF_bluespawn",			SP_info_player_deathmatch},

	// extra Trek stuff
	{"fx_spark",					SP_fx_spark},
	{"fx_steam",					SP_fx_steam},
	{"fx_bolt",						SP_fx_bolt},
	{"fx_transporter",				SP_fx_transporter},
	{"fx_drip",						SP_fx_drip},
	{"fx_fountain",					SP_fx_fountain},
	{"fx_surface_explosion",		SP_fx_surface_explosion },
	{"fx_blow_chunks",				SP_fx_blow_chunks },
	{"fx_smoke",					SP_fx_smoke },
	{"fx_electrical_explosion",		SP_fx_electrical_explosion },
	{"fx_phaser",					SP_fx_phaser},
	{"fx_torpedo",					SP_fx_torpedo},
	{"fx_particle_fire",			SP_fx_particleFire},
	{"fx_fire",						SP_fx_fire},

// Additional ports from SP by Harry Young
	{"fx_cooking_steam",			SP_fx_cooking_steam},
	{"fx_elecfire",					SP_fx_electricfire},
	//{"fx_forge_bolt",					SP_fx_forge_bolt},
	//{"fx_plasma",					SP_fx_plasma},
	//{"fx_energy_stream",			SP_fx_stream},
	//{"fx_transporter_stream",		SP_fx_transporter_stream},
	//{"fx_explosion_trail",			SP_fx_explosion_trail},
	//{"fx_borg_energy_beam",			SP_fx_borg_energy_beam},
	{"fx_shimmery_thing",			SP_fx_shimmery_thing},
	{"fx_borg_bolt",				SP_fx_borg_bolt},

	{"func_mover",					SP_func_mover},
	{"path_point",					SP_path_point},

	// ui entities
	{"ui_transporter",				SP_ui_transporter},
	{"ui_msd",						SP_ui_msd},
	{"ui_holodeck",					SP_ui_holodeck},
	
	{"ref_tag",						SP_info_notnull},

	// cinematic entities
	{"cinematic_camera",			SP_cinematic_camera},

	{0, 0}
};

/*
===============
G_CallSpawn

Finds the spawn function for the entity and calls it,
returning qfalse if not found
===============
*/
qboolean G_CallSpawn( gentity_t *ent ) {
	spawn_t	*s;
	gitem_t	*item;

	if ( !ent->classname )
	{
		G_Printf ("G_CallSpawn: NULL classname\n");
		return qfalse;
	}

	// check item spawn functions
	for ( item=bg_itemlist+1 ; item->classname ; item++ )
	{
		if ( !strcmp(item->classname, ent->classname) )
		{	// found it
			if( item->giType == IT_TEAM && g_gametype.integer != GT_CTF )
			{
				return qfalse;
			}
			G_SpawnItem( ent, item );

			#ifdef G_LUA
			if(ent->luaSpawn)
			{
				LuaHook_G_EntitySpawn(ent->luaSpawn, ent->s.number);
			}
			#endif

			return qtrue;
		}
	}

	// check normal spawn functions
	for ( s=spawns ; s->name ; s++ )
	{
		if ( !strcmp(s->name, ent->classname) )
		{
			// found it
			s->spawn(ent);

			return qtrue;
		}
	}

	if ( Q_stricmp( "item_botroam", ent->classname ) != 0 )
	{//suppress error message about botroams as those are actually valid
		DEVELOPER(G_Printf (S_COLOR_RED "%s doesn't have a spawn function\n", ent->classname););
	}

	#ifdef G_LUA
	if(ent->luaSpawn)
	{
		LuaHook_G_EntitySpawn(ent->luaSpawn, ent->s.number);
	}
	#endif

	return qfalse;
}

/*
=============
G_NewString

Builds a copy of the string, translating \n to real linefeeds
so message texts can be multi-line
=============
*/
/*@shared@*/ /*@null@*/ char *G_NewString( /*@null@*/ const char *string ) {
	char	*newb, *new_p;
	int		i,l;

	if(string == NULL) {
		return NULL;
	}

	l = strlen(string) + 1;

	newb = (char *)G_Alloc( l );

	if(newb == NULL) {
		return NULL;
	}

	new_p = newb;

	// turn \n into a real linefeed
	for ( i=0 ; i< l ; i++ ) {
		if (string[i] == '\\' && i < l-1) {
			i++;
			if (string[i] == 'n') {
				*new_p++ = '\n';
			} else {
				*new_p++ = '\\';
			}
		} else {
			*new_p++ = string[i];
		}
	}
	
	return newb;
}




/*
===============
G_ParseField

Takes a key/value pair and sets the binary values
in a gentity
===============
*/
qboolean G_ParseField( const char *key, const char *value, gentity_t *ent ) {
	field_t	*f;
	byte	*b;
	float	v;
	vec3_t	vec;
	vec4_t  vec4;

	for ( f=fields ; f->name ; f++ ) {
		if ( !Q_stricmp(f->name, key) ) {
			// found it
			b = (byte *)ent;

			switch( f->type ) {
			case F_LSTRING:
				*(char **)(b+f->ofs) = G_NewString (value);
				break;
			case F_VECTOR:
				sscanf (value, "%f %f %f", &vec[0], &vec[1], &vec[2]);
				((float *)(b+f->ofs))[0] = vec[0];
				((float *)(b+f->ofs))[1] = vec[1];
				((float *)(b+f->ofs))[2] = vec[2];
				break;
			case F_VECTOR4:
				sscanf (value, "%f %f %f %f", &vec4[0], &vec[1], &vec[2], &vec[3]);
				((float *)(b+f->ofs))[0] = vec4[0];
				((float *)(b+f->ofs))[0] = vec4[1];
				((float *)(b+f->ofs))[0] = vec4[2];
				((float *)(b+f->ofs))[0] = vec4[3];
				break;
			case F_INT:
				*(int *)(b+f->ofs) = atoi(value);
				break;
			case F_FLOAT:
				*(float *)(b+f->ofs) = atof(value);
				break;
			case F_ANGLEHACK:
				v = atof(value);
				((float *)(b+f->ofs))[0] = 0;
				((float *)(b+f->ofs))[1] = v;
				((float *)(b+f->ofs))[2] = 0;
				break;
			default:
			case F_IGNORE:
				return qfalse;
				break;
			}
			return qtrue;
		}
	}
	return qfalse;
}




/*
===================
G_SpawnGEntityFromSpawnVars

Spawn an entity and fill in all of the level fields from
level.spawnVars[], then call the class specfic spawn function
===================
*/
void G_SpawnGEntityFromSpawnVars( void )
{
	int			i;
	gentity_t	*ent;
	char		*s, *value, *gametypeName;
	static char *gametypeNames[] = {"ffa", "tournament", "single", "team", "ctf"};

	// get the next free entity
	ent = G_Spawn();

	for ( i = 0 ; i < level.numSpawnVars ; i++ ) {
		G_ParseField( level.spawnVars[i][0], level.spawnVars[i][1], ent );
	}

	// check for "notteam" / "notfree" flags
	if ( g_gametype.integer == GT_SINGLE_PLAYER ) {
		G_SpawnInt( "notsingle", "0", &i );
		if ( i ) {
			G_FreeEntity( ent );
			return;
		}
	}
	if ( g_gametype.integer >= GT_TEAM )
	{
		G_SpawnInt( "notteam", "0", &i );
		if ( i )
		{
			G_FreeEntity( ent );
			return;
		}
	}
	else
	{
		G_SpawnInt( "notfree", "0", &i );
		if ( i )
		{
			G_FreeEntity( ent );
			return;
		}
	}

	if ( G_SpawnString( "gametype", "", &value ) )
	{
		if ( g_gametype.integer >= GT_FFA && g_gametype.integer < GT_MAX_GAME_TYPE )
		{
			gametypeName = gametypeNames[g_gametype.integer];
			s = strstr( value, gametypeName );
			if ( !s )
			{
				G_FreeEntity( ent );
				return;
			}
		}
	}

	// move editor origin to pos
	VectorCopy( ent->s.origin, ent->s.pos.trBase );
	VectorCopy( ent->s.origin, ent->r.currentOrigin );

	// if we didn't get a classname, don't bother spawning anything
	if ( !G_CallSpawn( ent ) )
	{
		G_FreeEntity( ent );
	}
}



/*
====================
G_AddSpawnVarToken
====================
*/
char *G_AddSpawnVarToken( const char *string ) {
	int		l;
	char	*dest;

	l = strlen( string );
	if ( level.numSpawnVarChars + l + 1 > MAX_SPAWN_VARS_CHARS ) {
		G_Error( "G_AddSpawnVarToken: MAX_SPAWN_VARS" );
	}

	dest = level.spawnVarChars + level.numSpawnVarChars;
	memcpy( dest, string, l+1 );

	level.numSpawnVarChars += l + 1;

	return dest;
}

/*
====================
G_ParseSpawnVars

Parses a brace bounded set of key / value pairs out of the
level's entity strings into level.spawnVars[]

This does not actually spawn an entity.
====================
*/
qboolean G_ParseSpawnVars( void ) {
	char		keyname[MAX_TOKEN_CHARS];
	char		com_token[MAX_TOKEN_CHARS];

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;

	// parse the opening brace
	if ( !trap_GetEntityToken( com_token, sizeof( com_token ) ) ) {
		// end of spawn string
		return qfalse;
	}
	if ( com_token[0] != '{' ) {
		G_Error( "G_ParseSpawnVars: found %s when expecting {",com_token );
	}

	// go through all the key / value pairs
	while ( 1 ) {	
		// parse key
		if ( !trap_GetEntityToken( keyname, sizeof( keyname ) ) ) {
			Com_Printf( S_COLOR_RED "G_ParseSpawnVars: Keyname - %s\n", keyname );
			G_Error( "G_ParseSpawnVars: EOF without closing brace" );
		}

		if ( keyname[0] == '}' ) {
			break;
		}
		
		// parse value	
		if ( !trap_GetEntityToken( com_token, sizeof( com_token ) ) ) {
			Com_Printf( S_COLOR_RED "G_ParseSpawnVars: Token - %s\n", com_token );
			G_Error( "G_ParseSpawnVars: EOF without closing brace" );
		}

		if ( com_token[0] == '}' ) {
			G_Error( "G_ParseSpawnVars: closing brace without data" );
		}
		if ( level.numSpawnVars == MAX_SPAWN_VARS ) {
			G_Error( "G_ParseSpawnVars: MAX_SPAWN_VARS" );
		}
		level.spawnVars[ level.numSpawnVars ][0] = G_AddSpawnVarToken( keyname );
		level.spawnVars[ level.numSpawnVars ][1] = G_AddSpawnVarToken( com_token );
		level.numSpawnVars++;
	}

	return qtrue;
}



/*QUAKED worldspawn (0 0 0) ?
-----DESCRIPTION-----
Every map should have exactly one worldspawn.
It holds some general information on the map.

-----SPAWNFLAGS-----
none

-----KEYS-----
"music" - path to WAV or MP3 files (e.g. "music\intro.mp3 music\loopfile.mp3")
"gravity" - 800 is default gravity
"message" - Text to print during connection process

Keys irrelevant for RPG-X
"fraglimit" - overrides server's limit
"capturelimit" - overrides server's capturelimit (use with team AddScores)
"timelimit" - overrides server's timelimit
"timelimitWinningTeam" - "red" or "blue" - this team will win when the timelimit runs out

q3map2:
"_blocksize" block size for unconditional BSP subdivisions
"_celshader" use the specified cel shader for the world
"_lightmapscale" set the lightmapscale for the world
"_ignoreleaks" when set, no leak test is performed
"_foghull" must be set to a sky shader when _fog is used
"_fog" if set, the whole map is fogged using the given shader name
"gridsize" resolution of the light grid
"_ambient" amount of ambient light
"_minvertexlight" amount of minimum vertex light
"_mingridlight" amount of minimum grid light
"_minlight" amount of minimum light
"_keepLights" if set, light entities are not stripped from the BSP file when compiling
"_style42rgbgen" |rgbGen|-like shader definition string for light style 42 (works the same way for all style numbers)
"_style42alphagen" |alphaGen|-like shader definition string for light style 42 (works the same way for all style numbers)
*/
void SP_worldspawn( void ) {
	char	*s;

	G_SpawnString( "classname", "", &s );
	if ( Q_stricmp( s, "worldspawn" ) ) {
		G_Error( "SP_worldspawn: The first entity isn't 'worldspawn'" );
	}

	// make some data visible to connecting client
	trap_SetConfigstring( CS_GAME_VERSION, GAME_VERSION );

	trap_SetConfigstring( CS_LEVEL_START_TIME, va("%i", level.startTime ) );

	G_SpawnString( "music", "", &s );
	trap_SetConfigstring( CS_MUSIC, s );

	G_SpawnString( "message", "", &s );
	trap_SetConfigstring( CS_MESSAGE, s );				// map specific message

	trap_SetConfigstring( CS_MOTD, g_motd.string );		// message of the day
	trap_SetConfigstring( CS_CON_FAIL, rpg_passMessage.string );

	G_SpawnString( "gravity", "800", &s );
	trap_Cvar_Set( "g_gravity", s );

	//FIXME: in some cases, want to carry over from previous running of this map
	G_SpawnString( "fraglimit", "0", &s );
	if ( s && atoi(s) != 0 )
	{
		trap_Cvar_Set( "fraglimit", s );
	}
	G_SpawnString( "capturelimit", "0", &s );
	if ( s && atoi(s) != 0 )
	{
		trap_Cvar_Set( "capturelimit", s );
	}
	G_SpawnString( "timelimit", "0", &s );
	if ( s && atoi(s) != 0 )
	{
		trap_Cvar_Set( "timelimit", s );
	}
	G_SpawnString( "timelimitWinningTeam", "", &s );
	if ( s )
	{
		trap_Cvar_Set( "timelimitWinningTeam", s );
	}

	g_entities[ENTITYNUM_WORLD].s.number = ENTITYNUM_WORLD;
	g_entities[ENTITYNUM_WORLD].classname = "worldspawn";

	// see if we want a warmup time
	trap_SetConfigstring( CS_WARMUP, "" );
	if ( g_restarted.integer ) {
		level.warmupTime = 0;
	}
}


/*
==============
G_SpawnEntitiesFromString

Parses textual entity definitions out of an entstring and spawns gentities.
==============
*/
void G_SpawnEntitiesFromString( void ) {
	// allow calls to G_Spawn*()
	level.spawning = qtrue;
	level.numSpawnVars = 0;

	// the worldspawn is not an actual entity, but it still
	// has a "spawn" function to perform any global setup
	// needed by a level (setting configstrings or cvars, etc)
	if ( !G_ParseSpawnVars() ) {
		G_Error( "SpawnEntities: no entities" );
	}
	SP_worldspawn();

	// parse ents
	while( G_ParseSpawnVars() ) {
		G_SpawnGEntityFromSpawnVars();
	}	

	level.spawning = qfalse;			// any future calls to G_Spawn*() will be errors
}

