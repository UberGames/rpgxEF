#include "Rectangle.h"
#include <catch2/catch.hpp>

static_assert(common::Rectangle<int>().left == 0);
static_assert(common::Rectangle<int>().top == 0);
static_assert(common::Rectangle<int>().right == 0);
static_assert(common::Rectangle<int>().left == 0);

static_assert(common::Rectangle<int>(1).left == 1);
static_assert(common::Rectangle<int>(1).top == 0);
static_assert(common::Rectangle<int>(1).right == 1);
static_assert(common::Rectangle<int>(1).bottom == 0);

static_assert(common::Rectangle<int>(1, -4).left == 1);
static_assert(common::Rectangle<int>(1, -4).top == -4);
static_assert(common::Rectangle<int>(1, -4).right == 1);
static_assert(common::Rectangle<int>(1, -4).bottom == -4);

static_assert(common::Rectangle<int>(1, -4, 563).left == 1);
static_assert(common::Rectangle<int>(1, -4, 563).top == -4);
static_assert(common::Rectangle<int>(1, -4, 563).right == 564);
static_assert(common::Rectangle<int>(1, -4, 563).bottom == -4);

static_assert(common::Rectangle<int>(1, -4, 563, 49).left == 1);
static_assert(common::Rectangle<int>(1, -4, 563, 49).top == -4);
static_assert(common::Rectangle<int>(1, -4, 563, 49).right == 564);
static_assert(common::Rectangle<int>(1, -4, 563, 49).bottom == 45);

static_assert(common::Rectangle<int>(1.0, -4.3, 22.4f, 49.3).left == 1);
static_assert(common::Rectangle<int>(1.0, -4.3, 22.4f, 49.3).top == -4);
static_assert(common::Rectangle<int>(1.0, -4.3, 22.4f, 49.3).right == 23);
static_assert(common::Rectangle<int>(1.0, -4.3, 22.4f, 49.3).bottom == 45);

static_assert(common::Rectangle<int>(0, 0, 1, 1).width() == 1);
static_assert(common::Rectangle<int>(0, 0, 1, 1).height() == 1);
static_assert(common::Rectangle<int>(0, 0, 1, 1).area() == 1);

static_assert(common::Rectangle<int>(4, 4, 10, 10).width() == 10);
static_assert(common::Rectangle<int>(4, 4, 10, 10).height() == 10);
static_assert(common::Rectangle<int>(4, 4, 10, 10).area() == 100);

static_assert(common::Rectangle<int>(1, 2, 3, 4) ==
              common::Rectangle<int>(1, 2, 3, 4));
static_assert(common::Rectangle<int>(1, 2, 3, 4) !=
              common::Rectangle<int>(4, 3, 2, 1));

static_assert(common::bounds(common::Rectangle<int>{3, 6, 4, 4},
                             common::Rectangle<int>{4, 7, 2, 2})
                  .left == 3);
static_assert(common::bounds(common::Rectangle<int>{3, 6, 4, 4},
                             common::Rectangle<int>{4, 7, 2, 2})
                  .top == 6);
static_assert(common::bounds(common::Rectangle<int>{3, 6, 4, 4},
                             common::Rectangle<int>{4, 7, 2, 2})
                  .right == 7);
static_assert(common::bounds(common::Rectangle<int>{3, 6, 4, 4},
                             common::Rectangle<int>{4, 7, 2, 2})
                  .bottom == 10);

static_assert(common::bounds(common::Rectangle<int>{1, 1, 2, 2},
                             common::Rectangle<int>{4, 3, 2, 2})
                  .left == 1);
static_assert(common::bounds(common::Rectangle<int>{1, 1, 2, 2},
                             common::Rectangle<int>{4, 3, 2, 2})
                  .top == 1);
static_assert(common::bounds(common::Rectangle<int>{1, 1, 2, 2},
                             common::Rectangle<int>{4, 3, 2, 2})
                  .right == 6);
static_assert(common::bounds(common::Rectangle<int>{1, 1, 2, 2},
                             common::Rectangle<int>{4, 3, 2, 2})
                  .bottom == 5);

static_assert(common::bounds(common::Rectangle<int>{1, 1, 2, 2},
                             common::Rectangle<int>{2, 2, 3, 2})
                  .left == 1);
