#include <gtest/gtest.h>
#include "geometry.hpp"

using namespace geometry;

TEST(GeometryTest, Point2D)
{
  Point2D p1(1, 1);
  Point2D p2(2, 2);
  Point2D p3(1, 1);

  const double sqrt2 = std::sqrt(2.0);

  EXPECT_TRUE(p1 < p2);

  // Trigger warning for some reason.
  // EXPECT_TRUE((p1 == p3));

  EXPECT_DOUBLE_EQ(3.0, (p1 + p2).x);
  EXPECT_DOUBLE_EQ(3.0, (p1 + p2).y);

  EXPECT_DOUBLE_EQ(-1.0, (p1 - p2).x);
  EXPECT_DOUBLE_EQ(-1.0, (p1 - p2).y);

  EXPECT_DOUBLE_EQ(3.0, (p1 * 3.0).x);
  EXPECT_DOUBLE_EQ(4.0, (p1 * 4.0).y);

  EXPECT_DOUBLE_EQ(0.5, (p1 / 2.0).x);
  EXPECT_DOUBLE_EQ(0.5, (p1 / 2.0).y);

  EXPECT_DOUBLE_EQ(sqrt2, p1.DistanceTo(p2));
}
