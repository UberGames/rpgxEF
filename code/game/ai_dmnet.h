// Copyright (C) 1999-2000 Id Software, Inc.
//

/*****************************************************************************
 * name:		ai_dmnet.h
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /StarTrek/Code-DM/game/ai_dmnet.h $
 * $Author: Jmonroe $ 
 * $Revision: 1 $
 * $Modtime: 1/21/00 10:12p $
 * $Date: 1/25/00 6:26p $
 *
 *****************************************************************************/

#ifndef AI_DMNET_H_
#define AI_DMNET_H_

#define MAX_NODESWITCHES 50

void AI_dmnet_AIEnter_Stand(bot_state_t* bs);
void AI_dmnet_AIEnter_Seek_ActivateEntity(bot_state_t* bs);
void AI_dmnet_AIEnter_Seek_LTG(bot_state_t* bs);
int32_t AI_dmnet_AINode_Stand(bot_state_t* bs);
int32_t AI_dmnat_AINode_Seek_NBG(bot_state_t* bs);
int32_t AI_dmnet_AINode_Seek_LTG(bot_state_t* bs);

void AI_dmnet_BotResetNodeSwitches(void);
void AI_dmnet_BotDumpNodeSwitches(bot_state_t* bs);

#endif /* AI_DMNET_H_ */