static_assert(common::bounds(common::Rectangle<int>{1, 1, 2, 2},
                             common::Rectangle<int>{2, 2, 3, 2})
                  .top == 1);
static_assert(common::bounds(common::Rectangle<int>{1, 1, 2, 2},
                             common::Rectangle<int>{2, 2, 3, 2})
                  .right == 5);
static_assert(common::bounds(common::Rectangle<int>{1, 1, 2, 2},
                             common::Rectangle<int>{2, 2, 3, 2})
                  .bottom == 4);

static_assert(common::bounds(common::Rectangle<int>{2, 2, 3, 2},
                             common::Rectangle<int>{4, 3, 2, 2})
                  .left == 2);
static_assert(common::bounds(common::Rectangle<int>{2, 2, 3, 2},
                             common::Rectangle<int>{4, 3, 2, 2})
                  .top == 2);
static_assert(common::bounds(common::Rectangle<int>{2, 2, 3, 2},
                             common::Rectangle<int>{4, 3, 2, 2})
                  .right == 6);
static_assert(common::bounds(common::Rectangle<int>{2, 2, 3, 2},
                             common::Rectangle<int>{4, 3, 2, 2})
                  .bottom == 5);

static_assert(common::bounds(common::Rectangle<int>{3, 6, 4, 4},
                             common::Rectangle<int>{3, 6, 4, 4})
                  .left == 3);
static_assert(common::bounds(common::Rectangle<int>{3, 6, 4, 4},
                             common::Rectangle<int>{3, 6, 4, 4})
                  .top == 6);
static_assert(common::bounds(common::Rectangle<int>{3, 6, 4, 4},
                             common::Rectangle<int>{3, 6, 4, 4})
                  .right == 7);
static_assert(common::bounds(common::Rectangle<int>{3, 6, 4, 4},
                             common::Rectangle<int>{3, 6, 4, 4})
                  .bottom == 10);

static_assert(common::bounds(common::Rectangle<int>{1, 1, 3, 3},
                             common::Rectangle<int>{4, 3, 6, 5},
                             common::Rectangle<int>{2, 2, 5, 4},
                             common::Rectangle<int>{3, 6, 7, 10})
                  .left == 1);
static_assert(common::bounds(common::Rectangle<int>{1, 1, 3, 3},
                             common::Rectangle<int>{4, 3, 6, 5},
                             common::Rectangle<int>{2, 2, 5, 4},
                             common::Rectangle<int>{3, 6, 7, 10})
                  .top == 1);
static_assert(common::bounds(common::Rectangle<int>{1, 1, 2, 2},
                             common::Rectangle<int>{4, 3, 2, 2},
                             common::Rectangle<int>{2, 2, 3, 2},
                             common::Rectangle<int>{3, 6, 4, 4})
                  .right == 7);
static_assert(common::bounds(common::Rectangle<int>{1, 1, 2, 2},
                             common::Rectangle<int>{4, 3, 2, 2},
                             common::Rectangle<int>{2, 2, 3, 2},
                             common::Rectangle<int>{3, 6, 4, 4})
                  .bottom == 10);

