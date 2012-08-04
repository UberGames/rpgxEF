// Copyright (C) 1999-2000 Id Software, Inc.
//
/*
=======================================================================

USER INTERFACE MAIN

=======================================================================
*/


#include "ui_local.h"


/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .qvm file
================
*/
int vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6 ) {
	switch ( command ) {
	case UI_GETAPIVERSION:
		return UI_API_VERSION;

	case UI_INIT:
		UI_Init();
		return 0;

	case UI_SHUTDOWN:
		UI_Shutdown();
		return 0;

	case UI_KEY_EVENT:
		UI_KeyEvent( arg0 );
		return 0;

	case UI_MOUSE_EVENT:
		UI_MouseEvent( arg0, arg1 );
		return 0;

	case UI_REFRESH:
		UI_Refresh( arg0 );
		return 0;

	case UI_IS_FULLSCREEN:
		return UI_IsFullscreen();

	case UI_SET_ACTIVE_MENU:
		UI_SetActiveMenu( arg0 );
		return 0;

	case UI_CONSOLE_COMMAND:
		return UI_ConsoleCommand();

	case UI_DRAW_CONNECT_SCREEN:
		UI_DrawConnectScreen( arg0 );
		return 0;
	}

	return -1;
}


/*
================
cvars
================
*/

typedef struct {
	vmCvar_t	*vmCvar;
	char		*cvarName;
	char		*defaultString;
	int			cvarFlags;
} cvarTable_t;

vmCvar_t	ui_ffa_fraglimit;
vmCvar_t	ui_ffa_timelimit;

vmCvar_t	ui_tourney_fraglimit;
vmCvar_t	ui_tourney_timelimit;

vmCvar_t	ui_team_fraglimit;
vmCvar_t	ui_team_timelimit;
vmCvar_t	ui_team_friendly;

vmCvar_t	ui_ctf_capturelimit;
vmCvar_t	ui_ctf_timelimit;
vmCvar_t	ui_ctf_friendly;

vmCvar_t	ui_arenasFile;
vmCvar_t	ui_botsFile;
vmCvar_t	ui_botminplayers;
vmCvar_t	ui_spScores1;
vmCvar_t	ui_spScores2;
vmCvar_t	ui_spScores3;
vmCvar_t	ui_spScores4;
vmCvar_t	ui_spScores5;
vmCvar_t	ui_spAwards;
vmCvar_t	ui_spVideos;
vmCvar_t	ui_spSkill;

vmCvar_t	ui_weaponrespawn;
vmCvar_t	ui_speed;
vmCvar_t	ui_gravity;
vmCvar_t	ui_knockback;
vmCvar_t	ui_dmgmult;
vmCvar_t	ui_adaptRespawn;
vmCvar_t	ui_holoIntro;
vmCvar_t	ui_forcerespawn;
vmCvar_t	ui_respawnGhostTime;
vmCvar_t	ui_warmup;
vmCvar_t	ui_dowarmup;
vmCvar_t	ui_team_race_blue;
vmCvar_t	ui_team_race_red;

vmCvar_t	ui_pModAssimilation;
vmCvar_t	ui_pModDisintegration;
vmCvar_t	ui_pModActionHero;
vmCvar_t	ui_pModSpecialties;
vmCvar_t	ui_pModElimination;

vmCvar_t	ui_spSelection;

vmCvar_t	ui_browserMaster;
vmCvar_t	ui_browserGameType;
vmCvar_t	ui_browserSortKey;
vmCvar_t	ui_browserShowFull;
vmCvar_t	ui_browserShowEmpty;

vmCvar_t	ui_drawCrosshair;
vmCvar_t	ui_drawCrosshairNames;
vmCvar_t	ui_marks;

vmCvar_t	ui_server1;
vmCvar_t	ui_server2;
vmCvar_t	ui_server3;
vmCvar_t	ui_server4;
vmCvar_t	ui_server5;
vmCvar_t	ui_server6;
vmCvar_t	ui_server7;
vmCvar_t	ui_server8;
vmCvar_t	ui_server9;
vmCvar_t	ui_server10;
vmCvar_t	ui_server11;
vmCvar_t	ui_server12;
vmCvar_t	ui_server13;
vmCvar_t	ui_server14;
vmCvar_t	ui_server15;
vmCvar_t	ui_server16;
vmCvar_t	ui_server17;
vmCvar_t	ui_server18;
vmCvar_t	ui_server19;
vmCvar_t	ui_server20;
vmCvar_t	ui_server21;
vmCvar_t	ui_server22;
vmCvar_t	ui_server23;
vmCvar_t	ui_server24;
vmCvar_t	ui_server25;
vmCvar_t	ui_server26;
vmCvar_t	ui_server27;
vmCvar_t	ui_server28;
vmCvar_t	ui_server29;
vmCvar_t	ui_server30;
vmCvar_t	ui_server31;
vmCvar_t	ui_server32;

