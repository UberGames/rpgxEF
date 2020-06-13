#pragma once

#include <random>

namespace common {

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
T getRandomInt(T min, T max) {
  static auto seeder = std::random_device();
  static auto engine = std::mt19937_64(seeder());
  std::uniform_int_distribution<T> dist(min, max);
  return dist(engine);
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
T getRandomInt(T min) {
  return getRandomInt(min, std::numeric_limits<T>::max());
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
T getRandomInt() {
  return getRandomInt(std::numeric_limits<T>::min(),
                      std::numeric_limits<T>::max());
}

int getRandomInt();

template <typename T, bool Dummy = true,
          typename = std::enable_if_t<std::is_floating_point_v<T> && Dummy>>
T getRandomDouble(T min, T max) {
  static auto seeder = std::random_device();
  static auto engine = std::mt19937_64(seeder());
  std::uniform_real_distribution<T> dist(min, max);
  return dist(engine);
}

template <typename T, bool Dummy = true,
          typename = std::enable_if_t<std::is_floating_point_v<T> && Dummy>>
T getRandomDouble(T min) {
  return getRandomDouble(min, std::numeric_limits<T>::max());
}

template <typename T, bool Dummy = true,
          typename = std::enable_if_t<std::is_floating_point_v<T> && Dummy>>
T getRandomDouble() {
  return getRandomDouble(std::numeric_limits<T>::min(),
                         std::numeric_limits<T>::max());
}

double getRandomDouble();

} // namespace common
