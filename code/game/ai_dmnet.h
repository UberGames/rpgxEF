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

#define MAX_NODESWITCHES	50

void AIEnter_Intermission(bot_state_t *bs);
void AIEnter_Observer(bot_state_t *bs);
void AIEnter_Respawn(bot_state_t *bs);
void AIEnter_Stand(bot_state_t *bs);
void AIEnter_Seek_ActivateEntity(bot_state_t *bs);
void AIEnter_Seek_NBG(bot_state_t *bs);
void AIEnter_Seek_LTG(bot_state_t *bs);
void AIEnter_Seek_Camp(bot_state_t *bs);
void AIEnter_Battle_Fight(bot_state_t *bs);
void AIEnter_Battle_Chase(bot_state_t *bs);
void AIEnter_Battle_Retreat(bot_state_t *bs);
void AIEnter_Battle_NBG(bot_state_t *bs);
int AINode_Intermission(bot_state_t *bs);
int AINode_Observer(bot_state_t *bs);
int AINode_Respawn(bot_state_t *bs);
int AINode_Stand(bot_state_t *bs);
int AINode_Seek_ActivateEntity(bot_state_t *bs);
int AINode_Seek_NBG(bot_state_t *bs);
int AINode_Seek_LTG(bot_state_t *bs);
int AINode_Battle_Fight(bot_state_t *bs);
int AINode_Battle_Chase(bot_state_t *bs);
int AINode_Battle_Retreat(bot_state_t *bs);
int AINode_Battle_NBG(bot_state_t *bs);

void BotResetNodeSwitches(void);
void BotDumpNodeSwitches(bot_state_t *bs);