vmCvar_t	ui_cdkeychecked;
vmCvar_t	ui_cdkeychecked2;
vmCvar_t	ui_language;
vmCvar_t	ui_s_language;
vmCvar_t	ui_k_language;
vmCvar_t	ui_playerClass;
vmCvar_t	ui_playerRank;
//vmCvar_t	ui_precacheweapons;

vmCvar_t	ui_allowvote;
vmCvar_t	ui_chatsallowed;
vmCvar_t	ui_allowsuicide;
vmCvar_t	ui_selfdamage;
vmCvar_t	ui_rpg;
vmCvar_t	ui_kickspammers;
vmCvar_t	ui_kicksuiciders;
vmCvar_t	ui_allowspmaps;
vmCvar_t	ui_rangetricorder;
vmCvar_t	ui_rangehypo;
vmCvar_t	ui_norpgclasses;
vmCvar_t	ui_forceclasscolor;
vmCvar_t	ui_nosecurity;
vmCvar_t	ui_nomarine;
vmCvar_t	ui_nomedical;
vmCvar_t	ui_noscience;
vmCvar_t	ui_nocommand;
vmCvar_t	ui_noengineer;
vmCvar_t	ui_noalien;
vmCvar_t	ui_nomaker;
vmCvar_t	ui_nocloak;
vmCvar_t	ui_noflight;
vmCvar_t	ui_phaserdmg;
vmCvar_t	ui_rifledmg;
vmCvar_t	ui_stasisdmg;
vmCvar_t	ui_imoddmg;
vmCvar_t	ui_noweapons;
vmCvar_t	ui_alienflags;
vmCvar_t	ui_marineflags;
vmCvar_t	ui_securityflags;
vmCvar_t	ui_makerflags;
vmCvar_t	ui_medicalflags;
vmCvar_t	ui_scienceflags;
vmCvar_t	ui_commandflags;
vmCvar_t	ui_engineerflags;
vmCvar_t	ui_welcomemessage;
vmCvar_t	ui_timedmessage;
vmCvar_t	ui_timedmessagetime;
//vmCvar_t	ui_lastactive; //RPG-X | Phenix | 25/02/2005 (Trying to make a timer for intro) //TiM - crashes RPG-X -_-
vmCvar_t	ui_kickAfterXkills;
//RPG-X: TiM - Variables - 8-8-2005
vmCvar_t	ui_dynamicCrosshair;
vmCvar_t	ui_dynamicLensFlares;
vmCvar_t	ui_currentRankSet;	//RPG-X | TiM | Users can choose the rank they want in the main ui
vmCvar_t	ui_currentClassSet;

vmCvar_t	ui_age;
vmCvar_t	ui_height;
vmCvar_t	ui_weight;
vmCvar_t	ui_race;

vmCvar_t	ui_defaultChar;

//RPG-X: TiM - UI Emote variables
//-The last 16 emotes played
vmCvar_t	ui_recentEmote1;
vmCvar_t	ui_recentEmote2;
vmCvar_t	ui_recentEmote3;
vmCvar_t	ui_recentEmote4;
vmCvar_t	ui_recentEmote5;
vmCvar_t	ui_recentEmote6;
vmCvar_t	ui_recentEmote7;
vmCvar_t	ui_recentEmote8;
vmCvar_t	ui_recentEmote9;
vmCvar_t	ui_recentEmote10;
vmCvar_t	ui_recentEmote11;
vmCvar_t	ui_recentEmote12;
vmCvar_t	ui_recentEmote13;
vmCvar_t	ui_recentEmote14;
vmCvar_t	ui_recentEmote15;
vmCvar_t	ui_recentEmote16;

