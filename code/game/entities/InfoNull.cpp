#include "InfoNull.h"

namespace game::entities
{
  InfoNull::InfoNull(GameSharedEntity* sharedEntity, Entity* parent) : Entity{EntityType::ENT_INFO_NULL, "info_null", sharedEntity, parent}
  {
    free();
  }
}