TEST_CASE("rectangle_construct", "[common::Rectangle]") {
  REQUIRE(common::Rectangle<int>().left == 0);
  REQUIRE(common::Rectangle<int>().top == 0);
  REQUIRE(common::Rectangle<int>().right == 0);
  REQUIRE(common::Rectangle<int>().left == 0);

  REQUIRE(common::Rectangle<int>(1).left == 1);
  REQUIRE(common::Rectangle<int>(1).top == 0);
  REQUIRE(common::Rectangle<int>(1).right == 1);
  REQUIRE(common::Rectangle<int>(1).bottom == 0);

  REQUIRE(common::Rectangle<int>(1, -4).left == 1);
  REQUIRE(common::Rectangle<int>(1, -4).top == -4);
  REQUIRE(common::Rectangle<int>(1, -4).right == 1);
  REQUIRE(common::Rectangle<int>(1, -4).bottom == -4);

  REQUIRE(common::Rectangle<int>(1, -4, 563).left == 1);
  REQUIRE(common::Rectangle<int>(1, -4, 563).top == -4);
  REQUIRE(common::Rectangle<int>(1, -4, 563).right == 564);
  REQUIRE(common::Rectangle<int>(1, -4, 563).bottom == -4);

  REQUIRE(common::Rectangle<int>(1, -4, 563, 49).left == 1);
  REQUIRE(common::Rectangle<int>(1, -4, 563, 49).top == -4);
  REQUIRE(common::Rectangle<int>(1, -4, 563, 49).right == 564);
  REQUIRE(common::Rectangle<int>(1, -4, 563, 49).bottom == 45);

  REQUIRE(common::Rectangle<int>(1.0, -4.3, 22.4f, 49.3).left == 1);
  REQUIRE(common::Rectangle<int>(1.0, -4.3, 22.4f, 49.3).top == -4);
  REQUIRE(common::Rectangle<int>(1.0, -4.3, 22.4f, 49.3).right == 23);
  REQUIRE(common::Rectangle<int>(1.0, -4.3, 22.4f, 49.3).bottom == 45);
}

TEST_CASE("rectangle_width", "[common::Rectangle]") {
  REQUIRE(common::Rectangle<int>(0, 0, 1, 1).width() == 1);
  REQUIRE(common::Rectangle<int>(4, 4, 6, 6).width() == 6);
}

TEST_CASE("rectangle_height", "[common::Rectangle]") {
  REQUIRE(common::Rectangle<int>(0, 0, 1, 1).height() == 1);
  REQUIRE(common::Rectangle<int>(4, 4, 6, 6).height() == 6);
}

TEST_CASE("reactangle_area", "[common::Rectangle]") {
  REQUIRE(common::Rectangle<int>(0, 0, 1, 1).area() == 1);
  REQUIRE(common::Rectangle<int>(4, 4, 6, 6).area() == 36);
}

TEST_CASE("rectangle_equality", "[common::Rectangle]") {
  REQUIRE(common::Rectangle<int>(1, 2, 3, 4) ==
          common::Rectangle<int>(1, 2, 3, 4));
  REQUIRE(common::Rectangle<int>(1, 2, 3, 4) !=
          common::Rectangle<int>(4, 3, 2, 1));
}

TEST_CASE("rectangle_intersect", "[common::Rectangle]") {
  auto rect_a = common::Rectangle<int>{1, 1, 2, 2};
  auto rect_b = common::Rectangle<int>{4, 3, 2, 2};
  auto rect_c = common::Rectangle<int>{2, 2, 3, 2};

  REQUIRE(!common::intersect(rect_a, rect_b));
  REQUIRE(common::intersect(rect_a, rect_a));
  REQUIRE(common::intersect(rect_b, rect_b));
  REQUIRE(common::intersect(rect_c, rect_c));
  REQUIRE(common::intersect(rect_c, rect_a));
  REQUIRE(common::intersect(rect_c, rect_b));

  auto rect_d = common::Rectangle<int>{3, 6, 4, 4};
  auto rect_e = common::Rectangle<int>{4, 7, 2, 2};

  REQUIRE(common::intersect(rect_d, rect_e));
  REQUIRE(common::intersect(rect_e, rect_d));
}

