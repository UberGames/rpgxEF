#include "String.h"

#include <catch2/catch.hpp>

TEST_CASE("icompare_string_view", "[common::icompare]") {
  auto str_1 = "TesT"sv;
  auto str_2 = "test"sv;
  auto str_3 = "TEST"sv;

  REQUIRE(common::icompare(str_1, str_2));
  REQUIRE(common::icompare(str_1, str_2));
  REQUIRE(common::icompare(str_2, str_3));
  REQUIRE(common::icompare(str_1, str_3));
}

TEST_CASE("icompare_string", "[common::icompare]") {
  auto str_1 = "TesT"s;
  auto str_2 = "test"s;
  auto str_3 = "TEST"s;

  REQUIRE(common::icompare(str_1, str_2));
  REQUIRE(common::icompare(str_1, str_2));
  REQUIRE(common::icompare(str_2, str_3));
  REQUIRE(common::icompare(str_1, str_3));
}

TEST_CASE("icompare_cstring", "[common::icompare]") {
  const char *str_1 = "TesT";
  const char *str_2 = "test";
  const char *str_3 = "TEST";

  REQUIRE(common::icompare(str_1, str_2));
  REQUIRE(common::icompare(str_1, str_2));
  REQUIRE(common::icompare(str_2, str_3));
  REQUIRE(common::icompare(str_1, str_3));
}

TEST_CASE("icompare_mixed", "[common::icompare]") {
  const auto str_1 = "TesT"sv;
  const auto str_2 = "test"s;
  const char *str_3 = "TEST";

  REQUIRE(common::icompare(str_1, str_2));
  REQUIRE(common::icompare(str_1, str_2));
  REQUIRE(common::icompare(str_2, str_3));
  REQUIRE(common::icompare(str_1, str_3));
}

TEST_CASE("ifind_string_view", "[common::ifind]") {
  auto haystack = "tipedy TAPEDY tOpEDy tap"sv;

  REQUIRE(common::ifind(haystack, "blah"sv) == std::string_view::npos);
  REQUIRE(common::ifind(haystack, "tapedy"sv) == 7);
  REQUIRE(common::ifind(haystack, "TOPEDY"sv) == 14);
  REQUIRE(common::ifind(haystack, "iPe"sv) == 1);
}

TEST_CASE("ifind_string", "[common::ifind]") {
  auto haystack = "tipedy TAPEDY tOpEDy tap"s;

  REQUIRE(common::ifind(haystack, "blah"s) == std::string_view::npos);
  REQUIRE(common::ifind(haystack, "tapedy"s) == 7);
  REQUIRE(common::ifind(haystack, "TOPEDY"s) == 14);
  REQUIRE(common::ifind(haystack, "iPe"s) == 1);
}

TEST_CASE("ifind_cstring", "[common::ifind]") {
  auto haystack = "tipedy TAPEDY tOpEDy tap";

  REQUIRE(common::ifind(haystack, "blah") == std::string_view::npos);
  REQUIRE(common::ifind(haystack, "tapedy") == 7);
  REQUIRE(common::ifind(haystack, "TOPEDY") == 14);
  REQUIRE(common::ifind(haystack, "iPe") == 1);
}

TEST_CASE("ifind_mixed", "[common::ifind]") {
  auto haystack = "tipedy TAPEDY tOpEDy tap";

  REQUIRE(common::ifind(haystack, "blah") == std::string_view::npos);
  REQUIRE(common::ifind(haystack, "tapedy"s) == 7);
  REQUIRE(common::ifind(haystack, "TOPEDY"sv) == 14);
  REQUIRE(common::ifind(haystack, "iPe") == 1);

  auto haystack2 = "tipedy TAPEDY tOpEDy tap"sv;

  REQUIRE(common::ifind(haystack2, "blah") == std::string_view::npos);
  REQUIRE(common::ifind(haystack2, "tapedy"s) == 7);
  REQUIRE(common::ifind(haystack2, "TOPEDY"sv) == 14);
  REQUIRE(common::ifind(haystack2, "iPe") == 1);

  auto haystack3 = "tipedy TAPEDY tOpEDy tap"s;

  REQUIRE(common::ifind(haystack3, "blah") == std::string_view::npos);
  REQUIRE(common::ifind(haystack3, "tapedy"s) == 7);
  REQUIRE(common::ifind(haystack3, "TOPEDY"sv) == 14);
  REQUIRE(common::ifind(haystack3, "iPe") == 1);
}

// TODO ifind tests