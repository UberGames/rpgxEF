default: so
so: build_so

# determine arch and platform
ARCH=$(shell uname -m | sed -e s/i.86/i386/)
PLATFORM=$(shell uname|sed -e s/_.*//|tr '[:upper:]' '[:lower:]')

# compiler to use for building shared objects
CC = gcc

# cross compiling
ifneq ($(PLATFORM), mingw32)
ifeq ($(TARGET), win32)
  PLATFORM=mingw32
  ARCH=x86
  CC=i686-w64-mingw32-gcc
  CFLAGS+=-m32
endif
ifeq ($(TARGET), win64)
  PLATFORM=mingw32
  ARCH=x64
  CC=x86_64-w64-mingw32-gcc
  CFLAGS+=-m64
endif
else
#we are compiling on windows
ARCH=x86
endif

# cflags for the compiler
ifeq ($(PLATFORM), mingw32)
SOCFLAGS = $(CFLAGS) 
else
SOCFLAGS = $(CFLAGS) -fPIC
endif

# set extension
ifeq ($(PLATFORM), mingw32)
EXT=dll
else
EXT=so
endif

# warning level
ifeq ($(DEBUG), 1)
WL=-Wall
else
WL=-Wall -Wno-unused-but-set-variable 
endif

#defines
DEFINES=

ifeq ($(SQL), 1)
DEFINES+=-DSQL
endif

# game objects
OBJ = \
  g_ui.o \
  g_lua.o \
  q_shared.o \
  q_math.o \
  g_weapon.o \
  g_utils.o \
  g_usable.o \
  g_turrets.o \
  g_trigger.o \
  g_team.o \
  g_target.o \
  g_svcmds.o \
  g_spawn.o \
  g_session.o \
  g_mover.o \
  g_missile.o \
  g_misc.o \
  g_mem.o \
  g_main.o \
  g_log.o \
  g_items.o \
  g_fx.o \
  g_combat.o \
  g_sql.o \
  g_cmds.o \
  g_client.o \
  g_breakable.o \
  g_bot.o \
  g_arenas.o \
  g_active.o \
  g_cinematic.o \
  bg_slidemove.o \
  bg_pmove.o \
  bg_oums.o \
  bg_misc.o \
  ai_team.o \
  ai_main.o \
  ai_dmq3.o \
  ai_dmnet.o \
  ai_cmd.o \
  ai_chat.o \
  lua_game.o \
  lua_entity.o \
  lua_vector.o \
  lua_mover.o \
  lua_qmath.o \
  lua_cinematic.o \
  lua_sound.o \
  lua_weapons.o \
  lua_trace.o \
  lua_cvar.o \
  sqlite3.o \
  md5.o \
  list.o
 
# game object  for syscalls to the engine
SOOBJ = \
  g_syscalls.o

# objects for lua  
LUAOBJ = \
  lapi.o \
  lauxlib.o \
  lbaselib.o \
  lbitlib.o \
  lcode.o \
  lcorolib.o \
  lctype.o \
  ldblib.o \
  ldebug.o \
  ldo.o \
  ldump.o \
  lfunc.o \
  lgc.o \
  linit.o \
  liolib.o \
  llex.o \
  lmathlib.o \
  lmem.o \
  loadlib.o \
  lobject.o \
  lopcodes.o \
  loslib.o \
  lparser.o \
  lstate.o \
  lstring.o \
  lstrlib.o \
  ltable.o \
  ltablib.o \
  ltm.o \
  lua.o \
  luac.o \
  lundump.o \
  lvm.o \
  lzio.o
  
# sqlite
SQLITE= \
  sqlite3.o

# do cc for shared library
ifeq ($(DEBUG), 1)
DO_SOCC = $(CC) $(SOCFLAGS) $(WL) -g3 $(DEFINES) -o $@ -c $<
else
DO_SOCC = $(CC) $(SOCFLAGS) $(WL) $(DEFINES) -o $@ -c $<
endif
# do cc for lua
ifeq ($(DEBUF), 1)
DO_LUACC = $(CC) -O2 -Wall -g3 $(SOCFLAGS) -DLUA_COMPAT_ALL -o $@ -c $<
else
DO_LUACC = $(CC) -O2 -Wall $(SOCFLAGS) -DLUA_COMPAT_ALL -o $@ -c $<
endif

build_so: DO_CC=$(DO_SOCC)

# game
ai_chat.o : ai_chat.c; $(DO_CC)
ai_cmd.o : ai_cmd.c; $(DO_CC)
ai_dmnet.o : ai_dmnet.c; $(DO_CC)
ai_dmq3.o : ai_dmq3.c; $(DO_CC)
ai_main.o : ai_main.c; $(DO_CC)
ai_team.o : ai_team.c; $(DO_CC)
bg_misc.o : bg_misc.c; $(DO_CC)
bg_pmove.o : bg_pmove.c; $(DO_CC)
bg_slidemove.o : bg_slidemove.c; $(DO_CC)
g_active.o : g_active.c; $(DO_CC)
g_arenas.o : g_arenas.c; $(DO_CC)
g_bot.o : g_bot.c; $(DO_CC)
g_breakable.o : g_breakable.c; $(DO_CC)
g_client.o : g_client.c; $(DO_CC)
g_cmds.o : g_cmds.c; $(DO_CC)
g_combat.o : g_combat.c; $(DO_CC)
g_fx.o : g_fx.c; $(DO_CC)
g_items.o : g_items.c; $(DO_CC)
g_log.o : g_log.c; $(DO_CC)
g_main.o : g_main.c; $(DO_CC)
g_mem.o : g_mem.c; $(DO_CC)
g_misc.o : g_misc.c; $(DO_CC)
g_missile.o : g_missile.c; $(DO_CC)
g_mover.o : g_mover.c; $(DO_CC)
g_session.o : g_session.c; $(DO_CC)
g_spawn.o : g_spawn.c; $(DO_CC)
g_svcmds.o : g_svcmds.c; $(DO_CC)
g_target.o : g_target.c; $(DO_CC)
g_team.o : g_team.c; $(DO_CC)
g_trigger.o : g_trigger.c; $(DO_CC)
g_turrets.o : g_turrets.c; $(DO_CC)
g_usable.o : g_usable.c; $(DO_CC)
g_utils.o : g_utils.c; $(DO_CC)
g_weapon.o : g_weapon.c; $(DO_CC)
q_math.o : q_math.c; $(DO_CC)
q_shared.o : q_shared.c; $(DO_CC)
g_lua.o: g_lua.c; $(DO_CC)
g_ui.o: g_ui.c; $(DO_CC)
g_sql.o: g_sql.c; $(DO_CC)
g_cinematic.o: g_cinematic.c; $(DO_CC)
bg_oums.o : bg_oums.c; $(DO_CC)
lua_game.o: lua_game.c; $(DO_CC)
lua_entity.o: lua_entity.c; $(DO_CC)
lua_mover.o: lua_mover.c; $(DO_CC)
lua_qmath.o: lua_qmath.c; $(DO_CC)
lua_vector.o: lua_vector.c; $(DO_CC)
lua_cinematic.o: lua_cinematic.c; $(DO_CC)
lua_sound.o: lua_sound.c; $(DO_CC)
lua_weapons.o: lua_weapons.c; $(DO_CC)
lua_trace.o: lua_trace.c; $(DO_CC)
lua_cvar.o: lua_cvar.c; $(DO_CC)
sqlite3.o: sqlite3.c; $(DO_CC)
md5.o: md5.c; $(DO_CC)
list.o: list.c; $(DO_CC)
 
# game syscalls 
g_syscalls.o : g_syscalls.c; $(DO_CC)

# bg_lib
bg_lib.o : bg_lib.c; $(DO_CC)

# lua
lapi.o: lapi.c; $(DO_LUACC)
lauxlib.o: lauxlib.c; $(DO_LUACC)
lbaselib.o: lbaselib.c; $(DO_LUACC)
lbitlib.o: lbitlib.c; $(DO_LUACC)
lcode.o: lcode.c; $(DO_LUACC)
lcorolib.o: lcorolib.c; $(DO_LUACC)
lctype.o: lctype.c; $(DO_LUACC)
ldblib.o: ldblib.c; $(DO_LUACC)
ldebug.o: ldebug.c; $(DO_LUACC)
ldo.o: ldo.c; $(DO_LUACC)
ldump.o: ldump.c; $(DO_LUACC)
lfunc.o: lfunc.c; $(DO_LUACC)
lgc.o: lgc.c; $(DO_LUACC)
linit.o: linit.c; $(DO_LUACC)
liolib.o: liolib.c; $(DO_LUACC)
llex.o: llex.c; $(DO_LUACC)
lmathlib.o: lmathlib.c; $(DO_LUACC)
lmem.o: lmem.c; $(DO_LUACC)
loadlib.o: loadlib.c; $(DO_LUACC)
lobject.o: lobject.c; $(DO_LUACC)
lopcodes.o: lopcodes.c; $(DO_LUACC)
loslib.o: loslib.c; $(DO_LUACC)
lparser.o: lparser.c; $(DO_LUACC)
lstate.o: lstate.c; $(DO_LUACC)
lstring.o: lstring.c; $(DO_LUACC)
lstrlib.o: lstrlib.c; $(DO_LUACC)
ltable.o: ltable.c; $(DO_LUACC)
ltablib.o: ltablib.c; $(DO_LUACC)
ltm.o: ltm.c; $(DO_LUACC)
lua.o: lua.c; $(DO_LUACC)
luac.o: luac.c; $(DO_LUACC)
lundump.o: lundump.c; $(DO_LUACC)
lvm.o: lvm.c; $(DO_LUACC)
lzio.o: lzio.c; $(DO_LUACC)

build_so: $(OBJ) $(SOOBJ) $(LUAOBJ)
ifeq ($(PLATFORM), mingw32)
	$(CC) -shared -Wl,--export-all-symbols,-soname,qqgame$(ARCH).$(EXT) -o qagame$(ARCH).$(EXT) $(OBJ) $(SOOBJ) $(LUAOBJ) -lm
else
	$(CC) -shared -Wl,--export-dynamic,-soname,qagame$(ARCH).$(EXT) -o qagame$(ARCH).$(EXT) $(OBJ) $(SOOBJ) $(LUAOBJ) -lm -lpthread
endif
	
clean:
	rm -f *.o *.$(EXT) 
