#pragma once

#include <map>
#include <memory>
#include <functional>
#include <cassert>

namespace game
{
  namespace Detail
  {
    class ListenerStorageBase
    {
    public:
      using Handle = size_t;

      virtual ~ListenerStorageBase() = default;
      virtual bool remove(Handle) = 0;
    };

    template<typename ListenerType>
    class ListenerStorage : public ListenerStorageBase
    {
    public:
      using Handle = ListenerStorageBase::Handle;
      using container_type = std::map<Handle, ListenerType>;
      using iterator = typename container_type::iterator;
      using const_iterator = typename container_type::const_iterator;

      iterator begin()
      {
        return m_listeners.begin();
      }

      iterator end()
      {
        return m_listeners.end();
      }

      const_iterator begin() const
      {
        return m_listeners.begin();
      }

      const_iterator end() const
      {
        return m_listeners.end();
      }

      const_iterator cbegin() const
      {
        return m_listeners.cbegin();
      }

      const_iterator cend() const
      {
        return m_listeners.cend();
      }

      size_t size() const
      {
        return m_listeners.size();
      }

      void clear()
      {
        m_listeners.clear();
      }

      Handle addListener(ListenerType Listener)
      {
        auto current = m_next++;
        m_listeners[current] = Listener;
        return current;
      }

      bool remove(Handle handle) override
      {
        auto it = m_listeners.find(handle);
        if(it == m_listeners.end())
        {
          return false;
        }

        m_listeners.erase(handle);
        return true;
      }

      virtual ~ListenerStorage() = default;

    private:
      Handle m_next = 0;
      std::map<Handle, ListenerType> m_listeners;
    };
  }

  class EventConnection
  {
  public:
    using Handle = typename Detail::ListenerStorageBase::Handle;

    EventConnection() = default;
    EventConnection(const EventConnection&) = delete;
    EventConnection(EventConnection&&) = default;
    EventConnection& operator=(const EventConnection&) = delete;
    EventConnection& operator=(EventConnection&&) = default;
    EventConnection(std::weak_ptr<Detail::ListenerStorageBase> ListenerStorage, Handle handle)
      : m_handle{ handle }
      , m_listenerStorage{ ListenerStorage }
    {}

    void disconnect()
    {
      if(auto ptr = m_listenerStorage.lock())
      {
        ptr->remove(m_handle);
      }
    }

    virtual ~EventConnection() = default;

  protected:
    Handle m_handle;
    std::weak_ptr<Detail::ListenerStorageBase> m_listenerStorage;
  };

  class ScopedEventConnection : public EventConnection
  {
    ScopedEventConnection() = default;
    ScopedEventConnection(ScopedEventConnection&&) = default;
    ScopedEventConnection(EventConnection&& other)
      : EventConnection{std::forward<EventConnection>(other)}
    {}

    ScopedEventConnection& operator=(ScopedEventConnection&&) = default;
    ScopedEventConnection& operator=(EventConnection&& other)
    {
      EventConnection::operator=(std::forward<EventConnection>(other));
      return *this;
    }

    virtual ~ScopedEventConnection()
    {
      disconnect();
    }
  };

  template<typename>
  class Event;

  template<typename Ret, typename... Args>
  class Event<Ret(Args...)>
  {
  public:
    Event() = default;
    Event(const Event&) = delete;
    Event(Event&&) = default;
    Event& operator=(const Event&) = delete;
    Event& operator=(Event&&) = default;

    using ListenerType = std::function<Ret(Args...)>;
    using result_type = typename ListenerType::result_type;
    using Handle = typename Detail::ListenerStorage<ListenerType>::Handle;

    void operator()(Args... args)
    {
      if(m_emitting)
      {
        assert(false);
        return;
      }
      m_emitting = true;

      for(const auto& l : m_listenerStorage)
      {
        l.second(args...);
      }

      m_emitting = false;
    }

    template<typename L>
    EventConnection operator+=(L&& listener)
    {
      return Connect(std::forward<L>(listener));
    }

    EventConnection connect(ListenerType listener)
    {
      auto h = m_listenerStorage->addListener(listener);
      return EventConnection{m_listenerStorage, h};
    }

    void disconnect(EventConnection& con)
    {
      con.disconnect();
    }

    void disconnectAll()
    {
      m_listenerStorage->clear();
    }

  private:
    std::shared_ptr<Detail::ListenerStorage<ListenerType> > m_listenerStorage = std::make_shared<Detail::ListenerStorage<ListenerType>>();
    bool m_emitting = false;
  };
}