//-Users can have up to 16 favorite emotes
vmCvar_t	ui_favoriteEmote1;
vmCvar_t	ui_favoriteEmote2;
vmCvar_t	ui_favoriteEmote3;
vmCvar_t	ui_favoriteEmote4;
vmCvar_t	ui_favoriteEmote5;
vmCvar_t	ui_favoriteEmote6;
vmCvar_t	ui_favoriteEmote7;
vmCvar_t	ui_favoriteEmote8;
vmCvar_t	ui_favoriteEmote9;
vmCvar_t	ui_favoriteEmote10;
vmCvar_t	ui_favoriteEmote11;
vmCvar_t	ui_favoriteEmote12;
vmCvar_t	ui_favoriteEmote13;
vmCvar_t	ui_favoriteEmote14;
vmCvar_t	ui_favoriteEmote15;
vmCvar_t	ui_favoriteEmote16;

//TiM - SecurityCode
vmCvar_t	sv_securityHash;
vmCvar_t	sv_securityCode;

//Widescreen support
vmCvar_t	ui_handleWidescreen;

static cvarTable_t		cvarTable[] = {
	{ &ui_ffa_fraglimit, "ui_ffa_fraglimit", "20", CVAR_ARCHIVE },
	{ &ui_ffa_timelimit, "ui_ffa_timelimit", "0", CVAR_ARCHIVE },

	{ &ui_tourney_fraglimit, "ui_tourney_fraglimit", "0", CVAR_ARCHIVE },
	{ &ui_tourney_timelimit, "ui_tourney_timelimit", "15", CVAR_ARCHIVE },

	{ &ui_team_fraglimit, "ui_team_fraglimit", "0", CVAR_ARCHIVE },
	{ &ui_team_timelimit, "ui_team_timelimit", "20", CVAR_ARCHIVE },
	{ &ui_team_friendly, "ui_team_friendly",  "1", CVAR_ARCHIVE },

	{ &ui_ctf_capturelimit, "ui_ctf_capturelimit", "8", CVAR_ARCHIVE },
	{ &ui_ctf_timelimit, "ui_ctf_timelimit", "30", CVAR_ARCHIVE },
	{ &ui_ctf_friendly, "ui_ctf_friendly",  "0", CVAR_ARCHIVE },

	{ &ui_arenasFile, "g_arenasFile", "", CVAR_INIT|CVAR_ROM },
	{ &ui_botsFile, "g_botsFile", "", CVAR_INIT|CVAR_ROM },				// Used to have CVAR_ARCHIVE.
	{ &ui_botminplayers, "bot_minplayers","0", CVAR_SERVERINFO },
	{ &ui_spScores1, "g_spScores1", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spScores2, "g_spScores2", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spScores3, "g_spScores3", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spScores4, "g_spScores4", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spScores5, "g_spScores5", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spAwards, "g_spAwards", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spVideos, "g_spVideos", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spSkill, "g_spSkill", "2", CVAR_ARCHIVE | CVAR_LATCH },

	{ &ui_weaponrespawn, "g_weaponrespawn", "5", CVAR_ARCHIVE},
	{ &ui_speed, "g_speed", "250", CVAR_SERVERINFO | CVAR_ARCHIVE},
	{ &ui_gravity, "g_gravity", "800", CVAR_SERVERINFO | CVAR_ARCHIVE},
	{ &ui_knockback, "g_knockback", "500", CVAR_ARCHIVE},
	{ &ui_dmgmult, "g_dmgmult", "1", CVAR_ARCHIVE},
	{ &ui_adaptRespawn, "g_adaptrespawn", "1", CVAR_ARCHIVE},
	{ &ui_holoIntro, "g_holoIntro", "1", CVAR_ARCHIVE},
	{ &ui_forcerespawn, "g_forcerespawn", "0", CVAR_ARCHIVE },
	{ &ui_respawnGhostTime, "g_ghostRespawn", "5", CVAR_ARCHIVE },
	{ &ui_dowarmup, "g_dowarmup", "0", CVAR_ARCHIVE },
	{ &ui_warmup, "g_warmup", "20", CVAR_ARCHIVE },
	{ &ui_team_race_blue, "g_team_group_blue", "", CVAR_LATCH},			// Used to have CVAR_ARCHIVE
	{ &ui_team_race_red, "g_team_group_red", "", CVAR_LATCH},			// Used to have CVAR_ARCHIVE

	{ &ui_pModAssimilation, "g_pModAssimilation", "0", CVAR_SERVERINFO | CVAR_LATCH },
	{ &ui_pModDisintegration, "g_pModDisintegration", "0", CVAR_SERVERINFO | CVAR_LATCH },
	{ &ui_pModActionHero, "g_pModActionHero", "0", CVAR_SERVERINFO | CVAR_LATCH },
	{ &ui_pModSpecialties, "g_pModSpecialties", "0", CVAR_SERVERINFO | CVAR_LATCH },
	{ &ui_pModElimination, "g_pModElimination", "0", CVAR_SERVERINFO | CVAR_LATCH },

	{ &ui_spSelection, "ui_spSelection", "", CVAR_ROM },

	{ &ui_browserMaster, "ui_browserMaster", "0", CVAR_ARCHIVE },
	{ &ui_browserGameType, "ui_browserGameType", "0", CVAR_ARCHIVE },
	{ &ui_browserSortKey, "ui_browserSortKey", "4", CVAR_ARCHIVE },
	{ &ui_browserShowFull, "ui_browserShowFull", "1", CVAR_ARCHIVE },
	{ &ui_browserShowEmpty, "ui_browserShowEmpty", "1", CVAR_ARCHIVE },

	{ &ui_drawCrosshair, "cg_drawCrosshair", "1", CVAR_ARCHIVE },
	{ &ui_drawCrosshairNames, "cg_drawCrosshairNames", "1", CVAR_ARCHIVE },
	{ &ui_marks, "cg_marks", "1", CVAR_ARCHIVE },

	{ &ui_server1, "server1", "", CVAR_ARCHIVE },
	{ &ui_server2, "server2", "", CVAR_ARCHIVE },
	{ &ui_server3, "server3", "", CVAR_ARCHIVE },
	{ &ui_server4, "server4", "", CVAR_ARCHIVE },
	{ &ui_server5, "server5", "", CVAR_ARCHIVE },
	{ &ui_server6, "server6", "", CVAR_ARCHIVE },
	{ &ui_server7, "server7", "", CVAR_ARCHIVE },
	{ &ui_server8, "server8", "", CVAR_ARCHIVE },
	{ &ui_server9, "server9", "", CVAR_ARCHIVE },
	{ &ui_server10, "server10", "", CVAR_ARCHIVE },
	{ &ui_server11, "server11", "", CVAR_ARCHIVE },
	{ &ui_server12, "server12", "", CVAR_ARCHIVE },
	{ &ui_server13, "server13", "", CVAR_ARCHIVE },
	{ &ui_server14, "server14", "", CVAR_ARCHIVE },
	{ &ui_server15, "server15", "", CVAR_ARCHIVE },
	{ &ui_server16, "server16", "", CVAR_ARCHIVE },
	{ &ui_server17, "server17", "", CVAR_ARCHIVE },
	{ &ui_server18, "server18", "", CVAR_ARCHIVE },
	{ &ui_server19, "server19", "", CVAR_ARCHIVE },
	{ &ui_server20, "server20", "", CVAR_ARCHIVE },
	{ &ui_server21, "server21", "", CVAR_ARCHIVE },
	{ &ui_server22, "server22", "", CVAR_ARCHIVE },
	{ &ui_server23, "server23", "", CVAR_ARCHIVE },
	{ &ui_server24, "server24", "", CVAR_ARCHIVE },
	{ &ui_server25, "server25", "", CVAR_ARCHIVE },
	{ &ui_server26, "server26", "", CVAR_ARCHIVE },
	{ &ui_server27, "server27", "", CVAR_ARCHIVE },
	{ &ui_server28, "server28", "", CVAR_ARCHIVE },
	{ &ui_server29, "server29", "", CVAR_ARCHIVE },
	{ &ui_server30, "server30", "", CVAR_ARCHIVE },
	{ &ui_server31, "server31", "", CVAR_ARCHIVE },
	{ &ui_server32, "server32", "", CVAR_ARCHIVE },

	{ &ui_cdkeychecked, "ui_cdkeychecked", "0", CVAR_ARCHIVE | CVAR_NORESTART},
	{ &ui_cdkeychecked2, "ui_cdkeychecked2", "0", CVAR_ROM},
	{ &ui_language, "g_language", "", CVAR_ARCHIVE | CVAR_NORESTART},
	{ &ui_s_language, "s_language", "", CVAR_ARCHIVE | CVAR_NORESTART},
	{ &ui_k_language, "k_language", "", CVAR_ARCHIVE | CVAR_NORESTART},
	{ &ui_playerClass, "ui_playerClass", "noclass", CVAR_ARCHIVE /*| CVAR_ROM*/ | CVAR_USERINFO },
	{ &ui_playerRank, "ui_playerRank", "crewman", CVAR_ARCHIVE /*| CVAR_ROM*/ | CVAR_USERINFO }, //RPG-X: TiM- Rank
//	{ &ui_precacheweapons, "ui_precacheweapons", "1", CVAR_ARCHIVE},

	{ &ui_allowvote, "rpg_allowVote", "1", CVAR_ARCHIVE },
	{ &ui_chatsallowed, "rpg_chatsAllowed", "10", CVAR_ARCHIVE },
	{ &ui_allowsuicide, "rpg_allowSuicide", "1", CVAR_ARCHIVE },
	{ &ui_selfdamage, "rpg_selfDamage", "1", CVAR_ARCHIVE},
	{ &ui_rpg, "rpg_rpg", "1", CVAR_ARCHIVE},
	{ &ui_kickspammers, "rpg_kickSpammers", "0", CVAR_ARCHIVE},
	{ &ui_kicksuiciders, "rpg_kickSuiciders", "0", CVAR_ARCHIVE},
	{ &ui_allowspmaps, "rpg_allowSPMaps", "0", CVAR_ARCHIVE},
	{ &ui_rangetricorder, "rpg_rangeTricorder", "128", CVAR_ARCHIVE},
	{ &ui_rangehypo, "rpg_rangeHypo", "32", CVAR_ARCHIVE},
	{ &ui_norpgclasses, "rpg_noRPGClasses", "0", CVAR_ARCHIVE},
	{ &ui_forceclasscolor, "rpg_forceClassColor", "0", CVAR_ARCHIVE},
	{ &ui_nosecurity, "rpg_noSecurity", "0", CVAR_ARCHIVE},
	{ &ui_nomarine, "rpg_noMarine", "0", CVAR_ARCHIVE},
	{ &ui_nomedical, "rpg_noMedical", "0", CVAR_ARCHIVE},
	{ &ui_noscience, "rpg_noScience", "0", CVAR_ARCHIVE},
	{ &ui_nocommand, "rpg_noCommand", "0", CVAR_ARCHIVE},
	{ &ui_noengineer, "rpg_noEngineer", "0", CVAR_ARCHIVE},
	{ &ui_noalien, "rpg_noAlien", "0", CVAR_ARCHIVE},
	{ &ui_nomaker, "rpg_noMaker", "0", CVAR_ARCHIVE},
	{ &ui_nocloak, "rpg_noCloak", "0", CVAR_ARCHIVE},
	{ &ui_noflight, "rpg_noFlight", "0", CVAR_ARCHIVE},
	{ &ui_phaserdmg, "rpg_phaserDmg", "1", CVAR_ARCHIVE},
	{ &ui_rifledmg, "rpg_rifleDmg", "1", CVAR_ARCHIVE},
	{ &ui_stasisdmg, "rpg_stasisDmg", "1", CVAR_ARCHIVE},
	{ &ui_imoddmg, "rpg_imodDmg", "1", CVAR_ARCHIVE},
	{ &ui_noweapons, "rpg_noWeapons", "0", CVAR_ARCHIVE},
	{ &ui_alienflags, "rpg_alienFlags", "1026", CVAR_ARCHIVE }, //16
	{ &ui_marineflags, "rpg_marineFlags", "12398", CVAR_ARCHIVE }, //111
	{ &ui_securityflags, "rpg_securityFlags", "110", CVAR_ARCHIVE }, //15
	{ &ui_makerflags, "rpg_adminFlags", "65534", CVAR_ARCHIVE }, //24361
	{ &ui_medicalflags, "rpg_medicalFlags", "14382", CVAR_ARCHIVE }, //231
	{ &ui_scienceflags, "rpg_scienceFlags", "46", CVAR_ARCHIVE }, //7
	{ &ui_commandflags, "rpg_commandFlags", "62", CVAR_ARCHIVE }, //6
	{ &ui_engineerflags, "rpg_engineerFlags", "49198", CVAR_ARCHIVE }, //775
	{ &ui_welcomemessage, "rpg_welcomeMessage", "Welcome to the RPG-X Mod", CVAR_ARCHIVE },
	{ &ui_timedmessage, "rpg_timedMessage", "Server is in: Character Development Mode", CVAR_ARCHIVE },
	{ &ui_timedmessagetime, "rpg_timedMessageTime", "5", CVAR_ARCHIVE },
	{ &ui_kickAfterXkills, "rpg_kickAfterXkills", "2", CVAR_ARCHIVE },
//	{ &ui_lastactive, "sys_lastactive", "0", CVAR_ARCHIVE }, //RPG-X | Phenix | 25/02/2005 (Trying to make a timer for intro)
	//TiM : RPG-X variables 8-8-2005
	{ &ui_dynamicLensFlares, "cg_dynamicLensFlares", "1", CVAR_ARCHIVE },
	{ &ui_dynamicCrosshair, "cg_dynamicCrosshair", "1", CVAR_ARCHIVE }, 
	{ &ui_currentRankSet, "ui_currentRankSet", RANKSET_DEFAULT, CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_currentClassSet, "ui_currentClassSet", CLASS_DEFAULT, CVAR_ARCHIVE | CVAR_ROM },

	//TiM: Local UI versions of the PMS parameters
	{ &ui_age, "age", "Unknown", CVAR_ARCHIVE | CVAR_USERINFO },
	{ &ui_height, "height", "1.0", CVAR_ARCHIVE | CVAR_USERINFO },
	{ &ui_weight, "weight", "1.0", CVAR_ARCHIVE | CVAR_USERINFO },
	{ &ui_race, "race", "Unknown", CVAR_ARCHIVE | CVAR_USERINFO },

	{ &ui_defaultChar, "cg_defaultChar", DEFAULT_CHAR, CVAR_ARCHIVE },

	//TiM: Emote CVARS - Recently played Emotes
	{ &ui_recentEmote1, "ui_recentEmote1", "-1", CVAR_ARCHIVE },
	{ &ui_recentEmote2, "ui_recentEmote2", "-1", CVAR_ARCHIVE },
	{ &ui_recentEmote3, "ui_recentEmote3", "-1", CVAR_ARCHIVE },
	{ &ui_recentEmote4, "ui_recentEmote4", "-1", CVAR_ARCHIVE },
	{ &ui_recentEmote5, "ui_recentEmote5", "-1", CVAR_ARCHIVE },
	{ &ui_recentEmote6, "ui_recentEmote6", "-1", CVAR_ARCHIVE },
	{ &ui_recentEmote7, "ui_recentEmote7", "-1", CVAR_ARCHIVE },
	{ &ui_recentEmote8, "ui_recentEmote8", "-1", CVAR_ARCHIVE },
	{ &ui_recentEmote9, "ui_recentEmote9", "-1", CVAR_ARCHIVE },
	{ &ui_recentEmote10, "ui_recentEmote10", "-1", CVAR_ARCHIVE },
	{ &ui_recentEmote11, "ui_recentEmote11", "-1", CVAR_ARCHIVE },
	{ &ui_recentEmote12, "ui_recentEmote12", "-1", CVAR_ARCHIVE },
	{ &ui_recentEmote13, "ui_recentEmote13", "-1", CVAR_ARCHIVE },
	{ &ui_recentEmote14, "ui_recentEmote14", "-1", CVAR_ARCHIVE },
	{ &ui_recentEmote15, "ui_recentEmote15", "-1", CVAR_ARCHIVE },
	{ &ui_recentEmote16, "ui_recentEmote16", "-1", CVAR_ARCHIVE },

	//Emote CVARS - Favorites
	{ &ui_favoriteEmote1, "ui_favoriteEmote1", "-1", CVAR_ARCHIVE },
	{ &ui_favoriteEmote2, "ui_favoriteEmote2", "-1", CVAR_ARCHIVE },
	{ &ui_favoriteEmote3, "ui_favoriteEmote3", "-1", CVAR_ARCHIVE },
	{ &ui_favoriteEmote4, "ui_favoriteEmote4", "-1", CVAR_ARCHIVE },
	{ &ui_favoriteEmote5, "ui_favoriteEmote5", "-1", CVAR_ARCHIVE },
	{ &ui_favoriteEmote6, "ui_favoriteEmote6", "-1", CVAR_ARCHIVE },
	{ &ui_favoriteEmote7, "ui_favoriteEmote7", "-1", CVAR_ARCHIVE },
	{ &ui_favoriteEmote8, "ui_favoriteEmote8", "-1", CVAR_ARCHIVE },
	{ &ui_favoriteEmote9, "ui_favoriteEmote9", "-1", CVAR_ARCHIVE },
	{ &ui_favoriteEmote10, "ui_favoriteEmote10", "-1", CVAR_ARCHIVE },
	{ &ui_favoriteEmote11, "ui_favoriteEmote11", "-1", CVAR_ARCHIVE },
	{ &ui_favoriteEmote12, "ui_favoriteEmote12", "-1", CVAR_ARCHIVE },
	{ &ui_favoriteEmote13, "ui_favoriteEmote13", "-1", CVAR_ARCHIVE },
	{ &ui_favoriteEmote14, "ui_favoriteEmote14", "-1", CVAR_ARCHIVE },
	{ &ui_favoriteEmote15, "ui_favoriteEmote15", "-1", CVAR_ARCHIVE },
	{ &ui_favoriteEmote16, "ui_favoriteEmote16", "-1", CVAR_ARCHIVE },

	{ &sv_securityHash, "sv_securityHash", "4294967295", CVAR_ARCHIVE | CVAR_ROM | CVAR_NORESTART },
	{ &sv_securityCode, "sv_securityCode", "4294967295", CVAR_ARCHIVE | CVAR_USERINFO | CVAR_ROM | CVAR_NORESTART },

	{ &ui_handleWidescreen, "ui_handleWidescreen", "1", CVAR_ARCHIVE },
};

