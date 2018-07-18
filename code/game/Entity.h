#pragma once

#include "g_local.h"
#include <variant>
#include <optional>

namespace game
{
  class Entity
  {
  public:
    Entity(const Entity&) = default;
    Entity(Entity&&) = default;

    Entity& operator=(const Entity&) = default;
    Entity& operator=(Entity&&) = default;

    virtual ~Entity() = default;

    bool m_neverFree = false;
    int32_t m_flags = 0;

    enum class AfterEventAction
    {
      None,
      Unlink,
      Free
    };

    int32_t eventTime() const { return m_eventTime; }
    virtual bool addEvent(int32_t event, int32_t eventParm, AfterEventAction afterEventAction = AfterEventAction::None);

    void setOrigin(vec3_t origin);
    void setAngles(vec3_t a);

    void enablePhysicsObject(bool enabled = true) { m_physicsObject = enabled; }
    void setPhysicsBounce(float value) { m_physicsBounce = value; }

    int32_t clipMask() const { return m_clipMask; }

    std::string_view className() const { return m_classsName; }
    std::string_view targetName() const { return m_targetName; }
    std::string_view team() const { return m_team; }

    void setTarget(std::string target) { m_target = std::move(target); }
    void setPainTarget(std::string painTarget) { m_painTarget = std::move(painTarget); }
    void setTargetName(std::string targetName) { m_targetName = std::move(targetName); }
    void setTeam(std::string team) { m_team = std::move(team); }

    int32_t nextThink() const { return m_nextThink; }

    void enableDamage(bool enable = true) { m_takeDamage = enable; }
    void setHealth(int32_t health) { m_health = health; }

    void free();

    virtual void think() = 0;
    virtual void touch(const Entity& other, trace_t* trace) = 0;
    virtual void use(const Entity& other, const Entity& activator) = 0;
    virtual void pain(const Entity& attacker, int32_t damage) = 0;
    virtual void die(const Entity& inflictor, const Entity& attacker, int damage, int mod) = 0;

  protected:
    GameSharedEntity* m_sharedEntity;
    int32_t m_health = 0;
    int32_t m_oldHealth = 0;
    bool m_takeDamage = false;
    int32_t m_nextThink = 0;
    Entity * m_parent;
    int32_t m_clipMask = MASK_SHOT;
    bool m_physicsObject = false;
    float m_physicsBounce = 0;
    AfterEventAction m_afterEventAction = AfterEventAction::None;
    int32_t m_eventTime = 0;
    EntityType m_type;
    std::string m_classsName;
    std::string m_target;
    std::string m_painTarget;
    std::string m_targetName;
    std::string m_team;


    std::string m_targetShaderName;
    std::string m_targetShaderNewName;

    Entity(EntityType type, std::string classname, GameSharedEntity* sharedEntity, Entity* parent = nullptr)
      : m_sharedEntity{sharedEntity}
      , m_parent{ parent }
      , m_type{ type }
      , m_classsName{ std::move(classname) }
    {}

    bool useTargets();
    bool useTargets(const Entity& activator);
    bool useTargets(const Entity& activator, std::string_view target);
    bool useTargets(std::string_view target);
  };
}
