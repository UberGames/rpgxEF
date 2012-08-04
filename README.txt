README for rpgxEF
Project page: http://rpgxEF.hennecke-online.net
   - last updated 21.01.2012

This project is a modification to the icculus.org maintained quake3 engine
and hte Star Trek - Voyager: Elite Force compability patch for it. It was
meant as modification of the engine for RPG-X2 that increases some of the
limits the original engine has and which caused many problems with RPG-X2.

After succeeding to lift those limits I started to add many modifications 
like graphical enhancements. Which many people welcomed. But now as there
is an new state of the art renderer for ioqucke3 inbound I decided to create
a newer version of this engine which as you'll note does not have any of 
the graphical enhancements as I plan to use this new renderer once it is 
finished.

#################
###  License  ###
#################
rpgxEF is licensed under GPL v2, ecluding the contents of the folders 
code/game, code/cgame, and code/ui which are licensed under a different
license (see README-RPG-X2-.txt).

#################
### Changelog ###
#################

Version 1.0 (this version):
- Maximal number of chat lines displayed is back to original count (will 
  be changed to 10 again later)
- Bloom effect was removed
- Motion blur was removed
- Framebuffer GLSL post processign effects where removed
- r_showtris modification was removed (will be back later)

Previous versions:
- MAX_GAMESTATE_CHARS quadrupled to 64000 chars (fixed bad command byte/entity
  limit increase)
- MAX_CONFIGSTRINGS quadrupled to 4096 (needed for MAX_GAMESTATE_CHARS and
  MAX_MODELS increase)
- MAX_MSGLEN quadrupled to 65536 (needed for MAX_GAMESTATE_CHARS increase)
- MAX_MODELS doubled to 512
- messagemode1 is now displayed as local: instead of say:
- messagemode2 is now displayed as global: instead of say_team:
- maximal number of chat lines displayed on screen was increased to 10
- added messagemode5 for say_class
- increased Entity limit to 2048
- added Support for external lightmaps which are loaded from "maps//lightmaps"
  (tga only). To make q3map2 produce external lightmaps use -lightmapsize x 
  (where x is a power of 2, for example 1024), the lightmaps will then stored 
  in maps/ by q3map2 and have to be moved (if you use NetRadiant you can use 
  -lightmapdir to output the lightmaps directly to the correct directory). 
  External lightmaps tend to be much darker than standard lightmaps, therefore 
  I suggest to compile your map with -gamma y -compensate z where y >= 2 and z <= y.
- added optional Bloom effect (r_bloom 1 and r_bloom_* to configure)
- added optional Motion Blur effect (r_motionBlur 1 and r_motionBlurStrength 
  to configure)
- readded Shader Remapping support to rpgxEF and added support for it in RPG-X
- r_showtris is now able to show different entitytypes in different colors
  highlight entities: r_showtris += 2
  highlight md3: r_showtris += 4
  highlight Raven mdr: r_showtris += 8
  highlight md4: r_showtris += 16
  highlight other non world: r_showtris += 32
- applied FBO patch
  r_ext_framebuffer 1 to enable
  r_ext_framebuffer_bloom 1 to enable bloom, r_ext_framebuffer_bloom_* to configure, r_ext_framebuffer_blur_* to configure
  r_ext_framebuffer_rotoscope 1 to enable rotoscope effect, r_ext_framebuffer_rotoscope_* to configure
  r_ext_framebuffer_colorinvert to enable color invert (temporaly disabled)

############################
### GENERAL INSTRUCTIONS ###
############################

IMPORTANT NOTE:
This engine is for RPG-X2 and may only be used with the shared
libraries of RPG-X2 specificly released for rpgxEF. Also this
is Work in Progress and therefore may be unstable. Use of this 
engine at your own risk.

#########################################
### WINDOWS INSTALLATION INSTRUCTIONS ###
#########################################

Put all files contained in the archive to the same folder where
the original engine (stvefHM.exe) is located.
The standard path is C:\Program Files\Raven\Star Trek Voyager Elite Force

You need to have OpenAL installed to be able to use OpenAL.

########################################
### MACOSX INSTALLATION INSTRUCTIONS ###
########################################

Offical note Mac OS X support is currently stalled. Anyway if you manage
to compile the engine yourself here is how you install it.

Move the compiled binaries to where you installed Elite Force (for example ioEF).
If you haven't installed Elite Force before this is one way how you might 
do it:
- create the folder where you want it installed
- create a folder baseEF in this folder
- copy all pakx.pk3 file from your Elite Force and Elite Force XP disk to 
  this folder
