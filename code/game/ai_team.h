// Copyright (C) 1999-2000 Id Software, Inc.
//

/*****************************************************************************
 * name:		ai_team.h
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /StarTrek/Code-DM/game/ai_team.h $
 * $Author: Jmonroe $ 
 * $Revision: 1 $
 * $Modtime: 1/21/00 10:12p $
 * $Date: 1/25/00 6:26p $
 *
 *****************************************************************************/

#ifndef AI_TEAM_H_
#define AI_TEAM_H_

void BotTeamAI(bot_state_t *bs);
int BotGetTeamMateCTFPreference(bot_state_t *bs, int teammate);
void BotSetTeamMateCTFPreference(bot_state_t *bs, int teammate, int preference);

#endif /* AI_TEAM_H_ */

