default: build_so


# determine arch and platform
ARCH=$(shell uname -m | sed -e s/i.86/i386/)
PLATFORM=$(shell uname|sed -e s/_.*//|tr '[:upper:]' '[:lower:]')

# compiler to use for building shared objects
CC = gcc

# cross compiling
ifneq ($(TARGET), mingw32)
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


# ui objects
OBJ = \
  ui_turbolift.o \
  ui_transporter.o \
  ui_motd.o \
  ui_admin.o \
  ui_video.o \
  ui_teamorders.o \
  ui_team.o \
  ui_startserver.o \
  ui_spskill.o \
  ui_sppostgame.o \
  ui_splevel.o \
  ui_specifyserver.o \
  ui_sparena.o \
  ui_sound.o \
  ui_servers2.o \
  ui_serverinfo.o \
  ui_removebots.o \
  ui_qmenu.o \
  ui_preferences.o \
  ui_playersettings.o \
  ui_players.o \
  ui_playermodel.o \
  ui_network.o \
  ui_mods.o \
  ui_mfield.o \
  ui_menu.o \
  ui_main.o \
  ui_ingame.o \
  ui_gameinfo.o \
  ui_fonts.o \
  ui_emotes.o \
  ui_demo2.o \
  ui_cvars.o \
  ui_credits.o \
  ui_controls2.o \
  ui_connect.o \
  ui_confirm.o \
  ui_cdkey.o \
  ui_atoms.o \
  ui_addbots.o 

# dependencies from game
OBJDEP = \
  q_shared.o \
  q_math.o \
  bg_misc.o 
  
# object for syscalls to the engine
SOOBJ = \
  ui_syscalls.o 

# do cc for shared libraries
ifeq ($(DEBUG), 1)
DO_SOCC = $(CC) $(SOCFLAGS) $(WL) -g3 $(DEFINES) -o $@ -c $<
else
DO_SOCC = $(CC) $(SOCFLAGS) $(WL) $(DEFINES) -o $@ -c $<
endif

build_so: DO_CC=$(DO_SOCC)

cg_consolecmds.o : cg_consolecmds.c; $(DO_CC)

ui_addbots.o : ui_addbots.c; $(DO_CC)
ui_atoms.o : ui_atoms.c; $(DO_CC)
ui_cdkey.o : ui_cdkey.c; $(DO_CC)
ui_confirm.o : ui_confirm.c; $(DO_CC)
ui_connect.o : ui_connect.c; $(DO_CC)
ui_controls2.o : ui_controls2.c; $(DO_CC)
ui_cvars.o : ui_cvars.c; $(DO_CC)
ui_demo2.o : ui_demo2.c; $(DO_CC)
ui_fonts.o : ui_fonts.c; $(DO_CC)
ui_gameinfo.o : ui_gameinfo.c; $(DO_CC)
ui_ingame.o : ui_ingame.c; $(DO_CC)
ui_main.o : ui_main.c; $(DO_CC)
ui_menu.o : ui_menu.c; $(DO_CC)
ui_mfield.o : ui_mfield.c; $(DO_CC)
ui_mods.o : ui_mods.c; $(DO_CC)
ui_network.o : ui_network.c; $(DO_CC)
ui_playermodel.o : ui_playermodel.c; $(DO_CC)
ui_players.o : ui_players.c; $(DO_CC)
ui_playersettings.o : ui_playersettings.c; $(DO_CC)
ui_preferences.o : ui_preferences.c; $(DO_CC)
ui_qmenu.o : ui_qmenu.c; $(DO_CC)
ui_removebots.o : ui_removebots.c; $(DO_CC)
ui_serverinfo.o : ui_serverinfo.c; $(DO_CC)
ui_servers2.o : ui_servers2.c; $(DO_CC)
ui_sound.o : ui_sound.c; $(DO_CC)
ui_sparena.o : ui_sparena.c; $(DO_CC)
ui_specifyserver.o : ui_specifyserver.c; $(DO_CC)
ui_splevel.o : ui_splevel.c; $(DO_CC)
ui_sppostgame.o : ui_sppostgame.c; $(DO_CC)
ui_spreset.o : ui_spreset.c; $(DO_CC)
ui_spskill.o : ui_spskill.c; $(DO_CC)
ui_startserver.o : ui_startserver.c; $(DO_CC)
ui_team.o : ui_team.c; $(DO_CC)
ui_teamorders.o : ui_teamorders.c; $(DO_CC)
ui_video.o : ui_video.c; $(DO_CC)
ui_credits.o : ui_credits.c; $(DO_CC)
ui_admin.o : ui_admin.c; $(DO_CC)
ui_emotes.o : ui_emotes.c; $(DO_CC)
ui_motd.o : ui_motd.c; $(DO_CC)
ui_msd.o : ui_msd.c; $(DO_CC)
ui_transporter.o : ui_transporter.c; $(DO_CC)
ui_turbolift.o : ui_turbolift.c; $(DO_CC)

ui_syscalls.o : ui_syscalls.c; $(DO_CC)

q_shared.o: ../game/q_shared.cpp; $(DO_CC)
q_math.o: ../game/q_math.cpp; $(DO_CC)
bg_misc.o: ../game/bg_misc.cpp; $(DO_CC)

build_so: $(OBJDEP) $(OBJ) $(SOOBJ)
ifeq ($(PLATFORM), mingw32)
	$(CC) -shared -Wl,--export-all-symbols,-soname,ui$(ARCH).$(EXT) -o ui$(ARCH).$(EXT) $(OBJ) $(OBJDEP) $(SOOBJ)
else
	$(CC) -shared -Wl,--export-dynamic,-soname,ui$(ARCH).$(EXT) -o ui$(ARCH).$(EXT) $(OBJ) $(OBJDEP) $(SOOBJ)
endif
	
clean:
	rm -f *.o *.$(EXT)