static int	cvarTableSize = sizeof(cvarTable) / sizeof(cvarTable[0]);

/*
=================
UI_RegisterCvars
=================
*/
void UI_RegisterCvars( void ) {
	int			i;
	cvarTable_t	*cv;
	
	//RPG-X: RedTechie - Keep no class default
	//RPG-X: TiM - just commented this out for now to see if I can add class support to the main UI
//	trap_Cvar_Set( "ui_playerclass", "NOCLASS" );
	
	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		trap_Cvar_Register( cv->vmCvar, cv->cvarName, cv->defaultString, cv->cvarFlags );
	}
}

/*
=================
UI_UpdateCvars
=================
*/
void UI_UpdateCvars( void ) {
	int			i;
	cvarTable_t	*cv;

	//TiM - unexplained engine bug
	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		if ( cv->vmCvar->handle >= 1024 || cv->vmCvar->handle <= 0 )
		{
			//Com_Printf( S_COLOR_RED "ERROR: CVAR: %s Handle: %i \n", cv->cvarName, cv->vmCvar->handle );
		}
		else
			trap_Cvar_Update( cv->vmCvar );
	}
}

//TiM I wanted to make this linked in the function in pMove, but it seems impossible to get the right scope
//So I'll make a local version here

int UI_GetAnim ( int anim, int weapon, qboolean upper )
{
	// Called when player is in idle crouching
	switch ( anim ) {
		case ANIM_CROUCH:
			//2 handed weapon - "heavy"
			switch (weapon) {
				case WP_7:
				case WP_8:
				case WP_9:
					if (upper)
						return BOTH_STAND2;
					else
						return LEGS_KNEEL1;
					break;
				//2 handed weapon - "light"
				case WP_6:
				//case WP_7:
					if (upper)
						return TORSO_WEAPONREADY2;
					else
						return LEGS_KNEEL1;
					break;
				//1 handed weapon - "phaser"
				case WP_5:
				case WP_10:
					if ( upper )
						return TORSO_WEAPONPOSE1;
					else 
						return BOTH_CROUCH1IDLE;
					break;
				case WP_4:
					if (upper)
						return TORSO_COFFEE;
					//break;
				//Generic tools - "everything else"
				default:
					return BOTH_CROUCH2IDLE;
					break;
			}
			break;

		//Called when player is in idle standing
		case ANIM_IDLE:
			//2 handed weapon - "heavy"
			switch (weapon) {
				//case WP_7:
				case WP_8:
				case WP_9:
				case WP_7:
					return BOTH_STAND4;
					break;
				//2 handed weapon - "light"
				case WP_6:
					return BOTH_STAND4;
					break;
				//1 handed weapon - "phaser"
				case WP_5:
				case WP_10:
					if (upper)
						return BOTH_STAND1; //TORSO_WEAPONIDLE1
					else
						return BOTH_STAND1;
					break;
				//Generic tools - "everything else"
				case WP_4:
					if (upper)
						return TORSO_COFFEE;
					else
						return BOTH_STAND1;
					break;
				default:
					return BOTH_STAND1;
					break;
			}
			break;

		//Called when player fires their weapon
		case ANIM_ATTACK:
			//2 handed weapon - "heavy"
			switch (weapon) {
				//case WP_7:
				case WP_8:
				case WP_9:
				case WP_7:
					return BOTH_ATTACK3;
					break;
				//2 handed weapon - "light"
				case WP_6:
					if (upper)
						return BOTH_ATTACK2;
					else
						return BOTH_ATTACK3;
					break;
				//1 handed weapon - "phaser"
				case WP_5:
				case WP_10:
					if (upper)
						return TORSO_WEAPONREADY1; 
					else
						return BOTH_STAND1;
					break;
			//Other Tools "padd"
			case WP_3:
				if (upper)
					return TORSO_PADD1;
				else
					return BOTH_STAND1;
				break;
			//Other Tools "tricorder"
			case WP_2:
				if (upper)
					return TORSO_TRICORDER1;
				else
					return BOTH_STAND1;
				break;
			//Other: "Medkit"
			case WP_11:
				if (upper)
					return TORSO_ACTIVATEMEDKIT1;
				else
					return BOTH_STAND1;
				break;
			//Other: "Hypo
			case WP_12:
				if (upper)
					return TORSO_HYPOSPRAY1;
				else
					return BOTH_STAND1;
			//Other: "Toolkit"
			/*case WP_14:
				//Return nothing.  
				//A bit hacky, but the engine accepts it :P
				break;*/
			//Other Tools "everything else"
			/*case WP_1:
				switch(rand()%13)
				{
					case 0: return TORSO_HANDGESTURE1;
					case 1: return TORSO_HANDGESTURE2;
					case 2: return TORSO_HANDGESTURE3;
					case 3: return TORSO_HANDGESTURE4;
					case 4: //PM_StartTorsoAnim( TORSO_HANDGESTURE5 ); break;
					case 5: return TORSO_HANDGESTURE6;
					case 6: return TORSO_HANDGESTURE7;
					case 7: return TORSO_HANDGESTURE8;
					case 8: return TORSO_HANDGESTURE9;
					case 9: return TORSO_HANDGESTURE10;
					case 10: return TORSO_HANDGESTURE11;
					case 11: return TORSO_HANDGESTURE12;
					case 12: return TORSO_HANDGESTURE13;
				}
				break;*/
			case WP_4:
				if (upper)
					return TORSO_COFFEE;
				//break;
			default:
				if (upper)
					return TORSO_WEAPONREADY1; 
				else
					return BOTH_STAND1;
				break;
		}
		break;

		//When the player jumps
		case ANIM_JUMP:
			return BOTH_JUMP1;
	
		//When the player runs
		case ANIM_RUN:
			//2 handed weapons
			switch (weapon) {
				//case WP_7:
				case WP_8:
				case WP_9:
				case WP_6:
				case WP_7:
					if (upper)
						return BOTH_RUN2;
					else
						return BOTH_RUN1;
					break;
				case WP_4:
					if (upper)
						return TORSO_COFFEE;
					//break;
				//EVERYTHING ELSE
				default:
					return BOTH_RUN1;
			}
			break;

		//When the player walks
		case ANIM_WALK:
			//2 handed weapons
			switch (weapon) {
				//case WP_7:
				case WP_8:
				case WP_9:
				case WP_6:
				case WP_7:
					return BOTH_WALK4;
					break;
				//Other Tools "everything else"
				case WP_4:
					if (upper)
						return TORSO_COFFEE;
					//break;
				default:
					return BOTH_WALK1;
					break;
			}
			break;
		
		//When the player walks
		case ANIM_BACK:
			//2 handed weapons
			switch (weapon) 
			{
				//case WP_7:
				case WP_8:
				case WP_9:
				case WP_6:
				case WP_7:
					if ( upper )
						return BOTH_WALK4;
					else
						return LEGS_WALKBACK1;
					break;
				case WP_4:
					if (upper)
						return TORSO_COFFEE;
					//break;
				//Other Tools "everything else"
				default:
					if ( upper )
						return BOTH_WALK1;
					else
						return LEGS_WALKBACK1;
					break;
			}
			break;
	}

	return BOTH_STAND1;
}


