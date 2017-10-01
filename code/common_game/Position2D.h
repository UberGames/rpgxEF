#pragma once

#include <cstdint>

namespace Common
{
  namespace Detail
  {
    template<typename T>
    class Position2DImpl
    {
    public:
      T& X() { return m_Coords[0]; }
      T& Y() { return m_Coords[1]; }
      const T& X() const { return m_Coords[0]; }
      const T& Y() const { return m_Coords[1]; }
      T& operator[](size_t Index) { return Index >= 1 ? m_Coords[1] : m_Coords[0]; }
      const T& operator[](size_t Index) const  { return Index >= 1 ? m_Coords[1] : m_Coords[0]; }
      
    protected:
      T m_Coords[2] = { 0, 0 };
    };
  }

  using Position2D = Detail::Position2DImpl<int32_t>;
}