TEST_CASE("rectangle_intersection", "[common::Rectangle]") {
  auto rect_d = common::Rectangle<int>{3, 6, 4, 4};
  auto rect_e = common::Rectangle<int>{4, 7, 2, 2};

  auto intersectopn_d_e = common::intersection(rect_d, rect_e);
  REQUIRE(!intersectopn_d_e.empty());
  REQUIRE(intersectopn_d_e.left == rect_e.left);
  REQUIRE(intersectopn_d_e.top == rect_e.top);
  REQUIRE(intersectopn_d_e.right == rect_e.right);
  REQUIRE(intersectopn_d_e.bottom == rect_e.bottom);

  auto intersection_e_d = common::intersection(rect_e, rect_d);
  REQUIRE(!intersection_e_d.empty());
  REQUIRE(intersection_e_d.left == rect_e.left);
  REQUIRE(intersection_e_d.top == rect_e.top);
  REQUIRE(intersection_e_d.right == rect_e.right);
  REQUIRE(intersection_e_d.bottom == rect_e.bottom);

  auto rect_a = common::Rectangle<int>{1, 1, 2, 2};
  auto rect_b = common::Rectangle<int>{4, 3, 2, 2};
  auto rect_c = common::Rectangle<int>{2, 2, 3, 2};

  auto intersection_a_b = common::intersection(rect_a, rect_b);
  REQUIRE(intersection_a_b.empty());
  auto intersection_b_a = common::intersection(rect_b, rect_a);
  REQUIRE(intersection_b_a.empty());

  auto intersection_a_c = common::intersection(rect_a, rect_c);
  REQUIRE(!intersection_a_c.empty());
  REQUIRE(intersection_a_c.left == 2);
  REQUIRE(intersection_a_c.top == 2);
  REQUIRE(intersection_a_c.right == 3);
  REQUIRE(intersection_a_c.bottom == 3);

  auto intersection_c_a = common::intersection(rect_c, rect_a);
  REQUIRE(!intersection_c_a.empty());
  REQUIRE(intersection_c_a.left == 2);
  REQUIRE(intersection_c_a.top == 2);
  REQUIRE(intersection_c_a.right == 3);
  REQUIRE(intersection_c_a.bottom == 3);

  auto intersection_a_a = common::intersection(rect_a, rect_a);
  REQUIRE(!intersection_a_a.empty());
  REQUIRE(intersection_a_a.left == 1);
  REQUIRE(intersection_a_a.top == 1);
  REQUIRE(intersection_a_a.right == 3);
  REQUIRE(intersection_a_a.bottom == 3);
}

TEST_CASE("rectangle_bounds", "[common::Rectangle]") {
  auto rect_d = common::Rectangle<int>{3, 6, 4, 4};
  auto rect_e = common::Rectangle<int>{4, 7, 2, 2};

  auto bounds_d_e = common::bounds(rect_d, rect_e);
  REQUIRE(bounds_d_e.top == rect_d.top);
  REQUIRE(bounds_d_e.left == rect_d.left);
  REQUIRE(bounds_d_e.bottom == rect_d.bottom);
  REQUIRE(bounds_d_e.right == rect_d.right);

  auto rect_a = common::Rectangle<int>{1, 1, 2, 2};
  auto rect_b = common::Rectangle<int>{4, 3, 2, 2};
  auto rect_c = common::Rectangle<int>{2, 2, 3, 2};

  auto bounds_a_b = common::bounds(rect_a, rect_b);
  REQUIRE(bounds_a_b.left == rect_a.left);
  REQUIRE(bounds_a_b.top == rect_a.top);
  REQUIRE(bounds_a_b.right == rect_b.right);
  REQUIRE(bounds_a_b.bottom == rect_b.bottom);

  auto bounds_a_c = common::bounds(rect_a, rect_c);
  REQUIRE(bounds_a_c.left == rect_a.left);
  REQUIRE(bounds_a_c.top == rect_a.top);
  REQUIRE(bounds_a_c.right == rect_c.right);
  REQUIRE(bounds_a_c.bottom == rect_c.bottom);

  auto bounds_c_b = common::bounds(rect_c, rect_b);
  REQUIRE(bounds_c_b.left == rect_c.left);
  REQUIRE(bounds_c_b.top == rect_c.top);
  REQUIRE(bounds_c_b.right == rect_b.right);
  REQUIRE(bounds_c_b.bottom == rect_b.bottom);

  auto bounds_d_d = common::bounds(rect_d, rect_d);
  REQUIRE(bounds_d_d.left == rect_d.left);
  REQUIRE(bounds_d_d.top == rect_d.top);
  REQUIRE(bounds_d_d.right == rect_d.right);
  REQUIRE(bounds_d_d.bottom == rect_d.bottom);

  auto bounds_a_b_c_d = common::bounds(rect_a, rect_b, rect_c, rect_d);
  REQUIRE(bounds_a_b_c_d.left == rect_a.left);
  REQUIRE(bounds_a_b_c_d.top == rect_a.top);
  REQUIRE(bounds_a_b_c_d.right == rect_d.right);
  REQUIRE(bounds_a_b_c_d.bottom == rect_d.bottom);
}