- install RPG-X2 into your Elite Force folder
- copy the binaries into the Elite Force folder

#######################################
### LINUX INSTALLATION INSTRUCTIONS ###
#######################################

Prerequisites:
 * A working hardware accelerated OpenGL setup 
   (DRI, fireglx, nvidia-glx...)
 * libSDL - Simple Direct Layer libraries
 * libmad - MPEG Audio Decoder (also known as MAD sometimes).
 * libvorbis, libogg, libvorbisfile - OGG Vorbis audio decoder
Optional:
 * OpenAL - OpenAL sound libraries
 * libcurl - FTP/HTTP download support.

You can fullfill these dependencies by installing the correct packages from
your favorite distribution's packet manager.

Create a new rpgxef directory, you will probably want to use a dir like:
/usr/local/games/rpgxef

Copy a build of this engine that you can get from
http://rpgxef.hennecke-online.net
into the new directory.
Also copy any directories from MODs you installed previously on windows to
this directory.

Create a subdirectory named "baseEF" (case sensitive!!) and copy the
pak0.pk3, pak1.pk3 and pak2.pk3 from the original eliteforce
release in there and don't forget the new pak92.pk3!
pak1.pk3 and pak2.pk3 are files that have been released after the game went
gold. You must either get them by installing the latest official EF patch on
windows or you can get them from the expansion pack cd.
pak3.pk3 can only be found on the expansion pack CD. If you want to have the
maps and additions from the expension pack, you must copy this file as well.

#########################################
### FREEBSD INSTALLATION INSTRUCTIONS ###
#########################################

Alas, the installer for Linux does not support FreeBSD. However, you can still
play the game natively. Just follow the manual installation instructions from
the Linux part and all should be good. The required libraries are the same on
FreeBSD and Linux.

########################################################################
## For Engine fiddlers and Gentoo-I-compile-everything-myself-freaks ###
########################################################################

Since rpgxEF (and also ioEF and ioquake3) is GPL'ed, all my modifications are freely
available.

You must get a recent revision of rpgxEF
 - you need git to checkout the code
 - the rpgxEF repository is located at gitHub 
  (see https://github.com/UberGames/rpgxEF) for diffrent ways to check out
  the git repository

Linux users:
- Make sure you got all packages needed, especially the -dev packages.
  You need the header files for libmad, libSDL, libopenal and libGL
   
- You may need to edit the Makefile to make use of a gcc version < 4.
  gcc 4.0.x is not advisable if you need stability.

- Type in "make debug" for a version with debugging symbols or "make" for a
  release version. If all goes well, the new binaries will be found in the
  build/ subdir.

Windows users (will take a bit longer, experienced coders only):
The Windows version of rpgxEF is cross compiled on Linux. That means
the Windows executables are compiled on Linux and not on Windows.
Compiling rpgxEF on Windows is not officaly supported anyway you can
still compile it on Windows either with mingw or Microsoft Visual Studio

MSVC:
- Open up the project (project files can be found in code\win32\msvc\)
- You need to compile the libmad library from:
  http://sourceforge.net/projects/mad
  so that you have a .lib file you can link against
- If you compile and it complains about a missing dinput.h you need the
- DirectX SDK
- If you want to have OpenAL you need the OpenAL SDK, too.
- Back to your MSVC environment:
  look at the renderer and botlib subproject and modify the preprocessor
  definitions in a way so that ELITEFORCE and XTRA are defined
- Same goes to the main quake3 subproject, also add the define:
  USE_CODEC_MP3=1 and USE_OPENAL=1;USE_OPENAL_DLOPEN=1 (the last two ones
  only when you want OpenAL support).
- Add all include and library directories to the compiler/linker like
  OpenAL include/library dir, MAD include/library dir, etc...
- You may need to specify additional libraries in the linker dependencies
  like user32.lib Advapi32.lib ole32.lib etc...
- Build.

mingw:
Works very similar to linux. You may need to find some and even compile
some of the dependencies yourself.

Good luck!

Cross compiling:
- install mingw32/mingw64
- invoke the shell script
  - cross-make-mingw.sh for Win32
  - cross-make-mingw64.sh for Win64
- to use mp3 support and/or openAL support add the same arguments behind the 
  shell scripts as you would to when compiling for linux. For example
  cross-make-mingw.sh USE_CODEC_MP3=1 would compile rpgxEF for Win32 with mp3
  support.

###############
### Credits ###
###############
 -- Thilo Schulz (arny@ats.s.bawue.de)
 -- Anybody that helped with ioEF
 -- Anybody working/having worked on ioquake3
 -- Anybody I have forgotten
