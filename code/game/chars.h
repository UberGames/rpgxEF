// Copyright (C) 1999-2000 Id Software, Inc.
//
//===========================================================================
//
// Name:			chars.h
// Function:		bot characteristics
// Programmer:		Mr Elusive (MrElusive@idsoftware.com)
// Last update:		1999-09-08
// Tab Size:		4 (real tabs)
//===========================================================================

#ifndef CHARS_H_
#define CHARS_H_

//========================================================
//========================================================
enum charCharactetistic_e {
	//name
	CHARACTERISTIC_NAME,	//string
	//gender of the bot
	CHARACTERISTIC_GENDER,	//string ("male", "female", "it")
	//attack skill
	// >  0.0 && <  0.2 = don't move
	// >  0.3 && <  1.0 = aim at enemy during retreat
	// >  0.0 && <  0.4 = only move forward/backward
	// >= 0.4 && <  1.0 = circle strafing
	// >  0.7 && <  1.0 = random strafe direction change
	CHARACTERISTIC_ATTACK_SKILL,	//float [0, 1]
	//weapon weight file
	CHARACTERISTIC_WEAPONWEIGHTS,	//string
	//view angle difference to angle change factor
	CHARACTERISTIC_VIEW_FACTOR,		//float <0, 1]
	//maximum view angle change
	CHARACTERISTIC_VIEW_MAXCHANGE,	//float [1, 360]
	//reaction time in seconds
	CHARACTERISTIC_REACTIONTIME,	//float [0, 5]
	//accuracy when aiming
	CHARACTERISTIC_AIM_ACCURACY,	//float [0, 1]
	//weapon specific aim accuracy
	CHARACTERISTIC_AIM_ACCURACY_GRENADELAUNCHER,	//float [0, 1]
	CHARACTERISTIC_AIM_ACCURACY_STASIS,				//float [0, 1]
	CHARACTERISTIC_AIM_ACCURACY_PHASER,				//float [0, 1]
	CHARACTERISTIC_AIM_ACCURACY_DREADNOUGHT,		//float [0, 1]
	CHARACTERISTIC_AIM_ACCURACY_IMOD,				//float [0, 1]
	CHARACTERISTIC_AIM_ACCURACY_COMPRESSION,		//float [0, 1]
	CHARACTERISTIC_AIM_ACCURACY_TETRION,			//float [0, 1]
	CHARACTERISTIC_AIM_ACCURACY_SCAVENGER,			//float [0, 1]
	CHARACTERISTIC_AIM_ACCURACY_QUANTUM,			//float [0, 1]

	//skill when aiming
	// >  0.0 && <  0.9 = aim is affected by enemy movement
	// >  0.4 && <= 0.8 = enemy linear leading
	// >  0.8 && <= 1.0 = enemy exact movement leading
	// >  0.5 && <= 1.0 = prediction shots when enemy is not visible
	// >  0.6 && <= 1.0 = splash damage by shooting nearby geometry
	CHARACTERISTIC_AIM_SKILL,						//float [0, 1]

	CHARACTERISTIC_AIM_SKILL_STASIS,				//float [0, 1]
	CHARACTERISTIC_AIM_SKILL_GRENADELAUNCHER,		//float [0, 1]
	CHARACTERISTIC_AIM_SKILL_QUANTUM,				//float [0, 1]


	//========================================================
	//chat
	//========================================================
	//file with chats
	CHARACTERISTIC_CHAT_FILE,						//string
	//name of the chat character
	CHARACTERISTIC_CHAT_NAME,						//string
	//characters per minute type speed
	CHARACTERISTIC_CHAT_CPM,						//integer [1, 4000]
	//tendency to insult/praise
	CHARACTERISTIC_CHAT_INSULT,						//float [0, 1]
	//tendency to chat misc
	CHARACTERISTIC_CHAT_MISC,						//float [0, 1]
	//tendency to chat at start or end of level
	CHARACTERISTIC_CHAT_STARTENDLEVEL,				//float [0, 1]
	//tendency to chat entering or exiting the game
	CHARACTERISTIC_CHAT_ENTEREXITGAME,				//float [0, 1]
	//tendency to chat when killed someone
	CHARACTERISTIC_CHAT_KILL,						//float [0, 1]
	//tendency to chat when died
	CHARACTERISTIC_CHAT_DEATH,						//float [0, 1]
	//tendency to chat when enemy suicides
	CHARACTERISTIC_CHAT_ENEMYSUICIDE,				//float [0, 1]
	//tendency to chat when hit while talking
	CHARACTERISTIC_CHAT_HITTALKING,					//float [0, 1]
	//tendency to chat when bot was hit but didn't die
	CHARACTERISTIC_CHAT_HITNODEATH,					//float [0, 1]
	//tendency to chat when bot hit the enemy but enemy didn't die
	CHARACTERISTIC_CHAT_HITNOKILL,					//float [0, 1]
	//tendency to randomly chat
	CHARACTERISTIC_CHAT_RANDOM,						//float [0, 1]
	//tendency to reply
	CHARACTERISTIC_CHAT_REPLY,						//float [0, 1]
	//========================================================
	//movement
	//========================================================
	//tendency to crouch
	CHARACTERISTIC_CROUCHER,						//float [0, 1]
	//tendency to jump
	CHARACTERISTIC_JUMPER,							//float [0, 1]
	//tendency to walk
	CHARACTERISTIC_WALKER,							//float [0, 1]
	//tendency to jump using a weapon
	CHARACTERISTIC_WEAPONJUMPING,					//float [0, 1]
	//========================================================
	//goal
	//========================================================
	//item weight file
	CHARACTERISTIC_ITEMWEIGHTS,						//string
	//the aggression of the bot
	CHARACTERISTIC_AGGRESSION,						//float [0, 1]
	//the self preservation of the bot (rockets near walls etc.)
	CHARACTERISTIC_SELFPRESERVATION,				//float [0, 1]
	//how likely the bot is to take revenge
	CHARACTERISTIC_VENGEFULNESS,					//float [0, 1]	//use this!!
	//tendency to camp
	CHARACTERISTIC_CAMPER,							//float [0, 1]
	//========================================================
	//========================================================
	//tendency to get easy frags
	CHARACTERISTIC_EASY_FRAGGER,					//float [0, 1]
	//how alert the bot is (view distance)
	CHARACTERISTIC_ALERTNESS,						//float [0, 1]
	//how much the bot fires it's weapon
	CHARACTERISTIC_FIRETHROTTLE						//float [0, 1]
};

#endif /* CHARS_H_ */
