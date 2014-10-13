// Copyright (C) 1999-2000 Id Software, Inc.
//

/*****************************************************************************
 * name:		ai_cmd.h
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /StarTrek/Code-DM/game/ai_cmd.h $
 * $Author: Jmonroe $ 
 * $Revision: 1 $
 * $Modtime: 1/21/00 10:12p $
 * $Date: 1/25/00 6:26p $
 *
 *****************************************************************************/

#ifndef AI_CMD_H_
#define AI_CMD_H_

#include "g_local.h"

int32_t BotMatchMessage(bot_state_t* bs, char* message);

void BotPrintTeamGoal(bot_state_t* bs);

#endif /* AI_CMD_H_ */


