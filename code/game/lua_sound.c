// sound lib for lua

#include "g_lua.h"
#include "q_shared.h"

#ifdef G_LUA

/***
A documentation to play sounds.
@module sound
*/

/***
Play a sound on a given entity.
@function PlaySound
@param ent The entity the sound will be played on
@param sound The sound file which will be played
@param chan The sound channel the sound will be played on
*/
static int Sound_PlaySound(lua_State *L) {
	char	*sound;
	int		snd;
	int		chan;
	luaGentity_t	*l;

	l = Lua_GetEntity(L,1);
	if(!l || !l->e) return 1;

	sound = (char*)luaL_checkstring(L,2);
	if(!sound[0]) return 1;

	chan = luaL_checknumber(L,3);

	snd = G_SoundIndex(sound);
	G_AddEvent(l->e, EV_SCRIPT_SOUND, snd + (chan << 8));

	return 1;
}

static const luaL_Reg lib_sound[] = {
	{ "PlaySound",	Sound_PlaySound	},
	{ NULL,			NULL			}
};

int Luaopen_Sound(lua_State *L) {
	luaL_register(L, "sound", lib_sound);

	/* add contants */
	lua_pushstring(L, "CONSTANTS");
	lua_newtable(L);
	Lua_RegConstInteger(L, CHAN_ANNOUNCER);
	Lua_RegConstInteger(L, CHAN_AUTO);
	Lua_RegConstInteger(L, CHAN_BODY);
	Lua_RegConstInteger(L, CHAN_ITEM);
	Lua_RegConstInteger(L, CHAN_LOCAL);
	Lua_RegConstInteger(L, CHAN_LOCAL_SOUND);
	Lua_RegConstInteger(L, CHAN_MENU1);
	Lua_RegConstInteger(L, CHAN_VOICE);
	Lua_RegConstInteger(L, CHAN_WEAPON);
	lua_settable(L, -3);

	return 1;
}
#endif
