//
// Created by Walter on 06.08.2020.
//

#ifndef RPGXEF_CODE_GAME_AI_MAIN_H_
#define RPGXEF_CODE_GAME_AI_MAIN_H_

#include "../q_shared.h"
#include "../ai_main.h"

namespace ai::main {

void QDECL QDECL BotInitialChat(bot_state_t *bs, std::string_view type, ...);

}

#endif//RPGXEF_CODE_GAME_AI_MAIN_H_
