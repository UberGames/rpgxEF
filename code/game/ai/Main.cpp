//
// Created by Walter on 06.08.2020.
//

#include "Main.h"
#include "../ai_dmq3.h"
#include "../be_ai_chat.h"
#include "../g_local.h"
#include "../syn.h"
#include <array>

namespace ai::main {

void QDECL QDECL BotInitialChat(bot_state_t *bs, std::string_view type, ...) {
  auto vars = std::array<char *, MAX_MATCHVARIABLES>{};
  va_list ap;
  va_start(ap, type);
  auto *p = va_arg(ap, char *);
  for (auto i = 0; i < MAX_MATCHVARIABLES; i++) {
    if (!p) {
      break;
    }
    vars[i] = p;
    p = va_arg(ap, char *);
  }
  va_end(ap);
  auto mcontext = CONTEXT_NORMAL | CONTEXT_NEARBYITEM | CONTEXT_NAMES;
  if (BotCTFTeam(bs) == CTF_TEAM_RED) {
    mcontext |= CONTEXT_CTFREDTEAM;
  } else {
    mcontext |= CONTEXT_CTFBLUETEAM;
  }

  trap_BotInitialChat(bs->cs, type.data(), mcontext, vars[0], vars[1], vars[2], vars[3], vars[4], vars[5], vars[6], vars[7]);
}

}// namespace ai::main