#pragma once

#include <random>

namespace common {

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
T random(T min = std::numeric_limits<T>::min(),
         T max = std::numeric_limits<T>::max()) {
  static auto seeder = std::random_device();
  static auto engine = std::mt19937_64(seeder());
  std::uniform_int_distribution<T> dist(min, max);
  return dist(engine);
}

template <typename T, bool Dummy = true,
          typename = std::enable_if_t<std::is_floating_point_v<T> && Dummy>>
T random(T min = std::numeric_limits<T>::min(),
         T max = std::numeric_limits<T>::max()) {
  static auto seeder = std::random_device();
  static auto engine = std::mt19937_64(seeder());
  std::uniform_real_distribution<T> dist(min, max);
  return dist(engine);
}

} // namespace common
