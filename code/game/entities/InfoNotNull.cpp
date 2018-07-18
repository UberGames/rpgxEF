#include "InfoNotNull.h"

namespace game::entities
{
  InfoNotNull::InfoNotNull(GameSharedEntity* sharedEntity, Entity* parent) : Entity{EntityType::ENT_INFO_NOTNULL, "info_notnull", sharedEntity, parent}
  {
    setOrigin(m_sharedEntity->s.origin);
  }
}
