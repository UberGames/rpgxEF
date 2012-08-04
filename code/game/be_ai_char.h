// Copyright (C) 1999-2000 Id Software, Inc.
//

/*****************************************************************************
 * name:		be_ai_char.h
 *
 * desc:		bot characters
 *
 * $Archive: /StarTrek/Code-DM/game/be_ai_char.h $
 * $Author: Jmonroe $ 
 * $Revision: 1 $
 * $Modtime: 1/21/00 10:12p $
 * $Date: 1/25/00 6:26p $
 *
 *****************************************************************************/

//!loads a bot character from a file
int BotLoadCharacter(char *charfile, int skill);
//!frees a bot character
void BotFreeCharacter(int character);
//!float characteristic
float Characteristic_Float(int character, int index);
//!bounded float characteristic
float Characteristic_BFloat(int character, int index, float min, float max);
//!integer characteristic
int Characteristic_Integer(int character, int index);
//!bounded integer characteristic
int Characteristic_BInteger(int character, int index, int min, int max);
//!string characteristic
void Characteristic_String(int character, int index, char *buf, int size);
//!free cached bot characters
void BotShutdownCharacters(void);
