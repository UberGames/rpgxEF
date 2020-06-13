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
# we are compiling on windows
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

# cgame objects
OBJ = \
  fx_transporter.o \
  fx_tetrion.o \
  fx_disruptor.o \
  fx_hypospray.o \
  fx_quantum.o \
  fx_phaser.o \
  fx_misc.o \
  fx_lib.o \
  fx_item.o \
  fx_grenade.o \
  fx_compression.o \
  cg_weapons.o \
  cg_view.o \
  cg_snapshot.o \
  cg_servercmds.o \
  cg_screenfx.o \
  cg_scoreboard.o \
  cg_predict.o \
  cg_playerstate.o \
  cg_players.o \
  cg_marks.o \
  cg_main.o \
  cg_localents.o \
  cg_info.o \
  cg_event.o \
  cg_env.o \
  cg_ents.o \
  cg_effects.o \
  cg_drawtools.o \
  cg_draw.o \
  cg_consolecmds.o \
  cg_lua.o \
  lua_vector.o \
  lua_qmath.o \
  lua_cfx.o \
  lua_cent.o \
  lua_refent.o \

# depency objects from game
OBJDEP = \
  q_shared.o \
  q_math.o \
  bg_misc.o \
  bg_pmove.o \
  bg_slidemove.o 
  
 # object for syscalls to the engine
SOOBJ = \
  cg_syscalls.o 
  
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

# do cc for shared library
ifeq ($(DEBUG), 1)
DO_SOCC = $(CC) $(SOCFLAGS) $(WL) -g3 $(DEFINES) -o $@ -c $<
else
DO_SOCC = $(CC) $(SOCFLAGS) $(WL) $(DEFINES) -o $@ -c $<
endif
# do cc for lua
ifeq ($(DEBUG), 1)
DO_LUACC = $(CC) -O2 -Wall $(SOCFLAGS) -DLUA_COMPAT_ALL -o $@ -c $<
else
DO_LUACC = $(CC) -O2 -Wall -g3 $(SOCFLAGS) -DLUA_COMPAT_ALL -o $@ -c $<
endif

build_so: DO_CC=$(DO_SOCC)

# cgame
cg_consolecmds.o : cg_consolecmds.c; $(DO_CC)
cg_draw.o : cg_draw.c; $(DO_CC)
cg_drawtools.o : cg_drawtools.c; $(DO_CC)
cg_effects.o : cg_effects.c; $(DO_CC)
cg_ents.o : cg_ents.c; $(DO_CC)
cg_env.o : cg_env.c; $(DO_CC)
cg_event.o : cg_event.c; $(DO_CC)
cg_info.o : cg_info.c; $(DO_CC)
cg_localents.o : cg_localents.c; $(DO_CC)
cg_main.o : cg_main.c; $(DO_CC)
cg_marks.o : cg_marks.c; $(DO_CC)
cg_players.o : cg_players.c; $(DO_CC)
cg_playerstate.o : cg_playerstate.c; $(DO_CC)
cg_predict.o : cg_predict.c; $(DO_CC)
cg_scoreboard.o : cg_scoreboard.c; $(DO_CC)
cg_screenfx.o : cg_screenfx.c; $(DO_CC)
cg_servercmds.o : cg_servercmds.c; $(DO_CC)
cg_snapshot.o : cg_snapshot.c; $(DO_CC)
cg_view.o : cg_view.c; $(DO_CC)
cg_weapons.o : cg_weapons.c; $(DO_CC)
cg_lua.o : cg_lua.c; $(DO_CC)
fx_compression.o : fx_compression.c; $(DO_CC)
fx_grenade.o : fx_grenade.c; $(DO_CC)
fx_item.o : fx_item.c; $(DO_CC)
fx_lib.o : fx_lib.c; $(DO_CC)
fx_misc.o : fx_misc.c; $(DO_CC)
fx_phaser.o : fx_phaser.c; $(DO_CC)
fx_quantum.o : fx_quantum.c; $(DO_CC)
fx_hypospray.o : fx_hypospray.c; $(DO_CC)
fx_disruptor.o : fx_disruptor.c; $(DO_CC)
fx_tetrion.o : fx_tetrion.c; $(DO_CC)
fx_transporter.o : fx_transporter.c; $(DO_CC)
lua_qmath.o: ../game/lua_qmath.cpp; $(DO_CC)
lua_vector.o: ../game/lua_vector.cpp; $(DO_CC)
lua_cfx.o: lua_cfx.c; $(DO_CC)
lua_cent.o: lua_cent.c; $(DO_CC)
lua_refent.o: lua_refent.c; $(DO_CC);


