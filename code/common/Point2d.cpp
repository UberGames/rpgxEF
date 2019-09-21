#include "Point2d.h"
#include <catch2/catch.hpp>

TEST_CASE("point2di_construct", "[common::Point2dI]") {
  auto p = common::Point2dI{42, 4};
  REQUIRE(p.x_ == 42);
  REQUIRE(p.y_ == 4);

  p = common::Point2dI{32.3f, 43.2f};
  REQUIRE(p.x_ == 32);
  REQUIRE(p.y_ == 43);

  p = common::Point2dI{32.4, 85.3};
  REQUIRE(p.x_ == 32);
  REQUIRE(p.y_ == 85);

  p = common::Point2dI{44, 32.43f};
  REQUIRE(p.x_ == 44);
  REQUIRE(p.y_ == 32);

  p = common::Point2dI{44, 564.324};
  REQUIRE(p.x_ == 44);
  REQUIRE(p.y_ == 564);

  p = common::Point2dI{234.52f, 65};
  REQUIRE(p.x_ == 234);
  REQUIRE(p.y_ == 65);

  p = common::Point2dI{743.345, 76};
  REQUIRE(p.x_ == 743);
  REQUIRE(p.y_ == 76);
}
