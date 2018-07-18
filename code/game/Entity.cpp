#include "Entity.h"
#include "g_syscalls.h"

namespace game
{
  bool Entity::addEvent(int32_t event, int32_t eventParm, AfterEventAction afterEventAction)
  {
    if(event == 0)
    {
      return false;
    }

    auto bits = m_sharedEntity->s.event & EV_EVENT_BITS;
    bits = (bits + EV_EVENT_BIT1) & EV_EVENT_BITS;
    m_sharedEntity->s.event = event | bits;
    m_sharedEntity->s.eventParm = eventParm;
    m_afterEventAction = afterEventAction;
    m_eventTime = level.time;
    return true;
  }

  void Entity::setOrigin(vec3_t origin)
  {
    VectorCopy(origin, m_sharedEntity->s.pos.trBase);
    m_sharedEntity->s.pos.trType = TR_STATIONARY;
    m_sharedEntity->s.pos.trTime = 0;
    m_sharedEntity->s.pos.trDuration = 0;
    VectorClear(m_sharedEntity->s.pos.trDelta);

    VectorCopy(origin, m_sharedEntity->r.currentOrigin);
    VectorCopy(origin, m_sharedEntity->s.origin);
  }

  void Entity::setAngles(vec3_t a)
  {
    VectorCopy(a, m_sharedEntity->s.apos.trBase);
    m_sharedEntity->s.apos.trType = TR_STATIONARY;
    m_sharedEntity->s.apos.trTime = 0;
    m_sharedEntity->s.apos.trDuration = 0;
    VectorClear(m_sharedEntity->s.apos.trDelta);

    VectorCopy(a, m_sharedEntity->r.currentAngles);
    VectorCopy(a, m_sharedEntity->s.angles);
  }

  void Entity::free()
  {
    // TODO: free actual shared entity
    trap_UnlinkEntity(m_sharedEntity);

    if(m_neverFree)
    {
      return;
    }

    // TODO: lua hook

    memset(m_sharedEntity, 0, sizeof(GameSharedEntity));
    m_sharedEntity->m_inUse = false;
    m_sharedEntity->m_freeTime = level.time;

    delete this;
  }

  bool Entity::useTargets()
  {
    // TODO: implement me
    return true;
  }

  bool Entity::useTargets(const Entity& activator)
  {
    // TODO: implement me
    return true;
  }

  bool Entity::useTargets(std::string_view target)
  {
    // TODO: implement me
    return true;
  }

  bool Entity::useTargets(const Entity& activator, std::string_view target)
  {
    // TODO: implement me
    return true;
  }
}