# dependencies from game
q_shared.o: ../game/q_shared.cpp; $(DO_CC)
q_math.o: ../game/q_math.cpp; $(DO_CC)
bg_misc.o:  ../game/bg_misc.cpp; $(DO_CC)
bg_pmove.o: ../game/bg_pmove.cpp; $(DO_CC)
bg_slidemove.o: ../game/bg_slidemove.cpp; $(DO_CC)

# cgame syscalls
cg_syscalls.o : cg_syscalls.c; $(DO_CC)

# lua
lapi.o: ../game/lapi.c; $(DO_LUACC)
lauxlib.o: ../game/lauxlib.c; $(DO_LUACC)
lbaselib.o: ../game/lbaselib.c; $(DO_LUACC)
lbitlib.o: ../game/lbitlib.c; $(DO_LUACC)
lcode.o: ../game/lcode.c; $(DO_LUACC)
lcorolib.o: ../game/lcorolib.c; $(DO_LUACC)
lctype.o: ../game/lctype.c; $(DO_LUACC)
ldblib.o: ../game/ldblib.c; $(DO_LUACC)
ldebug.o: ../game/ldebug.c; $(DO_LUACC)
ldo.o: ../game/ldo.c; $(DO_LUACC)
ldump.o: ../game/ldump.c; $(DO_LUACC)
lfunc.o: ../game/lfunc.c; $(DO_LUACC)
lgc.o: ../game/lgc.c; $(DO_LUACC)
linit.o: ../game/linit.c; $(DO_LUACC)
liolib.o: ../game/liolib.c; $(DO_LUACC)
llex.o: ../game/llex.c; $(DO_LUACC)
lmathlib.o: ../game/lmathlib.c; $(DO_LUACC)
lmem.o: ../game/lmem.c; $(DO_LUACC)
loadlib.o: ../game/loadlib.c; $(DO_LUACC)
lobject.o: ../game/lobject.c; $(DO_LUACC)
lopcodes.o: ../game/lopcodes.c; $(DO_LUACC)
loslib.o: ../game/loslib.c; $(DO_LUACC)
lparser.o: ../game/lparser.c; $(DO_LUACC)
lstate.o: ../game/lstate.c; $(DO_LUACC)
lstring.o: ../game/lstring.c; $(DO_LUACC)
lstrlib.o: ../game/lstrlib.c; $(DO_LUACC)
ltable.o: ../game/ltable.c; $(DO_LUACC)
ltablib.o: ../game/ltablib.c; $(DO_LUACC)
ltm.o: ../game/ltm.c; $(DO_LUACC)
lua.o: ../game/lua.c; $(DO_LUACC)
luac.o: ../game/luac.c; $(DO_LUACC)
lundump.o: ../game/lundump.c; $(DO_LUACC)
lvm.o: ../game/lvm.c; $(DO_LUACC)
lzio.o: ../game/lzio.c; $(DO_LUACC)

build_so: $(OBJDEP) $(OBJ) $(SOOBJ) $(LUAOBJ)
ifeq ($(PLATFORM), mingw32)
	$(CC) -shared -Wl,--export-all-symbols,-soname,cgame$(ARCH).$(EXT) -o cgame$(ARCH).$(EXT) $(OBJ) $(OBJDEP) $(SOOBJ) $(LUAOBJ) -lm
else
	$(CC) -shared -Wl,--export-dynamic,-soname,cgame$(ARCH).$(EXT) -o cgame$(ARCH).$(EXT) $(OBJ) $(OBJDEP) $(SOOBJ) $(LUAOBJ) -lm -lpthread
endif
	
clean:
	rm -f *.o *.$(EXT)
