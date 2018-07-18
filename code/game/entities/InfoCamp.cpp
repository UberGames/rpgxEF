#include "InfoCamp.h"

namespace game::entities
{
  InfoCamp::InfoCamp(GameSharedEntity* sharedEntity, Entity* parent): Entity{EntityType::ENT_INFO_CAMP, "info_camp", sharedEntity, parent}
  {
    setOrigin(m_sharedEntity->s.origin);
  }
}
