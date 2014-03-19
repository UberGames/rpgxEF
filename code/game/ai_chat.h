// Copyright (C) 1999-2000 Id Software, Inc.
//

/*****************************************************************************
 * name:		ai_chat.h
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /StarTrek/Code-DM/game/ai_chat.h $
 * $Author: Jmonroe $ 
 * $Revision: 1 $
 * $Modtime: 1/21/00 10:12p $
 * $Date: 1/25/00 6:26p $
 *
 *****************************************************************************/

#ifndef AI_CHAT_H_
#define AI_CHAT_H_

//
int32_t BotChat_EnterGame(bot_state_t* bs);
//
int32_t BotChat_ExitGame(bot_state_t* bs);
//
int32_t BotChat_StartLevel(bot_state_t* bs);
//
int32_t BotChat_EndLevel(bot_state_t* bs);
//
int32_t BotChat_HitTalking(bot_state_t* bs);
//
int32_t BotChat_HitNoDeath(bot_state_t* bs);
//
int32_t BotChat_HitNoKill(bot_state_t* bs);
//
int32_t BotChat_Death(bot_state_t* bs);
//
int32_t BotChat_Kill(bot_state_t* bs);
//
int32_t BotChat_EnemySuicide(bot_state_t* bs);
//
int32_t BotChat_Random(bot_state_t* bs);
//! time the selected chat takes to type in
double BotChatTime(bot_state_t* bs);
//! returns true if the bot can chat at the current position
int32_t BotValidChatPosition(bot_state_t* bs);
//! test the initial bot chats
void BotChatTest(bot_state_t* bs);

#endif /* AI_CHAT_H_ */


