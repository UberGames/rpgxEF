//
// Created by Walter on 19.06.2020.
//

#ifndef RPGXEF_CODE_GAME_AI_CHAT_H_
#define RPGXEF_CODE_GAME_AI_CHAT_H_

#include "../ai_main.h"
#include "../be_aas.h"
#include "../be_ai_goal.h"
#include "../botlib.h"
#include "../g_local.h"
#include "../q_shared.h"

namespace ai::chat {

[[nodiscard]] int numActivePlayers();
[[nodiscard]] bool isFirstInRankings(bot_state_t *bs);
[[nodiscard]] bool isLastInRankings(bot_state_t *bs);
[[nodiscard]] std::string nameOfFirstClientInRankings();
[[nodiscard]] std::string nameOfLastClientInRankings();
[[nodiscard]] std::string randomOpponentName(bot_state_s *bs);
[[nodiscard]] std::string mapTitle();
[[nodiscard]] std::string weaponName(int means_of_death);
[[nodiscard]] std::string randomWeaponName();
[[nodiscard]] bool visibleEnemies(bot_state_s *bs);
[[nodiscard]] bool validChatPosition(bot_state_s *bs);
[[nodiscard]] bool enterGame(bot_state_s *bs);
[[nodiscard]] bool exitGame(bot_state_s *bs);
[[nodiscard]] bool startLevel(bot_state_s *bs);

}// namespace ai::chat

#endif//RPGXEF_CODE_GAME_AI_CHAT_H_
